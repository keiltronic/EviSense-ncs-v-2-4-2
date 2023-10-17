/**
 * @file spi.h
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 17 Oct 2023
 * @brief This file contains functions headers to communicate with the pheripherals
 * @version 2.0.0
 */

#ifndef SPI_H
#define SPI_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include "parameter_mem.h"

extern void spi_init(void);
extern void uninit_spi();

extern struct device *spi_dev;
extern struct spi_config spi_cfg1;
extern struct spi_config spi_cfg2;

#endif
