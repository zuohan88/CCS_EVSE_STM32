/*
 * AppControl.h
 *
 *  Created on: Feb 8, 2025
 *      Author: zuoha
 */

#ifndef SRC_APP_APPCONTROL_H_
#define SRC_APP_APPCONTROL_H_

typedef enum {
		idle,
		start,
		boot,
		boot2,
		boot3,
		check_NMK,
		check_CP,
		run,
		reset,
		stop,
		estop
	}enum_state;

typedef struct{
	enum_state control;
	uint32_t time;
}RingBufElementControl;

typedef struct{
	RingBufElementControl rb_buf[10];
	RingBuf3 rb;
	uint8_t Timer;
	uint8_t errorStartUp;
	uint8_t errorNMK;
	uint8_t countNMK;
}AOControl;

void AppControl_Init(void);
void AppControl_Task(void);

#endif /* SRC_APP_APPCONTROL_H_ */
