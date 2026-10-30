#ifndef PSX_H
#define PSX_H

#include "pico/stdlib.h"
#include "tusb.h"

#define CLK 1
#define CS 2
#define ACK 0
#define MOSI 3
#define MISO 4
#define LED 5

typedef enum psx_buttons {
    PSX_SELECT = 0x1,
    PSX_L3 = 0x2,
    PSX_R3 = 0x4,
    PSX_START = 0x8,
    PSX_UP = 0x10,
    PSX_RIGHT = 0x20,
    PSX_DOWN = 0x40,
    PSX_LEFT = 0x80,
    PSX_L2 = 0x100,
    PSX_R2 = 0x200,
    PSX_L1 = 0x400,
    PSX_R1 = 0x800,
    PSX_TRIANGLE = 0x1000,
    PSX_CIRCLE = 0x2000,
    PSX_CROSS = 0x4000,
    PSX_SQUARE = 0x8000
} psx_buttons;

typedef struct psx_interface {
    uint16_t digital_buttons;
    int8_t rx;
    int8_t ry;
    int8_t lx;
    int8_t ly;
    int8_t rx_dz;
    int8_t ry_dz;
    int8_t lx_dz;
    int8_t ly_dz;
    bool analogMode;
    uint8_t prev_id;
} psx_interface;

uint8_t psx_send_recieve(uint8_t data);
void psx_init_interface(psx_interface *interface);
void psx_poll_controller(psx_interface *interface);
void psx_convert_to_hid(psx_interface *interface, hid_gamepad_report_t *hid_gamepad);

#endif /* PSX_H */