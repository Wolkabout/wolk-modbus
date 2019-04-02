/*
 * driver_init.c

 *
 *  Created on: Mar 27, 2019
 *      Author: WolkAbout Technology s.r.o.
 */
#include "driver_init.h"


void rs485_interface_init(void)
{
}

void usart_enable(void)
{
}

void usart_disable(void)
{
}

int32_t write_byte(const uint8_t* const buf)
{
    return true;
}

int32_t read_byte(uint8_t* const buf)
{

    return true;
}


void gpio_set_re_pin_level(bool state)
{
}

void gpio_set_de_pin_level(bool state)
{
}

void led_tx(bool state)
{
}
void led_rx(bool state)
{
}


uint8_t timer_init(void)
{
    return true;
}

int8_t timer_start()
{
    return true;
}

uint8_t timer_stop()
{
    return true;
}
