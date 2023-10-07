/**
 * @file adc.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef ADC_H
#define ADC_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

#define BATVOLT_R1 10.0f                 // MOhm
#define BATVOLT_R2 5.6f                // MOhm
#define INPUT_VOLT_RANGE 5.5f           // Volts
#define VALUE_RANGE_12_BIT 4.096        // (2^10 - 1) / 1000

#define ADC_NODE DT_NODELABEL(adc)

#define ADC_RESOLUTION 12
#define ADC_GAIN ADC_GAIN_1_6
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ADC_1ST_CHANNEL_ID 0
#define ADC_1ST_CHANNEL_INPUT SAADC_CH_PSELP_PSELP_AnalogInput0

#define BUFFER_SIZE 1

#define ADC_RES_UV 805.66 //(3300mV / 4096 digit)

//extern struct device *adc_dev;
//extern void init_adc(void);
extern void adc_init(void);
extern uint16_t adc_sample(uint8_t channel);

#endif