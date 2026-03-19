/*
 * timer_arm.h
 *
 *  Created on: Nov 2, 2024
 *      Author: zuoha
 */

#ifndef CANBUS_RING_TIMER_ARM_H_
#define CANBUS_RING_TIMER_ARM_H_

typedef   void (*Operation)(void);

typedef struct {

	Operation  opt;       /* the AO that requested this TimeEvent */
    uint32_t timeout;  /* timeout counter; 0 means not armed */
    uint32_t interval; /* interval for periodic TimeEvent, 0 means one-shot */
    uint32_t lasttick;
} TimeEvent;





#define l_tevtNum 10
TimeEvent timer_arm[l_tevtNum]; /* all TimeEvents in the application */
//static uint_fast8_t l_tevtNum; /* current number of TimeEvents */

//#define Timer_LED timer_arm[0]
//#define Timer_Contractor timer_arm[1]
//#define Timer_canbus1 timer_arm[2]
//#define Timer_canbus2 timer_arm[3]

void TimeEvent_tick(void);
uint8_t TimeEvent_new_Timer(uint8_t* timer);
void TimeEvent_arm(uint8_t timer_number,uint32_t timeout, uint32_t interval,void (*task)());
void TimeEvent_disarm(uint8_t timer_number);




#endif /* CANBUS_RING_TIMER_ARM_H_ */
