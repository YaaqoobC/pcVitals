#include "serial_rx.h"

// Read USB Serial
// Collect one full line
// Parse the line

// Using a text protocol:
    // cpu_temp,cpu_load,cpu_clk,gpu_temp,gpu_load,gpu_vram,ram_used\n

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "stats.h"

#define RX_BUFFER_SIZE 96
#define INACTIVE_TIMEOUT_MS (5 * 1000)

static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;
static uint32_t last_stats_update_ms = 0;

static void update_inactive_status(PcStats *stats) {
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (last_stats_update_ms == 0) {
        stats->inactive = true;
        return;
    }

    stats->inactive = (now - last_stats_update_ms) >= INACTIVE_TIMEOUT_MS;
}

static bool parse_stats_line(const char *line, PcStats *stats) {
    PcStats temp_stats = {0};

    char start_marker = '\0';

    int parsed = sscanf(line,
                        "%c,%f,%f,%f,%f,%f,%f,%f",
                        &start_marker,
                        &temp_stats.cpu_temp,
                        &temp_stats.cpu_load,
                        &temp_stats.cpu_clk,
                        &temp_stats.gpu_temp,
                        &temp_stats.gpu_load,
                        &temp_stats.gpu_vram,
                        &temp_stats.ram_used);

    if (parsed != 8) {
        return false;
    }

    // 'S' Signifies the start of a packet
    if (start_marker != 'S') {
        return false;
    }

    *stats = temp_stats;
    return true;

}

bool serial_rx_read_stats(PcStats *stats) {
    int character = getchar_timeout_us(0);

    if (character == PICO_ERROR_TIMEOUT) {
        update_inactive_status(stats);
        return false;
    }

    while (character != PICO_ERROR_TIMEOUT) { 
        if (character == '\r' || character == '\n') {
            // Check if the buffer index moved ie, has it been filled at all?
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0';

                bool success = parse_stats_line(rx_buffer, stats);

                rx_index = 0;
                memset(rx_buffer, 0, sizeof(rx_buffer));

                if (success) {
                    last_stats_update_ms = to_ms_since_boot(get_absolute_time());
                    stats->inactive = false;
                } else {
                    update_inactive_status(stats);
                }

                return success;
            }
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index] = (char)character;
                rx_index++;
            } else {
                // Buffer overflow protection. If the line is too long, reset and wait for a new line.
                rx_index = 0;
                memset(rx_buffer, 0, sizeof(rx_buffer));
            }
        }

        character = getchar_timeout_us(0);
    }
    
    update_inactive_status(stats);
    return false;
}
