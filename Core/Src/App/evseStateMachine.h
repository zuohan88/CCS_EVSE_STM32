/* Interface header for pevStateMachine.c */
#ifndef EVSESTATE_H
#define EVSESTATE_H

/* Global Defines */
#include "stdint.h"
#include "inttypes.h"
#include "dinEXIDatatypes.h"
#include "evsePredefine.h"

#include <AppADC.h>
#include <AppCanBus.h>
#include "ccs32_globals.h"
#include "projectExiConnector.h"

#include "Low/McuUtility.h"
#include "evsePredefine.h"

#include "TimeEvent.h"
#include "app_func.h"

#include "dinEXIDatatypes.h"

#include "tcp.h"

#define V2GTP_HEADER_SIZE 8 /* The V2GTP header has 8 bytes */

/* Global Variables */
//extern uint16_t EVSEPresentVoltage;

/* Global Functions */

/* pev state machine */



//Local private
//void Task_Contractor(void);
//void Task_emergency(void);
//void Contractor(uint8_t sig);






typedef struct {
	int16_t MaxAmp;		//by EVCC request Charge Parameter Discovery and current demand
	int16_t MaxPow; 		//by EVCC request Charge Parameter Discovery and current demand
	int16_t MaxVolt; 	//by EVCC request Charge Parameter Discovery and current demand

	//request
	int32_t TargetVolt;		//by EVCC request precharger and current demand
	int32_t TargetAmp;		//by EVCC request precharger and current demand

	//request
	int8_t  FullSOC;			//Charge Parameter Discovery
	int8_t  BulkSOC;			//Charge Parameter Discovery
	int16_t EnergyCapacity;		//Charge Parameter Discovery
	int16_t	EnergyRequest;		//Charge Parameter Discovery

	//EV status
	uint8_t RESSSOC;			//State of charge of the EV’s battery (RESS)
	uint8_t Ready;				//charger parameter, precharge, current demand, power delivery
	uint8_t ErrorCode;
	int32_t RESSConditioning;
	int32_t CabinConditioning;

	int32_t ChargingComplete;		//current demand
	int32_t BulkChargingComplete;	//current demand
	int32_t RemainingTimeToBulkSoC;	//current demand
	int32_t RemainingTimeToFullSoC; //current demand

	uint8_t ReadyToChargeState;

}evccData;

typedef struct {
	int16_t MaxPow;		//by system set by user//
	int16_t MaxVolt;	//by system
	int16_t MinVolt;	//by system
	int16_t MaxAmp;		//by system
	int16_t MinAmp;		//by system

	int16_t MaxPowX;		//by system set by user//
	int16_t MaxVolX;		//by system
	int16_t MinVoltX;		//by system
	int16_t MaxAmpX;		//by system
	int16_t MinAmpX;		//by system

	int16_t NowVolt;			//sensed voltage
	int16_t NowAmp;				//sensed voltage

	int16_t NowVoltX;		//sensed voltage
	int16_t NowAmpX;		//sensed voltage

	dinDC_EVSEStatusCodeType EVSEStatusCode;
	dinisolationLevelType IsolationStatus;

	uint8_t fully_charged;
	uint8_t	pluged;
	uint32_t total_charge_time;
	uint32_t over_time;
}evseData;

typedef struct {
	int16_t MaxPow;		//by system set by user// this need to confirm as w or Kw
	int16_t MaxVolt;	//by system
	int16_t MinVolt;	//by system
	int16_t MaxAmp;		//by system
	int16_t MinAmp;		//by system

} userData;




typedef struct evse evse;
struct evse{
	evccData EVCC;	//= {0};
	evseData EVSE; 	//= {0};
	evseData LIMIT; //	= {0};
	userData USER; //={.MaxPow = 3000, .MaxVolt =100 , .MinVolt = 50, .MaxAmp = 100, .MinAmp = 50};

	module* module;
	module* system;
	tcp* 	tcp;

	uint8_t  relay;
	uint32_t cyclesInState;
	uint8_t TimerContractor;//	=0;
	enum EVSEstate State;		//EVSE_State_SAPRq;

	//ssId sessionId;
	//uint8_t sessionIdLen;

	uint8_t evccid[LEN_OF_EVCCID];//={0};					//collect from EV car
	uint8_t evseid[LEN_OF_EVSEID];// = {0xdc,0x44,0x27,0x1f,0xd9,0x1b};			//we set the ID here static one

	//int32_t PresentVoltage =0;

	uint32_t ChargingTime;
	uint32_t StopChargingTime;

	uint32_t DeliveredPower;


	projectExiConnector* projectExiConnector;
};

/*
 * void evse_ctor(evse* const me, tcp* tcp, module* module1, module* system){
	me->USER.MaxPow = 3000;
	me->USER.MaxVolt =100;
	me->USER.MinVolt = 50;
	me->USER.MaxAmp = 100;
	me->USER.MinAmp = 50;

	me->tcp    = tcp;
	me->module = module1;
	me->system = system;

	me->relay = 0;
	me->cyclesInState = 0;
};
*/


void evseStateMachine_Task(evse* const me);

void evseStateMachine_Counter(evse* const me);

void evseStateMachine_Reset_StateTimeout(evse* const me);
void evse_enterState(evse* const me, uint16_t n);

void evseStateMachine_Init(evse* const me);
void evseStateMachine_ReInit(evse* const me);
void evseStateMachine_Stop(evse* const me);


#endif
