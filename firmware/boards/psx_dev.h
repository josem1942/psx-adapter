/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

// modified to fit the hardware board

#ifndef _BOARDS_PSX_DEV_H
#define _BOARDS_PSX_DEV_H

// For board detection
#define PSX_DEV

// pico_cmake_set PICO_PLATFORM=rp2040

#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)

// --- LED ---
#define PICO_DEFAULT_LED_PIN 5

// --- FLASH ---
// pico_cmake_set_default PICO_FLASH_SIZE_BYTES = (2 * 1024 * 1024)
#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1
#define PICO_FLASH_SPI_CLKDIV 2
#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64

#define PICO_RP2040_B0_SUPPORTED 0

#endif
