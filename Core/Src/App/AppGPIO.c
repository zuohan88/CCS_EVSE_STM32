/*
 * AppGPIO.c
 *
 *  Created on: Feb 11, 2025
 *      Author: zuoha
 */

#include <AppADC.h>
#include <AppADC.h>
#include <AppCanBus.h>
#include <AppCanBus.h>
#include "main.h"
#include "app.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <TimeEvent.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "serial.h"
#include "cdc_serial.h"
#include "ccs32_globals.h"
#include "usbd_cdc_acm_if.h"
#include "evseStateMachine.h"
#include "app_func.h"
#include "ring_buf3.h"
#include "ring_buf_type.h"
#include "evsePredefine.h"
#include "projectExiConnector.h"
#include "AppControl.h"
#include "AppADC.h"
#include "AppSerial.h"
#include "AppCanBus.h"
#include "AppGPIO.h"

extern AOControl AO_Control;

void Task_Ebutton(void){
	static uint8_t latch = 0;
	RingBufElementControl tempRB ={0};
	if(HAL_GPIO_ReadPin (INPUT_1_GPIO_Port, INPUT_1_Pin)){
		if(!latch){
			latch = 1;
			tempRB.control = stop;
			//RingBuf3_put(&AO_Control.rb, &tempRB);
			addToTrace("[EButton]Pressed");
			//evseStateMachine_Stop();
		}
	}else{
		latch = 0;
	}
}
