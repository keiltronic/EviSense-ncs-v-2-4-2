/**
 * @file modem.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 11 Oct 2023
 * @brief This files contains function to communicate with the modem firmware
 * @version 2.0.0
 */

/*!
 * @defgroup Cloud
 * @brief This files contains function to communicate with the modem firmware
 * @{*/

#include "modem.h"

MODEM modem;
int16_t err = 0;
char modem_at_recv_buf[500];

void modem_init(void)
{
   int16_t err = 0;

   err = nrf_modem_lib_init();

   if (err < 0)
   {
      printk("Modem library init failed, err: %d", err);
      return err;
   }

   err = modem_info_init();
   if (err)
   {
      printk("Failed to initialize modem info: %d", err);
   }
}

int16_t network_info_log(void)
{
   char sbuf[128];
   modem_info_string_get(MODEM_INFO_RSRP, sbuf, sizeof(sbuf));
   printk("Signal strength: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_CUR_BAND, sbuf, sizeof(sbuf));
   printk("Current LTE band: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_SUP_BAND, sbuf, sizeof(sbuf));
   printk("Supported LTE bands: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_AREA_CODE, sbuf, sizeof(sbuf));
   printk("Tracking area code: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_UE_MODE, sbuf, sizeof(sbuf));
   printk("Current mode: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_OPERATOR, sbuf, sizeof(sbuf));
   printk("Current operator name: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_CELLID, sbuf, sizeof(sbuf));
   printk("Cell ID of the device: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_IP_ADDRESS, sbuf, sizeof(sbuf));
   printk("IP address of the device: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_FW_VERSION, sbuf, sizeof(sbuf));
   printk("Modem firmware version: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_LTE_MODE, sbuf, sizeof(sbuf));
   printk("LTE-M support mode: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_NBIOT_MODE, sbuf, sizeof(sbuf));
   printk("NB-IoT support mode: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_GPS_MODE, sbuf, sizeof(sbuf));
   printk("GPS support mode: %s\r\n", sbuf);

   modem_info_string_get(MODEM_INFO_DATE_TIME, sbuf, sizeof(sbuf));
   printk("Mobile network time and date: %s\r\n", sbuf);

   return 0;
}

void modem_initial_setup(void)
{
   /*
 BACKUP (initial setup to configure the modem)

 1. System start
 2. AT%XMAGPIO=1,1,1,7,1,746,803,2,698,748,2,1710,2200,3,824,894,4,880,960,5,791,849,7,1565,1586
 2. AT+CFUN=4
 3. AT%XSYSTEMMODE=1,0,0,0
 4. AT+CFUN=0
 */

   memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));

   rtc_print_debug_timestamp();
   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Starting initializing modem parameter, this takes a couple of seconds...\n");

   /* Set modem to flight mode */
   lte_lc_offline();

   /* Set pcb band filter information (R, L, C) */
   if (Parameter.debug == true || Parameter.modem_verbose == true)
   {
      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Set MAGPIO settings\n");
   }

   err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT%XMAGPIO=1,1,1,7,1,746,803,2,698,748,2,1710,2200,3,824,894,4,880,960,5,791,849,7,1565,1586");

   if (err)
   {
      if (Parameter.debug == true || Parameter.modem_verbose == true)
      {
         rtc_print_debug_timestamp();
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Changing MAGPIO failed.\n");
      }
      else
      {
         printk("Changed MAGPIO ok\n");
      }
   }
   k_msleep(500);

   /* Configure modem to use either LTE-M or NB-IoT */
   if (Parameter.network_connection_type == NB_IOT)
   {
      err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_NBIOT, LTE_LC_SYSTEM_MODE_PREFER_AUTO);

      rtc_print_debug_timestamp();
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Note: Device will use NB-IoT connection. It may take several minutes for a NB-IoT connection to be established successfully\n");

      if (err)
      {
         rtc_print_debug_timestamp();
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to NB-IoT failed\n");
      }
   }
   else
   {
      err = lte_lc_system_mode_set(LTE_LC_SYSTEM_MODE_LTEM, LTE_LC_SYSTEM_MODE_PREFER_AUTO);

      if (err)
      {
         rtc_print_debug_timestamp();
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: Setting modem to LTE-M failed\n");
      }
   }
   k_msleep(100);

   /* Turn modem off to store configuration in its nonvolile memory */
   lte_lc_power_off();

   k_msleep(500);

   /* Turn modem on - it will automatically search for networks*/
   lte_lc_normal();
}

const char *modem_get_imei(void)
{
   static char imei_buf[18];
   memset(imei_buf, 0, 18);
   uint8_t err = 0;

   /* IMEI number */
   err = nrf_modem_at_cmd(imei_buf, sizeof(imei_buf), "AT%XICCID");

   return imei_buf;
}

const char *modem_get_iccid(void)
{
   uint8_t i = 0;
   static char iccid_buf[40];
   memset(iccid_buf, 0x00, 40);
   uint8_t err = 0;

   /* IMEI number */
   err = nrf_modem_at_cmd(iccid_buf, sizeof(iccid_buf), "AT%XICCID");

   for (i = 0; i < 39; i++)
   {
      if (iccid_buf[i + 9] != (0x0D || 0x0A))
      {
         iccid_buf[i] = iccid_buf[i + 9];
      }
      else
      {
         break;
      }
   }

   return iccid_buf;
}

void modem_update_registration_status(void)
{
   int16_t err = 0;

   /* Update modem and connection paramters*/
   modem_update_information();

   /* Check registrations tatus: offline, searching, roaming */
   err = lte_lc_nw_reg_status_get(&modem.registration_status[0]);

   if (!err)
   {
      /* Print a meessage if there was a change in registration status*/
      if (modem.registration_status[0] != modem.registration_status[1])
      {
         rtc_print_debug_timestamp();
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "E-UTRAN: ");

         switch (modem.AcT)
         {
         case 7:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "7 - LTE-M, ");
            break;

         case 9:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "9 - NB-IoT, ");
            break;

         default:
            break;
         }

         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Network registration status: ");

         switch (modem.registration_status[0])
         {
         case LTE_LC_NW_REG_NOT_REGISTERED:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "0 - Not registered\n");

            modem.connection_stat = false;
            break;

         case LTE_LC_NW_REG_REGISTERED_HOME:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "1 - Registered, home network\n");

            modem.connection_stat = true;

            /* Connect also to keiltronic AWS cloud */
            if ((Parameter.fota_enable == true) && (fota_reboot_while_usb_connected == true))
            {
               aws_fota_process_state = AWS_FOTA_PROCESS_CONNECT;
            }

            /* Add event in event array which is send to cloud in next sync interval */
            NewEvent0x0B(); // Connection up event
            break;

         case LTE_LC_NW_REG_SEARCHING:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "2 - Searching\n");

            modem.connection_stat = false;
            break;

         case LTE_LC_NW_REG_REGISTRATION_DENIED:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "3 - Registration denied\n");

            modem.connection_stat = false;
            break;

         case LTE_LC_NW_REG_UNKNOWN:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "4 - Unkonwn (e.g. out of E-UTRAN coverage)\n");

            modem.connection_stat = false;

            /* Add event in event array which is send to cloud in next sync interval */
            NewEvent0x0C(); // Connection down event
            break;

         case LTE_LC_NW_REG_REGISTERED_ROAMING:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "5 - Registered, roaming\n");

            modem.connection_stat = true;

            /* Connect also to keiltronic AWS cloud */
            if ((Parameter.fota_enable == true) && (fota_reboot_while_usb_connected == true))
            {
               aws_fota_process_state = AWS_FOTA_PROCESS_CONNECT;
            }

            /* Add event in event array which is send to cloud in next sync interval */
            NewEvent0x0B(); // Connection up event
            break;

            // case LTE_LC_NW_REG_REGISTERED_EMERGENCY:
            //   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "8 - Registered, emergency\n");

            //   modem.connection_stat = true;

            //   /* Connect also to keiltronic AWS cloud */
            //   if ((Parameter.fota_enable == true) && (fota_reboot_while_usb_connected == true))
            //   {
            //  //   aws_fota_process_state = AWS_FOTA_PROCESS_CONNECT;
            //   }

            //   /* Add event in event array which is send to cloud in next sync interval */
            //   NewEvent0x0B(); // Connection up event
            //   break;

         case LTE_LC_NW_REG_UICC_FAIL:
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "90 - Not registered due to UICC failture\n");

            modem.connection_stat = false;

            /* Add event in event array which is send to cloud in next sync interval */
            NewEvent0x0C(); // Connection down event
            break;

         default:
            break;
         }

         if (modem.connection_stat == true)
         {
            /* Network operator MNC */
            rtc_print_debug_timestamp();
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Cellular connection successfully established. Operator: ");

            if (!strncmp(modem.oper + 3, "01", 2))
            {
               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Deutsche Telekom");
            }
            else if (!strncmp(modem.oper + 3, "02", 2))
            {
               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Vodafone");
            }
            else if (!strncmp(modem.oper + 3, "03", 2))
            {
               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Telefonica");
            }
            else
            {
               shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "unkown");
            }

            /* RSSI */
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, ", rssi: %ddBm", modem.RSSI);

            /* SIM IMEI number */
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, ", IMEI: %s", modem.IMEI);

            /* Connection details */
            memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
            err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+CGDCONT?");
            rtc_print_debug_timestamp();
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "%s", modem_at_recv_buf);
         }
      }

      /* Save curretn registration status*/
      modem.registration_status[1] = modem.registration_status[0];
   }
   else
   {
      printk("err\r\n");
   }
}

void modem_update_information(void)
{
   int16_t err = 0;
   char *ptr = NULL; // Pointer to string section
   char delimiter[10];

   memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));

   /* Just send an AT command to emptyx the receive buffer at modem*/
   err = nrf_modem_at_printf("AT");

   if (err == 0)
   {
      /* OK, success */
   }
   else if (err < 0)
   {
      /* Failed to send command, err is an nrf_errno */
      printk("nrf_errno\n\r");
      return;
   }
   else if (err > 0)
   {
      /* Command was sent, but response is not "OK" */
      switch (nrf_modem_at_err_type(err))
      {
      case NRF_MODEM_AT_ERROR:
         /* Modem returned "ERROR" */
         printk("error\n\r");
         return;
         break;
      case NRF_MODEM_AT_CME_ERROR:
         /* Modem returned "+CME ERROR" */
         printk("cme error: %d\n\r", nrf_modem_at_err(err));
         return;
         break;
      case NRF_MODEM_AT_CMS_ERROR:
         /* Modem returned "+CMS ERROR" */
         printk("cms error: %d\n\r", nrf_modem_at_err(err));
         return;
         break;
      }
   }

   k_msleep(100);
   memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));

   /* Check connection status */
   err = nrf_modem_at_printf("AT+CEREG=5");

   if (!err)
   {
      err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+CEREG?");

      if (!err)
      {
         strncpy(delimiter, ",", 1);
         ptr = NULL; // Pointer to string section

         /* Find first segment in string */
         ptr = strtok(modem_at_recv_buf, delimiter); // 1. segment strtok cuts the string into pieces separated by delimiter characters
         ptr = strtok(NULL, delimiter);              // 2. segment

         if (ptr != NULL)
         {
            modem.stat = atoi(ptr);

            if (modem.stat == 1 || modem.stat == 5)
            {
               modem.connection_stat = true;
            }
            else
            {
               modem.connection_stat = false;
            }
         }

         ptr = strtok(NULL, delimiter); // 3. segment

         if (ptr != NULL)
         {
            strncpy(modem.tac, ptr + 1, 4);
         }

         strncpy(delimiter, ",", 1);
         ptr = strtok(NULL, delimiter); // 5. segment

         if (ptr != NULL)
         {
            strncpy(modem.cell_id, ptr + 1, 8);
         }

         ptr = strtok(NULL, delimiter); // 5. segment

         if (ptr != NULL)
         {
            modem.AcT = atoi(ptr);
         }

         strncpy(delimiter, "\"", 1);
         ptr = strtok(NULL, delimiter); // 6. segment
         ptr = strtok(NULL, delimiter); // 7. segment

         if (ptr != NULL)
         {
            strncpy(modem.active_time, ptr, 8);
         }

         ptr = strtok(NULL, delimiter); // 9. segment
         ptr = strtok(NULL, delimiter); // 10. segment

         if (ptr != NULL)
         {
            strncpy(modem.tau, ptr, 8);
         }
      }
   }
   k_msleep(100);

   /* Unique SIM serial number */
   memset(modem.UICCID, 0x00, sizeof(modem.UICCID));
   strcpy(modem.UICCID, modem_get_iccid());
   k_msleep(100);

   /* IMEI number */
   memset(modem.IMEI, 0x0D, sizeof(modem.IMEI));
   strcpy(modem.IMEI, modem_get_imei());
   k_msleep(100);

   /* Network operator */
   memset(modem_at_recv_buf, 0, sizeof(modem_at_recv_buf));
   err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+COPS?");

   strncpy(delimiter, ",", 1);
   ptr = NULL; // Pointer to string section

   /* Find first segment in string */
   ptr = strtok(modem_at_recv_buf, delimiter); // strtok cuts the string into pieces separated by delimiter characters

   if (ptr != NULL)
   {
      modem.mode = atoi(ptr + 8); // mode: 0  Automatic network selection, 1  Manual network selection
   }
   k_msleep(100);

   /* Find second segment in string */
   ptr = strtok(NULL, delimiter);
   if (ptr != NULL)
   {
      modem.format = atoi(ptr); // 0 Long alphanumeric <oper> format, 1 Short alphanumeric <oper> format, 2 Numeric <oper> format
   }
   k_msleep(100);

   /* Find third segment in string */
   ptr = strtok(NULL, delimiter);

   memset(modem.oper, 0, sizeof(modem.oper));

   if (ptr != NULL)
   {
      strncpy(modem.oper, ptr + 1, 5); // Mobile Country Code (MCC) and Mobile Network Code (MNC) values.
   }
   k_msleep(100);

   /* band*/
   err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT%XCBAND");

   if (!err)
   {
      modem.band = atoi(modem_at_recv_buf + 9);
   }
   k_msleep(100);

   /* Temperature */
   err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT%XTEMP?");

   if (!err)
   {
      modem.temp = atoi(modem_at_recv_buf + 8);
   }
   k_msleep(100);

   /* Get RSSI */
   modem.RSSI = get_signal_strength();

   /* Get modem firmware version */
   err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+CGMR");

   if (!err)
   {
      strncpy(modem.version, modem_at_recv_buf, 18);
   }
   k_msleep(100);
}

void modem_print_settings(void)
{
   /* SIM IMEI number */
   shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "IMEI number: \t\t\t%s", modem.IMEI);

   if (modem.connection_stat == true)
   {
      char iccid[50];
      uint16_t len = 0;
      memset(iccid, 0, sizeof(iccid));
      len = strlen(modem.UICCID);
      strncpy(iccid, modem.UICCID, (len - 3)); // this removes \r and \n at the end of modem.UICCID

      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "SIM serial number (ICCID): \t%s\n", iccid);
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Connection status: \t\t%d\n", modem.connection_stat);

      /* Network operator MNC */
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Operator: \t\t\t%s:  ", modem.oper);

      if (!strncmp(modem.oper, "262", 3))
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MCC = Germany, ");
      }
      else
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MCC = unknown, ");
      }

      if (!strncmp(modem.oper + 3, "01", 2))
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MNC = Deutsche Telekom\n");
      }
      else if (!strncmp(modem.oper + 3, "02", 2))
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MNC = Vodafone\n");
      }
      else if (!strncmp(modem.oper + 3, "03", 2))
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MNC = Telefonica\n");
      }
      else
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "MNC = unkown\n");
      }

      /* Modem AcT */
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Network connection type (AcT):");

      if (modem.AcT == 7)
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\tLTE-M (%d)\n", modem.AcT);
      }
      else if (modem.AcT == 9)
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\tNB-IoT (%d)\n", modem.AcT);
      }
      else
      {
         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "unknown\n");
      }

      /* Modem AcT */
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Band: \t\t\t\t%d\n", modem.band);

      /* Modem AcT */
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "RSSI: \t\t\t\t%ddBm\n", modem.RSSI);

      /* Modem AcT */
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Temp: \t\t\t\t%d°C\n", modem.temp);

      /* Connection details */
      char modem_at_recv_buf[200]; /* Just send an AT command to emptyx the receive buffer at modem*/
      err = nrf_modem_at_cmd(modem_at_recv_buf, sizeof(modem_at_recv_buf), "AT+CGDCONT?");

      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Connection parameter:\t\t");
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%s", modem_at_recv_buf);
   }
   else
   {
      shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "No network information available because no network connection is currently active\n");
   }
}

int16_t get_signal_strength(void)
{
   // enum at_cmd_state at_state;
   static char buf[100];
   int16_t err = 0;
   int16_t signal_power = 0;

   char delimiter[] = ":, "; // These characters are used to separate the string
   char *ptr = NULL;
   uint16_t iterator = 0;

   err = nrf_modem_at_cmd(buf, sizeof(buf), "AT+CESQ");

   if (err < 0)
   {
      return 99;
   }
   else
   {

      ptr = strtok(buf, delimiter); // Separate the string, get first token

      while (ptr != NULL)
      {
         iterator++;

         // If it is the seventh token from received string, convert this to an integer
         if (iterator == 7)
         {
            signal_power = atoi(ptr);
         }

         // get next token
         ptr = strtok(NULL, delimiter); // strtok zerhackt einen String in mehrere Teilst�cke
      }
   }

   return (-140 + signal_power); // Convert response to real dBm value (nRF AT command datasheet v1.5, page 28)
}

int8_t get_signal_quality(void)
{
   int16_t signal = get_signal_strength();

   if (signal > -90)
   {
      return 4;
   }
   else if (signal <= -90 && signal > -100)
   {
      return 3;
   }
   else if (signal <= -100 && signal > -115)
   {
      return 2;
   }
   else if (signal <= -115)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}