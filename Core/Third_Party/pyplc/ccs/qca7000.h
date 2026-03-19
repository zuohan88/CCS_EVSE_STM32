/* Interface header for qca7000.c */

#ifndef QCA7000_H_
#define QCA7000_H_

#include "stm32g4xx_hal.h"

/* Global Defines */

//#define MY_SPI_TX_RX_BUFFER_SIZE 2100
//#define MY_ETH_TRANSMIT_BUFFER_LEN 250
//#define MY_ETH_RECEIVE_BUFFER_LEN 250


#define SPI_TX_RX_BUFFER_MAX 2100
#define ETH_TX_RX_BUFFER_MAX 250
//#define ETH_TX_BUFFER_LEN_MAX 250
//#define ETH_RX_BUFFER_LEN_MAX 250


/* Global Variables */

//extern uint16_t mySpiDataSize;
//extern uint8_t mySpiRxBuffer[MY_SPI_TX_RX_BUFFER_SIZE];
//extern uint8_t mySpiTxBuffer[MY_SPI_TX_RX_BUFFER_SIZE];
//extern uint32_t nTotalEthReceiveBytes; /* total number of bytes which has been received from the ethernet port */
//extern uint32_t nTotalTransmittedBytes;
//extern uint8_t myethtransmitbuffer[MY_ETH_TRANSMIT_BUFFER_LEN];
//extern uint16_t myethtransmitbufferLen; /* The number of used bytes in the ethernet transmit buffer */
//extern uint8_t myethreceivebuffer[MY_ETH_RECEIVE_BUFFER_LEN];
//extern uint16_t myethreceivebufferLen;
//extern const uint8_t myMAC[6];
//extern uint8_t nMaxInMyEthernetReceiveCallback, nInMyEthernetReceiveCallback;
//extern uint16_t nTcpPacketsReceived;

/* Global Functions */

//HandleTypeDef//
//PCD_BCD_MsgTypeDef
//TypeDef

typedef enum {
	ETHTYPE_UNKOWN = 0,
	ETHTYPE_HOMEPLUG = 0x88E1,
	ETHTYPE_IPv6 = 0x86dd,
}ETHTYPE;

typedef struct {
	uint8_t RxBuffer[SPI_TX_RX_BUFFER_MAX];
	uint8_t TxBuffer[SPI_TX_RX_BUFFER_MAX];
	uint16_t RxSize;	//EthRxLen;
	uint16_t TxSize;

	uint32_t nTotalTransmittedBytes;
	uint16_t nTcpPacketsReceived;
	SPI_HandleTypeDef* hspi;
}QCA7000_SPI;


typedef struct {
	uint8_t TxBuffer[ETH_TX_RX_BUFFER_MAX];
	uint8_t RxBuffer[ETH_TX_RX_BUFFER_MAX];
	uint16_t RxSize;	//EthRxLen;
	uint16_t TxSize; /* The number of used bytes in the ethernet transmit buffer */

	ETHTYPE etherType;
}QCA7000_ETH;


typedef struct {
	QCA7000_ETH ETH;
	QCA7000_SPI SPI;
}QCA7000;


void QCA7000_ctor(QCA7000* const me,SPI_HandleTypeDef* hspi);








void QCA7000_demo(QCA7000* const me);
void QCA7000_checkForReceivedData(QCA7000* const me);
void QCA7000_EthTransmit(QCA7000* const me);

uint16_t QCA7000_readSignature(QCA7000* const me);
void QCA7000_writeRegister(QCA7000* const me, uint16_t reg,uint16_t data);
uint16_t QCA7000_readRegister(QCA7000* const me,uint16_t reg);
void QCA7000_startUp(QCA7000* const me);
uint8_t QCA7000_startUp2(QCA7000* const me);
void QCA7000_reset(QCA7000* const me);
void QCA7000_shutDown(QCA7000* const me);
void QCA7000_powerUp(QCA7000* const me);

void QCA7000_enable_interrupts(QCA7000* const me);
void QCA7000_disable_interrupts(QCA7000* const me);
uint16_t QCA7000_read_interrupts(QCA7000* const me);
uint8_t QCA7000_thread(QCA7000* const me);

void QCA7000_SendEthFrame(QCA7000* const me);

#endif /* INC_APP_FUNC_H_ */
