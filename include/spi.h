/**
 * @file spi.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 27 Oct 2022
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 1.0.0
 */

#ifndef SPI_H
#define SPI_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include "parameter_mem.h"

extern void init_spi(void);
extern void uninit_spi();

extern struct device *spi_dev;
extern struct spi_config spi_cfg;

#endif
