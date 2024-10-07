/*
 * gyro_L3G4200D.h
 *
 *  Created on: Jul 20, 2024
 *      Author: matto
 */

#ifndef GYRO_L3G4200D_H_
#define GYRO_L3G4200D_H_

#include "spi_driver_hal.h"

/***
 * gyroscope register map
***/
#define GYRO_READ_MASK		0b10000000
#define GYRO_MULTITX_MASK		0b01000000

#define GYRO_ID				0x0F
#define GYRO_CTRL_REG1		0x20
#define GYRO_CTRL_REG2		0x21
#define GYRO_CTRL_REG3		0x22
#define GYRO_CTRL_REG4		0x23
#define GYRO_CTRL_REG5		0x24
#define GYRO_OUT_TEMP		0x26
#define GYRO_STATUS_REG		0x27
#define GYRO_OUT_X_L		0x28
#define GYRO_OUT_X_H		0x29
#define GYRO_OUT_Y_L		0x2A
#define GYRO_OUT_Y_H		0x2B
#define GYRO_OUT_Z_L		0x2C
#define GYRO_OUT_Z_H		0x2D
#define GYRO_FIFO_CTRL_REG	0x2E
#define GYRO_FIFO_SRC_REG	0x2F
#define GYRO_INT1_CFG		0x30
#define GYRO_INT1_SRC		0x31
#define GYRO_INT1_TSH_XH	0x32
#define GYRO_INT1_TSH_XL	0x33
#define GYRO_INT1_TSH_YH	0x34
#define GYRO_INT1_TSH_YL	0x35
#define GYRO_INT1_TSH_ZH	0x36
#define GYRO_INT1_TSH_ZL	0x37
#define GYRO_INT1_DURATION	0x38

#define GYRO_WATERMARK_VALUE 0x19 // 25


enum { // gyro sensibility

	e_DPS250 = 0,
	e_DPS500,
	e_DPS2000

};

enum { // gyro output data rate in Hz

	e_ODR100 = 0,
	e_ODR200,
	e_ODR400,
	e_ODR800

};

enum { // bandwith, with the ODR generates de cutoff freq
	e_BW0 = 0,
	e_BW1,
	e_BW2,
	e_BW3
};

enum { // power mode

	e_POWERDOWN = 0,
	e_NORMALMODE

};

enum { // axis to enable

	e_EN_ONLY_Z_AXIS = 0,
	e_EN_ALL_AXIS

};

enum { // full scale selection

	e_250DPS = 0,
	e_500DPS,
	e_2000DPS

};

enum { // High Pass Filtering cut-off

	e_HPF_0 = 0,
	e_HPF_1,
	e_HPF_2,
	e_HPF_3,
	e_HPF_4,
	e_HPF_5,
	e_HPF_6,
	e_HPF_7,
	e_HPF_8,
	e_HPF_9

};

enum { // output selection

	e_OUTDATANONHPF = 0,
	e_OUTDATAHPF

};

typedef struct {

	uint8_t outputDataRate;
	uint8_t bandwidth;
	uint8_t powerMode;	// power down or normal mode-sleep mode
	uint8_t axisToEnable; // Select which axes to enable
	uint8_t HPFcutOffFreq; //Set min freq to be measured
	uint8_t allowDataReady; // SET to get an interrupt when new data is available
	uint8_t OutputSelection; // Select how retrive data (filtering)
	uint8_t FullScaleSelection; // dps range
	uint8_t FIFOstreamEnable;	// Set to use FIFO in stream mode

	SPI_Handler_t *ptrSensorSPI;	// handler of the spi port of the sensor

}gyroHandler_t;

/*** Public Functions ***/

void L3Gyro_config(gyroHandler_t *pL3GyroHandler);
void L3Gyro_init(gyroHandler_t *pL3GyroHandler);
void L3Gyro_turnOff(gyroHandler_t *pL3GyroHandler);
int16_t* L3Gyro_read(gyroHandler_t *pL3GyroHandler, int16_t* dataBuffer);
uint8_t L3GyroReadReg(gyroHandler_t *pL3GyroHandler, uint8_t registerAddress);


#endif /* GYRO_L3G4200D_H_ */
