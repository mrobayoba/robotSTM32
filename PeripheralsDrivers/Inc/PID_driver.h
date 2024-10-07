/*
 * PID_driver.h
 *
 *  Created on: May 7, 2024
 *      Author: matto
 */

#ifndef PID_DRIVER_H_
#define PID_DRIVER_H_

typedef struct {
	/* Controller gains*/
	float Kp; // Proportional gain
	float Ki; // Integral gain
	float Kd; // Differential gain

	/* Derivative low-pass filter time constant */
	float tau;

	/* Output limits */
	float limMin;
	float limMax;

	/* delta Kp */
	float delta_Kp; //define Kp dead zone

	/* Sample time (in seconds) */
	float T;

	/* Controller "memory" (storage variables)*/
	float proportional;
	float integrator;
	float prevError;
	float differentiator;
	float prevMeasurement;

	/* Controller output */
	float out;

}PID_Controller_t;

void PIDController_Init(PID_Controller_t *pid); // To Init/Reset the PID algorithm
float PIDController_Update(PID_Controller_t *pid, float setPoint, float measurement);

#endif /* PID_DRIVER_H_ */
