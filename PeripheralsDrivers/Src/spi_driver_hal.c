/*
 * spi_driver_hal.C
 *
 *  Created on: Nov 10, 2023
 *      Author: mrobayoba
 */

#include "spi_driver_hal.h"

void spi_config(SPI_Handler_t ptrHandlerSPI){

	/*1. First activate clock signal for this peripheral */
	if(ptrHandlerSPI.ptrSPIx	== SPI1){
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	}
	else if(ptrHandlerSPI.ptrSPIx	== SPI2){
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	}
	else if(ptrHandlerSPI.ptrSPIx	== SPI3){
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}
	else if(ptrHandlerSPI.ptrSPIx	== SPI4){
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
	}
	else if(ptrHandlerSPI.ptrSPIx	== SPI5){
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}

	/*2. Clean the control register */
	ptrHandlerSPI.ptrSPIx->CR1	= 0x00;

	/*3. Configure the SPI clock speed (baudrate)*/
	ptrHandlerSPI.ptrSPIx->CR1 |= (ptrHandlerSPI.SPI_Config.SPI_baudrate << SPI_CR1_BR_Pos);

	/*4. Configure mode, that means polarity and phase*/
	switch(ptrHandlerSPI.SPI_Config.SPI_mode){
	case SPI_POL_PHA_00:
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		break;

	case SPI_POL_PHA_01:
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPHA);
		break;

	case SPI_POL_PHA_10:
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPOL);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		break;

	case SPI_POL_PHA_11:
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPOL);
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPHA);
		break;

	default: // Case 00 by default
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		break;
	}

	/*5. Configures full-duplex or receive-only*/

	if(ptrHandlerSPI.SPI_Config.SPI_fullDuplexEnable == SPI_FULL_DUPPLEX){
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_RXONLY); // selecct full-duplex
		/* Select mono-directional, each line has only one direction*/
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_BIDIMODE);
	}
	else{ // selecct RX-ONLY
		ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_RXONLY;
	}

	/*6. Transfer mode, MSB-first */ //create a selection function
	ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_LSBFIRST);

	/*7. Configure as master */ //create a selection function
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_MSTR;

	/*8. Configure data format (8-bit or 16-bit) */
	if(ptrHandlerSPI.SPI_Config.SPI_dataSize == SPI_DATASIZE_8_BIT){
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_DFF);
	}
	else{
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_DFF);
	}

	/*9. Configure manual slave selection*/
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SSM;
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SSI;

	/*10. Activate SPI peripheral*/
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SPE;

}

/*
 *
 */

void spi_sendData(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize){

	uint8_t auxData;
	(void) auxData; // to avoid auxData not in use warning!

	while(dataSize > 0){
		//Wait until buffer is empty...
		while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)){__NOP();}

		// Send the data targeted by the pointer
		ptrHandlerSPI.ptrSPIx->DR = (uint8_t) (0xFF & *ptrData);

		// Update the pointer and the remaining data to send
		ptrData++;
		dataSize--;
	}

	// Wait until the buffer is empty...
	while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)){__NOP();}

	// Wait until BUSY flag get down
	while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_BSY)){__NOP();}

	// The overrun flag must be get down by read DR and then read SR of the SPI module
	auxData = ptrHandlerSPI.ptrSPIx->DR;
	auxData = ptrHandlerSPI.ptrSPIx->SR;

}

void spi_receiveData(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize){

	while(dataSize){
		// Wait until the buffer is empty...
		while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)){__NOP();}

		// Send a dummy value
		ptrHandlerSPI.ptrSPIx->DR = 0x00;

		// Wait until the buffer have a data to read
		while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_RXNE)){__NOP();}

		// Load the value to the pointer
		*ptrData = ptrHandlerSPI.ptrSPIx->DR;

		// Update the pointer and dataSize
		ptrData++;
		dataSize--;
	}
}

/*
 *
 */

void spi_selectSlave(SPI_Handler_t* ptrHandlerSPI){
	gpio_WritePin(&ptrHandlerSPI->SPI_slavePin, RESET); // RESET VALUE TO SELECT SLAVE!
}
void spi_unSelectSlave(SPI_Handler_t* ptrHandlerSPI){
	gpio_WritePin(&ptrHandlerSPI->SPI_slavePin, SET);
}
