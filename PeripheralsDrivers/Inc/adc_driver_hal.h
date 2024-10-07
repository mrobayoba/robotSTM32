/*
 * adc_driver_hal.h
 *
 *  Created on: Oct 12, 2023
 *      Author: mrobayoba
 */

#ifndef ADC_DRIVER_HAL_H_
#define ADC_DRIVER_HAL_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "NVIC_FreeRTOS_priorities.h"

enum{
	CHANNEL_0	=	0,
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5,
	CHANNEL_6,
	CHANNEL_7,
	CHANNEL_8,
	CHANNEL_9,
	CHANNEL_10,
	CHANNEL_11,
	CHANNEL_12,
	CHANNEL_13,
	CHANNEL_14,
	CHANNEL_15

};

enum{
	RESOLUTION_12_BIT	=	0,
	RESOLUTION_10_BIT,
	RESOLUTION_8_BIT,
	RESOLUTION_6_BIT
};

enum{
	ALIGNMENT_RIGHT	=	0,
	ALIGNMENT_LEFT
};

enum{
	SCAN_OFF =	0,
	SCAN_ON
};

enum{
	ADC_OFF	=	0,
	ADC_ON
};

enum{
	ADC_INT_DISABLE	=	0,
	ADC_INT_ENABLE
};

enum{
	TRIGGER_AUTO	=	0,
	TRIGGER_MANUAL,
	TRIGGER_EXT
};

enum{
	SAMPLING_PERIOD_3_CYCLES	=	0b000,
	SAMPLING_PERIOD_15_CYCLES	=	0b001,
	SAMPLING_PERIOD_28_CYCLES	=	0b010,
	SAMPLING_PERIOD_56_CYCLES	=	0b011,
	SAMPLING_PERIOD_84_CYCLES	=	0b100,
	SAMPLING_PERIOD_112_CYCLES	=	0b101,
	SAMPLING_PERIOD_144_CYCLES	=	0b110,
	SAMPLING_PERIOD_480_CYCLES	=	0b111
};

enum{
	EXTERNAL_TRIGGER_DISABLE = 0,
	EXTERNAL_TRIGGER_RISING_EDGE,
	EXTERNAL_TRIGGER_FALLING_EDGE,
	EXTERNAL_TRIGGER_BOTH_RISING_AND_FALLING_EDGE
};

enum{
	EXTERNAL_EVENT_TIM1_CH1 = 0,
	EXTERNAL_EVENT_TIM1_CH2,
	EXTERNAL_EVENT_TIM1_CH3,
	EXTERNAL_EVENT_TIM2_CH2,
	EXTERNAL_EVENT_TIM2_CH3,
	EXTERNAL_EVENT_TIM2_CH4,
	EXTERNAL_EVENT_TIM2_TRGO,
	EXTERNAL_EVENT_TIM3_CH1,
	EXTERNAL_EVENT_TIM1_TRGO,
	EXTERNAL_EVENT_TIM4_CH4,
	EXTERNAL_EVENT_TIM5_CH1,
	EXTERNAL_EVENT_TIM5_CH2,
	EXTERNAL_EVENT_TIM5_CH3,
	EXTERNAL_EVENT_RESERVED1,
	EXTERNAL_EVENT_RESERVED2,
	EXTERNAL_EVENT_EXTI11,
};

/* ADC Handler definition
 * This handler is used to configure a single ADC channel.
 * - Channels			-> configures inside the driver the correct GPIO pin as ADC channel
 * - Resolution			-> 6bit, 8bit, 10bit, 12bit are the possible options
 * - Sampling Period	-> Related to the SARs procedure
 * - Data alignment		-> left or right, depends on the app. Default is right
 * - adc data			-> holds the data
 **/

typedef struct{

	uint8_t		channel;				// ADC channel that will be used to the ADC conversion
	uint8_t		resolution;				// Precision which data is acquire by ADC
	uint16_t	samplingPeriod;			// Desired time to do the data acquisition
	uint8_t		dataAlignment;			// Alignment to left or right
	uint16_t	adcData;				// Data storage variable
	uint8_t		interruptEnable;		// To configure whether or not to work with the interruption
	uint8_t		interruptPriority;		// Use e_PRIORITY_NORMAL_MIN_xx to set priority

} ADC_Config_t ;

/* Header definition for the "public functions" of adc_driver_hal */
void adc_ConfigSingleChannel(ADC_Config_t *adcConfig);
void adc_ConfigMultiChannel(ADC_Config_t *adcConfig, uint8_t channel_QTY);
void adc_ConfigAnalogPin(uint8_t adcChannel);
void adc_CompleteCallback(void);
void adc_StartSingleConv(void);
void adc_ScanMode(uint8_t state);
void adc_StartContinuousConv(void);
void adc_StopContinuousConv(void);
void adc_peripheralOnOFF(uint8_t state);
void adc_setExternalEvent(uint8_t externalTrigger, uint8_t externalEvent);
uint16_t adc_GetValue(void);

/* Advanced ADC configurations */
//void adc_ConfigMultiChannel(ADC_Config_t *adcConfig, uint8_t ChannelNumber);
//void adc_ConfigTrigger(uint8_t sourceType, PWM_Handler_t *triggerSignal);

#endif /* ADC_DRIVER_HAL_H_ */
