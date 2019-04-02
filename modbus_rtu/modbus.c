#include <stdbool.h>
#include <stdint.h>

#include "driver_init.h"
#include "modbus.h"
#include "modbus_dependencies.h"


enum { MAXIMUM_RECEIVE_PACKET_SIZE = 150, MAXIMUM_RESPONSE_PACKET_SIZE = 150 };

/* Registers tables */
volatile uint16_t HoldingRegister[HOLDING_REGISTER];
volatile uint16_t InputRegister[INPUT_REGISTER];
volatile uint16_t Coils[COILS];
volatile uint16_t InputBits[INPUT_BITS];

/* Modbus slave address*/
static uint8_t slave_address = 255;

/* Variables for handling sending/receiving of Modbus packets*/
static uint8_t modbus_response_bytes[MAXIMUM_RESPONSE_PACKET_SIZE];
static uint8_t modbus_received_bytes[MAXIMUM_RECEIVE_PACKET_SIZE];
static uint8_t modbus_message_length;

static uint8_t timer_counter, current_message_received_bytes = 0;
static bool modbus_message_received, end_of_message, new_message = true;

static bool command_received(void)
{
    return modbus_message_received;
}

static uint16_t generate_crc(uint8_t messageLength)
{
    uint16_t crc = 0xFFFF;
    uint16_t crcHigh = 0;
    uint16_t crcLow = 0;
    int i, j = 0;

    for (i = 0; i < messageLength - 2; ++i) {
        crc ^= modbus_response_bytes[i];
        for (j = 8; j != 0; j--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    /* Switch endianess */
    crcHigh = (crc & 0xFF) << 8;
    crcLow = (crc & 0xFF00) >> 8;
    crcHigh |= crcLow;
    crc = crcHigh;

    return crc;
}

static bool check_crc(void)
{
    uint16_t crc = 0xFFFF;
    uint16_t crcHigh = 0;
    uint16_t crcLow = 0;
    int i, j = 0;

    for (i = 0; i < modbus_message_length - 2; ++i) {
        crc ^= modbus_received_bytes[i];
        for (j = 8; j != 0; --j) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    /* Switch endianess */
    crcHigh = (crc & 0xFF);
    crcLow = (crc & 0xFF00) >> 8;

    if ((crcHigh == modbus_received_bytes[i]) && (crcLow == modbus_received_bytes[i + 1]))
        return true;
    else
        return false;
}

static void clear_response(void)
{
    for (uint16_t i = 0; i < MAXIMUM_RESPONSE_PACKET_SIZE; ++i) {
        modbus_response_bytes[i] = 0;
    }
}

static void read_register(void)
{
    uint16_t rr_Address = 0;
    uint16_t rr_numRegs = 0;
    uint16_t crc = 0;
    uint16_t i = 0;
    uint8_t j = 3;

    rr_Address = modbus_received_bytes[2];
    rr_Address <<= 8;
    rr_Address |= modbus_received_bytes[3];

    rr_numRegs = modbus_received_bytes[4];
    rr_numRegs <<= 8;
    rr_numRegs |= modbus_received_bytes[5];

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x03;
    modbus_response_bytes[2] = rr_numRegs * 2;

    for (i = rr_Address; i < (rr_Address + rr_numRegs); ++i) {
        if (i >= ARRAY_LENGTH(HoldingRegister))
            return;

        if (HoldingRegister[i] > 255) {
            modbus_response_bytes[j] = HoldingRegister[i] >> 8;
            j++;
            modbus_response_bytes[j] = HoldingRegister[i];
            j++;
        } else {
            modbus_response_bytes[j] = 0x00;
            j++;
            modbus_response_bytes[j] = HoldingRegister[i];
            j++;
        }
    }

    crc = generate_crc(j + 2);
    modbus_response_bytes[j++] = crc >> 8;
    modbus_response_bytes[j++] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < j; i++) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);

    j = 0;

    clear_response();
}

static void read_input_register(void)
{
    uint16_t rr_Address = 0;
    uint16_t rr_numRegs = 0;
    uint16_t crc = 0;
    uint16_t i = 0;
    uint8_t j = 3;

    rr_Address = modbus_received_bytes[2];
    rr_Address <<= 8;
    rr_Address |= modbus_received_bytes[3];

    rr_numRegs = modbus_received_bytes[4];
    rr_numRegs <<= 8;
    rr_numRegs |= modbus_received_bytes[5];

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x04;
    modbus_response_bytes[2] = rr_numRegs * 2;

    for (i = rr_Address; i < (rr_Address + rr_numRegs); ++i) {
        if (i >= ARRAY_LENGTH(InputRegister))
            return;

        if (InputRegister[i] > 255) {
            modbus_response_bytes[j] = InputRegister[i] >> 8;
            j++;
            modbus_response_bytes[j] = InputRegister[i];
            j++;
        } else {
            modbus_response_bytes[j] = 0x00;
            j++;
            modbus_response_bytes[j] = InputRegister[i];
            j++;
        }
    }
    crc = generate_crc(j + 2);
    modbus_response_bytes[j++] = crc >> 8;
    modbus_response_bytes[j++] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < j; i++) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);
    j = 0;

    clear_response();
}

static void write_register(void)
{
    uint16_t wr_AddressLow = 0;
    uint16_t wr_AddressHigh = 0;
    uint16_t wr_Address = 0;

    uint16_t wr_ValueToWrite = 0;
    uint16_t wr_ValueToWriteLow = 0;
    uint16_t wr_ValueToWriteHigh = 0;

    uint16_t crc = 0;
    uint16_t i = 0;

    wr_Address = modbus_received_bytes[2];
    wr_Address <<= 8;
    wr_Address |= modbus_received_bytes[3];

    wr_AddressLow = modbus_received_bytes[3];
    wr_AddressHigh = modbus_received_bytes[2];

    wr_ValueToWrite = modbus_received_bytes[4];
    wr_ValueToWrite <<= 8;
    wr_ValueToWrite |= modbus_received_bytes[5];

    wr_ValueToWriteLow = modbus_received_bytes[5];
    wr_ValueToWriteHigh = modbus_received_bytes[4];

    if (wr_Address >= ARRAY_LENGTH(HoldingRegister))
        return;

    HoldingRegister[wr_Address] = wr_ValueToWrite;

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x06;
    modbus_response_bytes[3] = wr_AddressLow;
    modbus_response_bytes[2] = wr_AddressHigh;
    modbus_response_bytes[4] = wr_ValueToWriteHigh;
    modbus_response_bytes[5] = wr_ValueToWriteLow;

    crc = generate_crc(8);
    modbus_response_bytes[6] = crc >> 8;
    modbus_response_bytes[7] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < 8; ++i) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);

    clear_response();
}

static void write_multiple_registers(void)
{
    uint16_t wmr_Address = 0;
    uint16_t wmr_AddressHigh = 0;
    uint16_t wmr_AddressLow = 0;
    uint16_t wmr_numRegs = 0;
    uint16_t wmr_numRegsHigh = 0;
    uint16_t wmr_numRegsLow = 0;
    uint16_t wmr_numBytes = 0;
    uint16_t wmr_numBytesTST = 0;
    uint16_t ValueToWrite = 0;

    uint16_t crc = 0;
    uint16_t i = 0;
    uint8_t j = 0;

    wmr_Address = modbus_received_bytes[2];
    wmr_Address <<= 8;
    wmr_Address |= modbus_received_bytes[3];
    wmr_AddressHigh = modbus_received_bytes[2];
    wmr_AddressLow = modbus_received_bytes[3];

    wmr_numRegs = modbus_received_bytes[4];
    wmr_numRegs <<= 8;
    wmr_numRegs |= modbus_received_bytes[5];
    wmr_numRegsHigh = modbus_received_bytes[4];
    wmr_numRegsLow = modbus_received_bytes[5];
    wmr_numBytes = modbus_received_bytes[6];

    j = 7;

    wmr_numBytesTST = wmr_numBytes / 2;

    for (i = 0; i < wmr_numBytesTST; ++i) {
        if ((wmr_Address + i) >= ARRAY_LENGTH(HoldingRegister))
            return;

        ValueToWrite = modbus_received_bytes[j];
        ValueToWrite <<= 8;
        j++;
        ValueToWrite |= modbus_received_bytes[j];
        j++;

        HoldingRegister[wmr_Address + i] = ValueToWrite;
    }

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x10;
    modbus_response_bytes[2] = wmr_AddressHigh;
    modbus_response_bytes[3] = wmr_AddressLow;
    modbus_response_bytes[4] = wmr_numRegsHigh;
    modbus_response_bytes[5] = wmr_numRegsLow;

    crc = generate_crc(8);

    modbus_response_bytes[6] = crc >> 8;
    modbus_response_bytes[7] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < 8; ++i) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);
    j = 0;

    clear_response();
}

static void read_coil(void)
{
    uint16_t rc_Address = 0;
    uint16_t rc_numCoils = 0;
    uint16_t crc = 0;

    uint8_t num_of_bytes = 0;
    uint8_t remainder = 0;
    uint8_t lsb = 0;
    uint8_t i, j, k, l = 0;

    rc_Address = modbus_received_bytes[2];
    rc_Address <<= 8;
    rc_Address |= modbus_received_bytes[3];

    rc_numCoils = modbus_received_bytes[4];
    rc_numCoils <<= 8;
    rc_numCoils |= modbus_received_bytes[5];

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x01;

    num_of_bytes = rc_numCoils / 8;
    remainder = rc_numCoils % 8;

    if (remainder) {
        num_of_bytes += 1;
    }
    modbus_response_bytes[2] = num_of_bytes;

    l = rc_Address;
    k = 3;

    for (i = num_of_bytes; i != 0; --i) {
        if (i > 1) {
            for (j = 0; j != 8; ++j) {
                if (l >= ARRAY_LENGTH(Coils))
                    return;

                if (Coils[l]) {
                    lsb = 1;
                } else {
                    lsb = 0;
                }
                modbus_response_bytes[k] ^= (lsb << j);
                l++;
            }
            k++;
        } else {
            for (j = 0; j != remainder; ++j) {
                if (l >= ARRAY_LENGTH(Coils))
                    return;

                if (Coils[l]) {
                    lsb = 1;
                } else {
                    lsb = 0;
                }
                modbus_response_bytes[k] ^= (lsb << j);
                l++;
            }
            k++;
        }
    }
    crc = generate_crc(k + 2);

    modbus_response_bytes[k++] = crc >> 8;
    modbus_response_bytes[k++] = crc;

    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(true);
    for (i = 0; i < k; i++) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);

    clear_response();
}

static void read_input_coil(void)
{
    uint16_t rc_Address = 0;
    uint16_t rc_numCoils = 0;
    uint16_t crc = 0;

    uint8_t num_of_bytes = 0;
    uint8_t remainder = 0;
    uint8_t lsb = 0;
    uint8_t i, j, k, l = 0;

    rc_Address = modbus_received_bytes[2];
    rc_Address <<= 8;
    rc_Address |= modbus_received_bytes[3];

    rc_numCoils = modbus_received_bytes[4];
    rc_numCoils <<= 8;
    rc_numCoils |= modbus_received_bytes[5];

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x02;

    num_of_bytes = rc_numCoils / 8;
    remainder = rc_numCoils % 8;

    if (remainder) {
        num_of_bytes += 1;
    }
    modbus_response_bytes[2] = num_of_bytes;

    l = rc_Address;
    k = 3;

    for (i = num_of_bytes; i != 0; --i) {
        if (i > 1) {
            for (j = 0; j != 8; ++j) {
                if (l >= ARRAY_LENGTH(InputBits))
                    return;

                if (InputBits[l]) {
                    lsb = 1;
                } else {
                    lsb = 0;
                }
                modbus_response_bytes[k] ^= (lsb << j);
                l++;
            }
            k++;
        } else {
            for (j = 0; j != remainder; ++j) {
                if (l >= ARRAY_LENGTH(InputBits))
                    return;

                if (InputBits[l]) {
                    lsb = 1;
                } else {
                    lsb = 0;
                }
                modbus_response_bytes[k] ^= (lsb << j);
                l++;
            }
            k++;
        }
    }
    crc = generate_crc(k + 2);

    modbus_response_bytes[k] = crc >> 8;
    modbus_response_bytes[k + 1] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i != (k + 3); ++i) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);
    clear_response();
}

static void write_coil(void)
{
    uint16_t wc_AddressLow = 0;
    uint16_t wc_AddressHigh = 0;
    uint16_t wc_Address = 0;

    uint16_t wc_ValueToWrite = 0;
    uint16_t wc_ValueToWriteLow = 0;
    uint16_t wc_ValueToWriteHigh = 0;
    uint16_t i = 0;
    uint16_t crc = 0;

    wc_Address = modbus_received_bytes[2];
    wc_Address <<= 8;
    wc_Address |= modbus_received_bytes[3];

    wc_AddressLow = modbus_received_bytes[3];
    wc_AddressHigh = modbus_received_bytes[2];

    wc_ValueToWrite = modbus_received_bytes[4];
    wc_ValueToWrite <<= 8;
    wc_ValueToWrite |= modbus_received_bytes[5];

    wc_ValueToWriteLow = modbus_received_bytes[5];
    wc_ValueToWriteHigh = modbus_received_bytes[4];

    if (wc_Address >= ARRAY_LENGTH(Coils))
        return;

    if (wc_ValueToWrite) {
        Coils[wc_Address] = 0xFF;
    } else {
        Coils[wc_Address] = 0x00;
    }

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x05;
    modbus_response_bytes[3] = wc_AddressLow;
    modbus_response_bytes[2] = wc_AddressHigh;

    modbus_response_bytes[4] = wc_ValueToWriteHigh;
    modbus_response_bytes[5] = wc_ValueToWriteLow;

    crc = generate_crc(8);

    modbus_response_bytes[6] = crc >> 8;
    modbus_response_bytes[7] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < 8; ++i) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);
    clear_response();
}

static void write_multiple_coils(void)
{
    uint16_t wmc_Address = 0;
    uint16_t wmc_AddressHigh = 0;
    uint16_t wmc_AddressLow = 0;
    uint16_t wmc_numCoils = 0;
    uint16_t wmc_numCoilsHigh = 0;
    uint16_t wmc_numCoilsLow = 0;
    uint16_t crc = 0;

    uint8_t num_of_bytes = 0;
    uint8_t remainder = 0;

    uint8_t i, j, l = 0;

    wmc_Address = modbus_received_bytes[2];
    wmc_AddressHigh = modbus_received_bytes[2];
    wmc_Address <<= 8;
    wmc_Address |= modbus_received_bytes[3];
    wmc_AddressLow = modbus_received_bytes[3];

    wmc_numCoils = modbus_received_bytes[4];
    wmc_numCoilsHigh = modbus_received_bytes[4];
    wmc_numCoils <<= 8;
    wmc_numCoils |= modbus_received_bytes[5];
    wmc_numCoilsLow = modbus_received_bytes[5];

    modbus_response_bytes[0] = slave_address;
    modbus_response_bytes[1] = 0x0F;

    num_of_bytes = wmc_numCoils / 8;
    remainder = wmc_numCoils % 8;

    if (remainder) {
        num_of_bytes += 1;
    }
    modbus_response_bytes[2] = wmc_AddressHigh;
    modbus_response_bytes[3] = wmc_AddressLow;

    modbus_response_bytes[4] = wmc_numCoilsHigh;
    modbus_response_bytes[5] = wmc_numCoilsLow;

    l = wmc_Address;

    uint8_t ValueToWrite;
    uint8_t q = 7;

    for (i = num_of_bytes; i != 0; --i) {
        ValueToWrite = modbus_received_bytes[q];
        q++;
        if (i > 1) {
            for (j = 0; j != 8; ++j) {
                if (l >= ARRAY_LENGTH(Coils))
                    return;

                if (CHECK_BIT(ValueToWrite, j)) {
                    Coils[l] = 1;
                } else {
                    Coils[l] = 0;
                }
                l++;
            }
        } else {
            for (j = 0; j != remainder; ++j) {
                if (l >= ARRAY_LENGTH(Coils))
                    return;

                if (CHECK_BIT(ValueToWrite, j)) {
                    Coils[l] = 1;
                } else {
                    Coils[l] = 0;
                }
                l++;
            }
        }
    }
    crc = generate_crc(8);

    modbus_response_bytes[6] = crc >> 8;
    modbus_response_bytes[7] = crc;

    gpio_set_re_pin_level(true);
    gpio_set_de_pin_level(true);
    for (i = 0; i < 8; ++i) {
        while (modbus_dependencies_usart_busy())
            ;
        modbus_dependencies_usart_put_byte(modbus_response_bytes[i]);
    }
    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);

    clear_response();
}

void modbus_init(uint8_t address)
{
    modbus_dependencies_init();

    slave_address = address;

    modbus_clean_all_registers();

    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(true);

    modbus_dependencies_usart_open();

    gpio_set_de_pin_level(true);
    gpio_set_re_pin_level(false);
}

void modbus_close(void)
{
    modbus_dependencies_usart_close();
}

void modbus_clean_all_registers(void)
{
    for (uint8_t i = 0; i < ARRAY_LENGTH(HoldingRegister); ++i) {
        HoldingRegister[i] = 0;
    }

    for (uint8_t i = 0; i < ARRAY_LENGTH(InputRegister); ++i) {
        InputRegister[i] = 0;
    }

    for (uint8_t i = 0; i < ARRAY_LENGTH(Coils); ++i) {
        Coils[i] = 0;
    }

    for (uint8_t i = 0; i < ARRAY_LENGTH(InputBits); ++i) {
        InputBits[i] = 0;
    }
}

bool modbus_process(void)
{
    if (command_received() && modbus_received_bytes[0] == slave_address && check_crc()) {

        switch (modbus_received_bytes[1]) {
        case 0x01:
            read_coil();
            break;

        case 0x02:
            read_input_coil();
            break;

        case 0x03:
            read_register();
            break;

        case 0x04:
            read_input_register();
            break;

        case 0x05:
            write_coil();
            break;

        case 0x06:
            write_register();
            break;

        case 0x10:
            write_multiple_registers();
            break;

        case 0x0F:
            write_multiple_coils();
            break;

        default:
            modbus_response_bytes[0] = 0;
            modbus_message_received = false;
            return false;
        }

        modbus_message_received = false;
        return true;
    }

    return false;
}

uint8_t modbus_get_slave_address(void)
{
    return slave_address;
}

void modbus_byte_received_handler(void)
{
    if ((!end_of_message) && (!new_message) && (!modbus_dependencies_usart_busy())) {
        modbus_received_bytes[current_message_received_bytes] = modbus_dependencies_usart_get_byte();
        timer_counter = 0;

        if (++current_message_received_bytes >= MAXIMUM_RECEIVE_PACKET_SIZE) {
            --current_message_received_bytes;
        }
    }

    if (new_message) {
        modbus_dependencies_timer_start();

        if (!modbus_dependencies_usart_busy()) {
            modbus_received_bytes[current_message_received_bytes] = modbus_dependencies_usart_get_byte();

            ++current_message_received_bytes;
            new_message = false;
            end_of_message = false;
            modbus_message_received = false;
            modbus_message_length = 0;
            timer_counter = 0;
        }
    }
}

void modbus_timer_tick_handler(void)
{
    ++timer_counter;

    if (timer_counter > 4) {
        modbus_message_received = true;

        end_of_message = true;
        new_message = true;

        modbus_message_length = current_message_received_bytes;

        for (current_message_received_bytes = (modbus_message_length);
             current_message_received_bytes != MAXIMUM_RECEIVE_PACKET_SIZE; ++current_message_received_bytes) {
            modbus_received_bytes[current_message_received_bytes] = 0;
        }
        current_message_received_bytes = 0;

        modbus_dependencies_timer_stop();

        timer_counter = 0;
    }
}
