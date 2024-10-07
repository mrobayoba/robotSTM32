/*
 * pwm_driver_hal.c
 *
 *  Created on: Nov 6, 2023
 *      Author: mrobayoba
 */

#include "../Inc/pwm_driver_hal.h"

/* Headers for private function prototypes */
static void pwm_enableClockPeripheral(PWM_Handler_t *ptrPwmHandler);
static void pwm_setChannel(PWM_Handler_t *ptrPwmHandler);
static void pwm_setFrequency(PWM_Handler_t *ptrPwmHandler);
static void pwm_setDutyCycle(PWM_Handler_t *ptrPwmHandler);
static void pwm_setPolarity(PWM_Handler_t *ptrPwmHandler);
static void pwm_enableOutput(PWM_Handler_t *ptrPwmHandler);

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	pwm_enableClockPeripheral(ptrPwmHandler);
	/* 1. Cargamos la frecuencia deseada */
	pwm_setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	pwm_setDutyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	/* agregue acá su código */

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	pwm_setChannel(ptrPwmHandler);

	pwm_setPolarity(ptrPwmHandler);
	/* 6. Activamos la salida seleccionada */
	pwm_enableOutput(ptrPwmHandler);
}

/* Función para activar el Timer y activar todo el módulo PWM */
void pwm_star_Signal(PWM_Handler_t *ptrPwmHandler) {
	/* 4. Restart counter register*/
	ptrPwmHandler->ptrTIMx->CNT = 0;

	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN; // Start counter (Timer)
}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void pwm_stop_Signal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN; // Stop counter
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
static void pwm_enableOutput(PWM_Handler_t *ptrPwmHandler) {
	/* TIM2 to TIM5 */
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
//		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1P; // keep it in 0 to active high polarity
		break;
	}
	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}

	// Channels 3 and 4 not available for timers 9 to 11
	case PWM_CHANNEL_3: {
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {
			// Activamos la salida del canal 1
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
		}
		break;
	}
	case PWM_CHANNEL_4: {
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {
			// Activamos la salida del canal 1
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		}
		break;
	}

	default: {
		__NOP();
		break;
	}
	}
}

/*
 * La frecuencia es definida por el conjunto formado por el prescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
static void pwm_setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler-1;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo;
}


/***
 *  Función para actualizar la frecuencia, funciona de la mano con setFrequency
 *  Debe ser un número de orden de magnitud menor o igual a 10⁴ y máximo dos cifras significativas!
 * * */
void pwm_updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
	ptrPwmHandler->config.periodo = newFreq;

	// Activamos el auto-reload register para el ARR (ARPE)
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_ARPE;

	// Llamamos a la función que cambia la frecuencia
	pwm_setFrequency(ptrPwmHandler);

	// Desactivamos el auto-reload register
	ptrPwmHandler->ptrTIMx->CR1 &= ~ TIM_CR1_ARPE;

	// Recalculamos el duty
	pwm_setDutyCycle(ptrPwmHandler);

}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
static void pwm_setDutyCycle(PWM_Handler_t *ptrPwmHandler){

	int16_t duty = ptrPwmHandler->config.dutyCycle;
	float auxVal = (((float) ptrPwmHandler->config.periodo)/1000) * duty;
	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	// Period * (duty)/1000 == 40*duty
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = (uint16_t) auxVal;

		break;
	}
	case PWM_CHANNEL_2:{
		ptrPwmHandler->ptrTIMx->CCR2 = (uint16_t) auxVal;


		break;
	}
	case PWM_CHANNEL_3:{
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {
			ptrPwmHandler->ptrTIMx->CCR3 = (uint16_t) auxVal;
		}

		break;
	}
	case PWM_CHANNEL_4:{
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {
			ptrPwmHandler->ptrTIMx->CCR4 = (uint16_t) auxVal;
		}

		break;
	}

	default:{
		__NOP();
		break;
	}

	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void pwm_updateDutyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDuty){
	// Actualizamos el registro que manipula el dutty
	ptrPwmHandler->config.dutyCycle = newDuty;

	// Llamamos a la función que cambia el dutty y cargamos el nuevo valor
	pwm_setDutyCycle(ptrPwmHandler);
}

/* RCC - Enable Clock Peripheral function */
static void pwm_enableClockPeripheral(PWM_Handler_t *ptrPwmHandler){
	/* TIM2 to TIM5*/

		if(ptrPwmHandler->ptrTIMx ==TIM2){ //32-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;
		}
		else if(ptrPwmHandler->ptrTIMx ==TIM3){ //16-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
		}
		else if(ptrPwmHandler->ptrTIMx ==TIM4){ //16-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM4EN;
		}
		else if(ptrPwmHandler->ptrTIMx ==TIM5){ //32-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM5EN;
		}

		/* TIM9 to TIM11 */
		else if(ptrPwmHandler->ptrTIMx ==TIM9){
			RCC->APB2ENR |=RCC_APB2ENR_TIM9EN;
		}
		else if(ptrPwmHandler->ptrTIMx ==TIM10){
			RCC->APB2ENR |=RCC_APB2ENR_TIM10EN;
		}
		else if(ptrPwmHandler->ptrTIMx ==TIM11){
			RCC->APB2ENR |=RCC_APB2ENR_TIM11EN;
		}

		else{
			__NOP();
		}
}

/* Channel setting function */
static void pwm_setChannel(PWM_Handler_t *ptrPwmHandler){
	switch(ptrPwmHandler->config.channel){
		case PWM_CHANNEL_1:{
			// Seleccionamos como salida el canal
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;

			// Configuramos el canal como PWM
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC1M;
			ptrPwmHandler->ptrTIMx->CCMR1 |= 0x6UL << TIM_CCMR1_OC1M_Pos; // 110 mode 1


			// Activamos la funcionalidad de pre-load
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC1PE;
			ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

			break;
		}

		case PWM_CHANNEL_2:{
			// Seleccionamos como salida el canal
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;

			// Configuramos el canal como PWM
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC2M;
			ptrPwmHandler->ptrTIMx->CCMR1 |= 0x6UL << TIM_CCMR1_OC2M_Pos; // 110 mode 1


			// Activamos la funcionalidad de pre-load
			ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_OC2PE;
			ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

			break;
		}

		case PWM_CHANNEL_3:{
			if ((ptrPwmHandler->ptrTIMx == TIM2)||
					(ptrPwmHandler->ptrTIMx == TIM3)||
					(ptrPwmHandler->ptrTIMx == TIM4)||
					(ptrPwmHandler->ptrTIMx == TIM5)) {
				// Seleccionamos como salida el canal
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;

				// Configuramos el canal como PWM
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC3M;
				ptrPwmHandler->ptrTIMx->CCMR2 |= 0x6UL << TIM_CCMR2_OC3M_Pos; // 110 mode 1


				// Activamos la funcionalidad de pre-load
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC3PE;
				ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;
			}

			break;
		}

		case PWM_CHANNEL_4:{
			if ((ptrPwmHandler->ptrTIMx == TIM2)||
					(ptrPwmHandler->ptrTIMx == TIM3)||
					(ptrPwmHandler->ptrTIMx == TIM4)||
					(ptrPwmHandler->ptrTIMx == TIM5)) {
				// Seleccionamos como salida el canal
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;

				// Configuramos el canal como PWM
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC4M;
				ptrPwmHandler->ptrTIMx->CCMR2 |= 0x6UL << TIM_CCMR2_OC4M_Pos; // 110 mode 1


				// Activamos la funcionalidad de pre-load
				ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_OC4PE;
				ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;
			}

			break;
		}

		default:{
			break;
		}

		}// fin del switch-case
}

/* Output Polarity function */
void pwm_setPolarity(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su polaridad
	switch(ptrPwmHandler->config.channel){

	case PWM_CHANNEL_1:{

		if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_LOW){
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1P;
		}
		else if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_HIGH){
			ptrPwmHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
		}

		break;
	}
	case PWM_CHANNEL_2:{
		if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_LOW){
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2P;
		}
		else if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_HIGH){
			ptrPwmHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
		}

		break;
	}
	case PWM_CHANNEL_3:{
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {

			if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_LOW){
				ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3P;
			}
			else if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_HIGH){
				ptrPwmHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			}

		}

		break;
	}
	case PWM_CHANNEL_4:{
		if ((ptrPwmHandler->ptrTIMx == TIM2)||
				(ptrPwmHandler->ptrTIMx == TIM3)||
				(ptrPwmHandler->ptrTIMx == TIM4)||
				(ptrPwmHandler->ptrTIMx == TIM5)) {
			if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_LOW){
				ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4P;
			}
			else if(ptrPwmHandler->config.outPolarity == PWM_POLARITY_HIGH){
				ptrPwmHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			}

		}

		break;
	}

	default:{
		break;
	}

	}// fin del switch-case
}

void pwm_updatePolarity (PWM_Handler_t *ptrPwmHandler, uint8_t newPolarity){
	// Actualizamos el registro que manipula el dutty
	ptrPwmHandler->config.outPolarity = newPolarity;

	// Llamamos a la función que cambia la polaridad y cargamos el nuevo valor
	pwm_setPolarity(ptrPwmHandler);
}




