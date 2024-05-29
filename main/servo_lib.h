#ifndef SERVO_LIB_H
#define SERVO_LIB_H

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "driver/gpio.h"
#include <stdbool.h>

void setServogpio(int gpio);
// Hàm khởi tạo servo
// Hàm để điều khiển servo theo state
void servo_set_state(int gpio, int state);
int getStateSer();

#endif // SERVO_LIB_H
    