#ifndef MODBUS_DEPENDENCIES_H
#define MODBUS_DEPENDENCIES_H

#include <stdbool.h>
#include <stdint.h>

#include "modbus.h"

void modbus_dependencies_init(void);

void modbus_dependencies_usart_open(void);
void modbus_dependencies_usart_close(void);

void modbus_dependencies_usart_put_byte(uint8_t byte);
uint8_t modbus_dependencies_usart_get_byte(void);
bool modbus_dependencies_usart_busy(void);

void rx_cb_modbus_dependencies_usart(void);
void tx_cb_modbus_dependencies_usart(void);

void modbus_dependencies_set_direction_pin(bool send);

void modbus_dependencies_timer_start(void);
void modbus_dependencies_timer_stop(void);

void modbus_dependencies_timer_callback(void);
void modbus_dependencies_sleep_tick_handler(void);

#endif
