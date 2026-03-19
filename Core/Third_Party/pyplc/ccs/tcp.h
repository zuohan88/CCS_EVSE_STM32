/* Interface header for tcp.c */

/* Global Defines */
#ifndef TCP_H_
#define TCP_H_


#include "qca7000.h"


#define TCP_RX_DATA_LEN 200
#define TCP_PAYLOAD_LEN 200

/* Global Variables */

//extern uint8_t tcp_rxdataLen;
//extern uint8_t tcp_rxdata[TCP_RX_DATA_LEN];
//extern uint8_t* tcp_rxdata;
//extern uint8_t tcpPayloadLen;
//extern uint8_t tcpPayload[TCP_PAYLOAD_LEN];

//extern uint8_t* tcpPayload;
//extern uint8_t* tcp_rxdata;
/* Global Functions */


//#define TCP_ACTIVITY_TIMER_START (5*33) /* 5 seconds */
//uint16_t tcpActivityTimer;

//#define TCP_TRANSMIT_PACKET_LEN 200
//uint8_t TcpIpRequestLen;
//static uint8_t* TcpIpRequest = &myethtransmitbuffer[14];
//uint8_t TcpTransmitPacketLen;
//static uint8_t* TcpTransmitPacket = &myethtransmitbuffer[14+40];
//uint8_t tcpHeaderLen;
//uint8_t tcpPayloadLen;
//uint8_t* tcpPayload = &myethtransmitbuffer[14+40+20];



//uint8_t tcpState = TCP_STATE_CLOSED;

//uint32_t TcpSeqNr = 2200; /* a "random" start sequence number */ //own number
//uint32_t TcpAckNr;		//reponse the number =
//uint8_t tcp_rxdataLen=0;
//uint8_t tcp_rxdata[TCP_RX_DATA_LEN];
//uint8_t* tcp_rxdata = &myethreceivebuffer[74];


//uint16_t evccTcpPort = 0;
//uint16_t seccTcpPort = 15118; /* the port number of the charger */// our server static port

//extern const uint8_t myMAC[6];		//from homeplug
//extern uint8_t pevMac[6] ; 			//from homeplug

//uint8_t TcpPevIp[16]={0};
//uint8_t TcpPevMac[6] = {0};


typedef struct tcpRxData tcpRxData;
struct tcpRxData{
	uint16_t Flag;
	uint32_t SeqNr; /* a "random" start sequence number */ //own number
	uint32_t AckNr;		//reponse the number =

	uint16_t sourcePort;
	uint8_t sourceMAC[6];
	uint8_t sourceIP[16];

	uint16_t destinationPort;
	uint8_t destinationMAC[6];
	uint8_t destinationIP[16];

	//uint8_t* Payload;
	uint8_t RxSize;
};

typedef struct tcpTxData tcpTxData;
struct tcpTxData{
	uint16_t Flag;
	uint32_t SeqNr; /* a "random" start sequence number */ //own number
	uint32_t AckNr;		//reponse the number =

	uint16_t sourcePort;
	uint8_t sourceMAC[6];
	uint8_t sourceIP[16];

	uint16_t destinationPort;
	uint16_t destinationMAC[6];
	uint16_t destinationIP[16];


	//uint8_t* TcpTransmitPacket = &myethtransmitbuffer[14+40];
	uint16_t HeaderLen;
	uint16_t PayloadLen;
	uint16_t PacketLen;
	uint16_t IpRequestLen;		//TcpIpRequest = &myethtransmitbuffer[14];

	uint8_t* Payload;
	uint8_t* Packet;
	uint8_t* IpRequest;
};


typedef struct tcp tcp;
struct tcp{
	QCA7000* QCA;


	uint16_t tcpActivityTimer;
	//uint8_t checkpointNumber;

	uint8_t tcpState;

	tcpRxData rx;
	tcpTxData tx;

	uint16_t seccTcpPort;// = 15118

	uint8_t rxdata[200];		//&myethreceivebuffer[74];
	uint16_t rxdataLen;

};


//extern void tcp_Mainfunction(tcp* const me);
void tcp_evaluateRxPacket(tcp* const me);
void tcp_Disconnect(tcp* const me);
void tcp_transmit(tcp* const me);

void tcp_ctor(tcp* const me,QCA7000* QCA);

#endif /* CCS_EVSEPREDEFINE_H_ */
