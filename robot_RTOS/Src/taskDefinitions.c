/*
 * taskDefinitions.c
 *
 *  Created on: Aug 13, 2024
 *      Author: matto
 */

#include "main.h"

/* Prototypes */
void process_command(command_t *cmd);
int extract_command(command_t *cmd);
float angleCalculation(float nextAngle, float prevAngle);
void freeMatrix(mapHandler_t* m);

uint8_t flag_driveOppyToPrint = SET;

const char *msg_invalid = "\n//// Invalid option ////\n\n";
const char *msg_help = "\n//// Show menu... ////\n\n";
const char *msg_option_0 = "\n---- Selected option - LED ----\n\n";
const char *msg_option_1 = "\n---- Selected option - Get ID ----\n\n";
const char *msg_option_se = "\n---- Selected option - Enable sampling ----\n\n";
const char *msg_option_sd = "\n---- Selected option - Disable sampling ----\n\n";
const char *msg_option_start = "\n---- Selected option - Run spirit ----\n\n";
const char *msg_option_stop = "\n---- Selected option - Stop spirit ----\n\n";
const char *msg_option_k_val = "\n---- Selected option - Change kIncrement value ----\n\n";
const char *msg_option_for = "\n---- Selected option - Move forward ----\n\n";
const char *msg_option_back = "\n---- Selected option - Move backward ----\n\n";
const char *msg_option_right = "\n---- Selected option - Turn to right ----\n\n";
const char *msg_option_left = "\n---- Selected option - Turn to left ----\n\n";
const char *msg_option_nup = "\n---- Selected option - Setpoint++ ----\n\n";
const char *msg_option_ndown = "\n---- Selected option - Setpoint-- ----\n\n";
const char *msg_option_r = "\n---- Selected option - Increase left Kp ----\n\n";
const char *msg_option_u = "\n---- Selected option - Increase right Kp ----\n\n";
const char *msg_option_f = "\n---- Selected option - Decrease left Kp ----\n\n";
const char *msg_option_j = "\n---- Selected option - Decrease right Kp ----\n\n";
const char *msg_option_t = "\n---- Selected option - Increase left Ki ----\n\n";
const char *msg_option_i = "\n---- Selected option - Increase right Ki ----\n\n";
const char *msg_option_g = "\n---- Selected option - Decrease left Ki ----\n\n";
const char *msg_option_k = "\n---- Selected option - Decrease right Ki ----\n\n";
const char *msg_option_y = "\n---- Selected option - Increase left Kd ----\n\n";
const char *msg_option_o = "\n---- Selected option - Increase right Kd ----\n\n";
const char *msg_option_h = "\n---- Selected option - Decrease left Kd ----\n\n";
const char *msg_option_l = "\n---- Selected option - Decrease right Kd ----\n\n";
const char *msg_option_w = "\n---- Selected option - Increase tau ----\n\n";
const char *msg_option_s = "\n---- Selected option - Decrease tau ----\n\n";
const char *msg_option_keys = "\n---- Selected option - Show PID constants ----\n\n";
const char *msg_option_map = "\n---- Selected option - Receive Map ----\n\n";
const char *msg_option_grid = "\n---- Selected option - Enter grid size ----\n\n";
const char *msg_option_delay = "\n---- Selected option - Change motion delay ----\n\n";
const char *msg_option_drive = "\n---- Selected option - Start motion ----\n\n";
const char *msg_option_square = "\n---- Selected option - Square test ----\n\n";
const char *msg_option_cup = "\n---- Selected option - Set correction factor up ----\n\n";
const char *msg_option_cdown = "\n---- Selected option - Set correction factor down ----\n\n";
const char *msg_option_squareShape = "\n---- Selected option - Change square shape ----\n\n";
const char *msg_option_print = "\n---- Selected option - Toggle print encoders PID output ----\n\n";
const char *msg_option_pdrive = "\n---- Selected option - Toggle print drive angle and dir ----\n\n";
const char *msg_option_free = "\n---- Selected option - Free heap/clear map ----\n\n";
const char *msg_option_2 = "\n---- Selected option - Exit ----\n\n";
const char *msg_option_n = "\n---- Option do not exist ----\n\n";

/* RTOS task's functions */
void vTask_menu( void * pvParameters ){
	uint32_t cmd_addr;
	command_t *cmd;

	char auxChar[10] = {0};
	int auxVal = 0;
	int auxVal2 = 0;
	float auxK = 0;
	//	char* option;

	const char* msg_menu_basic = "====================================================\n"
			"    	   |             Menu            |          \n"
			"====================================================\n"
			"LED effect								--->	led\n"
			"Run spirit								--->	start\n"
			"Stop spirit							--->	stop\n"
			"Move forward							--->	for\n"
			"Move backward							--->	back\n"
			"Turn to right							--->	right\n"
			"Turn to left							--->	left\n"
			"Receive Map							--->	m xxx\n"
			"Enter grid size						--->	g xx.xxx\n"
			"Change motion delay					--->	delay xxxxx\n"
			"Start motion							--->	drive\n"
			"Square test							--->	square\n"
			"Change square shape (max 16x16)		--->	s xx xx\n"
			"Free heap/clear map					--->	free\n"
			"Advanced options						--->	more\n"
			"Exit									--->	exit\n"
			"Enter your choice here: ";
	const char* msg_menu_advanced = "====================================================\n"
				"    	   |             Menu            |          \n"
				"====================================================\n"
				"Get gyro ID									--->	id\n"
				"Get and print gyro data						--->	get\n"
				"Enable get and print encoders values			--->	se\n"
				"Disable get and print encoders values			--->	sd\n"
				"Change kIncrement value						--->	k xx.xxx\n"
				"Setpoint++										--->	nup\n"
				"Setpoint--										--->	ndown\n"
				"Increase left Kp								--->	r\n"
				"Increase right Kp								--->	u\n"
				"Decrease left Kp								--->	f\n"
				"Decrease right Kp								--->	j\n"
				"Increase left Ki								--->	t\n"
				"Increase right Ki								--->	i\n"
				"Decrease left Ki								--->	g\n"
				"Decrease right Ki								--->	k\n"
				"Increase left Kd								--->	y\n"
				"Increase right Kd								--->	o\n"
				"Decrease left Kd								--->	h\n"
				"Decrease right Kd								--->	l\n"
				"Increase tau									--->	w\n"
				"Decrease tau									--->	s\n"
				"Show PID constants								--->	keys\n"
				"Set correction factor up N_Left				--->	q xx.xxx\n"
				"Set correction factor down N_Left				--->	a xx.xxx\n"
				"Toggle print encoders PID output				--->	print\n"
				"Toggle print drive angle and dir PID output	--->	pdrive\n"
				"Exit											--->	exit\n"
				"Enter your choice here: ";
	while(1){
		// Send to print in the console the menu
		//		xQueueSend(xQueueHandler_print, &msg_menu, portMAX_DELAY);
		// Waits until it receives the execution command
		if (xTaskNotifyWait(0,0,&cmd_addr,xBlockTimeMaxExpected)) {
			cmd = (command_t *) cmd_addr;
			// The received command must have only 1 character of length
			if(cmd->len < MAX_MESH_SIZE){

				if (strcmp((char*)(cmd->payload), "led") == 0) {
					xQueueSend(xQueueHandler_print, &msg_option_0, portMAX_DELAY); // Print message for option 0
					//				next_state = sLedEffect; // Return to main menu

					xTaskNotify(xTaskHandler_blinkyMode, 0, eNoAction);
					//				next_state = sMainMenu;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "help") == 0){
					xQueueSend(xQueueHandler_print, &msg_help, portMAX_DELAY);
					// Send to print in the console the menu
					xQueueSend(xQueueHandler_print, &msg_menu_basic, portMAX_DELAY);
					next_state = sMainMenu;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "more") == 0){
					xQueueSend(xQueueHandler_print, &msg_help, portMAX_DELAY);
					// Send to print in the console the menu
					xQueueSend(xQueueHandler_print, &msg_menu_advanced, portMAX_DELAY);
				}
				else if(strcmp((char*)(cmd->payload), "se") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_se, portMAX_DELAY);
					//				next_state = sEnableSampling;
					timer_SetState((&encoderS_Handler)->samplingTimer, TIMER_ON);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "sd") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_sd, portMAX_DELAY);
					//				next_state = sDisableSampling;
					timer_SetState((&encoderS_Handler)->samplingTimer, TIMER_OFF);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "print") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_print, portMAX_DELAY);
					//				next_state = sEnableSampling;
					flag_print = !flag_print;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "pdrive") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_pdrive, portMAX_DELAY);
					//				next_state = sEnableSampling;
					flag_driveOppyToPrint = !flag_driveOppyToPrint;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "start") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_start, portMAX_DELAY);
					//				next_state = sDisableSampling;
					oppyStart();
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "stop") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_stop, portMAX_DELAY);
					//				next_state = sDisableSampling;
					oppyStop();
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "k xx.xxx",2) == 0){ //
					xQueueSend(xQueueHandler_print, &msg_option_k_val, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %f", bufferData,&auxK);
					clear_string(bufferData);
					clear_string(auxChar);

					kIncrement = auxK;
					portENTER_CRITICAL();
					sprintf(bufferData,"The K increment was set to %.3f\n",auxK);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "for") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_for, portMAX_DELAY);
					clear_string((char*)(cmd->payload));
					distance = RESET;
					target = 0.5; // 0.5m
					oppyAdvanceTo(e_ADVANCE_TO_FORWARD);
					initSum();
					portENTER_CRITICAL();
					sprintf(bufferData,"Now Oppy goes forward %d\n",auxVal);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "back") == 0){ // changes direction backward
					xQueueSend(xQueueHandler_print, &msg_option_back, portMAX_DELAY);
					clear_string((char*)(cmd->payload));
					distance = RESET;
					target = 0.5; // 0.5m
					oppyAdvanceTo(e_ADVANCE_TO_BACKWARD);
					initSum();

					portENTER_CRITICAL();
					sprintf(bufferData,"Now Oppy goes backward %d\n",auxVal);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "right") == 0){ // Turn 90 degrees to the right
					xQueueSend(xQueueHandler_print, &msg_option_right, portMAX_DELAY);
					clear_string((char*)(cmd->payload));
					target = (PI/4.0f)*L/1000.0f; // 90°
					oppyTurn(e_TURN_RIGHT);

					portENTER_CRITICAL();
					sprintf(bufferData,"Now Oppy rotates 90° to the right %d\n",auxVal);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "left") == 0){ // Turn 90 degrees to the left
					xQueueSend(xQueueHandler_print, &msg_option_left, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					target = (PI/4.0f)*L/1000.0f; //90° (PI/2)*(L/2)=distance
					oppyTurn(e_TURN_LEFT);

					portENTER_CRITICAL();
					sprintf(bufferData,"Now Oppy rotates 90° to the left %d\n",auxVal);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "nup",2) == 0){ // update PWM frequency
					xQueueSend(xQueueHandler_print, &msg_option_nup, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %d",bufferData, &auxVal);
					clear_string(bufferData);
					clear_string(auxChar);

					N_setpoint++;
					//		PIDController_Init(&PID_Left);
					//		PIDController_Init(&PID_Right);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "ndown",2) == 0){ // update PWM frequency
					xQueueSend(xQueueHandler_print, &msg_option_ndown, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %d",bufferData, &auxVal);
					clear_string(bufferData);
					clear_string(auxChar);

					N_setpoint--;
					//		PIDController_Init(&PID_Left);
					//		PIDController_Init(&PID_Right);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "r") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_r, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Left.Kp += kIncrement;
					//				PID_Left.Ki += 1;
					//		PID_Left.Kd += 1;
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_L = %.2f\n",PID_Left.Kp);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "u") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_u, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Right.Kp += kIncrement;
					//				PID_Right.Ki += 1;
					//		PID_Right.Kd += 1;
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_R = %.2f\n",PID_Right.Kp);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "f") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_f, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Left.Kp -= kIncrement;
					//				PID_Left.Ki -= 1;
					//		PID_Left.Kd -= 1;
					//		if(PID_Left.Kp < 0){
					//			PID_Left.Kp = 0;
					//		}
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_L = %.2f\n",PID_Left.Kp);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "j") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_j, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Right.Kp -= kIncrement;
					//				PID_Right.Ki -= 1;
					//		PID_Right.Kd -= 1;
					//		if(PID_Right.Kp < 0){
					//			PID_Right.Kp = 0;
					//		}
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_R = %.2f\n",PID_Right.Kp);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "t") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_t, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Left.Kp += 0.1;
					PID_Left.Ki += kIncrement;
					//		PID_Left.Kd += 1;
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Ki_L = %.2f\n",PID_Left.Ki);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "i") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_i, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Right.Kp += 0.1;
					PID_Right.Ki += kIncrement;
					//		PID_Right.Kd += 1;
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Ki_R = %.2f\n",PID_Right.Ki);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "g") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_g, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Left.Kp -= 0.1;
					PID_Left.Ki -= kIncrement;
					//		PID_Left.Kd -= 1;
					if(PID_Left.Ki < 0){
						PID_Left.Ki = 0;
					}
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Ki_L = %.2f\n",PID_Left.Ki);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "k") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_k, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Right.Kp -= 0.1;
					PID_Right.Ki -= kIncrement;
					//		PID_Right.Kd -= 1;
					if(PID_Right.Ki < 0){
						PID_Right.Ki = 0;
					}
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Ki_R = %.2f\n",PID_Right.Ki);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "y") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_y, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Left.Kp += 0.1;
					//				PID_Left.Ki += 1;
					PID_Left.Kd += kIncrement;
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kd_L = %.2f\n",PID_Left.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "o") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_o, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Right.Kp += 0.1;
					//				PID_Right.Ki += 1;
					PID_Right.Kd += kIncrement;
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kd_R = %.2f\n",PID_Right.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "h") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_h, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Left.Kp -= 0.1;
					//				PID_Left.Ki -= 1;
					PID_Left.Kd -= kIncrement;
					if(PID_Left.Kd < 0){
						PID_Left.Kd = 0;
					}
					//		PIDController_Init(&PID_Left);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kd_L = %.2f\n",PID_Left.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "l") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_l, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					//		PID_Right.Kp -= 0.1;
					//				PID_Right.Ki -= 1;
					PID_Right.Kd -= kIncrement;
					if(PID_Right.Kd < 0){
						PID_Right.Kd = 0;
					}
					//		PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New Kd_R = %.2f\n",PID_Right.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "w") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_w, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Left.tau += kIncrement;
					PID_Right.tau += kIncrement;
					PIDController_Init(&PID_Left);
					PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New tau = %.2f\n",PID_Right.tau);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();

					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);

				}
				else if(strcmp((char*)(cmd->payload), "s") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_s, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					PID_Left.tau -= kIncrement;
					PID_Right.tau -= kIncrement;
					PIDController_Init(&PID_Left);
					PIDController_Init(&PID_Right);

					portENTER_CRITICAL();
					sprintf(bufferData,"New tau = %.2f\n",PID_Right.tau);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();

					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);

				}
				else if(strcmp((char*)(cmd->payload), "keys") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_keys, portMAX_DELAY);
					clear_string((char*)(cmd->payload));
					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_L = %.2f\nNew Ki_L = %.2f\nNew Kd_L = %.2f\n\n",
							PID_Left.Kp,PID_Left.Ki,PID_Left.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					portENTER_CRITICAL();
					sprintf(bufferData,"New Kp_R = %.2f\nNew Ki_R = %.2f\nNew Kd_R = %.2f\n",
							PID_Right.Kp,PID_Right.Ki,PID_Right.Kd);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "m xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",2) == 0)
				{ //
					xQueueSend(xQueueHandler_print, &msg_option_map, portMAX_DELAY);
					strcpy(bufferData,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(bufferData,"%s %s", auxChar,auxBufferData);
					clear_string(bufferData);
					clear_string(auxChar);


					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,auxBufferData); //auxBufferData is storing the map!
					portEXIT_CRITICAL();

					xTaskNotify(xTaskHandler_Astar, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "g xx.xxx",2) == 0){ //
					xQueueSend(xQueueHandler_print, &msg_option_grid, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %f", bufferData,&auxK);
					clear_string(bufferData);
					clear_string(auxChar);

					gridSize = auxK;
					portENTER_CRITICAL();
					sprintf(bufferData,"The grid size was set to %.3f\n",auxK);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strncmp((char*)(cmd->payload), "delay xxxxx",2) == 0){ //
					xQueueSend(xQueueHandler_print, &msg_option_delay, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %d", bufferData,&auxVal);
					clear_string(bufferData);
					clear_string(auxChar);

					motionDelay = pdMS_TO_TICKS(auxVal);
					portENTER_CRITICAL();
					sprintf(bufferData,"The motion delay was set to %d\n",auxVal);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "drive") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_drive, portMAX_DELAY);
					//				next_state = sEnableSampling;
					xTaskNotify(xTaskHandler_driveOppyTo, eRun, eSetValueWithOverwrite);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "square right") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_square, portMAX_DELAY);
					//				next_state = sEnableSampling;
					xTaskNotify(xTaskHandler_squareTest,eSquareRight,eSetValueWithOverwrite);
					// Create a task how pass to the navi queue a navilist,
					// designed to perform the square test and notify to driveOppyTo
				}
				else if(strcmp((char*)(cmd->payload), "square left") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_square, portMAX_DELAY);
					//				next_state = sEnableSampling;
					xTaskNotify(xTaskHandler_squareTest,eSquareLeft,eSetValueWithOverwrite);
					// Create a task how pass to the navi queue a navilist,
					// designed to perform the square test and notify to driveOppyTo
				}
				else if(strcmp((char*)(cmd->payload), "free") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_free, portMAX_DELAY);
					// Create a function to clear memory allocated areas from Map_Handler.Chart
					// Create Chart by pvPortMalloc... remember to free after reach goal
					freeMatrix(&Map_Handler);

					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"Map.Chart was successfully deallocated");
					portEXIT_CRITICAL();

				}
				else if(strncmp((char*)(cmd->payload), "s xx xx",2) == 0){ //
					xQueueSend(xQueueHandler_print, &msg_option_squareShape, portMAX_DELAY);
					strcpy(auxChar,(char*)(cmd->payload));
					clear_string((char*)(cmd->payload));
					sscanf(auxChar,"%s %d %d", bufferData,&auxVal, &auxVal2);
					clear_string(bufferData);
					clear_string(auxChar);

					squareShape[0] = auxVal;
					squareShape[1] = auxVal2;
					portENTER_CRITICAL();
					sprintf(bufferData,"The new square shape was set to [%d,%d]\n",auxVal,auxVal2);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();
					clear_string(bufferData);
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else if(strcmp((char*)(cmd->payload), "q") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_cup, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					correctionFactor += kIncrement;

					portENTER_CRITICAL();
					sprintf(bufferData,"New correctionFactor = %.2f\n",correctionFactor);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();

					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);

				}
				else if(strcmp((char*)(cmd->payload), "a") == 0){ // changes direction forward
					xQueueSend(xQueueHandler_print, &msg_option_cdown, portMAX_DELAY);
					clear_string((char*)(cmd->payload));

					correctionFactor -= kIncrement;

					portENTER_CRITICAL();
					sprintf(bufferData,"New correctionFactor = %.2f\n",correctionFactor);
					usart_writeMsg(&USART_commSerial,bufferData);
					portEXIT_CRITICAL();

					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);

				}


				//			else if(strcmp((char*)(cmd->payload), "id") == 0){
				//				xQueueSend(xQueueHandler_print, &msg_option_1, portMAX_DELAY);
				//
				//				next_state = sGyroID;
				//				xTaskNotify(xTaskHandler_gyroGetID, 0, eNoAction);
				//				next_state = sMainMenu;
				//				xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				//			}
				//			else if(strcmp((char*)(cmd->payload), "get") == 0){
				//				xQueueSend(xQueueHandler_print, &msg_option_2, portMAX_DELAY);
				//
				//				next_state = sGyroGetData;
				//				xTaskNotify(xTaskHandler_gyroGetData, 0, eNoAction);
				//				next_state = sMainMenu;
				//				xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				//			}
				//			else if(strcmp((char*)(cmd->payload), "stop") == 0){
				//				xQueueSend(xQueueHandler_print, &msg_option_2, portMAX_DELAY);
				//
				//				next_state = sGyroStop;
				//				xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				//			}
				else if(strcmp((char*)(cmd->payload), "exit") == 0){
					xQueueSend(xQueueHandler_print, &msg_option_2, portMAX_DELAY);

					next_state = sMainMenu;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
				else{

					xQueueSend(xQueueHandler_print, &msg_option_n, portMAX_DELAY);
					next_state = sMainMenu;
					//					xTaskNotify(xTaskHandler_menu, 0, eNoAction);
				}
			}

			else{
				xQueueSend(xQueueHandler_print,&msg_invalid, portMAX_DELAY);
				next_state = sMainMenu;
				//				xTaskNotify(xTaskHandler_menu, 0, eNoAction);
			}
		}

		//		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	} // End of the task loop

}
void vTask_print( void * pvParameters ){
	uint32_t *msg;

	while(1){
		xQueueReceive(xQueueHandler_print, &msg, portMAX_DELAY);
		portENTER_CRITICAL();
		usart_writeMsg(&USART_commSerial, (char *) msg);
		portEXIT_CRITICAL();
	}
}

void vTask_comm( void * pvParameters ){
	BaseType_t notify_status = {0};
	command_t cmd = {0};

	while(1){
		// Wait for a notification from the interrupt
		notify_status = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		//
		if(notify_status == pdTRUE){
			process_command(&cmd);
		}
	}

}

void vTask_blinkyMode(void* pvParameters){
	uint16_t* aux;
	BaseType_t notify_status = {0};
	aux =(uint16_t*) pvParameters;

	while(1){
		notify_status = xTaskNotifyWait(0,0,NULL,xBlockTimeMaxExpected);

		if(notify_status == pdTRUE){
			if(*aux == 1000){
				*aux = 150;
			} else{
				*aux = 1000; //1s
			}
			xTimerChangePeriod(xTimerHandler_LED,pdMS_TO_TICKS(*aux),xBlockTimeMaxExpected);
		}


	}
}

void vTask_PID_core(void* pvParameters){ // This is a periodic function

	uint32_t xNotifyValue = {0};

	encoder_handler_t* encoders = (encoder_handler_t*) pvParameters;

	//	float correctionFactor = D_LEFT_WHEEL/D_RIGHT_WHEEL;

	while(1){

		xTaskNotifyWait(0,0,&xNotifyValue,pdMS_TO_TICKS(50));

		if (encoderS_Handler.samplingDone_flag && (flag_PID || (xNotifyValue == SET))) { //can be used with oppyStart() or semaphore
			encoderS_Handler.samplingDone_flag = RESET;

			//			getCount(&encoderS_Handler); // To get N_left (counts[0]) and N_right (counts[1])
			// Now use the PID with N counts as feedback to get a speed or pwm response

			N_left = encoders->counts[0]; N_right = encoders->counts[1]; // This is because counts[x], may change while PID is in execution but N_x not...
			if(((float)N_left == N_setpoint+correctionFactor)&&((float)N_right ==  N_setpoint)){
				__NOP();
			}
			else{ // only performs PID when setpoint is not matched

				float pidPwm_L = PIDController_Update(&PID_Left, N_setpoint+correctionFactor, (float) N_left);// try lowering setpoint for this well
				float pidPwm_R = PIDController_Update(&PID_Right, N_setpoint, (float) N_right);

				// Modify pwm to correct the trajectory
				oppySetPWM(e_PWM_LEFT, pidPwm_L); // this wheel is over sized
				oppySetPWM(e_PWM_RIGHT,	pidPwm_R);
			}
			if(flag_restart_movement){
				initSum();
				flag_restart_movement = RESET;
				gpio_WritePin(&GPIO_enR, RESET);
				gpio_WritePin(&GPIO_enL, RESET);

			}
			// Sum N_counts to get the traveled distance/angle
			distance = (S_Left*getSum())/1000.0f; // [m]
			// compare with the target distance/angle COMMENT TO PERFORM CONTINUOUS TEST!!
			if ((distance > target) && flag_PID !=SET){
				// each time the semaphore is taken, check distance and give semaphore back when target is reached
				initSum();
				oppyStop();
				distance = RESET;
				xNotifyValue = RESET;
				xTaskNotify(xTaskHandler_PID_core,RESET,eSetValueWithOverwrite);
				xTaskNotify(xTaskHandler_driveOppyTo,SET,eSetValueWithOverwrite); // Notify to driveOppy task
			}

			if (flag_print == SET) {
				// To print N_left and N_right
				clear_string(bufferData);
				portENTER_CRITICAL();
				sprintf(bufferData,"%.2f %.2f %.2f %.2f\n",distance, N_setpoint+correctionFactor,(float)N_left,(float)N_right);
				//sprintf(bufferData,"%.2f %d %d\n", N_setpoint,encoderS_Handler.counts[0],encoderS_Handler.counts[1]);
				usart_writeMsg(&USART_commSerial, bufferData);
				portEXIT_CRITICAL();
				clear_string(bufferData);
			}
		}


	}
}

void vTask_Astar(void* pvParameters){
	uint16_t mapShape[2] = {0};
	uint16_t mapSize = 0;
	uint8_t goalFlag = RESET;
	nodeHandler_t* ptrCurrentNode = NULL;


	while(1){


		if(xTaskNotifyWait(0,0,NULL,pdMS_TO_TICKS(50))){ // Wait for the map to be given

			// Before this, get the map and its size to pass its pointer here
			// Get Map_Handler shape, looks unnecessary
			getMapShape(auxBufferData, mapShape);
			mapSize = mapShape[eROW]*mapShape[eCOL];
			nodeHandler_t* OpenList[mapSize];
			uint16_t openSize = mapSize;
			nodeHandler_t* ClosedList[mapSize];
			nodeHandler_t* naviList[mapSize];
			//			uint16_t navi_index = 0;
			uint16_t closedIndex = 0;
			// Create Chart by pvPortMalloc... remember to free after reach goal
			Map_Handler.Chart = (nodeHandler_t**)pvPortMalloc(mapShape[eROW] * sizeof(nodeHandler_t*)); // Allocate memory for rows
			for (int i = 0; i < mapShape[eROW]; i++) { // Allocate memory for columns
				Map_Handler.Chart[i] = (nodeHandler_t*)pvPortMalloc(mapShape[eCOL] * sizeof(nodeHandler_t));
			}
			getMap(auxBufferData, &Map_Handler, GRID_SIZE);
			// Create open and close lists... by pvPortMalloc...
			//			OpenList = (nodeHandler_t*)pvPortMalloc(sizeof(nodeHandler_t*) * mapSize);
			//			ClosedList = (nodeHandler_t*)pvPortMalloc(sizeof(nodeHandler_t*) * mapSize);
			initializeList((void*)OpenList, mapSize);
			initializeList((void*)ClosedList, mapSize);
			initializeList((void*)naviList, mapSize);

			//set and add start point to open
			SetStartPoint(&Map_Handler,GRID_SIZE);
			OpenList[0] = &Map_Handler.Chart[Map_Handler.startNode[eROW]][Map_Handler.startNode[eCOL]];
			//loop
			while(!goalFlag){
				//current= node in open with the lowest F_cost
				// First sort the array from lowest to highest and then take the first element
				nodeArraySortByF_cost(OpenList, mapSize);
				ptrCurrentNode = OpenList[0];

				//remove current from open
				for(uint16_t i = 0; OpenList[i] != NULL; i++){
					OpenList[i] = OpenList[i+1]; //move each next val to current val
					openSize--; // "reduce" array size (not really...) maybe it is not necessary to modify openSize
				}
				//add current to closed
				ClosedList[closedIndex] = ptrCurrentNode;
				closedIndex++;

				// if current is goalNode then return path array
				if(ptrCurrentNode->start_endFlag == eEndNode){
					// Create task or function who arrays the complete navi path to goal (DONE)
					getTrajectory(&Map_Handler, naviList); // CAUTION: naviList stores path from goalNode to startNode+1, it does not store startNode
					uintptr_t pointer = 0;
					//Find last node in navigationList, last valid node is the first grid to go
					for(uint16_t i = 0; naviList[i] != NULL; i++){
						pointer = (uintptr_t) naviList[i];
						xQueueSendToFront(xQueueHandler_naviList,(void*)&pointer,xBlockTimeMaxExpected);
					}



					// Then create a task for distance calculation and driveOppyThoughPath (DONE)
					// Call that function from PID task, who controls the movement of the robot (DONE)
					// Try to add proximity sensor to oppy
					// Try to create a recalculate path from new starting point protocol as function or task
					// Enjoy

					goalFlag = SET;
					break;
				}
				// for each neighbor of the current node
				//if  neighbor is not walkable or in closed
				//skip to next neighbor
				//if newpath to neighbor is shorter or neighbor is not in open
				// set F_cost to neighbor set parent of neighbor to current
				// if neighbor is not in open
				// add neighbor to open
				fillNodeNeighbors(&Map_Handler, ptrCurrentNode, OpenList);

			}

			//Notify to driveOppy task (NOTIFIED BY TASK MENU)
			//			clear_string(bufferData);
			portENTER_CRITICAL();
			usart_writeMsg(&USART_commSerial,"\n\nMap successfully allocated, use -drive- to start motion\n\n");
			portEXIT_CRITICAL();
			portENTER_CRITICAL();
			usart_writeMsg(&USART_commSerial,"\n\nWARNING: The only way to deallocate Map is using -free- command!\n\n");
			portEXIT_CRITICAL();
			//clear_string(bufferData);

		}
	}


}

void vTask_squareTest(void* pvParameters){
	BaseType_t notifyStatus = {0}; //For debug
	(void) notifyStatus;
	uint32_t notifyValue = {0}; // Drive using eRun - free Heap by eFinish
	uint8_t queueOrder_Left[4] = {eN,eW,eS,eE};
	uint8_t queueOrder_Right[4] = {eN,eE,eS,eW};
	uint8_t queueOrder[4];

	while(1){
		notifyStatus = xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(100));
		if(notifyValue == eSquareLeft || notifyValue == eSquareRight){
			switch(notifyValue){
			case eSquareLeft:
				for (int8_t i = 0; i < 4; i++){
					queueOrder[i] = queueOrder_Left[i];
				}
				break;
			case eSquareRight:
				for (int8_t i = 0; i < 4; i++){
					queueOrder[i] = queueOrder_Right[i];
				}
				break;
			}
			uint16_t size = (2*squareShape[0]-1)+(2*squareShape[1]-3);
			nodeHandler_t* naviSquareList; // size of the perimeter
			naviSquareList = (nodeHandler_t*)pvPortMalloc(size * sizeof(nodeHandler_t));
			uint16_t naviSquareIndex = 0;
			uintptr_t pointer = 0;

			nodeHandler_t TurnOnGrid = {0};
			nodeHandler_t StraightGrid = {0};

			uint8_t currentDir = queueOrder[0]; // Suppose the robot starts pointing to the north

			for (uint16_t i = 0;i < squareShape[0];++i){
				for(uint16_t j = 0; j < squareShape[1];++j){
					// statements to turn the oppy
					if(i == 0){ // when i is 0
						if(j == (squareShape[1]-1)){ // when j is max
							currentDir = queueOrder[1];
							TurnOnGrid.naviHere.direction = currentDir;
							TurnOnGrid.naviHere.gridDistance = gridSize;
							naviSquareList[naviSquareIndex] = TurnOnGrid;
							naviSquareIndex++;
						}
						else{ // in any other case
							StraightGrid.naviHere.direction = queueOrder[0];
							StraightGrid.naviHere.gridDistance = gridSize;

							naviSquareList[naviSquareIndex] = StraightGrid;
							naviSquareIndex++;
						}
					}
					else if(i == (squareShape[0]-1)){ // or when i is max

						if(j == (squareShape[0]-1)){ // when j is max
							currentDir = queueOrder[2];
							TurnOnGrid.naviHere.direction = currentDir;
							TurnOnGrid.naviHere.gridDistance = gridSize;
							naviSquareList[naviSquareIndex] = TurnOnGrid;
							naviSquareIndex++;
						}
						else if (j == 0){ // or when i is 0
							currentDir = queueOrder[3];
							TurnOnGrid.naviHere.direction = currentDir;
							TurnOnGrid.naviHere.gridDistance = gridSize;
							naviSquareList[naviSquareIndex] = TurnOnGrid;
							naviSquareIndex++;

						}
						else{ // in any other case
							StraightGrid.naviHere.direction = queueOrder[2];
							StraightGrid.naviHere.gridDistance = gridSize;

							naviSquareList[naviSquareIndex] = StraightGrid;
							naviSquareIndex++;
						}
					}
					else{ // i different of max or 0
						if(j == 0){
							StraightGrid.naviHere.direction = queueOrder[3];
							StraightGrid.naviHere.gridDistance = gridSize;

							naviSquareList[naviSquareIndex] = StraightGrid;
							naviSquareIndex++;
						}
						else if(j == (squareShape[1]-1)){
							StraightGrid.naviHere.direction = queueOrder[1];
							StraightGrid.naviHere.gridDistance = gridSize;

							naviSquareList[naviSquareIndex] = StraightGrid;
							naviSquareIndex++;
						}
					}

				}
			}

			// After fill the list, fill the motion queue in the right order
			for(uint8_t i = 0; i < 4; i++){
				uint8_t dirFlag = queueOrder[i];
				for(uint16_t n = 0; n < naviSquareIndex; n++){

					if (naviSquareList[n].naviHere.direction == dirFlag) {
						pointer = (uintptr_t) &naviSquareList[n];
						xQueueSendToBack(xQueueHandler_naviList,(void*)&pointer,xBlockTimeMaxExpected);
					}
				}
			}


			portENTER_CRITICAL();
			usart_writeMsg(&USART_commSerial,"\n\nSquare successfully loaded, use -drive- to start motion\n");
			portEXIT_CRITICAL();

			while(1){
				xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
				if(notifyValue == eFinish){
					vPortFree(naviSquareList); // For deallocate the memory
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"naviSquareList deallocated\n\n");
					portEXIT_CRITICAL();
					break; // Go back to the main loop
				}
			}
		}

	}
}
void vTask_driveOppyTo(void* pvParameters){
	// Pass a list with movement type (for, back, right, left) and distance(angle) to drive the spirit

	nodeHandler_t* nodeToGo = NULL;

	float currentAngle = RAD_0; // starts in North
	float angle = 0.0f;

	BaseType_t notifyStatus = {0};
	(void) notifyStatus;
	uint32_t notifyValue = 0;
	UBaseType_t queueStatus = {0};

	while(1){
		notifyStatus = xTaskNotifyWait(0,ULONG_MAX,&notifyValue,portMAX_DELAY);
		queueStatus = uxQueueMessagesWaiting(xQueueHandler_naviList); //To know how many elements holds the queue
		if((notifyValue == SET) && (queueStatus>=0)){ // When semphrHolder == pdTRUE, PID is waiting
			notifyValue = RESET;
			if(queueStatus == 0){
				xTaskNotify(xTaskHandler_squareTest,eFinish,eSetValueWithOverwrite);
				portENTER_CRITICAL();
				usart_writeMsg(&USART_commSerial,"\n\nNowhere to go now, queue empty\n\n");
				portEXIT_CRITICAL();
			}
			else{
				// Retrieve an element from the queue
				xQueueReceive(xQueueHandler_naviList, &nodeToGo, pdMS_TO_TICKS(50));
				// start using the first node to go there
				switch(nodeToGo->naviHere.direction){
				case eN:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to NORTH\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == RAD_0){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(RAD_0, currentAngle); // Holds the real angle to travel
						currentAngle = RAD_0;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eS:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to SOUTH\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == RAD_180 || currentAngle == RAD_N180){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						if (currentAngle < 0) {
							angle = angleCalculation(RAD_N180, currentAngle); // Holds the real angle to travel
							currentAngle = RAD_N180;
						}
						else{
							angle = angleCalculation(RAD_180, currentAngle); // Holds the real angle to travel
							currentAngle = RAD_180;
						}
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eW:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to WEST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == -RAD_90){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(-RAD_90, currentAngle); // Holds the real angle to travel
						currentAngle = -RAD_90;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eE:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to EAST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == RAD_90){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(RAD_90, currentAngle); // Holds the real angle to travel
						currentAngle = RAD_90;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eNW:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to NORTH-WEST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == -RAD_45){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(-RAD_45, currentAngle); // Holds the real angle to travel
						currentAngle = -RAD_45;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eNE:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to NORTH-EAST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == RAD_45){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(RAD_45, currentAngle); // Holds the real angle to travel
						currentAngle = RAD_45;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eSW:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to SOUTH-WEST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == -RAD_135){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(-RAD_135, currentAngle); // Holds the real angle to travel
						currentAngle = -RAD_135;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							//						vTaskDelay(motionDelay); // wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
						//					vTaskDelay(motionDelay);
					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;
				case eSE:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDriving to SOUTH-EAST\n\n");
					portEXIT_CRITICAL();
					if(currentAngle == RAD_135){
						// If angle remains unchanged perform one step {move straight}
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);

					}
					else{
						// If angle change perform two steps {turn, move straight}
						angle = angleCalculation(RAD_135, currentAngle); // Holds the real angle to travel
						currentAngle = RAD_135;
						if(angle < 0){ // If angle lower than zero turn left
							distance = RESET;
							target = ((-1.0f)*angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_LEFT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay);
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							// wait until PID take the semaphore
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						else if(angle>0){ // If angle greater than zero trun right
							distance = RESET;
							target = (angle*(L/2.0f))/1000.0f; // [m]
							oppyTurn(e_TURN_RIGHT);
							flag_restart_movement = SET;
							initSum();
							vTaskDelay(motionDelay); // wait until PID take the semaphore
							xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);
							while(!notifyValue){
								//wait until the semaphore is back to perform the next step
								xTaskNotifyWait(0,ULONG_MAX,&notifyValue,pdMS_TO_TICKS(50));
							}
						}
						notifyValue = RESET;
						// second step is move straight
						target = nodeToGo->naviHere.gridDistance;
						distance = RESET;
						oppyAdvanceTo(e_ADVANCE_TO_FORWARD); // config oppy to move forward
						flag_restart_movement = SET;
						initSum();
						vTaskDelay(motionDelay);
						xTaskNotify(xTaskHandler_PID_core,SET, eSetValueWithOverwrite);

					}
					if (flag_driveOppyToPrint) {
						clear_string(bufferData);
						portENTER_CRITICAL();
						sprintf(bufferData,"Angle set to %.2f\n",angle*(180/PI));
						usart_writeMsg(&USART_commSerial,bufferData);
						portEXIT_CRITICAL();
						clear_string(bufferData);
					}
					break;

				default:
					portENTER_CRITICAL();
					usart_writeMsg(&USART_commSerial,"\n\nDirection unknown...\n\n");
					portEXIT_CRITICAL();
					break;
				}// switch end
			} // if queue available end
		}
		else{}

	} // main loop end
}


//void vTask_gyroGetID(void* pvParameters){
//
//	BaseType_t notify_status = {0};
//
//	while(1){
//		notify_status = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
//
//		if(notify_status == pdTRUE){
//
//			clear_string(bufferData);
//			sprintf(bufferData,"ID: %d\n\r",(int)L3GyroReadReg(&L3G4200D, GYRO_FIFO_SRC_REG));
//			usart_writeMsg(&USART_commSerial, (char *) bufferData);
//			clear_string(bufferData);
//		}
//	}
//}

//void vTask_gyroGetData(void* pvParameters){
//	BaseType_t notify_status = {0};
//
//	while(1){
//		notify_status = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
//
//		if(notify_status == pdTRUE){
//			taskENTER_CRITICAL();
//			L3Gyro_read(&L3G4200D, gyroBuffer);
//			taskEXIT_CRITICAL();
//		}
//	}
//
//}

/* common functions */

void process_command(command_t *cmd){
	extract_command(cmd);

	switch(next_state){
	case sMainMenu:
		// Notify to the right task
		xTaskNotify(xTaskHandler_menu,(uint32_t)cmd, eSetValueWithOverwrite);
		break;
	case sLedEffect:
		//		xTaskNotify(xTaskHandler_blinkyMode, (uint32_t) 1, eNoAction);
		break;
	case sRtcMenu:
		//xTaskNotify(xHandler_rtc, (uint32_t)cmd, eSetValueWithOverwrite);
		break;
	default:
		__NOP();
		break;
	}
}
int extract_command(command_t *cmd){
	uint8_t item;
	uint16_t counter_j = 0;
	BaseType_t status;

	status = uxQueueMessagesWaiting(xQueueHandler_inputData);
	if(status == 0){
		return -1;
	}

	do{
		// retrieve an element and add it to item
		status = xQueueReceive(xQueueHandler_inputData, &item,0);
		if(status == pdTRUE){
			// Fill the command array
			cmd->payload[counter_j++] = item;
		}
	}while(item !='\r');

	cmd->payload[counter_j-1] = '\0';
	cmd->len = counter_j -1; // command length

	return 0;

}

void clear_string(char *string){

	for (int i = 0; i < sizeof(string); i++) {
		string[i] = 0;
	}
}

uint16_t arraySortAndMedian(uint16_t* arr, uint8_t arraySize) {

	for (uint8_t i = 0; i < arraySize - 1; i++) {
		uint8_t min_idx = i;
		for (uint8_t j = i + 1; j < arraySize; j++) {
			if (arr[j] < arr[min_idx]) {
				min_idx = j;
			}
		}
		// Swap the found minimum element with the first element
		uint8_t temp = arr[min_idx];
		arr[min_idx] = arr[i];
		arr[i] = temp;
	}

	return arr[(arraySize-1)/2];
}
void freeMatrix(mapHandler_t* m) { // NOT USED IN Heap_4
	// Free each column
	for (int i = 0; i < m->MapRows; i++) {
		vPortFree(m->Chart[i]);
	}
	// Free the row pointers
	vPortFree(m->Chart);
}
float angleCalculation(float nextAngle, float prevAngle){
	float aux = 0;
	aux = nextAngle - prevAngle;

	if (abs(aux)>RAD_180) {
		if(aux > RAD_180){ // x > target
			// For positive values
			return RAD_180 - aux;
		}
		else if(aux < RAD_N180){ // x< -target
			//For negative values
			return (-1.0f)*aux - RAD_180;
		}
	}

	return aux-(PI/180.0f)*2.4f;// -5° == -0.0872665
}
