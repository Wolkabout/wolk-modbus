#ifndef MODBUS_H
#define MODBUS_H

#include <stdbool.h>
#include <stdint.h>

#include "modbus_configuration.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CHECK_BIT(var, pos) (!!((var) & (1 << (pos))))
#define ARRAY_LENGTH(array) (sizeof(array) / (sizeof(array[0])))

extern volatile uint16_t HoldingRegister[HOLDING_REGISTER];
extern volatile uint16_t InputRegister[INPUT_REGISTER];
extern volatile uint16_t Coils[COILS];
extern volatile uint16_t InputBits[INPUT_BITS];

void modbus_init(uint8_t address);
void modbus_close(void);

void modbus_clean_all_registers(void);

bool modbus_process(void);

uint8_t modbus_get_slave_address(void);

void modbus_byte_received_handler(void);
void modbus_timer_tick_handler(void);

#ifdef __cplusplus
}
#endif
#endif
