#include "lcd.h"

// Wiring
#define LCD_RS 9
#define LCD_E  8

#define LCD_D4 10
#define LCD_D5 11
#define LCD_D6 12
#define LCD_D7 13

// HD44780 commands found in the datasheet
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME   0x02
#define LCD_ENTRY_MODE    0x06
#define LCD_DISPLAY_ON    0x0C
#define LCD_FUNCTION_SET  0x28
#define LCD_SET_DDRAM     0x80

// Using the 4 bit mode:
    // Every byte (8bits) is split into two parts: eg. 0x41 = 0100 0001, send 0100 first, send 0001 second
    // LCD only recives data when we pulse E
    
// Private Methods
static void lcd_pulse_enable() {
    gpio_put(LCD_E, 1);
    sleep_us(1);

    gpio_put(LCD_E, 0);
    sleep_us(100);
}

static void lcd_write_nibble(uint8_t nibble) {
    gpio_put(LCD_D4, (nibble >> 0) & 1);
    gpio_put(LCD_D5, (nibble >> 1) & 1);
    gpio_put(LCD_D6, (nibble >> 2) & 1);
    gpio_put(LCD_D7, (nibble >> 3) & 1);

    lcd_pulse_enable();
}

static void lcd_send_byte(uint8_t value, bool is_data) {
    gpio_put(LCD_RS, is_data ? 1 : 0);

    // Send upper 4 bits first
    lcd_write_nibble(value >> 4);

    // Then send lower 4 bits
    lcd_write_nibble(value & 0x0F);

    sleep_us(50);
}

static void lcd_command(uint8_t command) {
    lcd_send_byte(command, false);
}

static void lcd_data(uint8_t data) {
    lcd_send_byte(data, true);
}

/*
    Special HD44780 4-bit initialization sequence.

    We send 0x03 three times using only the upper data lines.
    Then we send 0x02 to switch into 4-bit mode. RTFM p17.
*/
static void set_to_4_bit_mode() {
    lcd_write_nibble(0x03);
    sleep_ms(5);

    lcd_write_nibble(0x03);
    sleep_us(150);

    lcd_write_nibble(0x03);
    sleep_us(150);

    lcd_write_nibble(0x02);
    sleep_us(150);
}

// Public API Methods
void lcd_init(void) {
    // Init all GPIO pins:
    gpio_init(LCD_RS);
    gpio_init(LCD_E);
    gpio_init(LCD_D4);
    gpio_init(LCD_D5);
    gpio_init(LCD_D6);
    gpio_init(LCD_D7);

    gpio_set_dir(LCD_RS, GPIO_OUT);
    gpio_set_dir(LCD_E, GPIO_OUT);
    gpio_set_dir(LCD_D4, GPIO_OUT);
    gpio_set_dir(LCD_D5, GPIO_OUT);
    gpio_set_dir(LCD_D6, GPIO_OUT);
    gpio_set_dir(LCD_D7, GPIO_OUT);

    // Enable=false + Instruction mode
    gpio_put(LCD_RS, 0);
    gpio_put(LCD_E, 0);

    // Wait for display to power up:
    sleep_us(50);

    // Rest of the setup:
    set_to_4_bit_mode();
    
    // 4-bit mode, 2 lines, 5x8 font
    lcd_command(LCD_FUNCTION_SET);

    // Display on, cursor off, blink off
    lcd_command(LCD_DISPLAY_ON);

    // Clear display
    lcd_clear();

    // Text moves left-to-right
    lcd_command(LCD_ENTRY_MODE);

}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR_DISPLAY);
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    // RTFM p8 => The rows are DRAM addresses, you need to set them accordingly 
    uint8_t row_offset[] = {
        0x00,
        0x40
    };

    if (row > 1)
        row = 1;

    if (col > 15)
        col = 15;

    lcd_command(LCD_SET_DDRAM | (row_offset[row] + col));
}

void lcd_print(const char *text) {
    // Loop till we get a '\0'
    while (*text) {
        lcd_data((uint8_t)*text);
        text++;
    }
}
