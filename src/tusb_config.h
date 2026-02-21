/*
 * TinyUSB Configuration for Push-to-Talk Pico
 *
 * Configures TinyUSB for USB HID Telephony Device with Phone Mute functionality.
 */

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for device (port 0 for Pico)
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// RHPort max operational speed (Full-Speed for Pico)
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

// Device mode configuration - required for TinyUSB initialization
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | BOARD_TUD_MAX_SPEED)

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// MCU type is defined by Pico SDK CMake
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

// No RTOS - bare-metal polling loop
#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

// Debug level (0 = off for release)
#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

// Enable Device stack
#define CFG_TUD_ENABLED       1

// Use default speed for this MCU
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

// Memory section and alignment for USB buffers
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN    __attribute__((aligned(4)))
#endif

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

// Endpoint 0 size (control endpoint)
#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif

//------------- USB CLASS CONFIGURATION -------------//

// Disable unused USB classes
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

// Enable HID class for Telephony Device
#define CFG_TUD_HID               1

// HID endpoint buffer size
// 16 bytes is sufficient for our 1-byte Phone Mute report
#define CFG_TUD_HID_EP_BUFSIZE    16

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
