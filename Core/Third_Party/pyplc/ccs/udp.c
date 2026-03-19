
#include "ccs32_globals.h"
#include "udp.h"
#include "tcp.h"


extern const uint8_t myMAC[6];		//myMAC act  evseMAC
extern tcp tcp1;
//multicast ip
const uint8_t broadcastIPv6[16] = { 0xff, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
/* our link-local IPv6 address. Todo: do we need to calculate this from the MAC? Or just use a "random"? */
/* For the moment, just use the address from the Win10 notebook, and change the last byte from 0x0e to 0x1e. */
uint8_t EvccIpCAT[16] = {0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0x02, 0xb0, 0x52, 0xff, 0xfe, 0x00, 0x00, 0x03};
uint8_t EvccIp[16] = {0};//{0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0xc6, 0x90, 0x83, 0xf3, 0xfb, 0xcb, 0x98, 0x1e};			//PEV MAC ipv6
uint8_t SeccIp[16]= {0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0x06, 0xaa, 0xaa, 0xff, 0xfe, 0, 0xaa, 0xaa}; /* the IP address of the charger *///pre-set here #charge station IP
//uint8_t SeccIp[16]= {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0, 0, 0x66, 0x99, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA}; /* the IP address of the charger *///pre-set here #charge station IP
//# 16 bytes, a default IPv6 address for the charging station
//# self.SeccIp = [ 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0x06, 0xaa, 0xaa, 0xff, 0xfe, 0, 0xaa, 0xaa ]
//# fe80::e0ad:99ac:52eb:85d3 is the Win10 laptop
//# self.SeccIp = [ 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0xe0, 0xad, 0x99, 0xac, 0x52, 0xeb, 0x85, 0xd3 ]
//uint16_t seccPort = 15118;																						//local port fo evse UDP static port set by us

//uint16_t evccPort=0; /* some "random port" *///ev car port	//need to set to 0 Pev is set to 59219

//uint8_t sourceMac[6];
//uint8_t sourceIp[16]={0};		// msg ip where it from incoming iP
//uint16_t sourceport=0;		//msg port where it from
//uint16_t destinationport=0;	//msg port where it go
//uint16_t udplen;
//uint16_t udpsum;




#define NEXT_UDP 0x11 /* next protocol is UDP */
#define NEXT_ICMPv6 0x3a /* next protocol is ICMPv6 */

#define NET_ICMPV6_DST_UNREACH    1	/* Destination unreachable */
#define NET_ICMPV6_PACKET_TOO_BIG 2	/* Packet too big */
#define NET_ICMPV6_TIME_EXCEEDED  3	/* Time exceeded */
#define NET_ICMPV6_PARAM_PROBLEM  4	/* IPv6 header is bad */
#define NET_ICMPV6_MLD_QUERY    130	/* Multicast Listener Query */
#define NET_ICMPV6_RS           133	/* Router Solicitation */
#define NET_ICMPV6_RA           134	/* Router Advertisement */
#define NET_ICMPV6_NS           135	/* Neighbor Solicitation */
#define NET_ICMPV6_NA           136	/* Neighbor Advertisement */
#define NET_ICMPV6_MLDv2        143	/* Multicast Listener Report v2

//#define iAmEvse 1
//#define iAmPev 0


/*** local function prototypes ******************************************/
//EVSE function

void udp_ctor(udp* const me,QCA7000* QCA){
	me->QCA = QCA;

	me->rx.udpPayloadLen = 0;//2
	me->rx.udpPayload = &QCA->ETH.RxBuffer[62];	//&v2gtpFrame[8]	//62

	me->tx.SdpPayloadLen = 0;
	me->tx.V2GframeLen = 0;//10
	me->tx.UdpResponseLen = 0;
	me->tx.IpResponseLen = 0;
	me->tx.SdpPayload = &QCA->ETH.TxBuffer[14+40+8+8]; //at V2GframeLen[8] //# SDP response has 20 bytes		//original size
	me->tx.V2Gframe = &QCA->ETH.TxBuffer[14+40+8];		//at UdpResponse[8]
	me->tx.UdpResponse = &QCA->ETH.TxBuffer[14+40];	// = &myethtransmitbuffer[14+40];
	me->tx.IpResponse = &QCA->ETH.TxBuffer[14];// = &myethtransmitbuffer[14];
};

//local function private
void sendSdpResponse(udp* const me);
void packResponseIntoUdp(udp* const me);
void packResponseIntoIp(udp* const me);
void packResponseIntoEthernet(udp* const me);
//PEV function;


void evaluateNeighborSolicitation(udp* const me);
void evaluateRouterSolicitation(udp* const me);
void evaluateRouterSolicitation2(udp* const me);
void evaluateRouterSolicitation3(udp* const me);
void evaluateRouterSolicitation4(udp* const me);



//uint8_t ownMac[6] ={0x9C,0xB6,0xD0,0xC1,0x93,0x3F};		///need to preset here		//change to myMAC
//uint8_t faultInjectionSuppressSdpResponse=0;


//extern uint8_t pevMac[6] ; 			//from homeplug



/*** functions **********************************************************/
//EVSE function
//*********************************************************

void packResponseIntoUdp(udp* const me){		//28 bytes evse added
	udpTxData* tx = &me->tx;

//# embeds the (SDP) response into the lower-layer-protocol: UDP
	uint16_t lenInclChecksum;
	uint16_t checksum;
	const uint16_t port = 15118;

	tx->UdpResponseLen = tx->V2GframeLen + 8;
	//uint8_t UdpResponse[36] = {0}; //# UDP needs 8 bytes: = 28 + 8
												  //#   2 bytes source port
												  //#   2 bytes destination port
												  // #   2 bytes length (incl checksum)
												  //#   2 bytes checksum

	tx->UdpResponse[0] = u8(port +1);			//source port
	tx->UdpResponse[1] = u8(port);			//
	tx->UdpResponse[2] = u8(me->evccPort+1);			//destination port
	tx->UdpResponse[3] = u8(me->evccPort);
	lenInclChecksum = tx->UdpResponseLen;
	tx->UdpResponse[4] = u8(lenInclChecksum + 1);
	tx->UdpResponse[5] = u8(lenInclChecksum);
	//# checksum will be calculated afterwards
	//UdpResponse[6] = 0;
	//UdpResponse[7] = 0;
	//my_memcpy(&UdpResponse[8], V2Gframe, V2GframeLen);
    //McuXFormat_xsprintf(strTmp,"(UDP response)(EVSE)evccPort = %d",evccPort);
    //addToTrace(strTmp);
	//showAsHex(UdpResponse, UdpResponseLen, "UDP response ");
	//# The content of buffer is ready. We can calculate the checksum. see https://en.wikipedia.org/wiki/User_Datagram_Protocol

    McuXFormat_xsprintf(strTmp,"[UDP]evccPort = %u",me->evccPort);
    addToTrace(strTmp);

	checksum = calculateUdpAndTcpChecksumForIPv6(tx->UdpResponse,(uint16_t)tx->UdpResponseLen, SeccIp, EvccIp,NEXT_UDP);
	tx->UdpResponse[6] = u8(checksum+1);
	tx->UdpResponse[7] = u8(checksum);
	//packResponseIntoIp();		//36 bytes
}

void packResponseIntoIp(udp* const me){		//36 bytes input  evse added
	uint16_t plen;
	uint8_t *IPptr;

	udpTxData* tx = &me->tx;

	//# embeds the (SDP) response into the lower-layer-protocol: IP, Ethernet
	tx->IpResponseLen = tx->UdpResponseLen+ 8 +16 +16; //76;
	//uint8_t IpResponse[76]={0};//= bytearray(len(buffer) + 8 + 16 + 16) //# IP6 needs 40 bytes:
	//#   4 bytes traffic class, flow
	//#   2 bytes destination port
	//#   2 bytes length (incl checksum)
	//#   2 bytes checksum
	tx->IpResponse[0] = 0x60;// # traffic class, flow
	tx->IpResponse[1] = 0;
	tx->IpResponse[2] = 0;
	tx->IpResponse[3] = 0;
	plen = tx->UdpResponseLen;//len(buffer) //# length of the payload. Without headers.
	tx->IpResponse[4] = u8(plen +1);
	tx->IpResponse[5] = u8(plen );
	tx->IpResponse[6] = 0x11; //# next level protocol, 0x11 = UDP in this case
	tx->IpResponse[7] = 0x0A; //# hop limit
	my_memcpy(&tx->IpResponse[8], SeccIp, 16);	//EVSEIP as myIP as source IP
	my_memcpy(&tx->IpResponse[24], EvccIp, 16);	//destination IP address
	//my_memcpy(&IpResponse[40], UdpResponse, UdpResponseLen);		//done by pointer
	IPptr = &tx->IpResponse[8];
    McuXFormat_xsprintf(strTmp,"[UDP](from IP Source)SeccIp = %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",IPptr[0],IPptr[1],IPptr[2],IPptr[3],IPptr[4],IPptr[5],IPptr[6],IPptr[7],IPptr[8],IPptr[9],IPptr[10],IPptr[11],IPptr[12],IPptr[13],IPptr[14],IPptr[15]);
    addToTrace(strTmp);
    IPptr = &tx->IpResponse[24];
    McuXFormat_xsprintf(strTmp,"[UDP](to IP destination)EvccIp = %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",IPptr[0],IPptr[1],IPptr[2],IPptr[3],IPptr[4],IPptr[5],IPptr[6],IPptr[7],IPptr[8],IPptr[9],IPptr[10],IPptr[11],IPptr[12],IPptr[13],IPptr[14],IPptr[15]);
    addToTrace(strTmp);
    //packResponseIntoEthernet();		//76 bytes
}

void packResponseIntoEthernet(udp* const me){			//76bytes
	uint8_t *MACptr;

	udpTxData* tx = &me->tx;
	udpRxData* rx = &me->rx;
	//QCA7000* QCA = me->QCA;
	QCA7000_ETH* ETH = &me->QCA->ETH;



    //# packs the IP packet into an ethernet packet
    //char EthTxFrame[90] = {0}; ///= bytearray(len(buffer) + 6 + 6 + 2) # Ethernet header needs 14 bytes:
                                                  //#  6 bytes destination MAC
                                                  //#  6 bytes source MAC
                                                  //#  2 bytes EtherType
	ETH->TxSize = tx->IpResponseLen + 6 + 6 + 2;//90;

	//# fill the destination MAC with the source MAC of the received package
	my_memcpy(&ETH->TxBuffer[0], rx->sourceMac, 6);	//destination mac
	my_memcpy(&ETH->TxBuffer[6], myMAC, 6);				//source mac
	ETH->TxBuffer[12] = 0x86; //# 86dd is IPv6
	ETH->TxBuffer[13] = 0xdd;
	//my_memcpy(&TxBuffer[14], IpResponse, IpResponseLen);	//done with pointer at beginning
    MACptr = &ETH->TxBuffer[0];
    McuXFormat_xsprintf(strTmp,"[UDP]to destination MAC = %x %x %x %x %x %x",MACptr[0],MACptr[1],MACptr[2],MACptr[3],MACptr[4],MACptr[5]);
    addToTrace(strTmp);
    MACptr = &ETH->TxBuffer[6];
    McuXFormat_xsprintf(strTmp,"[UDP]from source MAC(MyMac) = %x %x %x %x %x %x",MACptr[0],MACptr[1],MACptr[2],MACptr[3],MACptr[4],MACptr[5]);
    addToTrace(strTmp);
    showAsHex(ETH->TxBuffer, ETH->TxSize,"[UDP][ETH]");
    QCA7000_EthTransmit(me->QCA);
}

//######################################################################################################################
//EVSE resposne to 9001
void sendSdpResponse(udp* const me){			//evse added
	udpTxData* tx = &me->tx;


	addToTrace("[UDP][SDP] sendSdpResponse");
	uint8_t *IPptr;
    //# SECC Discovery Response.
    //# The response from the charger to the EV, which transfers the IPv6 address of the charger to the car.
    //if (faultInjectionSuppressSdpResponse>0){
       // print("Fault injection: SDP response suppressed");
     //   faultInjectionSuppressSdpResponse-=1;
    //    return;
    //}
	tx->SdpPayloadLen = 20;			//
    //SdpPayload[20] = {0}; //# SDP response has 20 bytes		//original size

    my_memcpy(tx->SdpPayload, SeccIp, 16);	//# 16 bytes send IP address of the evse to evcc(PEV)
    //# Here the charger decides, on which port he will listen for the TCP communication.
    //# We use port 15118, same as for the SDP. But also dynamically assigned port would be ok.
    //# The alpitronics seems to use different ports on different chargers, e.g. 0xC7A7 and 0xC7A6.
    //# The ABB Triple and ABB HPC are reporting port 0xD121, but in fact (also?) listening
    //# to the port 15118.
    IPptr = tx->SdpPayload;
    McuXFormat_xsprintf(strTmp,"[UDP][SDP] SeccIp = %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",IPptr[0],IPptr[1],IPptr[2],IPptr[3],IPptr[4],IPptr[5],IPptr[6],IPptr[7],IPptr[8],IPptr[9],IPptr[10],IPptr[11],IPptr[12],IPptr[13],IPptr[14],IPptr[15]);
    addToTrace(strTmp);
    const uint16_t seccPort = 15118;		//constant value in python
    tx->SdpPayload[16] = u8(seccPort + 1); //# SECC port high byte.
    tx->SdpPayload[17] = u8(seccPort); //# SECC port low byte.
    tx->SdpPayload[18] = 0x10; //# security. We only support "no transport layer security, 0x10".
    tx->SdpPayload[19] = 0x00; //# transport protocol. We only support "TCP, 0x00".

    McuXFormat_xsprintf(strTmp,"[UDP][SDP] V2GframeLen = %u",seccPort);
    addToTrace(strTmp);
    //showAsHex(SdpPayload, SdpPayloadLen,"[SDP payload]");
    //# add the SDP header

    tx->V2GframeLen = tx->SdpPayloadLen + 8;
    //uint8_t V2Gframe[28] = {0}; //# V2GTP header needs 8 bytes:		bytearray(lenSdp + 8)
                                                //# 1 byte protocol version
                                                //# 1 byte protocol version inverted
                                                //# 2 bytes payload type
                                                //# 4 byte payload length

    tx->V2Gframe[0] = 0x01; //# version
    tx->V2Gframe[1] = 0xfe; //# version inverted
    tx->V2Gframe[2] = 0x90; //# payload type. 0x9001 is the SDP response message
    tx->V2Gframe[3] = 0x01; //#
    tx->V2Gframe[4] = u8(tx->SdpPayloadLen + 3); //# length 4 byte.
    tx->V2Gframe[5] = u8(tx->SdpPayloadLen + 2);
    tx->V2Gframe[6] = u8(tx->SdpPayloadLen + 1);
    tx->V2Gframe[7] = u8(tx->SdpPayloadLen );
    //my_memcpy(&V2Gframe[8], SdpPayload, SdpPayloadLen);
    McuXFormat_xsprintf(strTmp,"[UDP][SDP]SdpPayloadLen = %u",tx->SdpPayloadLen);
    addToTrace(strTmp);

    packResponseIntoUdp(me);//28 frame
    packResponseIntoIp(me);
    packResponseIntoEthernet(me);


}


//######################################################################################################################
//ipv6 UDP  msg check response
void udp_evaluatePayload(udp* const me) {		//evse modified
	udpRxData* rx = &me->rx;

	uint16_t v2gptPayloadType;
	uint32_t v2gptPayloadLen;
	uint8_t seccDiscoveryReqSecurity;
	uint8_t seccDiscoveryReqTransportProtocol;
	uint8_t *IPptr;
	if ((rx->udpPayload[0]!=0x01) && (rx->udpPayload[1]!=0xFE)) {
		addToTrace("[UDP] not protocol version 1 and inverted");
		return;
	}
	if (rx->destinationPort != 15118){
		addToTrace("[UDP] Wrong Destination Port");
		return;
	}
	 memcpy(me->pevMac,&me->QCA->ETH.RxBuffer[6],6);				//evcc MAC
	//# it is a V2GTP message
	//showAsHex(udpPayload,udpPayloadLen, "V2GTP Received");
	me->evccPort = 	rx->sourcePort; //#then the source is the vehicle
	my_memcpy(EvccIp, rx->sourceIp, 16);
    McuXFormat_xsprintf(strTmp,"[UDP] received evcc port = %d ",me->evccPort);
    addToTrace(strTmp);
    IPptr = EvccIp;
    McuXFormat_xsprintf(strTmp,"[UDP] EvccIp = %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",IPptr[0],IPptr[1],IPptr[2],IPptr[3],IPptr[4],IPptr[5],IPptr[6],IPptr[7],IPptr[8],IPptr[9],IPptr[10],IPptr[11],IPptr[12],IPptr[13],IPptr[14],IPptr[15]);
    addToTrace(strTmp);

	v2gptPayloadType = m16(rx->udpPayload[2],rx->udpPayload[3]);
	//# 0x8001 EXI encoded V2G message (Will NOT come with UDP. Will come with TCP.)
	//# 0x9000 SDP request message (SECC Discovery)
	//# 0x9001 SDP response message (SECC response to the EVCC)
	//**************************************************************
	//EVSE function
	if (v2gptPayloadType == 0x9000){		//evse added
		v2gptPayloadLen = m32(rx->udpPayload[4],rx->udpPayload[5],rx->udpPayload[6],rx->udpPayload[7]);


		//McuXFormat_xsprintf(strTmp,"v2gptPayloadLen = %d",v2gptPayloadLen);
		//addToTrace(strTmp);
		if (v2gptPayloadLen == 2){
			 //# 2 is the only valid length for a SDP request.
			 seccDiscoveryReqSecurity = rx->udpPayload[8];// # normally 0x10 for "no transport layer security". Or 0x00 for "TLS".
			 seccDiscoveryReqTransportProtocol = rx->udpPayload[9]; //# normally 0x00 for TCP
			 if (seccDiscoveryReqSecurity!=0x10){
				 McuXFormat_xsprintf(strTmp,"[UDP] seccDiscoveryReqSecurity 0x%x is not supported",seccDiscoveryReqSecurity);
				 addToTrace(strTmp);
			 }else if(seccDiscoveryReqTransportProtocol!=0x00){
				 McuXFormat_xsprintf(strTmp,"[UDP] seccDiscoveryReqTransportProtocol 0x%x is not supported",seccDiscoveryReqTransportProtocol);
				 addToTrace(strTmp);
			 }else{
				 //# This was a valid SDP request. Let's respond, if we are the charger.
				#ifdef iAmEvse
					addToTrace("[UDP] SDP 2: this was a valid SDP request. Sending SDP response.");
					sendSdpResponse(me);	//send back 9001 for network information
					addToTrace("[UDP] Done SDP 2: evseState");
				#endif
			 }
		}else{
			McuXFormat_xsprintf(strTmp,"[UDP] v2gptPayloadLen on SDP request is %ld not supported",v2gptPayloadLen);
			addToTrace(strTmp);
		}
	}else{		//no
		McuXFormat_xsprintf(strTmp, "[UDP] v2gptPayloadType %x not supported", v2gptPayloadType);
		addToTrace(strTmp);
	}
}


//######################################################################################################################
//ipv6 check function
void ipv6_evaluateReceivedPacket(udp* const me) {
  udpRxData* rx = &me->rx;
  uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;

  //# The evaluation function for received ipv6 packages.
  uint16_t nextheader; 
  uint8_t icmpv6type; 
  uint8_t *MACptr;
  if (me->QCA->ETH.RxSize > 60) {
      //# extract the source ipv6 address
	  my_memcpy(rx->sourceMac, &RxBuffer[6], 6);		//after added
      my_memcpy(rx->sourceIp, &RxBuffer[22], 16);	//source ip where the msg from	evcc ip source
      nextheader = RxBuffer[20];
      MACptr = rx->sourceMac;
      McuXFormat_xsprintf(strTmp,"[UDP][ipv6]sourceMac = %x %x %x %x %x %x",MACptr[0],MACptr[1],MACptr[2],MACptr[3],MACptr[4],MACptr[5]);
      addToTrace(strTmp);
      MACptr = rx->sourceIp;
      McuXFormat_xsprintf(strTmp,"[UDP][ipv6]sourceMac = %x %x %x %x %x %x",MACptr[0],MACptr[1],MACptr[2],MACptr[3],MACptr[4],MACptr[5]);
      addToTrace(strTmp);

      if (nextheader == 0x11) { //  it is an UDP frame
    	  addToTrace("[UDP] UDP received");

    	  rx->sourcePort =      m16(RxBuffer[54],RxBuffer[55]);	//evcc
    	  rx->destinationPort = m16(RxBuffer[56],RxBuffer[57]);	//secc
    	  rx->udplen = 			m16(RxBuffer[58],RxBuffer[59]);
    	  rx->udpsum = 			m16(RxBuffer[60],RxBuffer[61]);

          McuXFormat_xsprintf(strTmp,"[UDP] received source port = %d ",rx->sourcePort);
          addToTrace(strTmp);
          McuXFormat_xsprintf(strTmp,"[UDP] received destination port = %d ",rx->destinationPort);
          addToTrace(strTmp);

          //# udplen is including 8 bytes header at the begin
          if (rx->udplen > UDP_PAYLOAD_LEN) {
			/* ignore long UDP */
			addToTrace("[UDP] Ignoring too long UDP");
			return;
          }
          else if (rx->udplen>8) {
        	rx->udpPayloadLen = rx->udplen-8;
			//my_memcpy(udpPayload, &myethreceivebuffer[62], udpPayloadLen);	//done in it pointer//after 62
			addToTrace("[UDP] process UDP Packet");
			udp_evaluatePayload(me);				//local function
			//evseStateMachine_ReInit();
          }                      
      }
      else if (nextheader == 0x06) { // # it is an TCP frame
        addToTrace("[UDP][TCP] process TCP Packet");
        tcp_evaluateRxPacket(&tcp1);							//from tcp.c //local function in python	//do evse
      }
      else if (nextheader == NEXT_ICMPv6) { // it is an ICMPv6 (NeighborSolicitation etc) frame
        addToTrace("[UDP][ICMPv6] process ICMPv6 Packet");
        icmpv6type = RxBuffer[54];
        if (icmpv6type == 0x87) { /* Neighbor Solicitation */	//135
            addToTrace("[UDP][ICMPv6] Neighbor Solicitation received");
            evaluateNeighborSolicitation(me);		//not in python
        }else if(icmpv6type == 0x85) {	//133
        	addToTrace("[UDP][ICMPv6] Router Solicitation received");	//failed to response well with NS
            evaluateRouterSolicitation3(me);		//not in python
        }else{
      	  McuXFormat_xsprintf(strTmp,"[UDP][ICMPv6] unknown icmpv6type= 0x%x",icmpv6type);
      	  addToTrace(strTmp);
        }
      }else{
    	  McuXFormat_xsprintf(strTmp,"[UDP][ICMPv6] unkown nextheader= 0x%x",nextheader);
    	  addToTrace(strTmp);
      }
  }
}


void evaluateNeighborSolicitation(udp* const me) {
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;

  uint16_t checksum;

  uint8_t NeighborsMac[6];//SourceMac[6];
  uint8_t NeighborsIp[16];//SourceIp[16];
  uint8_t TargetMac[6];//destinationMac[6]
  uint8_t TargetIp[16];//destinationIp[16]

  const uint8_t ICMP_LEN = 32; /* bytes in the ICMPv6 */

  addToTrace("[UDP] evaluateNeighborSolicitation");
  /* save the requesters IP. The requesters IP is the source IP on IPv6 level, at byte 22. */
  my_memcpy(NeighborsIp, &RxBuffer[22], 16);
  /* save the requesters MAC. The requesters MAC is the source MAC on Eth level, at byte 6. */
  my_memcpy(NeighborsMac, &RxBuffer[6], 6);	//soureMAC

  my_memcpy(TargetIp, &RxBuffer[38], 16);
  my_memcpy(TargetMac, &RxBuffer[0], 6);
  //destination MAC &myethreceivebuffer[0]
  //destination IP  &myethreceivebuffer[38]

  //check ip and mac address
  //McuXFormat_xsprintf(strTmp,"[UDP] NeighborsMac = %x,%x,%x,%x,%x,%x ",NeighborsMac[0],NeighborsMac[1],NeighborsMac[2],NeighborsMac[3],NeighborsMac[4],NeighborsMac[5]);
  //addToTrace(strTmp);
  //McuXFormat_xsprintf(strTmp,"[UDP] NeighborsIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ",NeighborsIp[0],NeighborsIp[1],NeighborsIp[2],NeighborsIp[3],NeighborsIp[4],NeighborsIp[5],NeighborsIp[6],NeighborsIp[7],NeighborsIp[8],NeighborsIp[9],NeighborsIp[10],NeighborsIp[11],NeighborsIp[12],NeighborsIp[13],NeighborsIp[14],NeighborsIp[15]);
  //addToTrace(strTmp);
  //McuXFormat_xsprintf(strTmp,"[UDP] TargetMac = %x,%x,%x,%x,%x,%x ",TargetMac[0],TargetMac[1],TargetMac[2],TargetMac[3],TargetMac[4],TargetMac[5]);
  //addToTrace(strTmp);
  //McuXFormat_xsprintf(strTmp,"[UDP] TargetIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ",TargetIp[0],TargetIp[1],TargetIp[2],TargetIp[3],TargetIp[4],TargetIp[5],TargetIp[6],TargetIp[7],TargetIp[8],TargetIp[9],TargetIp[10],TargetIp[11],TargetIp[12],TargetIp[13],TargetIp[14],TargetIp[15]);
  //addToTrace(strTmp);
  //McuXFormat_xsprintf(strTmp,"[UDP] myMAC = %x,%x,%x,%x,%x,%x ",myMAC[0],myMAC[1],myMAC[2],myMAC[3],myMAC[4],myMAC[5]);
  //addToTrace(strTmp);
  //McuXFormat_xsprintf(strTmp,"[UDP] SeccIp = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ",SeccIp[0],SeccIp[1],SeccIp[2],SeccIp[3],SeccIp[4],SeccIp[5],SeccIp[6],SeccIp[7],SeccIp[8],SeccIp[9],SeccIp[10],SeccIp[11],SeccIp[12],SeccIp[13],SeccIp[14],SeccIp[15]);
  //addToTrace(strTmp);

  if(0 != memcmp( NeighborsMac, me->pevMac, 6)){
	  addToTrace("[UDP] incorrect NeighborsMac");
  }
  if(0 != memcmp( NeighborsIp, EvccIp, 16)){
  	  addToTrace("[UDP] incorrect NeighborsIp");
    }
  if(0 != memcmp( TargetMac, myMAC, 6)){
	  addToTrace("[UDP] incorrect TargetMac");
  }
  if(0 != memcmp( TargetIp, SeccIp, 16)){
	  addToTrace("[UDP] incorrect TargetIp");
  }


  /* send a NeighborAdvertisement as response. */
  // destination MAC = neighbors MAC
  my_memcpy(&TxBuffer[0],me->pevMac,6);	// bytes 0 to 5 are the destination MAC
  // source MAC = my MAC
  my_memcpy(&TxBuffer[6],myMAC,6);  	// bytes 6 to 11 are the source MAC
  // Ethertype 86DD
  TxBuffer[12] = 0x86; // # 86dd is IPv6
  TxBuffer[13] = 0xdd;
  TxBuffer[14] = 0x60; // # traffic class, flow
  TxBuffer[15] = 0;
  TxBuffer[16] = 0;
  TxBuffer[17] = 0;
  // plen

  TxBuffer[18] = 0;
  TxBuffer[19] = ICMP_LEN;
  TxBuffer[20] = NEXT_ICMPv6;
  TxBuffer[21] = 0xff;
  // We are the PEV. So the EvccIp is our own link-local IP address.
  my_memcpy(&TxBuffer[22], SeccIp, 16);	// source IP address
  my_memcpy(&TxBuffer[38], NeighborsIp, 16);	// destination IP address c
  /* here starts the ICMPv6 */
  TxBuffer[54] = 0x88; /* Neighbor Advertisement */
  TxBuffer[55] = 0;
  TxBuffer[56] = 0; /* checksum (filled later) */
  TxBuffer[57] = 0;

  /* Flags */
  TxBuffer[58] = 0x60; /* Solicited, override */	//can change
  TxBuffer[59] = 0;
  TxBuffer[60] = 0;
  TxBuffer[61] = 0;

  my_memcpy(&TxBuffer[62], SeccIp, 16); /* The own IP address */
  TxBuffer[78] = 2; /* Type 2, Link Layer Address */
  TxBuffer[79] = 1; /* Length 1, means 8 byte */
  my_memcpy(&TxBuffer[80], myMAC, 6); /* The own Link Layer (MAC) address */
  checksum = calculateUdpAndTcpChecksumForIPv6(&TxBuffer[54], ICMP_LEN, SeccIp, NeighborsIp, NEXT_ICMPv6);
  TxBuffer[56] = checksum >> 8;
  TxBuffer[57] = checksum & 0xFF;
  me->QCA->ETH.TxSize = 86; /* Length of the NeighborAdvertisement */
  addToTrace("[UDP] transmitting Neighbor Advertisement");
  QCA7000_EthTransmit(me->QCA);
}


void evaluateRouterSolicitation(udp* const me) {
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
  uint16_t checksum;

  uint8_t NeighborsMac[6];//SourceMac[6];
  uint8_t NeighborsIp[16];//SourceIp[16];
  uint8_t TargetMac[6];//destinationMac[6]
  uint8_t TargetIp[16];//destinationIp[16]

  const uint8_t  ICMP_LEN=72; /* bytes in the ICMPv6 */

  addToTrace("[UDP] evaluateRouterSolicitation");
  /* save the requesters IP. The requesters IP is the source IP on IPv6 level, at byte 22. */
  my_memcpy(NeighborsIp, &RxBuffer[22], 16);
  /* save the requesters MAC. The requesters MAC is the source MAC on Eth level, at byte 6. */
  my_memcpy(NeighborsMac, &RxBuffer[6], 6);	//soureMAC

  my_memcpy(TargetIp, &RxBuffer[38], 16);
  my_memcpy(TargetMac, &RxBuffer[0], 6);

  if(0 != memcmp( NeighborsMac, me->pevMac, 6)){
	  addToTrace("[UDP] incorrect NeighborsMac");
  }
  if(0 != memcmp( NeighborsIp, EvccIp, 16)){
  	  addToTrace("[UDP] incorrect NeighborsIp");
    }
  if(0 != memcmp( TargetMac, myMAC, 6)){
	  addToTrace("[UDP] incorrect TargetMac");
  }
  if(0 != memcmp( TargetIp, SeccIp, 16)){
	  addToTrace("[UDP] incorrect TargetIp");
  }

  //************************************************************************
  /* send a Router Advertisement as response. */
  // destination MAC = neighbors MAC
  my_memcpy(&TxBuffer[0],me->pevMac,6);	// bytes 0 to 5 are the destination MAC //or multicast
  // source MAC = my MAC
  my_memcpy(&TxBuffer[6],myMAC,6);  	// bytes 6 to 11 are the source MAC
  // Ethertype 86DD
  TxBuffer[12] = 0x86; // # 86dd is IPv6
  TxBuffer[13] = 0xdd;
  TxBuffer[14] = 0x60; // # traffic class, flow
  TxBuffer[15] = 0;
  TxBuffer[16] = 0;
  TxBuffer[17] = 0;
  // plen

  TxBuffer[18] = 0;
  TxBuffer[19] = ICMP_LEN;//72 TODO PAYLOAD LEN THIS NEED TO CHANGHE
  TxBuffer[20] = NEXT_ICMPv6;	//NEXT HEADER
  TxBuffer[21] = 0xff;				//HOP LIMIT
  // We are the PEV. So the EvccIp is our own link-local IP address.
  my_memcpy(&TxBuffer[22], SeccIp, 16);	// source IP address
  my_memcpy(&TxBuffer[38], NeighborsIp, 16);	// destination IP address or multicast
  //Internet Control Message Protocol v6
  TxBuffer[54] = 0x86; /* Neighbor Advertisement *///NET_ICMPV6_RA        (0x86)   134	/* Router Advertisement */
  TxBuffer[55] = 0;		//code
  TxBuffer[56] = 0; /* checksum (filled later) */
  TxBuffer[57] = 0;
  TxBuffer[58] = 0x40; 	//CUR HOP LIMIT 64
  TxBuffer[59] = 0x00;	//flags
  TxBuffer[60] = 0x02;	//router lifetime
  TxBuffer[61] = 0x58;	//router lifetime
  TxBuffer[62] = 0;		//Reachable time
  TxBuffer[63] = 0;		//Reachable time
  TxBuffer[64] = 0x75;	//Reachable time
  TxBuffer[65] = 0x30;	//Reachable time
  TxBuffer[66] = 0;		//Retrans time
  TxBuffer[67] = 0;		//Retrans time
  TxBuffer[68] = 0x03;	//Retrans time
  TxBuffer[69] = 0xe8;	//Retrans time
  //ICMPv6 option (Source link-layer address)
  TxBuffer[70] = 0x01;// type: (1)Source link-layer address, 2target link
  TxBuffer[71] = 0x01;// length: 1(8bytes)
  my_memcpy(&TxBuffer[72], myMAC, 6); /* The own IP address */
  //ICMPv6 option (Prefix information)
  TxBuffer[78] = 0x03;		// Type: prefix information (3) (Every RA should have this field)
  TxBuffer[79] = 0x04;		//Length: 4 (32bytes)
  TxBuffer[80] = 0x40;		//Prefix Length: 64
  TxBuffer[81] = 0x0c;		//flag, on-link flag(L), Autonomous address-configuration flag(A)
  TxBuffer[82] = 0;			//valid Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[83] = 0;			//valid Lifetime
  TxBuffer[84] = 0xff;		//valid Lifetime
  TxBuffer[85] = 0xff;		//valid Lifetime

  TxBuffer[86] = 0;			//Preferred Lifetime Preferred Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[87] = 0;			//Preferred Lifetime
  TxBuffer[88] = 0xff;		//Preferred Lifetime
  TxBuffer[89] = 0xff;		//Preferred Lifetime
  TxBuffer[90] = 0;			//Reserved
  TxBuffer[91] = 0;			//Reserved
  TxBuffer[92] = 0;			//Reserved
  TxBuffer[93] = 0;			//Reserved

  uint8_t Prefix16[16] = {0x20,0x01,0,0x02,0,0,0x01,0,0,0,0,0,0,0,0,0};//2001:2:0:1000::
  my_memcpy(&TxBuffer[94], Prefix16, 16);

  //ICMPv6 Option (Route Information : Low 2001:2:0:2000::/64)
  TxBuffer[110] = 0x18;		//Type: Route Information (24)
  TxBuffer[111] = 0x02;		//Length: 2 (16 bytes)
  TxBuffer[112] = 0x40;		//Prefix Length: 64
  TxBuffer[113] = 0x18;		//Flag: 0x18, Route Preference: Low
  TxBuffer[114] = 0;			//Route Lifetime: 30 (30 seconds)
  TxBuffer[115] = 0;			//Route Lifetime
  TxBuffer[116] = 0;			//Route Lifetime
  TxBuffer[117] = 0x1e;		//Route Lifetime

  uint8_t Prefix[8] = {0x20,0x01,0,0x02,0,0,0x20,0};	//2001:2:0:2000::
  my_memcpy(&TxBuffer[118], Prefix, 8);		//

  //minimum with option 1(Source link-layer address), 3(prefix information),24(Route Information) only
  //total length 126

  checksum = calculateUdpAndTcpChecksumForIPv6(&TxBuffer[54], ICMP_LEN, SeccIp, NeighborsIp, NEXT_ICMPv6);
  TxBuffer[56] = u8(checksum+1);
  TxBuffer[57] = u8(checksum);
  me->QCA->ETH.TxSize = 126; /* Length of the NeighborAdvertisement */
  addToTrace("[UDP] transmitting Router Advertisement");
  QCA7000_EthTransmit(me->QCA);
}


void evaluateRouterSolicitation2(udp* const me) {
  uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
  uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
  uint16_t checksum;

  uint8_t NeighborsMac[6];//SourceMac[6];
  uint8_t NeighborsIp[16];//SourceIp[16];
  uint8_t TargetMac[6];//destinationMac[6]
  uint8_t TargetIp[16];//destinationIp[16]

  uint8_t ICMP_LEN = 64; /* bytes in the ICMPv6 */

  addToTrace("[UDP] evaluateRouterSolicitation2");
  /* save the requesters IP. The requesters IP is the source IP on IPv6 level, at byte 22. */
  my_memcpy(NeighborsIp, &RxBuffer[22], 16);
  my_memcpy(TargetIp, &RxBuffer[38], 16);

  /* save the requesters MAC. The requesters MAC is the source MAC on Eth level, at byte 6. */
  my_memcpy(NeighborsMac, &RxBuffer[6], 6);	//soureMAC
  my_memcpy(TargetMac, &RxBuffer[0], 6);
  //destination MAC &myethreceivebuffer[0]
  //destination IP  &myethreceivebuffer[38]



  //showAsHex(myethreceivebuffer, myethreceivebufferLen, "[UDP]myethreceivebuffer");

  if(0 != memcmp( NeighborsMac, me->pevMac, 6)){
	  addToTrace("[UDP] incorrect NeighborsMac");
  }
  if(0 != memcmp( NeighborsIp, EvccIp, 16)){
  	  addToTrace("[UDP] incorrect NeighborsIp");
    }
  if(0 != memcmp( TargetMac, myMAC, 6)){
	  addToTrace("[UDP] incorrect TargetMac");
  }
  if(0 != memcmp( TargetIp, SeccIp, 16)){
	  addToTrace("[UDP] incorrect TargetIp");
  }

  //************************************************************************
  /* send a RouterAdvertisement as response. */
  // destination MAC = neighbors MAC
  my_memcpy(&TxBuffer[0],me->pevMac,6);	// bytes 0 to 5 are the destination MAC
  // source MAC = my MAC
  my_memcpy(&TxBuffer[6],myMAC,6);  	// bytes 6 to 11 are the source MAC
  // Ethertype 86DD
  TxBuffer[12] = 0x86; // # 86dd is IPv6
  TxBuffer[13] = 0xdd;
  TxBuffer[14] = 0x60; // # traffic class, flow
  TxBuffer[15] = 0;
  TxBuffer[16] = 0;
  TxBuffer[17] = 0;
  // plen

  TxBuffer[18] = 0;
  TxBuffer[19] = ICMP_LEN;//72 TODO PAYLOAD LEN THIS NEED TO CHANGHE
  TxBuffer[20] = NEXT_ICMPv6;	//NEXT HEADER
  TxBuffer[21] = 0xff;				//HOP LIMIT
  // We are the PEV. So the EvccIp is our own link-local IP address.
  my_memcpy(&TxBuffer[22], SeccIp, 16);	// source IP address
  my_memcpy(&TxBuffer[38], NeighborsIp, 16);	// destination IP address c
  /* here starts the ICMPv6 */
  TxBuffer[54] = 0x86; /* Neighbor Advertisement *///NET_ICMPV6_RA        (0x86)   134	/* Router Advertisement */
  TxBuffer[55] = 0;		//code
  TxBuffer[56] = 0; /* checksum (filled later) */
  TxBuffer[57] = 0;

  /* Flags */
  TxBuffer[58] = 0x40; 	//CUR HOP LIMIT 64
  TxBuffer[59] = 0x00;	//flags
  TxBuffer[60] = 0x02;	//router lifetime
  TxBuffer[61] = 0x58;	//router lifetime
  TxBuffer[62] = 0;		//Reachable time
  TxBuffer[63] = 0;		//Reachable time
  TxBuffer[64] = 0;		//Reachable time
  TxBuffer[65] = 0;		//Reachable time
  TxBuffer[66] = 0;		//Retrans time
  TxBuffer[67] = 0;		//Retrans time
  TxBuffer[68] = 0;		//Retrans time
  TxBuffer[69] = 0;		//Retrans time
  //ICMPv6 option (Prefix information)
  TxBuffer[70] = 0x03;		// Type: prefix information (3)
  TxBuffer[71] = 0x04;		//Length: 4 (32bytes)
  TxBuffer[72] = 0x40;		//Prefix Length: 64
  TxBuffer[73] = 0x80;		//flag, on-link flag(L), Autonomous address-configuration flag(A)
  TxBuffer[74] = 0;			//valid Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[75] = 0;			//valid Lifetime
  TxBuffer[76] = 0;			//valid Lifetime
  TxBuffer[77] = 0;			//valid Lifetime

  TxBuffer[78] = 0;			//Preferred Lifetime Preferred Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[79] = 0;			//Preferred Lifetime
  TxBuffer[80] = 0;			//Preferred Lifetime
  TxBuffer[81] = 0;			//Preferred Lifetime
  TxBuffer[82] = 0;			//Reserved
  TxBuffer[83] = 0;			//Reserved
  TxBuffer[84] = 0;			//Reserved
  TxBuffer[85] = 0;			//Reserved
  uint8_t Prefix16[16] = {0x20,0x01,0,0x02,0,0,0x01,0,0,0,0,0,0,0,0,0};//2001:2:0:1000::
  my_memcpy(&TxBuffer[86], Prefix16, 16);
  //ICMPv6 Option (Route Information : Low 2001:2:0:2000::/64)
  TxBuffer[102] = 0x18;		//Type: Route Information (24)
  TxBuffer[103] = 0x02;		//Length: 2 (16 bytes)
  TxBuffer[104] = 0x40;		//Prefix Length: 64
  TxBuffer[105] = 0x18;		//Flag: 0x18, Route Preference: Low
  TxBuffer[106] = 0;			//Route Lifetime: 30 (30 seconds)
  TxBuffer[107] = 0;			//Route Lifetime
  TxBuffer[108] = 0;			//Route Lifetime
  TxBuffer[109] = 0x00;		//Route Lifetime

  uint8_t Prefix[8] = {0x20,0x01,0,0x02,0,0,0x20,0};	//2001:2:0:2000::
  my_memcpy(&TxBuffer[110], Prefix, 8);

  //minimum with option 3(prefix information),24(Route Information) only
  //total length 118

  checksum = calculateUdpAndTcpChecksumForIPv6(&TxBuffer[54], ICMP_LEN, SeccIp, NeighborsIp, NEXT_ICMPv6);
  TxBuffer[56] = u8(checksum + 1);
  TxBuffer[57] = u8(checksum);
  me->QCA->ETH.TxSize = 118; /* Length of the NeighborAdvertisement */
  addToTrace("[UDP] transmitting Router Advertisement");
  QCA7000_EthTransmit(me->QCA);
}

void evaluateRouterSolicitation3(udp* const me) {
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;

  uint16_t checksum;
  uint8_t NeighborsMac[6];//SourceMac[6];
  uint8_t NeighborsIp[16];//SourceIp[16];

  uint8_t TargetMac[6];//destinationMac[6]
  uint8_t TargetIp[16];//destinationIp[16]

  uint8_t ICMP_LEN = 48; /* bytes in the ICMPv6 */
  addToTrace("[UDP] evaluateRouterSolicitation3");
  /* save the requesters IP. The requesters IP is the source IP on IPv6 level, at byte 22. */
  my_memcpy(NeighborsIp, &RxBuffer[22], 16);
  /* save the requesters MAC. The requesters MAC is the source MAC on Eth level, at byte 6. */
  my_memcpy(NeighborsMac, &RxBuffer[6], 6);	//soureMAC

  my_memcpy(TargetIp, &RxBuffer[38], 16);
  my_memcpy(TargetMac, &RxBuffer[0], 6);

  if(0 != memcmp( NeighborsMac, me->pevMac, 6)){
	  addToTrace("[UDP] incorrect NeighborsMac");
  }
  if(0 != memcmp( NeighborsIp, EvccIp, 16)){
  	  addToTrace("[UDP] incorrect NeighborsIp");
    }
  if(0 != memcmp( TargetMac, myMAC, 6)){
	  addToTrace("[UDP] incorrect TargetMac");
  }
  if(0 != memcmp( TargetIp, SeccIp, 16)){
	  addToTrace("[UDP] incorrect TargetIp");
  }

  //************************************************************************
  /* send a RouterAdvertisement as response. */
  // destination MAC = neighbors MAC
  my_memcpy(&TxBuffer[0],NeighborsMac,6);	// bytes 0 to 5 are the destination MAC
  // source MAC = my MAC
  my_memcpy(&TxBuffer[6],myMAC,6);  	// bytes 6 to 11 are the source MAC
  // Ethertype 86DD
  TxBuffer[12] = 0x86; // # 86dd is IPv6
  TxBuffer[13] = 0xdd;
  TxBuffer[14] = 0x60; // # traffic class, flow
  TxBuffer[15] = 0;
  TxBuffer[16] = 0;
  TxBuffer[17] = 0;
  // plen

  TxBuffer[18] = 0;
  TxBuffer[19] = ICMP_LEN;//72 TODO PAYLOAD LEN THIS NEED TO CHANGHE
  TxBuffer[20] = NEXT_ICMPv6;	//NEXT HEADER
  TxBuffer[21] = 0xff;				//HOP LIMIT
  // We are the PEV. So the EvccIp is our own link-local IP address.
  my_memcpy(&TxBuffer[22], SeccIp, 16);	// source IP address
  my_memcpy(&TxBuffer[38], NeighborsIp, 16);	// destination IP address c
  /* here starts the ICMPv6 */
  TxBuffer[54] = 0x86; /* Neighbor Advertisement *///NET_ICMPV6_RA        (0x86)   134	/* Router Advertisement */
  TxBuffer[55] = 0;		//code
  TxBuffer[56] = 0; /* checksum (filled later) */
  TxBuffer[57] = 0;

  /* Flags */
  TxBuffer[58] = 0x40; 	//CUR HOP LIMIT 64
  TxBuffer[59] = 0x00;	//flags
  TxBuffer[60] = 0x02;	//router lifetime
  TxBuffer[61] = 0x58;	//router lifetime
  TxBuffer[62] = 0;		//Reachable time
  TxBuffer[63] = 0;		//Reachable time
  TxBuffer[64] = 0;		//Reachable time
  TxBuffer[65] = 0;		//Reachable time
  TxBuffer[66] = 0;		//Retrans time
  TxBuffer[67] = 0;		//Retrans time
  TxBuffer[68] = 0;		//Retrans time
  TxBuffer[69] = 0;		//Retrans time
  //ICMPv6 option (Prefix information)
  TxBuffer[70] = 0x03;		// Type: prefix information (3) needed
  TxBuffer[71] = 0x04;		//Length: 4 (32bytes)
  TxBuffer[72] = 0x40;		//Prefix Length: 64
  TxBuffer[73] = 0x80;		//flag, on-link flag(L), Autonomous address-configuration flag(A)
  TxBuffer[74] = 0;			//valid Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[75] = 0;			//valid Lifetime
  TxBuffer[76] = 0;			//valid Lifetime
  TxBuffer[77] = 0;			//valid Lifetime

  TxBuffer[78] = 0;			//Preferred Lifetime Preferred Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[79] = 0;			//Preferred Lifetime
  TxBuffer[80] = 0;			//Preferred Lifetime
  TxBuffer[81] = 0;			//Preferred Lifetime
  TxBuffer[82] = 0;			//Reserved
  TxBuffer[83] = 0;			//Reserved
  TxBuffer[84] = 0;			//Reserved
  TxBuffer[85] = 0;			//Reserved
  uint8_t Prefix16[16] = {0x20,0x01,0,0x02,0,0,0x01,0,0,0,0,0,0,0,0,0};//2001:2:0:1000::
  my_memcpy(&TxBuffer[86], Prefix16, 16);

  //minimum with option 3(prefix information) only
  //total length 102

  checksum = calculateUdpAndTcpChecksumForIPv6(&TxBuffer[54], ICMP_LEN, SeccIp, NeighborsIp, NEXT_ICMPv6);
  TxBuffer[56] = u8(checksum+1);
  TxBuffer[57] = u8(checksum);
  me->QCA->ETH.TxSize = 102; /* Length of the NeighborAdvertisement */
  addToTrace("[UDP] transmitting Router Advertisement");
  QCA7000_EthTransmit(me->QCA);
}


void evaluateRouterSolicitation4(udp* const me) {
  uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
  uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
  const uint8_t multiCastMac[6]={0x33,0x33,0,0,0,0x01};		//RA use 01  RN use 02
  const uint8_t multiCastIP[16]={0xff,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01};
  uint16_t checksum;
  uint8_t NeighborsMac[6];//SourceMac[6];
  uint8_t NeighborsIp[16];//SourceIp[16];

  uint8_t TargetMac[6];//destinationMac[6]
  uint8_t TargetIp[16];//destinationIp[16]

  const uint8_t  ICMP_LEN=56; /* bytes in the ICMPv6 */

  addToTrace("[UDP] evaluateRouterSolicitation4");
  /* save the requesters IP. The requesters IP is the source IP on IPv6 level, at byte 22. */
  my_memcpy(NeighborsIp, &RxBuffer[22], 16);
  /* save the requesters MAC. The requesters MAC is the source MAC on Eth level, at byte 6. */
  my_memcpy(NeighborsMac, &RxBuffer[6], 6);	//soureMAC

  my_memcpy(TargetIp, &RxBuffer[38], 16);
  my_memcpy(TargetMac, &RxBuffer[0], 6);


  if(0 != memcmp( NeighborsMac, me->pevMac, 6)){
	  addToTrace("[UDP] incorrect NeighborsMac");
  }
  if(0 != memcmp( NeighborsIp, EvccIp, 16)){
  	  addToTrace("[UDP] incorrect NeighborsIp");
    }
  if(0 != memcmp( TargetMac, myMAC, 6)){
	  addToTrace("[UDP] incorrect TargetMac");
  }
  if(0 != memcmp( TargetIp, SeccIp, 16)){
	  addToTrace("[UDP] incorrect TargetIp");
  }

  //************************************************************************
  /* send a Router Advertisement as response. */
  // destination MAC = neighbors MAC
  my_memcpy(&TxBuffer[0],multiCastMac,6);	// bytes 0 to 5 are the destination MAC //or multicast
  // source MAC = my MAC
  my_memcpy(&TxBuffer[6],myMAC,6);  	// bytes 6 to 11 are the source MAC
  // Ethertype 86DD
  TxBuffer[12] = 0x86; // # 86dd is IPv6
  TxBuffer[13] = 0xdd;
  TxBuffer[14] = 0x60; // # traffic class, flow
  TxBuffer[15] = 0;
  TxBuffer[16] = 0;
  TxBuffer[17] = 0;
  // plen

  TxBuffer[18] = 0;
  TxBuffer[19] = ICMP_LEN;//72 TODO PAYLOAD LEN THIS NEED TO CHANGHE
  TxBuffer[20] = NEXT_ICMPv6;	//NEXT HEADER
  TxBuffer[21] = 0xff;				//HOP LIMIT
  // We are the PEV. So the EvccIp is our own link-local IP address.
  my_memcpy(&TxBuffer[22], SeccIp, 16);	// source IP address
  my_memcpy(&TxBuffer[38], multiCastIP, 16);	// destination IP address or multicast
  //Internet Control Message Protocol v6
  TxBuffer[54] = 0x86; /* Neighbor Advertisement *///NET_ICMPV6_RA        (0x86)   134	/* Router Advertisement */
  TxBuffer[55] = 0;		//code
  TxBuffer[56] = 0; /* checksum (filled later) */
  TxBuffer[57] = 0;
  TxBuffer[58] = 0x40; 	//CUR HOP LIMIT 64
  TxBuffer[59] = 0x08;	//flags changing  0x10(resevered) 0x18(low) 0x08(high)
  TxBuffer[60] = 0x75;	//router lifetime 30000s
  TxBuffer[61] = 0x30;	//router lifetime
  TxBuffer[62] = 0;		//Reachable time   30000ms
  TxBuffer[63] = 0;		//Reachable time
  TxBuffer[64] = 0x75;	//Reachable time
  TxBuffer[65] = 0x30;	//Reachable time
  TxBuffer[66] = 0;		//Retrans time		1000ms
  TxBuffer[67] = 0;		//Retrans time
  TxBuffer[68] = 0x03;	//Retrans time
  TxBuffer[69] = 0xe8;	//Retrans time
  //ICMPv6 option (Source link-layer address)
  TxBuffer[70] = 0x01;// type: (1)Source link-layer address, 2target link
  TxBuffer[71] = 0x01;// length: 1(8bytes)
  my_memcpy(&TxBuffer[72], myMAC, 6); /* The own IP address */
  //ICMPv6 option (Prefix information)
  TxBuffer[78] = 0x03;		// Type: prefix information (3) (Every RA should have this field)
  TxBuffer[79] = 0x04;		//Length: 4 (32bytes)
  TxBuffer[80] = 0x40;		//Prefix Length: 64
  TxBuffer[81] = 0x0c;		//flag, on-link flag(L), Autonomous address-configuration flag(A)
  TxBuffer[82] = 0;			//valid Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[83] = 0;			//valid Lifetime
  TxBuffer[84] = 0xff;		//valid Lifetime
  TxBuffer[85] = 0xff;		//valid Lifetime

  TxBuffer[86] = 0;			//Preferred Lifetime Preferred Lifetime: 65535 (18 hours, 12 minutes, 15 seconds)
  TxBuffer[87] = 0;			//Preferred Lifetime
  TxBuffer[88] = 0xff;		//Preferred Lifetime
  TxBuffer[89] = 0xff;		//Preferred Lifetime
  TxBuffer[90] = 0;			//Reserved
  TxBuffer[91] = 0;			//Reserved
  TxBuffer[92] = 0;			//Reserved
  TxBuffer[93] = 0;			//Reserved

  const uint8_t Prefix16[16] = {0x20,0x01,0,0x02,0,0,0x01,0,0,0,0,0,0,0,0,0};//2001:2:0:1000::
  my_memcpy(&TxBuffer[94], Prefix16, 16);


  //minimum with option 1(Source link-layer address), 3(prefix information) only
  //total length 110

  checksum = calculateUdpAndTcpChecksumForIPv6(&TxBuffer[54], ICMP_LEN, SeccIp, NeighborsIp, NEXT_ICMPv6);
  TxBuffer[56] = u8(checksum +1);
  TxBuffer[57] = u8(checksum);
  me->QCA->ETH.TxSize = 110; /* Length of the NeighborAdvertisement */
  addToTrace("[UDP] transmitting Router Advertisement");
  QCA7000_EthTransmit(me->QCA);
}



//*****************************************
//new function but got bug








