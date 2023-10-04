// /**
//  * @file rfid.c
//  * @author Thomas Keilbach | keiltronic GmbH
//  * @date 27 Oct 2022
//  * @brief This file contains functions to communicate with the pheripherals
//  * @version 1.0.0
//  */

// /*!
//  * @defgroup Peripherals
//  * @brief This file contains functions to communicate with the pheripherals
//  * @{*/

// #include "rfid.h"

// uint8_t RFID_ScanEnable = false;
// uint8_t RFID_autoscan_enabled = false;
// uint8_t RFID_IsOn = false;
// uint8_t RFID_TriggeredRead = false;

// /*!
//  *  @brief This is the function description
//  */
// void rfid_power_off(void)
// {
//   gpio_pin_set_raw(gpio_dev, GPIO_PIN_5V_EN, 1);
//   gpio_pin_configure(gpio_dev, 7, GPIO_DISCONNECTED);
//   gpio_pin_configure(gpio_dev, 8, GPIO_DISCONNECTED);
//   gpio_pin_configure(gpio_dev, GPIO_PIN_RFID_TRIGGER, GPIO_DISCONNECTED);

//   /* Disable UARTE1 */
//   NRF_UARTE1_NS->TASKS_STOPTX = 1;
//   NRF_UARTE1_NS->TASKS_STOPRX = 1;
//   NRF_UARTE1_NS->ENABLE = 0;

//   RFID_IsOn = false;
// }

// /*!
//  *  @brief This is the function description
//  */
// void RFID_PowerOn(void)
// {
//   gpio_pin_set_raw(gpio_dev, GPIO_PIN_5V_EN, 0);
//   gpio_pin_configure(gpio_dev, 7, GPIO_INPUT);
//   gpio_pin_configure(gpio_dev, 8, GPIO_OUTPUT);
//   gpio_pin_configure(gpio_dev, GPIO_PIN_RFID_TRIGGER, GPIO_OUTPUT);
//   gpio_pin_set_raw(gpio_dev, GPIO_PIN_RFID_TRIGGER, 1);

//   /* Enable UARTE1 */
//   NRF_UARTE1_NS->ENABLE = 8;
//   NRF_UARTE1_NS->TASKS_STARTRX = 1;
//   NRF_UARTE1_NS->TASKS_STARTTX = 1;

//   RFID_IsOn = true;
// }

// /*!
//  *  @brief This is the function description
//  */
// void config_RFID(void)
// {
//   RFID_PowerOn();
//   k_msleep(300);
//   RFID_setFrequency(Parameter.rfid_frequency);
//   k_msleep(200);
//   RFID_setOutputPower(Parameter.rfid_output_power);
//   k_msleep(200);
// }

// /*!
//  *  @brief This is the function description
//  */
// void RFID_TurnOff(void)
// {
//   RFID_IsOn = false;
// }

// /*!
//  *  @brief This is the function description
//  */
// void RFID_TurnOn(void)
// {
//   RFID_IsOn = true;
// }

// /*!
//  *  @brief This is the function description
//  */
// void RFID_TriggerSingleScan(void)
// {
//   uart_fifo_fill(uart1, "\nQ\r", sizeof("\nQ\r"));
//   RFID_TriggeredRead = true;
// }

// /*!
//  *  @brief This is the function description
//  */
// void rfid_trigger_multi_read(void)
// {
//   uart_fifo_fill(uart1, "\nU\r", sizeof("\nU\r"));
//  // rtc_print_debug_timestamp();
//   //shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Triggered RFID reader\n");
//   RFID_TriggeredRead = true;
// }

// /*!
//  *  @brief This is the function description
//  */
// uint8_t RFID_getFWVersion(void)
// {
//   uart_fifo_fill(uart1, "\nV\r", sizeof("V\r"));
//   return 0;
// }
// /*!
//  *  @brief This is the function description
//  */
// uint8_t RFID_getID(void)
// {
//   uart_fifo_fill(uart1, "\nS\r", sizeof("S\r"));
//   return 0;
// }
// /*!
//  *  @brief This is the function description

//  */
// void RFID_setOutputPower(int8_t tx_dbm)
// {
//   switch (tx_dbm)
//   {
//   case 0:
//     uart_fifo_fill(uart1, RFID_TX_POWER_N2_DBM, sizeof(RFID_TX_POWER_N2_DBM));
//     break;
//   case 1:
//     uart_fifo_fill(uart1, RFID_TX_POWER_N1_DBM, sizeof(RFID_TX_POWER_N1_DBM));
//     break;
//   case 2:
//     uart_fifo_fill(uart1, RFID_TX_POWER_0_DBM, sizeof(RFID_TX_POWER_0_DBM));
//     break;
//   case 3:
//     uart_fifo_fill(uart1, RFID_TX_POWER_1_DBM, sizeof(RFID_TX_POWER_1_DBM));
//     break;
//   case 4:
//     uart_fifo_fill(uart1, RFID_TX_POWER_2_DBM, sizeof(RFID_TX_POWER_2_DBM));
//     break;
//   case 5:
//     uart_fifo_fill(uart1, RFID_TX_POWER_3_DBM, sizeof(RFID_TX_POWER_3_DBM));
//     break;
//   case 6:
//     uart_fifo_fill(uart1, RFID_TX_POWER_4_DBM, sizeof(RFID_TX_POWER_4_DBM));
//     break;
//   case 7:
//     uart_fifo_fill(uart1, RFID_TX_POWER_5_DBM, sizeof(RFID_TX_POWER_5_DBM));
//     break;
//   case 8:
//     uart_fifo_fill(uart1, RFID_TX_POWER_6_DBM, sizeof(RFID_TX_POWER_6_DBM));
//     break;
//   case 9:
//     uart_fifo_fill(uart1, RFID_TX_POWER_7_DBM, sizeof(RFID_TX_POWER_7_DBM));
//     break;
//   case 10:
//     uart_fifo_fill(uart1, RFID_TX_POWER_8_DBM, sizeof(RFID_TX_POWER_8_DBM));
//     break;
//   case 11:
//     uart_fifo_fill(uart1, RFID_TX_POWER_9_DBM, sizeof(RFID_TX_POWER_9_DBM));
//     break;
//   case 12:
//     uart_fifo_fill(uart1, RFID_TX_POWER_10_DBM, sizeof(RFID_TX_POWER_10_DBM));
//     break;
//   case 13:
//     uart_fifo_fill(uart1, RFID_TX_POWER_11_DBM, sizeof(RFID_TX_POWER_11_DBM));
//     break;
//   case 14:
//     uart_fifo_fill(uart1, RFID_TX_POWER_12_DBM, sizeof(RFID_TX_POWER_12_DBM));
//     break;
//   case 15:
//     uart_fifo_fill(uart1, RFID_TX_POWER_13_DBM, sizeof(RFID_TX_POWER_13_DBM));
//     break;
//   case 16:
//     uart_fifo_fill(uart1, RFID_TX_POWER_14_DBM, sizeof(RFID_TX_POWER_14_DBM));
//     break;
//   case 17:
//     uart_fifo_fill(uart1, RFID_TX_POWER_15_DBM, sizeof(RFID_TX_POWER_15_DBM));
//     break;
//   case 18:
//     uart_fifo_fill(uart1, RFID_TX_POWER_16_DBM, sizeof(RFID_TX_POWER_16_DBM));
//     break;
//   case 19:
//     uart_fifo_fill(uart1, RFID_TX_POWER_17_DBM, sizeof(RFID_TX_POWER_17_DBM));
//     break;
//   case 20:
//     uart_fifo_fill(uart1, RFID_TX_POWER_18_DBM, sizeof(RFID_TX_POWER_18_DBM));
//     break;
//   case 21:
//     uart_fifo_fill(uart1, RFID_TX_POWER_19_DBM, sizeof(RFID_TX_POWER_19_DBM));
//     break;
//   case 22:
//     uart_fifo_fill(uart1, RFID_TX_POWER_20_DBM, sizeof(RFID_TX_POWER_20_DBM));
//     break;
//   case 23:
//     uart_fifo_fill(uart1, RFID_TX_POWER_21_DBM, sizeof(RFID_TX_POWER_21_DBM));
//     break;
//   case 24:
//     uart_fifo_fill(uart1, RFID_TX_POWER_22_DBM, sizeof(RFID_TX_POWER_22_DBM));
//     break;
//   case 25:
//     uart_fifo_fill(uart1, RFID_TX_POWER_23_DBM, sizeof(RFID_TX_POWER_23_DBM));
//     break;
//   case 26:
//     uart_fifo_fill(uart1, RFID_TX_POWER_24_DBM, sizeof(RFID_TX_POWER_24_DBM));
//     break;
//   case 27:
//     uart_fifo_fill(uart1, RFID_TX_POWER_25_DBM, sizeof(RFID_TX_POWER_25_DBM));
//     break;
//   default:
//     break;
//   }
// }

// /*!
//  *  @brief This is the function description


//  */
// int8_t RFID_getOutputPower(void)
// {

//   volatile int8_t output_power = 0;
//   char command[8];

//   System.RFID_TransparentMode = true;

//   // if (RFID_IsOn == false) {
//   //   config_RFID();
//   // }

//   RFID_IsOn = true;

//   uart1_RFIDResponseTransmissionLength = 0;

//   /* Build a command with \n and \r signs which is accepted by the RFID reader module */
//   strcpy(command, "\nN0,00\r");

//   /* Send command to RFID module and wait for a certain time for the response */
//   memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
//   uart_fifo_fill(uart1, command, sizeof(command) - 1);

//   /* Record uart input coming from rfid module for 1000 ms */
//   while (!uart1_RFIDResponseFinished)
//   {
//   }

//   System.RFID_TransparentMode = false;

//   /* Decode string into number */
//   if (!strncmp(uart1_RFIDResponse, "N1B", 3))
//   {
//     output_power = 27;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N1A", 3))
//   {
//     output_power = 26;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N19", 3))
//   {
//     output_power = 25;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N18", 3))
//   {
//     output_power = 24;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N17", 3))
//   {
//     output_power = 23;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N16", 3))
//   {
//     output_power = 22;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N15", 3))
//   {
//     output_power = 21;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N14", 3))
//   {
//     output_power = 20;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N13", 3))
//   {
//     output_power = 19;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N12", 3))
//   {
//     output_power = 18;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N11", 3))
//   {
//     output_power = 17;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N10", 3))
//   {
//     output_power = 16;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0F", 3))
//   {
//     output_power = 15;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0E", 3))
//   {
//     output_power = 14;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0D", 3))
//   {
//     output_power = 13;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0C", 3))
//   {
//     output_power = 12;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0B", 3))
//   {
//     output_power = 11;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N0A", 3))
//   {
//     output_power = 10;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N09", 3))
//   {
//     output_power = 9;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N08", 3))
//   {
//     output_power = 8;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N07", 3))
//   {
//     output_power = 7;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N06", 3))
//   {
//     output_power = 6;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N05", 3))
//   {
//     output_power = 5;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N04", 3))
//   {
//     output_power = 4;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N03", 3))
//   {
//     output_power = 3;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N02", 3))
//   {
//     output_power = 2;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N01", 3))
//   {
//     output_power = 1;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N00", 3))
//   {
//     output_power = 0;
//   }
//   else
//   {
//   }

//   memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
//   uart1_RFIDResponseTransmissionLength = 0;
//   uart1_RFIDResponseFinished = false;

//   return output_power;
// }

// /*!
//  *  @brief This is the function description


//  */
// void RFID_setFrequency(uint8_t freq)
// {
//   switch (freq)
//   {
//   case 1:
//     uart_fifo_fill(uart1, RFID_FREQ_US, sizeof(RFID_FREQ_US));
//     break;
//   case 2:
//     uart_fifo_fill(uart1, RFID_FREQ_TW, sizeof(RFID_FREQ_TW));
//     break;
//   case 3:
//     uart_fifo_fill(uart1, RFID_FREQ_CN, sizeof(RFID_FREQ_CN));
//     break;
//   case 4:
//     uart_fifo_fill(uart1, RFID_FREQ_CN2, sizeof(RFID_FREQ_CN2));
//     break;
//   case 5:
//     uart_fifo_fill(uart1, RFID_FREQ_EU, sizeof(RFID_FREQ_EU));
//     break;
//   case 6:
//     uart_fifo_fill(uart1, RFID_FREQ_JP, sizeof(RFID_FREQ_JP));
//     break;
//   case 7:
//     uart_fifo_fill(uart1, RFID_FREQ_KR, sizeof(RFID_FREQ_KR));
//     break;
//   case 8:
//     uart_fifo_fill(uart1, RFID_FREQ_VN, sizeof(RFID_FREQ_VN));
//     break;
//   default:
//     break;
//   }
// }

// /*!
//  *  @brief This is the function description


//  */
// uint8_t RFID_getFrequency(void)
// {

//   volatile int8_t freq = 0;
//   char command[8];

//   System.RFID_TransparentMode = true;

//   // if (RFID_IsOn == false) {
//   //   config_RFID();
//   // }

//   RFID_IsOn = true;

//   uart1_RFIDResponseTransmissionLength = 0;

//   /* Build a command with \n and \r signs which is accepted by the RFID reader module */
//   strcpy(command, "\nN4,00\r");

//   /* Send command to RFID module and wait for a certain time for the response */
//   memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
//   uart_fifo_fill(uart1, command, sizeof(command) - 1);

//   /* Record uart input coming from rfid module for 1000 ms */
//   while (!uart1_RFIDResponseFinished)
//   {
//   }

//   System.RFID_TransparentMode = false;

//   /* Decode string into number */
//   if (!strncmp(uart1_RFIDResponse, "N01", 3))
//   {
//     freq = 1;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N02", 3))
//   {
//     freq = 2;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N03", 3))
//   {
//     freq = 3;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N04", 3))
//   {
//     freq = 4;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N05", 3))
//   {
//     freq = 5;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N06", 3))
//   {
//     freq = 6;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N07", 3))
//   {
//     freq = 7;
//   }
//   else if (!strncmp(uart1_RFIDResponse, "N08", 3))
//   {
//     freq = 8;
//   }
//   else
//   {
//   }

//   memset(uart1_RFIDResponse, '\0', sizeof(uart1_RFIDResponse));
//   uart1_RFIDResponseTransmissionLength = 0;
//   uart1_RFIDResponseFinished = false;

//   return freq;
// }