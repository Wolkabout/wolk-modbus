#include "modbus_variable.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define CRITICAL_BLOCK_ENTER
#define CRITICAL_BLOCK_EXIT

/* 16-bit */
void modbus_variable_init(modbus_variable_t* modbus_variable, volatile uint16_t* address)
{
    modbus_variable->address = address;
}

uint16_t modbus_variable_get_value(modbus_variable_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    uint16_t value = *modbus_variable->address;

    CRITICAL_BLOCK_EXIT
    return value;
}

void modbus_variable_set_value(modbus_variable_t* modbus_variable, uint16_t value)
{
    CRITICAL_BLOCK_ENTER
    *(modbus_variable->address) = value;

    CRITICAL_BLOCK_EXIT
}

void modbus_variable_increment_value(modbus_variable_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    uint16_t value = *modbus_variable->address;
    ++value;
    *(modbus_variable->address) = value;

    CRITICAL_BLOCK_EXIT
}

void modbus_variable_decrement_value(modbus_variable_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    uint16_t value = *modbus_variable->address;
    --value;
    *(modbus_variable->address) = value;

    CRITICAL_BLOCK_EXIT
}

/* 32-bit */
/*
typedef union
{
    uint16_t value_16[2];
    uint32_t value_32;
} variable_converter_t;

void modbus_variable_32_init(modbus_variable_32_t* modbus_variable, volatile
uint16_t* address_low, volatile uint16_t* address_high)
{
    modbus_variable->address_low = address_low;
    modbus_variable->address_high = address_high;
}

uint32_t modbus_variable_32_get_value(modbus_variable_32_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    variable_converter_t var;
    var.value_16[0] = *modbus_variable->address_low;
    var.value_16[1] = *modbus_variable->address_high;

    CRITICAL_BLOCK_EXIT
    return var.value_32;
}

void modbus_variable_32_set_value(modbus_variable_32_t* modbus_variable,
uint32_t value)
{
    CRITICAL_BLOCK_ENTER

    variable_converter_t var;
    var.value_32 = value;

    memcpy(modbus_variable->address_low, &var.value_16[0], sizeof(uint16_t));
    memcpy(modbus_variable->address_high, &var.value_16[1], sizeof(uint16_t));

    CRITICAL_BLOCK_EXIT
}

void modbus_variable_32_increment_value(modbus_variable_32_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    variable_converter_t var;
    var.value_32 = modbus_variable_32_get_value(modbus_variable);
    ++var.value_32;

    modbus_variable_32_set_value(modbus_variable, var.value_32);

    CRITICAL_BLOCK_EXIT
}

void modbus_variable_32_decrement_value(modbus_variable_32_t* modbus_variable)
{
    CRITICAL_BLOCK_ENTER

    variable_converter_t var;
    var.value_32 = modbus_variable_32_get_value(modbus_variable);
    --var.value_32;

    modbus_variable_32_set_value(modbus_variable, var.value_32);

    CRITICAL_BLOCK_EXIT
}
*/
