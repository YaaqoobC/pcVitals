#pragma once

#include <stdint.h>
#include "pico/stdlib.h"

void lcd_init(void);

void lcd_clear(void);

void lcd_set_cursor(uint8_t row, uint8_t col);

void lcd_print(const char *text);