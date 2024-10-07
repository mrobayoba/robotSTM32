/*
 * usart_driver_hal.c
 *
 *  Created on: Oct 5, 2023
 *      Author: mrobayoba (Transcriber)
 */

#include "usart_driver_hal.h"

#include "stm32f4xx.h"


uint16_t auxRxData = 0;

/* === Headers for private functions === */
static void usart_enable_clock_peripheral(USART_Handler_t *ptrUsartHandler);
static void usart_config_parity(USART_Handler_t *ptrUsartHandler);
static void usart_config_datasize(USART_Handler_t *ptrUsartHandler);
static void usart_config_stopbits(USART_Handler_t *ptrUsartHandler);
static void usart_config_baudrate(USART_Handler_t *ptrUsartHandler);
static void usart_config_mode(USART_Handler_t *ptrUsartHandler);
static void usart_config_interrupt(USART_Handler_t *ptrUsartHandler);
static void usart_enable_peripheral(USART_Handler_t *ptrUsartHandler);



/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void usart_Config(USART_Handler_t *ptrUsartHandler){

	//*ptrAuxUsartHandler = *ptrUsartHandler;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periférico */
	usart_enable_clock_peripheral(ptrUsartHandler);

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = RESET;
	ptrUsartHandler->ptrUSARTx->CR2 = RESET;

	// Limpiamos el registro DR
	//ptrUsartHandler->ptrUSARTx->DR = 0;

	// 2.2 Configuracion del Parity:
	usart_config_parity(ptrUsartHandler);

	// 2.3 Configuramos el tamaño del dato
	usart_config_datasize(ptrUsartHandler);

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	usart_config_stopbits(ptrUsartHandler);

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	usart_config_baudrate(ptrUsartHandler);

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	usart_config_mode(ptrUsartHandler);

	// 2.8 Verificamos la configuración de las interrupciones
	usart_config_interrupt(ptrUsartHandler);

	// 2.7 Activamos el modulo serial.
	usart_enable_peripheral(ptrUsartHandler);

	/* x. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}


/**/
static void usart_enable_clock_peripheral(USART_Handler_t *ptrUsartHandler){
	/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */

    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){

		RCC->APB2ENR |= (RCC_APB2ENR_USART1EN);

	}
	
    /* 1.2 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART2){

		RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);

	}
    
    /* 1.3 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){

		RCC->APB2ENR |= (RCC_APB2ENR_USART6EN);

	}
}

/**
 *
 */
static void usart_config_parity(USART_Handler_t *ptrUsartHandler){
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.parity != USART_PARITY_NONE){


		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.parity == USART_PARITY_EVEN){

			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler->ptrUSARTx->CR1 &= ~(0x1UL << USART_CR1_PS_Pos);
			
		}else{

			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_PS);
		}
	}else{

		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PCE);

	}
}

/**
 * Esta funcion debe estar relacionada con el parity.
 * Si NO hay parity,el tamaño debe ser 8bit.
 * SI HAY parity, el tamaño debe ser 9 bit.
 */
static void usart_config_datasize(USART_Handler_t *ptrUsartHandler){
	// Verificamos cual es el tamaño de dato que deseamos
	if(ptrUsartHandler->USART_Config.datasize == USART_DATASIZE_8BIT){

		// Verificamos si se esta trabajando o no con paridad
		if(ptrUsartHandler->USART_Config.parity == USART_PARITY_NONE){

			// Disable parity
			ptrUsartHandler->ptrUSARTx->CR1 &= ~(0x1UL << USART_CR1_PCE_Pos); // M-bit = 0
			// Deseamos trabajar con datos de 8 bits
			ptrUsartHandler->ptrUSARTx->CR1 &= ~(0x1UL << USART_CR1_M_Pos); // 8-bit data size

		}
		else{

			// Enable parity
			ptrUsartHandler->ptrUSARTx->CR1 |= (0x1UL << USART_CR1_PCE_Pos); // M-bit = 1
			// Si la paridad esta activa, debemos incluir un bit adicional
			ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_M); // 9-bit datasize

		}

	}else{

		// Disable parity
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(0x1UL << USART_CR1_PCE_Pos); // M-bit = 0
		// Deseamos trabajar con datos de 9 bits
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_M);
	}
}

/**
 *
 */
static void usart_config_stopbits(USART_Handler_t *ptrUsartHandler){

	switch(ptrUsartHandler->USART_Config.stopbits){
	case USART_STOPBIT_1: {
		// Debemos cargar el valor 0b00 en los dos bits de STOP
		// Clear bits 13 and 12, JUST AS A GOOD PRACTICE!
		ptrUsartHandler->ptrUSARTx->CR2 &=~(USART_CR2_STOP); // write 0 in bits 13 and 12
		/* the line above writes 00 and that means 1 stopbit */

		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemos cargar el valor 0b01 en los dos bits de STOP
		// Clear bits 13 and 12
		ptrUsartHandler->ptrUSARTx->CR2 &=~(USART_CR2_STOP);

		// Write 01 on bits 13-12
		ptrUsartHandler->ptrUSARTx->CR2 |=(0x1UL << USART_CR2_STOP_Pos);

		break;
	}
	case USART_STOPBIT_2: {
		// Debemos cargar el valor 0b10 en los dos bits de STOP
		// Clear bits 13 and 12
		ptrUsartHandler->ptrUSARTx->CR2 &=~(USART_CR2_STOP);

		// Write 10 on bits 13-12
		ptrUsartHandler->ptrUSARTx->CR2 |=(0x2UL << USART_CR2_STOP_Pos);

		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemos cargar el valor 0b11 en los dos bits de STOP
		// Clear bits 13 and 12
		ptrUsartHandler->ptrUSARTx->CR2 &=~(USART_CR2_STOP);

		// Write 11 on bits 13-12
		ptrUsartHandler->ptrUSARTx->CR2 |=(0x3UL << USART_CR2_STOP_Pos);

		break;
	}
	default: {
		// En el caso por defecto seleccionamos 1 bit de parada
		// Clear bits 13 and 12
		ptrUsartHandler->ptrUSARTx->CR2 &=~(USART_CR2_STOP); // write 0 in bits 13 and 12
		/* the line above writes 00 and that means 1 stopbit */

		break;
	}
	}
}

/**
 * Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
 */
static void usart_config_baudrate(USART_Handler_t *ptrUsartHandler){
	// Caso para configurar cuando se trabaja con el Cristal Interno
	switch(ptrUsartHandler->USART_Config.baudrate){
		case USART_BAUDRATE_9600:
		{
			// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
			// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3 = 0x3
			// Valor a cargar 0x683
			// Configurando el Baudrate generator para una velocidad de 9600bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
			break;

		}
		case USART_BAUDRATE_19200:
		{
			if ((RCC->CFGR & RCC_CFGR_SW_PLL) && (ptrUsartHandler->ptrUSARTx == USART1)) {
				// El valor a cargar es 325.52083 -> Mantiza = 325,fraction = 0.52083
				// Mantiza =  = 0x145, fraction = 16 * 0.2535 = 8 = 0x8
				// Valor a cargar 0x1458
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
			}
			else{
				// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
				// Mantiza =  = 0x34, fraction = 16 * 0.0625 = 1 = 0x1
				// Valor a cargar 0x341
				ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
			}

			break;
		}
		case USART_BAUDRATE_115200:
		{
			if ((RCC->CFGR & RCC_CFGR_SW_PLL) && (ptrUsartHandler->ptrUSARTx == USART2)) {
				// El valor a cargar es 27.1267 -> Mantiza = 27,fraction = 0.1267
				// Mantiza =  = 0x1B, fraction = 16 * 0.1267 = 2 = 0x2
				// Valor a cargar 0x01B2
				ptrUsartHandler->ptrUSARTx->BRR = 0x01B2;
			}
			else{
				// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
				// Mantiza =  = 0x8, fraction = 16 * 0.6875 = 11 = 0xB
				// Valor a cargar 0x008B
				ptrUsartHandler->ptrUSARTx->BRR = 0x008B;
			}
			break;
		}
		case USART_BAUDRATE_230400:
		{
			// El valor a cargar es 4.3125 -> Mantiza = 4,fraction = 0.3125
			// Mantiza =  = 0x4, fraction = 16 * 0.3125 = 5 = 0x5
			// Valor a cargar 0x45
			ptrUsartHandler->ptrUSARTx->BRR = 0x0045;
			break;
		}

		default:
			// Configurando el Baudrate generator para una velocidad de 115200bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x008B;
			break;
		}
}

/**
 *
 */
static void usart_config_mode(USART_Handler_t *ptrUsartHandler){
	switch(ptrUsartHandler->USART_Config.mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_TE);
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		ptrUsartHandler->ptrUSARTx->CR1 |= (0x3UL<<USART_CR1_RE_Pos); //writes 11 on bits 3-2
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(0x3UL<<USART_CR1_RE_Pos); //writes 00 on bits 3-2
//		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE; //What is this for?
		break;
	}
	default:

		break;
	}
}


/******/
static void usart_config_interrupt(USART_Handler_t *ptrUsartHandler){

	// 2.8a Interrupción por recepción
	if(ptrUsartHandler->USART_Config.enableIntRX == USART_RX_INTERRUP_ENABLE){
		// Como está activada, debemos configurar la interrupción por recepción
		/* Debemos activar la interrupción RX en la configuración del USART */
		ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_RXNEIE);

		/* Debemos matricular la interrupción en el NVIC */
		/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */
		if(ptrUsartHandler->ptrUSARTx == USART1){

			__NVIC_EnableIRQ(USART1_IRQn);
			__NVIC_SetPriority(USART1_IRQn, (uint32_t) (ptrUsartHandler->USART_Config.interruptPriority));
		}

		else if(ptrUsartHandler->ptrUSARTx == USART2){

			__NVIC_EnableIRQ(USART2_IRQn);
			__NVIC_SetPriority(USART2_IRQn, (uint32_t) (ptrUsartHandler->USART_Config.interruptPriority));
		}

		else if(ptrUsartHandler->ptrUSARTx == USART6){

			__NVIC_EnableIRQ(USART6_IRQn);
			__NVIC_SetPriority(USART6_IRQn, (uint32_t) (ptrUsartHandler->USART_Config.interruptPriority));
		}
	}
}

/**
 *
 */
void usart_config_newInterrupt(USART_Handler_t *ptrUsartHandler, uint8_t newPriority){

	ptrUsartHandler->USART_Config.interruptPriority = newPriority;

	__disable_irq();
	usart_config_interrupt(ptrUsartHandler);
	__enable_irq();
}

/**
 *
 */
static void usart_enable_peripheral(USART_Handler_t *ptrUsartHandler){

	if(ptrUsartHandler->USART_Config.mode != USART_MODE_DISABLE){

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}
}

/*
 * función para escribir un solo char
 */
uint16_t usart_WriteChar(USART_Handler_t *ptrUsartHandler, uint16_t dataToSend){

	/**
	 * This function writes 9 or 8 bits (one char) from the dataToSend variable
	 * to USART DATA REGISTER
	 */

	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

//	ptrUsartHandler->ptrUSARTx->DR &= ~(USART_DR_DR); // NOT NEED IT. IT CLEARS ITSELF.
	ptrUsartHandler->ptrUSARTx->DR = (dataToSend); //can i write only the first 9 bits from a 16 bit variable?

	return dataToSend;
}

/*
 *
 */
void usart_writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend ){
	// change this function to allow to send null characters

	int Char_counter = 0;

	char Char;

	while(1){

		Char = *(msgToSend+Char_counter);

//		if (ptrUsartHandler->ptrUSARTx->SR & USART_SR_TC) { // Is Transmission Complete Register (TC) necessary?
			if (Char != '\0') {

				usart_WriteChar(ptrUsartHandler,(uint16_t) Char);

				Char_counter++;
			}
			else{

				break;

			}
//		}

	}

}

char usart_getRxData(void){

	return auxRxData;

}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART2_IRQHandler(void){

	// Evaluamos si la interrupción que se dio es por RX
	if(USART2->SR & USART_SR_RXNE){

		auxRxData = USART2->DR; //from 32-bit DR to 16-bit auxRxData

		usart2_RxCallback();
	}

	else if(USART2->SR & USART_SR_TXE){
		__NOP();
	}

	else if(USART2->SR & USART_SR_PE){
		__NOP();
	}

}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART6_IRQHandler(void){
	// Evaluamos si la interrupción que se dio es por RX
	if(USART6->SR & USART_SR_RXNE){

		auxRxData = USART6->DR; //from 32-bit DR to 16-bit auxRxData

		usart6_RxCallback();
	}

	else if(USART6->SR & USART_SR_TXE){
		__NOP();
	}

	else if(USART6->SR & USART_SR_PE){
		__NOP();
	}
}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART1_IRQHandler(void){
	// Evaluamos si la interrupción que se dio es por RX
	if(USART1->SR & USART_SR_RXNE){

		auxRxData = USART1->DR; //from 32-bit DR to 16-bit auxRxData

		usart1_RxCallback();
	}

	else if(USART1->SR & USART_SR_TXE){
		__NOP();
	}

	else if(USART1->SR & USART_SR_PE){
		__NOP();
	}
}


__attribute__((weak)) void usart1_RxCallback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart2_RxCallback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart6_RxCallback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

