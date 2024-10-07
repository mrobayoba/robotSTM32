/*
 * gyro_L3G4200D.c
 *
 *  Created on: Jul 20, 2024
 *      Author: matto
 */

#include "gyro_L3G4200D.h"
#include "spi_driver_hal.h"

#define BUFFER_SIZE 16 // 72 char in the buffer_array

// For SPI communication
uint8_t gyro_spiBufferRX[BUFFER_SIZE]		= {0};
uint8_t gyro_registerBuffer					= 0;


/*** Private Functions ***/
void gyroReadData(SPI_Handler_t *ptrSPI, uint8_t address, uint8_t * ptrBufferData,uint32_t dataSize);
void gyroWriteData(SPI_Handler_t *ptrSPI,uint8_t address, uint8_t value);

void L3Gyro_config(gyroHandler_t *pL3GyroHandler){
	// Be sure power is off before modify registers
	L3Gyro_turnOff(pL3GyroHandler);

	//  Reset memory content
	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG5, 0b10000000);

	// Set Bandwith, Output Data Rate and enable axis in CTRL_REG1
	uint8_t AuxMask = 0;
	switch (pL3GyroHandler->outputDataRate){

	case e_ODR100:
		AuxMask |= (e_ODR100 << 6);
		break;
	case e_ODR200:
		AuxMask |= (e_ODR200 << 6);
		break;
	case e_ODR400:
		AuxMask |= (e_ODR400 << 6);
		break;
	case e_ODR800:
		AuxMask |= (e_ODR800 << 6);
		break;
	default:
		AuxMask |= (e_ODR400 << 6);
		break;
	}

	switch (pL3GyroHandler->bandwidth){

	case e_BW0:
		AuxMask |= (e_BW0 << 4);
		break;
	case e_BW1:
		AuxMask |= (e_BW1 << 4);
		break;
	case e_BW2:
		AuxMask |= (e_BW2 << 4);
		break;
	case e_BW3:
		AuxMask |= (e_BW3 << 4);
		break;
	default:
		AuxMask |= (e_BW3 << 4);
		break;
	}

	switch (pL3GyroHandler->axisToEnable){

	case e_EN_ONLY_Z_AXIS:
		AuxMask |= ( 1 << 2);
		break;
	case e_EN_ALL_AXIS:
		AuxMask |= 0b00000111;
		break;
	default:
		AuxMask |= 0b00000111;
		break;
	}

	// Set HPF cut-off in CTRL_REG2

	gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG2, &gyro_registerBuffer, 1);
	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG2, gyro_registerBuffer | pL3GyroHandler->HPFcutOffFreq);

	// Set Full scale in CTRL_REG4

	AuxMask = 0;

	AuxMask |= (pL3GyroHandler->FullScaleSelection) << 4;

	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG4, AuxMask);

	gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG5, &gyro_registerBuffer, 1);
	gyro_registerBuffer |= 0b1 << 4; // To enable HPF

	if (pL3GyroHandler->OutputSelection) {
		gyro_registerBuffer |= 0b10; // To set High pass and Low Pass filtering in DataReg and FIFO
	}

	if(pL3GyroHandler->FIFOstreamEnable){
		gyro_registerBuffer |= 0b1 << 6; // FIFO enable
	}

	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG5, gyro_registerBuffer);
	gyro_registerBuffer = RESET;

	// Activate data ready interrupt
	if(pL3GyroHandler->allowDataReady == SET){

		gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG3, 0b00001000);

	}

	// Config FIFO
	if(pL3GyroHandler->FIFOstreamEnable){
		gyro_registerBuffer = 0b10 << 5; // Set Stream mode
		gyro_registerBuffer |= GYRO_WATERMARK_VALUE;
		gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_FIFO_CTRL_REG, gyro_registerBuffer);
	}
	gyro_registerBuffer = RESET;

	L3Gyro_init(pL3GyroHandler);
}

void L3Gyro_init(gyroHandler_t *pL3GyroHandler){

	uint8_t powerAuxMask = 0b00001000; //Sets to normal mode
	gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG1, &gyro_registerBuffer, 1);
	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG1, (gyro_registerBuffer | powerAuxMask));

}
void L3Gyro_turnOff(gyroHandler_t *pL3GyroHandler){
	uint8_t powerAuxMask = 0b00001000; // Power down mode
	gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG1, &gyro_registerBuffer, 1);
	gyroWriteData(pL3GyroHandler->ptrSensorSPI, GYRO_CTRL_REG1, gyro_registerBuffer & (~powerAuxMask));
}
int16_t* L3Gyro_read(gyroHandler_t *pL3GyroHandler, int16_t *dataBuffer){

	// See if there is data available
	uint8_t mask;

	if (pL3GyroHandler->FIFOstreamEnable) {
		mask = 0b1<<7;
		do {
			gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_FIFO_SRC_REG, gyro_spiBufferRX, 1);
		} while (!(gyro_spiBufferRX[0] & mask));
		// Retrieve the sensor data
		gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_OUT_X_L, (uint8_t*)dataBuffer, (uint32_t)GYRO_WATERMARK_VALUE);

	}
	else{
		mask = 0b1<<2;
		do {
			gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_STATUS_REG, gyro_spiBufferRX, 1);
		} while (!(gyro_spiBufferRX[0] & mask));
		// Retrieve the sensor data
		gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_OUT_Z_L, gyro_spiBufferRX, 2);

		// Assemble data in auxData to get the two's complement value
		dataBuffer[0] = (gyro_spiBufferRX[1] << 8) | gyro_spiBufferRX[0];
	}

	return	dataBuffer;
}

uint8_t L3GyroReadReg(gyroHandler_t *pL3GyroHandler, uint8_t registerAddress){
	uint8_t aux = 0;

//	while(!((gyro_spiBufferRX[0]<<5)>>7)){
//		gyroReadData(pL3GyroHandler->ptrSensorSPI, GYRO_STATUS_REG, gyro_spiBufferRX, 1);
//	}
	// Retrieve the sensor data
	gyroReadData(pL3GyroHandler->ptrSensorSPI, registerAddress, gyro_spiBufferRX, 1);
	// Assemble data in auxData to get the two's complement value
	aux = gyro_spiBufferRX[0];

	return aux;
}

//void ADXL345_set_powerControl(ADXL345_Handler_t * accelHandler){
//	//to start or stop measurements
//	ADXL345_WriteData(ADXL345_POWER_CTRL, accelHandler->measureState);
//}

void gyroReadData(SPI_Handler_t *ptrSPI,uint8_t address, uint8_t * ptrBufferData,uint32_t dataSize){

	//For specify we are reading...
	address |= GYRO_READ_MASK;

	if(dataSize!=1){
		address |= GYRO_MULTITX_MASK;
	}

	/* Activate slavePin */
	spi_selectSlave(ptrSPI);

	/* Send address */
	spi_sendData(*ptrSPI, &address, 1);

	/* Receive Data */
	spi_receiveData(*ptrSPI, gyro_spiBufferRX, dataSize);

	/* Deactivate slavePin */
	spi_unSelectSlave(ptrSPI);

}

void gyroWriteData(SPI_Handler_t *ptrSPI, uint8_t address, uint8_t value){

	// create array to send address and 8bit of data
	uint8_t data[2];

	// add to data the address and specify writing...
	data[0] = (address & ~GYRO_READ_MASK);

	// add data to send
	data[1] = value;

	/* Activate slavePin */
	spi_selectSlave(ptrSPI);

	/* Send data */
	spi_sendData(*ptrSPI, data, 2);

	/* Deactivate slavePin */
	spi_unSelectSlave(ptrSPI);

}
