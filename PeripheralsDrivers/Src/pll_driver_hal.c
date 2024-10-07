/*
 * pll_driver_hal.c
 *
 *  Created on: Nov 16, 2023
 *      Author: mrobayoba
 */

#include "../Inc/pll_driver_hal.h"

#include <stm32f4xx.h>

void pllConfig(void){
	//(RCC_PLLCFGR)
	RCC->PLLCFGR &=~RCC_PLLCFGR_PLLSRC; // Set HSI as clock

	RCC->PLLCFGR &=~RCC_PLLCFGR_PLLM; // Clear PLLM bits
	RCC->PLLCFGR |= 8UL << RCC_PLLCFGR_PLLM_Pos;

	//  Set PLLP and PLLN
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;

	RCC->PLLCFGR |= 100UL << RCC_PLLCFGR_PLLN_Pos;

	// Set VOS ((PWR_CR)
	PWR->CR &= ~PWR_CR_VOS;
	PWR->CR |= PWR_CR_VOS;

	// Set PLLON (RCC_CR)
	RCC->CR &= ~RCC_CR_PLLON;
	RCC->CR |= RCC_CR_PLLON;

	// Read PLLRDY until its SET
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}
	// Set prescalers (RCC_CFGR) APB 1 and 2 and FLASH latency (FLASH_ACR)
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (RCC_CFGR_PPRE1_2);

	FLASH->ACR &= ~(0XF << FLASH_ACR_LATENCY_Pos);
	FLASH->ACR |= (3 << FLASH_ACR_LATENCY_Pos);

	// Set SW mux as PLLCLK (RCC_CFGR)
	RCC->CFGR |= (RCC_CFGR_SW_1);

}

