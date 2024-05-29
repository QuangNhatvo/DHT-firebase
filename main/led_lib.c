// #include "led_lib.h"

// // Function to initialize the LED GPIO
// void led_init(led_t *led, gpio_num_t gpio_num) {
//     led->gpio_num = gpio_num;

//     // Configure the GPIO pin as an output
//     esp_rom_gpio_pad_select_gpio(gpio_num);
//     gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
// }

// // Function to set the LED state
// void led_set(led_t *led, uint8_t state) {
//   gpio_set_level(led->gpio_num, state);
// }

#include "led_lib.h"
#include "esp_log.h"
#include "driver/gpio.h"

int LEDgpio = GPIO_NUM_21;
static int LedState;

void setLEDgpio(int gpio){
    LEDgpio = gpio;
}
    
void led_set(int state){
    gpio_set_level(LEDgpio, state);
    gpio_set_direction(LEDgpio, GPIO_MODE_OUTPUT);
    LedState = state;
}
int getState(){
    return LedState;
}

    