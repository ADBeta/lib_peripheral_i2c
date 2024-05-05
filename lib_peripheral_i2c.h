/******************************************************************************
* ESP32 Peripheral Library Collection: I2C Master Only Handler
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
* ADBeta (c) 2024
******************************************************************************/
#ifndef LIB_PERIPH_I2C
#define LIB_PERIPH_I2C

#include <esp_err.h>
#include <driver/i2c_master.h>

/*** Interface Structs & Functions *******************************************/
typedef struct {
	// User Modifiable values
	uint8_t addr;         // I2C Device address
	uint8_t scl, sda;     // SCL and SDA IO pins
	uint32_t hz;          // Device transfer speed in Hz
	i2c_port_num_t port;  // Port Number: -1 for autoselection
	int timeout_ms;       // Timeout the device after x millis. -1 to disable

	// WARN: Please DO NOT Change these values
	i2c_master_bus_config_t b_config;    // Bus Config
	i2c_master_bus_handle_t b_handle;    // Bus Handle
	i2c_device_config_t     d_config;    // Device Handle
	i2c_master_dev_handle_t d_handle;    // Device Config
} i2c_handler_t;
 

// Initialise the I2C Device and Bus
// Returns ESP_OK on success
esp_err_t I2C_Init(i2c_handler_t *i2c);

// Deletes and cleans up a passed i2c_handler
// Returns ESP_OK on success
esp_err_t I2C_Delete(i2c_handler_t *i2c);


// Scans the I2C Bus for any device that responds, then prints their address 
// to the debugging console.
// Returns ESP_OK on success
esp_err_t I2C_PrintDeviceScan(const i2c_handler_t *i2c);

/*** Data Functions **********************************************************/
// Takes an I2C Handler, a buffer, and a length of bytes to copy. Does not
// specify a register to access
// Returns ESP_OK on success
esp_err_t I2C_Read(const i2c_handler_t *i2c, uint8_t *buf, const size_t len);


// Takes an I2C Handler, a register, a buffer and a length of bytes to copy.
// Returns ESP_OK on success
esp_err_t I2C_ReadFromRegister(const i2c_handler_t *i2c, const uint8_t reg,
							                   uint8_t *buf, const size_t len);

// Takes an I2C Handler, a buffer and length of bytes to copy.
// Returns ESP_OK on success
esp_err_t I2C_Write(const i2c_handler_t *i2c, const uint8_t *buf, 
					                                         const size_t len);

// Takes an I2C Handler, a regiser, a buffer, and a length of bytes to copy.
// Returns ESP_OK on success
esp_err_t I2C_WriteToRegister(const i2c_handler_t *i2c, const uint8_t reg,
                                         const uint8_t *buf, const size_t len);

#endif
