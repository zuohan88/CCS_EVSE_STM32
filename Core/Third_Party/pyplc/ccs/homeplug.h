/* Interface header for homeplug.c */

#ifndef HOMEPLUG_H_
#define HOMEPLUG_H_

#include "qca7000.h"
/* Global Defines */

/* Global Variables */

extern uint8_t pevMac[6];			//foe evse Mode
//extern uint8_t evseMac[6];			//evseMac for PEV mode

//extern uint8_t numberOfSoftwareVersionResponses;
//extern uint16_t checkpointNumber;

/* Global Functions */
typedef struct{
	QCA7000* QCA;

	uint8_t pevRunId[8];// = {0};//{0xDC, 0x0E, 0xA1, 0xDE, 0xAD, 0xBE, 0xEF, 0x55 }; //evse added
	//uint8_t numberOfSoftwareVersionResponses;
	uint8_t pevMac[6];  //pevMac as car mac

	uint8_t NMK_condition;
	uint8_t numberOfFoundModems;
	uint32_t randomMac;	//0
}homeplug;

void homeplug_ctor(homeplug* const me,QCA7000* QCA);

void homeplug_reset(homeplug* const me);



void homeplug_evaluateReceivedPacket(homeplug* const me);
void homeplug_runEvseSlacHandler(homeplug* const me);


#endif /* INC_APP_FUNC_H_ */
