

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <decthandler.h>


LOG_MODULE_REGISTER(application);

#define LED0_NODE DT_ALIAS(led0)
#define STACKSIZE 1024

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


int task_blinker(void)
{
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

    while(!DECT.is_ready()){
        k_msleep(100);
    }
    nrf_modem_dect_phy_pdc_event TEMP;
    while (1) {
        k_msgq_get(DECT.get_rx_avalible(), &TEMP, K_FOREVER);


        gpio_pin_toggle_dt(&led);
        k_msleep(500);
    }
}



K_THREAD_DEFINE(blinker, STACKSIZE, task_blinker, NULL, NULL, NULL, 7, 0,0);
