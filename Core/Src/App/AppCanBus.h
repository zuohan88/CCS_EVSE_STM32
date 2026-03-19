//============================================================================
// Lock-Free Ring Buffer (LFRB) for embedded systems
// GitHub: https://github.com/QuantumLeaps/lock-free-ring-buffer
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
//
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//============================================================================
#ifndef APPCANBUS_H
#define APPCANBUS_H

#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>
#include "et.h" /* ET: embedded test */

#include "ring_buf3.h"


#include "main.h"




typedef union{
	struct{
		// will usually occupy 2 bytes:

		uint8_t module_load_limit	: 1;
		uint8_t ID_reused			: 1;
		uint8_t module_imblance		: 1;
		uint8_t phase_missing		: 1;
		uint8_t phase_imbalance 	: 1;
		uint8_t undervoltage  	    : 1;
		uint8_t overvoltage  		: 1;
		uint8_t pfc 				: 1; // 1st 3 bits (in 1st byte) are b1


	};
	uint8_t flags;
}condition0;

typedef union{
	struct{
		// will usually occupy 2 bytes:
		uint8_t DC_offline			: 1;
		uint8_t faulty				: 1;
		uint8_t protection_warning	: 1;
		uint8_t fan_error			: 1;
		uint8_t over_heat		 	: 1;
		uint8_t output_overvoltage  : 1;
		uint8_t walk_in		  		: 1;
		uint8_t network_error 		: 1;



	};
	uint8_t flags;
}condition1;

typedef union{
	struct{
		uint8_t output_short			: 1;
		uint8_t 						: 1;
		uint8_t internal_network_error	: 1;
		uint8_t input_error		 		: 1;
		uint8_t sleep			    	: 1;
		uint8_t output_error			: 1;
		uint8_t 						: 2;
	};
	uint8_t flags;
}condition2;

typedef struct {
	 uint32_t VomaxHi,VominHi,IomaxHi,PrateHi;			//i need divide by 10, P need to time 1k
	 uint32_t VoExHi, VoExLo, IoExHi, IoExLo;
	 uint32_t Vab10,Vbc10,Vca10;										//need divide by 10
	 float voltage,current,power,energy;
	 uint32_t milivolt32,miliamp32,power32,energy32;
	 uint8_t energy_reset,energy32_reset;
	 uint32_t set_milivolt,set_miliamp;
	 uint32_t setting_milivolt,setting_miliamp;
	 uint8_t number;
	 int8_t temperture;
	 condition2 condition2;
	 condition1 condition1;
	 condition0 condition0;
	 uint8_t serial_number;
	 uint8_t available;
	 uint32_t last_tick;
	 uint32_t last_tick_vc;
	 uint32_t last_tick_vc32;
	 uint32_t last_tick_3ph;
	 uint32_t last_tick_condition;
	 uint32_t last_tick_VA;
	 uint32_t last_tick_MaxOutput;
}module;


typedef struct {
	 float voltage,current,power,energy;
	 uint32_t voltage32,current32,power32,energy32;
	 uint8_t numberOfModule;
	 int8_t temperture;
	 uint32_t last_tick;
}sys;

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint32_t time;

}RingBufElementCANBUS;


#define CAN_MAX_SIZE	50

typedef struct{
	RingBufElementCANBUS can_buf_rx[CAN_MAX_SIZE];			//for RX buffer
	RingBuf3 rb_rx;
	RingBufElementCANBUS can_buf_tx[CAN_MAX_SIZE];			//for TX going buffer
	RingBuf3 rb_tx;

	RingBufElementCANBUS can_buf_tx_hw[CAN_MAX_SIZE];			//for TX going buffer for hardware canbus
	RingBuf3 rb_tx_hw;


	FDCAN_HandleTypeDef* hfdcan;
	FDCAN_TxHeaderTypeDef* TxHeader;

	uint8_t  TxData[8],RxData[8];



}AppCanBus;





typedef struct {

	uint8_t timer1;
	uint8_t timer2;
	module module, PEV_module, system;
	AppCanBus* AppCanBus;
}AppCanBusTask;










void AppCanBus_ctor(AppCanBus* const me, FDCAN_HandleTypeDef* hfdcan);


void AppCanBus_task_ctor(AppCanBus* const me);

void AppCanBusTask_stopTask(AppCanBusTask* const task);
void AppCanBusTask_setVoltCurr(AppCanBusTask* const task, uint32_t volt,uint32_t curr);
void AppCanBusTask_sendingTask(AppCanBusTask* const task);
void AppCanBusTask_receivedTask(AppCanBusTask* const task);



#endif // RING_BUF_H
