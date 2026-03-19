
#include "ccs32_globals.h"
#include "tcp.h"
#include "udpChecksum.h"






/* Todo: implement a retry strategy, to cover the situation that single packets are lost on the way. */

#define NEXT_TCP 0x06 /* the next protocol is TCP */

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20



#define TCP_STATE_CLOSED 0
#define TCP_STATE_SYN_SENT 1
#define TCP_STATE_ESTABLISHED 2

#define TCP_ACTIVITY_TIMER_START (5*33) /* 5 seconds */
//uint16_t tcpActivityTimer;

#define TCP_TRANSMIT_PACKET_LEN 200


extern const uint8_t myMAC[6];


void tcp_ctor(tcp* const me,QCA7000* QCA){
	me->QCA = QCA;


	me->seccTcpPort = 15118;

	me->rx.Flag = 0;
	me->rxdataLen = 0;

	me->tx.Flag = 0;
	me->tx.SeqNr = 2200;

	me->tx.HeaderLen =0;
	me->tx.IpRequestLen = 0;
	me->tx.PacketLen = 0;
	me->tx.PayloadLen = 0;

	me->tx.IpRequest = &QCA->ETH.TxBuffer[14];
	me->tx.Packet = &QCA->ETH.TxBuffer[14+40];
	me->tx.Payload = &QCA->ETH.TxBuffer[14+40+20];
}


/*** local function prototypes ****************************************************/

void tcp_packRequestIntoEthernet(tcp* const me);
void tcp_packRequestIntoIp(tcp* const me);
void tcp_prepareTcpHeader(tcp* const me,uint8_t tcpFlag);
void tcp_sendAck(tcp* const me);
void tcp_sendFirstAck(tcp* const me);




/*** functions *********************************************************************/

void tcp_evaluateRxPacket(tcp* const me) {

  uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
  tcpRxData* rx = &me->rx;
  tcpTxData* tx = &me->tx;
  uint16_t RxPacketLen, RxHeaderLen;

  RxPacketLen = m16(RxBuffer[18],RxBuffer[19]);
  RxHeaderLen = ((uint16_t)RxBuffer[66]>>2); /* header length in byte */ //data offset(myethreceivebuffer[66]>>4) * 4;

  if (RxPacketLen >= RxHeaderLen) {
	rx->RxSize = RxPacketLen - RxHeaderLen;
  } else {
	  rx->RxSize = 0; /* no TCP payload data */
	return;
  } 

  rx->sourcePort = m16(RxBuffer[64],RxBuffer[65]);//evcc
  rx->destinationPort = m16(RxBuffer[56],RxBuffer[57]);//secc

  memcpy(rx->destinationMAC,&RxBuffer[0],6);		//nothing to do
  memcpy(rx->sourceMAC,&RxBuffer[6],6);				//evcc MAC

  memcpy(rx->sourceIP, &RxBuffer[22], 16);	//source ip where the msg from	evcc ip source
  memcpy(rx->destinationIP, &RxBuffer[22], 16);	//source ip where the msg from	evcc ip source

//  McuXFormat_xsprintf(strTmp,"[TCP]TcpPevMac = %x,%x,%x,%x,%x,%x",TcpPevMac[0],TcpPevMac[1],TcpPevMac[2],TcpPevMac[3],TcpPevMac[4],TcpPevMac[5]);
//  addToTrace(strTmp);
//  McuXFormat_xsprintf(strTmp,"[TCP]TcpPevIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",TcpPevIp[0],TcpPevIp[1],TcpPevIp[2],TcpPevIp[3],TcpPevIp[4],TcpPevIp[5],TcpPevIp[6],TcpPevIp[7],TcpPevIp[8],TcpPevIp[9],TcpPevIp[10],TcpPevIp[11],TcpPevIp[12],TcpPevIp[13],TcpPevIp[14],TcpPevIp[15]);
//  addToTrace(strTmp);
//  McuXFormat_xsprintf(strTmp,"[TCP]sourcePort = %d",sourcePort);
//  addToTrace(strTmp);
//  McuXFormat_xsprintf(strTmp,"[TCP]destinationPort = %d",destinationPort);
//  addToTrace(strTmp);


  if (rx->sourcePort != me->seccTcpPort) {			//modified for evse
	McuXFormat_xsprintf(strTmp, "[TCP]destinationPort= %d, seccTcpPort= %d", rx->destinationPort,me->seccTcpPort);
	addToTrace(strTmp);
	addToTrace("[TCP] wrong port.");
	return;/* wrong port */
//    return; /* wrong port */
  }
  me->tcpActivityTimer = TCP_ACTIVITY_TIMER_START;


  rx->SeqNr = m32(RxBuffer[58],RxBuffer[59],RxBuffer[60],RxBuffer[61]);
  rx->AckNr = m32(RxBuffer[52],RxBuffer[53],RxBuffer[54],RxBuffer[55]);
  rx->Flag = RxBuffer[67];

  McuXFormat_xsprintf(strTmp, "[TCP]remoteSeqNr= %ld", rx->SeqNr);
  addToTrace(strTmp);
  McuXFormat_xsprintf(strTmp, "[TCP]remoteAckNr= %ld", rx->AckNr);
  addToTrace(strTmp);
  McuXFormat_xsprintf(strTmp, "[IPV6][TCP]flags= 0x%x", rx->Flag);
  addToTrace(strTmp);


  //evse function

  switch(rx->Flag){
  	  case(TCP_FLAG_SYN):{
  		addToTrace("[TCP]flags= SYN. evcc connecting us.");
  		tx->SeqNr = rx->AckNr;/* The sequence number of our next transmit packet is given by the received ACK number. */
  		tx->AckNr = rx->SeqNr + 1;/* The ACK number of our next transmit packet is one more than the received seq number. */
  		tx->HeaderLen = 20; /* 20 bytes normal header, no options */
  		tx->PayloadLen = 0;   /* only the TCP header, no data is in the first ACK message. */
  		tcp_prepareTcpHeader(me,TCP_FLAG_SYN + TCP_FLAG_ACK);
  		tcp_packRequestIntoIp(me);	// this need to modify for evse
  		addToTrace("[TCP] evse sent ACK back for connection.");
  		break;
  	  }
  	  //PEV function
  	  case(TCP_FLAG_SYN + TCP_FLAG_ACK):{		//0x12/* This is the connection setup response from the server. *///PEV function
  		addToTrace("[TCP]flags = SYN + ACK");
		tx->SeqNr = rx->AckNr; /* The sequence number of our next transmit packet is given by the received ACK number. */
		tx->AckNr = rx->SeqNr + 1; /* The ACK number of our next transmit packet is one more than the received seq number. */
		tcp_sendFirstAck(me);
		break;
  	  }
  	  case(TCP_FLAG_ACK):{//0x10
  		  addToTrace("[TCP] received TCP_FLAG_ACK.and do nothing");
  		break;
  	  }
  	  case(TCP_FLAG_PSH + TCP_FLAG_ACK):{	//0x18
  		addToTrace("[TCP]flags = SYN + ACK");
  		tx->SeqNr = rx->AckNr; /* The sequence number of our next transmit packet is given by the received ACK number. */
  		tx->AckNr = rx->SeqNr + 1; /* The ACK number of our next transmit packet is one more than the received seq number. */
  		tcp_sendFirstAck(me);
  		if ((rx->RxSize > 0) && (rx->RxSize < TCP_RX_DATA_LEN)) {
  			addToTrace("[TCP] received TCP data and EVSE process.");
  			me->rxdataLen = rx->RxSize;
  			memcpy(me->rxdata, &RxBuffer[74], rx->RxSize);  /* provide the received data to the EVSE state application */
  		}
  		break;

  	  }
  	  case(TCP_FLAG_FIN + TCP_FLAG_ACK):{//0x11
		addToTrace("[TCP] 0x11 FIN + ACK");
		tx->SeqNr = rx->AckNr;
		tx->AckNr = rx->SeqNr + 1;
		tcp_sendFirstAck(me);
		addToTrace("[TCP] received FIN + ACK. send back ACK");

  		break;
  	  }
  	  case(TCP_FLAG_RST):{//0x04
  		addToTrace("[TCP] 0x04 TCP_FLAG_RST");
  		break;
  	  }
  	  default:{
  		addToTrace("[TCP] Unknown Flag");
  		break;
  	  }
  }



}

void tcp_connect(tcp* const me) {		//sent from evcc at the beginning
  tcpTxData* tx = &me->tx;
  //addToTrace("[TCP] Checkpoint301: connecting");
  //me->checkpointNumber = 301;
  tx->Packet[20] = 0x02; /* options: 12 bytes, just copied from the Win10 notebook trace */
  tx->Packet[21] = 0x04;
  tx->Packet[22] = 0x05;
  tx->Packet[23] = 0xA0;

  tx->Packet[24] = 0x01;
  tx->Packet[25] = 0x03;
  tx->Packet[26] = 0x03;
  tx->Packet[27] = 0x08;

  tx->Packet[28] = 0x01;
  tx->Packet[29] = 0x01;
  tx->Packet[30] = 0x04;
  tx->Packet[31] = 0x02;

  tx->HeaderLen = 32; /* 20 bytes normal header, plus 12 bytes options */
  tx->PayloadLen = 0;   /* only the TCP header, no data is in the connect message. */
  tcp_prepareTcpHeader(me,TCP_FLAG_SYN);
  tcp_packRequestIntoIp(me);
  me->tcpState = TCP_STATE_SYN_SENT;
  me->tcpActivityTimer=TCP_ACTIVITY_TIMER_START;
}

void tcp_sendFirstAck(tcp* const me) {
  tcpTxData* tx = &me->tx;
  //addToTrace("[TCP] sending first ACK");
  tx->HeaderLen = 20; /* 20 bytes normal header, no options */
  tx->PayloadLen = 0;   /* only the TCP header, no data is in the first ACK message. */
  tcp_prepareTcpHeader(me,TCP_FLAG_ACK);
  tcp_packRequestIntoIp(me);
}

void tcp_sendAck(tcp* const me) {
  tcpTxData* tx = &me->tx;
  //addToTrace("[TCP] sending ACK");
  tx->HeaderLen = 20; /* 20 bytes normal header, no options */
  tx->PayloadLen = 0;   /* only the TCP header, no data is in the first ACK message. */
  tcp_prepareTcpHeader(me,TCP_FLAG_ACK);
  tcp_packRequestIntoIp(me);
}

void tcp_transmit(tcp* const me){
	tcpTxData* tx = &me->tx;
	tx->HeaderLen = 20; /* 20 bytes normal header, no options */
    if ((tx->PayloadLen + tx->HeaderLen) < TCP_TRANSMIT_PACKET_LEN) {

      tcp_prepareTcpHeader(me,TCP_FLAG_PSH + TCP_FLAG_ACK); /* data packets are always sent with flags PUSH and ACK. */
      tcp_packRequestIntoIp(me);
    } else {
      addToTrace("Error: tcpPayload and header do not fit into TcpTransmitPacket.");
    }      

}


void tcp_testSendData(tcp* const me) {
	tcpTxData* tx = &me->tx;
    tx->HeaderLen = 20; /* 20 bytes normal header, no options */
    tx->PayloadLen = 3;   /* demo length */
    tx->Packet[tx->HeaderLen] = 0x55; /* demo data */
    tx->Packet[tx->HeaderLen+1] = 0xAA; /* demo data */
    tx->Packet[tx->HeaderLen+2] = 0xBB; /* demo data */
    tcp_prepareTcpHeader(me,TCP_FLAG_PSH + TCP_FLAG_ACK); /* data packets are always sent with flags PUSH and ACK. */
    tcp_packRequestIntoIp(me);

}


void tcp_prepareTcpHeader(tcp* const me, uint8_t tcpFlag) {
  uint16_t checksum;
  tcpRxData* rx = &me->rx;
  tcpTxData* tx = &me->tx;
  // # TCP header needs at least 24 bytes:
  // 2 bytes source port
  // 2 bytes destination port
  // 4 bytes sequence number
  // 4 bytes ack number
  // 4 bytes DO/RES/Flags/Windowsize
  // 2 bytes checksum
  // 2 bytes urgentPointer
  // n*4 bytes options/fill (empty for the ACK frame and payload frames)
  tx->Packet[0] = u8(me->seccTcpPort+1); /* source port */ //modifed for evse
  tx->Packet[1] = u8(me->seccTcpPort);
  tx->Packet[2] = u8(rx->sourcePort +1); /* destination port */
  tx->Packet[3] = u8(rx->sourcePort);

  McuXFormat_xsprintf(strTmp, "[TCP][PTH]TcpSeqNr = %ld", tx->SeqNr);
  addToTrace(strTmp);
  McuXFormat_xsprintf(strTmp, "[TCP][PTH]TcpAckNr = %ld", tx->AckNr);
  addToTrace(strTmp);

  tx->Packet[4] = u8(tx->SeqNr+3); 	/* sequence number */
  tx->Packet[5] = u8(tx->SeqNr+2);
  tx->Packet[6] = u8(tx->SeqNr+1);	//sequence number raw
  tx->Packet[7] = u8(tx->SeqNr);		//sequence number raw

  tx->Packet[8] = u8(tx->AckNr+3); /* acknowledgment number */
  tx->Packet[9] = u8(tx->AckNr+2);
  tx->Packet[10] = u8(tx->AckNr+1);		//acknowledgment number (raw)
  tx->Packet[11] = u8(tx->AckNr);			//acknowledgment number (raw)
  tx->PacketLen = tx->HeaderLen + tx->PayloadLen;
  tx->Packet[12] = (tx->HeaderLen/4) << 4; //header Length /* High-nibble: DataOffset in 4-byte-steps. Low-nibble: Reserved=0. */

  tx->Packet[13] = tx->Flag;					//Flags (SYN) or other
  const uint16_t TCP_RECEIVE_WINDOW = 1000;
  tx->Packet[14] = (uint8_t)(TCP_RECEIVE_WINDOW>>8);		//window non static number
  tx->Packet[15] = (uint8_t)(TCP_RECEIVE_WINDOW);		//big indian

  // checksum will be calculated afterwards
  tx->Packet[16] = 0;
  tx->Packet[17] = 0;

  tx->Packet[18] = 0; /* 16 bit urgentPointer. Always zero in our case. */
  tx->Packet[19] = 0;
  checksum = calculateUdpAndTcpChecksumForIPv6(tx->Packet, tx->PacketLen, rx->sourceIP, rx->destinationIP, NEXT_TCP);
  tx->Packet[16] = u8(checksum +1);
  tx->Packet[17] = u8(checksum);
}


void tcp_packRequestIntoIp(tcp* const me) {
	// # embeds the TCP into the lower-layer-protocol: IP, Ethernet
	uint8_t plen;
	tcpTxData* tx = &me->tx;
	tcpRxData* rx = &me->rx;
	tx->IpRequestLen = tx->PacketLen + 8 + 16 + 16; // # IP6 header needs 40 bytes:
												//  #   4 bytes traffic class, flow
												//  #   2 bytes destination port
												//  #   2 bytes length (incl checksum)
												//  #   2 bytes checksum
	//IPV6
	tx->IpRequest[0] = 0x60; // Version 6 # traffic class, flow
	tx->IpRequest[1] = 0;
	tx->IpRequest[2] = 0;
	tx->IpRequest[3] = 0;
	plen = tx->PacketLen; 	// Payload Length. Without headers.
	//TcpIpRequest[4] = plen >> 8;	// Payload Length.
	tx->IpRequest[4] = 0;
	tx->IpRequest[5] = plen;	// Payload Length.
	tx->IpRequest[6] = NEXT_TCP; 	// next level protocol, 0x06 = TCP in this case
	tx->IpRequest[7] = 0x80; 		// hop limit 0x0A 255 ?
	// We are the PEV. So the EvccIp is our own link-local IP address.
	//EvccIp = addressManager_getLinkLocalIpv6Address("bytearray");

	//McuXFormat_xsprintf(strTmp,"[TCP]Sending SeccIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",SeccIp[0],SeccIp[1],SeccIp[2],SeccIp[3],SeccIp[4],SeccIp[5],SeccIp[6],SeccIp[7],SeccIp[8],SeccIp[9],SeccIp[10],SeccIp[11],SeccIp[12],SeccIp[13],SeccIp[14],SeccIp[15]);
	//addToTrace(strTmp);
	//McuXFormat_xsprintf(strTmp,"[TCP]Sending TcpPevIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",TcpPevIp[0],TcpPevIp[1],TcpPevIp[2],TcpPevIp[3],TcpPevIp[4],TcpPevIp[5],TcpPevIp[6],TcpPevIp[7],TcpPevIp[8],TcpPevIp[9],TcpPevIp[10],TcpPevIp[11],TcpPevIp[12],TcpPevIp[13],TcpPevIp[14],TcpPevIp[15]);
	//addToTrace(strTmp);

	memcpy(&tx->IpRequest[8],rx->destinationIP,16);		//source ip
	memcpy(&tx->IpRequest[24],rx->sourceIP,16);		//destination ip
	//memcpy(&TcpIpRequest[40],TcpTransmitPacket,TcpTransmitPacketLen);	//used pointer to handle
	//showAsHex(TcpIpRequest, TcpIpRequestLen, "TcpIpRequest");
	tcp_packRequestIntoEthernet(me);
}

void tcp_packRequestIntoEthernet(tcp* const me) {
		tcpTxData* tx = &me->tx;
		tcpRxData* rx = &me->rx;

		//QCA7000_SPI* SPI = &me->QCA->SPI;
		QCA7000_ETH* ETH = &me->QCA->ETH;

        //# packs the IP packet into an ethernet packet
		ETH->TxSize = tx->IpRequestLen + 6 + 6 + 2; // # Ethernet header needs 14 bytes:
                                                       // #  6 bytes destination MAC
                                                       // #  6 bytes source MAC
                                                       // #  2 bytes EtherType
        //# fill the destination MAC with the MAC of the charger
        memcpy(&ETH->TxBuffer[0],rx->sourceMAC,6);	//detination MAC TcpPevMac
        memcpy(&ETH->TxBuffer[6],myMAC,6);	//detination MAC myMAC
        ETH->TxBuffer[12] = 0x86; // # 86dd is IPv6
        ETH->TxBuffer[13] = 0xdd;
        //memcpy(&myethtransmitbuffer[14],TcpIpRequest,TcpIpRequestLen); used pointer
        QCA7000_EthTransmit(me->QCA);
}




void tcp_Disconnect(tcp* const me) {
  /* we should normally use the FIN handshake, to tell the charger that we closed the connection.
  But for the moment, just go away silently, and use an other port for the next connection. The
  server will detect our absense sooner or later by timeout, this should be good enough. */    
	me->tcpState = TCP_STATE_CLOSED;
	addToTrace("[TCP] tcp_Disconnect close TCP connection.");
  /* use a new port */
  /* But: This causes multiple open connections and the Win10 messes-up them. */

}

uint8_t tcp_isClosed(tcp* const me) {
  return (me->tcpState == TCP_STATE_CLOSED);
}  

uint8_t tcp_isConnected(tcp* const me) {
  return (me->tcpState == TCP_STATE_ESTABLISHED);
}

void tcp_Mainfunction(tcp* const me) {
	 //if (connMgr_getConnectionLevel()<50) {
	  /* No SDP done. Means: It does not make sense to start or continue TCP. */
	  //tcpState = TCP_STATE_CLOSED;
	  //addToTrace("[TCP] close TCP CtLvl()<50");

	 // return;
	 //}
	 //if ((connMgr_getConnectionLevel()==50) && (tcpState == TCP_STATE_CLOSED)) {// pev function
	   /* SDP is finished, but no TCP connected yet. */
	   /* use a new port */
	 //  tcp_connect(); //find a new port and connect ???
	 //}
}
