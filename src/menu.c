#include "menu.h"
#include "constants.h"
#include "log.h"
#include "external_interrupts.h"
#include "display.h"
#include "eeprom.h"
#include "helpers.h"
#include "utilities.h"
#include "rtc.h"
#include "flash.h"
#include "time.h"

#include <stdbool.h>
#include <stdint.h>

#define B_SP (1<<BUTTON_SP)
#define B_R (1<<BUTTON_R)
#define B_L (1<<BUTTON_L)
#define I_RTC (1<<RTC_INT)
#define SET_TIME_EXIT 7
#define BLINK_FREQ 1

/*=============================================================================
 * Helpers
 *===========================================================================*/

enum TimeMode { second, minute, hour, dotw, day, month, year, exit };

void show_active_value(enum TimeMode mode) {
    char mode_name[] = "SEC MIN HOURDOTWDAY MON YEAREXIT";
    set_display_buffer_string(mode_name + mode*4, 0);
    uint8_t value = RTC_get(mode);
    switch (mode) {
    case second:
    case minute:
    case hour:
    case day:
    case month:
        set_display_buffer_number(value, 1);
        break;
    case dotw:
        char dotw[5] = "     ";
        add_dotw_to_string(dotw, value, 1);
        set_display_buffer_string(dotw, 1);
        break;
    case year:
        set_display_buffer_number(value + 2000, 1);
        break;
    case exit:
        clear_display_buffer(1);
        break;
    }
    write_to_all_displays();
}

void change_and_show_time_value(uint8_t type, int8_t change) {
    char mode_name[] = "SEC MIN HOURDOTWDAY MON YEAR";
    set_display_buffer_string(mode_name + type*4, 0);
    uint8_t value;
    switch (type) {
    case SECOND:
    case MINUTE:
        {
            value = RTC_get(type);
            value = circular_addition(0, 59, value, change);
            set_display_buffer_number(value, 1);
            break;
        }
    case HOUR:
        {
            value = RTC_get(type);
            value = circular_addition(0, 23, value, change);
            set_display_buffer_number(value, 1);
        }
        break;
    case DOTW:
        {
            value = RTC_get(type);
            value = circular_addition(1, 7, value, change);
            char dotw[5] = "     ";
            add_dotw_to_string(dotw, value, 1);
            set_display_buffer_string(dotw, 1);
        }
        break;
    case DAY:
        {
            uint8_t values[7];
            RTC_get_all(values);
            uint8_t day = values[4];
            uint8_t month = values[5];
            uint8_t year;
            switch (month) {
            // Jan, Mar, May, July, Aug, Oct, Dec
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                value = circular_addition(1, 31, day, change);
                set_display_buffer_number(value, 1);
                break;
            // Feb
            case 2:
                year = values[6];
                if (year % 4) {
                    // Not divisible by 4
                    value = circular_addition(1, 28, day, change);
                } else {
                    // Divisible by 4
                    value = circular_addition(1, 29, day, change);
                }
                set_display_buffer_number(value, 1);
                break;
            // Apr, June, Sept, Nov
            case 4:
            case 6:
            case 9:
            case 11:
                value = circular_addition(1, 30, day, change);
                set_display_buffer_number(value, 1);
                break;
            default:
                value = -1;
                log(ERROR, "change_time_value, invalid month (%d) when changing day\n", month);
            }
            break;
        }
        break;
    case MONTH:
        {
            value = RTC_get(type);
            value = circular_addition(1, 12, value, change);
            set_display_buffer_number(value, 1);
        }
        break;
    case YEAR:
        {
            value = RTC_get(type);
            value = circular_addition(0, 99, value, change);
            set_display_buffer_number(value + 2000, 1);
        }
        break;
    default:
        log(ERROR, "Change value of unknown type=%d\n", type);
        value = -1;
    }
    if (type == SECOND)
        RTC_set_second_and_start(value);
    else
        RTC_set(type, value);
    write_to_all_displays();
}

/*=============================================================================
 * The menu state machine
 *===========================================================================*/

void menu() {

menu_brightness:
    log(STATE, "Enter menu brightness\n");
    set_display_buffer_long_string("MENUBRIG", 8);
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
    log(STATE, "Enter set brightness\n");

    // Get brightness level
    uint8_t level = EEPROM_read(EEPROM_ADDR_DISPLAY_BRIGHTNESS);
    uint8_t original_level = level;

    // Show brightness level
    apply_display_brightness_level(level);

    set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                if (original_level != level) {
                    EEPROM_write(EEPROM_ADDR_DISPLAY_BRIGHTNESS, level);
                }
                set_display(ON, 0, 1);  // disable blinkmode on display 1
                goto menu_brightness;
            case B_R:
                log(INFO, "Increment brightness\n");
                level = (level + 1) % 8;
                apply_display_brightness_level(level);
                break;
            case B_L:
                log(INFO, "Decrement brightness\n");
                level = level ? (level - 1) : 7;
                apply_display_brightness_level(level);
                break;
        }
    }

menu_time:
    log(STATE, "Enter menu time\n");
    set_display_buffer_long_string("MENUTIME", 8);
    write_to_all_displays();
    int8_t menu_time_mode = 0;
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                goto choose_time;
            case B_R:
                goto menu_exit;
            case B_L:
                goto menu_brightness;
        }
    }

choose_time:
{
    log(STATE, "Enter choose time state\n");
    // Display MODE on the left display and the value of that mode on the
    // right display. Example if MODE=seconds: SEC_ __02.
    // If L/R button is pressed de-/increment the mode. If SP button
    // pressed enter set_time state.

    while (true) {
        log(STATE, "Choose time: mode=%d\n", menu_time_mode);
        show_active_value(menu_time_mode);
        set_alarm_next(menu_time_mode);
        uint8_t interrupts;

        // Busy wait for interrupt
        while (!(interrupts = extract_external_interrupts()));

        switch (interrupts) {
        case 0:
            break;
        case I_RTC:
            set_alarm_next(menu_time_mode);
            log(INFO, "Reset due to RTC update\n");
            show_active_value(menu_time_mode);
            break;
        case B_L:
            RTC_disable_alarm(ALARM0);
            menu_time_mode = (menu_time_mode == SECOND) ? SECOND : menu_time_mode - 1;
            break;
        case B_R:
            RTC_disable_alarm(ALARM0);
            menu_time_mode = (menu_time_mode == exit) ? exit : menu_time_mode + 1;
            break;
        case B_SP:
            if (menu_time_mode != exit)
                goto set_time;
            else
                goto menu_time;
        default:
            log(WARNING, "choose_second unknown interrupt: 0x%x\n", interrupts);
        }
    }
}

set_time:
{
    log(STATE, "Set time: mode=%d\n", menu_time_mode);
    // Display MODE on the left display and the value of that mode on the
    // right display. Example if MODE=seconds: SEC_ __02.
    // If L/R button is pressed de-/increment the value. If SP button
    // pressed go back to choose_time state.
    set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case 0:
                break;
            case I_RTC:
                set_alarm_next(menu_time_mode);
                log(INFO, "Reset due to RTC update\n");
                show_active_value(menu_time_mode);
                break;
            case B_R:
                set_alarm_next(menu_time_mode);
                change_and_show_time_value(menu_time_mode, 1);
                break;
            case B_L:
                set_alarm_next(menu_time_mode);
                change_and_show_time_value(menu_time_mode, -1);
                break;
            case B_SP:
                set_display(ON, 0, 1);  // disable blinkmode on display 1
                goto choose_time;
            default:
                log(WARNING, "set_second unknown interrupt: 0x%x\n", interrupts);
        }
    }
}

menu_exit:
    log(STATE, "Enter menu exit\n");
    set_display_buffer_long_string("MENUEXIT", 8);
    write_to_all_displays();
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                return;
            case B_L:
                goto menu_time;
        }
    }
}

