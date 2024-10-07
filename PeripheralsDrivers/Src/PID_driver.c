/*
 * PID_driver.c
 *
 *  Created on: May 7, 2024
 *      Author: matto
 */

#include "PID_driver.h"

void PIDController_Init(PID_Controller_t *pid){
	// To Init/Reset the PID algorithm

	/* Clear controller variables */
	pid->proportional = 0.0f;
	pid->integrator = 0.0f;
	pid->differentiator = 0.0f;

	pid->prevError = 0.0f;
	pid->prevMeasurement = 0.0f;

	pid->out = 0.0f;

}
float PIDController_Update(PID_Controller_t *pid, float setPoint, float measurement){

	/* Error signal */
	float error  = setPoint - measurement;

	/* Proportional */
	if (error > 2.0f || error < -2.0f) { // when error is greather than N = 5
		pid->proportional = pid->Kp *error;
	}
	else{ // when error is lower or equal to N = 15 then reduce Kp slowly to zero
//		proportional = pid->Kp * ((error-pid->delta_Kp)/pid->delta_Kp) * error;
		pid->proportional = 0;
	}

	/* Integral */
//	pid->integrator += 0.5f * pid->Ki * pid->T * (error + pid->prevError);
	pid->integrator += pid->Ki * pid->T * error;

	/* Anti_wind_up via dynamic integrator clamping */
	float limMinInt, limMaxInt;

	/* Compute integrator limits */

	if(pid->limMax > pid->proportional){

		limMaxInt = pid->limMax - pid->proportional;

	}
	else {
		limMaxInt = 0.0f;
	}

	if (pid->limMin < pid->proportional){

		limMinInt = pid->limMin - pid->proportional;

	}
	else {
		limMinInt = 0.0f;
	}

	/* Clamp integrator */

	/* To be sure the integrator is between an acceptable range */
	if(pid->integrator > limMaxInt) {
		pid->integrator = limMaxInt;
	}
	else if(pid->integrator < limMinInt){
		pid->integrator = limMinInt;
	}

	/* Derivative (band-limited differentiator) */
	if (pid->Kd > 0) {
		pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)
							+ (2.0f * pid->tau - pid->T) * pid->differentiator)
							/ (2.0f * pid->tau + pid-> T);
//		pid->differentiator = ((pid->Kd*(measurement - pid->prevMeasurement)/(pid->T))
//				+pid->differentiator)/2.0f;
	}

	/* Compute output and apply limits */
	pid->out = pid->proportional + pid->integrator + pid->differentiator;

	if (pid->out > pid->limMax){

		pid->out = pid->limMax;

	}
	else if (pid->out < pid->limMin) {

		pid->out = pid->limMin;

	}

	/* Store error and measurement for later use */

	pid->prevError = error;
	pid->prevMeasurement = measurement;

	/* Return the output */

	return pid->out;

}
