```
██╗    ██╗ ██████╗ ██╗     ██╗  ██╗     ███╗   ███╗ ██████╗ ██████╗ ██████╗ ██╗   ██╗███████╗
██║    ██║██╔═══██╗██║     ██║ ██╔╝     ████╗ ████║██╔═══██╗██╔══██╗██╔══██╗██║   ██║██╔════╝
██║ █╗ ██║██║   ██║██║     █████╔╝█████╗██╔████╔██║██║   ██║██║  ██║██████╔╝██║   ██║███████╗
██║███╗██║██║   ██║██║     ██╔═██╗╚════╝██║╚██╔╝██║██║   ██║██║  ██║██╔══██╗██║   ██║╚════██║
╚███╔███╔╝╚██████╔╝███████╗██║  ██╗     ██║ ╚═╝ ██║╚██████╔╝██████╔╝██████╔╝╚██████╔╝███████║
 ╚══╝╚══╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝     ╚═╝     ╚═╝ ╚═════╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚══════╝
```

This is Modbus RTU slave library written in C99. It is intended to be used on a bare-metal device.
Hardware communication interface needs to be implemented by the user. The usual communication interface is UART/USART. Modbus itself needs to be configured by setting the number and type of wanted registers.
Supported Modbus registers are:
 * Holding Register
 * Input Register
 * Coils
 * Input Bits


# Usage

`driver_init.h` lists and describes interfaces that user needs to provide for establishing communication. This file needs to be included in user space. Three hardware interfaces are needed from MCU:
 * timer - with counter(IRQ) on every 1ms
 * UART/USART - receiving bytes one by one. It is recommended to use both interrupts - for Tx and Rx.
 * Two signal diodes(optional) - used to visualize bytes received and bytes transmited.

`modbus_configuration.h` requires setup of Modbus interfaces. The following needs to be defined:
 * Number of registers - separatly for holding registers, input registers, coils and input bits. Eg: `#define HOLDING_REGISTER 	1`
 * A unique name for each position in each register type. If you define 3 holding registers then you will define 3 different names in enumeration. Eg: `HOLDING_REGISTER_1`
 * For each register type, define unique address in struct modbus_variables_map_t. Eg: `modbus_variable_t holding_register_1;`
 * Map all defined registers in modbus_register_mapping_init(). Eg: `modbus_variable_init(&modbus_variables_map.holding_register_1, &HoldingRegister[HOLDING_REGISTER_1]);`

#### User space
These API functions need to be called before starting the manipulation of Modbus registers:
```
//init all HW interfaces
rs485_interface_init();

//Init Modbus, set slave address
modbus_init(SLAVE_ADDRESS);

//Init your own reigsters
modbus_register_mapping_init();
```

To set the value of any modbus reigster use:
`modbus_variable_set_value()`

To read the current value of any register check the register's array by navigating to wanted position:
 * HoldingRegister[HOLDING_REGISTER]
 * InputRegister[INPUT_REGISTER]
 * Coils[COILS]
 * InputBits[INPUT_BITS]