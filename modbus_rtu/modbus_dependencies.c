#include "modbus_dependencies.h"

#include <stdbool.h>
#include <stdint.h>

extern void modbus_timer_tick_handler(void);
extern void modbus_byte_received_handler(void);

void rx_cb_modbus_dependencies_usart(void)
{
    led_rx(true);
    modbus_byte_received_handler();
    led_rx(false);
}

volatile bool is_byte_sent = false;
void tx_cb_modbus_dependencies_usart(void)
{
    is_byte_sent = true;
}

static inline void usart_sync_put_byte(uint8_t byte)
{
    is_byte_sent = false;
    write_byte(&byte);
    while (!is_byte_sent)
        ;
}

void modbus_dependencies_usart_open(void)
{
    modbus_dependencies_usart_close();
    usart_enable();
}

void modbus_dependencies_usart_close(void)
{
    usart_disable();
}

void modbus_dependencies_usart_put_byte(uint8_t byte)
{
    led_tx(true);
    usart_sync_put_byte(byte);
    led_tx(false);
}

uint8_t modbus_dependencies_usart_get_byte(void)
{
    uint8_t read_buffer = 0;

    read_byte(&read_buffer);

    return read_buffer;
}

bool modbus_dependencies_usart_busy(void)
{
    return false;
}

void modbus_dependencies_set_direction_pin(bool send)
{
    gpio_set_re_pin_level(send);
    gpio_set_de_pin_level(send);
}

void modbus_dependencies_timer_callback(void)
{
    modbus_timer_tick_handler();
}

void modbus_dependencies_timer_start(void)
{
    timer_start();
}

void modbus_dependencies_timer_stop(void)
{
    timer_stop();
}

void modbus_dependencies_init(void)
{
    timer_init();
}
