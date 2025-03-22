#include "i2c_driver.h"
#include "nrf_twi_mngr.h"
#include "nrf_drv_clock.h"
#include "app_error.h"

// Create TWI manager instance
NRF_TWI_MNGR_DEF(twi_mngr_instance, 3, 0);
static nrf_twi_mngr_t * p_twi_mngr = &twi_mngr_instance;


void rtc_set_twi_manager(nrf_twi_mngr_t *twi_mngr)
{
    p_twi_mngr = twi_mngr;
}

ret_code_t i2c_init(void)
{
    ret_code_t err;

    // Initialize the low-frequency clock required by the TWI manager
    err = nrf_drv_clock_init();
    APP_ERROR_CHECK(err);
    nrf_drv_clock_lfclk_request(NULL);


    // Configure the TWI manager
    nrf_drv_twi_config_t twi_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    
        twi_config.scl                    = SCL_PIN,
        twi_config.sda                    = SDA_PIN,
        twi_config.frequency              = NRF_TWI_FREQ_400K,
        twi_config.interrupt_priority     = APP_IRQ_PRIORITY_HIGH, 
    
    
    err = nrf_twi_mngr_init(p_twi_mngr, &twi_config);
    return err;

    
}

bool i2c_is_initialized(void)
{
    return (p_twi_mngr != NULL);
}

ret_code_t i2c_write_reg(uint8_t device_addr, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = { reg, data};
    nrf_twi_mngr_transfer_t transfer = NRF_TWI_MNGR_WRITE(device_addr, buf, sizeof(buf), 0);
    return nrf_twi_mngr_perform(p_twi_mngr, NULL, &transfer, 1, NULL);
}

ret_code_t i2c_write_block(uint8_t device_addr, uint8_t *data, uint8_t length)
{
    nrf_twi_mngr_transfer_t transfer = NRF_TWI_MNGR_WRITE(device_addr, data, length, 0);
    return nrf_twi_mngr_perform(p_twi_mngr, NULL, &transfer, 1, NULL); 
}

ret_code_t i2c_write_block_reg(uint8_t device_addr, uint8_t reg, uint8_t *data, uint8_t length)
{
    uint8_t buf[length + 1];
    buf[0] = reg;
    memcpy(&buf[1], data, length);
    return i2c_write_block(device_addr, data, length);
}

ret_code_t i2c_read_reg(uint8_t device_addr, uint8_t reg, uint8_t *data)
{
    
    nrf_twi_mngr_transfer_t transfers[2] = {
        NRF_TWI_MNGR_WRITE(device_addr, &reg, 1, NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ(device_addr, data, 1, 0)
    };

    return nrf_twi_mngr_perform(p_twi_mngr, NULL, transfers, 2, NULL);
}




ret_code_t i2c_read_block(uint8_t device_addr, uint8_t*data, uint8_t length)
{
    nrf_twi_mngr_transfer_t transfer = NRF_TWI_MNGR_READ(device_addr, data, length, 0);
    return nrf_twi_mngr_perform(p_twi_mngr, NULL, &transfer, 1, NULL);    
}