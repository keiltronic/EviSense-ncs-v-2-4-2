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

#include <hal/nrf_saadc.h>
#define ADC_DEVICE_NAME       DT_ADC_0_NAME
#define ADC_RESOLUTION        12 //#define ADC_RESOLUTION 10
#define ADC_GAIN              ADC_GAIN_1_6
#define ADC_REFERENCE         ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME  ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)
#define ADC_1ST_CHANNEL_ID    0
#define ADC_1ST_CHANNEL_INPUT NRF_SAADC_INPUT_AIN0

#define BUFFER_SIZE 1

#define ADC_RES_UV 805.66 //(3300mV / 4096 digit)

extern struct device *adc_dev;
extern void init_adc(void);
extern uint16_t adc_sample(uint8_t channel);

#endif