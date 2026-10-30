#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "psx.h"

const uint32_t interval_ms = 5;
static uint32_t start_ms = 0;

int main() {
    psx_interface psx;
    hid_gamepad_report_t gamepad = {0};
    psx_init_interface(&psx);

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(25, 1);

    tusb_rhport_init_t dev_init = {
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };

    board_init();
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
    board_init_after_tusb();
    
    int max = 5;
    int count = 0;

    while (true) {
        tud_task();

        uint32_t current_time = board_millis();
        if (current_time - start_ms < interval_ms) {
            continue;
        }

        if (tud_hid_ready()) {
            psx_poll_controller(&psx);

            if (psx.digital_buttons) {
                gpio_put(LED, 0);
            }
            else {
                gpio_put(LED, 1);
            }

            psx_convert_to_hid(&psx, &gamepad);

            if (psx.analogMode) {
                if (count < max) {
                    gamepad.buttons |= GAMEPAD_BUTTON_22;
                    count++;
                }
                else {
                    count = 0;
                    psx.analogMode = false;
                }
            }

            tud_hid_report(0, &gamepad, sizeof(gamepad));


            sleep_ms(5);
        }
    }
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}
