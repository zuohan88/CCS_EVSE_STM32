/*
 * timer_arm.c
 *
 *  Created on: Nov 2, 2024
 *      Author: zuoha
 */

#include <stdint.h>
#include <stdbool.h>
#include <TimeEvent.h>
#include "main.h"


static uint8_t Timer_Number = 1;		//start with 1,  0 as unkwon timer

void TimeEvent_tick(void){
	uint32_t ticknow = HAL_GetTick();
    uint_fast8_t i;
    for (i = 1U; i < Timer_Number; ++i) {
        TimeEvent *t = &timer_arm[i];
        if(t->timeout > 0U){
			if((ticknow - t->lasttick) > t->timeout){
				t->timeout = t->interval;	/* rearm or disarm (one-shot) */
				t->lasttick = ticknow;
				if(t->opt != 0){
					(t->opt)();
				}
			}
        }
    }
}

uint8_t TimeEvent_new_Timer(uint8_t* timer){
	if(*timer == 0){			//only 0 is can be given new value
		*timer = Timer_Number;
		if(Timer_Number < (l_tevtNum-1)){
			Timer_Number++;
			return 0;	//yes
		}
		return 1;		//no
	}
	return 1;		//no
}

void TimeEvent_arm(uint8_t timer_number,uint32_t timeout, uint32_t interval,void (*task)()) {
	timer_arm[timer_number].lasttick = HAL_GetTick();
	timer_arm[timer_number].timeout = timeout;
	timer_arm[timer_number].interval = interval;
	timer_arm[timer_number].opt = task;		//pass in the task
}

void TimeEvent_disarm(uint8_t timer_number) {
	timer_arm[timer_number].timeout = 0U;
	timer_arm[timer_number].interval = 0U;	//stop all
}
