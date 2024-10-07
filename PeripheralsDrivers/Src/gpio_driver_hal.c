/* Transcribed by mrobayoba */

#include "gpio_driver_hal.h"

#include "stm32f4xx.h"

#include "stm32_assert.h"

/* **** Headers for private functions **** */
static void gpio_enable_clock_peripheral(GPIO_Handler_t *pGPIOHandler);
static void gpio_config_mode(GPIO_Handler_t *pGPIOHandler);
static void gpio_config_output_type(GPIO_Handler_t *pGPIOHandler);
static void gpio_config_output_speed(GPIO_Handler_t *pGPIOHandler);
static void gpio_config_pullup_pulldown(GPIO_Handler_t *pGPIOHandler);
static void gpio_config_alternate_function(GPIO_Handler_t *pGPIOHandler);

/*
 * For every peripheral, there are several steps that have to be follow them always
 * in a strict order to guarantee that the system allow us to configure the X peripheral.
 * The first and most important thing is activate the main clock signal to pass though
 * the specific element (related with the RCC peripheral), to this we will called
 * "peripheral activation or activation of the peripheral clock signal.
 */
void gpio_Config (GPIO_Handler_t *pGPIOHandler){

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_PIN(pGPIOHandler->pinConfig.GPIO_PinNumber));

	// 1) Activate the peripheral
	gpio_enable_clock_peripheral(pGPIOHandler);

	// After the activation is done, we can start to configure.

	// 2) To configure the GPIOx_MODER register
	gpio_config_mode(pGPIOHandler);

	// 3) To configure the GPIOx_OTYPER register
	gpio_config_output_type(pGPIOHandler);

	// 4) To configure the speed
	gpio_config_output_speed(pGPIOHandler);

	// 5) To configure if there are needed pull-up, pull-down or floating
	gpio_config_pullup_pulldown(pGPIOHandler);

	// 6) To configure the alternative functions... we'll see it later in the course
	gpio_config_alternate_function(pGPIOHandler);

} // END of the GPIO_config


/* Enable the Clock signal for specific GPIOx port */
void gpio_enable_clock_peripheral(GPIO_Handler_t *pGPIOHandler){

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_ALL_INSTANCE(pGPIOHandler->pGPIOx));

	// GPIOA Verification
	if(pGPIOHandler->pGPIOx == GPIOA){
		// Write 1 (SET) in the respective GPIOA address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
	}
	// GPIOB Verification
	else if(pGPIOHandler->pGPIOx == GPIOB){
		// Write 1 (SET) in the respective GPIOB address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	}
	// GPIOC Verification
	else if(pGPIOHandler->pGPIOx == GPIOC){
		// Write 1 (SET) in the respective GPIOC address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
	}
	// GPIOD Verification
	else if(pGPIOHandler->pGPIOx == GPIOD){
		// Write 1 (SET) in the respective GPIOD address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
	}
	// GPIOE Verification
	else if(pGPIOHandler->pGPIOx == GPIOE){
		// Write 1 (SET) in the respective GPIOE address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
	}
	// GPIOH Verification
	else if(pGPIOHandler->pGPIOx == GPIOH){
		// Write 1 (SET) in the respective GPIOH address
		RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
	}

} //END


/*
 * Configures the mode in which the pin will work:
 * - Input
 * - Output
 * - Analog
 * - Alternate Function
 */
void gpio_config_mode(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_MODE(pGPIOHandler->pinConfig.GPIO_PinMode));

	//Here we are reading the config, moving 'PinNumber' times to the left that value (shift left)
	//and all that is loaded in the variable auxConfig
	auxConfig = (pGPIOHandler->pinConfig.GPIO_PinMode << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	//Before to load the new value, we clean up the specific bits of that register (0b00 must be wrote)
	//to do that apply a mask and a bitwise AND operation
	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	//Now load it to auxConfig in the register MODER
	pGPIOHandler->pGPIOx->MODER |= auxConfig;


}//END

/* Configures which type of output the PinX will use:
* - Push-Pull
* - openDrain
*/

void gpio_config_output_type(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_OUTPUT_TYPE(pGPIOHandler->piConfig.GPIO_PinOutputType));

	// Again, we read and move the value 'PinNumber' times
	auxConfig = (pGPIOHandler->pinConfig.GPIO_PinOutputType << pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Clean up before load it
	pGPIOHandler->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Load it on the respective register
	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

}

/*
 * Select between four different possible speeds for output PinX
 * - Low
 * - Medium
 * - Fast
 * - HighSpeed
 */
void gpio_config_output_speed(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_OSPEED(pGPIOHandler->piConfig.GPIO_PinOutputSpeed));

	// Again, we read and move the value 'PinNumber' times
	auxConfig = (pGPIOHandler->pinConfig.GPIO_PinOutputSpeed << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Clean up before load it
	pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Load it on the respective register
	pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;
}//END

/* Turns ON/OFF the pull-up and pull-down resistor for each PinX in selected GPIO port */
void gpio_config_pullup_pulldown(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;

	/*To verify if the selected pin is the right one */
	assert_param(IS_GPIO_PUPDR(pGPIOHandler->piConfig.GPIO_PinPuPdControl));

	// Again, we read and move the value 'PinNumber' times
	auxConfig = (pGPIOHandler->pinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Clean up before load it
	pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->pinConfig.GPIO_PinNumber);

	// Load it on the respective register
	pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

}

/* Allows to configure other functions (more specialized) on the selected PinX */
void gpio_config_alternate_function(GPIO_Handler_t *pGPIOHandler){

	uint32_t auxPosition = 0;

	if(pGPIOHandler->pinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		// First, select whether use the low register (AFRL) or the high register (AFRH)
		if(pGPIOHandler->pinConfig.GPIO_PinNumber < 8){
			// We are in the AFRL register, that controls the Pins from PIN_0 to PIN_7
			auxPosition = 4 * pGPIOHandler->pinConfig.GPIO_PinNumber;

			// First clean up the register address where are going to be wrote
			pGPIOHandler->pGPIOx->AFR[0] &= ~(0b1111 << auxPosition);

			//Now we write the configured value in the selected address
			pGPIOHandler->pGPIOx->AFR[0] |= (pGPIOHandler->pinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{
			// We are in the AFRH register, that controls the Pins from PIN_8 to PIN_15
			auxPosition = 4 * (pGPIOHandler->pinConfig.GPIO_PinNumber -8);

			// First clean up the register address where are going to be wrote
			pGPIOHandler->pGPIOx->AFR[1] &= ~(0b1111 << auxPosition);

			//Now we write the configured value in the selected address
			pGPIOHandler->pGPIOx->AFR[1] |= (pGPIOHandler->pinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
}//END

/*
 * The next function is used to load the pin state in the handler,
 * assigning the given value to the newState variable
 */
void gpio_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState){

	/* Verifying if the desired action is permitted */
	assert_param(IS_GPIO_PIN_ACTION(newState));

	// Clean up the desired address
	// pPinHandler->pGPIOx->ODR &= ~(SET << pPinHandler->pinConfig.GPIO_PinNumber);
	if(newState == SET){
		//Working in the low part of the register
		pPinHandler->pGPIOx->BSRR |= (SET << pPinHandler->pinConfig.GPIO_PinNumber);
	}
	else{
		// Clean up the low part of the register
		pPinHandler->pGPIOx->BSRR &= ~(SET << pPinHandler->pinConfig.GPIO_PinNumber);
		//Working in the high part of the register
		pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->pinConfig.GPIO_PinNumber + 16));
	}
}//END

/* The next function is used to read the state on a specific pin */

uint32_t gpio_ReadPin(GPIO_Handler_t *pPinHandler){
	//Create a aux variable to return a value later
	uint32_t pinValue = 0;
	uint32_t pinMask_IN = 0;

	pinMask_IN = 1; // To isolate the PinX value

	//Load the IDR register value, shifted to RIGHT as many times as the address of
	//the specific pin
	pinValue = pinMask_IN & (pPinHandler->pGPIOx->IDR >> pPinHandler->pinConfig.GPIO_PinNumber); //Turns everything into 0 but save the PinX value
	//pinValue = pinValue;THIS MOST BE THE ERROR

	return pinValue;
}

uint8_t gpio_ODR_ReadPin(GPIO_Handler_t *pPinHandler){

	//Create a aux variable to return a value later
	uint32_t pinState = 0;
	uint32_t pinMask_OUT = 0;

	pinMask_OUT = 0b1; // To isolate the PinX value

	//Load the ODR register value, shifted to RIGHT as many times
	// as the address of the specific pin
	pinState = pinMask_OUT & (pPinHandler->pGPIOx->ODR >> pPinHandler->pinConfig.GPIO_PinNumber); //Turns everything into 0 but save the PinX value

	return (uint8_t) pinState; // pinState only contains a 0b0 or 0b1
}

void gpio_TogglePin(GPIO_Handler_t *pPinHandler){

	uint8_t currentState = 0;

	currentState = gpio_ODR_ReadPin(pPinHandler); // To know the current state of the output PinX

	if(currentState ^ SET){ // Turn ON the pin if it was OFF
		gpio_WritePin(pPinHandler, SET);
	}

	else{ // Turn OFF the pin if it was ON
		gpio_WritePin(pPinHandler, RESET);
	}

}

void gpio_PinOFF(GPIO_Handler_t *pPinHandler){

	uint8_t currentState = 0;

	currentState = gpio_ODR_ReadPin(pPinHandler); // To know the current state of the output PinX

	if(currentState & SET){ // Turn OFF the pin if it was ON
		gpio_WritePin(pPinHandler, RESET);
	}

	else{ // Do nothing...
		__NOP();
	}
}

void gpio_PinON(GPIO_Handler_t *pPinHandler){

	uint8_t currentState = 0;

	currentState = gpio_ODR_ReadPin(pPinHandler); // To know the current state of the output PinX

	if(currentState & RESET){ // Turn ON the pin if it was OFF
		gpio_WritePin(pPinHandler, SET);
	}

	else{ // DO nothing...
		__NOP();
	}
}
//END







