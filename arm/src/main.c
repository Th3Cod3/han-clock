#include <MKL25Z4.h>
#include <time.h>
#include "clock_functions.h"
#include "uart0.h"
#include "lcd_4bit.h"
#include "string.h"
#include "stepper.h"
#include "hall.h"
#include "pit.h"

int main(void)
{
    uart0_init();
    lcd_init();
    pit_init();
    stepper_init();
    hall_init();

    lcd_clear();
    lcd_print("Connecting to");
    lcd_set_cursor(0, 1);
    lcd_print("UART0");

    uint32_t wait_to_update = millis + 500;
    screen_mode = LCD_CONNECTING_UART0;
    uint32_t timeout = millis + 3000;
    uint8_t dots = 0;
    uint8_t times = 0;
    do
    {
        if (wait_millis(wait_to_update))
        {
            dots % 2 ?: uart0_send_string("ECHO\n");
            if (!dots)
            {
                lcd_set_cursor(5, 1);
                lcd_print("      ");
            }
            else
            {
                lcd_set_cursor(4 + dots, 1);
                lcd_print(".");
            }

            wait_to_update = millis + 500;
            dots++;
            dots %= 6;
        }

        if (uart0_num_rx_chars_available() > 0)
        {
            char str[MAX_RECEIVE_CHARS];
            uart0_receive_string(&str);

            if (!strcmp(&str, "ACK"))
            {
                lcd_clear();
                lcd_print("Connected.");
                break;
            }
        }

        if (wait_millis(timeout))
        {
            lcd_clear();
            lcd_print("Timeout.");
            screen_mode = LCD_DATETIME_MODE;
            break;
        }
    } while (1);

    char str[MAX_RECEIVE_CHARS];

    uint8_t prev_screen_mode = screen_mode;
    while (1)
    {
        prev_screen_mode = screen_mode;
        if (uart0_num_rx_chars_available())
        {
            uart0_receive_string(&str);

            if (!str[0])
            {
                // Do nothing
            }
            else if (!strcmp(&str, "calibrate"))
            {
                steppers_mode = STEPPER_CALIBRATION_MODE;
            }
            else if (!strcmp(&str, "sync"))
            {
                steppers_mode = STEPPER_SYNC_MODE;
            }
            else if (!strcmp(&str, "datetime"))
            {
                screen_mode = LCD_DATETIME_MODE;
            }
            else if (!strcmp(&str, "update-clock"))
            {
                timestamp = 1718665080;
            }
            else if (!strcmp(&str, "yefri"))
            {
                screen_mode = LCD_YEFRI_MODE;
            }
            else if (!strcmp(&str, "ruben"))
            {
                screen_mode = LCD_RUBEN_MODE;
            }
            else if (!strcmp(&str, "chris"))
            {
                screen_mode = LCD_CHRIS_MODE;
            }
        }

        // update screen on change
        if (prev_screen_mode != screen_mode)
        {
            switch (screen_mode)
            {
            case LCD_UNKNOWN_MODE:
                lcd_clear();
                lcd_print("Unknown mode");
                break;

            case LCD_YEFRI_MODE:
                lcd_clear();
                lcd_print("Yefri Gonzalez");
                lcd_set_cursor(0, 1);
                lcd_print("30/12/1994");
                break;

            case LCD_RUBEN_MODE:
                lcd_clear();
                lcd_print("Ruben Groenendijk");
                lcd_set_cursor(0, 1);
                lcd_print("30/12/1994");
                break;

            case LCD_CHRIS_MODE:
                lcd_clear();
                lcd_print("Chris Salimans");
                lcd_set_cursor(0, 1);
                lcd_print("30/12/1994");
                break;
            }
        }

        if (screen_mode == LCD_DATETIME_MODE && wait_millis(wait_to_update))
        {
            wait_to_update = millis + 500;
            struct tm *tm_info = localtime(&timestamp);

            char formatted_date[16];
            strftime(formatted_date, sizeof(formatted_date), "%Y-%m-%d", tm_info);
            lcd_clear();
            lcd_print("Date: ");
            lcd_print(formatted_date);

            lcd_set_cursor(0, 1);
            strftime(formatted_date, sizeof(formatted_date), "%H:%M:%S", tm_info);
            lcd_print("Time: ");
            lcd_print(formatted_date);
        }
    }
}