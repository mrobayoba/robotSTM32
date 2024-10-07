/*
 * systick_driver_hal.h
 *
 *  Created on: Oct 20, 2023
 *      Author: mrobayoba
 */

#ifndef SYSTICK_DRIVER_HAL_H_
#define SYSTICK_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum
{
	SYSTICK_INT_DISABLE	= 0,
	SYSTICK_INT_ENABLE
};

#define SYSTICK_LOAD_16MHz_1ms		16000
#define SYSTICK_LOAD_100MHz_1ms		100000


/* The following structure has the minimal config to drive the Systick */

typedef struct
{

	uint32_t	Systick_Reload;		// Value to generate 1 ms
	uint8_t		Systick_IntState;	// Turns ON or OFF the interruption mode
} SysTick_Config_t;

/* Systick's Handler */

typedef struct
{
	SysTick_Type				*pSystick;
	SysTick_Config_t			Systick_Config;

} Systick_Handler_t;

void SysTick_config_ms(Systick_Handler_t *SysTick_Handler);
uint64_t SysTick_get_ms(void);
void SysTick_delay_ms(uint32_t wait_time_ms);

/* This function must be over-write in the main in order to the system works */

/* for 32 or 16-bits autoreload counters */
void SysTick_Handler(void);

void SysTick_callback(void);

#endif /* SYSTICK_DRIVER_HAL_H_ */
