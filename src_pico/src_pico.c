#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"
#include "stats.h"
#include "display.h"
#include "serial_rx.h"

#define DISPLAY_REFRESH_MS 250
#define RX_BUFFER_SIZE 17


static void print_lcd_line(uint8_t row, const char *text) {
    char line[17];

    snprintf(line, sizeof(line), "%-16s", text);

    lcd_set_cursor(row, 0);
    lcd_print(line);
}

int main() {
    stdio_init_all();
    display_init();

    PcStats stats = {
        .cpu_temp = 0.0f,
        .cpu_load = 0.0f,
        .cpu_clk = 0.0f,

        .gpu_temp = 0.0f,
        .gpu_load = 0.0f,
        .gpu_vram = 0.0f,

        .ram_used = 0.0f
    };

    while (true) {
        if (serial_rx_read_stats(&stats)) {
            printf("Stats updated\n");
        }

        display_show_stats(&stats);

        sleep_ms(DISPLAY_REFRESH_MS);
    }
}