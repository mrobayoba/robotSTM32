/*
 * main.h
 *
 *  Created on: Aug 13, 2024
 *      Author: matto
 */

#ifndef MAIN_H_
#define MAIN_H_


// Basic includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <arm_math.h>

// For recognize the nucleo-64 board
#include "stm32f4xx.h"
#include "stm32_assert.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

// PeripheralsDrivers includes
#include "pll_driver_hal.h"
#include "gpio_driver_hal.h"
#include "usart_driver_hal.h"
#include "timer_driver_hal.h"
#include "pwm_driver_hal.h"
#include "exti_driver_hal.h"
//#include "spi_driver_hal.h"
//#include "SysTick_driver_hal.h"
#include "NVIC_FreeRTOS_priorities.h"

// Private includes
//#include "gyro_L3G4200D.h"
#include "encoderCounting_driver.h"
#include "PID_driver.h"
#include "A_star.h"

/* Set definitions */
#define BUFFER 100

/* Possible angles in octal grid */
#define RAD_0	0.0f
#define RAD_45	PI/4.0f
#define RAD_90	PI/2.0f
#define RAD_135	PI*(3.0f/4.0f)
#define RAD_180	PI
#define RAD_N180 -PI

/* Oppy definitions */
#define GRID_SIZE 0.255f // [m] from 21-401 lab floor
#define N 120.0f // number of step divisions in the encoder wheel 60 and double it 'cause rising and falling edge exti
#define D_RIGHT_WHEEL 51.0f // 52mm diameter also nominal diameter!!!
#define D_LEFT_WHEEL 50.5f // 51.95mm diameter
#define L 105.60f // 105.60mm (Distance between wheels)

#define C_RIGHT_WHEEL  PI*D_RIGHT_WHEEL // Circumference of the right wheel
#define C_LEFT_WHEEL  PI*D_LEFT_WHEEL // Circumference of the left wheel


#define SAMPLING_TIME 100.0f // 125ms time of N sampling in ms by default
#define DELTA_T 10.0f // 10s of calibration time sampling

#define N_FACTOR (N * SAMPLING_TIME)/C_RIGHT_WHEEL // N factor to calculate N from speed


// PWM(N) function coefficients (f[x]:= M*x+B)
#define M_LEFT 0.00453f // slope
#define B_LEFT -0.262f // intercept

#define M_RIGHT 0.00350f
#define B_RIGHT -0.109f

#define MAX_PWM 950.0f
#define MIN_PWM 300.0f

#define TAU	2.15f // SIGNAL FILTER FOR DERIVATIVE OF PID

// FOR A-star and MAPPING
#define MAX_MESH_SIZE	(17*17) // at least must have max matrix_size for the map of 16x16 plus null character '\0' plus one '\n' column

// For drive
enum{
	eNot_Run = 0,
	eRun,
	eFinish,
	eSquareRight, // Draw the square to the right
	eSquareLeft // Draw the square to the left
};

enum{
	e_TURN_RIGHT 	= 0,
	e_TURN_LEFT 	= 1
};

enum{
	e_ADVANCE_TO_FORWARD 	= 0,
	e_ADVANCE_TO_BACKWARD 	= 1
};

enum{
	e_PWM_RIGHT		= 0,
	e_PWM_LEFT		= 1
};

// FreeRTOS
typedef struct{
	uint8_t payload[MAX_MESH_SIZE];	// data retrieved by the serial comm
	uint32_t len;			// len of the string
} command_t;

typedef enum{
	sMainMenu = 0,
	sLedEffect,
	sRtcMenu,
}state_t;

/* extern RTOS Handlers */
extern TaskHandle_t xTaskHandler_menu;
extern TaskHandle_t xTaskHandler_print;
extern TaskHandle_t xTaskHandler_comm;

extern TaskHandle_t xTaskHandler_blinkyMode;
extern TaskHandle_t xTaskHandler_PID_core;
extern TaskHandle_t xTaskHandler_Astar;
extern TaskHandle_t xTaskHandler_driveOppyTo;
extern TaskHandle_t xTaskHandler_squareTest;

extern TaskHandle_t xTaskHandler_gyroGetData;

extern QueueHandle_t xQueueHandler_print;
extern QueueHandle_t xQueueHandler_inputData;
extern QueueHandle_t xQueueHandler_naviList;

extern TaskHandle_t xTaskHandler_gyroGetID;
extern QueueHandle_t xQueueHandler_gyroData;

extern TimerHandle_t xTimerHandler_LED;

//extern SemaphoreHandle_t xBinSemphrHandler_MoveOneStep;

/* extern Handlers */

//extern gyroHandler_t L3G4200D;
extern encoder_handler_t encoderS_Handler; // Private

// For oppy Wheels
// PWM wheels
extern GPIO_Handler_t GPIO_pwm_R; //
extern GPIO_Handler_t GPIO_enR; //PinA5
extern GPIO_Handler_t GPIO_inR; //PinA5

extern GPIO_Handler_t GPIO_pwm_L; //
extern GPIO_Handler_t GPIO_enL; //PinA5
extern GPIO_Handler_t GPIO_inL; //PinA5

extern PWM_Handler_t PWM_Right; // RIGHT WHEEL
extern PWM_Handler_t PWM_Left; // LEFT WHEEL

extern PID_Controller_t PID_Left;
extern PID_Controller_t PID_Right;

extern mapHandler_t Map_Handler;

// For drive the Oppy_1
extern uint16_t N_right; // Number of steps counted in DELTA_T seconds
extern uint16_t N_left;
extern float speed_fixed_L;
extern float speed_fixed_R;
extern uint16_t pwmRight;
extern uint16_t pwmLeft;

extern float omega; // angular frequency
extern float S_Right; // distance factor: To calculate distance
extern float S_Left;

extern float speed; // 30mm/s by Default
extern float N_setpoint; // 12 counts on 100ms
extern float distance;
extern float target; // To define a distance/angule target

extern uint8_t flag_newSpeed;
extern uint8_t flag_refreshPwm;
extern uint8_t flag_restart_movement;
extern uint8_t flag_PID;
extern uint8_t flag_print;

//extern SemaphoreHandle_t xBinarySemaphore;

extern state_t next_state;
extern USART_Handler_t USART_commSerial;
extern uint16_t msToBlink;
extern float kIncrement;
extern float correctionFactor;

extern char bufferData[MAX_MESH_SIZE];
extern char auxBufferData[MAX_MESH_SIZE];

extern float gridSize;

extern uint16_t squareShape[2];

extern const TickType_t xBlockTimeMaxExpected;
extern TickType_t motionDelay;
extern float cFactor;
//extern int16_t gyroBuffer[GYRO_WATERMARK_VALUE*3];

/* FreeRTOS Function Prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

void vTask_menu( void * pvParameters );
void vTask_print( void * pvParameters );
void vTask_comm( void * pvParameters );

void vTask_blinkyMode(void* pvParameters);

void vTask_PID_core(void* pvParameters); // This function refresh PWM based in PID
void vTask_distance_measure(void* pvParameters); // To measure distance and Notify it
void vTask_testSquare(void* pvParameters); // To draw the square test
void vTask_Astar(void* pvParameters); // To calculate the best path to goal
void vTask_driveOppyTo(void* pvParameters); // Pass a list with movement type (for, back, right, left) and distance(angle) to drive the spirit
void vTask_squareTest(void* pvParameters);

//void vTask_gyroGetID(void* pvParameters);
//void vTask_gyroGetData(void* pvParameters);

void vTimer_Callback_LED(TimerHandle_t xTimer);
void vTimer_Callback_encoderSampling(TimerHandle_t xTimer);

/* Function Prototypes */
void clear_string(char *string);
uint16_t arraySortAndMedian(uint16_t* arr, uint8_t arraySize);

void oppyStart(void);
void oppyStop(void);

void oppySetSpeed(float speed_in_mm_per_seg);
void oppySetPWM(uint8_t pwm_right_or_left, float pwmVal);
void oppyTurn(uint8_t direction);
void oppyAdvanceTo(uint8_t direction);

#endif /* MAIN_H_ */
