#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "nrf_twi_mngr.h"
#include <stdint.h>
#include <stdbool.h>
#include "nrf_error.h"
#include "microbit_v2.h"

#define SCL_PIN I2C_QWIIC_SCL
#define SDA_PIN I2C_QWIIC_SDA
#define TWI_INSTANCE_ID 0


/**
 * @brief initialize the TWI manager for I2C and store the instance pointer
 * 
 * @return ret_code_t NRF_SUCCESS on success, or an error_code
 */
ret_code_t i2c_init(void);




/**
 * @brief Check if the I2C driver is initialized
 * 
 * @return true if initialized
 */
bool i2c_is_initialized(void);


 /**
  * @brief Set the shared TWI manager instance.
  * 
  * @param twi_mngr Pointer to an initialized nrf_twi_mngr_t instance
  */
 void i2c_set_twi_mngr(nrf_twi_mngr_t *twi_mngr);




/**
 * @brief Write a single byte to a given register of an I2C device.
 * 
 * @param device_addr I2C address of the device
 * @param reg         Register address
 * @param data        Data byte to write
 */
ret_code_t i2c_write_reg(uint8_t device_addr, uint8_t reg, uint8_t data);

/**
 * @brief Write a block of bytes to an I2C device
 * 
 * @param device_addr I2C address of the device
 * @param data Pointer to the data buffer to write
 * @param length Number of bytes to write
 * @return ret_code_t result of the transfer
 */
ret_code_t i2c_write_block(uint8_t device_addr, uint8_t *data, uint8_t length);



/**
 * @brief Write a block of bytes to a given register of an I2C device
 * @param device_addr I2C address of the device
 * @param reg Address of the register to write to
 * @param data Pointer to the data buffer to write
 * @param length Number of bytes to write
 * @return ret_code_t result of the transfer
 */
ret_code_t i2c_write_block_reg(uint8_t device_addr, uint8_t reg, uint8_t *data, uint8_t length);



/**
 * @brief Read a single byte from a given register of an I2C device.alignas
 * 
 * @param device_addr I2C address of the device
 * @param reg         Register address
 * @param data        Pointer where the read byte will be stored
 */
ret_code_t i2c_read_reg(uint8_t device_addr, uint8_t reg, uint8_t *data);


/**
 * @brief Read a block of bytes from a device
 * 
 * @param device_addr I2C address of the device
 * @param data Pointer to where the read block will be stored
 * @param length Number of bytes to be read
 * @return ret_code_t result of the transfer
 */
ret_code_t i2c_read_block(uint8_t device_addr, uint8_t*data, uint8_t length);


/**
 * @brief Get the TWI manager instance
 * 
 * @return Pointer to the TWI manager instance
 */
nrf_twi_mngr_t* i2c_get_instance(void);


#endif //I2C_DRIVER_H