#include <MKL25Z4.h>
#include "clock_functions.h"
#include "uart0.h"
#include "lcd_4bit.h"
#include "string.h"
#include "pit.h"

int main(void)
{
    uart0_init();
    lcd_init();
    pit_init();

    lcd_clear();
    lcd_print("Connecting to");
    lcd_set_cursor(0, 1);
    lcd_print("UART0");

    uint32_t wait_to_update = millis + 500;
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
                break;
            }
        }
    } while (1);

    lcd_clear();
    lcd_print("Connected.");

    char str[7];
    while (1)
    {
        
    }
}
