/*
 * Push-to-Talk Pico - USB Descriptors
 * USB HID Telephony Device with Phone Mute (INPUT) and LED feedback (OUTPUT)
 */

#include "bsp/board_api.h"
#include "tusb.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

#define USB_VID   0x1209
#define USB_PID   0x0001
#define USB_BCD   0x0200

tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const* tud_descriptor_device_cb(void)
{
    return (uint8_t const*)&desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor - Telephony Device with INPUT + OUTPUT
//--------------------------------------------------------------------+

/*
 * INPUT report (1 byte): Device -> Host
 *   - Bit 0: Phone Mute state (0=unmuted, 1=muted)
 *   - Bits 1-7: Padding
 *
 * OUTPUT report (1 byte): Host -> Device
 *   - Bit 0: Mute LED
 *   - Bit 1: Off-Hook
 *   - Bit 2: Ring
 *   - Bits 3-7: Padding
 */
uint8_t const desc_hid_report[] =
{
    0x05, 0x0B,        // USAGE_PAGE (Telephony Devices)
    0x09, 0x01,        // USAGE (Phone)
    0xA1, 0x01,        // COLLECTION (Application)

    // === INPUT: Device -> Host (Phone Mute) ===
    0x09, 0x2F,        //   USAGE (Phone Mute)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x75, 0x07,        //   REPORT_SIZE (7) padding
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x03,        //   INPUT (Cnst,Var,Abs)

    // === OUTPUT: Host -> Device (LED feedback) ===
    0x05, 0x08,        //   USAGE_PAGE (LEDs)
    0x09, 0x09,        //   USAGE (Mute)
    0x09, 0x17,        //   USAGE (Off-Hook)
    0x09, 0x18,        //   USAGE (Ring)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x03,        //   REPORT_COUNT (3)
    0x91, 0x02,        //   OUTPUT (Data,Var,Abs)
    0x75, 0x05,        //   REPORT_SIZE (5) padding
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x91, 0x03,        //   OUTPUT (Cnst,Var,Abs)

    0xC0               // END_COLLECTION
};

uint8_t const* tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID         0x81

uint8_t const desc_configuration[] =
{
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 10)
};

uint8_t const* tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

enum
{
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
};

char const* string_desc_arr[] =
{
    (const char[]){ 0x09, 0x04 },  // 0: English (0x0409)
    "masawada",                    // 1: Manufacturer
    "Push-to-Talk Pico",           // 2: Product
    NULL,                          // 3: Serial (uses Pico's unique ID)
};

static uint16_t _desc_str[32 + 1];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    size_t chr_count;

    switch (index)
    {
        case STRID_LANGID:
            memcpy(&_desc_str[1], string_desc_arr[0], 2);
            chr_count = 1;
            break;

        case STRID_SERIAL:
            chr_count = board_usb_get_serial(_desc_str + 1, 32);
            break;

        default:
            if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])))
                return NULL;

            const char* str = string_desc_arr[index];
            chr_count = strlen(str);
            size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1;
            if (chr_count > max_count)
                chr_count = max_count;

            for (size_t i = 0; i < chr_count; i++)
            {
                _desc_str[1 + i] = str[i];
            }
            break;
    }

    _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
    return _desc_str;
}
