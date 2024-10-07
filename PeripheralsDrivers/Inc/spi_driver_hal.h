/*
 * spi_driver_hal.h
 *
 *  Created on: Nov 10, 2023
 *      Author: mrobayoba
 */

#ifndef SPI_DRIVER_HAL_H_
#define SPI_DRIVER_HAL_H_

#include "stm32f4xx.h"
#include "stdint.h"
#include "gpio_driver_hal.h"

enum{
	SPI_POL_PHA_00	= 0,
	SPI_POL_PHA_01,
	SPI_POL_PHA_10,
	SPI_POL_PHA_11
};

enum{
	SPI_BAUDRATE_FPCLK_2	= 0,
	SPI_BAUDRATE_FPCLK_4,
	SPI_BAUDRATE_FPCLK_8,
	SPI_BAUDRATE_FPCLK_16,
	SPI_BAUDRATE_FPCLK_32,
	SPI_BAUDRATE_FPCLK_64,
	SPI_BAUDRATE_FPCLK_128,
	SPI_BAUDRATE_FPCLK_256
};

enum{
	SPI_RECEIVE_ONLY	= 0,
	SPI_FULL_DUPPLEX
};

enum{
	SPI_DATASIZE_8_BIT	= 0,
	SPI_DATASIZE_16_BIT
};


typedef struct
{
	uint8_t SPI_mode;	//define ClOCK and PHASE for the transmission
	uint8_t SPI_baudrate;	// define the data rate of the SPI communication
	uint8_t SPI_fullDuplexEnable;	// configure if only receive data or bi-directional
	uint8_t SPI_dataSize;	// data size for transmission
	uint8_t SPI_enableIntRX;
	uint8_t SPI_enableIntTx;
}SPI_Config_t;


typedef struct
{
	SPI_TypeDef		*ptrSPIx;
	SPI_Config_t	SPI_Config;
	GPIO_Handler_t	SPI_slavePin;
}SPI_Handler_t;


/* Public function prototypes */

void spi_config(SPI_Handler_t ptrHandlerSPI);
void spi_sendData(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize);
void spi_receiveData(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize);
void spi_selectSlave(SPI_Handler_t* ptrHandlerSPI);
void spi_unSelectSlave(SPI_Handler_t* ptrHandlerSPI);

#endif /* SPI_DRIVER_HAL_H_ */
