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


void timer_callback(uint gpio, uint32_t events){
     if (events == 0x4){
        tempo_final =  to_us_since_boot(get_absolute_time());
        diferenca = tempo_final - tempo_inicial;
        pos = diferenca * 0.017015;
     }else {
        tempo_inicial = to_us_since_boot(get_absolute_time());
     }
}

int main() {

    int medir = 0;
    stdio_init_all();
    printf("Digite 's' para iniciar e 'o' para parar as medições.\n");

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



       


    while (true) {
        int caracter = getchar_timeout_us(1000);

        if (caracter != PICO_ERROR_TIMEOUT){
            if(caracter == 's'){
                medir = 1;
                printf("Iniciando medições...\n");
            }else if (caracter == 'o') {
                medir = 0;
                printf("Parando medições...\n");
            }
        }

        if (medir == 1){


           gpio_put(trigger, 1);
           sleep_us(10);
           gpio_put(trigger, 0);
           rtc_get_datetime(&t);
           printf("%02d:%02d:%02d - %d cm\n", t.hour, t.min, t.sec, pos);
            sleep_ms(500);

         }
    }
}
