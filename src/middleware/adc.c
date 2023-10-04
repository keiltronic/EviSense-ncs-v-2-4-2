/**
 * @file adc.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions to communicate with the pheripherals
 * @version 1.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "adc.h"

struct device *adc_dev;

static const struct adc_channel_cfg m_1st_channel_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = ADC_1ST_CHANNEL_ID,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
    .input_positive = ADC_1ST_CHANNEL_INPUT,
#endif
};

static int16_t m_sample_buffer[BUFFER_SIZE];

void init_adc(void) {

  uint16_t err;

  adc_dev = device_get_binding("ADC_0");
  if (!adc_dev) {
    printk("device_get_binding ADC_0 failed\n");
  }
  err = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
  if (err) {
    printk("Error in adc setup: %d\n", err);
  }

  NRF_SAADC_NS->TASKS_CALIBRATEOFFSET = 1;

  /* First ADC sample is not valid */
  adc_sample(0);
}

uint16_t adc_sample(uint8_t channel) {
  int ret;

  const struct adc_sequence sequence = {
      .channels = BIT(channel),
      .buffer = m_sample_buffer,
      .buffer_size = sizeof(m_sample_buffer),
      .resolution = ADC_RESOLUTION,
  };

  if (!adc_dev) {
    return -1;
  }

  ret = adc_read(adc_dev, &sequence);
  if (ret) {
    printk("ADC read err: %d\n", ret);
  }
  
  return m_sample_buffer[0];
}