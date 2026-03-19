/*
 * AppSerial.c
 *
 *  Created on: Feb 9, 2025
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

#include "qca7000.h"
#include "tcp.h"
#include "udp.h"
#include "homeplug.h"

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart5;
extern host host_com;


AppADC 				AppADC1;
adc_measure 		adc_measured;
AOControl 			AO_Control;

QCA7000				QCA1;
//QCA7000_SPIData  	SPIData1;
//QCA7000_ETHData 	ETHData1;
homeplug 			homeplug1;
udp 				udp1;
tcp 				tcp1;
evse 				evse1;
module 				module1;
module 				system1;
//**********************************************************************************************************************
// sensor read and trigger and send alarm
//**********************************************************************************************************************
static uint8_t Timer_Read_EVCC_Data = 0;
extern char strTmp[STR_TMP_SIZE];

extern USBD_HandleTypeDef hUsbDevice;//hUsbDeviceFS;	//from usb_device
extern host host_com;


//locall function
int CDC_RUN_Ava(void);
int CDC_RUN_Char(char *s);
int UART_RUN_Ava(void);
int UART_RUN_Char(char *s);
int Process_Normal_123(char* msg);

//***************************************************************
//ISR function
//***************************************************************
void mySerialPrint(void) {
  uasrt_write_1(mySerialPrintOutputBuffer,strlen(mySerialPrintOutputBuffer));
  //usart_send_string_1(mySerialPrintOutputBuffer);
  //#define TIMEOUT_100_MS 100
  //HAL_UART_Transmit(&huart5, (uint8_t*)mySerialPrintOutputBuffer, strlen(mySerialPrintOutputBuffer), TIMEOUT_100_MS);
  CDC_write_0(mySerialPrintOutputBuffer,strlen(mySerialPrintOutputBuffer));
  //memset(mySerialPrintOutputBuffer,0,100);
}



void AppSerial_Init(void){
	 if(TimeEvent_new_Timer(&Timer_Read_EVCC_Data));
}

void AppSerial_SerialTask(void){
	char dummy[100];
	while(UART_RUN_Ava()){
		if(UART_RUN_Char(dummy)){
			Process_Normal_123(dummy);
		}
	}
}
void AppSerial_CDCTask(void){
	char dummy[100];
	while(CDC_RUN_Ava()){
		if(CDC_RUN_Char(dummy)){
			Process_Normal_123(dummy);
		}
	}
}

void AppSerial_CheckUSB(void){
	RingBufElementControl tempRB ={0};
	static uint8_t latch = 1;
	if((hUsbDevice.dev_state == USBD_STATE_CONFIGURED) && host_com.port_0  && host_com.port_1){
		if(latch){
			addToTrace("USB connected");
		}
		latch = 0;
	}else{
		if(!latch){
			tempRB.control = stop;					//ask control to stop
			RingBuf3_put(&AO_Control.rb, &tempRB);
			addToTrace("USB disconnected");
		}
		latch = 1;
	}
}




//ISR end
int CDC_RUN_Ava(void){
	return CDC_rx_buffsize_1();
}


int UART_RUN_Ava(void){
	return usart_read_buff_1();
}
//****************************************
//UART RUN 12 for serial read
//****************************************
int CDC_RUN_Char(char *s){
	static char  msg[100];
	static uint8_t i=0;
	char c;
	int r = 0;
	if(CDC_rx_buffsize_1()){		//if rx ringbuff got data
		while (CDC_rx_readbuff_1(&c,1) == 1){
			if(isalpha(c)||isalnum(c)||c == ':'||c == '\r' || c == '\n'){				//ascii data
				msg[i] = c;
				if(i<90){
					i++;
				}
				if(msg[0] != ':'){
					i = 0;
				}
				if(i > 3){
					if(msg[i-1] == '\n'){
						if(msg[0] == ':'&& msg[i-2] == '\r'){		//find last time
							my_memcpy(s,msg,i);
							r = 1;
						}
						i = 0;	//reset word count(i)
					}
				}
			}else{						//got none ascii data
				i = 0;					//reset the array
			}
		}
	}
	return r;
}

int UART_RUN_Char(char *s){
	static char  msg[100];
	static uint8_t i=0;
	char c;
	int r = 0;
	if(usart_read_buff_1()){		//if rx ringbuff got data
		while (usart_read_char_buff_1(&c) == 1){
			if(isalpha(c)||isalnum(c)||c == ':'||c == '\r' || c == '\n'){				//ascii data
				msg[i] = c;
				if(i<90){
					i++;
				}
				if(i > 3){
					if(msg[i-1] == '\n'){
						if(msg[0] == ':'&& msg[i-2] == '\r'){		//find last time
							my_memcpy(s,msg,i);
							r = 1;
						}
						i = 0;	//reset word count(i)
					}
				}
			}else{						//got none ascii data
				i = 0;					//reset the array
			}
		}
	}
	return r;
}

void Task_Read_EVCC_Data(evse* const me){
	addToTrace("EVCC_Data");

	//int16_t MaxAmp;		//by EVCC request Charge Parameter Discovery and current demand
	//int16_t MaxPow; 		//by EVCC request Charge Parameter Discovery and current demand
	//int16_t MaxVolt; 	//by EVCC request Charge Parameter Discovery and current demand

	//request
	//int32_t TargetVolt;		//by EVCC request precharger and current demand
	//int32_t TargetAmp;		//by EVCC request precharger and current demand

	//request
	//int8_t  FullSOC;			//Charge Parameter Discovery
	//int8_t  BulkSOC;			//Charge Parameter Discovery
	//int16_t EnergyCapacity;		//Charge Parameter Discovery
	//int16_t	EnergyRequest;		//Charge Parameter Discovery

	//EV status
	//uint8_t RESSSOC;			//State of charge of the EV’s battery (RESS)
	//uint8_t Ready;				//charger parameter, precharge, current demand, power delivery
	//uint8_t ErrorCode;
	//int32_t RESSConditioning;
	//int32_t CabinConditioning;

	//int32_t ChargingComplete;		//current demand
	//int32_t BulkChargingComplete;	//current demand
	//int32_t RemainingTimeToBulkSoC;	//current demand
	//int32_t RemainingTimeToFullSoC; //current demand
	McuXFormat_xsprintf(strTmp, "[EVCC][1]%d,%d,%d", me->EVCC.MaxAmp, me->EVCC.MaxPow, me->EVCC.MaxVolt);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVCC][2]%ld,%ld", me->EVCC.TargetVolt, me->EVCC.TargetAmp);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVCC][3]%d,%d,%d,%d",me->EVCC.FullSOC, me->EVCC.BulkSOC,me->EVCC.EnergyCapacity, me->EVCC.EnergyRequest);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVCC][4]%u,%u,%u,%ld,%ld",me->EVCC.RESSSOC,me->EVCC.Ready,me->EVCC.ErrorCode,me->EVCC.RESSConditioning,me->EVCC.CabinConditioning);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVCC][5]%ld,%ld,%ld,%ld",me->EVCC.ChargingComplete,me->EVCC.BulkChargingComplete,me->EVCC.RemainingTimeToBulkSoC,me->EVCC.RemainingTimeToFullSoC);
	addToTrace(strTmp);

	//int16_t MaxPow;		//by system set by user//
	//int16_t MaxVolt;	//by system
	//int16_t MinVolt;	//by system
	//int16_t MaxAmp;		//by system
	//int16_t MinAmp;		//by system

	//int16_t MaxPowX;		//by system set by user//
	//int16_t MaxVolX;		//by system
	//int16_t MinVoltX;		//by system
	//int16_t MaxAmpX;		//by system
	//int16_t MinAmpX;		//by system

	//int16_t NowVolt;			//sensed voltage
	//int16_t NowAmp;				//sensed voltage

	//int16_t NowVoltX;		//sensed voltage
	//int16_t NowAmpX;		//sensed voltage

	//dinDC_EVSEStatusCodeType EVSEStatusCode;
	//dinisolationLevelType IsolationStatus;

	//uint8_t fully_charged;
	//uint8_t	pluged;
	//uint32_t total_charge_time;
	//uint32_t over_time;
	McuXFormat_xsprintf(strTmp, "[EVSE][1]%d,%d,%d,%d,%d",me->EVSE.MaxPow,me->EVSE.MaxVolt,me->EVSE.MinVolt,me->EVSE.MaxAmp,me->EVSE.MinAmp);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVSE][2]%d,%d,%d,%d,%d",me->EVSE.MaxPowX,me->EVSE.MaxVolX,me->EVSE.MinVoltX,me->EVSE.MaxAmpX,me->EVSE.MinAmpX);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVSE][3]%d,%d,%d,%d",me->EVSE.NowVolt,me->EVSE.NowAmp,me->EVSE.NowVoltX,me->EVSE.NowAmpX);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVSE][4]%d,%d",me->EVSE.EVSEStatusCode,me->EVSE.IsolationStatus);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[EVSE][5]%u,%u,%lu,%lu",me->EVSE.fully_charged,me->EVSE.pluged,me->EVSE.total_charge_time,me->EVSE.over_time);
	addToTrace(strTmp);

	//int16_t MaxPow;		//by system set by user// this need to confirm as w or Kw
	//int16_t MaxVolt;	//by system
	//int16_t MinVolt;	//by system
	//int16_t MaxAmp;		//by system
	//int16_t MinAmp;		//by system

	McuXFormat_xsprintf(strTmp, "[USER][1]%d,%d,%d,%d,%d",me->USER.MaxPow,me->USER.MaxVolt,me->USER.MinVolt,me->USER.MaxAmp,me->USER.MinAmp);
	addToTrace(strTmp);
	//typedef struct {
	//	float V12;
	//	float temperture_MCP9700A;
	//	float temperture_MCU;
	//	float output_voltage;
	//	float output_current;
	//	float pt1000_1;
	//	float pt1000_2;
	//	float ground_test_1;
	//	float ground_test_2;
	//	float pilot_voltage;
	//	State CP_State;
	//	State Previous_CP_State;
	//	uint32_t rate;
	//	uint32_t last_tick;
	//}adc_measure;

	McuXFormat_xsprintf(strTmp, "[ADC][1]%f,%f,%f,%f,%f",adc_measured.V12,adc_measured.temperture_MCP9700A,adc_measured.temperture_MCU,adc_measured.output_voltage,adc_measured.output_current);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[ADC][2]%f,%f,%f,%f,%f",adc_measured.pt1000_1,adc_measured.pt1000_2,adc_measured.ground_test_1,adc_measured.ground_test_2,adc_measured.pilot_voltage);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[ADC][3]%d,%lu,%lu",adc_measured.CP_State,adc_measured.rate,adc_measured.last_tick);
	addToTrace(strTmp);

	 //uint32_t VomaxHi,VominHi,PrateHi;			//i need divide by 10, P need to time 1k
	 //float IomaxHi;
	 //uint32_t Vab10,Vbc10,Vca10;										//need divide by 10
	 //float voltage,current,power,energy;
	 //uint32_t voltage32,current32,power32,energy32;
	 //uint8_t energy_reset,energy32_reset;
	 //uint32_t set_milivolt,set_miliamp;
	 //uint32_t setting_milivolt,setting_miliamp;
	 //uint8_t number;
	 //int8_t temperture;
	 //condition2 condition2;
	 //condition1 condition1;
	 //condition0 condition0;
	 //uint8_t serial_number;
	 //uint8_t available;
	 //uint32_t last_tick;
	 //uint32_t last_tick_vc;
	 //uint32_t last_tick_vc32;
	 //uint32_t last_tick_3ph;
	 //uint32_t last_tick_condition;
	 //uint32_t last_tick_VA;
	 //uint32_t last_tick_MaxOutput;

	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][1]%lu,%lu,%lu,%lu",module1.VomaxHi,module1.VominHi,module1.IomaxHi,module1.PrateHi);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][2]%lu,%lu,%lu",module1.Vab10,module1.Vbc10,module1.Vca10);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][3]%f,%f,%f,%f",module1.voltage,module1.current,module1.power,module1.energy);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][4]%lu,%lu,%lu,%lu",module1.milivolt32,module1.miliamp32,module1.power32,module1.energy32);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][5]%u,%u,%lu,%lu",module1.energy_reset,module1.energy32_reset,module1.set_milivolt,module1.set_miliamp);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][6]%lu,%lu,%u,%d",module1.setting_milivolt,module1.setting_miliamp,module1.number,module1.temperture);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][7]%u,%u,%u,%u",module1.condition2.flags,module1.condition1.flags,module1.condition0.flags,module1.available);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][8]%lu,%lu,%lu,%lu",module1.last_tick,module1.last_tick_vc,module1.last_tick_vc32,module1.last_tick_3ph);
	addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp, "[CANBUS][MODULE1][9]%lu,%lu,%lu",module1.last_tick_condition,module1.last_tick_VA,module1.last_tick_MaxOutput);
	addToTrace(strTmp);


}

int Process_Normal_123(char* msg){
	RingBufElementControl tempRB ={0};
	char command_temp[] = {msg[1],msg[2]};		//2 byte
	uint16_t command = atoi(command_temp);	//2 number become command
	//uint8_t command = msg[1];
	switch (command){
		//start
		case 0:
			addToTrace("START SYSTEM");
			tempRB.control = start;
			RingBuf3_put(&AO_Control.rb, &tempRB);
			break;
		//stop
		case 1:
			addToTrace("STOP SYSTEM");
			tempRB.control = stop;
			RingBuf3_put(&AO_Control.rb, &tempRB);
			break;
		//reset
		case 2:
			addToTrace("RESET SYSTEM");
			tempRB.control = reset;
			RingBuf3_put(&AO_Control.rb, &tempRB);
			break;

		case 3:
			addToTrace("EMERGY STOP SYSTEM");
			tempRB.control = stop;
			RingBuf3_put(&AO_Control.rb, &tempRB);
			break;
		case 4://read power
			addToTrace("POWER");
			break;
		case 5:// read voltage current system
			addToTrace("system voltage current");
			break;
		case 6://read voltage current module
			addToTrace("module energy");
			break;
		case 7:	//read voltage current module
			addToTrace("Start reading EVCC Loop");
			 TimeEvent_arm(Timer_Read_EVCC_Data,1000, 1000,Task_Read_EVCC_Data);
			break;
		case 8:		//request EVSE data
			addToTrace("Stop reading EVCC Loop");
			TimeEvent_disarm(Timer_Read_EVCC_Data);
			break;
		case 9:		//request EVCC data
			addToTrace("reading EVCC Loop once");
			 TimeEvent_arm(Timer_Read_EVCC_Data,1000, 0,Task_Read_EVCC_Data);
			break;
		case 10:	//
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		default:

			break;
	}
	return 0;
}
