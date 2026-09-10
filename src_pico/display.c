#include "display.h"

#define LCD_COLS 16

static void display_print_line(uint8_t row, const char *text) {
    char buffer[LCD_COLS + 1];

    snprintf(buffer, sizeof(buffer), "%-16s", text);

    lcd_set_cursor(row, 0);
    lcd_print(buffer);
}

void display_init(void) {
    lcd_init();
    lcd_clear();

    display_print_line(0, "PC Vitals");
    display_print_line(1, "Starting...");
    sleep_ms(1000);
}

void display_show_stats(const PcStats *stats) {
    char line0[LCD_COLS + 1];
    char line1[LCD_COLS + 1];

    // Format the string and dump it into the line buffers
    snprintf(line0, sizeof(line0), "CPU: %.1fC GPU: %.1fC", stats->cpuTemp, stats->gpuTemp);
    snprintf(line1, sizeof(line1), "CPU: %.0f%% GPU: %.0f%%", stats->cpuLoad, stats->gpuLoad);

    display_print_line(0, line0);
    display_print_line(1, line1);
}