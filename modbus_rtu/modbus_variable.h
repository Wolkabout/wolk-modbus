#ifndef MODBUS_VARIABLE_H
#define MODBUS_VARIABLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile uint16_t* address;
    //    uint16_t* address;
} modbus_variable_t;

typedef struct {
    volatile uint16_t* address_low;
    volatile uint16_t* address_high;
} modbus_variable_32_t;

/* 16-bit */
void modbus_variable_init(modbus_variable_t* modbus_variable, volatile uint16_t* address);

uint16_t modbus_variable_get_value(modbus_variable_t* modbus_variable);
void modbus_variable_set_value(modbus_variable_t* modbus_variable, uint16_t value);

void modbus_variable_increment_value(modbus_variable_t* modbus_variable);
void modbus_variable_decrement_value(modbus_variable_t* modbus_variable);

/* 32-bit */
/*
void modbus_variable_32_init(modbus_variable_32_t* modbus_variable, volatile
uint16_t* address_low, volatile uint16_t* address_high);

uint32_t modbus_variable_32_get_value(modbus_variable_32_t* modbus_variable);
void modbus_variable_32_set_value(modbus_variable_32_t* modbus_variable,
uint32_t value);

void modbus_variable_32_increment_value(modbus_variable_32_t* modbus_variable);
void modbus_variable_32_decrement_value(modbus_variable_32_t* modbus_variable);
*/

#ifdef __cplusplus
}
#endif
#endif
