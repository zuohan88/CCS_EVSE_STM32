/*
 * app.c
 *
 *  Created on: May 3, 2024
 *      Author: zuoha
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/



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
#include "AppInsulation.h"
#include "AppGPIO.h"
#include "qca7000.h"
#include "tcp.h"
#include "udp.h"
#include "homeplug.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

const uint8_t myMAC[6] = {0x9C,0xB6,0xD0,0xC1,0x93,0x3F};		//myMAC act  evseMAC

extern SPI_HandleTypeDef hspi1;


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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void runMyScheduler(void);
void Task_Serial(void);
void Task_CDC_Serial(void);
int Process_Normal_123(char* msg);
void QCA7000_START(void);

void Task_Ebutton(void);
void Task_Isolation_Check(void);
void Task_Check_PT1000(void);
void Task_Check_USB(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t QCA7000_IRQ_PIN = 0;
/* USER CODE END PM */



/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart1_rx;
DMA_HandleTypeDef hdma_lpuart1_tx;

extern AppCanBusTask AppCanBusTask1;
/* USER CODE BEGIN PV */

//enum system_state system_state = idle;


/* USER CODE END PV */





/* USER CODE END 0 */



void Task_LED1(void){
	LED_1_TOGGLE;
}

void Task_test(void){
//	usart_send_string_1("HELLO\r\n");
	addToTrace("testing");
}

void QCA7000_START(void){
	const char *my_json = "{\"users\":[{\"name\":\"John\", \"age\":30}, {\"name\":\"Jane\", \"age\":25}]}";
	addToTrace("I AM EVSE\r\n");
	QCA7000_startUp2(&QCA1);
	//HAL_Delay(500);
	homeplug_runEvseSlacHandler(&homeplug1);							//set nmk MODEM for once only
	//HAL_Delay(500);
	homeplug_runEvseSlacHandler(&homeplug1);
	evseStateMachine_Init(&evse1);
}



//########################################################################################################
//app init
//########################################################################################################

void app_init(){


	QCA7000_ctor(&QCA1,&hspi1);
	homeplug_ctor(&homeplug1,&QCA1);
	udp_ctor(&udp1,&QCA1);
	tcp_ctor(&tcp1,&QCA1);
	//evse_ctor(&evse1,&tcp1,&module1,&system1);



	//TestTCP();
	//RingBufElementControl tempRB[2] ={0};

	static uint8_t Timer_LED = 0;
	static uint8_t Timer_test = 0;
	//  while(host_com_port_open == 0){};
	//  LED_1_TOGGLE;
	  HAL_Delay(1000);

	  AppADC_Init(&AppADC1);
	  AppCanBusTask_init(&AppCanBusTask1);
	  usart_init_1();
	  CDC_init_0();		//dual port system
	  AppControl_Init();
	  if(TimeEvent_new_Timer(&Timer_LED)){
		  addToTrace("[Timer] failed Initializing timer for Timer_LED");
	  }
	  if(TimeEvent_new_Timer(&Timer_test)){
	  		  addToTrace("[Timer] failed Initializing timer for Task_test");
	  }
	  TimeEvent_arm(Timer_LED,1000, 1000,Task_LED1);
	  //TimeEvent_arm(Timer_test,1000, 1000,Task_test);

	  addToTrace("[app] ccs32EVSE started. Version 2024-11-25 ===");
	  while(1){
		  app_run();
	  }

}




void app_run(){

	TimeEvent_tick();			//timer task

	AppSerial_SerialTask();
	AppSerial_CheckUSB();			//if USB not connected, we need to stop system
	AppControl_Task();				// control task
	AppADC_task(&AppADC1);					//adc task
	AppCanBusTask_receivedTask(&AppCanBusTask1);	//check if anything received
	AppCanBusTask_sendingTask(&AppCanBusTask1);	//check if anyting to send
	AppInsulation_Task();		//check if anysignal insulation to run

	Task_Ebutton();				//e button task
	Task_Check_PT1000();		//if gun over heat

	runMyScheduler();					//network and EVSE task

	AppInsulation_test();
}

/* The tasks */

//###################################################################################################
//Main task


//###################################################################################################
void runMyScheduler(void) {
	uint32_t TickNow = HAL_GetTick();
	static uint32_t lastTime30ms;
	if ((TickNow - lastTime30ms)>6000) {			//every 1	task got something here only
	  lastTime30ms = TickNow;
	  addToTrace("[app] running");
	}
	if(QCA7000_INT_READ){
		QCA7000_IRQ_PIN = 1;
	}
	if(QCA7000_IRQ_PIN == 1){
		QCA7000_IRQ_PIN = 0;
		if(QCA7000_thread(&QCA1)){
			QCA7000_checkForReceivedData(&QCA1);  		//read spi
			if(QCA1.ETH.etherType == ETHTYPE_HOMEPLUG){
				addToTrace( "[QCA]HomePlug message.");
				homeplug_evaluateReceivedPacket(&homeplug1);	//homeplug packet			//do slac packet
			}else if(QCA1.ETH.etherType == ETHTYPE_IPv6){
				addToTrace( "[QCA]IPv6 message.");
				ipv6_evaluateReceivedPacket(&udp1);			//ipv6 check data udp or tcpv6.c
			}else{
				McuXFormat_xsprintf(strTmp, "[QCA]Other message.Type=0x%X",QCA1.ETH.etherType); //0x%04X
				addToTrace(strTmp);
			}
			evseStateMachine_Task(&evse1);				//do evse task
		}
	}
	evseStateMachine_Counter(&evse1);					//check state and reset if too long
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == QCA_INT_Pin) {
	  QCA7000_IRQ_PIN = 1;
  }
}





void Task_Check_PT1000(void){
	if(adc_measured.pt1000_1 == -1.0){
	//	addToTrace("Pt1000_1 got issue");
	}
	if(adc_measured.pt1000_2 == -1.0){
	//	addToTrace("Pt1000_1 got issue");
	}
	if(adc_measured.pt1000_1 > 89.0){
	//	addToTrace("Pt1000_1 overheat");
	}
	if(adc_measured.pt1000_2 > 89.0){
	//	addToTrace("Pt1000_1 overheat");
	}
	//EVSE.EVSEStatusCode =dinDC_EVSEStatusCodeType_EVSE_Malfunction;
}


//***************************************************************************************







