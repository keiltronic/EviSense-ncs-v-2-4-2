/**
 * @file aws_fota.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Oct 2023
 * @brief This file contains all functions to do a firmware update over the air (FOTA)
 * @version 2.0.0
 */

/*!
 * @defgroup FOTA
 * @brief This file contains all functions to do a firmware update over the air (FOTA)
 * @{*/
#include "aws_fota.h"

// uint8_t client_id_buf[CLIENT_ID_LEN];
uint8_t client_id_buf[16];
uint8_t current_job_id[AWS_JOBS_JOB_ID_MAX_LEN];

uint8_t aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
uint8_t fota_reboot_while_usb_connected = false;
uint8_t fota_is_connected = false;
uint32_t fota_connection_timer = 0;
uint8_t fota_download_in_progress = false;

/* Buffers for MQTT client. */
uint8_t rx_buffer[CONFIG_MQTT_MESSAGE_BUFFER_SIZE];
uint8_t tx_buffer[CONFIG_MQTT_MESSAGE_BUFFER_SIZE];
uint8_t payload_buf[CONFIG_MQTT_PAYLOAD_BUFFER_SIZE];

/* The mqtt client struct */
struct mqtt_client client;

/* MQTT Broker details. */
struct sockaddr_storage broker_storage;
struct sockaddr_storage coap_server;

/* File descriptor */
struct pollfd fds;

/* Set to true when application should teardown and reboot */
bool do_reboot;

/* Set to true when application should reconnect the LTE link*/
bool link_offline;

#if defined(CONFIG_BSD_LIBRARY)
/**@brief Recoverable BSD library error. */
void bsd_recoverable_error_handler(uint32_t err)
{
	rtc_print_debug_timestamp();
	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "bsdlib recoverable error: %u\n", err);
}

#endif /* defined(CONFIG_BSD_LIBRARY) */

#if !defined(CONFIG_USE_NRF_CLOUD)
/* Topic for updating shadow topic with version number */
#define AWS "$aws/things/"
#define UPDATE_DELTA_TOPIC AWS "%s/shadow/update"
#define SHADOW_STATE_UPDATE "{\"state\":{\"reported\":{\"nrf9160_app_version\":\"%s\"}}}"

int update_device_shadow_version(struct mqtt_client *const client)
{
	struct mqtt_publish_param param;
	char update_delta_topic[strlen(AWS) + strlen("/shadow/update") + sizeof(client_id_buf)];
	uint8_t shadow_update_payload[CONFIG_DEVICE_SHADOW_PAYLOAD_SIZE];

	int ret = 0;

	ret = snprintf(update_delta_topic, sizeof(update_delta_topic), UPDATE_DELTA_TOPIC, client->client_id.utf8);
	uint32_t update_delta_topic_len = ret;

	if (ret >= sizeof(update_delta_topic))
	{
		return -ENOMEM;
	}
	else if (ret < 0)
	{
		return ret;
	}

	ret = snprintf(shadow_update_payload, sizeof(shadow_update_payload), SHADOW_STATE_UPDATE, get_firmware_version_string());
	uint32_t shadow_update_payload_len = ret;

	if (ret >= sizeof(shadow_update_payload))
	{
		return -ENOMEM;
	}
	else if (ret < 0)
	{
		return ret;
	}

	param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;
	param.message.topic.topic.utf8 = update_delta_topic;
	param.message.topic.topic.size = update_delta_topic_len;
	param.message.payload.data = shadow_update_payload;
	param.message.payload.len = shadow_update_payload_len;
	//	param.message_id = sys_rand32_get();
	param.message_id = k_cycle_get_32();
	param.dup_flag = 0;
	param.retain_flag = 0;

	return mqtt_publish(client, &param);
}
#endif /* !defined(CONFIG_USE_NRF_CLOUD) */

/**@brief Function to print strings without null-termination. */
void data_print(uint8_t *prefix, uint8_t *data, uint32_t len)
{
	char buf[len + 1];

	memcpy(buf, data, len);
	buf[len] = 0;
	rtc_print_debug_timestamp();
	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s%s\n", prefix, buf);
}

/**@brief Function to read the published payload.
 */
int publish_get_payload(struct mqtt_client *c, uint8_t *write_buf, uint32_t length)
{
	uint8_t *buf = write_buf;
	uint8_t *end = buf + length;

	if (length > sizeof(payload_buf))
	{
		return -EMSGSIZE;
	}
	while (buf < end)
	{
		int ret = mqtt_read_publish_payload_blocking(c, buf, end - buf);

		if (ret < 0)
		{
			return ret;
		}
		else if (ret == 0)
		{
			return -EIO;
		}
		buf += ret;
	}
	return 0;
}

/**@brief MQTT client event handler */
void mqtt_evt_handler(struct mqtt_client *const c, const struct mqtt_evt *evt)
{
	int16_t err;

	err = aws_fota_mqtt_evt_handler(c, evt);
	if (err == 0)
	{
		/* Event handled by FOTA library so we can skip it */
		return;
	}
	else if (err < 0)
	{
		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - Failed! %d\n", err);
	}

	switch (evt->type)
	{
	case MQTT_EVT_CONNACK:

		if (evt->result != 0)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - MQTT connection failed %d\n", evt->result);
			break;
		}

		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - MQTT client connected!\n");

		fota_is_connected = true;

		err = update_device_shadow_version(c);

		if (err)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - Unable to update device shadow err: %d\n", err);
		}
		break;

	case MQTT_EVT_DISCONNECT:
		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "keiltronic cloud - MQTT client disconnected %d\n", evt->result);

		fota_is_connected = false;

		aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		break;

	case MQTT_EVT_PUBLISH:
	{
		const struct mqtt_publish_param *p = &evt->param.publish;

		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - MQTT PUBLISH result=%d len=%d\n", evt->result, p->message.payload.len);

		err = publish_get_payload(c, payload_buf, p->message.payload.len);

		if (err)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - mqtt_read_publish_payload: Failed! %d\n", err);
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Disconnecting MQTT client...\n");

			err = mqtt_disconnect(c);

			fota_is_connected = false;

			if (err)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - Could not disconnect: %d\n", err);
			}
		}

		if (p->message.topic.qos == MQTT_QOS_1_AT_LEAST_ONCE)
		{
			const struct mqtt_puback_param ack = {.message_id = p->message_id};

			/* Send acknowledgment. */
			err = mqtt_publish_qos1_ack(c, &ack);
			if (err)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - unable to ack\n");
			}
		}
		data_print("Received: ", payload_buf, p->message.payload.len);
		break;
	}

	case MQTT_EVT_PUBACK:

		if (evt->result != 0)
		{
			if (Parameter.fota_verbose == true)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - MQTT PUBACK error %d\n", evt->result);
			}
			break;
		}

		if (Parameter.fota_verbose == true)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - PUBACK packet id: %u\n", evt->param.puback.message_id);
		}
		break;

	case MQTT_EVT_SUBACK:

		if (evt->result != 0)
		{
			if (Parameter.fota_verbose == true)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud - MQTT SUBACK error %d\n", evt->result);
			}
			break;
		}

		if (Parameter.fota_verbose == true)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - SUBACK packet id: %u\n", evt->param.suback.message_id);
		}
		break;

	default:
		//	if ( Parameter.fota_verbose == true){
		//	rtc_print_debug_timestamp();
		//	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - default: %d\n", evt->type);
		//}
		break;
	}
}

/**@brief Resolves the configured hostname and
 * initializes the MQTT broker structure
 */
void broker_init(const char *hostname)
{
	int16_t err = 0;

	/******************* AWS TLS MQTT  ***************************************/
	char addr_str[INET6_ADDRSTRLEN];
	struct addrinfo *result;
	struct addrinfo *addr;
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM};

	err = getaddrinfo(hostname, NULL, &hints, &result);

	if (err)
	{
		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "keiltronic cloud error: getaddrinfo failed %d\n", err);

		aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
	}
	else
	{
		addr = result;
		err = -ENOENT;

		/* IPv4 Address. */
		if (addr->ai_addrlen == sizeof(struct sockaddr_in))
		{
			struct sockaddr_in *broker = ((struct sockaddr_in *)&broker_storage);

			broker->sin_addr.s_addr = ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr;
			broker->sin_family = AF_INET;
			broker->sin_port = htons(CONFIG_MQTT_BROKER_PORT);

			inet_ntop(AF_INET, &broker->sin_addr, addr_str, sizeof(addr_str));
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud IPv4 address: %s\n", addr_str);
		}
		else if (addr->ai_addrlen == sizeof(struct sockaddr_in6))
		{
			/* IPv6 Address. */
			struct sockaddr_in6 *broker = ((struct sockaddr_in6 *)&broker_storage);

			memcpy(broker->sin6_addr.s6_addr, ((struct sockaddr_in6 *)addr->ai_addr)->sin6_addr.s6_addr, sizeof(struct in6_addr));
			broker->sin6_family = AF_INET6;
			broker->sin6_port = htons(CONFIG_MQTT_BROKER_PORT);

			inet_ntop(AF_INET6, &broker->sin6_addr, addr_str, sizeof(addr_str));
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud IPv6 address %s\n", addr_str);
		}
		else
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud error: ai_addrlen = %u should be %u or %u\n", (unsigned int)addr->ai_addrlen, (unsigned int)sizeof(struct sockaddr_in), (unsigned int)sizeof(struct sockaddr_in6));
		}

		addr = addr->ai_next;

		/* Free the address. */
		freeaddrinfo(result);
	}
}

/**@brief Initialize the MQTT client structure */
int client_init(struct mqtt_client *client, char *hostname)
{
	mqtt_client_init(client);
	broker_init(hostname);

	/* Set client ID to IMEI number */
	memset(client_id_buf, 0, 16);
	memcpy(client_id_buf, modem_get_imei(), 15);

	if (pcb_test_is_running == false)
	{
		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud client id: %s\n", client_id_buf);
	}

	/* MQTT client configuration */
	client->broker = &broker_storage;
	client->evt_cb = mqtt_evt_handler;
	client->client_id.utf8 = client_id_buf;
	client->client_id.size = strlen(client_id_buf);
	client->password = NULL;
	client->user_name = NULL;
	client->protocol_version = MQTT_VERSION_3_1_1;

	/* MQTT buffers configuration */
	client->rx_buf = rx_buffer;
	client->rx_buf_size = sizeof(rx_buffer);
	client->tx_buf = tx_buffer;
	client->tx_buf_size = sizeof(tx_buffer);

	/* MQTT transport configuration */
	client->transport.type = MQTT_TRANSPORT_SECURE;

	static sec_tag_t sec_tag_list[] = {CONFIG_CLOUD_CERT_SEC_TAG};
	struct mqtt_sec_config *tls_config = &(client->transport).tls.config;

	tls_config->peer_verify = 2;
	tls_config->cipher_list = NULL;
	tls_config->cipher_count = 0;
	tls_config->sec_tag_count = ARRAY_SIZE(sec_tag_list);
	tls_config->sec_tag_list = sec_tag_list;
	tls_config->hostname = hostname;

	return 0;
}

/**@brief Initialize the file descriptor structure used by poll. */
int fds_init(struct mqtt_client *c)
{
	fds.fd = c->transport.tls.sock;
	fds.events = POLLIN;
	return 0;
}

/**@brief Configures AT Command interface to the modem link. */
void at_configure(void)
{
	int err;

	err = at_notif_init();
	__ASSERT(err == 0, "AT Notify could not be initialized.");
	err = at_cmd_init();
	__ASSERT(err == 0, "AT CMD could not be established.");
}

/**@brief Configures AT Command interface to the modem link. */
void aws_fota_cb_handler(struct aws_fota_event *fota_evt)
{
	int err;

	if (fota_evt == NULL)
	{
		return;
	}

	switch (fota_evt->id)
	{

	case AWS_FOTA_EVT_START:

		if (aws_fota_get_job_id(current_job_id, sizeof(current_job_id)) <= 0)
		{
			snprintf(current_job_id, sizeof(current_job_id), "N/A");
		}
		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_START, job id = %s\n", current_job_id);
		}
		break;

	case AWS_FOTA_EVT_DL_PROGRESS:

		/* CONFIG_FOTA_DOWNLOAD_PROGRESS_EVT must be enabled */
		/* to receive progress events */
		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_DL_PROGRESS, %d%% downloaded\n", fota_evt->dl.progress);
		}
		fota_download_in_progress = true;
		break;

	case AWS_FOTA_EVT_DONE:

		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_DONE, rebooting to apply update\n");
		}
		do_reboot = true;
		fota_download_in_progress = false;
		break;

	case AWS_FOTA_EVT_ERASE_PENDING:

		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_ERASE_PENDING, reboot or disconnect the LTE link\n");
		}

		err = lte_lc_offline();

		if (err)
		{
			if (pcb_test_is_running == false)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Error turning off the LTE link\n");
			}
			break;
		}
		link_offline = true;
		break;

	case AWS_FOTA_EVT_ERASE_DONE:

		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_ERASE_DONE\n");
		}

		if (link_offline)
		{
			if (pcb_test_is_running == false)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Reconnecting the LTE link\n");
			}
			err = lte_lc_connect();
			if (err)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Error reconnecting the LTE link\n");
				}
				break;
			}
			link_offline = false;
		}
		break;

	case AWS_FOTA_EVT_ERROR:

		if (Parameter.fota_verbose == true)
		{
			if (pcb_test_is_running == false)
			{
				rtc_print_debug_timestamp();
				shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - AWS_FOTA_EVT_ERROR\n");
			}
		}
		fota_download_in_progress = false;
		break;
	}
}

/**@brief Configures AT Command interface to the modem link. */
void aws_fota_statemachine(void)
{
	switch (aws_fota_process_state)
	{

	case AWS_FOTA_PROCESS_IDLE:
		break;

	case AWS_FOTA_PROCESS_CONNECT:

		client_init(&client, CONFIG_MQTT_BROKER_HOSTNAME);

		err = aws_fota_init(&client, aws_fota_cb_handler);

		if (err != 0)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: aws_fota_init %d\n", err);
				}
			}
			aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		}

		err = mqtt_connect(&client);

		if (err != 0)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: mqtt_connect %d\n", err);
				}
			}
			aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		}

		err = fds_init(&client);

		if (err != 0)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: fds_init %d\n", err);
				}
			}
			aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		}

		aws_fota_process_state = AWS_FOTA_PROCESS_RUNNING;
		break;

	case AWS_FOTA_PROCESS_RUNNING:

		err = poll(&fds, 1, MQTT_KEEPALIVE);

		if (err < 0)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: poll %d\n", errno);
				}
			}
		}

		err = mqtt_live(&client);

		if ((err != 0) && (err != -EAGAIN))
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: mqtt_live %d\n", err);
				}
			}
		}

		if ((fds.revents & POLLIN) == POLLIN)
		{
			err = mqtt_input(&client);
			if (err != 0)
			{
				if (Parameter.fota_verbose == true)
				{
					if (pcb_test_is_running == false)
					{
						rtc_print_debug_timestamp();
						shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - ERROR: mqtt_input %d\n", err);
					}
				}
			}
		}

		if ((fds.revents & POLLERR) == POLLERR)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - POLLERR\n");
				}
			}
		}

		if ((fds.revents & POLLNVAL) == POLLNVAL)
		{
			if (Parameter.fota_verbose == true)
			{
				if (pcb_test_is_running == false)
				{
					rtc_print_debug_timestamp();
					shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - POLLNVAL\n");
				}
			}
		}

		if (do_reboot)
		{		
			/* Teardown */
			mqtt_disconnect(&client);
			sys_reboot(0);

			k_msleep(1000);
		}
		break;

	case AWS_FOTA_PROCESS_DISCONNECT:

		if (pcb_test_is_running == false)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Disconnecting MQTT client...\n");
		}

		err = mqtt_disconnect(&client);

		fota_is_connected = false;

		if (err)
		{
			rtc_print_debug_timestamp();
			shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "keiltronic cloud - Could not disconnect MQTT client. Error: %d\n", err);
		}

		aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		break;

	default:

		aws_fota_process_state = AWS_FOTA_PROCESS_IDLE;
		break;
	}
}
