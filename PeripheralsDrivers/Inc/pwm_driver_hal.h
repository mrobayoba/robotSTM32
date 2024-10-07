/*
 * pwm_driver_hal.h
 *
 *  Created on: Nov 6, 2023
 *      Author: mrobayoba
 */

#ifndef PWM_DRIVER_HAL_H_
#define PWM_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum{
	PWM_CHANNEL_1 = 0,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4
};

enum{
	PWM_POLARITY_HIGH = 0,
	PWM_POLARITY_LOW
};

#define PWM_DUTY_0_PERCENT		0
#define PWM_DUTY_100_PERCENT	100

/**/
typedef struct
{
	uint8_t		channel; 		// Canal PWM relacionado con el TIMER
	uint32_t	prescaler;		// A qué velocidad se incrementa el Timer
	uint16_t	periodo;		// Indica el número de veces que el Timer se incrementa, el periodo de la frecuencia viene dado por Time_Fosc * PSC * ARR
	uint16_t	dutyCycle;		// Expresado como un número entre 001 y 999, Valor en porcentaje (xx.x%) del tiempo que la señal está en alto
	uint8_t		outPolarity;	// Invirte el los valores en High o low en la señal PWM de salida
}PWM_Config_t;

/**/
typedef struct
{
	TIM_TypeDef		*ptrTIMx;	// Timer al que esta asociado el PWM
	PWM_Config_t	config;	// Configuración inicial del PWM
}PWM_Handler_t;

/* Prototipos de las funciones publicas*/
void pwm_Config(PWM_Handler_t *ptrPwmHandler);
void pwm_updateDutyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty);
void pwm_updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq);
void pwm_updatePolarity (PWM_Handler_t *ptrPwmHandler, uint8_t newPolarity);
void pwm_star_Signal(PWM_Handler_t *ptrPwmHandler);
void pwm_stop_Signal(PWM_Handler_t *ptrPwmHandler);



#endif /* PWM_DRIVER_HAL_H_ */
