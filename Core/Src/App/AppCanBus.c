#include <AppCanBus.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#include "TimeEvent.h"
#include "ccs32_globals.h"

FDCAN_HandleTypeDef hfdcan2;
FDCAN_HandleTypeDef hfdcan3;







AppCanBus AppCanBus1;
AppCanBus AppCanBus2;





AppCanBusTask AppCanBusTask1;

FDCAN_TxHeaderTypeDef TxHeader = {
			.Identifier = 0x029B3FF0,
			.IdType = FDCAN_EXTENDED_ID,
			.TxFrameType = FDCAN_DATA_FRAME,
			.DataLength = FDCAN_DLC_BYTES_8,
			.ErrorStateIndicator = FDCAN_ESI_ACTIVE,
			.BitRateSwitch = FDCAN_BRS_OFF,
			.FDFormat = FDCAN_CLASSIC_CAN,
			.TxEventFifoControl = FDCAN_NO_TX_EVENTS,
			.MessageMarker = 0
};


void AppCanBus_ctor(AppCanBus* const me, FDCAN_HandleTypeDef* hfdcan){

	me->TxHeader = &TxHeader;
	me->hfdcan = hfdcan;

};


void AppCanBus_task_ctor(AppCanBus* const me){

};


void AppCanBusTask_ctor(AppCanBusTask* const me){
	me->AppCanBus = &AppCanBus1;
}


void AppCanBusTask_timerTask();
void AppCanBusTask_timerTaskReadSystem();
void AppCanBusTask_startTask(AppCanBusTask* const task);

void AppCanBusTask_initTask(AppCanBusTask* const task);
void AppCanBusTask_init(AppCanBusTask* const task);
void AppCanBusTask_sendingTask(AppCanBusTask* const task);
void AppCanBusTask_offAllModule(AppCanBusTask* const task);
void AppCanBusTask_onAllModule(AppCanBusTask* const task);
void AppCanBusTask_setVoltCurr(AppCanBusTask* const task, uint32_t volt,uint32_t curr);
void AppCanBusTask_power(AppCanBusTask* const task, uint32_t voltage,uint32_t current);

void AppCanBus_send(AppCanBus* const me, uint32_t ID, uint8_t TxData[8]);
void AppCanBus_configPort(AppCanBus* const me);


//########################################################################################################
//CAN BUs task
//########################################################################################################
void AppCanBusTask_timerTask(){		//request module 1
	RingBufElementCANBUS rb_temp = {0};
	rb_temp.id = 1;
	RingBuf3_put(&AppCanBus1.rb_tx, &rb_temp);
}

void AppCanBusTask_timerTaskReadSystem(){		//request system
	uint8_t TxData[8] = {0,0,0,0,0,0,0,0}; // = {0,1,2,3,4,5,6,7};
	AppCanBus_send(&AppCanBus1, 0x02813FF0, TxData);	//read system volt and current in floating
	AppCanBus_send(&AppCanBus1, 0x02823FF0, TxData);	//read number of module
	AppCanBus_send(&AppCanBus1, 0x02883FF0, TxData);	//read system volt and current in int32
}

void AppCanBusTask_energyCounterReset(AppCanBusTask* const task){
	task->system.energy_reset = 1;
	task->system.energy32_reset = 1;
	task->module.energy_reset = 1;
	task->module.energy32_reset = 1;


}

void AppCanBusTask_startTask(AppCanBusTask* const task){
	TimeEvent_arm(task->timer1,1000, 1000,AppCanBusTask_timerTask);
	TimeEvent_arm(task->timer2,1000, 1000,AppCanBusTask_timerTaskReadSystem);		// 200
}

void AppCanBusTask_stopTask(AppCanBusTask* const task){
	TimeEvent_disarm(task->timer1);
	TimeEvent_disarm(task->timer2);		// read
}






//########################################################################################################
//CAN BUs main task
//########################################################################################################
void AppCanBusTask_init(AppCanBusTask* const task){
	AppCanBus* const me = task->AppCanBus;
	RingBufElementCANBUS rb_temp = {0};

	AppCanBus_configPort(task->AppCanBus);
	//AppCanBus_configPort(&AppCanBus2);

	rb_temp.id = 1;
	RingBuf3_put(&me->rb_tx, &rb_temp);

	task->module.last_tick = 0;
	if(TimeEvent_new_Timer(&task->timer1)){
	addToTrace("[Timer] Failed Initializing timer for Timer_canbus1");
	}
	if(TimeEvent_new_Timer(&task->timer2)){
	addToTrace("[Timer] Failed Initializing timer for Timer_canbus2");
	}

	//AppCanBusTask_energyCounterReset(task);
	AppCanBusTask_energyCounterReset(task);





}


#define canbus_state_read_vc 				1
#define canbus_state_read_condition 		2
#define canbus_state_read_max				3
#define canbus_state_read_phase				4
#define canbus_state_set_vc					5
#define mV	1000
#define mA	1000



void AppCanBusTask_sendingTask(AppCanBusTask* const task){
	AppCanBus* const me = task->AppCanBus;
	uint8_t TxData[8] = {0};
	RingBufElementCANBUS temp;

	while(true == RingBuf3_get(&me->rb_tx, &temp)){
		switch (temp.id) {
			case 1:		//read module info


				AppCanBus_send(me, 0x02C201F0, TxData);	//read number of module in group 1
				AppCanBus_send(me, 0x028C00F0, TxData);	//output voltage and current limit
				AppCanBus_send(me, 0x028300F0, TxData);	//read curent voltage current module in floating
				AppCanBus_send(me, 0x028900F0, TxData);	//read curent voltage current module in int32
				AppCanBus_send(me, 0x028400F0, TxData);	//read condition of module done
				AppCanBus_send(me, 0x028600F0, TxData);//read 3 phase voltage of module done
				AppCanBus_send(me, 0x028A00F0, TxData);//read module maximum output voltage and current
				AppCanBus_send(me, 0x028C00F0, TxData);	//output voltage and current limit of module with current condition done
				AppCanBus_send(me, 0x028B00F0, TxData);	// read serial number of module
				break;
			case 2:		//wait
				AppCanBus_send(me, 0x02813FF0, TxData);	//read system volt and current in floating
				AppCanBus_send(me, 0x02823FF0, TxData);	//read number of module
				AppCanBus_send(me, 0x02883FF0, TxData);	//read system volt and current in int32
				break;


			case 5:		//off all module
				TxData[0]=1;
				AppCanBus_send(me, 0x029A3FF0, TxData);
				break;
			case 51:	//on all module
				TxData[0]=0;
				AppCanBus_send(me, 0x029A3FF0, TxData);
				break;
			default:
				break;

		}
	}
}

//########################################################################################################
//CAN BUs sending function
//########################################################################################################
void AppCanBusTask_offAllModule(AppCanBusTask* const task){
	uint8_t TxData[8] = {0};
	TxData[0]=1;
	AppCanBus_send(task->AppCanBus, 0x029A3FF0, TxData);

}

void AppCanBusTask_onAllModule(AppCanBusTask* const task){
	uint8_t TxData[8] = {0};
	AppCanBus_send(task->AppCanBus, 0x029A3FF0, TxData);
}

void AppCanBusTask_setVoltCurr(AppCanBusTask* const task, uint32_t volt,uint32_t curr){			//using for EVSE
	AppCanBus* const me = task->AppCanBus;
	uint8_t *voltage_int;
	uint8_t *current_int;
	uint8_t TxData[8] ={0};
	if(task->module.condition1.DC_offline==1 && task->module.set_milivolt!=0){
		TxData[0] = 0;
		AppCanBus_send(me, 0x029A00F0, TxData);
	}
	//if()
	task->module.set_milivolt = volt*mV;	//mV		//max test 950
	task->module.set_miliamp = curr*mA; //mA
	voltage_int = (uint8_t*)&task->module.set_milivolt;
	TxData[0] = voltage_int[3];
	TxData[1] = voltage_int[2];
	TxData[2] = voltage_int[1];
	TxData[3] = voltage_int[0];
	current_int = (uint8_t*)&task->module.set_miliamp;
	TxData[4] = current_int[3];
	TxData[5] = current_int[2];
	TxData[6] = current_int[1];
	TxData[7] = current_int[0];
	AppCanBus_send(me, 0x029B3FF0, TxData);
}

void AppCanBusTask_power(AppCanBusTask* const task, uint32_t voltage,uint32_t current){			//mot using
	AppCanBus* const me = task->AppCanBus;
	uint8_t TxData[8]= {0};
	/* Prepare Tx Header */							  //0x1FFFFFFF
	TxHeader.Identifier = 0x029B3FF0;		//with return data

	//uint32_t voltage = 300*1000;		//mV
	uint8_t *voltage_int = (uint8_t*)&voltage;
    TxData[0] = voltage_int[3];
    TxData[1] = voltage_int[2];
    TxData[2] = voltage_int[1];
    TxData[3] = voltage_int[0];
    //uint32_t current = 10*1000;
    uint8_t *current_int = (uint8_t*)&current;
    TxData[4] = current_int[3];
    TxData[5] = current_int[2];
    TxData[6] = current_int[1];
    TxData[7] = current_int[0];

    /* Start the Transmission process */
    if (HAL_FDCAN_AddMessageToTxFifoQ(me->hfdcan, me->TxHeader, TxData) != HAL_OK){
      Error_Handler();
    }
}
//*************************************************************************************
// canbus slave (
//*************************************************************************************
void AppCanBusTask_receivedTask(AppCanBusTask* const task){
	AppCanBus* const me = task->AppCanBus;
	RingBufElementCANBUS RxBuffer;
	uint8_t temp[8]={0};

	while(true == RingBuf3_get(&me->rb_rx, &RxBuffer)){
		//module1.available =1;
		switch (RxBuffer.id) {
//***********************************************************************************
//I am PEV process
//***********************************************************************************
		  case 0x028300F0:			//from evse to module //check voltage and current		//floating//0x03
			task->PEV_module.voltage = 300.0; //mV
			uint8_t *voltage_int = (uint8_t*)&task->PEV_module.voltage;
			temp[0] = voltage_int[3];
			temp[1] = voltage_int[2];
			temp[2] = voltage_int[1];
			temp[3] = voltage_int[0];
			task->PEV_module.current = 3.5; //mV
			uint8_t *current_int = (uint8_t*)&task->PEV_module.current;
			temp[4] = current_int[3];
			temp[5] = current_int[2];
			temp[6] = current_int[1];
			temp[7] = current_int[0];
			AppCanBus_send(me, 0x0283F000,temp);
			break;
		  case 0x029B3FF0:			//from evse to module //set voltage and current 0x03
			temp[0] = RxBuffer.data[3];
			temp[1] = RxBuffer.data[2];
			temp[2] = RxBuffer.data[1];
			temp[3] = RxBuffer.data[0];
			task->PEV_module.set_milivolt = *((uint32_t*)&temp);
			temp[4] = RxBuffer.data[7];
			temp[5] = RxBuffer.data[6];
			temp[6] = RxBuffer.data[5];
			temp[7] = RxBuffer.data[4];
			task->PEV_module.set_miliamp = *((uint32_t*)&temp);
			break;
//***********************************************************************************
//I am EVSE
//***********************************************************************************
		  //system response
		  case 0x0281F03F:		//system voltage and current	in floating
			  temp[0] = RxBuffer.data[3];
			  temp[1] =	RxBuffer.data[2];
			  temp[2] =	RxBuffer.data[1];
			  temp[3] = RxBuffer.data[0];
			  task->system.voltage = *((float*)&temp);
			  temp[0] = RxBuffer.data[7];
			  temp[1] =	RxBuffer.data[6];
			  temp[2] =	RxBuffer.data[5];
			  temp[3] = RxBuffer.data[4];
			  task->system.current = *((float*)&temp);
			  task->system.power = task->system.voltage * task->system.current;

			  if(task->system.energy_reset){
				  task->system.energy = task->system.power;
				  task->system.energy_reset = 0;
			  }else{
				  task->system.energy += task->system.power*(((float)RxBuffer.time - (float)task->system.last_tick)/1000.0f);
			  }
			  task->system.last_tick_vc = RxBuffer.time;
			  break;
		  case 0x0288F03F:		//system voltage and current	in int32 in miliV
			  temp[0] = RxBuffer.data[3];
			  temp[1] =	RxBuffer.data[2];
			  temp[2] =	RxBuffer.data[1];
			  temp[3] = RxBuffer.data[0];
			  task->system.milivolt32 = *((uint32_t*)&temp);
			  temp[0] = RxBuffer.data[7];
			  temp[1] =	RxBuffer.data[6];
			  temp[2] =	RxBuffer.data[5];
			  temp[3] = RxBuffer.data[4];
			  task->system.miliamp32 = *((uint32_t*)&temp);
			  task->system.power32 = (uint32_t)(((uint64_t)task->system.milivolt32 * (uint64_t)task->system.miliamp32)/1000000);	//overflow issue
			  if(task->system.energy32_reset){
				  task->system.energy32 = task->system.power32;
				  task->system.energy32_reset = 0;
			  }else{
				  task->system.energy32 += task->system.power32*((RxBuffer.time - task->system.last_tick_vc32 ))/1000;
			  }
			  task->system.last_tick_vc32 = RxBuffer.time;
			  break;
		  //module response
		  case 0x0289F000:		//module voltage and amp now	in int32
			  temp[0] = RxBuffer.data[3];
			  temp[1] =	RxBuffer.data[2];
			  temp[2] =	RxBuffer.data[1];
			  temp[3] = RxBuffer.data[0];
			  task->module.milivolt32 = *((uint32_t*)&temp);
			  temp[0] = RxBuffer.data[7];
			  temp[1] =	RxBuffer.data[6];
			  temp[2] =	RxBuffer.data[5];
			  temp[3] = RxBuffer.data[4];
			  task->module.miliamp32 = *((uint32_t*)&temp);
			  task->module.power32 = (uint32_t)(((uint64_t)task->module.milivolt32 * (uint64_t)task->module.miliamp32)/1000000);		//overflow	issue
			  if(task->module.energy32_reset){
				  task->module.energy32 = task->system.power32;
				  task->module.energy32_reset = 0;
			  }else{
				  task->module.energy32 += task->module.power32*((RxBuffer.time - task->module.last_tick_vc32 ))/1000;
			  }
			  task->module.last_tick_vc32 = RxBuffer.time;
			  break;
		  case 0x0283F000:		//module voltage and current in floating point
			  temp[0] = RxBuffer.data[3];
			  temp[1] =	RxBuffer.data[2];
			  temp[2] =	RxBuffer.data[1];
			  temp[3] = RxBuffer.data[0];
			  task->module.voltage = *((float*)&temp);
			  temp[0] = RxBuffer.data[7];
			  temp[1] =	RxBuffer.data[6];
			  temp[2] =	RxBuffer.data[5];
			  temp[3] = RxBuffer.data[4];
			  task->module.current = *((float*)&temp);
			  task->module.power = task->module.voltage * task->module.current;
			  task->module.last_tick_vc = RxBuffer.time;
			  break;

		  case 0x029BF03F:			//V,A setting response
			  temp[0] = RxBuffer.data[3];
			  temp[1] =	RxBuffer.data[2];
			  temp[2] =	RxBuffer.data[1];
			  temp[3] = RxBuffer.data[0];
			  task->module.setting_milivolt = *((uint32_t*)&temp);
			  temp[0] = RxBuffer.data[7];
			  temp[1] =	RxBuffer.data[6];
			  temp[2] =	RxBuffer.data[5];
			  temp[3] = RxBuffer.data[4];
			  task->module.setting_miliamp = *((uint32_t*)&temp);
			  task->module.last_tick_VA = RxBuffer.time;
			  break;
		  case 0x0284F000:	//condition response 00 00 02 00 1B 00 40 00
			  task->module.number=RxBuffer.data[2];
			  task->module.temperture=RxBuffer.data[4];
			  task->module.condition2.flags =RxBuffer.data[5];
			  task->module.condition1.flags =RxBuffer.data[6];
			  task->module.condition0.flags =RxBuffer.data[7];
			  if(task->module.condition2.flags){
			  }
			  if(task->module.condition1.flags){
			  }
			  if(task->module.condition0.flags){
			  }
			  task->module.last_tick_condition = RxBuffer.time;
			  break;
			  // 00 04 93 C9 00 00 27 10:
		  case 0x02C4F000:// //condition resposnce 2
			  ///float module_voltage = data1;
			 // float module_current = data2;
			  break;
		  case 0x0286F000:		//read 3 phase voltage response
			 //module1.Vab10 = *((uint16_t*)&RxBuffer.data[0])/10;	// need devide by 10
			  task->module.Vab10 = (((uint32_t)RxBuffer.data[0]<<8) | RxBuffer.data[1])/10;	// need devide by 10
			  task->module.Vbc10 = (((uint32_t)RxBuffer.data[2]<<8) | RxBuffer.data[3])/10;	// need devide by 10
			  task->module.Vca10 = (((uint32_t)RxBuffer.data[4]<<8) | RxBuffer.data[5])/10;	// need devide by 10
			  task->module.last_tick_3ph = RxBuffer.time;
			  break;
		  case 0x028AF000:				//read module maximum output
			  task->module.VomaxHi = ((uint32_t)RxBuffer.data[0]<<8) | RxBuffer.data[1];
			  task->module.VominHi = ((uint32_t)RxBuffer.data[2]<<8) | RxBuffer.data[3];
			  task->module.IomaxHi = (((uint32_t)RxBuffer.data[4]<<8) | RxBuffer.data[5])/10;
			  task->module.PrateHi = (((uint32_t)RxBuffer.data[6]<<8) | RxBuffer.data[7])*1000;
			  task->module.last_tick_MaxOutput = RxBuffer.time;
			  break;
		  case 0x28CF000:
			  task->module.VoExHi = (uint32_t)RxBuffer.data[0];
			  task->module.VoExLo = (uint32_t)RxBuffer.data[1];
			  task->module.IoExHi = (uint32_t)RxBuffer.data[2];
			  task->module.IoExLo = (uint32_t)RxBuffer.data[3];
			  break;

		  default:
			  break;
		}
	}
}
//########################################################################################################
//CAN BUs funciton
//########################################################################################################




void AppCanBus_configPort(AppCanBus* const me){
  FDCAN_FilterTypeDef sFilterConfig;

  /* Configure Rx filter */
  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x0;			//0x321
  sFilterConfig.FilterID2 = 0x1FFFFFFF;			//0x7FF
  if (HAL_FDCAN_ConfigFilter(me->hfdcan, &sFilterConfig) != HAL_OK){
    Error_Handler();
  }
  /* Configure global filter:
     Filter all remote frames with STD and EXT ID
     Reject non matching frames with STD ID and EXT ID */
  if (HAL_FDCAN_ConfigGlobalFilter(me->hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK){
    Error_Handler();
  }
  /* Start the FDCAN module */
  if (HAL_FDCAN_Start(me->hfdcan) != HAL_OK){
    Error_Handler();
  }
  if (HAL_FDCAN_ActivateNotification(me->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK){
    Error_Handler();
  }
  if (HAL_FDCAN_ActivateNotification(me->hfdcan, FDCAN_IT_TX_COMPLETE, FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2) != HAL_OK){
	Error_Handler();
  }

  RingBuf3_ctor(&me->rb_tx, me->can_buf_tx,sizeof(RingBufElementCANBUS), ARRAY_NELEM(me->can_buf_tx));
  RingBuf3_ctor(&me->rb_rx, me->can_buf_rx,sizeof(RingBufElementCANBUS), ARRAY_NELEM(me->can_buf_rx));
  RingBuf3_ctor(&me->rb_tx_hw, me->can_buf_tx_hw,sizeof(RingBufElementCANBUS), ARRAY_NELEM(me->can_buf_tx_hw));
}


void AppCanBus_send(AppCanBus* const me, uint32_t ID, uint8_t TxData[8]){
	RingBufElementCANBUS rb_temp= {0};
	uint8_t buffer_pending0 = HAL_FDCAN_IsTxBufferMessagePending(me->hfdcan, FDCAN_TX_BUFFER0);
	uint8_t buffer_pending1 = HAL_FDCAN_IsTxBufferMessagePending(me->hfdcan, FDCAN_TX_BUFFER1);
	uint8_t buffer_pending2 = HAL_FDCAN_IsTxBufferMessagePending(me->hfdcan, FDCAN_TX_BUFFER2);
	if(!(buffer_pending0 && buffer_pending1 && buffer_pending2)){
		me->TxHeader->Identifier = ID;
		if (HAL_FDCAN_AddMessageToTxFifoQ(me->hfdcan, me->TxHeader, TxData) != HAL_OK){
			Error_Handler();
		}
	}else{
		rb_temp.id = ID;
		my_memcpy(rb_temp.data,TxData,8);
		RingBuf3_put(&me->rb_tx, &rb_temp);
	}
}



void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef RxHeader;
	RingBufElementCANBUS temp = {0};
	if(FDCAN3 == hfdcan->Instance){
	  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET){
		/* Retrieve Rx messages from RX FIFO0 */
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, temp.data) != HAL_OK){
		Error_Handler();
		}
		if ((RxHeader.IdType == FDCAN_EXTENDED_ID) && (RxHeader.DataLength == FDCAN_DLC_BYTES_8)){
			temp.id = RxHeader.Identifier;
			temp.time = HAL_GetTick();		//record time when received
			RingBuf3_put(&AppCanBus1.rb_rx, &temp);
		}
		//if ((RxHeader.Identifier == 0x321) && (RxHeader.IdType == FDCAN_EXTENDED_ID) && (RxHeader.DataLength == FDCAN_DLC_BYTES_2))
		//{
		  //LED_Display(RxData[0]);
		 // ubKeyNumber = RxData[0];
		//}
	  }
	}
	if(FDCAN2 == hfdcan->Instance){
	  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET){
		/* Retrieve Rx messages from RX FIFO0 */
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, temp.data) != HAL_OK){
			Error_Handler();
		}
		if ((RxHeader.IdType == FDCAN_EXTENDED_ID) && (RxHeader.DataLength == FDCAN_DLC_BYTES_8)){
			temp.id = RxHeader.Identifier;
			temp.time = HAL_GetTick();
			RingBuf3_put(&AppCanBus2.rb_rx, &temp);
		}
	  }
	}
}


void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
	//AppCanBus* const me = &AppCanBus1;
	RingBufElementCANBUS temp;
    if (hfdcan == &hfdcan3)
    {
    	if(true == RingBuf3_get(&AppCanBus1.rb_tx, &temp)){
    		TxHeader.Identifier = temp.id;
    	    /* Start the Transmission process */
    	    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, temp.data) != HAL_OK){
    	      /* Transmission request Error */
    	    	Error_Handler();
    	    }
    	}
    //    HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_TX_COMPLETE, BufferIndexes);
    }
    if (hfdcan == &hfdcan2){
    	if(true == RingBuf3_get(&AppCanBus2.rb_tx, &temp)){
    		TxHeader.Identifier = temp.id;
    	    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, temp.data) != HAL_OK){
    	      Error_Handler();
    	    }
    	}
  //      HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_TX_COMPLETE, BufferIndexes);
    }
}

void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
	uint32_t tmp_u32Errcount;
	uint32_t tmp_u32status;
	uint32_t ret = HAL_FDCAN_GetError(hfdcan);
	if(hfdcan->Instance == FDCAN3)
	{
	#ifdef BSP_USING_FDCAN1
	//can1
	if( (ret & FDCAN_IT_ARB_PROTOCOL_ERROR) &&
	(hfdcan->Instance->CCCR & FDCAN_CCCR_INIT_Msk))
	{
		//hfdcan->Instance->CCCR |= FDCAN_CCCR_CCE_Msk;
		hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT_Msk;
		st_DrvCan1.device.status.errcode = 0xff;
	}
	else
	{
		tmp_u32Errcount = st_DrvCan1.fdcanHandle.Instance->ECR;
		tmp_u32status = st_DrvCan1.fdcanHandle.Instance->PSR;
		st_DrvCan1.device.status.rcverrcnt = (tmp_u32Errcount>>8)&0x000000ff;
		st_DrvCan1.device.status.snderrcnt = (tmp_u32Errcount)&0x000000ff;
		st_DrvCan1.device.status.lasterrtype = tmp_u32status&0x000000007;
		rt_hw_can_isr(&st_DrvCan1.device, RT_CAN_EVENT_TX_FAIL);
	}
	#endif /BSP_USING_FDCAN1/
	}
	else
	{
	#ifdef BSP_USING_FDCAN2
	if( (ret & FDCAN_IT_ARB_PROTOCOL_ERROR) &&
	(hfdcan->Instance->CCCR & FDCAN_CCCR_INIT_Msk))
	{
	//hfdcan->Instance->CCCR |= FDCAN_CCCR_CCE_Msk;
		hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT_Msk;
		st_DrvCan2.device.status.errcode = 0xff;
	}
	else
	{
		//can2
		tmp_u32Errcount = st_DrvCan2.fdcanHandle.Instance->ECR;
		tmp_u32status = st_DrvCan2.fdcanHandle.Instance->PSR;
		st_DrvCan2.device.status.rcverrcnt = (tmp_u32Errcount>>8)&0x000000ff;
		st_DrvCan2.device.status.snderrcnt = (tmp_u32Errcount)&0x000000ff;
		st_DrvCan2.device.status.lasterrtype = tmp_u32status&0x000000007;
		rt_hw_can_isr(&st_DrvCan2.device, RT_CAN_EVENT_TX_FAIL);
	}
	#endif /BSP_USING_FDCAN2/
	}
	if(hfdcan->Instance == FDCAN2){

	}

}


void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs){

	if(hfdcan->Instance == FDCAN2){

	}
	if(hfdcan->Instance == FDCAN3){

	}


}






