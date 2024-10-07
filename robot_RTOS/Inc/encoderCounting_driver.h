/*
 * encoderCounting_driver.h
 *
 *  Created on: May 8, 2024
 *      Author: matto
 */

/* This driver needs some pre-configured peripherals: x2 GPIOx input mode  and x1 Timx */

#ifndef ENCODERCOUNTING_DRIVER_H_
#define ENCODERCOUNTING_DRIVER_H_

#include <stdio.h>
#include <stdint.h>
#include "exti_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"

typedef struct{

	// peripherals to counting
	// OVERWRITE ONLY THE NEEDED ELEMENTS OF EACH HANDLER!!!
	GPIO_Handler_t	*encoderLeft;
	GPIO_Handler_t	*encoderRight;

	EXTI_Config_t	exti_encoderLeft;
	EXTI_Config_t	exti_encoderRight;

	Timer_Handler_t	*samplingTimer;

	uint16_t counts[2];

	uint16_t samplingTime; // By default 1 second
	uint8_t samplingDone_flag; // To how when new data is available to perform PID control

}encoder_handler_t;

void counterConfig(encoder_handler_t* encoders);
void getCount(encoder_handler_t* encoders);
uint32_t getSum(void);
void initSum(void);
void changeExtiEdges(encoder_handler_t* encoders,uint8_t leftCounterEdge, uint8_t rightCounterEdge);
void changeSamplingTime(encoder_handler_t* encoders,uint16_t newSamplingTime_in_seconds);


#endif /* ENCODERCOUNTING_DRIVER_H_ */
