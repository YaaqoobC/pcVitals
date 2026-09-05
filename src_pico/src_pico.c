#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    // Initialize the LED pin
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize all standard I/O (including USB serial)
    stdio_init_all();

    while (true) {
        // Turn the LED on and print a message
        gpio_put(LED_PIN, 1);
        printf("Pico is ALIVE and broadcasting!\n");
        sleep_ms(500);

        // Turn the LED off
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
