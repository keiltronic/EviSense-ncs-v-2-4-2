/**
 * @file test.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 05 Oct 2023
 * @brief This files provides functions for pcb self test and optinally an API for automated testing
 * @version 2.0.0
 */

/*!
 * @defgroup Test
 * @brief This files provides functions for pcb self test and optinally an API for automated testing
 * @{*/
#include "test.h"

uint8_t suppress_rfid_command_charcaters = false;

/* Test result flags*/
uint8_t pcb_test_is_running = false;
uint8_t battery_ok = false;
uint8_t usb_ok = false;
uint8_t button_ok = false;
uint8_t sim_ok = false;
uint8_t connectivity_ok = false;
uint8_t rfid_ok = false;
uint8_t imu_ok = false;
uint8_t mag_ok = false;
uint8_t i2c_ok = false;

uint8_t test_pcb(void)
{
    pcb_test_is_running = true;
    char device_imei[16];
    char iccid[50];
    uint16_t len = 0;

    memset(device_imei, 0, sizeof(device_imei));
    strncpy(device_imei, modem.IMEI, 15);

    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "\n\n=======================================\nRoH PCB Check is starting...\n\n"); // green
    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check connectivity: ");                                                          // blue
    modem_update_information();
    k_msleep(100);

    if (modem.registration_status[0] == LTE_LC_NW_REG_UICC_FAIL)
    {
        shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "No sim card detected.\n");
        sim_ok = false;
    }
    else
    {
        sim_ok = true;

        if (modem.connection_stat == true)
        {
            memset(iccid, 0, sizeof(iccid));
            len = strlen(modem.UICCID);
            strncpy(iccid, modem.UICCID, (len - 3)); // this removes \r and \n at the end of modem.UICCID

            if (modem.RSSI > 0)
            {
                shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Can not connect to the network, RSSI:%d dBm, IMEI:\t%s\n", modem.RSSI, modem.IMEI);
            }
            else
            {
                uint8_t testc = 0;
                shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Connected to network successfully, RSSI:%d dBm, IMEI:\t%s\n", modem.RSSI, device_imei);
                shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\tRSSI:\t");

                connectivity_ok = true;

                for (testc = 0; testc < 5; testc++)
                {
                    modem_update_information();
                    shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d dBm\t", modem.RSSI);
                    k_msleep(1000);
                }
                shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "\n");
            }
        }
        else
        {
            connectivity_ok = false;
            shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "No connection established. If this problem persists, check if the sim card is activated and if the antenna circuit is working.\n");
        }
    }

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check Buzzer and LEDs: "); // blue
//     buzzer.status = BUZZER_BEEP;
//     buzzer.frequency = BUZZER_RES_FREQ;
//     buzzer.duty_cycle = Parameter.buzzer_duty_cycle;
//     buzzer.beep_on_time = 500;
//     buzzer.beep_off_time = 1;
//     buzzer.delay = 0;
//     buzzer.beep_cycles = 1;
//     buzzer.burst_repeat_periode = 0;
//     buzzer.burst_repeat_cycles = 1;
//     set_buzzer(&buzzer);

//     rgb_led.blink_on_time = 500;
//     rgb_led.blink_off_time = 500;
//     rgb_led.pos_slope = 0;
//     rgb_led.neg_slope = 0;
//     rgb_led.repeats = 1;
//     rgb_led.red_value = 0;
//     rgb_led.green_value = 255;
//     rgb_led.blue_value = 0;
//     rgb_led.brightness_value = 255;
//     led_next_state = FLASH;

//     k_msleep(1000);

//     rgb_led.blink_on_time = 500;
//     rgb_led.blink_off_time = 500;
//     rgb_led.pos_slope = 0;
//     rgb_led.neg_slope = 0;
//     rgb_led.repeats = 1;
//     rgb_led.red_value = 255;
//     rgb_led.green_value = 0;
//     rgb_led.blue_value = 0;
//     rgb_led.brightness_value = 255;
//     led_next_state = FLASH;

//     k_msleep(1000);

//     rgb_led.blink_on_time = 500;
//     rgb_led.blink_off_time = 500;
//     rgb_led.pos_slope = 0;
//     rgb_led.neg_slope = 0;
//     rgb_led.repeats = 1;
//     rgb_led.red_value = 0;
//     rgb_led.green_value = 0;
//     rgb_led.blue_value = 255;
//     rgb_led.brightness_value = 255;
//     led_next_state = FLASH;

//     k_msleep(1000);

//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Hearable Beeping and Visible LED flashing in Green-Red-Blue?\n");

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check USB voltage: ");
//     uint16_t rslt = 0;
//     rslt = adc_sample(0);
//     float usb_voltage = 0.0;
//     usb_voltage = (((float)rslt * ADC_RES_UV) / 1000.0) * (15600.0 / 10000.0); // voltage divider ratio
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%.0f mV\n", usb_voltage);

//     if (usb_voltage > 4000) // mv
//     {
//         usb_ok = true;
//     }

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check Battery: ");
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%4.0fmV, %4.1fmA, %2.1f\%\n", battery.Voltage, battery.Current, battery.StateOfCharge);

//     if ((battery.Voltage > 2000) && (battery.Voltage < 4300)) // mV
//     {
//         battery_ok = true;
//     }

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check i2c bus:\n");
//     if (i2c_scanner(1) == I2C_SLAVE_COUNT)
//     {
//         i2c_ok = true;
//     }

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check IMU:\n");
//     uint8_t testi = 0;

//     float imu_sample[5];

//     float test_imu_sensor_time = ((float)accel.sensortime * 0.000039);

//     for (testi = 0; testi < 5; testi++)
//     {
//         imu_sample[testi] = acc_lsb_to_ms2(accel.x);

//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Sensor time:%8.3fs; Acc x: %4.2f; y: %4.2f; z: %4.2f  m/s^2;\t Gyro x: %4.2f; y: %4.2f; z: %4.2f dps\n",
//                       ((float)accel.sensortime * 0.000039),
//                       acc_lsb_to_ms2(accel.x),
//                       acc_lsb_to_ms2(accel.y),
//                       acc_lsb_to_ms2(accel.z),
//                       gyro_lsb_to_dps(gyro.x),
//                       gyro_lsb_to_dps(gyro.y),
//                       gyro_lsb_to_dps(gyro.z));

//         k_msleep(100);
//     }

//     float imu_deviation = 0.0;

//     imu_deviation = standard_deviation(imu_sample, 5);

//     if (imu_deviation != 0.0)
//     {
//         imu_ok = true;
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Standard derivation of acc x = %f. Valid IMU measuments received!\n", imu_deviation);
//     }
//     else
//     {
//         imu_ok = false;
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Standard derivation of acc x = %f. Error: No changes in IMU measuments received.\n", imu_deviation);
//     }

//     k_msleep(100);

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check MAG:\n");
//     uint8_t testm = 0;

//     float mag_sample[5];

//     for (testm = 0; testm < 5; testm++)
//     {
//         mag_sample[testm] = (float)bmm150.data.x;

//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Sensor time:%8.3fs;\t Mag: x: %3d; y: %3d; z: %3d\n",
//                       ((float)accel.sensortime * 0.000039),
//                       bmm150.data.x,
//                       bmm150.data.y,
//                       bmm150.data.z);
//         k_msleep(100);
//     }

//     float mag_deviation = 0.0;

//     mag_deviation = standard_deviation(mag_sample, 5);

//     if (mag_deviation != 0.0)
//     {
//         mag_ok = true;
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Standard derivation of mag x = %f. Valid MAG measuments received!\n", mag_deviation);
//     }
//     else
//     {
//         mag_ok = false;
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Standard derivation of mag x = %f. Error: No changes in MAG measuments received.\n", mag_deviation);
//     }
//     k_msleep(100);

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Check RFID reader: \n");
//     Parameter.notifications_while_usb_connected = true;
//     Parameter.algocontrol_bymag_det = 0;  algorithm_lock = false; Parameter_PushRAMToFlash();

//     RFID_TurnOn();

//     k_msleep(100);

//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Set RFID reader settings. ");
//     Parameter.notifications_while_usb_connected = true;

//     bool rfid_ouput_power_ok = false;
//     uint8_t loop_number = 0;

//     // Set output power
//     do
//     {
//         k_msleep(100);
//         RFID_setOutputPower(Parameter.rfid_output_power);

//         k_msleep(500);

//         if (RFID_getOutputPower() == Parameter.rfid_output_power)
//         {
//             rfid_ouput_power_ok = true;
//         }
//         else
//         {
//             rfid_ouput_power_ok = false;
//             loop_number++;
//         }

//         if (loop_number > 5)
//         {
//             break;
//         }
//     } while (rfid_ouput_power_ok == false);

//     loop_number = 0;

//     // Set frequency
//     bool rfid_ouput_frequency_ok = false;
//     do
//     {
//         k_msleep(100);
//         RFID_setFrequency(Parameter.rfid_frequency);

//         k_msleep(500);

//         if (RFID_getFrequency() == Parameter.rfid_frequency)
//         {
//             rfid_ouput_frequency_ok = true;
//         }
//         else
//         {
//             rfid_ouput_frequency_ok = false;
//             loop_number++;
//         }

//         if (loop_number > 5)
//         {
//             break;
//         }
//     } while (rfid_ouput_frequency_ok == false);

//     loop_number = 0;

//     k_msleep(100);

//     if (rfid_ouput_power_ok == true && rfid_ouput_frequency_ok == true)
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Output power: %d dBm,  Frequency band: %d [1:US, 5:EU]\n", RFID_getOutputPower(), RFID_getFrequency());

//         suppress_rfid_command_charcaters = true;
//         System.RFID_Sniff = true;
//         RFID_autoscan_enabled = true;

//         k_msleep(50);

//         RFID_ScanEnable = true;

//         k_msleep(5000);

//         suppress_rfid_command_charcaters = false;
//         System.RFID_Sniff = false;
//         RFID_autoscan_enabled = false;
//         RFID_ScanEnable = false;

//         RFID_TurnOff();

//         Parameter.notifications_while_usb_connected = false;
        
//         k_msleep(50);

//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Valid RFID readings received?\n");
//     }
//     else
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Error: RFID reader settings not set. Output power is %d dBm, should be %d dBm,  Frequency band is %d, should be %d [1:US, 5:EU]\n", RFID_getOutputPower(), Parameter.rfid_output_power, RFID_getFrequency(), Parameter.rfid_frequency);
//         rfid_ok = false;
//     }

//     ///////////////////////////////////
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_BLUE, "Press the user button: ");

//     uint32_t button_ress_timeout_counter = 0;

//     buzzer.status = BUZZER_BEEP;
//     buzzer.frequency = BUZZER_RES_FREQ;
//     buzzer.duty_cycle = Parameter.buzzer_duty_cycle;
//     buzzer.beep_on_time = 800;
//     buzzer.beep_off_time = 1;
//     buzzer.delay = 0;
//     buzzer.beep_cycles = 1;
//     buzzer.burst_repeat_periode = 0;
//     buzzer.burst_repeat_cycles = 1;
//     set_buzzer(&buzzer);

//     while (1)
//     {
//         if (!btn2_pressed)
//         {
//             button_ress_timeout_counter++;

//             if (button_ress_timeout_counter >= 30000000)
//             {
//                 shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "No button press detected!\n");
//                 break;
//             }
//         }
//         else
//         {
//             shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Button pressed!\n");
//             button_ok = true;
//             break;
//         }
//     }

//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "\n==================================================\n");
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "RoH PCB Check is completed. Comma separated summery:\n");
//     if (modem.connection_stat == true)
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "IMEI: %s, ICCID: %s, ", device_imei, iccid);
//     }
//     else
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "IMEI: %s, ICCID: none, ", device_imei);
//     }
//     shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "battery: %d, usb: %d, sim: %d, connectivity: %d, button: %d, i2c_bus: %d, imu: %d, mag: %d, rfid: %d\n",
//                   battery_ok,
//                   usb_ok,
//                   sim_ok,
//                   connectivity_ok,
//                   button_ok,
//                   i2c_ok,
//                   imu_ok,
//                   mag_ok,
//                   rfid_ok);

//     if (battery_ok && usb_ok && (!sim_ok || (sim_ok && connectivity_ok)) && i2c_ok && imu_ok && mag_ok && rfid_ok)
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "\n#########################################");
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "\n#############  TEST PASSED  #############");
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_GREEN, "\n#########################################\n\n");
//     }
//     else
//     {
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "\n#########################################");
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "\n#############  TEST FAILED  #############");
//         shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "\n#########################################\n\n");
//     }

//     /* Reset all test result flags*/
//     battery_ok = false;
//     usb_ok = false;
//     button_ok = false;
//     connectivity_ok = false;
//     rfid_ok = false;
//     imu_ok = false;
//     mag_ok = false;
//     i2c_ok = false;
//     rfid_ok = false;

//     k_msleep(1000);
//     pcb_test_is_running = false;
//     factorysettings();
//     return 0;
 }