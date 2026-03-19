/* Interface header for ipv6.c */
#ifndef UDP_H_
#define UDP_H_


#include <string.h> /* memcpy */
#include <stdio.h>
#include <stdlib.h> /* abs */
#include <stdint.h>
/* Global Defines */
#include "qca7000.h"
/* Global Variables */

//extern uint16_t evccPort;
//extern uint16_t seccPort; /* the port number of the charger */
//extern uint8_t EvccIp[16];		///modified in EVSE
//extern uint8_t SeccIp[16];

/* Global Functions */

#define UDP_PAYLOAD_LEN 100

typedef struct udpRxData udpRxData;
struct udpRxData{
	uint16_t evccPort; /* some "random port" *///ev car port	//need to set to 0 Pev is set to 59219
	uint8_t sourceMac[6];
	uint8_t sourceIp[16];		// msg ip where it from incoming iP
	uint16_t sourcePort;		//msg port where it from
	uint16_t destinationPort;	//msg port where it go
	uint16_t udplen;
	uint16_t udpsum;

	//for receiving part
	uint16_t udpPayloadLen; //2
	uint8_t* udpPayload;// = &myethreceivebuffer[62];	//&v2gtpFrame[8]	//62

	//uint8_t udpPayload[UDP_PAYLOAD_LEN];

	//for PEV transmitting part
	//uint8_t v2gtpFrameLen; //10
	//uint8_t UdpRequestLen;
	//uint8_t IpRequestLen;

	//uint8_t* v2gtpFrame;// = &myethtransmitbuffer[14+40+8];
	//uint8_t* UdpRequest;// = &myethtransmitbuffer[14+40];
	//uint8_t* IpRequest;// = &myethtransmitbuffer[14];
};


typedef struct udpTxData udpTxData;
struct udpTxData{
		//for EVSE transmitting part
		uint8_t SdpPayloadLen;
		uint8_t V2GframeLen;
		uint8_t UdpResponseLen;
		uint8_t IpResponseLen;

		uint8_t* SdpPayload;// = &myethtransmitbuffer[14+40+8+8]; //at V2GframeLen[8] //# SDP response has 20 bytes		//original size
		uint8_t* V2Gframe;// = &myethtransmitbuffer[14+40+8];//at UdpResponse[8]
		uint8_t* UdpResponse;// = &myethtransmitbuffer[14+40];
		uint8_t* IpResponse;// = &myethtransmitbuffer[14];
};

//for receiving part
//uint16_t udpPayloadLen; //2
//uint8_t* udpPayload = &myethreceivebuffer[62];	//&v2gtpFrame[8]	//62
//#define UDP_PAYLOAD_LEN 100
//uint8_t udpPayload[UDP_PAYLOAD_LEN];

//for PEV transmitting part
//uint8_t v2gtpFrameLen; //10
//uint8_t UdpRequestLen;
//uint8_t IpRequestLen;

//static uint8_t* const v2gtpFrame = &myethtransmitbuffer[14+40+8];
//static uint8_t* const UdpRequest = &myethtransmitbuffer[14+40];
//static uint8_t* const IpRequest = &myethtransmitbuffer[14];
//for EVSE transmitting part
//uint8_t SdpPayloadLen;
//uint8_t V2GframeLen;
//uint8_t UdpResponseLen;
//uint8_t IpResponseLen;

//static uint8_t* const SdpPayload = &myethtransmitbuffer[14+40+8+8]; //at V2GframeLen[8] //# SDP response has 20 bytes		//original size
//static uint8_t* const V2Gframe = &myethtransmitbuffer[14+40+8];//at UdpResponse[8]
//static uint8_t* const UdpResponse = &myethtransmitbuffer[14+40];
//static uint8_t* const IpResponse = &myethtransmitbuffer[14];

typedef struct udp udp;
struct udp{
	QCA7000* QCA;

	udpRxData rx;
	udpTxData tx;

	uint16_t evccPort;
	uint8_t pevMac[6];
};



void udp_ctor(udp* const me,QCA7000* QCA);

void ipv6_evaluateReceivedPacket(udp* const me);
void ipv6_initiateSdpRequest(udp* const me);


#endif
