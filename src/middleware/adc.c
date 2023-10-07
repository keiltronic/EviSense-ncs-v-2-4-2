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

static int16_t m_sample_buffer[BUFFER_SIZE];

static const struct device *adc_dev;

static const struct adc_channel_cfg m_1st_channel_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = ADC_1ST_CHANNEL_ID,
    .input_positive = ADC_1ST_CHANNEL_INPUT,
};

void adc_init(void)
{
  int err;

  adc_dev = DEVICE_DT_GET(ADC_NODE);
  if (!adc_dev)
  {
    printk("Error getting adc failed\n");

    return false;
  }

  err = adc_channel_setup(adc_dev, &m_1st_channel_cfg);
  if (err)
  {
    printk("Error in adc setup: %d\n", err);

    return false;
  }

  return true;
}

uint16_t adc_sample(uint8_t channel)
{
  int err;
  uint16_t battery_voltage = 0;

  const struct adc_sequence sequence = {
      .channels = BIT(ADC_1ST_CHANNEL_ID),
      .buffer = m_sample_buffer,
      .buffer_size = sizeof(m_sample_buffer), // in bytes!
      .resolution = ADC_RESOLUTION,
  };

  if (!adc_dev)
  {
    return -1;
  }

  err = adc_read(adc_dev, &sequence);
  if (err)
  {
    printk("ADC read err: %d\n", err);
    return err;
  }

  float sample_value = 0;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    sample_value += (float)m_sample_buffer[i];
  }
  sample_value /= BUFFER_SIZE;

  return (uint16_t) sample_value;
}