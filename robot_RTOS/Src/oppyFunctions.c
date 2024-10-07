/*
 * oppyFunctions.c
 *
 *  Created on: Sep 26, 2024
 *      Author: matto
 */
#include "main.h"


void oppyStart(void){
//	gpio_WritePin(&enR, RESET);
//	gpio_WritePin(&enL, RESET);
	// Enables movement
	flag_restart_movement = SET;
	flag_PID = SET;
	xTaskNotify(xTaskHandler_PID_core,SET,eSetValueWithOverwrite);
}
void oppyStop(void){
	/// Disables movement
	gpio_WritePin(&enR, SET);
	gpio_WritePin(&enL, SET);
	PIDController_Init(&pid_Left);
	PIDController_Init(&pid_Right);
	flag_PID = RESET;
}

/***
 * This function calculates the speed comparison factor
 */
void oppySetSpeed(float speed_in_mm_per_seg){
	// To set new speed

	//	if ((speed_in_mm_per_seg < MAX_SPEED) && (speed_in_mm_per_seg >MIN_SPEED)) { // To be sure that speed is in the acceptable range
	//		N_setpoint = N_FACTOR * speed_in_mm_per_seg;
	//	}
	//	else{
	//		__NOP();
	//	}
}

/***
 * This function adjust the N_right and N_left values to set both PWM
 */
void oppySetPWM(uint8_t pwm_right_or_left, float pwmVal){

	float auxVal = 0;

	if (!pwm_right_or_left) {
		auxVal = pwmVal; // calculate new PWM
		/*
		 * if decimals in auxVal are lower than 0.5 then give the integer part to pwm
		 */
		if((auxVal-((uint16_t) auxVal) < 0.54)){
			pwmRight = (uint16_t) auxVal; // aprox. new float pwm to uint16 pwm
		}
		/*
		 * if decimals in auxVal are greater than 0.5 then give the integer part plus one to pwm
		 */
		else{
			pwmRight = (uint16_t) auxVal;
			pwmRight++;
		}
		portENTER_CRITICAL();
		pwm_updateDutyCycle(&pwm_R, pwmRight); // Set updated pwm
		portEXIT_CRITICAL();
	}

	else if (pwm_right_or_left) {
		auxVal = pwmVal;
		/*
		 * if decimals in auxVal are lower than 0.5 then give the integer part to pwm
		 */
		if((auxVal-((uint16_t) auxVal) < 0.54)){
			pwmLeft = (uint16_t) auxVal; // aprox. new float pwm to uint16 pwm
		}
		/*
		 * if decimals in auxVal are greater than 0.5 then give the integer part plus one to pwm
		 */
		else {
			pwmLeft = (uint16_t) auxVal;
			pwmLeft++;
		}
		portENTER_CRITICAL();
		pwm_updateDutyCycle(&pwm_L, pwmLeft);
		portEXIT_CRITICAL();
	}

	//		sprintf(bufferData,"New pwms {%d, %d}\n", pwmRight,pwmLeft);
	//		usart_writeMsg(&commSerial,bufferData);
	//		clear_string(bufferData);


}

void oppyTurn(uint8_t direction){
	// To turn to direction right or left at a fixed angle
	switch(direction){
	case e_TURN_RIGHT:
		oppyStop();
		gpio_WritePin(&inR, RESET);
		pwm_updatePolarity(&pwm_R, PWM_POLARITY_HIGH);
		gpio_WritePin(&inL, RESET);
		pwm_updatePolarity(&pwm_L, PWM_POLARITY_HIGH);
//		oppyStart();

		break;
	case e_TURN_LEFT:
		oppyStop();
		gpio_WritePin(&inR, SET);
		pwm_updatePolarity(&pwm_R, PWM_POLARITY_LOW);
		gpio_WritePin(&inL, SET);
		pwm_updatePolarity(&pwm_L, PWM_POLARITY_LOW);
//		oppyStart();

		break;
	}

}

void oppyAdvanceTo(uint8_t direction){  // second parameter not in use yet!!!
	// To drive the oppy_1 to direction forward or backward at a fixed distance
	switch(direction){
	case e_ADVANCE_TO_FORWARD:
		oppyStop();
		gpio_WritePin(&inR, SET);
		pwm_updatePolarity(&pwm_R, PWM_POLARITY_LOW);
		gpio_WritePin(&inL, RESET);
		pwm_updatePolarity(&pwm_L, PWM_POLARITY_HIGH);
//		oppyStart();

		break;
	case e_ADVANCE_TO_BACKWARD:
		oppyStop();
		gpio_WritePin(&inR, RESET);
		pwm_updatePolarity(&pwm_R, PWM_POLARITY_HIGH);
		gpio_WritePin(&inL, SET);
		pwm_updatePolarity(&pwm_L, PWM_POLARITY_LOW);
//		oppyStart();

		break;
	}
}



