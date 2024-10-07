/*
 * pll_driver_hal.h
 *
 *  Created on: Nov 16, 2023
 *      Author: mrobayoba
 */

#ifndef PLL_DRIVER_HAL_H_
#define PLL_DRIVER_HAL_H_

#include <stm32f4xx.h>
/* Constants */
enum{
	HSI_CLOCK_CONFIGURE,
	HSE_CLOCK_CONFIGURE,
	PLL_CLOCK_CONFIGURE
};

/* Public function prototypes */
void pllConfig(void);

#endif /* PLL_DRIVER_HAL_H_ */
