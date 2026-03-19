/* Homeplug message handling */

#include "ccs32_globals.h"

#include "homeplug.h"

#include <time.h>
#include <stdlib.h>

#define CM_SET_KEY  0x6008
#define CM_GET_KEY  0x600C
#define CM_SC_JOIN  0x6010
#define CM_CHAN_EST  0x6014
#define CM_TM_UPDATE  0x6018
#define CM_AMP_MAP  0x601C
#define CM_BRG_INFO  0x6020
#define CM_CONN_NEW  0x6024
#define CM_CONN_REL  0x6028
#define CM_CONN_MOD  0x602C
#define CM_CONN_INFO  0x6030
#define CM_STA_CAP  0x6034
#define CM_NW_INFO  0x6038
#define CM_GET_BEACON  0x603C
#define CM_HFID  0x6040
#define CM_MME_ERROR  0x6044
#define CM_NW_STATS  0x6048
#define CM_SLAC_PARAM  0x6064
#define CM_START_ATTEN_CHAR  0x6068
#define CM_ATTEN_CHAR  0x606C
#define CM_PKCS_CERT  0x6070
#define CM_MNBC_SOUND  0x6074
#define CM_VALIDATE  0x6078
#define CM_SLAC_MATCH  0x607C
#define CM_SLAC_USER_DATA  0x6080
#define CM_ATTEN_PROFILE  0x6084
#define CM_GET_SW  0xA000

#define CM_HOST_ACTION_IND 0xA062
#define CM_HOST_ACTION 0xA060

#define MMTYPE_REQ  0x0000
#define MMTYPE_CNF  0x0001
#define MMTYPE_IND  0x0002
#define MMTYPE_RSP  0x0003

/*
#define STATE_INITIAL  0
#define STATE_MODEM_SEARCH_ONGOING  1
#define STATE_READY_FOR_SLAC        2
#define STATE_WAITING_FOR_MODEM_RESTARTED  3
#define STATE_WAITING_FOR_SLAC_PARAM_CNF   4
#define STATE_SLAC_PARAM_CNF_RECEIVED      5
#define STATE_BEFORE_START_ATTEN_CHAR      6
#define STATE_SOUNDING                     7
#define STATE_WAIT_FOR_ATTEN_CHAR_IND      8
#define STATE_ATTEN_CHAR_IND_RECEIVED      9
#define STATE_DELAY_BEFORE_MATCH           10
#define STATE_WAITING_FOR_SLAC_MATCH_CNF   11
#define STATE_WAITING_FOR_RESTART2         12
#define STATE_FIND_MODEMS2                 13
#define STATE_WAITING_FOR_SW_VERSIONS      14
#define STATE_READY_FOR_SDP                15
#define STATE_SDP                          16

*/

//#define iAmPev 1 /* This project is intended only for PEV mode at the moment. */
//#define iAmEvse 1




extern const uint8_t myMAC[6];		//myMAC act  evseMAC
const uint8_t MAC_BROADCAST[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const uint8_t NID[7]={ 0x3F, 0x5B, 0x4F, 0xDC, 0xC4, 0x3D, 0x05 };			//Network ID (NID) IND[6] MSB 2 bits 0b00 = Simple Connect

///uint8_t localModemMac[6];			//EVSE dont care MODEM MAC
//uint8_t evseMac[6];					//for PEV mode only
//3F5B4FDC4D3D05//{ 1, 2, 3, 4, 5, 6, 7 };
#ifdef iAmEvse
	uint8_t NMK_EVSE_random2[16] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77 } ;//new added for EVSE  # In EvseMode, we use this key.
	uint8_t NMK_EVSE_random[16] = { 0xB5, 0x93, 0x19, 0xD7, 0xE8, 0x15, 0x7B, 0xA0, 0x01, 0xB0, 0x18, 0x66, 0x9C, 0xCE, 0xE3, 0x0D};	//from document

#else
	uint8_t NMK[16];			//Network Membership Key (NMK)
#endif

//uint8_t localModemCurrentKey[16];
//uint8_t localModemFound;
//uint8_t numberOfSoftwareVersionResponses;
//uint8_t numberOfFoundModems;

//uint32_t randomMac = 0;		//evse added
//uint8_t evseSlacHandlerState = 0;		//evse added
//uint8_t pevSequenceState = 0;
//uint16_t pevSequenceCyclesInState = 0;
//uint16_t pevSequenceDelayCycles;
//uint8_t nRemainingStartAttenChar;
//uint8_t remainingNumberOfSounds;
//uint8_t AttenCharIndNumberOfSounds;
//uint8_t SdpRepetitionCounter;
//uint8_t isSDPDone;
//uint8_t sdp_state;
//uint8_t nEvseModemMissingCounter;
//uint16_t checkpointNumber;

/********** local prototypes *****************************************/
//void composeAttenCharRsp(void);			//PEV mode function
//void slac_enterState(int n);			//not sure why put here, PEV function,python enterState
void homeplug_composeSetKey(homeplug* const me, uint8_t variation);		//modfidied to vairable(EVSE), original variable =0;
void homeplug_fillDestinationMac(homeplug* const me,const uint8_t *mac, uint8_t offset);
void homeplug_fillSourceMac(homeplug* const me, const uint8_t *mac, uint8_t offset);
void homeplug_fillRunId(homeplug* const me, uint8_t offset);

void homeplug_evaluateReceivedHomeplugPacket(homeplug* const me);
void homeplug_runSlacSequencer(homeplug* const me);
void homeplug_runSdpStateMachine(homeplug* const me);

void homeplug_evaluateMnbcSoundInd(homeplug* const me);			//evse added
void homeplug_evaluateSlacParamReq(homeplug* const me);

void homeplug_composeSlacMatchCnf(homeplug* const me);		//evse added
void homeplug_runEvseSlacHandler(homeplug* const me);		//evse added

/*********************************************************************************/
/*

*/
/* Extracting the EtherType from a received message. */
/*
void fillSourceMac(const uint8_t *mac, uint8_t offset) {
 // at offset 6 in the ethernet frame, we have the source MAC.
 //   we can give a different offset, to re-use the MAC also in the data area
  my_memcpy(&myethtransmitbuffer[offset], mac, 6);
}

void fillDestinationMac(const uint8_t *mac, uint8_t offset) {
 // at offset 0 in the ethernet frame, we have the destination MAC.
 //   we can give a different offset, to re-use the MAC also in the data area
  my_memcpy(&myethtransmitbuffer[offset], mac, 6);
}
*/

void homeplug_ctor(homeplug* const me,QCA7000* QCA){
	me->QCA = QCA;
	me->NMK_condition = 0;
	me->numberOfFoundModems = 0;
	me->randomMac = 0;
}

void homeplug_reset(homeplug* const me){
	me->NMK_condition = 0;
	me->numberOfFoundModems = 0;
	me->randomMac = 0;
}

void homeplug_fillRunId(homeplug* const me, uint8_t offset){		//evse added
    // at the given offset in the transmit buffer, fill the 8-bytes-RunId.
	my_memcpy(&me->QCA->ETH.TxBuffer[offset],me->pevRunId,8);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] Sending pevRunId = %x,%x,%x,%x,%x,%x,%x,%x" ,me->pevRunId[0],me->pevRunId[1],me->pevRunId[2],me->pevRunId[3],me->pevRunId[4],me->pevRunId[5],me->pevRunId[6],me->pevRunId[7]);

	//int McuXFormat_xsprintf(char *buf, const char *fmt, ...)
	addToTrace(strTmp);
}


void homeplug_cleanTxBuffer(homeplug* const me) {
  /* fill the complete ethernet transmit buffer with 0x00 */
  memset(&me->QCA->ETH.TxBuffer, 0, ETH_TX_RX_BUFFER_MAX);
}

void homeplug_setNmkAt(homeplug* const me, uint8_t index) {
  /* sets the Network Membership Key (NMK) at a certain position in the transmit buffer */
#ifdef iAmEvse
	  my_memcpy(&me->QCA->ETH.TxBuffer[index], NMK_EVSE_random, 16);
#else
	  my_memcpy(&me->QCA->ETH.TxBuffer[index], NMK, 16);
#endif
}


void homeplug_setNidAt(homeplug* const me, uint8_t index) {
  /* copies the network ID (NID, 7 bytes) into the wished position in the transmit buffer */
  my_memcpy(&me->QCA->ETH.TxBuffer[index], NID, 7);
}

uint16_t homeplug_getManagementMessageType(homeplug* const me) {
  /* calculates the MMTYPE (base value + lower two bits), see Table 11-2 of homeplug spec */
  return m16(me->QCA->ETH.RxBuffer[16] , me->QCA->ETH.RxBuffer[15]);
}
/*
void composeGetSwReq(void) {			//not using
	// GET_SW.REQ request, as used by the win10 laptop
    myethtransmitbufferLen = 60;
    cleanTransmitBuffer();
    // Destination MAC
    my_memcpy(&myethtransmitbuffer[0], MAC_BROADCAST, 6);
    // Source MAC
    my_memcpy(&myethtransmitbuffer[6], myMAC, 6);
    // Protocol
    myethtransmitbuffer[12]=0x88; // Protocol HomeplugAV
    myethtransmitbuffer[13]=0xE1; //
    myethtransmitbuffer[14]=0x00; // version
    myethtransmitbuffer[15]=0x00; // GET_SW.REQ
    myethtransmitbuffer[16]=0xA0; // 
    myethtransmitbuffer[17]=0x00; // Vendor OUI
    myethtransmitbuffer[18]=0xB0; // 
    myethtransmitbuffer[19]=0x52; //  
}
*/



void homeplug_composeGetSwWithRamdomMac(homeplug* const me){			//evse added	not using
	// GET_SW.REQ request, as used by the win10 laptop
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 60;


    homeplug_cleanTxBuffer(me);
    my_memcpy(&TxBuffer[0], MAC_BROADCAST, 6);	// Destination MAC
    my_memcpy(&TxBuffer[6], myMAC, 6);			// Source MAC
    // patch the lower three bytes of the MAC with a random value
    TxBuffer[8] =  me->randomMac 	  & 0xff;
    TxBuffer[9] =  (me->randomMac>>16) & 0xff;
    TxBuffer[10] = (me->randomMac>>8)  & 0xff;
    TxBuffer[11] =  me->randomMac      & 0xff;
	if ((me->randomMac%16)==0){
		uint8_t tempMAC[6] = {0xb8, 0x27, 0xeb, 0xa3, 0xaf, 0x34};
		my_memcpy(&TxBuffer[6], tempMAC, 6);	// Source MAC
	}
	else if ((me->randomMac%16)==1){
		uint8_t tempMAC[6] = {0xb8, 0x27, 0xeb, 0x72, 0x66, 0x06};
		my_memcpy(&TxBuffer[6], tempMAC, 6);	// Source MAC
	}
	me->randomMac += 1; // new MAC for the next round
    // Protocol
    TxBuffer[12]=0x88; // Protocol HomeplugAV
    TxBuffer[13]=0xE1;
    TxBuffer[14]=0x00; // version
    TxBuffer[15]=0x00; // GET_SW.REQ
    TxBuffer[16]=0xA0; //
    TxBuffer[17]=0x00; // Vendor OUI
    TxBuffer[18]=0xB0; //
    TxBuffer[19]=0x52; //
}

/*
void composeSlacParamReq(void) {
	// SLAC_PARAM request, as it was recorded 2021-12-17 WP charger 2
    myethtransmitbufferLen = 60;
    cleanTransmitBuffer();
    // Destination MAC
    fillDestinationMac(MAC_BROADCAST, 0);
    // Source MAC
    fillSourceMac(myMAC, 6);
    // Protocol
    myethtransmitbuffer[12]=0x88; // Protocol HomeplugAV
    myethtransmitbuffer[13]=0xE1; //
    myethtransmitbuffer[14]=0x01; // version
    myethtransmitbuffer[15]=0x64; // SLAC_PARAM.REQ
    myethtransmitbuffer[16]=0x60; // 
    myethtransmitbuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
    myethtransmitbuffer[18]=0x00; // 
    myethtransmitbuffer[19]=0x00; // 
    myethtransmitbuffer[20]=0x00; //
    fillSourceMac(myMAC, 21); // 21 to 28: 8 bytes runid. The Ioniq uses the PEV mac plus 00 00.
    myethtransmitbuffer[27]=0x00; // 
    myethtransmitbuffer[28]=0x00; // 
    // rest is 00
}
*/

void homeplug_composeSlacParamCnf(homeplug* const me){			//evse mode return data when recieve PEV mac
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 60;

	homeplug_cleanTxBuffer(me);
	my_memcpy(&TxBuffer[0], me->pevMac, 6);		    // Destination MAC
	my_memcpy(&TxBuffer[6], myMAC, 6);    		// Source MAC
    // Protocol
	TxBuffer[12]=0x88; // Protocol HomeplugAV
	TxBuffer[13]=0xE1;
	TxBuffer[14]=0x01; // version
	TxBuffer[15]=0x65; // SLAC_PARAM.confirm
	TxBuffer[16]=0x60; //
	TxBuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
	TxBuffer[18]=0x00; //
	TxBuffer[19]=0xff; // 19-24 sound target
	TxBuffer[20]=0xff; //
	TxBuffer[21]=0xff; //
	TxBuffer[22]=0xff; //
	TxBuffer[23]=0xff; //
	TxBuffer[24]=0xff; //
	TxBuffer[25]=0x0A; // sound count
    TxBuffer[26]=0x06; // timeout
    TxBuffer[27]=0x01; // resptype
	my_memcpy(&TxBuffer[28], me->pevMac, 6);    // forwarding_sta, same as PEV MAC, plus 2 bytes 00 00
	TxBuffer[34]=0x00; //
	TxBuffer[35]=0x00; //
    homeplug_fillRunId(me,36);  // 36 to 43 runid 8 bytes
    // rest is 00
}



void homeplug_composeStartAttenCharInd(homeplug* const me) {			//PEV mode function
    /* reference: see wireshark interpreted frame from ioniq */
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 60;
    homeplug_cleanTxBuffer(me);
    // Destination MAC
	my_memcpy(&TxBuffer[0], MAC_BROADCAST, 6);
    // Source MAC
	my_memcpy(&TxBuffer[6], myMAC, 6);
    // Protocol
	TxBuffer[12]=0x88; // Protocol HomeplugAV
	TxBuffer[13]=0xE1; //
	TxBuffer[14]=0x01; // version
	TxBuffer[15]=0x6A; // START_ATTEN_CHAR.IND
	TxBuffer[16]=0x60; //
	TxBuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
	TxBuffer[18]=0x00; //
	TxBuffer[19]=0x00; // apptype
	TxBuffer[20]=0x00; // sectype
	TxBuffer[21]=0x0a; // number of sounds: 10
	TxBuffer[22]=6; // timeout N*100ms. Normally 6, means in 600ms all sounds must have been tranmitted.
                            // Todo: As long we are a little bit slow, lets give 1000ms instead of 600, so that the
                            // charger is able to catch it all.
	TxBuffer[23]=0x01; // response type
    my_memcpy(&TxBuffer[24],myMAC,6);	// 24 to 29: sound_forwarding_sta, MAC of the PEV
    homeplug_fillRunId(me,30);   // 30 to 37: runid, filled with MAC of PEV and two bytes 00 00
    // rest is 00
}
/*
void composeNmbcSoundInd(void) {
    // reference: see wireshark interpreted frame from Ioniq
    myethtransmitbufferLen = 71;
    homeplug_cleanTransmitBuffer();
    //Destination MAC
    fillDestinationMac(MAC_BROADCAST, 0);
    // Source MAC
    fillSourceMac(myMAC, 6);
    // Protocol
    myethtransmitbuffer[12]=0x88; // Protocol HomeplugAV
    myethtransmitbuffer[13]=0xE1; //
    myethtransmitbuffer[14]=0x01; // version
    myethtransmitbuffer[15]=0x76; // NMBC_SOUND.IND
    myethtransmitbuffer[16]=0x60; //
    myethtransmitbuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
    myethtransmitbuffer[18]=0x00; //
    myethtransmitbuffer[19]=0x00; // apptype
    myethtransmitbuffer[20]=0x00; // sectype
    myethtransmitbuffer[21]=0x00; // 21 to 37 sender ID, all 00
    myethtransmitbuffer[38]=remainingNumberOfSounds; // countdown. Remaining number of sounds. Starts with 9 and counts down to 0.
    homeplug_fillRunId(39); // 39 to 46: runid, filled with MAC of PEV and two bytes 00 00
    myethtransmitbuffer[47]=0x00; // 47 to 54: reserved, all 00
    //55 to 70: random number. All 0xff in the ioniq message.
    memset(&myethtransmitbuffer[55], 0xFF, 16);//in range(55, 71):
}
*/

void homeplug_composeAttenCharInd(homeplug* const me){			//evse mode function
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 129;

	homeplug_cleanTxBuffer(me);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] sending pevMac = %x,%x,%x,%x,%x,%x" ,me->pevMac[0],me->pevMac[1],me->pevMac[2],me->pevMac[3],me->pevMac[4],me->pevMac[5]);
    addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] sending myMAC = %x,%x,%x,%x,%x,%x" ,myMAC[0],myMAC[1],myMAC[2],myMAC[3],myMAC[4],myMAC[5]);
    addToTrace(strTmp);
	my_memcpy(&TxBuffer[0], me->pevMac, 6);	// Destination MAC
	my_memcpy(&TxBuffer[6], myMAC, 6);		// Source MAC
	// Protocol
	TxBuffer[12]=0x88; // Protocol HomeplugAV
	TxBuffer[13]=0xE1;
	TxBuffer[14]=0x01; // version
	TxBuffer[15]=0x6E; /// ATTEN_CHAR.IND
	TxBuffer[16]=0x60; //
	TxBuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
	TxBuffer[18]=0x00; //
	TxBuffer[19]=0x00; // apptype
	TxBuffer[20]=0x00; // security
	my_memcpy(&TxBuffer[21],me->pevMac,6);  // The wireshark calls it source_mac, but alpitronic fills it with PEV mac. We use the PEV MAC.
	my_memcpy(&TxBuffer[27],me->pevRunId,8);// PEVrunid 8 bytes
	//fillRunId(27);  // PEVrunid 8 bytes
	TxBuffer[35]=0x00; // 35 - 51 source_id, 17 bytes. The alpitronic fills it with 00
	//memset(&myethtransmitbuffer[36],0,15);//??
	TxBuffer[52]=0x00; // 52 - 68 response_id, 17 bytes. The alpitronic fills it with 00.
	TxBuffer[69]=0x0A; // Number of sounds. 10 in normal case. Should this be more flexible, e.g. using the counter from first MNBC_SOUND?
	TxBuffer[70]=0x3A; // Number of groups = 58. Should this be more flexible?
	memset(&TxBuffer[71], 9, 58);
	// 71 to 128: The group attenuation for the 58 announced groups.
	// Typical values are between 1 and 0x19. Since we have no real measurements from the AR7020,
	//   we just simulate something. 0 seems to be interpreted as "defect", the IONIQ does not send
	// a positive response in this case.
	// higher attenuation for the higher frequencies, to be a little bit realistic (real data from alpitronic trace)
	TxBuffer[126]=0x0f;
	TxBuffer[127]=0x13;
	TxBuffer[128]=0x19;
}
/*
void composeAttenCharRsp(void) {			//PEV mode function
    // reference: see wireshark interpreted frame from Ioniq
    myethtransmitbufferLen = 70;
    homeplug_cleanTransmitBuffer();
    // Destination MAC
    fillDestinationMac(evseMac, 0);
    // Source MAC
    fillSourceMac(myMAC, 6);
    // Protocol
    myethtransmitbuffer[12]=0x88; // Protocol HomeplugAV
    myethtransmitbuffer[13]=0xE1; //
    myethtransmitbuffer[14]=0x01; // version
    myethtransmitbuffer[15]=0x6F; // ATTEN_CHAR.RSP
    myethtransmitbuffer[16]=0x60; // 
    myethtransmitbuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
    myethtransmitbuffer[18]=0x00; // 
    myethtransmitbuffer[19]=0x00; // apptype
    myethtransmitbuffer[20]=0x00; // sectype
    fillSourceMac(myMAC, 21); // 21 to 26: source MAC
    fillRunId(27); // 27 to 34: runid. The PEV mac, plus 00 00.
    // 35 to 51: source_id, all 00
    // 52 to 68: resp_id, all 00
    // 69: result. 0 is ok
}
*/

/*
void composeSlacMatchReq(void) {				//PEV mode runslace function
    // reference: see wireshark interpreted frame from Ioniq
    myethtransmitbufferLen = 85;
    homeplug_cleanTransmitBuffer();
    // Destination MAC
    fillDestinationMac(evseMac, 0);
    // Source MAC
    fillSourceMac(myMAC, 6);
    // Protocol
    myethtransmitbuffer[12]=0x88; // Protocol HomeplugAV
    myethtransmitbuffer[13]=0xE1; //
    myethtransmitbuffer[14]=0x01; // version
    myethtransmitbuffer[15]=0x7C; // SLAC_MATCH.REQ
    myethtransmitbuffer[16]=0x60; // 
    myethtransmitbuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
    myethtransmitbuffer[18]=0x00; // 
    myethtransmitbuffer[19]=0x00; // apptype
    myethtransmitbuffer[20]=0x00; // sectype
    myethtransmitbuffer[21]=0x3E; // 21 to 22: length
    myethtransmitbuffer[22]=0x00; // 
    // 23 to 39: pev_id, all 00
    fillSourceMac(myMAC, 40); // 40 to 45: PEV MAC
    // 46 to 62: evse_id, all 00
    fillDestinationMac(evseMac, 63); // 63 to 68: EVSE MAC
    fillSourceMac(myMAC, 69); // 69 to 76: runid. The PEV mac, plus 00 00.
    // 77 to 84: reserved, all 00        
}
*/
void homeplug_composeSlacMatchCnf(homeplug* const me){		//evse added	send data to PEV
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 109;

	homeplug_cleanTxBuffer(me);
    // Destination MAC
	my_memcpy(&TxBuffer[0],me->pevMac,6);
    // Source MAC
	my_memcpy(&TxBuffer[6],myMAC,6);
    // Protocol
	TxBuffer[12]=0x88;// / Protocol HomeplugAV
	TxBuffer[13]=0xE1;
	TxBuffer[14]=0x01; // version
	TxBuffer[15]=0x7D; // SLAC_MATCH.CNF
	TxBuffer[16]=0x60; //
	TxBuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
	TxBuffer[18]=0x00; //
	TxBuffer[19]=0x00; // apptype
	TxBuffer[20]=0x00; // security
	TxBuffer[21]=0x56; // length 2 byte
	TxBuffer[22]=0x00; // 23 - 39: pev_id 17 bytes. All zero in alpi/Ioniq trace.
    my_memcpy(&TxBuffer[40],me->pevMac,6); // 40 - 45 pev_mac
                                   // 46 - 62: evse_id 17 bytes. All zero in alpi/Ioniq trace.
    my_memcpy(&TxBuffer[63],myMAC,6); // 63 - 68 evse_mac
    my_memcpy(&TxBuffer[69],me->pevRunId,8);// runid 8 bytes 69-76 run_id. Is the ioniq mac plus 00 00.
    //fillRunId(69);
    // 77 to 84 reserved 0
    my_memcpy(&TxBuffer[85], NID, 7);// 85-91 NID. We can nearly freely choose this, but the upper two bits need to be zero
    //setNidAt(85);
   // 92 reserved 0
    my_memcpy(&TxBuffer[93], NMK_EVSE_random, 16);// 93 to 108 NMK. We can freely choose this. Normally we should use a random number.
    //setNmkAt(93);

	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] pevMac = %x,%x,%x,%x,%x,%x" ,me->pevMac[0],me->pevMac[1],me->pevMac[2],me->pevMac[3],me->pevMac[4],me->pevMac[5]);
    addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] myMAC = %x,%x,%x,%x,%x,%x" ,myMAC[0],myMAC[1],myMAC[2],myMAC[3],myMAC[4],myMAC[5]);
    addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] pevRunId = %x,%x,%x,%x,%x,%x,%x,%x" ,me->pevRunId[0],me->pevRunId[1],me->pevRunId[2],me->pevRunId[3],me->pevRunId[4],me->pevRunId[5],me->pevRunId[6],me->pevRunId[7]);
    addToTrace(strTmp);
    McuXFormat_xsprintf(strTmp,"[HOMEPLUG] NID = %d,%d,%d,%d,%d,%d,%d" ,NID[0],NID[1],NID[2],NID[3],NID[4],NID[5],NID[6]);
    addToTrace(strTmp);
	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] NMK_EVSE_random = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x" ,NMK_EVSE_random[0],NMK_EVSE_random[1],NMK_EVSE_random[2],NMK_EVSE_random[3],NMK_EVSE_random[4],NMK_EVSE_random[5],NMK_EVSE_random[6],NMK_EVSE_random[7],NMK_EVSE_random[8],NMK_EVSE_random[9],NMK_EVSE_random[10],NMK_EVSE_random[11],NMK_EVSE_random[12],NMK_EVSE_random[13],NMK_EVSE_random[14],NMK_EVSE_random[15]);
	addToTrace(strTmp);
}
/*
void sendTestFrame(void) {
  composeGetSwReq();
  myEthTransmit();
}
*/

void homeplug_evaluateGetKeyCnf(homeplug* const me) {			//PEV mode fucntion
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;

	uint8_t result;
	char strMac[20];
	char strResult[100];
	char strNID[50];
	char s[300];
	uint8_t sourceMac[6];
	char localModemMac[6];

	uint8_t RNID[7]={0};
	//uint8_t NMKdevelopment[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }; // network key for development access
	uint8_t localModemCurrentKey[16];
	//uint8_t localModemFound,isDeveloperLocalKey;
    addToTrace("[HOMEPLUG] received GET_KEY.CNF");
    me->numberOfFoundModems += 1;
    my_memcpy(&sourceMac[0], &RxBuffer[6], 6);
    McuXFormat_xsprintf(strMac,"%x:%x:%x:%x:%x:%x", sourceMac[0],sourceMac[1],sourceMac[2],sourceMac[3],sourceMac[4],sourceMac[5]);
    result = RxBuffer[19]; // 0 in case of success
    if (result==0) {
    	McuXFormat_xsprintf(strResult,"(OK)");
    } else {
    	McuXFormat_xsprintf(strResult,"(NOK)");
	}
    McuXFormat_xsprintf(strTmp,"[HOMEPLUG]  Modem # %d has(MAC) %s and result code is %d %s", me->numberOfFoundModems,strMac,result,strResult);
    addToTrace(strTmp);
    if (me->numberOfFoundModems>1) {
    	addToTrace("[HOMEPLUG] Info: NOK is normal for remote modems.");
    }
    //    We observed the following cases:
    //    (A) Result=1 (NOK), NID all 00, key all 00: We requested the key with the wrong NID.
    //    (B) Result=0 (OK), NID all 00, key non-zero: We used the correct NID for the request.
    //               It is the local TPlink adaptor. A fresh started non-coordinator, like the PEV side.
    //    (C) Result=0 (OK), NID non-zero, key non-zero: We used the correct NID for the request.
    //               It is the local TPlink adaptor.
    //    (D) Result=1 (NOK), NID non-zero, key all 00: It was a remote device. They are rejecting the GET_KEY.
    if (result==0) {
      // The ok case is for sure the local modem. Let's store its data.
      my_memcpy(localModemMac, sourceMac, 6);		//modem punya mac
	  my_memcpy(&localModemCurrentKey[0], &RxBuffer[41], 16);		// NMK has 16 bytes
	  McuXFormat_xsprintf(s,"%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",localModemCurrentKey[0],localModemCurrentKey[1],localModemCurrentKey[2],localModemCurrentKey[3],localModemCurrentKey[4],localModemCurrentKey[5],localModemCurrentKey[6],localModemCurrentKey[7],localModemCurrentKey[8],localModemCurrentKey[9],localModemCurrentKey[10],localModemCurrentKey[11],localModemCurrentKey[12],localModemCurrentKey[13],localModemCurrentKey[14],localModemCurrentKey[15]);
	  McuXFormat_xsprintf(strTmp,"[HOMEPLUG] The local modem has NMKey %s",s);
	  addToTrace(strTmp);
	  if(0 == memcmp( localModemCurrentKey, NMK_EVSE_random, 16)){
		addToTrace("[HOMEPLUG] This is the right NMK.");
		if(me->numberOfFoundModems == 1){
			me->NMK_condition = 2;
		}
		//isDeveloperLocalKey = 1;
	  }else{
		addToTrace("[HOMEPLUG] This is NOT the right NMK.");
		if(me->numberOfFoundModems == 1){
			me->NMK_condition = 1;
		}
		//localModemFound = 1;
	  }
    }else{
    	addToTrace("[HOMEPLUG] cannot read NMK.");
    	if(me->numberOfFoundModems == 1){
    		me->NMK_condition = 1;
    	}
    }
    //    The getkey response contains the Network ID (NID), even if the request was rejected. We store the NID,
    //    to have it available for the next request. Use case: A fresh started, unconnected non-Coordinator
    //    modem has the default-NID all 00. On the other hand, a fresh started coordinator has the
    //    NID which he was configured before. We want to be able to cover both cases. That's why we
    //    ask GET_KEY, it will tell the NID (even if response code is 1 (NOK), and we will use this
    //    received NID for the next request. This will be anwsered positive (for the local modem).
    my_memcpy(&RNID[0], &RxBuffer[29], 7);// NID has 7 bytes
    McuXFormat_xsprintf(strNID,"%d %d %d %d %d %d %d",RNID[0],RNID[1],RNID[2],RNID[3],RNID[4],RNID[5],RNID[6]);
    McuXFormat_xsprintf(strTmp,"[HOMEPLUG] From GetKeyCnf, got network ID (NID) %s",strNID);
    addToTrace(strTmp);
}


void homeplug_composeSetKey(homeplug* const me, uint8_t variation) {			//original modified to vaiable //to set NMK to local modem
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 60;
	homeplug_cleanTxBuffer(me);

	addToTrace("[HOMEPLUG] CM_SET_KEY.REQ");
	// CM_SET_KEY.REQ request
  // From example trace from catphish https://openinverter.org/forum/viewtopic.php?p=40558&sid=9c23d8c3842e95c4cf42173996803241#p40558
  //   Table 11-88 in the homeplug_av21_specification_final_public.pdf

  // Destination MAC
  my_memcpy(&TxBuffer[0],MAC_BROADCAST,6);
  // Source MAC
  my_memcpy(&TxBuffer[6],myMAC,6);
  // Protocol
  TxBuffer[12]=0x88; // Protocol HomeplugAV
  TxBuffer[13]=0xE1; //
  TxBuffer[14]=0x01; // version
  TxBuffer[15]=0x08; // CM_SET_KEY.REQ
  TxBuffer[16]=0x60; //
  TxBuffer[17]=0x00; // frag_index
  TxBuffer[18]=0x00; // frag_seqnum
  //CM_SET_KEY.REQ Message
  TxBuffer[19]=0x01; // 0 key info type NMK

  TxBuffer[20]=0xaa; // 1 my nonce
  TxBuffer[21]=0xaa; // 2
  TxBuffer[22]=0xaa; // 3
  TxBuffer[23]=0xaa; // 4

  TxBuffer[24]=0x00; // 5 your nonce
  TxBuffer[25]=0x00; // 6
  TxBuffer[26]=0x00; // 7
  TxBuffer[27]=0x00; // 8

  TxBuffer[28]=0x04; // 9 nw info pid HLE protoco 0x04

  TxBuffer[29]=0x2D; // 10 info prn		AB
  TxBuffer[30]=0x37; // 11		34
  TxBuffer[31]=0x03; // 12 pmn ff
  TxBuffer[32]=0x00; // 13 cco cap
  my_memcpy(&TxBuffer[33], NID, 7);
  //setNidAt(33); // 14-20 nid  7 bytes from 33 to 39
                //          Network ID to be associated with the key distributed herein.
                //          The 54 LSBs of this field contain the NID (refer to Section 3.4.3.1). The
                //          two MSBs shall be set to 0b00.
  TxBuffer[40]=0x01; // 21 peks (payload encryption key select) Table 11-83. 01 is NMK. We had 02 here, why???
                               // with 0x0F we could choose "no key, payload is sent in the clear"
  //setNmkAt(41);
  my_memcpy(&TxBuffer[41], NMK_EVSE_random, 16);
  //#define variation 0
  if(variation==0){
	  TxBuffer[41]+=variation; // to try different NMKs
  }
  //Key Types DAK and HASH KEY are never permitted.
  // and three remaining zeros
}



void homeplug_composeGetKey(homeplug* const me) {
	uint8_t* TxBuffer = me->QCA->ETH.TxBuffer;
	me->QCA->ETH.TxSize = 60;
	homeplug_cleanTxBuffer(me);
		// CM_GET_KEY.REQ request
        // from https://github.com/uhi22/plctool2/blob/master/listen_to_eth.c
        // and homeplug_av21_specification_final_public.pdf
    // Destination MAC
    my_memcpy(&TxBuffer[0],MAC_BROADCAST,6);
    // Source MAC
    my_memcpy(&TxBuffer[6],myMAC,6);
    // Protocol
    TxBuffer[12]=0x88; // Protocol HomeplugAV
    TxBuffer[13]=0xE1;
    TxBuffer[14]=0x01; // version
    TxBuffer[15]=0x0C; // CM_GET_KEY.REQ https://github.com/uhi22/plctool2/blob/master/plc_homeplug.h
    TxBuffer[16]=0x60; //
    TxBuffer[17]=0x00; // 2 bytes fragmentation information. 0000 means: unfragmented.
    TxBuffer[18]=0x00; //
    TxBuffer[19]=0x00; // 0 Request Type 0=direct
    TxBuffer[20]=0x01; // 1 RequestedKeyType only "NMK" is permitted over the H1 interface.
                               //           value see HomeplugAV2.1 spec table 11-89. 1 means AES-128.
    my_memcpy(&TxBuffer[21], NID, 7);    // NID starts here (table 11-91 Homeplug spec is wrong. Verified by accepted command.)
    //homeplug_setNidAt(me,21); // NID starts here (table 11-91 Homeplug spec is wrong. Verified by accepted command.)
    TxBuffer[28]=0xaa; // 10-13 mynonce. The position at 28 is verified by the response of the devolo.
    TxBuffer[29]=0xaa; //
    TxBuffer[30]=0xaa; //
    TxBuffer[31]=0xaa; //
    TxBuffer[32]=0x04; // 14 PID. According to  ISO15118-3 fix value 4, "HLE protocol"
    TxBuffer[33]=0x00; // 15-16 PRN Protocol run number
    TxBuffer[34]=0x00; //
    TxBuffer[35]=0x00; // 17 PMN Protocol message number
}



void homeplug_evaluateSetKeyCnf(homeplug* const me) {
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
    // The Setkey confirmation
    uint8_t result;
    // In spec, the result 0 means "success". But in reality, the 0 means: did not work. When it works,
    // then the LEDs are blinking (device is restarting), and the response is 1.
    addToTrace("[HOMEPLUG] received SET_KEY.CNF");
    //addToTrace("[PEVSLAC] received SET_KEY.CNF");
    result = RxBuffer[19];
    if (result == 0) {
        addToTrace("[HOMEPLUG] SetKeyCnf says 0, this would be a bad sign for local modem, but normal for remote.");
    } else {
    	McuXFormat_xsprintf(strTmp,"[HOMEPLUG] SetKeyCnf says %d, this is formally 'rejected', but indeed ok." , result);
        addToTrace(strTmp);
        addToTrace("[HOMEPLUG] modem is restarting");	//
        //connMgr_SlacOk();		//todo problem of setup the Key NMK
        homeplug_composeGetKey(me);
        QCA7000_EthTransmit(me->QCA);
		addToTrace("[HOMEPLUG] transmitting GET_KEY.REQ");	//
    }
    addToTrace("[HOMEPLUG] transmitting GET_SOFTWARE.REQ");	//
    homeplug_composeGetSwWithRamdomMac(me);
    QCA7000_EthTransmit(me->QCA);
}


void homeplug_evaluateGetSwCnf(homeplug* const me) {// this a bit diffrent from python  //PEV mode function
    // The GET_SW confirmation. This contains the software version of the homeplug modem.
    //   Reference: see wireshark interpreted frame from TPlink, Ioniq and Alpitronic charger
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	uint8_t i, x;
    uint8_t sourceMac[6];
    char strVersion[200];
    uint8_t verLen;
    //char strMac[20];
    addToTrace("[HOMEPLUG] received GET_SW.CNF"); //[PEVSLAC]
    //numberOfSoftwareVersionResponses+=1;
    my_memcpy(sourceMac,&RxBuffer[6],6);
    verLen = RxBuffer[22];
    if ((verLen>0) && (verLen<0x30)) {
      for (i=0; i<verLen; i++) {
            x = RxBuffer[23+i];
            if (x<0x20) { x=0x20; }// make unprintable character to space.
            strVersion[i]=x;
      }
      strVersion[i] = 0; 
      //addToTrace(strMac);
      McuXFormat_xsprintf(strTmp, "[HOMEPLUG] software version %s\r\n", strVersion);
      addToTrace(strTmp);
      //addToTrace("For " + strMac + " the software version is " + String(strVersion));

    }
}



void homeplug_evaluateSlacParamReq(homeplug* const me){		//EVSE mode received PEV mac
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	// We received a SLAC_PARAM request from the PEV. This is the initiation of a SLAC procedure.
	// We extract the pev MAC from it.
	#ifdef iAmEvse
		addToTrace("[HOMEPLUG] received SLAC_PARAM.REQ");
		//extract the RunId from the SlacParamReq, and store it for later use
		my_memcpy(me->pevMac,&RxBuffer[6],6);
		my_memcpy(me->pevRunId,&RxBuffer[21],8);
		// We are EVSE, we want to answer.
		//addToTrace("SLAC started: evseState");

		McuXFormat_xsprintf(strTmp,"[HOMEPLUG] Received pevMac = %x,%x,%x,%x,%x,%x" ,me->pevMac[0],me->pevMac[1],me->pevMac[2],me->pevMac[3],me->pevMac[4],me->pevMac[5]);
	    addToTrace(strTmp);
		McuXFormat_xsprintf(strTmp,"[HOMEPLUG] Received pevRunId = %x,%x,%x,%x,%x,%x,%x,%x" ,me->pevRunId[0],me->pevRunId[1],me->pevRunId[2],me->pevRunId[3],me->pevRunId[4],me->pevRunId[5],me->pevRunId[6],me->pevRunId[7]);
	    addToTrace(strTmp);

		addToTrace("[HOMEPLUG] transmitting CM_SLAC_PARAM.CNF");
		homeplug_composeSlacParamCnf(me);
		QCA7000_EthTransmit(me->QCA);
	#endif



}

/*
void evaluateSlacParamCnf(void) {			//pev mode function
  // As PEV, we receive the first response from the charger.
  addToTrace("[PEVSLAC] Checkpoint102: received SLAC_PARAM.CNF");
  checkpointNumber = 102;					//what purpose ?
  if (iAmPev) {
    if (pevSequenceState==STATE_WAITING_FOR_SLAC_PARAM_CNF) { //  we were waiting for the SlacParamCnf
      pevSequenceDelayCycles = 4; // original Ioniq is waiting 200ms
      slac_enterState(STATE_SLAC_PARAM_CNF_RECEIVED); // enter next state. Will be handled in the cyclic runSlacSequencer
		}
	}
}
*/

void homeplug_evaluateMnbcSoundInd(homeplug* const me){			//evse added
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
	// We received MNBC_SOUND.IND from the PEV. Normally this happens 10times, with a countdown (remaining number of sounds)
	// running from 9 to 0. If the countdown is 0, this is the last message. In case we are the EVSE, we need
	// to answer with a ATTEN_CHAR.IND, which normally contains the attenuation for 10 sounds, 58 groups.
	addToTrace("[HOMEPLUG] received MNBC_SOUND.IND");
	#ifdef iAmEvse
		uint8_t countdown = RxBuffer[38];
		McuXFormat_xsprintf(strTmp, "[HOMEPLUG] Countdown=%d",countdown);
		addToTrace(strTmp);
		if (countdown == 0){
			homeplug_composeAttenCharInd(me);
			addToTrace("[HOMEPLUG] transmitting ATTEN_CHAR.IND");
			QCA7000_EthTransmit(me->QCA);
		}
	#endif
}

void homeplug_evaluateStartAttenCharInd(homeplug* const me){
	uint8_t* RxBuffer = me->QCA->ETH.RxBuffer;
    addToTrace("[HOMEPLUG] received START_ATTEN_CHAR.IND");
    float uPresent;
    // nothing to do as PEV or EVSE.
    // interpretation just in case we use it as special message in EVSE mode to get information from the power supply
    if (iAmEvse==1){
        if((RxBuffer[38] == 0xDC) && (RxBuffer[39] == 0x55) && (RxBuffer[40] == 0xAA)){
            uPresent = (float)RxBuffer[43];
            uPresent*=256.0f;
            uPresent+=RxBuffer[44];
            uPresent/=10.0f; // scaling in the message is 0.1V
    		McuXFormat_xsprintf(strTmp,"[HOMEPLUG] uPresent = %f" ,uPresent);
    	    addToTrace(strTmp);
            //callbackShowStatus(str(uPresent), "PowerSupplyUPresent")
            //# Todo: evaluate other information of the power supply, like cable check result, current, temperature, ...
        }
    }
}
/*
void evaluateAttenCharInd(void) {			//PEV orignal function	recieved evseMAC address
  uint8_t i;
  addToTrace("[PEVSLAC] received ATTEN_CHAR.IND");
  if (iAmPev==1) {
        //addToTrace("[PEVSLAC] received AttenCharInd in state " + str(pevSequenceState))
        if (pevSequenceState==STATE_WAIT_FOR_ATTEN_CHAR_IND) { // we were waiting for the AttenCharInd
            //todo: Handle the case when we receive multiple responses from different chargers.
            //      Wait a certain time, and compare the attenuation profiles. Decide for the nearest charger.
            //Take the MAC of the charger from the frame, and store it for later use.
            for (i=0; i<6; i++) {
                evseMac[i] = myethreceivebuffer[6+i]; // source MAC starts at offset 6
            }
            AttenCharIndNumberOfSounds = myethreceivebuffer[69];
            //addToTrace("[PEVSLAC] number of sounds reported by the EVSE (should be 10): " + str(AttenCharIndNumberOfSounds))
            composeAttenCharRsp();
            addToTrace("[PEVSLAC] transmitting ATTEN_CHAR.RSP...");
            checkpointNumber = 140;
            myEthTransmit();
            pevSequenceState=STATE_ATTEN_CHAR_IND_RECEIVED; // enter next state. Will be handled in the cyclic runSlacSequencer
		    }
	}
}
*/

void homeplug_evaluateSlacMatchReq(homeplug* const me){			//evse added

     // We received SLAC_MATCH.REQ from the PEV.
     // If we are EVSE, we send the response.
     addToTrace("[HOMEPLUG] received SLAC_MATCH.REQ");
     if (iAmEvse==1){
         homeplug_composeSlacMatchCnf(me);
         QCA7000_EthTransmit(me->QCA);
         addToTrace("[HOMEPLUG] transmitting SLAC_MATCH.CNF");

     }
}
/*
void evaluateSlacMatchCnf(void) {			//PEV mode function	, EVSE used ? //PEV using when EVSE send NMK
    uint8_t i;
    // The SLAC_MATCH.CNF contains the NMK and the NID.
    // We extract this information, so that we can use it for the CM_SET_KEY afterwards.
    // References: https://github.com/qca/open-plc-utils/blob/master/slac/evse_cm_slac_match.c
    // 2021-12-16_HPC_säule1_full_slac.pcapng
    if (iAmEvse==1) {
            // If we are EVSE, nothing to do. We have sent the match.CNF by our own.
            // The SET_KEY was already done at startup.
    } else {
            addToTrace("[PEVSLAC] received SLAC_MATCH.CNF");
            for (i=0; i<7; i++) { // NID has 7 bytes
                NID[i] = myethreceivebuffer[85+i];
            }
            for (i=0; i<16; i++) {
                NMK[i] = myethreceivebuffer[93+i];
            }
            addToTrace("[PEVSLAC] From SlacMatchCnf, got network membership key (NMK) and NID.");
            // use the extracted NMK and NID to set the key in the adaptor:
            composeSetKey(0);
            addToTrace("[PEVSLAC] Checkpoint170: transmitting CM_SET_KEY.REQ");
            checkpointNumber = 170;
            publishStatus("SLAC", "set key");
            myEthTransmit();		///spi qca7005
            if (pevSequenceState==STATE_WAITING_FOR_SLAC_MATCH_CNF) { // we were waiting for finishing the SLAC_MATCH.CNF and SET_KEY.REQ
                slac_enterState(STATE_WAITING_FOR_RESTART2);
            }
		}
}
*/

void homeplug_evaluateReceivedPacket(homeplug* const me) {
	uint16_t MsgType = m16(me->QCA->ETH.RxBuffer[16] , me->QCA->ETH.RxBuffer[15]);
	//uint16_t MsgType = homeplug_getManagementMessageType(me);
	//McuXFormat_xsprintf(strTmp, "Running Home Plug packet.Type=0x%X",MsgType); //0x%04X
	//addToTrace(strTmp);
	switch (MsgType) {//0x6076  0x6086
		case CM_GET_KEY + MMTYPE_CNF:    		homeplug_evaluateGetKeyCnf(me);    		break;	//original  //0x600D
		case CM_SLAC_MATCH + MMTYPE_REQ: 		homeplug_evaluateSlacMatchReq(me); 		break;	//evse added	//0x607C
		//case CM_SLAC_MATCH + MMTYPE_CNF: 		break;//evaluateSlacMatchCnf(); break;	//evse might run	//0x607D
		case CM_SLAC_PARAM + MMTYPE_REQ: 		homeplug_evaluateSlacParamReq(me); 		break;	//evse added  //0x6064
		//case CM_SLAC_PARAM + MMTYPE_CNF: 		homeplug_evaluateSlacParamCnf(); 		break;	//original    //0x6065
		case CM_START_ATTEN_CHAR + MMTYPE_IND:	homeplug_evaluateStartAttenCharInd(me);	break;
		case CM_MNBC_SOUND + MMTYPE_IND: 		homeplug_evaluateMnbcSoundInd(me); 		break;	//evse added  //0x6076
		//case CM_ATTEN_CHAR + MMTYPE_IND: 		homeplug_evaluateAttenCharInd(); 		break;	//orignal		//0x606E
		case CM_SET_KEY + MMTYPE_CNF:    		homeplug_evaluateSetKeyCnf(me);    		break;	//orignal  //evse needed response because we send SET_KEY.REQ //0x6009
		case CM_GET_SW + MMTYPE_CNF:     		homeplug_evaluateGetSwCnf(me);     		break;	//orignal  //this show software version of the homeplug modem //0xA001
		//case 													  break;//Type=0xA062   host action ind

		case CM_ATTEN_PROFILE + MMTYPE_IND: 	addToTrace("[HOMEPLUG] received CM_ATTEN_PROFILE.IND and Do nothing");		break;  //0x6086 not improtant evse
		//case CM_START_ATTEN_CHAR + MMTYPE_IND:	addToTrace("received CM_START_ATTEN_CHAR.IND and DO nothing"); 	break; //0x6070 not important evse
		case CM_ATTEN_CHAR + MMTYPE_RSP: 		addToTrace("[HOMEPLUG] received CM_ATTEN_CHAR.RSP for confirmation only"); 		break;//6060c+3 =0x606F not important evse ATTEN_CHAR.RSP
		default:								addToTrace("[HOMEPLUG] No type for SLAC");

	}
}
/*
uint8_t isEvseModemFound(void) {
  // todo: look whether the MAC of the EVSE modem is in the list of detected modems
  // as simple solution we say: If we see two modems, then it should be one
  //   local in the car, and one in the charger.
  return numberOfFoundModems>1;
}
*/
/*
void slac_enterState(int n) {	//python named enterState  //pev mode function
  McuXFormat_xsprintf(strTmp, "[PEVSLAC] from %d entering %d", pevSequenceState, n);
  addToTrace(strTmp);
  pevSequenceState = n;
  pevSequenceCyclesInState = 0;
}
*/
/*
int isTooLong(void) {
  // The timeout handling function.
  return (pevSequenceCyclesInState > 500);
}
*/


uint32_t homeplug_randomNumber(homeplug* const me){			//not using
	extern RNG_HandleTypeDef hrng;
	uint32_t myRandomNumber=0;
	HAL_RNG_GenerateRandomNumber(&hrng,&myRandomNumber);
	return myRandomNumber;
}


void homeplug_runEvseSlacHandler(homeplug* const me){                                  //#evse loop added this mode need to modify to QCA7005 type , current one is TP link one
	 //HAL_Delay(500);		//this can make NID work well but MNK not yet done
	//static uint8_t evseSlacHandlerState = 0;
	//if (evseSlacHandlerState==0){                  //#run once only
		// we did not yet configure our EVSE modem with the random key. Do it now.
		// Fill some of the bytes of the NMK with random numbers. The others stay at 0x77 for easy visibility.
		//srand(time(NULL));   // Initialization, should only be called once.
	    //uint32_t randomNumber = my_random();
		//NMK_EVSE_random[2] = (uint8_t)(randomNumber);			//int(random()*255);	//python random is 0 to 0.99
		//NMK_EVSE_random[3] = (uint8_t)(randomNumber>>8);			//int(random()*255);
		//NMK_EVSE_random[4] = (uint8_t)(randomNumber>>16);			//int(random()*255);
		//NMK_EVSE_random[5] = (uint8_t)(randomNumber>>24);			//int(random()*255);
		//NMK_EVSE_random[6] = (uint8_t)(randomNumber>>3);			//int(random()*255);
		homeplug_composeSetKey(me,0);   //##fill up myethtransmitbuffer
		McuXFormat_xsprintf(strTmp,"[HOMEPLUG] transmitting SET_KEY.REQ, to configure the EVSE modem with random NMK = %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x" ,NMK_EVSE_random[0],NMK_EVSE_random[1],NMK_EVSE_random[2],NMK_EVSE_random[3],NMK_EVSE_random[4],NMK_EVSE_random[5],NMK_EVSE_random[6],NMK_EVSE_random[7],NMK_EVSE_random[8],NMK_EVSE_random[9],NMK_EVSE_random[10],NMK_EVSE_random[11],NMK_EVSE_random[12],NMK_EVSE_random[13],NMK_EVSE_random[14],NMK_EVSE_random[15]);
		addToTrace(strTmp);
		QCA7000_EthTransmit(me->QCA);       //#tell Homeplug to set NMK
	//	evseSlacHandlerState = 1; // setkey was done
	//}
}



void homeplug_Init(homeplug* const me) {
  McuXFormat_xsprintf(strTmp,"[HOMEPLUG] NID = %d,%d,%d,%d,%d,%d,%d" ,NID[0],NID[1],NID[2],NID[3],NID[4],NID[5],NID[6]);
  addToTrace(strTmp);
  McuXFormat_xsprintf(strTmp,"[HOMEPLUG] RESET RANDOMMAC");
  addToTrace(strTmp);
  me->randomMac = 0;
  me->numberOfFoundModems = 0;

}

