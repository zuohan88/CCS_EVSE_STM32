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
#ifndef ADC_FUNTION_H
#define ADC_FUNTION_H

#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>
#include "et.h" /* ET: embedded test */
#include "ring_buf3.h"

//the enum become global enum

typedef enum {
	StateA,		//12v		//No EV connected to the EVSE
	StateB,		//9V		//EV connected to the EVSE, but not ready for charging
	StateC,		//6V		//Connected and ready for charging, ventilation is not required
	StateD,		//3V		//Connected, ready for charging and ventilation is required
	StateE,		//0V		//Electrical short to earth on the controller of the EVSE, no power supply
	StateF		//-12V		//EVSE is unavailable
}State;

typedef struct {
	float V12;
	float temperture_MCP9700A;
	float temperture_MCU;


	float output_voltage;
	float output_current;
	float pt1000_1;
	float pt1000_2;
	int32_t AdcRawHVP;
	int32_t AdcRawHVN;
	int32_t AdcRawGroundPP;
	int32_t AdcRawGroundPN;
	int32_t AdcRawGroundNP;
	int32_t AdcRawGroundNN;
	float ground_test_1;
	float ground_test_2;
	float pilot_voltage;
	State CP_State;
	State Previous_CP_State;
	uint32_t rate;
	uint32_t last_tick;
}adc_measure;

typedef struct {
	uint8_t command;
	int32_t adc_data[20];
	uint32_t last_tick;

}RingBufElementADC;


typedef struct{
	adc_measure 	adc_measured;
	RingBufElementADC adc_buf1[100];			//for reciever buffer
	RingBuf3 adc_rb1;
	uint32_t adc_data[20];
}AppADC;



//void AppADC_ctor(AppADC* const me){
//};


void AppADC_Init(AppADC* const me);
void AppADC_task(AppADC* const me);
void AppADC_PilotCtrl(uint8_t on,uint32_t PWM);


#endif // RING_BUF_H
