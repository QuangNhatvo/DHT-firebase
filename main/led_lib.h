// #ifndef LED_LIB_H
// #define LED_LIB_H

// #include "driver/gpio.h"

// // Define a struct to hold the LED configuration
// typedef struct {
//     gpio_num_t gpio_num;  // GPIO pin number
// } led_t;

// // Function prototypes
// void led_init(led_t *led, gpio_num_t gpio_num);
// void led_set(led_t *led, uint8_t state);

// #endif // LED_LIB_H

#ifndef LED_LIB_H
#define LED_LIB_H

#include "driver/gpio.h"
#include <stdbool.h>
#define LED_OK 0
#define LED_CHECKSUM_ERROR -1
#define LED_TIMEOUT_ERROR -2

void setLEDgpio(int gpio);
void led_set(int state);
int getState();
#endif // LED_LIB_H



