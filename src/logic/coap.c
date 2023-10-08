/**
 * @file coap.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 08 Oct 2023
 * @brief This file contains functions to establish a CoAP connection over DTLS and functions to create CoAP messages
 * @version 2.0.0
 */

/*!
 * @defgroup Cloud
 * @brief This file contains functions to establish a CoAP connection over DTLS and functions to create CoAP messages
 * @{*/

// https://github.com/nrfconnect/sdk-zephyr/blob/main/samples/net/sockets/echo_client/src/udp.c
// https://devzone.nordicsemi.com/f/nordic-q-a/80841/how-to-add-dtls-to-nrf-coap-client

/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 For code samples, you can look at large_get() in zephyr/samples/net/sockets/coap_server/src/coap-server.c,
 send_block_wise_coap_request in zephyr/sampls/net/sockets/coap_client/src/coap-client.c and the CoAP implementation
 of the download_client in nrf/subsys/net/lib/download_client/src/coap.c.
 */
#include "coap.h"

struct sockaddr_storage server;
struct coap_packet request;
struct coap_block_context blk_ctx;
uint8_t data[MAX_COAP_MSG_LEN];
uint8_t trigger_tx = false;
time_t time_since_last_cloud_transmission;

int16_t coap_sock;

//struct pollfd coap_fds[1];
int nfds;

char uri_path[] = "telemetry"; // Resource to use

// /*!
//  * @brief Creates a DTLS CoAP socket to the Bosch IoT cloud
//  * @return 0 if successfull, -1 if failed
//  */
// int16_t start_coap_client(void)
// {
//   if (Parameter.debug == true || Parameter.coap_verbose == true)
//   {
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Start resolving IP address\n");
//   }

//   int16_t err = 0;

//   char ipv4_addr[NET_IPV4_ADDR_LEN];
//   struct addrinfo *result_coap;
//   struct addrinfo hints_coap = {
//       .ai_family = AF_INET,
//       .ai_socktype = SOCK_DGRAM};

//   err = getaddrinfo("coap.bosch-iot-hub.com", NULL, &hints_coap, &result_coap); // Bosch IoT Suite from FHCS (IP: 3.124.207.218)  // err = getaddrinfo("64.225.102.80", NULL, &hints_coap, &result_coap); // keiltronic CoAP test server on digital ocean droplet

//   //  err = getaddrinfo("3.123.219.173", NULL, &hints_coap, &result_coap); // Bosch IoT Suite from FHCS (IP: 3.124.207.218)  // err = getaddrinfo("64.225.102.80", NULL, &hints_coap, &result_coap); // keiltronic CoAP test server on digital ocean droplet
//   // err = getaddrinfo("3.69.222.237", NULL, &hints_coap, &result_coap); // Bosch IoT Suite from FHCS (IP: 3.124.207.218)  // err = getaddrinfo("64.225.102.80", NULL, &hints_coap, &result_coap); // keiltronic CoAP test server on digital ocean droplet
//   // err = getaddrinfo("3.125.180.239", NULL, &hints_coap, &result_coap); // Bosch IoT Suite from FHCS (IP: 3.124.207.218)  // err = getaddrinfo("64.225.102.80", NULL, &hints_coap, &result_coap); // keiltronic CoAP test server on digital ocean droplet

//   if (err == 0)
//   {
//     /* IPv4 Address. */
//     struct sockaddr_in *server4 = ((struct sockaddr_in *)&server);

//     server4->sin_addr.s_addr = ((struct sockaddr_in *)result_coap->ai_addr)->sin_addr.s_addr;
//     server4->sin_family = AF_INET;
//     server4->sin_port = htons(CONFIG_COAP_SERVER_PORT);

//     err = inet_ntop(AF_INET, &server4->sin_addr.s_addr, ipv4_addr, sizeof(ipv4_addr));

//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "IPv4 address found %s\n", ipv4_addr);
//     }

//     /* Free the address. */
//     freeaddrinfo(result_coap);
//   }
//   else
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "ERROR %d: getaddrinfo() failed. LTE connection status: %d\n", err, modem.connection_stat);

//       if (modem.connection_stat == true)
//       {
//         rtc_print_debug_timestamp();
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Maybe data volume limit of the sim card reached. Check data volume!\n");
//       }
//     }
//     return -1;
//   }

//   if (Parameter.debug == true || Parameter.coap_verbose == true)
//   {
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Start connecting to socket\n");
//   }

//   coap_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_DTLS_1_2); // sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//   sec_tag_t sec_tag_list[] = {PSK_TAG};

//   if (coap_sock < 0)
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Failed to create UDP socket with DTLS\n");
//     }
//     (void)close(coap_sock);
//     return -1;
//   }
//   else
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Create UDP socket with DTLS successfully\n");
//     }
//   }

//   err = setsockopt(coap_sock, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_list, sizeof(sec_tag_list));

//   if (err < 0)
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Failed to set TLS_SEC_TAG_LIST option\n");
//     }
//     (void)close(coap_sock);
//     return -1;
//   }
//   else
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Set TLS_SEC_TAG_LIST option successfully\n");
//     }
//   }

//   err = connect(coap_sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in));

//   if (err == 0)
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Connected to socket successfully\n");
//     }
//   }
//   else
//   {
//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Failed to connect to socket.\n");
//     }
//     (void)close(coap_sock);
//     return -1;
//   }

//   prepare_fds();

//   return 0;
// }

// /**
//  * @brief Sends blockwise data over CoAP to the cloud
//  * 
//  * @param method: COAP_METHOD_GET, COAP_METHOD_DELETE, COAP_METHOD_PUT, COAP_METHOD_POST
//  * @param message: Pointer to the message which should be send 
//  * @param len: Length of the message 
//  * @return int16_t: 0 if successfull, -1 if failed
//  */
// int16_t send_coap_request(uint8_t method, uint8_t *message, uint16_t len)
// {
//   int16_t rslt = 0;
//   uint16_t packet_id = 0;

//   /* Open UDP socket to server address and connect to it */
//   rslt = start_coap_client();

//   if (rslt == 0)
//   {
//     /* Init structure for block transfer */
//     struct coap_block_context ctx;

//     int16_t coap_next_block_available = true;
//     uint8_t block_number = 0;

//     /* Will return 0 when it's the last block. */
//     memset(&ctx, 0, sizeof(ctx));

//     while (coap_next_block_available)
//     {
//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "###### CoAP blockwise transfer no. %d ##############################################\n", block_number);
//       }

//       if (ctx.total_size == 0)
//       {
//         coap_block_transfer_init(&ctx, COAP_BLOCK_SIZE, len);
//       }

//       /* Build new CoAP packet (PUT method, TYPE-CON) and insert URI path, URI query and payload */
//       packet_id = coap_next_id();
//       rslt = coap_packet_init(&request, data, MAX_COAP_MSG_LEN, 1, COAP_TYPE_CON, 0, NULL, method, packet_id);

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         if (rslt < 0)
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not init CoAP packet\n", rslt);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Init CoAP packet successful\n");
//         }
//       }

//       rslt = coap_packet_append_option(&request, COAP_OPTION_URI_PATH, uri_path, strlen(uri_path));

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         if (rslt < 0)
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append uri_path\n", rslt);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending uri_path successful\n");
//         }
//       }

//       /* Append Content-Format = application/octet-stream -> COAP_CONTENT_FORMAT_APP_OCTET_STREAM*/
//       uint8_t content_type = 42; // 42 = COAP_CONTENT_FORMAT_APP_OCTET_STREAM

//       rslt = coap_packet_append_option(&request, COAP_OPTION_CONTENT_FORMAT, &content_type, sizeof(content_type));

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         if (rslt < 0)
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append content-format option\n", rslt);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending content-format successful\n");
//         }
//       }

//       rslt = coap_append_block1_option(&request, &ctx);

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         if (rslt < 0)
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append block1 option\n", rslt);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending block1 successful\n");
//         }
//       }

//       rslt = coap_append_size1_option(&request, &ctx);

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         if (rslt < 0)
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append size1 option\n", rslt);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending size1 successful\n");
//         }
//       }

//       /* Append payload if method is PUT or POST */
//       switch (method)
//       {

//       case COAP_METHOD_GET:
//       case COAP_METHOD_DELETE:
//         break;

//       case COAP_METHOD_PUT:
//       case COAP_METHOD_POST:

//         rslt = coap_packet_append_payload_marker(&request);

//         if (Parameter.debug == true || Parameter.coap_verbose == true)
//         {
//           rtc_print_debug_timestamp();
//           if (rslt < 0)
//           {
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append payload marker\n", rslt);
//           }
//           else
//           {
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending payload marker successful\n");
//           }
//         }

//         /* Split payload into several block to fit into one packet with set block size */
//         uint16_t rest_payload_length = len - (block_number * coap_block_size_to_bytes(COAP_BLOCK_SIZE));

//         if (rest_payload_length > coap_block_size_to_bytes(COAP_BLOCK_SIZE))
//         {
//           rslt = coap_packet_append_payload(&request, &message[block_number * coap_block_size_to_bytes(COAP_BLOCK_SIZE)], coap_block_size_to_bytes(COAP_BLOCK_SIZE));
//         }
//         else
//         {
//           rslt = coap_packet_append_payload(&request, &message[block_number * coap_block_size_to_bytes(COAP_BLOCK_SIZE)], rest_payload_length);
//         }

//         if (Parameter.debug == true || Parameter.coap_verbose == true)
//         {
//           rtc_print_debug_timestamp();
//           if (rslt < 0)
//           {
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR %d: Could not append payload\n", rslt);
//           }
//           else
//           {
//             if (rest_payload_length > coap_block_size_to_bytes(COAP_BLOCK_SIZE))
//             {
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending payload successful, payload size: %d bytes\n", coap_block_size_to_bytes(COAP_BLOCK_SIZE));
//             }
//             else
//             {
//               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Appending end of payload successful, payload size: %d bytes\n", rest_payload_length);
//             }
//           }
//         }

//         break;

//       default:
//         break;
//       }

//       /* Get next block */
//       coap_next_block_available = coap_next_block(&request, &ctx);

//       if (coap_next_block_available != 0)
//       {
//         block_number++;
//       }

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {
//         rtc_print_debug_timestamp();
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "coap_next_block result: %d\n", coap_next_block_available);
//       }

//       /* Send CoAP message over the socket (IP4, UDP) */
//       rslt = send(coap_sock, request.data, request.offset, 0);

//       if (Parameter.debug == true || Parameter.coap_verbose == true)
//       {

//         rtc_print_debug_timestamp();
//         if (rslt > 0)
//         {
//          shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "successfully send CoAP packet, packet length: %d bytes byte\n", request.offset);
//         }
//         else
//         {
//           shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "ERROR: Sending CoAP packet failed\n");
//         }
//       }

//       /* Just delay the blockwise transfer a bit after each block */
//       k_msleep(30);
//     }

//     (void)close(coap_sock);

//     if (Parameter.debug == true || Parameter.coap_verbose == true)
//     {
//       rtc_print_debug_timestamp();
//       shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Socket closed\n");
//     }
//   }

//   return 0;
// }

// /*!
//  *  @brief This is the function description
//  *  @param[in] destination: Pointer to destination char array
//  *  @param[in] source: Pointer to source char array
//  */
// void convert_ASCII_text_in_hexadecimal_string(char *destination, char *source)
// {
//   uint8_t len = 0;
//   len = strlen(source);

//   for (int i = 0; i < len; ++i)
//   {
//     sprintf(&destination[i * 2], "%02X", source[i]);
//   }
// }

// /*!
//  *  @brief This function prepares the "flash data storage" to store data into nRF9160 internal flash memory
//  */
// void prepare_fds(void)
// {
//   coap_fds[nfds].fd = coap_sock;
//   coap_fds[nfds].events = POLLIN;
//   nfds++;
// }

// /*!
//  *  @brief This wait function creates the timeout for an incoming message after it was requested
//  */
// void wait(void)
// {
//   if (poll(coap_fds, nfds, k_SEC(5)) < 0) // 5000 is the time out (I guess it is milli sec)
//   {
//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "[%s:%d] Waiting!\n", __func__, __LINE__);
//   }
// }

// /*!
//  *  @brief This wait function handles the receivment of large coap data
//  */
// int16_t process_large_coap_reply(void)
// {
//   struct coap_packet reply;
//   uint8_t *data;
//   bool last_block;
//   int16_t rcvd;
//   int16_t ret;

//   wait();

//   data = (uint8_t *)malloc(MAX_COAP_MSG_LEN);

//   if (!data)
//   {
//     return -ENOMEM;
//   }

//   rcvd = recv(coap_sock, data, MAX_COAP_MSG_LEN, MSG_DONTWAIT);

//   if (rcvd == 0)
//   {
//     ret = -EIO;

//     //   rtc_print_debug_timestamp();
//     //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "[%s:%d] Error: %d!\n", __func__, __LINE__, ret);

//     goto end;
//   }

//   if (rcvd < 0)
//   {
//     if (errno == EAGAIN || errno == EWOULDBLOCK)
//     {
//       ret = 0;
//     }
//     else
//     {
//       ret = -errno;

//       //   rtc_print_debug_timestamp();
//       //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "[%s:%d] Error: %d!\n", __func__, __LINE__, ret);
//     }

//     goto end;
//   }

//   // net_hexdump("Response", data, rcvd);

//   ret = coap_packet_parse(&reply, data, rcvd, NULL, 0);
//   if (ret < 0)
//   {
//     // LOG_ERR("Invalid data received");

//     rtc_print_debug_timestamp();
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "[%s:%d] Invalid data received. Error: %d!\n", __func__, __LINE__, ret);
//     goto end;
//   }

//   ret = coap_update_from_block(&reply, &blk_ctx);
//   if (ret < 0)
//   {
//     goto end;
//   }

//   last_block = coap_next_block(&reply, &blk_ctx);
//   if (!last_block)
//   {
//     ret = 1;
//     goto end;
//   }

//   ret = 0;

// end:
//   free(data);

//   return ret;
// }