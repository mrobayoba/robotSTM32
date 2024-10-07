/*
 * timer_driver_hal.h
 *
 *  Created on: Sep 20, 2023
 *      Author: mrobayoba
 */

#ifndef INC_TIMER_DRIVER_HAL_H_
#define INC_TIMER_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum{

	TIMER_INT_DESABLE	=	0,
	TIMER_INT_ENABLE	=	0

};

enum{

	TIMER_UP_COUNTER	=	0,
	TIMER_DOWN_COUNTER

};

enum{

	TIMER_OFF	=	0,
	TIMER_ON

};

/* The following structure has the minimal config to drive the timer */

typedef struct
{
	uint8_t		TIMx_mode;				// Up or Down
	uint16_t	TIMx_Prescaler;			// Prescaler...
	uint32_t	TIMx_Period;			// Value in ms of the Timer period
	uint8_t		TIMx_InterruptEnable;	// Turns ON or OFF the interruption mode
	uint8_t		interruptPriority;		// Use e_PRIORITY_NORMAL_MIN_xx to set priority
} Timer_BasicConfig_t;

/* Timer's Handler */

typedef struct
{
	TIM_TypeDef				*pTIMx;
	Timer_BasicConfig_t		TIMx_Config;
} Timer_Handler_t;

/* For testing assert parameters - checking basic configurations */

#define IS_TIMER_INTERRUP(VALUE)	(((VALUE) == TIMER_INT_DISABLE) || ((VALUE) == TIMER_INT_ENABLE))

#define IS_TIMER_MODE(VALUE)		(((VALUE) == TIMER_UP_COUNTER) || ((VALUE) == TIMER_DOWN_COUNTER))

#define IS_TIMER_STATE(VALUE)		(((VALUE) == TIMER_OFF) || ((VALUE) == TIMER_ON))

#define IS_TIMER_PRESC(VALUE)		(((uint32_t)VALUE) > 1 && ((uint32_t)VALUE) < 0xFFFE)

#define IS_TIMER_PERIOD(PERIOD)		(((uint32_t)PERIOD) > 1)

void timer_Config(Timer_Handler_t		*pTimerHandler);
void timer_SetState(Timer_Handler_t		*pTimerHandler, uint8_t newState);

/* This function must be over-write in the main in order to the system works */

/* for 32 or 16-bits autoreload counters */
void Timer2_Callback(void);
void Timer3_Callback(void);
void Timer4_Callback(void);
void Timer5_Callback(void);

/* for 16-bits autoreload counters */
void Timer9_Callback(void);
void Timer10_Callback(void);
void Timer11_Callback(void);


#endif /* INC_TIMER_DRIVER_HAL_H_ */
