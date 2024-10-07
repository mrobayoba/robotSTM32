/*
 * systick_driver_hal.c
 *
 *  Created on: Oct 20, 2023
 *      Author: mrobayoba
 */

#include "../Inc/SysTick_driver_hal.h"

#include <stm32f4xx.h>

uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;

static void SysTick_interrupt(Systick_Handler_t *SysTick_Handler);

void SysTick_config_ms(Systick_Handler_t *SysTick_Handler){
	// Restart the ticks value
	ticks = 0;
	// Configure the inner clock as Timer Clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// Loading the limit value (1ms)
	if(SysTick_Handler->Systick_Config.Systick_Reload == SYSTICK_LOAD_16MHz_1ms){ // modify to 16 (1us)
	// HSI Clock case (16MHz)
		// Reload register value starts at 0 to reload-1
		SysTick->LOAD = (SysTick_Handler->Systick_Config.Systick_Reload)-1;
	}
	// HSE Clock case NOT IN USE
//		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms-1;
//		break;
	else if(SysTick_Handler->Systick_Config.Systick_Reload == SYSTICK_LOAD_100MHz_1ms){
			// PLL Clock case at 100MHz NOT IN USE
			SysTick->LOAD = SysTick_Handler->Systick_Config.Systick_Reload-1;
	}
//		break;
	// Any other case... NOT IN USE
//	default:
//		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms-1;
//		break;

	// Clear SysTick current value
	SysTick->VAL = 0;

	// Deactivate global interruptions
	__disable_irq();

	// Set Interruptions
	SysTick_interrupt(SysTick_Handler);

	// Activate global interruptions
	__enable_irq();

	// Activate Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
static void SysTick_interrupt(Systick_Handler_t *SysTick_Handler){

	if (SysTick_Handler->Systick_Config.Systick_IntState) {
		// Assign the interrupt on NVIC
		NVIC_EnableIRQ(SysTick_IRQn);

		// Activate the interruption due to SysTick zero-counting
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	}
	else{

		// Unassign the interrupt on NVIC
		NVIC_DisableIRQ(SysTick_IRQn);

		// Deactivate the interruption
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	}
}
uint64_t SysTick_get_ms(void){
	return ticks;
}

void SysTick_delay_ms(uint32_t wait_time_us){
	// Capture the first time value to compare
	ticks_start = SysTick_get_ms();

	//Capture the second time value to compare
	ticks_counting = SysTick_get_ms();

	// Compare if counting value is less than start+wait
	// Update counting value
	// Repeat this operation until counting is greater
	while (ticks_counting < (ticks_start + (uint64_t)wait_time_us)){
		ticks_counting = SysTick_get_ms();
	}

}
__attribute__ ((weak)) void SysTick_callback(void){
	__NOP();
}

void SysTick_Handler(void){
	// Verify if the interruption was launched
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		// Clear flag
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		// Increase in 1 the counter
		ticks++;
		// Call callback
		SysTick_callback();
	}
}


