/**\mainpage
 * Copyright (C) 2020 - 2022 keiltronic GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED

 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 */

/**
 * @file main.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 06 Oct 2023
 * @brief This main file is the initial application starting point after Zephyr OS booted up
 * @version 2.0.0
 */

/*!
 * @defgroup main
 * @brief This main file is the initial application starting point after Zephyr OS booted up
 * @{*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/sys/printk.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
// #include "algorithms.h"
// #include "battery_gauge.h"
// #include "cloud.h"
// #include "coap.h"
#include "commands.h"
#include "datalog_mem.h"
#include "device_mem.h"
#include "flash.h"
#include "gpio.h"
#include "i2c.h"
#include "led.h"
// #include "modem.h"
// #include "notification.h"
// #include "parameter_mem.h"
// #include "pwm.h"
// #include "rfid.h"
#include "rtc.h"
#include "spi.h"
#include "system_mem.h"
#include "threads.h"
#include "uart.h"
// #include "usb.h"
// #include "aws_fota.h"
// #include "test.h"
#include "hard_reset.h"

/*!
 * @brief This functions validates if all parameter values stored in the external flash are valid.
 * @note This function talks directly over the SPI bus with the external NOR flash memory. For this it uses the API calls defined in flash.c
 * @see flash.c
 */
void ValidateParameterInExernalFlash(void)
{
	/* Load default values into RAM */
	System_InitRAM();
	Parameter_InitRAM();
	Device_InitRAM();
	epc_mem_init();
	notification_init();
	notification_init_action_matrix();

	/* Fetch modem version */
	// modem_update_information();

	/* Print firmware version */
	if (pcb_test_is_running == false)
	{
		rtc_print_debug_timestamp();
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "FW version application: %d.%d.%d, Build time: " __DATE__ ", " __TIME__ "\n", Device.FirmwareMajorVersion, Device.FirmwareMinorVersion, Device.FirmwareInternVersion);

		/* Print modem firmware version */
		//	rtc_print_debug_timestamp();
		//	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "FW version of modem: \t%s", modem.version);

		/* Print hardware information */
		//		rtc_print_debug_timestamp();
		//		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "PCB version:\t\t%d.%d,\tIMEI: %s", Device.HardwareMajorVersion, Device.HardwareMinorVersion, modem.IMEI);
	}

	/* Read stored firmware version from flash */
	//	DEVICE device_mem_flash;
	//	flash_read(GPIO_PIN_FLASH_CS2, DEVICE_MEM, &device_mem_flash.device_mem_bytes[0], DEVICE_MEM_LENGTH_RAM);

	/* Firmware update recognized, reset parameter setting */
	//	if (device_mem_flash.FirmwareMajorVersion != Device.FirmwareMajorVersion || device_mem_flash.FirmwareMinorVersion != Device.FirmwareMinorVersion || device_mem_flash.FirmwareInternVersion != Device.FirmwareInternVersion)
	// {
	// 	if (pcb_test_is_running == false)
	// 	{
	// 		rtc_print_debug_timestamp();
	// 		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "Stored fw version: %d.%d.%d! Data in external flash is obsolete. Restoring default values, please wait...\n", device_mem_flash.FirmwareMajorVersion, device_mem_flash.FirmwareMinorVersion, device_mem_flash.FirmwareInternVersion);
	// 	}
	// 	Device_PopFlashToRAM();
	// 	factorysettings();
	// 	//modem_initial_setup();
	// }
	// else
	// {
	// 	if (pcb_test_is_running == false)
	// 	{
	// 		rtc_print_debug_timestamp();
	// 		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Loading settings from external flash\n");
	// 	}
	// 	Parameter_PopFlashToRAM();
	// 	Device_PopFlashToRAM();
	// }

	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Checking last valid frame number in log memory (stored in external flash memory): ");
	// }

	// System.datalogFrameNumber = flash_GetLastFrameNumber(GPIO_PIN_FLASH_CS1, FLASH_SUBSUBSECTOR_SIZE, DATALOG_MEM, DATALOG_MEM_LENGTH, DATALOG_FRAME_LENGTH);
	// if (pcb_test_is_running == false)
	// {
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "%d\n", System.datalogFrameNumber);
	// }

	// /* Check for EPCs in external flash */
	// EPC_last_rfid_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, RFID_RECORD_REGION, RFID_RECORD_REGION_LENGTH, RFID_RECORD_BYTE_LENGTH);
	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Stored rfid records: %d\n", EPC_last_rfid_record_index);
	// }

	// EPC_last_room_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, ROOM_RECORD_REGION, ROOM_RECORD_REGION_LENGTH, ROOM_RECORD_BYTE_LENGTH);
	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Stored room records: %d\n", EPC_last_room_record_index);
	// }

	// EPC_last_mop_record_index = EPC_Memory_GetLastIndex(GPIO_PIN_FLASH_CS2, MOP_RECORD_REGION, MOP_RECORD_REGION_LENGTH, MOP_RECORD_BYTE_LENGTH);
	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Stored mop records: %d\n", EPC_last_mop_record_index);
	// }
}

/**
 * @brief This functions checks how much space is avaliable in heap memory
 *
 * @return uint32_t: Returns the available bytes in heap.
 */
uint32_t memcheck_heap_freespace(void)
{
	uint32_t blockSize = 8;
	uint32_t i = 1;

	wdt_reset();

	while (true)
	{
		char *p = (char *)malloc(i * blockSize);
		if (p == NULL)
		{
			break;
		}
		free(p);
		++i;
	}
	return ((i - 1) * blockSize);
}

/*!
 * @brief This function resets all parameters and device settings back to factory default settings.
 * @note This function resets only globale variables which are defined in parameter_mem. Device-/datalog memory
 * and EPC database will not get cleared.
 * @see parameter_mem.c
 */
void factorysettings(void)
{
	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Erasing parameter, device and datalog memory and reset them to default values. This may take a while, please wait.\n");
	// }
	// DEVICE Device_cpy;

	// /* Deep copy device structure from RAM*/
	// Device_cpy.OpertingTime = Device.OpertingTime;
	// Device_cpy.ChargeCycles = Device.ChargeCycles;

	// if (Device_cpy.ChargeCycles == 0xFFFFFFFF)
	// {
	// 	Device_cpy.ChargeCycles = 0UL;
	// }

	// /* Load default values to RAM */
	// Parameter_InitRAM();
	// Device_InitRAM();

	// /* Rewrite local device parameter in RAM with the temporarily stored data */
	// Device.OpertingTime = Device_cpy.OpertingTime;
	// Device.ChargeCycles = Device_cpy.ChargeCycles;

	// /* Reset datalog memory */
	// datalog_CleardatalogAll();
	// datalog_MemoryFull = false;

	// /* Write the data back to flash memory*/
	// Device_PushRAMToFlash();
	// Parameter_PushRAMToFlash();

	if (Parameter.rfid_disable == false)
	{
		//	config_RFID();
	}

	// if (pcb_test_is_running == false)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Factory reset done.\n");
	// }
}

/*!
 * @brief This main function is the initial starting point of the application after Zephyr OS booted up.
 * @details The purpose of this main function is to initialize all pheriperals and finally to start all threads.
 * Since the main function itself is also a thread from Zephyr OS point of view, the main function (thread)
 * will get terminated and is running only once after the system booted up.
 */
void main(void)
{
	uint32_t reset_reason = 0;
	int16_t err = 0;

	hard_reset_init();
	dev_led_init();
	button_init();
	gpio_pin_set_dt(&dev_led, 1); // Enable blue dev led while booting
	rfid_init();
	rfid_power_on();

	Event_ClearArray();

	// // at_cmd_init();
	init_rtc();

	k_msleep(50);

	/* Readout and output last reset reason */
	reset_reason = nrf_power_resetreas_get(NRF_POWER_NS);

	/* If device restarts from hibernate mode, do a real hardware reset */
	if (reset_reason == 0x4)
	{
		gpio_pin_set_dt(&reset_switch, 1);
	}

	/* Welcome message */
	rtc_print_debug_timestamp();
	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "VILEDA PROFESSIONAL - EviSense\n");
	rtc_print_debug_timestamp();
	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "==============================\n");

	nrf_power_resetreas_clear(NRF_POWER_NS, 0xFFFFFFFF);
	rtc_print_debug_timestamp();
	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "Last reset reason: ");

	switch (reset_reason)
	{
	case 0x0:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Power-on reset or a brownout reset\n", reset_reason);
		break;

	case 0x1:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset from pin reset detected\n", reset_reason);
		break;

	case 0x2:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset from global watchdog detected\n", reset_reason);
		break;

	case 0x4:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset due to wakeup from System OFF mode, when wakeup is triggered by DETECT signal from GPIO\n", reset_reason);
		break;

	case 0x10:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset due to wakeup from System OFF mode, when wakeup is triggered by entering debug interface mode\n", reset_reason);
		break;

	case 0x10000:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset from AIRCR.SYSRESETREQ detected\n", reset_reason);
		break;

	case 0x20000:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset from CPU lock-up detected\n", reset_reason);
		break;

	case 0x30000:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_CYAN, "0x%X - Reset triggered through CTRL-AP\n", reset_reason);
		break;

	default:
		shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "0x%X - Unkown reset reason\n", reset_reason);
		break;
	}

	if (reset_reason & POWER_RESETREAS_OFF_Msk)
	{
		nrf_power_resetreas_clear(NRF_POWER_NS, 0x70017); // Clear all RESETREAS when waking up from System OFF sleep by GPIO.
	}

	/* Init peripherals */
	wdt_init();
	wdt_reset();
	adc_init();
	// init_spi();
	uart1_init(); // Inits UART 1 for rfid module (UART 0 for shell and temrinal is initialized by Zephyr OS and devicetree)
	// init_pwm();

	/* Check if charger is pluged into the device while it is booting */
	uint16_t vusb_digit = 0;
	vusb_digit = adc_sample(0);

	if (vusb_digit >= VUSB_THRES_DIGIT && vusb_digit <= 4095)
	{
		charger_plug_in_while_reboot = true;
	}

	// /* Init flash memory and load NVM parameters to RAM */
	// init_flash(GPIO_PIN_FLASH_CS1);
	// init_flash(GPIO_PIN_FLASH_CS2);
	wdt_reset();
	ValidateParameterInExernalFlash();

	// /* Init propritary driver  which depents on loaded parameters*/
	battery_gauge_init();
	rgb_led_init();
	imu_init();
	command_init();
	// init_algorithms();
	wdt_reset();

	/* Create power on event*/
	NewEvent0x13();

	// /* All initializations were successful mark image as working so that we
	//  * will not revert upon reboot.
	//  */
	// err = boot_write_img_confirmed();

	// if (err < 0)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "ERROR: boot_write_img not confirmed errno %d\n", errno);
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_RED, "ERROR: boot_write_img not confirmed err %d\n", err);
	// }

	// /* Init some variables */
	// modem.registration_status[0] = LTE_LC_NW_REG_UNKNOWN;
	// modem.registration_status[1] = LTE_LC_NW_REG_UNKNOWN;

	// /* Threads takeover the system handling, main (main thread) is destroyed after the end of this function is reached */
	// wdt_reset();
	init_threads();

	// k_msleep(100);

	// /* Allow FOTA connection to server only after reboot and if USB is connected (charging)*/
	// wdt_reset();
	// if (System.charger_connected == true)
	// {
	// 	fota_reboot_while_usb_connected = true;
	// }

	if (Parameter.rfid_autoscan == true)
	{
		RFID_autoscan_enabled = true;
		config_RFID();
		k_msleep(50);
		RFID_ScanEnable = true;

		/* For debugging prupose enable blue dev led when motion detected*/
		gpio_pin_set_dt(&dev_led, 0);
	}
	else
	{
		/* For debugging prupose enable blue dev led when motion detected*/
		gpio_pin_set_dt(&dev_led, 1);
	}

	// /* Set "device on frame" detection */
	// k_msleep(100);
	// if (Parameter.algocontrol_bymag_det == 0)
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Mop algorithm is unlocked (mop magnet detection deactivated, mop algorithm is running)\n");
	// 	algorithm_lock = false;
	// }
	// else
	// {
	// 	rtc_print_debug_timestamp();
	// 	shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_YELLOW, "Automatic device on frame detecion activated\n");
	// }

	// if (Parameter.notification_test == true)
	// {
	// 	notification_set_priority(NOTIFICATION_PRIORITY_LEVEL_LOWEST);
	// }

	// /* Clean event storage region in external flash */
	// rtc_print_debug_timestamp();
	// shell_fprintf(shell_backend_uart_get_ptr(), SHELL_VT100_COLOR_DEFAULT, "Erasing stored events in flash memory\n");
	// Event_ClearCompleteFlash();

	/* Set flag that boot sequence completed before main thread is terminated */
	System.boot_complete = true;

	/* Disable blue dev led */
	//gpio_pin_set_dt(&dev_led, 0);
}