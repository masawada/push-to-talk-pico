/*
 * Push-to-Talk Pico - Main Application
 * Raspberry Pi Pico / Pico 2 USB HID Telephony Device
 *
 * Push-to-Talk: BOOTSEL pressed = unmuted, released = muted
 * LED reflects mute state: ON = unmuted (speaking), OFF = muted
 * Google Meet can update mute state via SET_REPORT (OUTPUT)
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

//--------------------------------------------------------------------+
// BOOTSEL Button Reading
//--------------------------------------------------------------------+

static bool __no_inline_not_in_flash_func(get_bootsel_button)(void) {
    const uint CS_PIN_INDEX = 1;

    uint32_t flags = save_and_disable_interrupts();

    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    for (volatile int i = 0; i < 1000; ++i);

    #ifdef __ARM_ARCH_6M__
        #define CS_BIT (1u << 1)
    #else
        #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
    #endif
    bool button_state = !(sio_hw->gpio_hi_in & CS_BIT);

    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

//--------------------------------------------------------------------+
// Configuration
//--------------------------------------------------------------------+

#define BUTTON_POLL_MS      10
#define DEBOUNCE_COUNT      2

//--------------------------------------------------------------------+
// State
//--------------------------------------------------------------------+

static bool mute_state = true;             // Start muted
static bool last_button_state = false;     // BOOTSEL not pressed
static uint8_t debounce_counter = 0;
static bool pending_button_state = false;

//--------------------------------------------------------------------+
// LED Control
//--------------------------------------------------------------------+

static void update_led(void)
{
    // LED ON = unmuted (speaking), LED OFF = muted
    board_led_write(!mute_state);
}

//--------------------------------------------------------------------+
// Function Prototypes
//--------------------------------------------------------------------+

void button_task(void);

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+

int main(void)
{
    board_init();

    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1)
    {
        tud_task();
        button_task();
    }

    return 0;
}

//--------------------------------------------------------------------+
// USB Device Callbacks
//--------------------------------------------------------------------+

void tud_mount_cb(void)
{
    update_led();
}

void tud_umount_cb(void)
{
    board_led_write(false);
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    board_led_write(false);
}

void tud_resume_cb(void)
{
    update_led();
}

//--------------------------------------------------------------------+
// Button Task - Push-to-Talk with debounce
//--------------------------------------------------------------------+

static void send_mute_report(void)
{
    if (!tud_hid_ready()) return;

    uint8_t report = mute_state ? 0x01 : 0x00;
    tud_hid_report(0, &report, sizeof(report));
}

/*
 * Push-to-Talk button handler:
 *   Rising edge (pressed):  unmute + send report + LED ON
 *   Falling edge (released): mute + send report + LED OFF
 */
void button_task(void)
{
    static uint32_t last_poll_ms = 0;

    uint32_t now = board_millis();
    if (now - last_poll_ms < BUTTON_POLL_MS) return;
    last_poll_ms = now;

    bool current_state = get_bootsel_button();

    if (current_state == pending_button_state) {
        if (debounce_counter < DEBOUNCE_COUNT) {
            debounce_counter++;
        }
    } else {
        pending_button_state = current_state;
        debounce_counter = 1;
    }

    if (debounce_counter >= DEBOUNCE_COUNT && pending_button_state != last_button_state) {
        last_button_state = pending_button_state;

        if (last_button_state) {
            // Button pressed: unmute
            mute_state = false;
        } else {
            // Button released: mute
            mute_state = true;
        }

        send_mute_report();
        update_led();
    }
}

//--------------------------------------------------------------------+
// TinyUSB HID Callbacks
//--------------------------------------------------------------------+

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

// Host -> Device feedback (Google Meet sends mute state via SET_REPORT)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;
    (void) report_id;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize < 1) return;
        bool host_mute = (buffer[0] & 0x01) != 0;  // bit0 = Mute LED
        mute_state = host_mute;
        update_led();
    }
}
