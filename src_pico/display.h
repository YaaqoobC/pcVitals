#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"

// The UI layer for the LCD display
void display_init(void);
void display_show_stats(const PcStats *stats);