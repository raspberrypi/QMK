// Copyright 2024 Raspberry Pi
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

// Double tap reset to go into bootloader definitions:
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 0

// Sets the flash to run in DSPI mode rather than QSPI
#define RP2040_FLASH_W25X10CL

// Crucial to the function of the keyboard when the Pi is off. Ignores USB
#define NO_USB_STARTUP_CHECK

// spare pin to use for debug uart tx
#define UART_TX_PIN GP16

// No diodes
#define MATRIX_HAS_GHOST
