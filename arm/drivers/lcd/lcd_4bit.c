/*! ***************************************************************************
 *
 * \brief     Low level driver for the 16x2 LCD
 * \file      lcd.c
 * \author    Hugo Arends
 * \date      June 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include "lcd_4bit.h"

// TODO: implement with interrupts
void delay_us(uint32_t d)
{

#if (CLOCK_SETUP != 1)
#warning This delay function does not work as designed
#endif

    volatile uint32_t t;

    for(t=4*d; t>0; t--)
    {
        __asm("nop");
        __asm("nop");
    }
}

/// \name Macros for setting pins
/// \{
#define SET_LCD_E(x)            \
    if (x)                      \
    {                           \
        PIN_E_PT->PSOR = PIN_E; \
    }                           \
    else                        \
    {                           \
        PIN_E_PT->PCOR = PIN_E; \
    }
#define SET_LCD_RS(x)             \
    if (x)                        \
    {                             \
        PIN_RS_PT->PSOR = PIN_RS; \
    }                             \
    else                          \
    {                             \
        PIN_RS_PT->PCOR = PIN_RS; \
    }
#define SET_LCD_RW(x)             \
    if (x)                        \
    {                             \
        PIN_RW_PT->PSOR = PIN_RW; \
    }                             \
    else                          \
    {                             \
        PIN_RW_PT->PCOR = PIN_RW; \
    }

/// \}

/*!
 * \brief Translate the logic values in x to pin values DB4-DB7
 *
 * Uses the lower nibble of the variable x only. The bits are mapped as follows
 * - x[7] : not used
 * - x[6] : not used
 * - x[5] : not used
 * - x[4] : not used
 * - x[3] : DB7 pin
 * - x[2] : DB6 pin
 * - x[1] : DB5 pin
 * - x[0] : DB4 pin
 *
 * \param[in]  x  Logic values for DB4 and DB7
 */
void lcd_set_data(const uint8_t x)
{
    if (x & 0x01)
        PIN_DB4_PT->PSOR = PIN_DB4;
    else
        PIN_DB4_PT->PCOR = PIN_DB4;

    if (x & 0x02)
        PIN_DB5_PT->PSOR = PIN_DB5;
    else
        PIN_DB5_PT->PCOR = PIN_DB5;

    if (x & 0x04)
        PIN_DB6_PT->PSOR = PIN_DB6;
    else
        PIN_DB6_PT->PCOR = PIN_DB6;

    if (x & 0x08)
        PIN_DB7_PT->PSOR = PIN_DB7;
    else
        PIN_DB7_PT->PCOR = PIN_DB7;
}

/*!
 * \brief Translate the pin values DB4-DB7 to logic values in x
 *
 * Uses the lower nibble of the variable x only. The bits are mapped as follows
 * - x[7] : not used
 * - x[6] : not used
 * - x[5] : not used
 * - x[4] : not used
 * - x[3] : DB7
 * - x[2] : DB6
 * - x[1] : DB5
 * - x[0] : DB4
 *
 * \return  Logic values on DB4 and DB7
 */
uint8_t lcd_get_data(void)
{
    uint8_t x = 0;

    if (PIN_DB4_PT->PDIR & PIN_DB4)
        x |= 0x01;

    if (PIN_DB5_PT->PDIR & PIN_DB5)
        x |= 0x02;

    if (PIN_DB6_PT->PDIR & PIN_DB6)
        x |= 0x04;

    if (PIN_DB7_PT->PDIR & PIN_DB7)
        x |= 0x08;

    return x;
}

/*!
 * \brief Blocking wait while the LCD is busy
 *
 * The functions keeps polling the busy flag in the LCD as long as the busy
 * flag is set.
 */
void lcd_wait_while_busy(void)
{
    // Make all databus pins input
    PIN_DB4_PT->PDDR = PIN_DB4_PT->PDDR & ~PIN_DB4;
    PIN_DB5_PT->PDDR = PIN_DB5_PT->PDDR & ~PIN_DB5;
    PIN_DB6_PT->PDDR = PIN_DB6_PT->PDDR & ~PIN_DB6;
    PIN_DB7_PT->PDDR = PIN_DB7_PT->PDDR & ~PIN_DB7;

    SET_LCD_E(0);
    SET_LCD_RS(0);
    SET_LCD_RW(1);

    uint8_t status;

    do
    {
        delay_us(1);
        SET_LCD_E(1);
        delay_us(1);

        status = lcd_get_data() << 4;

        SET_LCD_E(0);
        delay_us(1);
        SET_LCD_E(1);
        delay_us(1);

        status |= lcd_get_data();

        SET_LCD_E(0);
    } while ((status & 0x80) != 0);

    // Make all databus pins output
    PIN_DB4_PT->PDDR = PIN_DB4_PT->PDDR | PIN_DB4;
    PIN_DB5_PT->PDDR = PIN_DB5_PT->PDDR | PIN_DB5;
    PIN_DB6_PT->PDDR = PIN_DB6_PT->PDDR | PIN_DB6;
    PIN_DB7_PT->PDDR = PIN_DB7_PT->PDDR | PIN_DB7;
}

/*!
 * \brief Write cycle for 4 bits
 *
 * This function implements the write cycle for 4 bits by toggling the control
 * lines. Uses only the lowest nibble of c.
 *
 * \param[in]  c  4-bit data for the LCD
 */
void lcd_write_4bit(const uint8_t c)
{
    SET_LCD_RW(0);
    SET_LCD_E(1);
    lcd_set_data(c);
    delay_us(1);
    SET_LCD_E(0);
    delay_us(1);
}

/*!
 * \brief Write command to the LCD
 *
 * This function writes a command to the LCD by executing two 4-bit transfers
 *
 * \param[in]  c  8-bit command for the LCD
 */
void lcd_write_cmd(const uint8_t c)
{
    lcd_wait_while_busy();

    SET_LCD_RS(0);
    lcd_write_4bit(c >> 4);
    lcd_write_4bit(c);
}

/*!
 * \brief Write data to the LCD
 *
 * This function writes data to the LCD by executing two 4-bit transfers
 *
 * \param[in]  c  8-bit data for the LCD
 */
void lcd_write_data(const uint8_t c)
{
    lcd_wait_while_busy();

    SET_LCD_RS(1);
    lcd_write_4bit(c >> 4);
    lcd_write_4bit(c);
}

/*!
 * \brief Initializes the GPIO pins
 *
 * All pins for DB4-DB7, E, RW and RS are configured to GPIO output pins.
 */
void lcd_init_port(void)
{
    // Enable clocks for peripherals
    ENABLE_LCD_PORT_CLOCKS;

    // Setting all pins to output mode
    PIN_DB4_PT->PDDR = PIN_DB4_PT->PDDR | PIN_DB4;
    PIN_DB5_PT->PDDR = PIN_DB5_PT->PDDR | PIN_DB5;
    PIN_DB6_PT->PDDR = PIN_DB6_PT->PDDR | PIN_DB6;
    PIN_DB7_PT->PDDR = PIN_DB7_PT->PDDR | PIN_DB7;
    PIN_E_PT->PDDR = PIN_E_PT->PDDR | PIN_E;
    PIN_RS_PT->PDDR = PIN_RS_PT->PDDR | PIN_RS;
    PIN_RW_PT->PDDR = PIN_RW_PT->PDDR | PIN_RW;

    // Setting all pin mux to GPIO
    PIN_DB4_PORT->PCR[PIN_DB4_SHIFT] = PORT_PCR_MUX(1);
    PIN_DB5_PORT->PCR[PIN_DB5_SHIFT] = PORT_PCR_MUX(1);
    PIN_DB6_PORT->PCR[PIN_DB6_SHIFT] = PORT_PCR_MUX(1);
    PIN_DB7_PORT->PCR[PIN_DB7_SHIFT] = PORT_PCR_MUX(1);
    PIN_E_PORT->PCR[PIN_E_SHIFT] = PORT_PCR_MUX(1);
    PIN_RS_PORT->PCR[PIN_RS_SHIFT] = PORT_PCR_MUX(1);
    PIN_RW_PORT->PCR[PIN_RW_SHIFT] = PORT_PCR_MUX(1);
}

/*!
 * \brief Initialises the LCD
 *
 * This functions initializes the LCD on the shield. It initializes the hardware
 * pins and performs the startup sequence for the LCD.
 */
void lcd_init(void)
{
    lcd_init_port();

    delay_us(100);

    // Startup sequence
    SET_LCD_RS(0);
    lcd_write_4bit(0x3);
    delay_us(100);
    lcd_write_4bit(0x3);
    delay_us(10);
    lcd_write_4bit(0x3);
    lcd_write_4bit(0x2);
    lcd_write_cmd(0x28);
    lcd_write_cmd(0x0C);
    lcd_write_cmd(0x06);
    lcd_write_cmd(0x80);
}

/*!
 * \brief Clears the LCD
 *
 * Sends a command to the LCD to clear it and moves the cursur to (0,0).
 */
void lcd_clear(void)
{
    lcd_write_cmd(0x01);
    lcd_set_cursor(0, 0);
}

/*!
 * \brief Moves the cursor
 *
 * Moves the cursor to position (column, row)
 *
 * \param[in]  column  New column position of the cursor
 * \param[in]  row     New row position of the cursor
 */
void lcd_set_cursor(const uint8_t column, const uint8_t row)
{
    uint8_t address;

    address = (row * 0x40) + column;
    address |= 0x80;
    lcd_write_cmd(address);
}

/*!
 * \brief Write an ASCII character to the LCD
 *
 * Uses the lcd_write_data() function to write an ASCII character to the LCD.
 *
 * \param[in]  c  ASCII character to display
 */
void lcd_putchar(const char c)
{
    lcd_write_data(c);
}

/*!
 * \brief Writes a string of ASCII characters to the LCD
 *
 * Uses the lcd_putchar() function to write a '\0' terminated ASCII character
 * string to the LCD.
 *
 * \param[in]  string  String of ASCII characters to display
 */
void lcd_print(const char *string)
{
    while (*string)
    {
        lcd_putchar(*string++);
    }
}
