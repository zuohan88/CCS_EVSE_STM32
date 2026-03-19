/*
 * AppControl.c
 *
 *  Created on: Feb 8, 2025
 *      Author: zuoha
 */


#include <AppADC.h>
#include <AppADC.h>
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

#include "qca7000.h"
#include "tcp.h"
#include "udp.h"
#include "homeplug.h"

//external Object
//extern uint8_t NMK_condition;
//extern adc_measure adc_measured;


//local Active Object
extern AOControl 			AO_Control;

extern adc_measure 		adc_measured;
extern AOControl 			AO_Control;

extern QCA7000				QCA1;

extern homeplug 			homeplug1;
extern udp 				udp1;
extern tcp 				tcp1;
extern evse 				evse1;
extern module 				module1;
extern module 				system1;

extern AppCanBusTask AppCanBusTask1;
//local function

void AppControl_Timer_Task(void);

void AppControl_Init(void){
	 memset(&AO_Control, 0, sizeof(AO_Control));
	 RingBuf3_ctor(&AO_Control.rb, AO_Control.rb_buf,sizeof(RingBufElementControl), ARRAY_NELEM(AO_Control.rb_buf));
	 if(TimeEvent_new_Timer(&AO_Control.Timer)){
		 addToTrace("[Timer] failed Initializing timer for Task_test");
	 }
	 QCA7000_shutDown(&QCA1);
}

void AppControl_Timer_Task(void){
	static RingBufElementControl tempRB ={0};
	tempRB.control = boot;
	RingBuf3_put(&AO_Control.rb, &tempRB);
}
void AppControl_Timer_Task_RESET(void){
	static RingBufElementControl tempRB ={0};
	tempRB.control = start;
	RingBuf3_put(&AO_Control.rb, &tempRB);
}
void AppControl_Timer_Task_boot2(void){
	static RingBufElementControl tempRB ={0};
	tempRB.control = boot2;
	RingBuf3_put(&AO_Control.rb, &tempRB);
}
void AppControl_Timer_Task_boot3(void){
	static RingBufElementControl tempRB ={0};
	tempRB.control = boot3;
	RingBuf3_put(&AO_Control.rb, &tempRB);
}
void AppControl_Timer_Task_NMK(void){
	static RingBufElementControl tempRB ={0};
	tempRB.control = check_NMK;
	RingBuf3_put(&AO_Control.rb, &tempRB);
}


void AppControl_Task(void){
	uint8_t result = 0;
	RingBufElementControl tempRB ={0};
	if(true == RingBuf3_get(&AO_Control.rb, &tempRB)){
		switch (tempRB.control){
			case idle:		//read module info
			case stop:
			case estop:{
				AO_Control.errorStartUp = 0;
				AppCanBusTask_stopTask(&AppCanBusTask1);
				QCA7000_shutDown(&QCA1);
				evseStateMachine_Stop(&evse1);
				TimeEvent_disarm(AO_Control.Timer);
				AppADC_PilotCtrl(1,0);	//make it to stateF
				break;
			}
			case reset:{
				addToTrace("[AppControl]reset state");
				QCA7000_shutDown(&QCA1);
				TimeEvent_arm(AO_Control.Timer,1000, 0,AppControl_Timer_Task_RESET);
				break;
			}
			case start:{
				addToTrace("[AppControl]start state");
				QCA7000_powerUp(&QCA1);
				TimeEvent_arm(AO_Control.Timer,2000, 0,AppControl_Timer_Task);
				homeplug1.NMK_condition = 0;
				//homeplug_reset(&homeplug1);


				break;
			}
			case boot:{
				addToTrace("[AppControl]boot state");
				result = QCA7000_startUp2(&QCA1);
				if(result ==1){
					TimeEvent_arm(AO_Control.Timer,1000, 0,AppControl_Timer_Task_boot2);
				}else{
					AO_Control.errorStartUp++;
					if(AO_Control.errorStartUp>100){
						addToTrace("StartUp Failed over 100");
						tempRB.control = stop;
						RingBuf3_put(&AO_Control.rb, &tempRB);
						addToTrace("[AppControl]goto stop");
					}else{
						tempRB.control = reset;
						RingBuf3_put(&AO_Control.rb, &tempRB);
						addToTrace("[AppControl]goto reset");
					}
				}
				break;
			}
			case boot2:{
				homeplug_runEvseSlacHandler(&homeplug1);							//set nmk MODEM for once only
				TimeEvent_arm(AO_Control.Timer,1000, 0,AppControl_Timer_Task_boot3);
			}
			case boot3:{
				homeplug_runEvseSlacHandler(&homeplug1);
				//homeplugInit();
				evseStateMachine_Init(&evse1);
				addToTrace("[AppControl]goto check NMK");
				TimeEvent_arm(AO_Control.Timer,1000, 0,AppControl_Timer_Task_NMK);
				AO_Control.countNMK = 0;
				AO_Control.errorNMK = 0;
			}
			case check_NMK:{
				addToTrace("[AppControl]check NMK state");
				if(homeplug1.NMK_condition == 1){			//check NVM setting failed
					homeplug1.NMK_condition = 0;

					AO_Control.errorNMK++;
					if(AO_Control.errorNMK>100){
						addToTrace("[AppControl]NMK Failed over 100");
						tempRB.control = stop;					//
						RingBuf3_put(&AO_Control.rb, &tempRB);
						addToTrace("[AppControl]goto stop");
					}else{
						addToTrace("[AppControl]NMK Failed and try again");
						tempRB.control = reset;					//
						RingBuf3_put(&AO_Control.rb, &tempRB);
					}
					addToTrace("[AppControl]NMK Failed");
					tempRB.control = reset;					//
					RingBuf3_put(&AO_Control.rb, &tempRB);
				}else if(homeplug1.NMK_condition == 2){
					addToTrace("[AppControl]NMK passed");
					AppADC_PilotCtrl(1,100);	//100
					tempRB.control = check_CP;
					RingBuf3_put(&AO_Control.rb, &tempRB);
				}else{			//if 0
					AO_Control.countNMK++;
					if(AO_Control.countNMK>100){
						addToTrace("[AppControl]NMK over count");
						tempRB.control = stop;					//
						RingBuf3_put(&AO_Control.rb, &tempRB);
						addToTrace("[AppControl]goto stop");
					}else{
						TimeEvent_arm(AO_Control.Timer,1000, 0,AppControl_Timer_Task_NMK);
					}
				}
				break;
			}
			case check_CP:{
				if(adc_measured.CP_State == StateA){			//+12V
					//addToTrace("ADC StateA");
					tempRB.control = check_CP;
				}else if(adc_measured.CP_State == StateB){			//reset all state when 9 v //detected car
					//addToTrace("ADC StateB, detected car");
					AppADC_PilotCtrl(1,5);		//start 5% PWM
					evseStateMachine_Init(&evse1);		//got delay
					AppCanBusTask_energyCounterReset(&AppCanBusTask1);			//reset energy
					AppCanBusTask_startTask(&AppCanBusTask1);
					tempRB.control = run;
				}else if(adc_measured.CP_State == StateC){			//6V ,Connected and ready for charging, ventilation is not required
					//addToTrace("ADC StateC, ready for charge");
					tempRB.control = check_CP;
				}else if(adc_measured.CP_State == StateD){			//3V,Connected, ready for charging and ventilation is required
					//addToTrace("ADC StateD, ready for charge");
					tempRB.control = check_CP;
				}else if(adc_measured.CP_State == StateE){			//0v,Electrical short to earth on the controller of the EVSE, no power supply
					//addToTrace("ADC StateE, shorted to ground");
					tempRB.control = check_CP;
				}else if(adc_measured.CP_State == StateF){		//-12V,EVSE is unavailable
					//addToTrace("ADC StateF, EVSE is unavailable");
					tempRB.control = check_CP;
				}
				RingBuf3_put(&AO_Control.rb, &tempRB);
				break;
			}
			case run:{
				if(adc_measured.CP_State == StateA){			//no car here

				}else if(adc_measured.CP_State == StateB){		//got car not yet charge

				}else if(adc_measured.CP_State == StateC){		//6V ,Connected and ready for charging, ventilation is not required

				}else if(adc_measured.CP_State == StateD){		//3V,Connected, ready for charging and ventilation is required

				}else if(adc_measured.CP_State == StateE){		//0v,Electrical short to earth on the controller of the EVSE, no power supply

				}else if(adc_measured.CP_State == StateF){		//-12V,EVSE is unavailable

				}
				tempRB.control = run;					//keep running
				RingBuf3_put(&AO_Control.rb, &tempRB);
				break;
			}
			default:
				break;
		}
	}
}
