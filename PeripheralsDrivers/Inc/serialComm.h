/*
 * serialComm.h
 *
 *  Created on: Mar 21, 2024
 *      Author: matto
 */

#ifndef SERIALCOMM_H_
#define SERIALCOMM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32_assert.h"


#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "timer_driver_hal.h"


typedef struct{

	USART_Handler_t 	*commSerial;
	GPIO_Handler_t 		*pinTx;
	GPIO_Handler_t 		*pinRx;

}serialComm_Handler_t;

// Public Function Prototypes
void serialCommInit(serialComm_Handler_t *ptrserialComm_Handler);				// To initialize the USARTx and GPIOs Tx and Rx
void communication_f(serialComm_Handler_t *ptrserialComm_Handler, char * ptrCommandBuffer); 		// To evaluate each keyboard entry
void clear_string(char *string); 	// To clear char array
__attribute__ ((weak)) void help_menu(serialComm_Handler_t *ptrserialComm_Handler);
__attribute__ ((weak)) void analyzeCommand(serialComm_Handler_t *ptrserialComm_Handler, char * ptrCommandBuffer);

#endif /* SERIALCOMM_H_ */
