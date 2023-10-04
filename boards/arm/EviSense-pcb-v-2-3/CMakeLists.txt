# Kconfig - nRF91 THINGY91 board configuration
#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause

if(CONFIG_BOARD_EVISENSE_V_2_3 AND NOT DEFINED CONFIG_MCUBOOT)
	zephyr_library()
endif()

if(CONFIG_BOARD_EVISENSE_V_2_3_NS)
	# If TF-M is used, the ADP5360 configuration must be done in non-secure
	# as we can't instruct TF-M to run it.
	if(CONFIG_BUILD_WITH_TFM)
		zephyr_library()
	endif()
endif()
