/*
 * encoderCounting_driver.c
 *
 *  Created on: May 8, 2024
 *      Author: matto
 */

#include "encoderCounting_driver.h"
#include "gpio_driver_hal.h"
#include "exti_driver_hal.h"
#include "timer_driver_hal.h"
#include "main.h"
//#include <stdlib.h>

uint8_t samplingDone_flag = RESET;

uint16_t counter_L, counter_R; // To count on each wheel
encoder_handler_t *ptrEncoders; // To save the last N values
uint32_t sum_L, sum_R;

//int compare(const void* a, const void* b);
//int median(int* array);

void counterConfig(encoder_handler_t *encoders){
	// FIRST ASSIGN THE POINTER OF ENCODERS TO A GLOBAL POINTER TO CAPTURE COUNTS IN TIMER
	ptrEncoders = encoders;

	// To ensure gpios and timer are well configured, overwrite the original config
	encoders->encoderLeft->pGPIOx							=	GPIOC;
	encoders->encoderLeft->pinConfig.GPIO_PinNumber			=	PIN_1;
	encoders->encoderLeft->pinConfig.GPIO_PinMode			= 	GPIO_MODE_IN;
	encoders->encoderLeft->pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	gpio_Config(encoders->encoderLeft);

	encoders->encoderRight->pGPIOx							=	GPIOC;
	encoders->encoderRight->pinConfig.GPIO_PinNumber		=	PIN_3;
	encoders->encoderRight->pinConfig.GPIO_PinMode			= 	GPIO_MODE_IN;
	encoders->encoderRight->pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;
	gpio_Config(encoders->encoderRight);

	// configure EXTIs here
	encoders->exti_encoderLeft.pGPIOHandler					=	encoders->encoderLeft;
	encoders->exti_encoderLeft.edgeType						=	EXTI_RISING_AND_FALLING_EDGES;
	encoders->exti_encoderLeft.interruptPriority			=	e_PRIORITY_NORMAL_MIN_PLUS_10; //5
	exti_Config(&encoders->exti_encoderLeft);

	encoders->exti_encoderRight.pGPIOHandler				=	encoders->encoderRight;
	encoders->exti_encoderRight.edgeType					=	EXTI_RISING_AND_FALLING_EDGES;
	encoders->exti_encoderRight.interruptPriority			=	e_PRIORITY_NORMAL_MIN_PLUS_10; //5
	exti_Config(&encoders->exti_encoderRight);

	encoders->samplingTimer->pTIMx								= TIM3;
	encoders->samplingTimer->TIMx_Config.TIMx_Prescaler			= 10000; // Generates 0.1 ms increments
	encoders->samplingTimer->TIMx_Config.TIMx_Period			= (uint32_t) (SAMPLING_TIME*10);	// With the prescaler, generates 250ms increments
	encoders->samplingTimer->TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;
	encoders->samplingTimer->TIMx_Config.TIMx_InterruptEnable	= TIMER_INT_ENABLE;
	timer_Config(encoders->samplingTimer);
	timer_SetState(encoders->samplingTimer, TIMER_ON);

}
/**
 * getCounts returns a list with the last N values
 **/
void getCount(encoder_handler_t *encoders){ // Not needed if timer pointer capture works...

	// Reset counter variables
//	counter_L = 0; counter_R = 0;
	// Procedure to get counts in the defined sampling time
//	timer_SetState(encoders->samplingTimer, SET);

//	while(!samplingDone_flag){ __NOP();}

	// Stop counting...
//	timer_SetState(encoders->samplingTimer, RESET);
//	samplingDone_flag =RESET;

	// Store last values, Store the values in the structure
//	encoders->counts[0] = counter_L, encoders->counts[1] = counter_R;
}

uint32_t getSum(void){
	return (sum_L+sum_R)/2;
}

void initSum(void){ // To restart total counting
	sum_L = RESET, sum_R = RESET;
}

void changeExtiEdges(encoder_handler_t* encoders,uint8_t leftCounterEdge, uint8_t rightCounterEdge){

	encoders->exti_encoderLeft.edgeType						=	leftCounterEdge;
	exti_Config(&encoders->exti_encoderLeft);

	encoders->exti_encoderRight.edgeType					= 	rightCounterEdge;
	exti_Config(&encoders->exti_encoderRight);


}
void changeSamplingTime(encoder_handler_t* encoders,uint16_t newSamplingTime_in_ms){
	timer_SetState(encoders->samplingTimer, RESET);

	encoders->samplingTimer->TIMx_Config.TIMx_Period		= newSamplingTime_in_ms*10;	// With the prescaler, generates 250ms increments
	timer_Config(encoders->samplingTimer);
	timer_SetState(encoders->samplingTimer, SET);
}

void callback_ExtInt1(void){
	counter_R++;
	sum_R++;
}
//
void callback_ExtInt3(void){
	counter_L++;
	sum_L++;
}
void Timer3_Callback(void){

//	BaseType_t xHigherPriorityTaskWoken;
//	//
//	xHigherPriorityTaskWoken = pdFALSE; // Comment this!!!

	// By this way the first value is not true but the next ones will be right
	ptrEncoders->counts[0] = counter_L, ptrEncoders->counts[1] = counter_R;
	counter_L = 0; counter_R = 0;
	ptrEncoders->samplingDone_flag = SET;
//	xTaskNotifyFromISR(xTaskHandler_printEncoders,0,eNoAction,&xHigherPriorityTaskWoken);

}

