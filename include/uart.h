/**
 * @file uart.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */


#ifndef UART_H
#define UART_H

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <string.h>
#include "rfid.h"
#include "test.h"

#define UART1_BUFFERSIZE             1024
#define EPC_CUT_OFF_START            4 // 4 byte PC
#define EPC_CUT_OFF_END              4 // CRC 4 byte

extern void init_uart(void);

extern struct device *uart1;
extern volatile char uart1_InputBuffer[UART1_BUFFERSIZE];
extern volatile char uart1_RFIDResponse[UART1_BUFFERSIZE];
extern volatile uint8_t uart1_TransmissionReady;
extern volatile uint16_t uart1_TransmissionLength;
extern volatile uint16_t uart1_RFIDResponseTransmissionLength;
extern volatile uint8_t uart1_RFIDResponseFinished;
extern volatile uint8_t uart1_NewDataReceived;
extern volatile uint8_t EPC_tag_buffer_rollover;
extern volatile uint8_t EPC_extended_tag_buffer_rollover;
extern volatile uint32_t tag_interrupt_counter;

#endif