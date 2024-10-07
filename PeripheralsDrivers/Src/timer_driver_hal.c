/*
 * timer_driver_hal.c
 *
 * 	Created on: Sep 20, 2023
 * 		Author: mrobayoba
 */

#include "../Inc/timer_driver_hal.h"
#include "../Inc/timer_driver_hal.h"

#include "stm32f4xx.h"
#include "stm32_assert.h"

#include "../Inc/stm32_assert.h"

/* The next variable will store the reference (address) of the peripheral it is working on */
TIM_TypeDef	*ptrTimerUsed;

/* === Headers for private functions === */
static void timer_enable_clock_peripheral(Timer_Handler_t *pTimerHandler);
static void timer_set_prescaler(Timer_Handler_t *pTimerHandler);
static void timer_set_period(Timer_Handler_t *pTimerHandler);
static void timer_set_mode(Timer_Handler_t *pTimerHandler);
static void timer_config_interrupt(Timer_Handler_t *pTimerHandler);

/*
 * Function where it is loaded the Timer config
 * Remember always start by activate the clock signal
 * of the peripheral in use.
 *
 * Also, in this case, take care when use the interruptions
 * The Timers are  directly connected to the NVIC element of the Cortex-Mx
 *
 * Must configure and/or use:
 * - TIMx_CR1	(control Register 1)
 * - TIMx_SMCR 	(slave mode control register) -> keep on 0 to Basic Timer Mode
 * - TIMx_DIER	(DMA and Interrupt enable register)
 * - TIMx_SR	(Status register)
 * - TIMx_CNT	(Counter)
 * - TIMx_PSC	(Pre-scaler)
 * - TIMx_ARR	(Auto-reload register)
 *
 * Since we will work with interruptions, before configure a new one, must be deactivated
 * the global interruption system, activate the specific IRQ and then activate back the system.
 */

void timer_Config(Timer_Handler_t *pTimerHandler){
	//Save a reference to the peripheral that is working on
	ptrTimerUsed = pTimerHandler->pTIMx;

	/* 0. To deactivate the global interruptions while configure the system */
	__disable_irq();
	/* 1. To activate the clock signal on the required peripheral */
	timer_enable_clock_peripheral(pTimerHandler);

	/* 2. To configure the Pre-scaler */
	timer_set_prescaler(pTimerHandler);

	/* 3. To configure whether UP_COUNT or DOWN_COUNT (mode) */
	timer_set_mode(pTimerHandler);

	/* 4. To configure the Auto-reload */
	timer_set_period(pTimerHandler);

	/* 5. To configure the interruption */
	timer_config_interrupt(pTimerHandler);

	/* x. To activate back the system interruptions */
	__enable_irq();

	/* The Timer starts OFF */
	timer_SetState(pTimerHandler, TIMER_OFF);

}



void timer_enable_clock_peripheral(Timer_Handler_t *pTimerHandler){

	// To verify if it is a allowed Timer
	assert_param(IS_TIM_INSTANCE(pTimerHandler->pTIMx));

	/* TIM2 to TIM5*/

	if(pTimerHandler->pTIMx ==TIM2){ //32-bit
		RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;
	}
	else if(pTimerHandler->pTIMx ==TIM3){ //16-bit
		RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
	}
	else if(pTimerHandler->pTIMx ==TIM4){ //16-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM4EN;
		}
	else if(pTimerHandler->pTIMx ==TIM5){ //32-bit
			RCC->APB1ENR |=RCC_APB1ENR_TIM5EN;
		}

	/* TIM9 to TIM11 (16-bit)*/
	else if(pTimerHandler->pTIMx ==TIM9){
			RCC->APB2ENR |=RCC_APB2ENR_TIM9EN;
		}
	else if(pTimerHandler->pTIMx ==TIM10){
			RCC->APB2ENR |=RCC_APB2ENR_TIM10EN;
		}
	else if(pTimerHandler->pTIMx ==TIM11){
			RCC->APB2ENR |=RCC_APB2ENR_TIM11EN;
		}

	else{
		__NOP();
	}
}


/*
 * The Pre-scaler configures the ratio at what the register will increase
 *
 * Timer CNT
 */
void timer_set_prescaler(Timer_Handler_t *pTimerHandler){

	// To verify if the pre-scaler value is allowed
	assert_param(IS_TIMER_PRESC(pTimerHandler->TIMx_Config.TIMx_Prescaler));

	// To configures the pre-scaler value
	pTimerHandler->pTIMx->PSC = pTimerHandler->TIMx_Config.TIMx_Prescaler -1; // From 0 to prescaler-1

}

/*
 * This function configures the count limit of the Timer to generate an "update" event (when it is UP_COUNT)
 * or configures the starting count value (when it is DOWN_COUNT).
 */
void timer_set_period(Timer_Handler_t *pTimerHandler){

	// To verify if the period value generated is allowed
	assert_param(IS_TIMER_PERIOD(pTimerHandler->TIMx_Config.TIMx_Prescaler));// IT MUST BE TIMx_Period RIGHT???

	// To configure the auto-reload value
	pTimerHandler->pTIMx->ARR = pTimerHandler->TIMx_Config.TIMx_Period;

}

/*
 * Upcounter or Downcounter
 */
void timer_set_mode(Timer_Handler_t *pTimerHandler){

	//To verify if the mode is right
	assert_param(IS_TIMER_MODE(pTimerHandler->TIMx_Config.TIMx_mode));

	//To verify which is the desire mode to be configure
	if(pTimerHandler->TIMx_Config.TIMx_mode ==TIMER_UP_COUNTER){
		// Configure it as Upcounter DIR = 0
		pTimerHandler->pTIMx->CR1 &= ~TIM_CR1_DIR;
	}
	else{
		// Configure it as Downcounter DIR = 1
		pTimerHandler->pTIMx->CR1 |= TIM_CR1_DIR;
	}
}

void timer_config_interrupt(Timer_Handler_t *pTimerHandler){

	// To verify the possible configured value
	assert_param(IS_TIMER_INTERRUP(pTimerHandler->TIMx_Config.TIMx_InterruptEnable));

	if(pTimerHandler->TIMx_Config.TIMx_InterruptEnable == TIMER_INT_ENABLE){

		// Activate the interruption due to the used Timerx
		pTimerHandler->pTIMx->DIER |= TIM_DIER_UIE;

		//Activate the NVIC channel to read the interruption
		/* TIM2 toTIM3 */
		if(pTimerHandler->pTIMx == TIM2){ //32-bit
			__NVIC_SetPriority(TIM2_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM2_IRQn);
		}
		else if(pTimerHandler->pTIMx == TIM3){ //16-bit
			__NVIC_SetPriority(TIM3_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM3_IRQn);
		}
		else if(pTimerHandler->pTIMx == TIM4){ //16-bit
			__NVIC_SetPriority(TIM4_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM4_IRQn);
		}
		else if(pTimerHandler->pTIMx == TIM5){ //32-bit
			__NVIC_SetPriority(TIM5_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM5_IRQn);
		}

		/* TIM9 to TIM11 */
		else if(pTimerHandler->pTIMx == TIM9){
			__NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
		}
		else if(pTimerHandler->pTIMx == TIM10){
			__NVIC_SetPriority(TIM1_UP_TIM10_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
		}
		else if(pTimerHandler->pTIMx == TIM11){
			__NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, (uint32_t) (pTimerHandler->TIMx_Config.interruptPriority));
			__NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
		}
		//repeat for the other general timers
		else{
			__NOP();
		}
	}
}


void timer_SetState(Timer_Handler_t *pTimerHandler, uint8_t newState){

	// To verify if the given state is the right one
	assert_param(IS_TIMER_STATE(newState));

	/* 4. Restart counter register*/
	pTimerHandler->pTIMx->CNT = 0;

	if(newState == TIMER_ON){
		/* 5a. Activate the Timer (CNT must begin to count) */
		pTimerHandler->pTIMx->CR1 |= TIM_CR1_CEN;
	}
	else{
		/* 5b. Deactivate the Timer (CNT must stop) */
		pTimerHandler->pTIMx->CR1 &= ~TIM_CR1_CEN;
	}
}


/*******************************************************************/
/* TIM2 to TIM5 */
__attribute__((weak)) void Timer2_Callback(void){
	__NOP();
}
__attribute__((weak)) void Timer3_Callback(void){
	__NOP();
}
__attribute__((weak)) void Timer4_Callback(void){
	__NOP();
}
__attribute__((weak)) void Timer5_Callback(void){
	__NOP();
}

/* TIM9 to TIM11 */
__attribute__((weak)) void Timer9_Callback(void){
	__NOP();
}
__attribute__((weak)) void Timer10_Callback(void){
	__NOP();
}
__attribute__((weak)) void Timer11_Callback(void){
	__NOP();
}

/*
 * This is the function where the system is targeting on the interrution's vector.
 *
 *
 * It must be used by using exactly the defined name in the interruption's vector!!!
 *
 *
 * After correctly done, the system target to this function and when the interruption
 * is called the system jumps into this memory place (address) immediately.
 */
/* TIM2 to TIM5 */
void TIM2_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM2->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer2_Callback();
}
void TIM3_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM3->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer3_Callback();
}
void TIM4_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM4->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer4_Callback();
}
void TIM5_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM5->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer5_Callback();
}

/* TIM9 to TIM11 */
void TIM1_BRK_TIM9_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM9->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer9_Callback();
}
void TIM1_UP_TIM10_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM10->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer10_Callback();
}
void TIM1_TRG_COM_TIM11_IRQHandler(void){
	// To clean the flag who says when the interruption is generated
	TIM11->SR &= ~TIM_SR_UIF;

	// Call the function who has to do something at this interruption
	Timer11_Callback();
}
