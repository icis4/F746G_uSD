/*
 * task_uart.c
 *
 *  Created on: 27.04.2018
 *      Author: Ivaylo Ilchev iic@melexis.com
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "gpio.h"

#include "fatfs.h"

//extern void Startup(void);

void __attribute__ ((noreturn)) StartDefaultTask(void const * argument)
{
	FIL ftest;
//	Startup();
	printf("\n*** System Ready ***\n");
	printf("retSD:%d\n", retSD);
	scan_files(stdout, "0:/");
	printf("\n-----------------------------------\n");

	f_open(&ftest, "0:/ftest.txt", FA_CREATE_ALWAYS | FA_WRITE);
	for (int i = 0; i < 2000; i ++) {
		f_printf(&ftest, "%d:%030d\n", i, i);
	}
	f_close(&ftest);

	for (;;) {
		HAL_GPIO_WritePin(ARDUINO_SCK_D13_LD1_GPIO_Port, ARDUINO_SCK_D13_LD1_Pin, GPIO_PIN_SET);
		osDelay(50);
		HAL_GPIO_WritePin(ARDUINO_SCK_D13_LD1_GPIO_Port, ARDUINO_SCK_D13_LD1_Pin, GPIO_PIN_RESET);
		osDelay(950);
	}
}

