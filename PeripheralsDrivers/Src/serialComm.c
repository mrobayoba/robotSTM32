/*
 * serialComm.c
 *
 *  Created on: Mar 21, 2024
 *      Author: matto
 */
#include "serialComm.h"

uint8_t ReceivedChar;

//Private Function Prototypes
uint8_t isNumber(char Char);		// To verify if the char is a number (ASCII)
uint8_t isLetter(char Char);		// To verify if the char is a letter (ASCII)
uint8_t isENTER(char Char);		// To verify the finish command key \r (ASCII)
//uint8_t isAtSign(char Char);		// To verify the finish command key @ (ASCII)
uint8_t isSpace(char Char);			// To verify if the char is space character (ASCII)
uint8_t isPoint(char Char);

void serialCommInit(serialComm_Handler_t *ptrserialComm_Handler){
	gpio_Config(ptrserialComm_Handler->pinRx);
	gpio_Config(ptrserialComm_Handler->pinTx);
	usart_Config(ptrserialComm_Handler->commSerial);
}


uint8_t isNumber(char Char){
	if(Char>='0' && Char<='9'){
		return 1;
	}

	else{
		return 0;
	}
}
uint8_t isLetter(char Char){

	if((Char>='A' && Char<='Z') ||
		(Char>='a' && Char<='z')){
		return 1;
	}
	else{
		return 0; // Can be put it out the else, because return breaks the function execution
	}
}
uint8_t isENTER(char Char){

	if(Char=='\r'){
		return 1;
	}
	else{
		return 0; // Can be put it out the else, because return breaks the function execution
	}
}
//uint8_t isAtSign(char Char){ // @ == At sign
//
//	if(Char=='@'){
//		return 1;
//	}
//	else{
//		return 0; // Can be put it out the else, because return breaks the function execution
//	}
//}

uint8_t isSpace(char Char){ // ' ' == Space character

	if(Char==32){
		return 1;
	}
	else{
		return 0; // Can be put it out the else, because return breaks the function execution
	}
}
uint8_t isPoint(char Char){ // ' ' == Space character

	if(Char==46){
		return 1;
	}
	else{
		return 0; // Can be put it out the else, because return breaks the function execution
	}
}

void clear_string(char *string){

	for (int i = 0; i < sizeof(string); i++) {
		string[i] = 0;
	}
}

void communication_f(serialComm_Handler_t *ptrserialComm_Handler, char * ptrCommandBuffer){
	uint8_t commandFlag = RESET;

	if (ReceivedChar) {
		if (isLetter((char) ReceivedChar)) {
			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
			strncat(ptrCommandBuffer, (char*) &ReceivedChar, 1); // appends the first n characters of string2 to string1

			// Uncomment to command only with letters
//			usart_WriteChar(&commSerial, '\n');
//			usart_WriteChar(&commSerial, '\r');
//			commandFlag = SET;
		}
		else if (isNumber((char) ReceivedChar)) { // comment to command only with letters
			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
			strncat(ptrCommandBuffer, (char*) &ReceivedChar, 1); // appends the first n characters of string2 to string1
//			commandFlag = SET;
		}
		else if (isSpace((char) ReceivedChar)) { // comment to command only with letters
			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
			strncat(ptrCommandBuffer, (char*) &ReceivedChar, 1); // appends the first n characters of string2 to string1
//			commandFlag = SET;
		}
		else if (isPoint((char) ReceivedChar)) { // comment to command only with letters
			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
			strncat(ptrCommandBuffer, (char*) &ReceivedChar, 1); // appends the first n characters of string2 to string1
//			commandFlag = SET;
		}

		else if (isENTER((char) ReceivedChar)) { // comment to command only with letters
			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
			usart_WriteChar(ptrserialComm_Handler->commSerial, '\n');
			usart_WriteChar(ptrserialComm_Handler->commSerial, '\r');
			commandFlag = SET;
		}
//		else if (isAtSign((char) ReceivedChar)) { // comment to command only with letters
//			usart_WriteChar(ptrserialComm_Handler->commSerial, ReceivedChar);
//			usart_WriteChar(ptrserialComm_Handler->commSerial, '\n');
//			usart_WriteChar(ptrserialComm_Handler->commSerial, '\r');
//			commandFlag = SET;
//		}
		ReceivedChar = 0;
	}

	if (commandFlag) {
		commandFlag = RESET;
		analyzeCommand(ptrserialComm_Handler,ptrCommandBuffer);
		clear_string(ptrCommandBuffer);
	}
}

__attribute__ ((weak)) void help_menu(serialComm_Handler_t *ptrserialComm_Handler){

	usart_writeMsg(ptrserialComm_Handler->commSerial, "Help Command Menu: \n");

	usart_writeMsg(ptrserialComm_Handler->commSerial,
			"1)  help -> Imprimir menu de ayuda \n");

	usart_writeMsg(ptrserialComm_Handler->commSerial,
			/*"17)   p -> Imprimir valor actual en milivoltios\n"*/
			"2) test -> Testing...\n");

}

__attribute__ ((weak)) void analyzeCommand(serialComm_Handler_t *ptrserialComm_Handler, char * ptrCommandBuffer){

	if (strcmp(ptrCommandBuffer, "help") == 0) { // Help menu
		clear_string(ptrCommandBuffer);
		help_menu(ptrserialComm_Handler);
	}
	else if(strcmp(ptrCommandBuffer, "test") == 0){ // Print current_value as milivolt
		clear_string(ptrCommandBuffer);
		usart_writeMsg(ptrserialComm_Handler->commSerial, "Testing... \r\n");
	}

}

void usart2_RxCallback(void){ // For serial communication with PC
	 ReceivedChar= usart_getRxData();
}

void usart6_RxCallback(void){ // For serial communication with PC
	ReceivedChar = usart_getRxData();
}

void usart1_RxCallback(void){ // For serial communication with PC
	ReceivedChar = usart_getRxData();
}
