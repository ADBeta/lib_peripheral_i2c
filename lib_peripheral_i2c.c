/******************************************************************************
* ESP32 Peripheral Library Collection: New Driver I2C Master Only Handler
* 
* This program is free software; you can redistribute it and/or modify it 
* under the terms of the GNU General Public License as published by the 
* Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General 
* Public License for more details.
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* ADBeta (c) 2024    Ver 2.0.1    05 May 2024
******************************************************************************/
#include "lib_peripheral_i2c.h"

#include <esp_err.h>
#include <driver/i2c_master.h>

// Takes a function, if the return is NOT ESP_OK, it will return the same 
// esp_err_t value. NOTE: This is similar to ESP_RETURN_ON_ERROR, but quiet
#define QUIET_RETURN_ON_ERROR(funct) do {                                     \
	esp_err_t f_ret_ = (funct);                                               \
	if(unlikely(f_ret_ != ESP_OK)) return f_ret_;                             \
} while(0)

/*** Interface Functions *****************************************************/
esp_err_t I2C_Init(i2c_handler_t *i2c)
{
	// Guard unconfigured handler_t
	if(!i2c)                           return ESP_ERR_INVALID_ARG;
	if(i2c->sda == 0 || i2c->scl == 0) return ESP_ERR_INVALID_ARG;

	// Configure the bus
	i2c->b_config = (i2c_master_bus_config_t) {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = i2c->port,
		.scl_io_num = i2c->scl,
		.sda_io_num = i2c->sda,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = false
	};

	// Create a new master bus
	QUIET_RETURN_ON_ERROR(i2c_new_master_bus(&i2c->b_config, &i2c->b_handle));

	i2c->d_config = (i2c_device_config_t){
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = i2c->addr,
		.scl_speed_hz = i2c->hz
	};

	// Add a master bus device from the config. Return its ERROR state
	return i2c_master_bus_add_device(
		i2c->b_handle, 
		&i2c->d_config,
		&i2c->d_handle
	);
}


esp_err_t I2C_Delete(i2c_handler_t *i2c)
{
	// Do not do anything if the input is NULL
	if(!i2c) return ESP_ERR_INVALID_ARG;
	
	// Delete the bus and master device from the bus
	QUIET_RETURN_ON_ERROR(i2c_del_master_bus(i2c->b_handle));
	return i2c_master_bus_rm_device(i2c->d_handle);
}


esp_err_t I2C_PrintDeviceScan(const i2c_handler_t *i2c)
{
	// Do not do anything if the input is NULL
	if(!i2c) return ESP_ERR_INVALID_ARG;

	// Scan through all addresses
	uint8_t addr = 0x00;
	while(addr < 0xFF)
	{
		// Scan the current address, with 1ms timeout
		esp_err_t resp = i2c_master_probe(i2c->b_handle, addr, 1);
		
		// If that address responded, print a message
		if(resp == ESP_OK) printf("Device Address 0x%02X Responded\n", addr);

		++addr;
	}
	printf("Done Scanning.\n");
	return ESP_OK;
}


/*** Data Functions **********************************************************/
esp_err_t I2C_Read(const i2c_handler_t *i2c, uint8_t *buf, const size_t len)
{
	// Make sure the passed variables are valid - and there is something to do
	if(!i2c || !buf) return ESP_ERR_INVALID_ARG;
	if(len == 0)     return ESP_OK;
	return i2c_master_receive(i2c->d_handle, buf, len, i2c->timeout_ms);
}


esp_err_t I2C_ReadFromRegister(const i2c_handler_t *i2c, const uint8_t reg,
							                    uint8_t *buf, const size_t len)
{
	// Make sure the passed variables are valid - and there is something to do
	if(!i2c || !buf) return ESP_ERR_INVALID_ARG;
	if(len == 0)     return ESP_OK;
	return i2c_master_transmit_receive(i2c->d_handle, &reg, 1, buf, len, 
									                        i2c->timeout_ms);
}

esp_err_t I2C_Write(const i2c_handler_t *i2c, const uint8_t *buf, 
					                                          const size_t len)
{
	// Make sure the passed variables are valid - and there is something to do
	if(!i2c || !buf) return ESP_ERR_INVALID_ARG;
	if(len == 0)     return ESP_OK;
	return i2c_master_transmit(i2c->d_handle, buf, len, i2c->timeout_ms);
}

esp_err_t I2C_WriteToRegister(const i2c_handler_t *i2c, const uint8_t reg,
                                          const uint8_t *buf, const size_t len)
{
	// Make sure the passed variables are valid - and there is something to do
	if(!i2c || !buf) return ESP_ERR_INVALID_ARG;
	if(len == 0)     return ESP_OK;
	
	// Write the address byte to the device, then repeat start and send the
	// buffer data
	i2c_master_transmit(i2c->d_handle, &reg, 1,  i2c->timeout_ms); 
	return i2c_master_transmit(i2c->d_handle, buf, len, i2c->timeout_ms);
}
