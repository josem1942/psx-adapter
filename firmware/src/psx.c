#include "psx.h"

uint8_t psx_send_recieve(uint8_t data) {
    uint8_t recv = 0;

    for (int i = 0; i < 8; i++) {
        uint8_t bit = (data >> i) & 1;

        gpio_put(CLK, 1);
        gpio_put(MOSI, bit);
        sleep_us(5);

        gpio_put(CLK, 0);
        sleep_us(10);

        gpio_put(CLK, 1);
        if (gpio_get(MISO))
            recv |= (1 << i);
        sleep_us(10);
    }

    int timeout = 1000;
    while (gpio_get(ACK) == 1) { 
        sleep_us(1);
        timeout--;
        if (timeout == 0) break;
    }

    timeout = 1000;
    while (gpio_get(ACK) == 0) {
        sleep_us(1);
        timeout--;
        if (timeout == 0) break;
    }

    sleep_us(10);

    return recv;
}

void psx_init_interface(psx_interface *interface) {
    gpio_init(CLK); gpio_init(CS);
    gpio_init(ACK); gpio_init(MOSI);
    gpio_init(MISO); gpio_init(LED);

    gpio_set_dir(CLK, GPIO_OUT);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_set_dir(MOSI, GPIO_OUT);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_set_dir(MISO, GPIO_IN);
    gpio_set_dir(ACK, GPIO_IN);

    gpio_put(CLK, 1);
    gpio_put(MOSI, 1);
    gpio_put(CS, 1);

    gpio_put(LED, 1);

    interface->digital_buttons = 0;
    interface->prev_id = 0x41;
    interface->lx = 0;
    interface->ly = 0;
    interface->rx = 0;
    interface->ry = 0;
    interface->lx_dz = 30;
    interface->ly_dz = 30;
    interface->rx_dz = 30;
    interface->ry_dz = 30;
    interface->analogMode = false;
}

void psx_poll_controller(psx_interface *interface) {
    bool analog_mode = false;
    uint8_t data;

    interface->digital_buttons = 0;
    interface->lx = 0;
    interface->ly = 0;
    interface->rx = 0;
    interface->ry = 0;

    gpio_put(CS, 0);
    sleep_us(10);
    
    data = psx_send_recieve(0x01);
    data = psx_send_recieve(0x42);
    if (data == 0x73) {
        analog_mode = true;
        if (interface->prev_id == 0x41) {
            interface->prev_id = 0x73;
            interface->analogMode = true;
        }
    }
    else {
        if (interface->prev_id == 0x73) {
            interface->prev_id = 0x41;
            interface->analogMode = true;
        }
    }

    data = psx_send_recieve(0x00);
    data = psx_send_recieve(0x00);
    data = ~data;
    interface->digital_buttons = data;
        
    data = psx_send_recieve(0x00);

    interface->digital_buttons |= (~data) << 8;

    if (!analog_mode) {
        gpio_put(CS, 1);
        return;
    }
    
    uint8_t joy = psx_send_recieve(0x00);
    if (joy & 0x80)
        interface->rx += joy & 0x7F;
    else
        interface->rx -= ~joy - 0x7F;

    joy = psx_send_recieve(0x00);
    if (joy & 0x80)
        interface->ry += joy & 0x7F;
    else
        interface->ry -= ~joy - 0x7F;

    joy = psx_send_recieve(0x00);
    if (joy & 0x80)
        interface->lx += joy & 0x7F;
    else
        interface->lx -= ~joy - 0x7F;

    joy = psx_send_recieve(0x00);
    if (joy & 0x80)
        interface->ly += joy & 0x7F;
    else
        interface->ly -= ~joy - 0x7F;

    gpio_put(CS, 1);

    interface->ly = 255 - interface->ly;
    interface->ry = 255 - interface->ry;

    if (interface->lx <= interface->lx_dz && interface->lx >= -interface->lx_dz) {
        interface->lx = 0;
    }
    if (interface->rx <= interface->rx_dz && interface->rx >= -interface->rx_dz) {
        interface->rx = 0;
    }

    if (interface->ly <= interface->ly_dz && interface->ly >= -interface->ly_dz) {
        interface->ly = 0;
    }
    if (interface->ry <= interface->ry_dz && interface->ry >= -interface->ry_dz) {
        interface->ry = 0;
    }
}

void psx_convert_to_hid(psx_interface *psx, hid_gamepad_report_t *hid_gamepad) {
    hid_gamepad->buttons = 0;
    hid_gamepad->x = psx->lx;
    hid_gamepad->y = psx->ly;
    hid_gamepad->z = psx->rx;
    hid_gamepad->rz = psx->ry;
    hid_gamepad->rx = -128;
    hid_gamepad->ry = -128;

    if (psx->digital_buttons & PSX_UP && psx->digital_buttons & PSX_RIGHT) {
        hid_gamepad->hat = GAMEPAD_HAT_UP_RIGHT;
    }
    else if (psx->digital_buttons & PSX_UP && psx->digital_buttons & PSX_LEFT) {
        hid_gamepad->hat = GAMEPAD_HAT_UP_LEFT;
    }
    else if (psx->digital_buttons & PSX_DOWN && psx->digital_buttons & PSX_RIGHT) {
        hid_gamepad->hat = GAMEPAD_HAT_DOWN_RIGHT;
    }
    else if (psx->digital_buttons & PSX_DOWN && psx->digital_buttons & PSX_LEFT) {
        hid_gamepad->hat = GAMEPAD_HAT_DOWN_LEFT;
    }
    else if (psx->digital_buttons & PSX_UP) {
        hid_gamepad->hat = GAMEPAD_HAT_UP;
    }
    else if (psx->digital_buttons & PSX_UP) {
        hid_gamepad->hat = GAMEPAD_HAT_UP;
    }
    else if (psx->digital_buttons & PSX_DOWN) {
        hid_gamepad->hat = GAMEPAD_HAT_DOWN;
    }
    else if (psx->digital_buttons & PSX_LEFT) {
        hid_gamepad->hat = GAMEPAD_HAT_LEFT;
    }
    else if (psx->digital_buttons & PSX_RIGHT) {
        hid_gamepad->hat = GAMEPAD_HAT_RIGHT;
    }
    else {
        hid_gamepad->hat = GAMEPAD_HAT_CENTERED;
    }

    if (psx->digital_buttons & PSX_CROSS) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_A;
    }
    if (psx->digital_buttons & PSX_CIRCLE) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_B;
    }
    if (psx->digital_buttons & PSX_TRIANGLE) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_Y;
    }
    if (psx->digital_buttons & PSX_SQUARE) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_X;
    }
    if (psx->digital_buttons & PSX_L1) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_TL;
    }
    if (psx->digital_buttons & PSX_R1) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_TR;
    }
    if (psx->digital_buttons & PSX_SELECT) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_SELECT;
    }
    if (psx->digital_buttons & PSX_START) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_START;
    }
    if (psx->digital_buttons & PSX_L3) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_THUMBL;
    }
    if (psx->digital_buttons & PSX_R3) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_THUMBR;
    }

    if (psx->digital_buttons & PSX_L2) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_TL2;
    }
    if (psx->digital_buttons & PSX_R2) {
        hid_gamepad->buttons |= GAMEPAD_BUTTON_TR2;
    }
}
