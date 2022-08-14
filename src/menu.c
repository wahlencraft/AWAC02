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

#include <stdbool.h>
#include <stdint.h>

#define B_SP (1<<BUTTON_SP)
#define B_R (1<<BUTTON_R)
#define B_L (1<<BUTTON_L)
#define I_RTC (1<<RTC_INT)
#define SET_TIME_EXIT 7
#define BLINK_FREQ 1

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
    while (true) {
        uint8_t interrupts = extract_external_interrupts();
        switch (interrupts) {
            case B_SP:
                goto choose_second;
            case B_R:
                goto menu_exit;
            case B_L:
                goto menu_brightness;
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

/*=============================================================================
 * Set time
 *===========================================================================*/
choose_second:
    log(STATE, "Enter choose_second\n");
    {
        // Show active value
        set_display_buffer_string("SEC ", 0);
        uint8_t value = RTC_get(SECOND);
        set_display_buffer_number(value, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(SECOND);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                case B_L:
                    break;
                case I_RTC:
                    set_alarm_next(SECOND);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(SECOND);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_minute;
                    break;
                case B_SP:
                    goto set_second;
                default:
                    log(WARNING, "choose_second unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_minute:
    log(STATE, "Enter choose_minute\n");
    {
        // Show active value
        set_display_buffer_string("MIN ", 0);
        uint8_t value = RTC_get(MINUTE);
        set_display_buffer_number(value, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(MINUTE);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(MINUTE);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(MINUTE);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_hour;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_second;
                    break;
                case B_SP:
                    goto set_minute;
                default:
                    log(WARNING, "choose_second unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_hour:
    log(STATE, "Enter choose_hour\n");
    {
        // Show active value
        set_display_buffer_string("HOUR", 0);
        uint8_t value = RTC_get(HOUR);
        set_display_buffer_number(value, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(HOUR);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_dotw;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_minute;
                    break;
                case B_SP:
                    goto set_hour;
                default:
                    log(WARNING, "choose_hour unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_dotw:
    log(STATE, "Enter choose_dotw\n");
    {
        // Show active value
        set_display_buffer_string("DOTW", 0);
        uint8_t value = RTC_get(DOTW);
        char dotw[5] = "     ";
        add_dotw_to_string(dotw, value, 1);
        set_display_buffer_string(dotw, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(HOUR);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(DOTW);
                    add_dotw_to_string(dotw, value, 1);
                    set_display_buffer_string(dotw, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_day;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_hour;
                    break;
                case B_SP:
                    goto set_dotw;
                default:
                    log(WARNING, "choose_dotw unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_day:
    log(STATE, "Enter choose_day\n");
    {
        // Show active value
        set_display_buffer_string("DAY ", 0);
        uint8_t value = RTC_get(DAY );
        set_display_buffer_number(value, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(HOUR);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(DAY);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_month;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_dotw;
                    break;
                case B_SP:
                    goto set_day;
                default:
                    log(WARNING, "choose_day unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_month:
    log(STATE, "Enter choose_month\n");
    {
        // Show active value
        set_display_buffer_string("MON ", 0);
        uint8_t value = RTC_get(MONTH);
        set_display_buffer_number(value, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(HOUR);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(MONTH);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_year;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_day;
                    break;
                case B_SP:
                    goto set_month;
                default:
                    log(WARNING, "choose_month unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

choose_year:
    log(STATE, "Enter choose_year\n");
    {
        // Show active value
        set_display_buffer_string("YEAR", 0);
        uint8_t value = RTC_get(YEAR);
        set_display_buffer_number(value + 2000, 1);
        write_to_all_displays();

        // Set alarm for value change
        set_alarm_next(HOUR);

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(YEAR);
                    set_display_buffer_number(value + 2000, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    RTC_disable_alarm(ALARM0);
                    goto choose_exit;
                    break;
                case B_L:
                    RTC_disable_alarm(ALARM0);
                    goto choose_month;
                    break;
                case B_SP:
                    goto set_year;
                default:
                    log(WARNING, "choose_year unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }


choose_exit:
    log(STATE, "Enter choose_exit\n");
    {
        // Show active value
        set_display_buffer_string("EXIT", 0);
        clear_display_buffer(1);
        write_to_all_displays();

        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                case B_R:
                    break;
                case B_L:
                    goto choose_year;
                case B_SP:
                    goto menu_time;
                default:
                    log(WARNING, "choose_exit unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }


set_second:
    {
        log(STATE, "Set second\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(SECOND);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(SECOND);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(SECOND);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(SECOND);
                    set_alarm_next(SECOND);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(SECOND);
                    set_alarm_next(SECOND);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_second;
                default:
                    log(WARNING, "set_second unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_minute:
    {
        log(STATE, "Set minute\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(MINUTE);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(MINUTE);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(MINUTE);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(MINUTE);
                    set_alarm_next(MINUTE);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(MINUTE);
                    set_alarm_next(MINUTE);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_minute;
                default:
                    log(WARNING, "set_minute unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_hour:
    {
        log(STATE, "Set hour\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(HOUR);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(HOUR);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(HOUR);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_hour;
                default:
                    log(WARNING, "set_hour unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_dotw:
    {
        log(STATE, "Set dotw\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(DOTW);
        char dotw[5] = "     ";
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(DOTW);
                    add_dotw_to_string(dotw, value, 1);
                    set_display_buffer_string(dotw, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(DOTW);
                    set_alarm_next(HOUR);
                    add_dotw_to_string(dotw, value, 1);
                    set_display_buffer_string(dotw, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(DOTW);
                    set_alarm_next(HOUR);
                    add_dotw_to_string(dotw, value, 1);
                    set_display_buffer_string(dotw, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_dotw;
                default:
                    log(WARNING, "set_dotw unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_day:
    {
        log(STATE, "Set day\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(DAY);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(DAY);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(DAY);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(DAY);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_day;
                default:
                    log(WARNING, "set_day unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_month:
    {
        log(STATE, "Set minute\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(MONTH);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(MONTH);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(MONTH);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(MONTH);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_month;
                default:
                    log(WARNING, "set_month unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

set_year:
    {
        log(STATE, "Set year\n");
        set_display(ON, BLINK_FREQ, 1);  // Set blinkmode on display 1
        uint8_t value = RTC_get(YEAR);
        // Act on interrupts
        while (true) {
            uint8_t interrupts = extract_external_interrupts();
            switch (interrupts) {
                case 0:
                    break;
                case I_RTC:
                    set_alarm_next(HOUR);
                    log(INFO, "Reset due to RTC update\n");
                    value = RTC_get(YEAR);
                    set_display_buffer_number(value, 1);
                    write_to_display(1);
                    break;
                case B_R:
                    value = increment_time_value(YEAR);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value + 2000, 1);
                    write_to_display(1);
                    break;
                case B_L:
                    value = decrement_time_value(YEAR);
                    set_alarm_next(HOUR);
                    set_display_buffer_number(value + 2000, 1);
                    write_to_display(1);
                    break;
                case B_SP:
                    set_display(ON, 0, 1);  // disable blinkmode on display 1
                    goto choose_year;
                default:
                    log(WARNING, "set_year unknown interrupt: 0x%x\n", interrupts);
            }
        }
    }

}

