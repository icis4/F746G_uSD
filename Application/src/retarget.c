/**
 * @file retarget.c
 *
 * @date Created on: Feb 23, 2021
 *      Author: icis4
 */

#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "usart.h"

int _write(int file, char *ptr, int len)
{
	UNUSED(file);

	HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 100);

	return len;
}

