/**
 * @file drive_init.h
 *
 * driver_init.h
 *
 *  Created on: Mar 27, 2019
 *      Author: WolkAbout Technology s.r.o.
 *
 */

#ifndef DRIVER_INIT_H_
#define DRIVER_INIT_H_

#include <stdbool.h>
#include <stdio.h>


/**
 * \defgroup Firmware Version defines
 */
/*@{*/

/** major version position */
#define FIRMWARE_VERSION_MAJOR 0
/** minor version position */
#define FIRMWARE_VERSION_MINOR 1
/** patch version position */
#define FIRMWARE_VERSION_PATCH 2

/*@}*/

/**
 * @brief Init interfaces for RS-485 communication: USART.
 *
 * @param None
 *
 * @return None
 */
void rs485_interface_init();

/**
 * @brief Write One byte on selected USART interface
 *
 * @param buf One byte of data
 *
 * @return Error Code
 */
int32_t write_byte(const uint8_t* const buf);

/**
 * @brief Read One byte from selected USART interface
 *
 * @note This function is called when IRQ occurs. USART receive callback called
 * it until received whole message, @see rx_cb_modbus_dependencies_usart()
 * implementation.
 *
 * @param buf One byte of data
 *
 * @return Error Code
 */
int32_t read_byte(uint8_t* const buf);

/**
 * @brief Enable USART interface
 *
 * @return None
 */
void usart_enable(void);

/**
 * @brief Disable USART interface
 *
 * @return None
 */
void usart_disable(void);


/**
 * @brief GPIO interface for controlling RE pin of RS-485 driver
 *
 * @param state Can be true or false.
 *
 * @return None
 */
void gpio_set_re_pin_level(bool state);

/**
 * @brief GPIO interface for controlling DE pin of RS-485 driver
 *
 * @param state Can be true or false.
 *
 * @return None
 */
void gpio_set_de_pin_level(bool state);

/**
 * @brief Signal LED used for presenting USART occupancy while sending bytes
 *
 * @param state Can be true or false.
 *
 * @return None
 */
void led_tx(bool state);

/**
 * @brief Signal LED used for presenting USART occupancy while receiving bytes
 *
 * @param state Can be true or false.
 *
 * @return None
 */
void led_rx(bool state);


/**
 * @brief Timer setup. This timer is used by ModBus library to timeout incoming
 * traffic.
 *
 * @note  That timer initially needs to be stopped.
 *
 * @return Error Code
 */
uint8_t timer_init(void);

/**
 * @brief Start selected timer
 *
 * @return Error Code
 */
int8_t timer_start();

/**
 * @brief Stop selected timer
 *
 * @return Error Code
 */
uint8_t timer_stop();


#endif /* DRIVER_INIT_H_ */
