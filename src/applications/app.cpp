

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(application);

#define LED0_NODE DT_ALIAS(led0)
#define STACKSIZE 1024

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


int task_blinker(void)
{
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(100);
    }
}



K_THREAD_DEFINE(blinker, STACKSIZE, task_blinker, NULL, NULL, NULL, 7, 0,0);
