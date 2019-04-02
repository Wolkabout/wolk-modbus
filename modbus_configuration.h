/*
 * configuration.h
 *
 *  Created on: Mar 27, 2019
 *      Author: WolkAbout Technology s.r.o.
 */

#ifndef MODBUS_CONFIGURATION_H_
#define MODBUS_CONFIGURATION_H_

#include "modbus_variable.h"


/**
 * \defgroup Insert how many Modbus registers you want to use in your project.
 * 			 Supported types are HOLDING_REGISTER, INPUT_REGISTER, COILS and INPUT_BITS.
 * 			 If you don't want to use any of registers types insert 0
 */
/*@{*/

/** Number of Holding Registers */
#define HOLDING_REGISTER 0
/** Number of Input Register */
#define INPUT_REGISTER 0
/** Number of Coils */
#define COILS 0
/** Number of Input Bits */
#define INPUT_BITS 0

/**
 * @brief Enumerate defined holding registers. It will be used in registers table to discriminate holding registers.
 *
 * @note If you don't use holding registers comment this typedef enum. Most of compilers will raise error for empty
enum.
 * Example of usage
~~~~~{.c}
typedef enum {
    HOLDING_REGISTER_1
} holding_register_mapping_t;

~~~~~
 */
typedef enum {
    /* Enumerate own holding registers */
} holding_register_mapping_t;

/**
 * @brief Enumerate defined input registers. It will be used in registers table to discriminate input registers.
 */
typedef enum {
    /* Enumerate own input registers */
} input_register_mapping_t;

/**
 * @brief Enumerate defined coils. It will be used in registers table to discriminate coils.
 */
typedef enum {
    /* Enumerate own coils */
} coil_mapping_t;

/**
 * @brief define Modbus variable address stack. Type is modbus_variable_t.
 * Example of usage
~~~~~{.c}
 typedef struct {
    modbus_variable_t holding_register_1;
 } modbus_variables_map_t;
~~~~~
 */
typedef struct {
    /* List your modbus variables using type modbus_variable_t */
} modbus_variables_map_t;

extern modbus_variables_map_t modbus_variables_map;

/**
 * @brief User defined registers. Here user need to map address stack define in modbus_variables_map_t struct to
 * Registers table.
 *
 * Registers table consist following types:
 * 	- HoldingRegister[HOLDING_REGISTER]
 * 	- InputRegister[INPUT_REGISTER]
 *	- Coils[COILS]
 *	- InputBits[INPUT_BITS]
 *
 * Example of usage
~~~~~{.c}
void modbus_register_mapping_init(void)
{
    modbus_variable_init(&modbus_variables_map.holding_register_1, &HoldingRegister[HOLDING_REGISTER_1]);
}
~~~~~
 *
 * @see modbus_variable_init() for details.
 * @see Register table in modbus.c for details.
 *
 * @param 	None
 *
 * @return 	None
 */
void modbus_register_mapping_init(void);

/*@}*/

#endif /* MODBUS_CONFIGURATION_H_ */
