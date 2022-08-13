#include "menu.h"
#include "constants.h"
#include "log.h"
#include "external_interrupts.h"
#include "display.h"
#include "eeprom.h"
#include "helpers.h"
#include "utilities.h"

#include <stdbool.h>
#include <stdint.h>

#define B_SP (1<<BUTTON_SP)
#define B_R (1<<BUTTON_R)
#define B_L (1<<BUTTON_L)

void menu() {

menu_brightness:
    log(INFO, "Enter menu brightness\n");
    set_display_buffer_long_string("BRIGHTN", 6);
    write_to_all_displays();
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                goto set_brightness;
            case B_R:
                goto menu_time;
        }
    }

set_brightness:
    log(INFO, "Enter set brightness\n");

    // Get brightness level
    uint8_t level = EEPROM_read(EEPROM_ADDR_DISPLAY_BRIGHTNESS);
    uint8_t original_level = level;
    printf("level=%d\n", level);

    // Show brightness level
    apply_display_brightness_level(level);
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                if (original_level != level) {
                    EEPROM_write(EEPROM_ADDR_DISPLAY_BRIGHTNESS, level);
                }
                goto menu_brightness;
            case B_R:
                log(INFO, "Increment brightness\n");
                level = (level + 1) % 8;
                printf("level=%d\n", level);
                apply_display_brightness_level(level);
                break;
            case B_L:
                log(INFO, "Decrement brightness\n");
                level = level ? (level - 1) : 7;
                printf("level=%d\n", level);
                apply_display_brightness_level(level);
                break;
        }
    }

menu_time:
    log(INFO, "Exit menu\n");
    return;
}
