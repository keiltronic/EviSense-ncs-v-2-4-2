/**
 * @file spi.c
 * @author Thomas Keilbach | keiltronic GmbH
 * @date 17 Oct 2023
 * @brief This file contains functions to communicate with the pheripherals
 * @version 2.0.0
 */

/*!
 * @defgroup Peripherals
 * @brief This file contains functions to communicate with the pheripherals
 * @{*/

#include "spi.h"

const struct device *spi_flash_1;
const struct device *spi_flash_2;
struct device *spi_dev;

struct spi_cs_control spim_cs1 = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(mt25ql256_0)),
	.delay = 0,
};

struct spi_cs_control spim_cs2 = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(mt25ql256_1)),
	.delay = 0,
};

struct spi_config spi_cfg1 = {
	.operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 8000000,
	.slave = 0,
	.cs = &spim_cs1,
};

struct spi_config spi_cfg2 = {
	.operation = SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_LINES_SINGLE | SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 8000000,
	.slave = 0,
	.cs = &spim_cs2,
};

void spi_init(void)
{
	spi_flash_1 = DEVICE_DT_GET(DT_ALIAS(myflash1));
	if (!device_is_ready(spi_flash_1))
	{
		printk("SPI master device not ready!\n");
	}

	if (!device_is_ready(spim_cs1.gpio.port))
	{
		printk("SPI master chip select 1 device not ready!\n");
	}

	// spi_flash_2 = DEVICE_DT_GET(DT_ALIAS(myflash2));
	// if (!device_is_ready(spi_flash_2))
	// {
	// 	printk("SPI master device not ready!\n");
	// }

	// if (!device_is_ready(spim_cs2.gpio.port))
	// {
	// 	printk("SPI master chip select 2 device not ready!\n");
	// }
}

void uninit_spi(void)
{
	spi_release(spi_dev, &spi_cfg1);
	//spi_release(spi_dev, &spi_cfg2);
}

void multi_sector_test(const struct device *flash_dev)
{
#define SPI_FLASH_TEST_REGION_OFFSET 0xff000
#define SPI_FLASH_SECTOR_SIZE 4096

	const uint8_t expected[] = {0x55, 0xaa, 0x66, 0x99};
	const size_t len = sizeof(expected);
	uint8_t buf[sizeof(expected)];
	int rc;

	printf("\nPerform test on multiple consequtive sectors");

	/* Write protection needs to be disabled before each write or
	 * erase, since the flash component turns on write protection
	 * automatically after completion of write and erase
	 * operations.
	 */
	printf("\nTest 1: Flash erase\n");

	/* Full flash erase if SPI_FLASH_TEST_REGION_OFFSET = 0 and
	 * SPI_FLASH_SECTOR_SIZE = flash size
	 * Erase 2 sectors for check for erase of consequtive sectors
	 */
	rc = flash_erase(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, SPI_FLASH_SECTOR_SIZE * 2);
	if (rc != 0)
	{
		printf("Flash erase failed! %d\n", rc);
	}
	else
	{
		/* Read the content and check for erased */
		memset(buf, 0, len);
		size_t offs = SPI_FLASH_TEST_REGION_OFFSET;

		while (offs < SPI_FLASH_TEST_REGION_OFFSET + 2 * SPI_FLASH_SECTOR_SIZE)
		{
			rc = flash_read(flash_dev, offs, buf, len);
			if (rc != 0)
			{
				printf("Flash read failed! %d\n", rc);
				return;
			}
			if (buf[0] != 0xff)
			{
				printf("Flash erase failed at offset 0x%x got 0x%x\n",
					   offs, buf[0]);
				return;
			}
			offs += SPI_FLASH_SECTOR_SIZE;
		}
		printf("Flash erase succeeded!\n");
	}

	printf("\nTest 2: Flash write\n");

	size_t offs = SPI_FLASH_TEST_REGION_OFFSET;

	while (offs < SPI_FLASH_TEST_REGION_OFFSET + 2 * SPI_FLASH_SECTOR_SIZE)
	{
		printf("Attempting to write %zu bytes at offset 0x%x\n", len, offs);
		rc = flash_write(flash_dev, offs, expected, len);
		if (rc != 0)
		{
			printf("Flash write failed! %d\n", rc);
			return;
		}

		memset(buf, 0, len);
		rc = flash_read(flash_dev, offs, buf, len);
		if (rc != 0)
		{
			printf("Flash read failed! %d\n", rc);
			return;
		}

		if (memcmp(expected, buf, len) == 0)
		{
			printf("Data read matches data written. Good!!\n");
		}
		else
		{
			const uint8_t *wp = expected;
			const uint8_t *rp = buf;
			const uint8_t *rpe = rp + len;

			printf("Data read does not match data written!!\n");
			while (rp < rpe)
			{
				printf("%08x wrote %02x read %02x %s\n",
					   (uint32_t)(offs + (rp - buf)),
					   *wp, *rp, (*rp == *wp) ? "match" : "MISMATCH");
				++rp;
				++wp;
			}
		}
		offs += SPI_FLASH_SECTOR_SIZE;
	}
}
