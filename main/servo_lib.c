#include "servo_lib.h"

static int ServoState;
int Servogpio = GPIO_NUM_23;
void setServogpio(int gpio){
    Servogpio = gpio;
}

void servo_set_state(int gpio, int state) {
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, gpio);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;  // Tần số PWM cho servo (50 Hz)
    pwm_config.cmpr_a = 0;      // Duty cycle ban đầu
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    float duty_cycle;

    if (state == 0) {
        duty_cycle = 2.5;  // 0 độ (duty cycle = 2.5%)
        ServoState = state;
    } else if (state == 1) {
        duty_cycle = 7.5;  // 90 độ (duty cycle = 7.5%)
        ServoState = state;
    } else {
        return;  // Trạng thái không hợp lệ
    }
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

int getStateSer(){
    return ServoState;
}
