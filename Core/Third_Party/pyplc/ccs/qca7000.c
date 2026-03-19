
/* QCA7000 / QCA7005 PLC modem driver at STM32.
 github.com/uhi22/....
*/

/* The SPI interface is configured in the CubeIDE, by opening the .ioc file. The Device Configuration Tool
   generates the configuration code into the main.c.
   Here in the driver, we use the SPI via the interface function QCA7000_TxRx(), which
   is provided by the main.c.
   
   */


#include "ccs32_globals.h"
#include "qca7000.h"
#include "tcp.h"



#define QCA_RST_OFF 	HAL_GPIO_WritePin(QCA_RST_GPIO_Port, QCA_RST_Pin, 0)	//6  QCA7000_INT_Pin
#define QCA_RST_ON		HAL_GPIO_WritePin(QCA_RST_GPIO_Port, QCA_RST_Pin, 1) //6




#define	SPI_REG_BFR_SIZE        0x0100
#define SPI_REG_WRBUF_SPC_AVA   0x0200
#define SPI_REG_RDBUF_BYTE_AVA  0x0300
#define SPI_REG_SPI_CONFIG      0x0400
#define SPI_REG_INTR_CAUSE      0x0C00
#define SPI_REG_INTR_ENABLE     0x0D00
#define SPI_REG_RDBUF_WATERMARK 0x1200
#define SPI_REG_WRBUF_WATERMARK 0x1300
#define SPI_REG_SIGNATURE       0x1A00
#define SPI_REG_ACTION_CTRL     0x1B00



#define SPI_INT_WRBUF_BELOW_WM (1 << 10)
#define SPI_INT_CPU_ON         (1 << 6)
#define SPI_INT_ADDR_ERR       (1 << 3)
#define SPI_INT_WRBUF_ERR      (1 << 2)
#define SPI_INT_RDBUF_ERR      (1 << 1)
#define SPI_INT_PKT_AVLBL      (1 << 0)

#define QCA7K_SPI_READ (1 << 15)
#define QCA7K_SPI_WRITE (0 << 15)
#define QCA7K_SPI_INTERNAL (1 << 14)
#define QCA7K_SPI_EXTERNAL (0 << 14)

#define QCASPI_GOOD_SIGNATURE 0xAA55
#define QCASPI_HW_BUF_LEN 0xC5B

/* Private variables ---------------------------------------------------------*/


//extern const uint8_t myMAC[6];
//extern uint8_t numberOfFoundModems;


//uint16_t mySpiDataSize;
//uint8_t mySpiRxBuffer[MY_SPI_TX_RX_BUFFER_SIZE];
//uint8_t mySpiTxBuffer[MY_SPI_TX_RX_BUFFER_SIZE];
//uint32_t nTotalTransmittedBytes;
//uint16_t nTcpPacketsReceived;

//uint8_t myethtransmitbuffer[MY_ETH_TRANSMIT_BUFFER_LEN];
//uint16_t myethtransmitbufferLen; /* The number of used bytes in the ethernet transmit buffer */
//uint8_t myethreceivebuffer[MY_ETH_RECEIVE_BUFFER_LEN];
//uint16_t ETH->RxSize;	//EthRxLen;

//uint16_t debugCounter_cutted_ETH->RxSize;

//extern uint8_t numberOfFoundModems;

void QCA7000_ctor(QCA7000* const me,SPI_HandleTypeDef* hspi){
	me->ETH.etherType = 0;
	me->SPI.hspi = hspi;

};


/* Local Functions ---------------------------------------------------------*/
void QCA7000_TxRx(QCA7000* const me);




/* Public and Local Functions ---------------------------------------------------------*/
void QCA7000_TxRx(QCA7000* const me){		//hspi
  //QCA7000_ETH* ETH = &me->ETH;
  QCA7000_SPI* SPI = &me->SPI;
  //memset(mySpiRxBuffer,0,mySpiDataSize);
  QCA_CS_OFF;
  HAL_SPI_TransmitReceive(SPI->hspi, SPI->TxBuffer, SPI->RxBuffer, SPI->TxSize, 200);
  QCA_CS_ON;
  //McuXFormat_xsprintf(strTmp,"[SPI] Sent SPI Size %d .",mySpiDataSize);
  //addToTrace(strTmp);
  //showAsHex(mySpiTxBuffer, mySpiDataSize, "[SPI] mySpiTxBuffer");
  //showAsHex(mySpiRxBuffer, mySpiDataSize, "[SPI] mySpiRxBuffer");

}


void QCA7000_setup(QCA7000* const me) {
  /* nothing to do here. The SPI interface setup is done in the main.c in the
  generated code of the Device Configuration Tool */
}

uint16_t QCA7000_readSignature(QCA7000* const me){
  /* Demo for reading the signature of the QCA7000. This should show AA55. */
	return QCA7000_readRegister(me,SPI_REG_SIGNATURE);
}


uint16_t QCA7000_ReadWRBUF_SPC_AVA(QCA7000* const me) {			//read write buffer size
  /* Demo for reading the available write buffer size from the QCA7000 */
	return QCA7000_readRegister(me,SPI_REG_WRBUF_SPC_AVA);
}

void QCA7000_WriteBFR_SIZE(QCA7000* const me, uint16_t n) {
  QCA7000_writeRegister(me,SPI_REG_BFR_SIZE,n);//internal, reg 1

}


uint16_t QCA7000_ReadRDBUF_BYTE_AVA(QCA7000* const me) {		//return size of buffer
  /* Demo for retrieving the amount of available received data from QCA7000 */
  return QCA7000_readRegister(me,SPI_REG_RDBUF_BYTE_AVA );
}



void QCA7000_checkRxDataAndDistribute(QCA7000* const me, int16_t DataSize) {
  QCA7000_ETH* ETH = &me->ETH;
  QCA7000_SPI* SPI = &me->SPI;
  uint16_t  L1, L2;
  uint8_t *p;
  uint8_t  blDone = 0; 
  uint16_t size;

  ETH->etherType = ETHTYPE_UNKOWN;

  static uint8_t buffer[SPI_TX_RX_BUFFER_MAX]={0};
  static uint16_t bufferLen = 0;


  p = buffer;

  if(DataSize <SPI_TX_RX_BUFFER_MAX){
	  my_memcpy(buffer,SPI->RxBuffer+2,DataSize);
	  bufferLen = DataSize;
  }else{
	  addToTrace( "[QCA] buffer overflow");

  }

  //showAsHex(buffer, bufferLen, "[QCA]buffer");
  if(bufferLen < 10) return;		//too short  ethernet package is 64
  while (!blDone) {  /* The SPI receive buffer may contain multiple Ethernet frames. Run through all. */
      /* the SpiRxBuffer contains more than the ethernet frame:
        4 byte length // i got 55 byte sometime 55 00 00 00 52 just delete starting data if not 00
        4 byte start of frame AA AA AA AA (5,6,7,8)
        2 byte frame length, little endian
        2 byte reserved 00 00
        payload
        2 byte End of frame, 55 55 */
      /* The higher 2 bytes of the len are assumed to be 0. */
      /* The lower two bytes of the "outer" len, big endian: */
	  if((p[0] == 0) && (p[1] == 0) && (p[4] == 0xAA) && (p[5] == 0xAA) && (p[6] == 0xAA) && (p[7] == 0xAA)){
		  L1 = m16(p[2],p[3]);			//total Length of generated data
		  if(bufferLen >= L1 + 4){
			  if((p[L1+3] == 0x55) && (p[L1+2] == 0x55)){					//complete data then can process can check 0x55 as well
				  if(bufferLen == (L1+4)){					//complete data then can process
					  addToTrace( "[QCA] It is a complete msg");
				  }else if(bufferLen > (L1+4)){			//got multiple data inside
					  addToTrace( "[QCA] It got multiple msg");
				  }
				  L2 = m16(p[9],p[8]);			//ethernet packet size
				  if (L2 > ETH_TX_RX_BUFFER_MAX){
					  L2 = ETH_TX_RX_BUFFER_MAX;
				  }
				  ETH->RxSize = L2;
				  my_memcpy(ETH->RxBuffer, &p[12], L2);
				  ETH->etherType = m16(ETH->RxBuffer[12],ETH->RxBuffer[13]);

				  //if (ETH->etherType == 0x88E1) { /* it is a HomePlug message */
					///  ETH->RxEthType = ETHTYPE_HOMEPLUG;
				//	  addToTrace( "[QCA]HomePlug message.");
					  //evaluateReceivedHomeplugPacket();	//homeplug packet			//do slac packet
				 // } else if (ETH->etherType == 0x86dd) { /* it is an IPv6 frame */		//tcp or udp
				//	  ETH->RxEthType = ETHTYPE_IPv6;
				//	  addToTrace( "[QCA]IPv6 message.");
					  //ipv6_evaluateReceivedPacket();			//ipv6 check data udp or tcpv6.c
				//  } else {
				//	  ETH->RxEthType = ETHTYPE_UNKOWN;
				//	  McuXFormat_xsprintf(strTmp, "[QCA]Other message.Type=0x%X",ETH->etherType); //0x%04X
				//	  addToTrace(strTmp);
				//  }
			  }else{		//not ending point
				  addToTrace( "[QCA] wrong data");
			  }
			  size = L1 + 4;
			  bufferLen -= (size);
			  memmove(buffer, &buffer[size], bufferLen);
			  if(bufferLen==0)	blDone=1;
		  }else{	//not enough data need to wait for next round
			  addToTrace( "[QCA] It not complete msg,wait next round");
			  blDone=1;
		  }
	  }else{			//msg with not a starting point
		  addToTrace( "[QCA] error msg start with none starting point");
		  showAsHex(buffer, bufferLen, "[QCA]error buffer");
		  bufferLen= 0;	 //deletd all
		  blDone=1;
	  }
  }
}


void QCA7000_checkForReceivedData(QCA7000* const me) {	///QCA7000_main
  //QCA7000_ETH* ETH = &me->ETH;
  QCA7000_SPI* SPI = &me->SPI;
  /* checks whether the QCA7000 indicates received data, and if yes, fetches the data. */
  uint16_t availBytes= 0;

  availBytes = QCA7000_ReadRDBUF_BYTE_AVA(me);	//check available byte in QCA7000
  if(availBytes==0){
	  return; /* nothing to do */
  }
  //McuXFormat_xsprintf(strTmp, "[QCA]avail rx bytes: %d", availBytes);
  //addToTrace(strTmp);
  if (availBytes<4000) {
    if (availBytes+2 > SPI_TX_RX_BUFFER_MAX) {
        availBytes = SPI_TX_RX_BUFFER_MAX - 2;
    }
    /* announce to the QCA how many bytes we want to fetch */
    QCA7000_WriteBFR_SIZE(me,availBytes);
    //memset(mySpiTxBuffer, 0, availBytes); //clean up array
    SPI->TxBuffer[0]=0x80; /* 0x80 is read, external */
    SPI->TxBuffer[1]=0x00;

    SPI->TxSize = availBytes+2;
    QCA7000_TxRx(me);
    //showAsHex(mySpiRxBuffer, mySpiDataSize, "[Received SPI Data]");
    QCA7000_checkRxDataAndDistribute(me,availBytes); /* Takes the data from the SPI rx buffer, splits it into ethernet frames and distributes them. */
  }
  else{
	McuXFormat_xsprintf(strTmp, "[QCA]SPI buffer overflow");
	addToTrace(strTmp);
  }
}

void QCA7000_SendEthFrame(QCA7000* const me) {
  /* to send an ETH frame, we need two steps:
     1. Write the BFR_SIZE (internal reg 1)
     2. Write external, preamble, size, data */
/* Example (from CCM)
  The SlacParamReq has 60 "bytes on wire" (in the Ethernet terminology).
  The   BFR_SIZE is set to 0x46 (command 41 00 00 46). This is 70 decimal.
  The transmit command on SPI is
  00 00  
  AA AA AA AA
  3C 00 00 00    (where 3C is 60, matches the "bytes on wire")
  <60 bytes payload>
  55 55 After the footer, the frame is finished according to the qca linux driver implementation.
  xx yy But the Hyundai CCM sends two bytes more, either 00 00 or FE 80 or E1 FF or other. Most likely not relevant.
  Protocol explanation from https://chargebyte.com/assets/Downloads/an4_rev5.pdf 
*/
  /* Todo:
    1. Check whether the available transmit buffer size is big enough to get the intended frame.
       If not, this is an error situation, and we need to instruct the QCA to heal, e.g. by resetting it.
  */
  QCA7000_ReadWRBUF_SPC_AVA(me);
  QCA7000_ETH* ETH = &me->ETH;
  QCA7000_SPI* SPI = &me->SPI;

  uint16_t TxSize = ETH->TxSize;
  //uint8_t* SPI_TxBuffer = SPI->TxBuffer;

  QCA7000_WriteBFR_SIZE(me, TxSize + 10); /* The size in the BFR_SIZE is 10 bytes more than in the size after the preamble below (in the original CCM trace) */

  SPI->TxBuffer[0] = 0x00; /* external write command */
  SPI->TxBuffer[1] = 0x00;
  SPI->TxBuffer[2] = 0xAA; /* Start of frame */
  SPI->TxBuffer[3] = 0xAA;
  SPI->TxBuffer[4] = 0xAA;
  SPI->TxBuffer[5] = 0xAA;
  SPI->TxBuffer[6] = (uint8_t)TxSize; /* LSB of the length */
  SPI->TxBuffer[7] = TxSize>>8; /* MSB of the length */
  SPI->TxBuffer[8] = 0x00; /* to bytes reserved, 0x00 */
  SPI->TxBuffer[9] = 0x00;
  my_memcpy(&SPI->TxBuffer[10], ETH->TxBuffer, TxSize); /* the ethernet frame */
  SPI->TxBuffer[10+TxSize] = 0x55; /* End of frame, 2 bytes with 0x55. Aka QcaFrmCreateFooter in the linux driver */
  SPI->TxBuffer[11+TxSize] = 0x55;
  SPI->TxSize = 12+TxSize;
  QCA7000_TxRx(me);
}

/* The Ethernet transmit function. */
void QCA7000_EthTransmit(QCA7000* const me) {
	//showAsHex(myethtransmitbuffer, myethtransmitbufferLen, "Sending myEthTransmit");
	QCA7000_SendEthFrame(me);
}


void QCA7000_SendSoftwareVersionRequest(QCA7000* const me) {		//not using
  //composeGetSwReq();
  QCA7000_SendEthFrame(me);
}


void QCA7000_demo(QCA7000* const me) {
  QCA7000_readSignature(me);
  //spiQCA7000DemoReadWRBUF_SPC_AVA();
  QCA7000_SendSoftwareVersionRequest(me);
  QCA7000_checkForReceivedData(me);
  QCA7000_checkForReceivedData(me);
}

//spi_config = qcaspi_read_register(qca, SPI_REG_SPI_CONFIG);
	//qcaspi_write_register(qca, SPI_REG_SPI_CONFIG, spi_config | QCASPI_SLAVE_RESET_BIT);
void QCA7000_reset(QCA7000* const me){
	McuXFormat_xsprintf(strTmp, "RESET over SPI");
	addToTrace(strTmp);

	const uint32_t QCA7K_SLAVE_RESET_BIT    = 1 << 6;
	uint16_t config;
    /* Reset is the only known bit of the config register, so no point in making a wider API */
    config = QCA7000_readRegister(me,SPI_REG_SPI_CONFIG);//reg 4 QCA7K_REG_SPI_CONFIG */
    config = config | QCA7K_SLAVE_RESET_BIT;
	QCA7000_writeRegister(me,SPI_REG_SPI_CONFIG,config);
	//McuXFormat_xsprintf(strTmp, "Done RESET over SPI");
	addToTrace(strTmp);

	//numberOfFoundModems = 0;
};



//interrupts = <23 0x1>; /* rising edge */
//spi-max-frequency = <12000000>;
void QCA7000_enableInterrupts(QCA7000* const me){
										//(SPI_INT_CPU_ON | SPI_INT_PKT_AVLBL | SPI_INT_RDBUF_ERR | SPI_INT_WRBUF_ERR);
	QCA7000_writeRegister(me,SPI_REG_INTR_ENABLE,(SPI_INT_PKT_AVLBL));	/* 0x4 is write, internal, reg 4 QCA7K_REG_SPI_CONFIG */ //QCA7K_SPI_WRITE | QCA7K_SPI_INTERNAL | reg
   // addToTrace("[QCA]enable irq");

}

void QCA7000_disableInterrupts(QCA7000* const me){
	QCA7000_writeRegister(me,SPI_REG_INTR_ENABLE,0);	/* 0x4 is write, internal, reg 4 QCA7K_REG_SPI_CONFIG */ //QCA7K_SPI_WRITE | QCA7K_SPI_INTERNAL | reg
   // addToTrace("[QCA]disable irq");
}


uint16_t QCA7000_readRegister(QCA7000* const me, uint16_t reg){
	QCA7000_SPI* SPI = &me->SPI;
	SPI->TxSize = 4;
	SPI->TxBuffer[0]=0xC0 | (uint8_t)reg>>8; // Read= 0xC0
	SPI->TxBuffer[1]=0x00 | (uint8_t)reg;
	SPI->TxBuffer[2]=0x00;
	SPI->TxBuffer[3]=0x00;
	QCA7000_TxRx(me);
	return m16(SPI->RxBuffer[2],SPI->RxBuffer[3]);
}

void QCA7000_writeRegister(QCA7000* const me, uint16_t reg,uint16_t data){
	QCA7000_SPI* SPI = &me->SPI;
	SPI->TxSize = 4;
	SPI->TxBuffer[0]=0x40 | (uint8_t)reg>>8; // Write= 0x40,
	SPI->TxBuffer[1]=0x00 | (uint8_t)reg;
	SPI->TxBuffer[2]=u8(data+1);
	SPI->TxBuffer[3]=u8(data);
	QCA7000_TxRx(me);
}
void QCA7000_shutDown(QCA7000* const me){
	QCA_RST_OFF;
}

void QCA7000_powerUp(QCA7000* const me){
	QCA_RST_ON;
}


void QCA7000_startUp(QCA7000* const me){
	uint16_t sig;
	uint16_t wrbuf_space;

	QCA_RST_ON;
	//HAL_Delay(1000);
	//QCA7000_reset();
	HAL_Delay(2000);

	do{
		//sig=QCA7000_read_signature();
		sig = QCA7000_readRegister(me,0x1A);	//read_signature
		McuXFormat_xsprintf(strTmp, "Sig is %x\r\n", sig);/* should be AA 55  */
		addToTrace(strTmp);
	}
	while(sig != QCASPI_GOOD_SIGNATURE);

	sig = QCA7000_readRegister(me,0x1A);
	if (sig != QCASPI_GOOD_SIGNATURE) {
		addToTrace("qcaspi: sync: got CPU on, but signature was invalid, restart\n"); /* should be AA 55  */
		QCA7000_reset(me);		//this didnt work
		//qca->sync = QCASPI_SYNC_UNKNOWN;
	} else {
		/* ensure that the WRBUF is empty */
		wrbuf_space = QCA7000_ReadWRBUF_SPC_AVA(me);
		if (wrbuf_space != QCASPI_HW_BUF_LEN){
			addToTrace("qcaspi: sync: got CPU on, but wrbuf not empty. reset!\n");
			QCA7000_reset(me);
		} else {
			addToTrace("qcaspi: sync: got CPU on, now in sync\n");
		}
	}
	QCA7000_enableInterrupts(me);
	return;
}




uint8_t QCA7000_startUp2(QCA7000* const me){
	uint16_t sig;
	uint16_t wrbuf_space;
	uint32_t i = 0;
	uint8_t r = 0;

	for(i=0;i<100;i++){
		sig = QCA7000_readRegister(me, SPI_REG_SIGNATURE);	//read_signature
		if(sig == QCASPI_GOOD_SIGNATURE){
			r = 1;
			break;
		}
	}
	if(r == 0){
		addToTrace("[QCA7000]failed to get AA55");
		addToTrace("[QCA7000]got CPU on, but signature was invalid, restart"); /* should be AA 55  */
		QCA7000_reset(me);
	}else{
		wrbuf_space = QCA7000_ReadWRBUF_SPC_AVA(me);
		if (wrbuf_space != QCASPI_HW_BUF_LEN){
			addToTrace("[QCA7000]got CPU on, but wrbuf not empty. reset!");
			QCA7000_reset(me);
		} else {
			addToTrace("[QCA7000]got CPU on, now in sync");
		}
	}
	QCA7000_enableInterrupts(me);
	//numberOfFoundModems = 0;
	return r;
};


uint16 QCA7000_readInterrupts(QCA7000* const me){
	uint16_t sig;
	sig = QCA7000_readRegister(me,SPI_REG_INTR_ENABLE);
	mySerialPrint();
	McuXFormat_xsprintf(strTmp, "[QCA7000]IRQ is %x\r\n", sig);/* should be AA 55  */
	addToTrace(strTmp);
	return sig;
}


uint8_t QCA7000_thread(QCA7000* const me) {	//
	uint16_t vInterruptCause=0;
	//uint16_t IRQtype = 0;
	uint16_t sig = 0;
	uint8_t ret	=0;

	//IRQtype = QCA7000_readRegister(me,SPI_REG_INTR_ENABLE);
	QCA7000_disableInterrupts(me);
	vInterruptCause = QCA7000_readRegister(me,SPI_REG_INTR_CAUSE);

	//McuXFormat_xsprintf(strTmp, "[SPI] interrupts: 0x%08x", vInterruptCause);/*
	//addToTrace(strTmp);
	if (vInterruptCause & SPI_INT_CPU_ON) {
		/* not synced. */
		//addToTrace("[QCA] CPU interrupts");
		sig=QCA7000_readRegister(me,SPI_REG_SIGNATURE);
		sig=QCA7000_readRegister(me,SPI_REG_SIGNATURE);
		if (sig != QCASPI_GOOD_SIGNATURE) {
			addToTrace("[QCA] got CPU on, but signature was invalid, restart\n");
			//TODO
		}
	}
	if (vInterruptCause & SPI_INT_RDBUF_ERR) {
		addToTrace("[QCA] RD BUF ERROR");
	}
	if (vInterruptCause & SPI_INT_WRBUF_ERR) {
		addToTrace("[QCA] INT WRBUF ERROR");
	}
	if (vInterruptCause & SPI_INT_PKT_AVLBL) {
		//addToTrace("[QCA] PKT AVLBL");
		ret=1;
	}
	QCA7000_enableInterrupts(me);
	return ret;

}





