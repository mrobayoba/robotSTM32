/*
 * adc_driver_hal.c
 *
 *  Created on: Oct 12, 2023
 *      Author: mrobayoba
 */

#include "adc_driver_hal.h"

#include "stm32f4xx.h"

#include "gpio_driver_hal.h"
#include "NVIC_FreeRTOS_priorities.h"
#include "stm32_assert.h"

/* === Headers for private functions === */
static void adc_enable_clock_peripheral(void);
static void adc_set_resolution(ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_multi_set_sampling_and_hold(ADC_Config_t *adcConfig, uint8_t channel_QTY);
static void adc_single_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_single_set_one_channel_sequence(ADC_Config_t *adcConfig);
static void adc_multi_set_channel_sequence(ADC_Config_t *adcConfig, uint8_t channel_QTY);
static void adc_config_interrupt(ADC_Config_t *adcConfig);

/* Variables needed to the correct performance of the driver*/
GPIO_Handler_t		handlerADCPin	=	{0};
uint16_t			adcRawData		=	0;


/*
 *
 */
void adc_ConfigSingleChannel(ADC_Config_t *adcConfig){

	/* 1. Configures PinX to works as analog channel */
	adc_ConfigAnalogPin(adcConfig->channel);

	/* 2. Activate clock signal for the ADC */
	adc_enable_clock_peripheral();

	// Clear registers before starts the configuration
	ADC1->CR1	=	RESET;
	ADC1->CR2	=	RESET;

	/* Time to start with the ADC1 configuration */

	/* 3. ADC Resolution */
	adc_set_resolution(adcConfig);

	/* 4. Configures the Scan Mode as deactivated */
	adc_ScanMode(SCAN_OFF);

	/* 5.Configures the data alignment (left or right) */
	adc_set_alignment(adcConfig);

	/* 6. Deactivate the continuous mode */
	adc_StopContinuousConv();

	/* 7. Here should'll be configured the sampling */
	adc_single_set_sampling_and_hold(adcConfig);

	/* 8. Configures the sequence and how many elements will have the sequences */
	adc_single_set_one_channel_sequence(adcConfig);

	/* 9. Configures the ADC prescaler at 2:1 (the fastest one) */
	ADC->CCR &= ~ADC_CCR_ADCPRE;

	/* 10. Deactivate global interruptions */
	__disable_irq();

	/* 11. Configures the interruption (if it is able), even inscribe/remove the interruption from NVIC */
	adc_config_interrupt(adcConfig);

	/* 12. Activate ADC module */
	adc_peripheralOnOFF(ADC_ON);

	/* 13. Activate the global interruptions */
	__enable_irq();

}

/*
 * Enable Clock signal for ADC peripheral
 */
static void adc_enable_clock_peripheral(void){

	RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN);

}

/*
 * Configures the resolution for the conversion
 */
static void adc_set_resolution(ADC_Config_t *adcConfig){

	switch(adcConfig->resolution){

	case RESOLUTION_12_BIT:
		ADC1->CR1 &= ~(ADC_CR1_RES);
		break;

	case RESOLUTION_10_BIT:
		ADC1->CR1 |= (RESOLUTION_10_BIT << ADC_CR1_RES_Pos);
		break;

	case RESOLUTION_8_BIT:
		ADC1->CR1 |= (RESOLUTION_8_BIT << ADC_CR1_RES_Pos);
		break;

	case RESOLUTION_6_BIT:
		ADC1->CR1 |= (ADC_CR1_RES);
		break;

	}

}

/*
 * Set the number alignment: left or right
 */
static void adc_set_alignment(ADC_Config_t *adcConfig){

	switch(adcConfig->dataAlignment){

	case ALIGNMENT_RIGHT:
		ADC1->CR2 &=~(ADC_CR2_ALIGN);
		break;

	case ALIGNMENT_LEFT:
		ADC1->CR2 |=(ADC_CR2_ALIGN);
		break;

	}

}

/*
 * Set sampling and hold (of the capacitor) load time value
 */
static void adc_single_set_sampling_and_hold(ADC_Config_t *adcConfig){

	switch(adcConfig->channel){
		case CHANNEL_0 ... CHANNEL_9:
		// channel * 3 is equivalent to the position address to shift
			ADC1->SMPR2 &= ~(0x7UL << (3 * (adcConfig->channel)));
			ADC1->SMPR2 |= ((adcConfig->samplingPeriod) << (3 * (adcConfig->channel)));

			break;

		case CHANNEL_10 ... CHANNEL_15:
		// (channel - 10) * 3 is equivalent to the position address to shift
			ADC1->SMPR1 &= ~(0x7UL << (3 * ((adcConfig->channel) - 10)));
			ADC1->SMPR1 |= ((adcConfig->samplingPeriod) << (3 * ((adcConfig->channel) - 10)));

			break;

		default:
			__NOP();
			break;

	}

}

/*
 * Configures the number of elements in the sequence (only one element)
 * Configures which channel will acquire the ADC signal
 */
static void adc_single_set_one_channel_sequence(ADC_Config_t *adcConfig){

	ADC1->SQR1 &= ~(ADC_SQR1_L); // To get 1 conversion from the channel conversion sequence
//	ADC1->SQR1 |= (0x1UL << ADC_SQR1_L_Pos); // To change the length value NOT IN USE
	ADC1->SQR3 &= ~(0x1F << ADC_SQR3_SQ1_Pos); // To clear the first conversion of the sequence
	ADC1->SQR3 |= ((adcConfig->channel) << ADC_SQR3_SQ1_Pos); // To set the channel 0 to the first conversion of the sequence

}

/*
 * Configures the interruption enable register and the activation of NVIC
 */
static void adc_config_interrupt(ADC_Config_t *adcConfig){

	/* End of conversion of a regular group Interrupt
	 * Event flag : EOC at ADC Status Register
	 * Enable control bit: EOCIE at ADC Control Register 1
	 */
	if (adcConfig->interruptEnable) {
		ADC1->CR1 |= (ADC_CR1_EOCIE); // Enable End Of Conversion Interrupt

		//To set NVIC interrupt priority
		__NVIC_SetPriority(ADC_IRQn, (uint32_t) (adcConfig->interruptPriority));

		// To Activate the ADC global interruptions
		__NVIC_EnableIRQ(ADC_IRQn);
	}

}

/*
 * Manage the activation and deactivation of the ADC module from the CR2 ADC register
 */
void adc_peripheralOnOFF(uint8_t state){

	/* ADON bit from ADC_CR2 Register
	 * use ADC_OFF or ADC_ON as argument
	 */

	if(state){
		ADC1->CR2 |= (ADC_CR2_ADON); // ADC_ON

	}
	else{
		ADC1->CR2 &= ~(ADC_CR2_ADON); // ADC_OFF

	}

}

/*
 * Enables and disables the Scan mode...
 * Works hand by hand with the multi-channel sequence
 * It is not necessary for the one simple channel case
 */
void adc_ScanMode(uint8_t state){

	/* SCAN bit from ADC_CR1 Register
	 * use SCAN_OFF or SCAN_ON as argument
	 */

	if(state){
		ADC1->CR1 |= (ADC_CR1_SCAN); // Scan mode Enable

	}
	else{
		ADC1->CR1 &= ~(ADC_CR1_SCAN); // Scan mode disable

	}


}

/*
 * Function to starts the ADC simple conversion
 */
void adc_StartSingleConv(void){
	/* Set CONT bit as 0 (single_mode) */
	ADC1->CR2 &= ~(ADC_CR2_CONT); // Set Conversion mode as single conversion

	/* SWSTART bit of ADC_CR2 */
	ADC1->CR2 |= (ADC_CR2_SWSTART); // Start conversion

}

/*
 * Function to starts the ADC continuous conversion
 */
void adc_StartContinuousConv(void){
	/* Set CONT bit as 1 (continuous_mode) */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* SWSTART bit from ADC_CR2 */
	ADC1->CR2 |= (ADC_CR2_SWSTART); // Starts by set as 1

}

/*
 * Function to stops the ADC continuous conversion
 */
void adc_StopContinuousConv(void){
	/* Stops by clear CONT bit from ADC_CR2 */
	ADC1->CR2 &= ~(ADC_CR2_CONT); // Stops by set it as 0

}

/*
 * Function to returns the last acquire data by the ADC
 */
uint16_t adc_GetValue(void){

	adcRawData = ADC1->DR; // Assign CDR value to adcRawData variable

	return adcRawData;

}

/*
 * This is the ISR of the interruption by ADC conversion
 */
void ADC_IRQHandler(void){

	//First, verify the EOC flag
	if((ADC1->SR & (ADC_SR_EOC))){
		//Call the CallBack at the end of the conversion
		adc_CompleteCallback();

	}

}

__attribute__((weak)) void adc_CompleteCallback(void){
	__NOP();
}

/* This function configures which Pin is desire to works as ADC */
void adc_ConfigAnalogPin(uint8_t adcChannel){
	switch(adcChannel){

	case CHANNEL_0 ... CHANNEL_7:
		handlerADCPin.pGPIOx						=	GPIOA;
		handlerADCPin.pinConfig.GPIO_PinMode		=	GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber		=	adcChannel; // Channel_number equivalent to Pin_number
		handlerADCPin.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

		break;

	case CHANNEL_8:
		handlerADCPin.pGPIOx						=	GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode		=	GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber		=	PIN_0;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

		break;

	case CHANNEL_9:
		handlerADCPin.pGPIOx						=	GPIOB;
		handlerADCPin.pinConfig.GPIO_PinMode		=	GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber		=	PIN_1;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

		break;

	case CHANNEL_10 ... CHANNEL_15:
		handlerADCPin.pGPIOx						=	GPIOC;
		handlerADCPin.pinConfig.GPIO_PinMode		=	GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinNumber		=	adcChannel-10; //Channel_number - 10 == Pin_number
		handlerADCPin.pinConfig.GPIO_PinPuPdControl	=	GPIO_PUPDR_NOTHING;

		break;

	default:
		__NOP();
		break;

	}

	gpio_Config(&handlerADCPin);

}

/* Configuration to do conversions on multiple channels at a specified order */
void adc_ConfigMultiChannel(ADC_Config_t *adcConfig, uint8_t channel_QTY){
	/* 1. Configures PinX to works as analog channel */
		for (int8_t i = 0; i < channel_QTY; i++) {
			adc_ConfigAnalogPin((adcConfig+i)->channel);
		}

		/* 2. Activate clock signal for the ADC */
		adc_enable_clock_peripheral();

		// Clear registers before starts the configuration
		ADC1->CR1	=	RESET;
		ADC1->CR2	=	RESET;

		/* Time to start with the ADC1 configuration */

		/* 3. ADC Resolution */
		adc_set_resolution(adcConfig);

		/* 4. Configures the Scan Mode as activated */
		adc_ScanMode(SCAN_ON);
		// SET EOCS bit from CR2 to Call an Interrupt at each channel conversion
		ADC1->CR2 |= ADC_CR2_EOCS;

		/* 5.Configures the data alignment (left or right) */
		adc_set_alignment(adcConfig);

		/* 6. Deactivate the continuous mode */
		adc_StopContinuousConv();

		/* 7. Here should'll be configured the sampling */
		adc_multi_set_sampling_and_hold(adcConfig,channel_QTY);

		/* 8. Configures the sequence and how many elements will have the sequences */
		adc_multi_set_channel_sequence(adcConfig,channel_QTY);

		/* 9. Configures the ADC prescaler at 2:1 (the fastest one) */
		ADC->CCR &= ~ADC_CCR_ADCPRE;

		/* 10. Deactivate global interruptions */
		__disable_irq();

		/* 11. Configures the interruption (if it is able), even inscribe/remove the interruption from NVIC */
		adc_config_interrupt(adcConfig);

		/* 12. Activate ADC module */
		adc_peripheralOnOFF(ADC_ON);

		/* 13. Activate the global interruptions */
		__enable_irq();

}

static void adc_multi_set_sampling_and_hold(ADC_Config_t *adcConfig, uint8_t channel_QTY){

	for (int j = 0; j < channel_QTY; j++) {
		adc_single_set_sampling_and_hold(adcConfig+j);
	}

}
static void adc_multi_set_channel_sequence(ADC_Config_t *adcConfig, uint8_t channel_QTY){

	ADC1->SQR1 &= ~(ADC_SQR1_L); // To clean length conversion from the channel conversion sequence
	ADC1->SQR1 |= ((channel_QTY-1) << ADC_SQR1_L_Pos); // To change the length value

	for (int k = 0; k < channel_QTY; k++) {

		switch(k+1){
			case 1 ... 6: // Range of SQR3
				ADC1->SQR3 &= ~(0x1F << (k*5U)); // To clear the conversion of the sequence
				ADC1->SQR3 |= (((adcConfig+k)->channel) << (k*5U)); // To set the channel to the sequence
				break;
			case 7 ... 12: // Range of SQR2
				ADC1->SQR2 &= ~(0x1F << ((k-6)*5U)); // To clear the conversion of the sequence
				ADC1->SQR2 |= (((adcConfig+k)->channel) << ((k-6)*5U)); // To set the channel to the sequence
				break;
			case 13 ... 16: // Range of SQR1
				ADC1->SQR1 &= ~(0x1F << ((k-12)*5U)); // To clear the conversion of the sequence
				ADC1->SQR1 |= (((adcConfig+k)->channel) << ((k-12)*5U)); // To set the channel to the sequence
				break;
			default:
				__NOP();
				break;
		}
	}
}

/* Configuration to external trigger */
void adc_setExternalEvent(uint8_t externalTrigger, uint8_t externalEvent){

	ADC1->CR2 &=~(ADC_CR2_EXTEN); // To clear EXTEN bits
	ADC1->CR2 |=(externalTrigger << ADC_CR2_EXTEN_Pos); // To SET External Trigger

	ADC1->CR2 &=~(ADC_CR2_EXTSEL);
	ADC1->CR2 |=(externalEvent << ADC_CR2_EXTSEL_Pos); // To SET External Event (Timer_CHx or EXTI11)
}







