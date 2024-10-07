/*
 * exti_driver_hal.h
 *
 *  Created on: Sep 28, 2023
 *      Author: mrobayoba
 */

#ifndef EXTI_DRIVER_HAL_H_
#define EXTI_DRIVER_HAL_H_

#include "stm32f4xx.h"

#include "NVIC_FreeRTOS_priorities.h"
#include "gpio_driver_hal.h"

enum
{
	EXTI_FALLING_EDGE	= 0,
	EXTI_RISING_EDGE,
	EXTI_RISING_AND_FALLING_EDGES
}; // EXTERNAL_INTERRUPTS_EDGES

typedef struct
{
	GPIO_Handler_t *pGPIOHandler;		// Handler del pin GPIO que lanzara la interrupción
	uint8_t			edgeType;			// Se selecciona si se desea un tipo de flanco subiendo o bajando
	uint8_t			interruptPriority;	// Use e_PRIORITY_NORMAL_MIN_xx to set priority

}EXTI_Config_t;


void exti_Config(EXTI_Config_t *extiConfig);
void exti_config_newInterrupt(EXTI_Config_t *extiConfig, uint8_t newPriority);
void callback_ExtInt0(void);
void callback_ExtInt1(void);
void callback_ExtInt2(void);
void callback_ExtInt3(void);
void callback_ExtInt4(void);
void callback_ExtInt5(void);
void callback_ExtInt6(void);
void callback_ExtInt7(void);
void callback_ExtInt8(void);
void callback_ExtInt9(void);
void callback_ExtInt10(void);
void callback_ExtInt11(void);
void callback_ExtInt12(void);
void callback_ExtInt13(void);
void callback_ExtInt14(void);
void callback_ExtInt15(void);

#endif /* EXTI_DRIVER_HAL_H_ */
