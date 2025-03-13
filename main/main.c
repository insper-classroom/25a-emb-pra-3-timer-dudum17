/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

int trigger = 15;
int echo = 14;

volatile int tempo_inicial;
volatile int tempo_final;
volatile int diferenca;
volatile int pos;
static volatile bool fired = false;

static void alarm_callback(void) {
    fired = true;
}

void timer_callback(uint gpio, uint32_t events){
     if (events == 0x4){
        tempo_final =  to_us_since_boot(get_absolute_time());
        diferenca = tempo_final - tempo_inicial;
        pos = diferenca * 0.017015;
        if(pos > 300){
            printf("erro \n");
        }
     }else{
        tempo_inicial = to_us_since_boot(get_absolute_time());
     }
}

int main() {
    stdio_init_all();
    printf("RTC Alarm Repeat!\n");

    gpio_init(echo);
    gpio_set_dir(echo, GPIO_IN);

    gpio_init(trigger);
    gpio_set_dir(trigger, GPIO_OUT);


    gpio_set_irq_enabled_with_callback(
        echo, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &timer_callback);

        datetime_t t = {
            .year  = 2025,
            .month = 03,
            .day   = 12,
            .dotw  = 3, // 0 is Sunday, so 3 is Wednesday
            .hour  = 11,
            .min   = 20,
            .sec   = 00
        };

        rtc_init();
        rtc_set_datetime(&t);

        datetime_t alarm = {
            .year  = -1,
            .month = -1,
            .day   = -1,
            .dotw  = -1,
            .hour  = -1,
            .min   = -1,
            .sec   = 00
        };

        rtc_set_alarm(&alarm, &alarm_callback);

    while (true) {
        gpio_put(trigger, 1);
        sleep_us(10);
        gpio_put(trigger, 0);
        datetime_t t = {0};
        rtc_get_datetime(&t);
        printf("%02d:%02d:%02d - %d cm\n", t.hour, t.min, t.sec, pos);
        if (fired) {
            fired = 0;    
        }

    }
}
