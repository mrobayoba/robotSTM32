/**
 ******************************************************************************
 * @file           : main.c
 * @author         : namontoy@unal.edu.co - Nerio Andres Montoya G
 * @brief          : Counter_Semaphore
 ******************************************************************************
 * Este programa utiliza a los semaforos contadores para recibir varias solicitudes
 * de ejecución de una tarea, lanzadas desde la interrupcion de un boton.
 *
 * Se utiliza una funcion random para generar un numero x de peticiones de ejecucion
 * de la tarea, de forma que por cada pulsación del boton se generar varias (y no solo
 * una) solicitud de ejecución.
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>

/* Incluir las librerias de freeRTOS que sean necesarias (5)*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Incluir los drivers que sean necesarios (4)*/
#include "gpio_driver_hal.h"
#include "exti_driver_hal.h"
#include "usart_driver_hal.h"
#include "pll_driver_hal.h"
#include "NVIC_FreeRTOS_priorities.h"

/* tamaño del stack */
#define STACK_SIZE 200

/*definicion de variables del sistema */
uint32_t SystemCoreClock = 100000000;

/* Definición de Perifericos de nuestras librerias para controlar un LED (1)*/
GPIO_Handler_t handler_statusLED = {0};

/* Definición de Periphericos de nuestras librerias para recibir una interrupcion EXTI (2)*/
GPIO_Handler_t handler_userButton = {0};
EXTI_Config_t exti_userButton = {0};

/* Definición de Periphericos del nuestras librerias para recibir el manejo del USART (3)*/
USART_Handler_t usart_commSerial = {0};
GPIO_Handler_t handler_TX = {0};
GPIO_Handler_t handler_RX = {0};

/* Definición de Variable auxiliar para la recepcion serial*/
uint8_t usartData = 0;

/* Elemento del freeRTOS con el que se recibe el resultado de la creación de
 * tareas, y otros elementos del kernel del freeRTOS (1)*/
BaseType_t xReturned;

/* Definicion de un semaforo binario y un semaforo contador (2)*/
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xCounterSemaphore;

/*  Definición de Software timer para el led de estado (1)*/
TimerHandle_t xHandler_timerBlinky;

TaskHandle_t xHandleTask_counting = NULL;
TaskHandle_t xHandleTask_print = NULL;

QueueHandle_t xQueue_Print;

const char *msg_working = "\n - - Working - - \n";
char auxMsg[64] = {0};
//char *dummy = auxMsg;

volatile uint8_t printOneTime = 0;
volatile uint16_t randomNumber = 0;

/* cabeceras de las funciones que representan las tareas que ejecuta el FreeRTOS (2)*/
void vTask_counting( void * pvParameters );
void vTask_Print( void * pvParameters );

/* Cabeceras de otras funciones, como el initsystem, la función para generar el número random
 * y el callback para el led de estado (3)*/
void initSystem(void);
uint16_t getRandomNumber(void);
void vTimer_callback_LED(TimerHandle_t xTimer);

/* Funcion principal. Aca es donde sucede todo!! */
int main(void)
{
	/*Inicializacion del sistema:
	 * - Encender la FPU
	 * - Activar el contador de Ticks para debuging
	 * - Configurar adecuadamente el reloj principal del equipo para 100MHz
	 * - Lllamar a la funcion que inicializa nuestros perifericos (initSystem)
	 * */

	SCB->CPACR    |= (0xF << 20);

	DWT->CTRL    |= (1 << 0);

	pllConfig();

	initSystem();

	/* Crear las tareas que necesita el programa*/
	/* Creando la Tarea Counter */
	xReturned = xTaskCreate(vTask_counting,
							"counter task",
							STACK_SIZE,
							NULL,
							e_PRIORITY_FREERTOS_MIN_PLUS_2,
							&xHandleTask_counting);

    configASSERT(xReturned == pdPASS);

    /* Creando la Tarea Print */
    xReturned = xTaskCreate(vTask_Print,
    						"printing task",
							STACK_SIZE,
							NULL,
							e_PRIORITY_FREERTOS_MIN_PLUS_2,
							&xHandleTask_print);

    configASSERT(xReturned == pdPASS);

    /* Crear la cola para la tarea de impresion*/
    xQueue_Print = xQueueCreate(10,sizeof(char*));
    configASSERT(xQueue_Print != NULL);  // verificamos que se ha creado la queue correctamente.

    /* Creando el semaforo binario (1)*/
//   	xBinarySemaphore = xSemaphoreCreateBinary();
//   	configASSERT(xBinarySemaphore != NULL);  // verificamos que se ha creado la queue correctamente.

   	/* Creando el semaforo counter de 10 elementos, que inicia en 0 (1)*/
   	xCounterSemaphore = xSemaphoreCreateCounting(10,0);
   	configASSERT(xCounterSemaphore != NULL);  // verificamos que se ha creado la queue correctamente.

    /* Creando el timer que controla el blinky del led de estado, con un periodo de 350ms (1)*/
   	xHandler_timerBlinky = xTimerCreate("Blinky Timer",
   										pdMS_TO_TICKS(350),
										pdTRUE,
										(void*)(1),
										vTimer_callback_LED);
   	configASSERT(xHandler_timerBlinky !=NULL);
   	/* Activar el Timer que controla el led (1)*/
   	xTimerStart(xHandler_timerBlinky,portMAX_DELAY);

    /* Start the created tasks running. */
    vTaskStartScheduler();


    /* Loop forever */
	while(1)
	{
		/*Si llegamos aca, es que algo salio mal...*/
	}
}


/* Funcion que gobierna a la tarea que muestra como funciona un semaforo contador */
void vTask_counting( void * pvParameters )
{

	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS( 1000 );

    while(1)
    {
    	/* En este if() el sistema espera que se tenga un elemento disponible en el semaforo
    	 * cuando esta disponible actual -> imprime un mensaje que dice "working"*/
    	if ( xSemaphoreTake(xCounterSemaphore,xMaxExpectedBlockTime)== pdPASS) {

    		/* Este bloque es para imprimir solo 1 vez el valor del numero aleatorio*/
    		if(printOneTime == 1){
    			sprintf(auxMsg, "counter i = %d\n", randomNumber);
    			char *dummy = auxMsg;
    			xQueueSend(xQueue_Print, &dummy, portMAX_DELAY);
    			printOneTime = 0;
    		}
    		/* Imprime las peticiones de trabajo que genera el numero aleatorio en la interupcion*/
			xQueueSend(xQueue_Print, &msg_working, portMAX_DELAY);

    	}
    }
}

/* Funcion que gobierna a la tarea Print */
void vTask_Print( void * pvParameters )
{

	uint32_t *msg;
    while(1)
    {
        /* Task code goes here. */
    	xQueueReceive(xQueue_Print, &msg, portMAX_DELAY);
    	usart_writeMsg(&usart_commSerial, (char*)msg);

    }
}

/* Funcion para inicializar el hardware del equipo (peripheralDrivers) */
void initSystem(void){
	/* Configurando el LED_STATE, GPIOA PIN5 (6)*/
	handler_statusLED.pGPIOx						= GPIOA;
	handler_statusLED.pinConfig.GPIO_PinNumber		= PIN_5;
	handler_statusLED.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handler_statusLED.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	handler_statusLED.pinConfig.GPIO_PinOutputType	= GPIO_OTYPER_PUSHPULL;
	handler_statusLED.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&handler_statusLED);
	gpio_WritePin(&handler_statusLED, SET);

	/* Configurando el UserButton, GPIOC PIN-13 */
	handler_userButton.pGPIOx						= GPIOC;
	handler_userButton.pinConfig.GPIO_PinNumber		= PIN_13;
	handler_userButton.pinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handler_userButton.pinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	gpio_Config(&handler_userButton);

	/* Matriculando el PINC13 en los elementos del EXTI, ademas de configurar la
	 * prioridad de la interrupcion (4) */
//	-----------------------(x4);
	exti_userButton.edgeType						= EXTI_FALLING_EDGE;
	exti_userButton.interruptPriority				= e_PRIORITY_FREERTOS_MIN_PLUS_6;
	exti_userButton.pGPIOHandler					= &handler_userButton;
	exti_Config(&exti_userButton);
	exti_config_newInterrupt(&exti_userButton, e_PRIORITY_FREERTOS_MIN_PLUS_5);

	/* configuramos primero los pines que corresponden al USART2 */
	/* Config Rx (6)*/
//	-----------------------(x6);
	handler_RX.pGPIOx								= GPIOA;
	handler_RX.pinConfig.GPIO_PinNumber				= PIN_2;
	handler_RX.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handler_RX.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handler_RX.pinConfig.GPIO_PinAltFunMode			= AF7;
	gpio_Config(&handler_RX);

	/* Config Tx (6)*/
//	-----------------------(x6);
	handler_TX.pGPIOx								= GPIOA;
	handler_TX.pinConfig.GPIO_PinNumber				= PIN_3;
	handler_TX.pinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handler_TX.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEED_HIGH;
	handler_TX.pinConfig.GPIO_PinOutputType			= GPIO_OTYPER_PUSHPULL;
	handler_TX.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handler_TX.pinConfig.GPIO_PinAltFunMode			= AF7;
	gpio_Config(&handler_TX);

	/* Configurando el puerto serial (7)*/
//	-----------------------(x7)
	usart_commSerial.ptrUSARTx						= USART2;
	usart_commSerial.USART_Config.baudrate			= USART_BAUDRATE_115200;
	usart_commSerial.USART_Config.datasize			= USART_DATASIZE_8BIT;
	usart_commSerial.USART_Config.enableIntRX		= USART_RX_INTERRUP_ENABLE;
	usart_commSerial.USART_Config.mode				= USART_MODE_RXTX;
	usart_commSerial.USART_Config.parity			= USART_PARITY_NONE;
	usart_commSerial.USART_Config.stopbits			= USART_STOPBIT_1;
	usart_commSerial.USART_Config.interruptPriority	= e_PRIORITY_FREERTOS_MIN_PLUS_6;

	usart_config_newInterrupt(&usart_commSerial, e_PRIORITY_FREERTOS_MIN_PLUS_6);
	usart_Config(&usart_commSerial);
}

/*
 * Generar un numero aleatorio entre 1 y 10
 * la funcion sran(number), es para generar una semilla, de forma
 * que rand() genere un numero diferente siempre.
 * La funcion srand(number) requiere siempre un numero diferente, para
 * generar siempre un numero diferente...
 * */
uint16_t getRandomNumber(void)
{
	srand(xTaskGetTickCount());
	int randomnumber;
	randomnumber = (rand() % 10) +1;
	return (uint16_t)randomnumber;
}

/*Controla el estado del Led */
void vTimer_callback_LED( TimerHandle_t xTimer )
{
	/* Toogle LED */
	gpio_TogglePin(&handler_statusLED);

}

/* Interrupcion lanzada por el encider del BlueMotor. */
void callback_ExtInt13(void)
{
	/* Activar la variable printOneTime*/
	printOneTime = 1;
	/* Generar un numero aleatorio */
	randomNumber = getRandomNumber();

	BaseType_t pxHigherPriorityTaskWoken;
	pxHigherPriorityTaskWoken = pdFALSE;
	//(void)pxHigherPriorityTaskWoken;

	/* Hacer un for que se ejecute tantas veces como el numero aleatorio lo indica
	 * y allí cargar el semaforo (ejemplo binario y ejemplo counter)*/
   	for (uint16_t ii = 0; ii < randomNumber; ii++) {
   		/*cargar el semaforo e indicar que hay un cambio en las funciones.*/
   		xSemaphoreGiveFromISR(xCounterSemaphore,&pxHigherPriorityTaskWoken);
	}
}

/* Interrupcion debida al puerto serial */
void usart2_RxCallback(void)
{
	/* Recibir un dato en el serial y no hacer nada mas*/
	usartData = usart_getRxData();

}
