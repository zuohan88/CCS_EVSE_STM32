#include "evseStateMachine.h"

#include "AppCanBus.h"
/* The Charging State Machine for the charger */
/*** user variable *****************************************/
//todo EVSEStatus need to do if esle if e btton
//emergency stop run at where ?
// and isolation check
//exchange data with pi
//USB system


//evccData EVCC	= {0};
//evseData EVSE 	= {0};
//evseData LIMIT 	= {0};
//userData USER ={.MaxPow = 3000, .MaxVolt =100 , .MinVolt = 50, .MaxAmp = 100, .MinAmp = 50};

//extern module module1 ,system1;

//EVSE.MaxPow = 3781;				//w
//EVSE.MaxAmp = 200;			//A Max
//EVSE.MinAmp = 0;				//A Min
//EVSE.MaxVolt = 500;			//V Max
//EVSE.MinVolt = 50;			//V Min

//uint8_t EVSE_RELAY = 0;
//uint32_t evse_cyclesInState;		//


extern AppCanBusTask AppCanBusTask1;




#define SenssionIDCheck 	1
#define ISO15118			0
/*
void testModuleForPreCharge(void){
	if(simulatedPresentVoltage<EVCCTargetVoltage/2)
	   simulatedPresentVoltage = EVCCTargetVoltage/2;
	if(simulatedPresentVoltage<EVCCTargetVoltage-30)
	   simulatedPresentVoltage += 20;
	if(simulatedPresentVoltage<EVCCTargetVoltage)
	   simulatedPresentVoltage += 5;
	EVSENowVolt = simulatedPresentVoltage; //# "345"
}
*/
/***local function prototypes *****************************************/

uint8_t evse_isTooLong(evse* const me);

void addV2GTPHeader(evse* const me, const uint8_t *exiBuffer, uint8_t exiBufferLen);
void encode_only_appHand(evse* const me);
void encode_only_Din(evse* const me);
void routeDecoderInputData(evse* const me);

uint8_t SessionIdCheckRq(evse* const me);


void DinV2GHeaderCheck(evse* const me);
int8_t decodeTCP(evse* const me);
void stateFunctionSAPRq(evse* const me);
void stateFunctionSSRq(evse* const me);
void stateFunctionSDRq(evse* const me);
void stateFunctionSPSRq(evse* const me);
void stateFunctionCARq(evse* const me);
void stateFunctionCPDRq(evse* const me);
void stateFunctionCCRq(evse* const me);
void stateFunctionPDRq(evse* const me);
void stateFunctionPCRq(evse* const me);
void stateFunctionCDRq(evse* const me);
void stateFunctionWDRq(evse* const me);
void stateFunctionSSpRq(evse* const me);


dinDC_EVSEStatusCodeType User_EVSEStatusCheck(evse* const me);
dinisolationLevelType User_EVSEIsolationStatusCheck(evse* const me);
dinEVSEProcessingType User_CCRs_EVSEProcessingCheck(evse* const me);
dinEVSEProcessingType User_CPDRs_EVSEProcessingCheck(evse* const me);
dinEVSEProcessingType User_CPDRs_EVSEProcessingCheck(evse* const me);
dinEVSEProcessingType User_CARs_EVSEProcessingCheck(evse* const me);
// not sure if the response if emergency jump to where

uint8_t EVSE_set_volt_curr(evse* const me, uint32_t volt,uint32_t curr);
void EVSE_now_update(evse* const me);
uint8_t FunctionCableCheck(evse* const me);
uint8_t FunctionInternalCheck(evse* const me);



void Contractor_On(evse* const me){
	RELAY_1_ON;
	me->relay =1;
}
void Contractor_Off(evse* const me){
	RELAY_1_OFF;
	me->relay = 0;
}


uint8_t FunctionCableCheck(evse* const me){
	return 0;		//finished
}

uint8_t FunctionInternalCheck(evse* const me){
	return 0;
}

void EVSE_now_update(evse* const me){
	me->EVSE.NowVolt = (int16_t)(me->system->milivolt32/1000);		//sensed voltage
	me->EVSE.NowAmp = (int16_t)(me->system->miliamp32/1000);
}

void  EVSE_set_EVSE_Limit(evse* const me){
	me->LIMIT.MaxVolt = me->USER.MaxVolt;	//starting I set 500
	me->LIMIT.MinVolt = me->USER.MinVolt;
	me->LIMIT.MaxAmp = me->USER.MaxAmp;
	me->LIMIT.MinAmp = me->USER.MinAmp;
	me->LIMIT.MaxPow = me->USER.MaxPow;


	me->LIMIT.MaxVolt = (me->LIMIT.MaxVolt > me->EVSE.MaxVolt) ? me->EVSE.MaxVolt :  me->LIMIT.MaxVolt;
	me->LIMIT.MaxVolt = (me->LIMIT.MaxVolt > me->EVCC.MaxVolt) ? me->EVCC.MaxVolt :  me->LIMIT.MaxVolt;	//if i bigger than use smaller value

	me->LIMIT.MaxAmp = (me->LIMIT.MaxAmp > me->EVSE.MaxAmp) ? me->EVSE.MaxAmp :  me->LIMIT.MaxAmp;
	me->LIMIT.MaxAmp = (me->LIMIT.MaxAmp > me->EVCC.MaxAmp) ? me->EVCC.MaxAmp :  me->LIMIT.MaxAmp;	//if i bigger than use smaller value

	me->LIMIT.MaxPow = (me->LIMIT.MaxPow > me->EVSE.MaxPow) ? me->EVSE.MaxPow :  me->LIMIT.MaxPow;
	me->LIMIT.MaxPow = (me->LIMIT.MaxPow > me->EVCC.MaxPow) ? me->EVCC.MaxPow :  me->LIMIT.MaxPow;

	me->LIMIT.MinVolt = (me->USER.MinVolt < me->EVSE.MinVolt) ? me->EVSE.MinVolt :  me->USER.MinVolt;
	//LIMIT.MinVolt = (LIMIT.MinVolt < EVCC.MinVolt) ? EVCC.MinVolt :  LIMIT.MinVolt;

	me->LIMIT.MinAmp = (me->USER.MinAmp < me->EVSE.MinAmp) ? me->EVSE.MinAmp :  me->USER.MinAmp;
	//LIMIT.MinAmp = (LIMIT.MinAmp < EVCC.MinAmp) ? EVCC.MinAmp :  LIMIT.MinAmp;


}

uint8_t EVSE_set_volt_curr(evse* const me, uint32_t volt,uint32_t amp){
	if(volt>me->LIMIT.MaxVolt)	volt = me->LIMIT.MaxVolt;		//maximum value of voltage
	if(volt<me->LIMIT.MinVolt)	volt = me->LIMIT.MinVolt;		//minimum value of voltage
	if(amp>me->LIMIT.MaxAmp)	amp = me->LIMIT.MaxAmp;			//

	AppCanBusTask_setVoltCurr(&AppCanBusTask1,volt,amp);
	//AppCanBusTask_setVoltCurr(AppCanBusTask* const task, uint32_t volt,uint32_t curr)
	return 0;
}

void DC_Status_Update(evse* const me, struct dinDC_EVStatusType *ptr,char *ptrstring){
	//char *ptrstring ="PDq";
	//struct dinDC_EVStatusType *ptr = &CDRq_DCst;

	me->EVCC.RESSSOC = ptr->EVRESSSOC;
	me->EVCC.Ready = ptr->EVReady;
	me->EVCC.ErrorCode = ptr->EVErrorCode;
	xsprintfln_user(strTmp, "[Flex][%s] EVReady = %d",ptrstring,ptr->EVReady);
	xsprintfln_user(strTmp, "[Flex][%s] EVErrorCode = %d",ptrstring,ptr->EVErrorCode);
	xsprintfln_user(strTmp, "[Flex][%s] EVRESSSOC = %d",ptrstring,ptr->EVRESSSOC);
	if(ptr->EVRESSConditioning_isUsed){
		xsprintfln_user(strTmp, "[Flex][%s] EVRESSConditioning = %d",ptrstring,ptr->EVRESSConditioning);
		me->EVCC.RESSConditioning = ptr->EVRESSConditioning;
	}
	if(ptr->EVCabinConditioning_isUsed){
		xsprintfln_user(strTmp, "[Flex][%s] EVCabinConditioning = %d",ptrstring,ptr->EVCabinConditioning);
		me->EVCC.CabinConditioning = ptr->EVCabinConditioning;
	}
}

void DinV2GHeaderCheck(evse* const me){
	char strNameSpace[200] = {0};
	uint32_t* strNameSpacePtr;
	uint16_t strNameSpaceLen;

	projectExiConnector* exi = me->projectExiConnector;
	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;

	//#define dDecBody 					dinDocDec.V2G_Message.Body
	//#define dEncBody 					dinDocEnc.V2G_Message.Body

	if(exi->dinDocDec.V2G_Message.Header.Notification_isUsed){
		xsprintfln_user(strTmp, "[V2G][Header][Notification]FaultCode = %d",dDecHeader->Notification.FaultCode);
		if(dDecHeader->Notification.FaultMsg_isUsed){
			strNameSpaceLen = dDecHeader->Notification.FaultMsg.charactersLen;
			strNameSpacePtr = dDecHeader->Notification.FaultMsg.characters;
			for(uint8_t i = 0;i < strNameSpaceLen;i++ ){		//32 bit to 8 bit
				strNameSpace[i] = (char)(strNameSpacePtr[i]);
			}
			strNameSpace[strNameSpaceLen] = 0;
			McuXFormat_xsprintf(strTmp, "[V2G][Header][Notification]FaultMsg = %s\r\n", strNameSpace);/* should be AA 55  */
			addToTrace(strTmp);
		}
	}
	if(dDecHeader->Signature_isUsed){
		if(dDecHeader->Signature.Id_isUsed){
			strNameSpaceLen = dDecHeader->Signature.Id.charactersLen;
			strNameSpacePtr = dDecHeader->Signature.Id.characters;
			for(uint8_t i = 0;i < strNameSpaceLen;i++ ){
				strNameSpace[i] = (char)(strNameSpacePtr[i]);
			}
			strNameSpace[strNameSpaceLen] = 0;
			//const char *my_json = "{\"users\":[{\"name\":\"John\", \"age\":30}, {\"name\":\"Jane\", \"age\":25}]}";
			McuXFormat_xsprintf(strTmp, "[V2G][Header][Notification]SignatureID = %s\r\n", strNameSpace);
			addToTrace(strTmp);
		}
		if(dDecHeader->Signature.KeyInfo_isUsed){
			//dinDocDec.V2G_Message.Header.Signature.KeyInfo;
		}
		dDecHeader->Signature.Object.array;
		dDecHeader->Signature.Object.arrayLen;
		if(dDecHeader->Signature.KeyInfo_isUsed){
			dDecHeader->Signature.KeyInfo;
		}
		dDecHeader->Signature.SignatureValue.CONTENT.bytes;
		dDecHeader->Signature.SignatureValue.CONTENT.bytesLen;
		if(dDecHeader->Signature.SignatureValue.Id_isUsed){
			dDecHeader->Signature.SignatureValue.Id.characters;
			dDecHeader->Signature.SignatureValue.Id.charactersLen;
		}
		if(dDecHeader->Signature.SignedInfo.Id_isUsed){
			dDecHeader->Signature.SignedInfo.Id.characters;
			dDecHeader->Signature.SignedInfo.Id.charactersLen;
		}
	}
	if(exi->dinDocDec._warning_){
		xsprintfln_user(strTmp, "[V2G][Header]_warning_ = %d",exi->dinDocDec._warning_);
	}

}


void EVSESetStatus(evse* const me, struct dinDC_EVSEStatusType* ptr){

	ptr->NotificationMaxDelay = 0;
	ptr->EVSENotification = dinEVSENotificationType_None;		//none stop charging or re nego
	ptr->EVSEIsolationStatus = me->EVSE.IsolationStatus;//User_EVSEIsolationStatusCheck();
	ptr->EVSEIsolationStatus_isUsed = 1;
	ptr->EVSEStatusCode = me->EVSE.EVSEStatusCode;//not sure why , onlt PD is ready
}


/*** *****************************************/




//**************************************************************************************************************

int8_t decodeTCP(evse* const me){
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	//struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;


	int g_errn_app = 255;
	int g_errn_din = 255;
	if(!(me->tcp->rxdataLen > V2GTP_HEADER_SIZE)) return -1;
	xsprintfln_user(strTmp, "[EVSE] EVSE_State = %d",me->State);
	evseStateMachine_Reset_StateTimeout(me);			//it will go to each state

	showAsHex(me->tcp->rxdata, me->tcp->rxdataLen, "tcp_rxdata");
	routeDecoderInputData(me);
	me->tcp->rxdataLen = 0;
	g_errn_app = projectExiConnector_decode_appHandExiDocument(exi);			//corvert for aps and check
	xsprintfln_user(strTmp, "[EVSE] appHand g_errn = %u",g_errn_app);
	g_errn_din = projectExiConnector_decode_DinExiDocument(exi);			//convert for din and check
	xsprintfln_user(strTmp, "[EVSE] DinExi g_errn = %u",g_errn_din);

	if(g_errn_app == 0 && g_errn_din == 0){
		addToTrace("[EVSE] no both is 0");
	}
	if(g_errn_app != 0 && g_errn_din != 0){
		addToTrace("[EVSE] no both is not 0, data corrupted ");
	}

	if(g_errn_app == 0){
		if (exi->aphsDocDec.supportedAppProtocolReq_isUsed) {
			stateFunctionSAPRq(me);
		}
	}else if(g_errn_din == 0){
		//check header
		DinV2GHeaderCheck(me);		//check header already
		xsprintfln_user(strTmp, "[Flex]dDecHeader.SessionID = %llu",*((uint64_t*)dDecHeader->SessionID.bytes));


		//check body
		if (exi->dinDocDec.V2G_Message.Body.SessionSetupReq_isUsed) {					//new id session
			stateFunctionSSRq(me);
		}else if (dDecBody->ServiceDiscoveryReq_isUsed) {			//tell evcc we are DC supply only
			stateFunctionSDRq(me);
		}else if(dDecBody->ServicePaymentSelectionReq_isUsed) {	//tell evcc external payment
			stateFunctionSPSRq(me);
		}else if(dDecBody->ContractAuthenticationReq_isUsed){		//waiting agree from cloud
			stateFunctionCARq(me);
		}else if(dDecBody->ChargeParameterDiscoveryReq_isUsed){	//min and mx voltage and current from EV and reply secc Limit
			//stateC
			stateFunctionCPDRq(me);
		}else if(dDecBody->CableCheckReq_isUsed){					// close relay and check isolation loop
			stateFunctionCCRq(me);
		}else if (dDecBody->PowerDeliveryReq_isUsed) {			//wait for EV ready to charge
			stateFunctionPDRq(me);
		}else if(dDecBody->PreChargeReq_isUsed){				//push to target voltage got loop
			stateFunctionPCRq(me);
		}else if(dDecBody->CurrentDemandReq_isUsed){			//charging loop
			stateFunctionCDRq(me);
		}else if(dDecBody->WeldingDetectionReq_isUsed){		//discharge
			stateFunctionWDRq(me);
		}else if(dDecBody->SessionStopReq_isUsed){
			stateFunctionSSpRq(me);
		}else if(dDecBody->CertificateInstallationReq_isUsed){
			addToTrace("[ERROR] CertificateInstallationReq no function");
		}else if(dDecBody->CertificateUpdateReq_isUsed){
			addToTrace("[ERROR] CertificateUpdate no function");
		}else if(dDecBody->MeteringReceiptReq_isUsed){
			addToTrace("[ERROR] MeteringReceiptReq no function");
		}else{
			addToTrace("[ERROR] no State can be done");
		}
	}

	return 0;
}
//***********************************************************************************************************************
//evse
//***********************************************************************************************************************
//todo : use previous evse state is better than going state
void stateFunctionSAPRq(evse* const me){	//SupportedApplicationProtocolRequest
	projectExiConnector* exi = me->projectExiConnector;
	//struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	//struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	//struct dinSessionSetupResType* SessionSetupRes = &exi->dinDocDec.V2G_Message.Body.SessionSetupRes;

	//struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	//struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;
	//struct appHandAnonType_supportedAppProtocolReq* sAPPRq = &exi->aphsDocEnc.supportedAppProtocolReq;
	struct appHandAnonType_supportedAppProtocolRes* sAPPRs = &exi->aphsDocEnc.supportedAppProtocolRes;

	struct appHandAppProtocolType* array = exi->aphsDocDec.supportedAppProtocolReq.AppProtocol.array;


	uint8_t nDinSchemaID = 255; // invalid default value
	uint16_t nAppProtocol_ArrayLen;
	uint32_t* strNameSpacePtr;
	uint16_t strNameSpaceLen;
	char strNameSpace[200] ={0};
	char strNameSpacePrint[200];
	int8_t nSchemaId;
	uint32_t Major,Minor;
	uint8_t Priority;

	//char* ns0 = "urn:iso:15118:2:2010:MsgDef";	//15118-2-2016 (ISO2) started besides 15118-2-2013 (ISO1)
	//char* ns1 = "urn:din:70121:2012:MsgDef";

	addToTrace("[Flex][SAPR] Supported Application Protocol Req");
	nAppProtocol_ArrayLen = exi->aphsDocDec.supportedAppProtocolReq.AppProtocol.arrayLen;
	xsprintfln_user(strTmp, "[Flex][SAPR] The car supports %d schemas.",nAppProtocol_ArrayLen);

	for(uint8_t i=0;i<nAppProtocol_ArrayLen;i++){		//how many version
	  strNameSpacePtr = array[i].ProtocolNamespace.characters;		//pass the pointer
	  strNameSpaceLen = array[i].ProtocolNamespace.charactersLen;
	  nSchemaId 	  = array[i].SchemaID;
	  Major			  = array[i].VersionNumberMajor;
	  Minor			  = array[i].VersionNumberMinor;
	  Priority		  = array[i].Priority;

	  for(uint8_t j=0;j<strNameSpaceLen;j++){
		  strNameSpace[j] = (char)(strNameSpacePtr[j]);
	  }
	  strNameSpace[strNameSpaceLen] = 0;

	  //major,minor,Priority  dont care
	  xsprintfln_user(strTmp, "[Flex][SAPR] The NameSpace %s has SchemaID[%d],Major[%d],Minor[%d],Priority[%u]",strNameSpace,nSchemaId,Major,Minor,Priority);

#if (ISO15118 == 1)
	  if (McuUtility_strFind((uint8_t *)strNameSpace,(uint8_t *)":iso:15118:2") > 0){	//urn:iso:15118:2:2010:MsgDef //urn:iso:15118:2:2010//>urn:iso:15118:2:2013:MsgDef
		  nDinSchemaID = nSchemaId;		//we will this schemaid base on :iso:15118:2:2013
		  my_memcpy(strNameSpacePrint,strNameSpace,strNameSpaceLen);
		  strNameSpacePrint[strNameSpaceLen] = 0;
	  }
#else
	  if (McuUtility_strFind((uint8_t *)strNameSpace,(uint8_t *)":din:70121:") > 0){	//urn:iso:15118:2:2010:MsgDef	//urn:din:70121:2012:MsgDef //urn:iso:15118:2:2010//>urn:iso:15118:2:2013:MsgDef
		  nDinSchemaID = nSchemaId;		//we will this schemaid base on din:70121"
		  my_memcpy(strNameSpacePrint,strNameSpace,strNameSpaceLen);
		  strNameSpacePrint[strNameSpaceLen] = 0;
	  }
#endif

	}
	xsprintfln_user(strTmp, "[Flex][SAPR] We use the NameSpace %s has SchemaID [%d]",strNameSpacePrint,nSchemaId);
	//xsprintfln_user(strTmp, "[Flex][SAPR]responding to AppProtocolReq: send back AppProtocolRes SchemaID=%d",nDinSchemaID);

	init_appHandEXIDocument(&exi->aphsDocEnc);
	exi->aphsDocEnc.supportedAppProtocolRes_isUsed = 1u;
	if (nDinSchemaID<255 ){	//&& check with error e-button

		sAPPRs->SchemaID_isUsed = 1u;
		sAPPRs->SchemaID = nDinSchemaID;
		sAPPRs->ResponseCode = appHandresponseCodeType_OK_SuccessfulNegotiation;
	}else{
		sAPPRs->SchemaID_isUsed = 0u;
		sAPPRs->SchemaID = 0u;
		sAPPRs->ResponseCode = appHandresponseCodeType_Failed_NoNegotiation;
		addToTrace("[Flex][SAPR] Schema failed negotiation. No Suitable Schema");
	}
	encode_only_appHand(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][SAPR] Supported Application Protocol done");
	evse_enterState(me,EVSE_State_SSRq);	// jump to one
}




void stateFunctionSSRq(evse* const me){		//new SessioSetupRequest
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinSessionSetupResType* SSRs = &dDecBody->SessionSetupRes;


	//struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	addToTrace("[Flex][SSR] Session Setup Req Msg");
	//simulatedPresentVoltage = 0;


	my_memcpy(me->evccid,&dDecBody->SessionSetupReq.EVCCID.bytes,6);		//copy current evccid 8 byte starting with 2 letters
	xsprintfln_user(strTmp, "[Flex][SSR]received EVCCID = %x %x %x %x %x %x",me->evccid[0],me->evccid[1],me->evccid[2],me->evccid[3],me->evccid[4],me->evccid[5]);

	projectExiConnector_prepare_DinExiDocument(exi);//dinDocEnc.V2G_Message.Header.SessionID.bytesLen
	dEncBody->SessionSetupRes_isUsed = 1u;	// # EDa for Encode, Din, SessionSetupResponse
	init_dinSessionSetupResType(SSRs);

	//new session
	exi->sessionId.u64t++;
	dEncHeader->SessionID.bytesLen = SESSIONID_LEN;
	my_memcpy(dEncHeader->SessionID.bytes, exi->sessionId.u8t,SESSIONID_LEN);	//copy
	xsprintfln_user(strTmp, "[Flex][SSR]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	//new session id this need change if new session dinDocEnc.V2G_Message.Header.SessionID.bytesLen
	if((me->State != EVSE_State_SSRq) && (me->State != EVSE_State_SDRq)){
		SSRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][SSR] SequenceError EVSE_State=  %d",me->State);

	}else if(FunctionInternalCheck(me)){
		SSRs->ResponseCode = dinresponseCodeType_FAILED;
	}else{
		SSRs->ResponseCode = dinresponseCodeType_OK;//	or  OK_NewSessionEstablished
		evse_enterState(me, EVSE_State_SDRq);			//jump to 2
	}

	//EVSE_id
	SSRs->EVSEID.bytesLen = 6u;		// or 1
	my_memcpy(&SSRs->EVSEID.bytes,me->evseid,6u);		//send the EVSEID
	uint8_t* ptr = SSRs->EVSEID.bytes;
	xsprintfln_user(strTmp, "[Flex][SSR]SSRs.EVSEID.bytes = %x %x %x %x %x %x",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5]);
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][SSR]New Session Setup done");
}
void stateFunctionSDRq(evse* const me){ //2  //Service Discovery
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinServiceDiscoveryReqType* ServiceDiscoveryReq = &dDecBody->ServiceDiscoveryReq;
	struct dinServiceDiscoveryResType* ServiceDiscoveryRes = &dEncBody->ServiceDiscoveryRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	uint16_t len =200;
	char strNameSpace[len];
	addToTrace("[Flex][SDSRq] Service Discovery Response Msg");
	//ServiceScope Don’t care
	//ServiceCategory Don’t care
	//we dont care


	if(ServiceDiscoveryReq->ServiceCategory_isUsed){
		if(ServiceDiscoveryReq->ServiceCategory == 0){
			addToTrace("[Flex][SDSRq] ServiceCategory = EVCharging");
		}else if(ServiceDiscoveryReq->ServiceCategory == 1){
			addToTrace("[Flex][SDSRq] ServiceCategory = Internet");
		}else if(ServiceDiscoveryReq->ServiceCategory == 2){
			addToTrace("[Flex][SDSRq] ServiceCategory = ContractCertificate");
		}else if(ServiceDiscoveryReq->ServiceCategory == 3){
			addToTrace("[Flex][SDSRq] ServiceCategory = OtherCustom");
		}

		len = ServiceDiscoveryReq->ServiceScope.charactersLen < len ? ServiceDiscoveryReq->ServiceScope.charactersLen : len;
		for(uint8_t j=0;j< len;j++){
		  strNameSpace[j] = (char)(ServiceDiscoveryReq->ServiceScope.characters[j]);
		}
		xsprintfln_user(strTmp, "[Flex][SDSRq]ServiceScope = %s",strNameSpace);
	}

	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->ServiceDiscoveryRes_isUsed = 1u;	// EDb for Encode, Din, ServiceDiscoveryResponse
	init_dinServiceDiscoveryResType(ServiceDiscoveryRes);

	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][SDSRs]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	if((me->State != EVSE_State_SDRq) && (me->State != EVSE_State_SPSRq)){
		ServiceDiscoveryRes->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][SDSRs] SequenceError EVSE_State=  %d",me->State);
	}else if(SessionIdCheckRq(me)){
		ServiceDiscoveryRes->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
		addToTrace("[Flex][SDR] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		ServiceDiscoveryRes->ResponseCode = dinresponseCodeType_FAILED;
		addToTrace("[Flex][SDSRs] InernalCheckError");
	}else{
		ServiceDiscoveryRes->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me,EVSE_State_SPSRq); //jump to 3
	}

	ServiceDiscoveryRes->PaymentOptions.PaymentOption.arrayLen = 1;
	ServiceDiscoveryRes->PaymentOptions.PaymentOption.array[0] = dinpaymentOptionType_ExternalPayment;
	ServiceDiscoveryRes->ChargeService.ServiceTag.ServiceID = 1;
	ServiceDiscoveryRes->ChargeService.ServiceTag.ServiceCategory = dinserviceCategoryType_EVCharging;
	ServiceDiscoveryRes->ChargeService.ServiceTag.ServiceName.charactersLen = 9;
	ServiceDiscoveryRes->ChargeService.EnergyTransferType = dinEVSESupportedEnergyTransferType_DC_extended;
	ServiceDiscoveryRes->ChargeService.FreeService = 0u;
	my_memcpy(ServiceDiscoveryRes->ChargeService.ServiceTag.ServiceName.characters, "OPPCharge", 9);

	//here got alot of function
	//Don’t care
	//ServiceList Don’t care. Refer to subclause 8.5.2.2
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][SDSRs] Service Discovery done");
}
void stateFunctionSPSRq(evse* const me){ //3		//Selected Payment Option Req //nothing to do
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinServicePaymentSelectionReqType* SPSRq = &dDecBody->ServicePaymentSelectionReq;
	struct dinServicePaymentSelectionResType* SPSRs = &dEncBody->ServicePaymentSelectionRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;


	dinpaymentOptionType SPSRq_SelectedPaymentOption;///* not paying per car */
	uint16_t SPSRq_ServiceID;///* todo: what ever this means. The Ioniq uses 1. */
	uint16_t SPSRq_arrayLen;
	int16_t ParameterSetID;

	addToTrace("[Flex][SPSR] Selected Payment Option Req Msg");

	SPSRq_SelectedPaymentOption = SPSRq->SelectedPaymentOption;///* not paying per car */ //"ExternalPayment" by other system not by car
	xsprintfln_user(strTmp, "[Flex][SPSR] SPSRq_SelectedPaymentOption = %d", SPSRq_SelectedPaymentOption);
		//SPSRq.SelectedServiceList.SelectedService
	SPSRq_arrayLen = SPSRq->SelectedServiceList.SelectedService.arrayLen;
	for(uint16_t i;i<SPSRq_arrayLen;i++){
		if(SPSRq->SelectedServiceList.SelectedService.array[i].ParameterSetID_isUsed){
			ParameterSetID = SPSRq->SelectedServiceList.SelectedService.array[i].ParameterSetID;
			SPSRq_ServiceID = SPSRq->SelectedServiceList.SelectedService.array[i].ServiceID;
			xsprintfln_user(strTmp, "[Flex][SPSR] SPSRq_SelectedPaymentOption = %d", ParameterSetID);
			xsprintfln_user(strTmp, "[Flex][SPSR] SPSRq_ServiceID = %d", SPSRq_ServiceID);
		}
	}
	//return msg
	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->ServicePaymentSelectionRes_isUsed = 1u;	// # EDc for Encode, Din, ServicePaymentSelectionResponse
	init_dinServicePaymentSelectionResType(SPSRs);			//only one variable

	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes, dDecHeader->SessionID.bytes, dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][SPSR]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	if((me->State != EVSE_State_SPSRq) && (me->State != EVSE_State_CARq)){
		SPSRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][SPSR] SequenceError EVSE_State=  %d",me->State);
	}else if(SessionIdCheckRq(me)){
		SPSRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
		addToTrace("[Flex][SPSR] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		SPSRs->ResponseCode = dinresponseCodeType_FAILED;

	}else{
		SPSRs->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me, EVSE_State_CARq);
	}

	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][SPSR]ServicePayment done");
}
void stateFunctionCARq(evse* const me){		//ContractAuthenticationReq_isUsed, do nothing also
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	//struct dinContractAuthenticationReqType* CARq = &dDecBody->ContractAuthenticationReq;
	struct dinContractAuthenticationResType* CARs = &dEncBody->ContractAuthenticationRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;


	addToTrace("[Flex][CAR] Contract Authentication Req Msg");
	//id and GenChallenge Dont care
	//reply new msg dEncBody	////reply new msg
	xsprintfln_user(strTmp, "[Flex][CAR]dDecHeader.SessionID = %llu",*((uint64_t*)dDecHeader->SessionID.bytes));

	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->ContractAuthenticationRes_isUsed = 1u;		//# EDl for Encode, Din, ContractAuthenticationResponse
	init_dinContractAuthenticationResType(CARs);

	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][CAR]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	if((me->State != EVSE_State_CARq) && (me->State != EVSE_State_CPDRq)){
		CARs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][CAR] SequenceError EVSE_State=  %d",me->State);
	}else if(SessionIdCheckRq(me)){
		CARs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
		addToTrace("[Flex][CAR] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		CARs->ResponseCode = dinresponseCodeType_FAILED;
	}else{
		CARs->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me,EVSE_State_CPDRq);	 //# todo: not clear, what is specified in DIN
	}

	if(CARs->ResponseCode == dinresponseCodeType_OK){
		CARs->EVSEProcessing = dinEVSEProcessingType_Finished;	//nothing to check
	}else{
		CARs->EVSEProcessing = dinEVSEProcessingType_Ongoing;	//nothing to check
	}
	addToTrace("[Flex][CAR]responding for ContractAuthentication:");

	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][CAR] ContractAuthentication done");

}
void stateFunctionCPDRq(evse* const me){		//Charge Parameter Discovery			//PD
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinChargeParameterDiscoveryReqType* CPDRq = &dDecBody->ChargeParameterDiscoveryReq;
	struct dinChargeParameterDiscoveryResType* CPDRs = &dEncBody->ChargeParameterDiscoveryRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	struct dinDC_EVChargeParameterType* DCPara = &CPDRq->DC_EVChargeParameter;


	int32_t Value;
	int8_t Multiplier;

	addToTrace("[Flex][CPD] Charge Parameter Discovery Req Msg");

	//EVReady = True (Dont care)
	//EVCabinConditioning Dont care
	//EVRESSConditioning dont care
	//EVErrorCode don’t care(default NO_ERROR)
	//EVRESSSOC available SOC,mandatory use in CurrentDemandReq
	//BulkChargingComplete dont care
	//ChargingComple dont care
	//RemainingTimeToFullSoc dont care
	//RemainingTimeToBulkSoC dont care

	uint8_t CPDRq_EVRETT = CPDRq->EVRequestedEnergyTransferType;			//normally is 3
	char arr[][30]={
		"single_phase_core(0)",
		"three_phase_core(1)",
		"DC_core(2)",
		"DC_extended(3)",
		"DC_combo_core(4)",
		"DC_unique(5)"};

	xsprintfln_user(strTmp, "[Flex][CPDq] EVCC RequestedEnergyTransferType = %s",arr[CPDRq_EVRETT]);

	//DC_Status_Update(me,&CPDRq->DC_EVChargeParameter.DC_EVStatus,"CPDq");
	//Limit Amp
	if(DCPara->EVMaximumCurrentLimit.Unit_isUsed && DCPara->EVMaximumPowerLimit.Unit_isUsed && DCPara->EVMaximumVoltageLimit.Unit_isUsed && DCPara->FullSOC_isUsed) {

	}
	me->EVCC.MaxAmp = combineValueAndMultiplier(DCPara->EVMaximumCurrentLimit.Value,DCPara->EVMaximumCurrentLimit.Multiplier);
	me->EVCC.MaxPow = combineValueAndMultiplier(DCPara->EVMaximumPowerLimit.Value,DCPara->EVMaximumPowerLimit.Multiplier);
	me->EVCC.MaxVolt = combineValueAndMultiplier(DCPara->EVMaximumVoltageLimit.Value,DCPara->EVMaximumVoltageLimit.Multiplier);
	me->EVCC.FullSOC = DCPara->FullSOC;
	me->EVCC.BulkSOC = DCPara->BulkSOC;
	me->EVCC.EnergyCapacity = combineValueAndMultiplier(DCPara->EVEnergyCapacity.Value,DCPara->EVEnergyCapacity.Multiplier);
	me->EVCC.EnergyRequest = combineValueAndMultiplier(DCPara->EVEnergyRequest.Value,DCPara->EVEnergyRequest.Multiplier);

	EVSE_set_EVSE_Limit(me);		//update VPA limit

	//return msg
	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->ChargeParameterDiscoveryRes_isUsed = 1u;		//# EDf for Encode, Din, ChargeParameterDiscoveryRes
	init_dinChargeParameterDiscoveryResType(CPDRs);

	//##############################################################################################
	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][CPD]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));



	//##############################################################################################
	if((me->State != EVSE_State_CPDRq) && (me->State != EVSE_State_CCRq)){
		CPDRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][CPD] SequenceError EVSE_State=  %d",me->State);
	}else if(SessionIdCheckRq(me)){
		CPDRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
		addToTrace("[Flex][CPD] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		CPDRs->ResponseCode = dinresponseCodeType_FAILED;
		addToTrace("[Flex][CPD] InernalCheckError");
	}

	if(CPDRq_EVRETT != dinEVSESupportedEnergyTransferType_DC_extended){	//not DC request
		CPDRs->ResponseCode = dinresponseCodeType_FAILED_WrongChargeParameter;
		addToTrace("[Flex][CPD] dinresponseCodeType_FAILED_WrongChargeParameter");
	}else{
		CPDRs->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me, EVSE_State_CCRq);
	}

	if(CPDRs->ResponseCode == dinresponseCodeType_OK){
		CPDRs->EVSEProcessing = dinEVSEProcessingType_Finished;	//nothing to check
	}else{
		CPDRs->EVSEProcessing = dinEVSEProcessingType_Ongoing;	//nothing to check
	}


	//TODO it stop here
	//EVSE Status
	//CPDRs->DC_EVSEChargeParameter.DC_EVSEStatus;



	//##############################################################################################
	// tell the car what is our evse limit

	CPDRs->DC_EVSEChargeParameter_isUsed = 1;		//
	CPDRs->AC_EVSEChargeParameter_isUsed = 0;	//we don't supply AC power
	CPDRs->EVSEChargeParameter_isUsed =0;
	CPDRs->SAScheduleList_isUsed = 0;
	CPDRs->SASchedules_isUsed = 0;


	xsprintfln_user(strTmp, "[Flex][CPD]My EVSEMaxPowLim = %u",me->EVSE.MaxPow);
	xsprintfln_user(strTmp, "[Flex][CPD]My EVSEMaxCurrLim = %u",me->EVSE.MaxAmp);
	xsprintfln_user(strTmp, "[Flex][CPD]My EVSEMinCurrLim = %u",me->EVSE.MinAmp);
	xsprintfln_user(strTmp, "[Flex][CPD]My EVSEMaxVoltLim = %u",me->EVSE.MaxVolt);
	xsprintfln_user(strTmp, "[Flex][CPD]My EVSEMinVoltLim = %u",me->EVSE.MinVolt);

	//EVSE maximum power limit
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumPowerLimit.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumPowerLimit.Unit = dinunitSymbolType_W;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumPowerLimit.Multiplier = 1;		//normally in w issue//int8 only
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumPowerLimit.Value = me->EVSE.MaxPow;

	//EVSE maximum current limit


	OutputValueAndMultiplier(me->EVSE.MaxAmp,&Value,&Multiplier);
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit.Unit = dinunitSymbolType_A;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit.Multiplier = Multiplier;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit.Value = Value;		//A max


	//EVSE minimum current limit
	OutputValueAndMultiplier(me->EVSE.MaxAmp,&Value,&Multiplier);
	CPDRs->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit.Unit = dinunitSymbolType_A;
	CPDRs->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit.Multiplier = Multiplier;
	CPDRs->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit.Value = Value;		//A min

	//EVSE maximum voltage limit
	OutputValueAndMultiplier(me->EVSE.MaxVolt,&Value,&Multiplier);
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Unit = dinunitSymbolType_V;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Multiplier = Multiplier;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Value = Value;		//V max

	//EVSE maximum voltage limit
	OutputValueAndMultiplier(me->EVSE.MinVolt,&Value,&Multiplier);
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Unit = dinunitSymbolType_V;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Multiplier = Multiplier;
	CPDRs->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit.Value = Value;		//V max

	//Amp ripple
	Multiplier = 0;
	OutputValueAndMultiplier(me->EVSE.MinVolt,&Value,&Multiplier);
	CPDRs->DC_EVSEChargeParameter.EVSEPeakCurrentRipple.Unit_isUsed = 1;
	CPDRs->DC_EVSEChargeParameter.EVSEPeakCurrentRipple.Unit = dinunitSymbolType_A;
	CPDRs->DC_EVSEChargeParameter.EVSEPeakCurrentRipple.Multiplier = Multiplier;
	CPDRs->DC_EVSEChargeParameter.EVSEPeakCurrentRipple.Value = 6;		//V max

	//CPDRs->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = 0;		//

	EVSESetStatus(me,&CPDRs->DC_EVSEChargeParameter.DC_EVSEStatus);
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][CPD] Charge Parameter Discovery done");
}
void stateFunctionCCRq(evse* const me){			//	Cable Check , isolation check loop if cable check not done yet
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinCableCheckReqType* CCRq = &dDecBody->CableCheckReq;
	struct dinCableCheckResType* CCRs = &dEncBody->CableCheckRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	struct dinDC_EVSEStatusType* DCStatus = &CCRs->DC_EVSEStatus;


	addToTrace("[Flex][CC] Cable Check Req Msg");

	DC_Status_Update(me, &CCRq->DC_EVStatus,"CCRq");		//update
	//xsprintfln_user(strTmp, "[Flex][CC] EVReady = %d",CCRq->DC_EVStatus.EVReady);	//DC_EVStatus Dont care

	//##############################################################################################
	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->CableCheckRes_isUsed = 1u;
	init_dinCableCheckResType(CCRs);

	//session ID udpdate
	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][CC]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	//EVSE_State = 10000;		//make it jam
	if((me->State != EVSE_State_CCRq) && (me->State != EVSE_State_PCRq)){
		CCRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][CC] SequenceError EVSE_State=  %d",me->State);
	}else if(SessionIdCheckRq(me)){
    	CCRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
    	addToTrace("[Flex][CC] UnknownSession");

	}else if(FunctionInternalCheck(me)){
		CCRs->ResponseCode = dinresponseCodeType_FAILED;
		addToTrace("[Flex][CC] Internal Error");
	}else{
		CCRs->ResponseCode = dinresponseCodeType_OK;
		addToTrace("[Flex][CC] dinresponseCodeType_OK");
		evse_enterState(me, EVSE_State_PCRq);
	}


	if(!User_EVSEIsolationStatusCheck(me)){		//main reply check isoalation
		DCStatus->EVSEIsolationStatus = dinisolationLevelType_Invalid;	//EVCC care		// 0 or 1						//EVCC dont care
		DCStatus->EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive;	//EVCC care  //EVSE_IsolationMonitoringActive
		CCRs->EVSEProcessing = dinEVSEProcessingType_Ongoing;		// EVCC care, it loop back
		addToTrace("[Flex][CC] dinEVSEProcessingType_Ongoing");
	}else{
		DCStatus->EVSEIsolationStatus = dinisolationLevelType_Valid;	//EVCC care		// 0 or 1						//EVCC dont care
		DCStatus->EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready;	//EVCC care  //EVSE_IsolationMonitoringActive
		CCRs->EVSEProcessing = dinEVSEProcessingType_Finished;		//EVCC care, it will jump to PreCharge
		addToTrace("[Flex][CC] dinEVSEProcessingType_Finished");
	}

	EVSESetStatus(me,&CCRs->DC_EVSEStatus);
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][CC] CableCheck done");
}
void stateFunctionPCRq(evse* const me){		//PreCharge Req push the target voltage before on the relay
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinPreChargeReqType* PCRq = &dDecBody->PreChargeReq;
	struct dinPreChargeResType* PCRs = &dEncBody->PreChargeRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	//struct dinDC_EVSEStatusType* DCStatus = &CCRs->DC_EVSEStatus;

	int32_t Value;
	int8_t Multiplier;

	addToTrace("[Flex][PCq] PreCharge Req Msg");

	//DC_EVStatus Don’t care.
	//EVTargetVoltage Target Voltage requested by EV.
	//EVTargetCurrent Current demanded by EV set to 0, but don’t care.

	DC_Status_Update(me, &PCRq->DC_EVStatus,"PCRq");		//update

	me->EVCC.TargetVolt = combineValueAndMultiplier((int32_t)PCRq->EVTargetVoltage.Value, PCRq->EVTargetVoltage.Multiplier);
	me->EVCC.TargetAmp = combineValueAndMultiplier((int32_t)PCRq->EVTargetCurrent.Value, PCRq->EVTargetCurrent.Multiplier);
	xsprintfln_user(strTmp, "[Flex][PC] EVCCTargetVoltage = %ld",me->EVCC.TargetVolt);
	xsprintfln_user(strTmp, "[Flex][PC] EVCCTargetCurrent = %ld",me->EVCC.TargetAmp);

	//EV DC status
	EVSE_set_volt_curr(me,(uint32_t)me->EVCC.TargetVolt,(uint32_t)me->EVCC.TargetAmp);

	//return new msg
	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->PreChargeRes_isUsed = 1u;
	init_dinPreChargeResType(PCRs);

	//copy session id
	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][PCq]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	//check condition
	if((me->State != EVSE_State_PCRq) && (me->State != EVSE_State_PDRq)){
		PCRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][PC] SequenceError EVSE_State=  %d",me->State);
    }else if(SessionIdCheckRq(me)){
    	PCRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
    	addToTrace("[Flex][PC] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		PCRs->ResponseCode = dinresponseCodeType_FAILED;
		addToTrace("[Flex][PC] Internal Error");
	}else{
		PCRs->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me, EVSE_State_PDRq);
	}
	//testModuleForPreCharge();
	//only 5 status

	//only 4 status
	OutputValueAndMultiplier(me->EVSE.NowVolt,&Value,&Multiplier);

	PCRs->EVSEPresentVoltage.Multiplier = Multiplier;		//no need this
	PCRs->EVSEPresentVoltage.Value = Value;			//22862 //or take can bus or ADC voltage
	PCRs->EVSEPresentVoltage.Unit = dinunitSymbolType_V;
	PCRs->EVSEPresentVoltage.Unit_isUsed = 1;	//-2// it didnt show in the log

	xsprintfln_user(strTmp, "[Flex][PC] EVSENowVolt = %d",me->EVSE.NowVolt);

	EVSESetStatus(me,&PCRs->DC_EVSEStatus);
	encode_only_Din(me);
	tcp_transmit(me->tcp);

	xsprintfln_user(strTmp, "[Flex][PC] PreCharging done");

}
void stateFunctionPDRq(evse* const me){	//Power Delivery Req	true to start charging, false to stop charging //control relay  //no off relay
		addToTrace("[Flex][PDq] Power Delivery Req Msg");
		//ReadyToChargeState 	true to start charging,false to stop charging
		//ChargingProfile Dont care
		//DC_EVPowerDeliveryParameter Dont care
		//PDRq.DC_EVPowerDeliveryParameter.DC_EVStatus.EVReady;
		//PDRq.DC_EVPowerDeliveryParameter.DC_EVStatus.EVErrorCode;
		projectExiConnector* exi = me->projectExiConnector;
		struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
		struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
		struct dinPowerDeliveryReqType* PDRq = &dDecBody->PowerDeliveryReq;
		struct dinPowerDeliveryResType* PDRs = &dEncBody->PowerDeliveryRes;

		struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
		struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;
		uint16_t arrayLen;
		uint32_t MaxPower;
		uint32_t EntryStart;
		//exi->PDRq_DCpara_st
		//#define CPDRq_DCpara_st 			CPDRq_DCpara.DC_EVStatus CPDRq.DC_EVChargeParameter

		//PDRq->DC_EVPowerDeliveryParameter.DC_EVStatus



		me->EVCC.ReadyToChargeState = PDRq->ReadyToChargeState;	//stop charge or run charge
	  //TODO
	  //

	  //if(PDRq->DC_EVPowerDeliveryParameter_isUsed){
	  //	  DC_Status_Update(me,&PDRq->DC_EVPowerDeliveryParameter.DC_EVStatus,"PDRq");
	  //}

	  //new Msg to response
	  projectExiConnector_prepare_DinExiDocument(exi);
	  dEncBody->PowerDeliveryRes_isUsed = 1u;
	  init_dinPowerDeliveryResType(PDRs);

	  //copy session id
	  dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	  my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	  xsprintfln_user(strTmp, "[Flex][PDq]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	  if((me->State != EVSE_State_PDRq) && (me->State != EVSE_State_PCRq) && (me->State != EVSE_State_CDRq)){
		  PDRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		  xsprintfln_user(strTmp, "[Flex][PD] SequenceError EVSE_State=  %d",me->State);
      }else if(SessionIdCheckRq(me)){		//check sessionID  if same
    	  PDRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
    	  addToTrace("[Flex][PD] UnknownSession");
	  }else if(FunctionInternalCheck(me)){
		  PDRs->ResponseCode = dinresponseCodeType_FAILED;
		  addToTrace("[Flex][PD] Internal Error");
	  }else{
		  PDRs->ResponseCode = dinresponseCodeType_OK;
	  }

	  EVSESetStatus(me, &PDRs->DC_EVSEStatus);
	  encode_only_Din(me);
	  tcp_transmit(me->tcp);

	  if(me->EVCC.ReadyToChargeState){		//start to charge
		  addToTrace("[Flex][PD] Close Contractor(connected)");
		  TimeEvent_arm(me->TimerContractor,250, 0,Contractor_On);
		  evse_enterState(me, EVSE_State_CDRq);
	  }else{							//stop to charge
		  addToTrace("[Flex][PD] Open Contractor (Disconnect)");
		  EVSE_set_volt_curr(me,0,0);
		  TimeEvent_arm(me->TimerContractor,250, 0,Contractor_Off);
	  }
	  addToTrace("[Flex][PD] PowerDelivery done");
}
void stateFunctionCDRq(evse* const me){	//Current Demand
	addToTrace("[Flex][CD] Current Demand Req Msg");

	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinCurrentDemandReqType* CDRq = &dDecBody->CurrentDemandReq;
	struct dinCurrentDemandResType* CDRs = &dEncBody->CurrentDemandRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;
	//DC_EVStatus Only EVRESSSOC is used, all other parameters may not be handeled by EVSE. Refer to subclause 8.5.4.2
	//EVTargetCurrent BMS current requested by the EV.
	//EVMaximumVoltageLimit Maximum voltage allowed by the EV.
	//EVMaximumCurrentLimit Maximum current allowed by the EV.
	//EVMaximumPowerLimit Maximum power allowed by the EV.
	//BulkChargingComplete Don’t care
	//ChargingComple Don’t care
	//RemainingTimeToFullSoc Don’t care
	//RemainingTimeToBulkSoC Don’t care
	//EVTargetVoltage Voltage requested for charging

	int32_t Value;
	int8_t Multiplier;

	//******************************************************
	//DC status update
	DC_Status_Update(me, &CDRq->DC_EVStatus,"CDRq");


	//******************************************************
	//V,A,P limit



	//if(CDRq->EVMaximumVoltageLimit.Unit_isUsed || CDRq->EVMaximumCurrentLimit.Unit_isUsed || CDRq->EVMaximumPowerLimit.Unit_isUsed || CDRq->BulkChargingComplete_isUsed){
	//}
	//Max Volt
	//me->EVCC.MaxVolt = combineValueAndMultiplier((int32_t)CDRq->EVMaximumVoltageLimit.Value, CDRq->EVMaximumVoltageLimit.Multiplier);
	//me->EVCC.MaxAmp = combineValueAndMultiplier((int32_t)CDRq->EVMaximumCurrentLimit.Value, CDRq->EVMaximumCurrentLimit.Multiplier);
	//me->EVCC.MaxPow = combineValueAndMultiplier((int32_t)CDRq->EVMaximumPowerLimit.Value, CDRq->EVMaximumPowerLimit.Multiplier);
	//EVSE_set_EVSE_Limit(me);

	//xsprintfln_user(strTmp, "[Flex][CDRq] EVCCMaxVoltLim =  %d",me->EVCC.MaxVolt);
	//xsprintfln_user(strTmp, "[Flex][CDRq] EVCCMaxCurrLim =  %d",me->EVCC.MaxAmp);
	//xsprintfln_user(strTmp, "[Flex][CDRq] EVCCMaxPowLim =  %d",me->EVCC.MaxPow);
	//update VPA limit

	//boolean approx. 80% SOC
	me->EVCC.BulkChargingComplete = CDRq->BulkChargingComplete;
	xsprintfln_user(strTmp, "[Flex][CDRq] BulkChargingComplete =  %d",CDRq->BulkChargingComplete);	//dont care


	//******************************************************
	//voltage and current
	//important
	me->EVCC.TargetVolt = combineValueAndMultiplier((int32_t)CDRq->EVTargetVoltage.Value, CDRq->EVTargetVoltage.Multiplier);
	me->EVCC.TargetAmp = combineValueAndMultiplier((int32_t)CDRq->EVTargetCurrent.Value, CDRq->EVTargetCurrent.Multiplier);
	xsprintfln_user(strTmp, "[Flex][CDRq] EVCCTargetVoltage =  %ld",me->EVCC.TargetVolt);
	xsprintfln_user(strTmp, "[Flex][CDRq] EVCCTargetCurrent =  %ld",me->EVCC.TargetAmp);

	EVSE_set_volt_curr(me,(uint32_t)me->EVCC.TargetVolt,(uint32_t)me->EVCC.TargetAmp);		//send voltage level

	//******************************************************
	//100% soc 1 or 0
	if(CDRq->RemainingTimeToBulkSoC_isUsed && CDRq->RemainingTimeToFullSoC_isUsed){	//time to 80% //time to 100%

	}

	me->EVCC.ChargingComplete = CDRq->ChargingComplete;
	me->EVCC.RemainingTimeToBulkSoC = combineValueAndMultiplier((int32_t)CDRq->RemainingTimeToFullSoC.Value, CDRq->RemainingTimeToFullSoC.Multiplier);
	me->EVCC.RemainingTimeToFullSoC = combineValueAndMultiplier((int32_t)CDRq->RemainingTimeToFullSoC.Value, CDRq->RemainingTimeToFullSoC.Multiplier);

	xsprintfln_user(strTmp, "[Flex][CDRq] ChargingComplete =  %d",CDRq->ChargingComplete);		//dont care
	xsprintfln_user(strTmp, "[Flex][CDRq] RemainingTimeToBulkSoC =  %d",me->EVCC.RemainingTimeToBulkSoC);
	xsprintfln_user(strTmp, "[Flex][CDRq] RemainingTimeToFullSoC =  %d",me->EVCC.RemainingTimeToFullSoC);

	//reply new msg
	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->CurrentDemandRes_isUsed = 1u;		//# EDi for Encode, Din, CurrentDemandRes
	init_dinCurrentDemandResType(CDRs);

	//copy session
	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][CDs]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	if((me->State != EVSE_State_CDRq) && (me->State != EVSE_State_PDRq)){
		CDRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][CDRq] SequenceError EVSE_State=  %d",me->State);

    }else if(SessionIdCheckRq(me)){
    	CDRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
    	addToTrace("[Flex][CDRq] UnknownSession");
	}else if(FunctionInternalCheck(me)){
		CDRs->ResponseCode = dinresponseCodeType_FAILED;
		addToTrace("[Flex][CDRq] Internal Error");
	}else{
		CDRs->ResponseCode = dinresponseCodeType_OK;
		evse_enterState(me, EVSE_State_CDRq);
	}

	xsprintfln_user(strTmp, "[Flex][CD] EVSENowVolt =  %d",me->EVSE.NowVolt);
	xsprintfln_user(strTmp, "[Flex][CDRq] EVSENowAmp =  %d",me->EVSE.NowAmp);

	//current V and I
	Multiplier = 0;
	OutputValueAndMultiplier(me->EVSE.NowVolt,&Value,&Multiplier);
	CDRs->EVSEPresentVoltage.Multiplier = Multiplier;//-2
	CDRs->EVSEPresentVoltage.Value = Value;//22898		//this will keep changing
	CDRs->EVSEPresentVoltage.Unit = dinunitSymbolType_V;

	Multiplier = 0;
	OutputValueAndMultiplier(me->EVSE.NowAmp,&Value,&Multiplier);
	CDRs->EVSEPresentCurrent.Multiplier = Multiplier;//-3
	CDRs->EVSEPresentCurrent.Value = Value;//5			//this will keep changing
	CDRs->EVSEPresentCurrent.Unit = dinunitSymbolType_A;



	EVSESetStatus(me,&CDRs->DC_EVSEStatus);
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][CD] CurrentDemand done");

}
void stateFunctionWDRq(evse* const me){		//last only can unplug under 60v
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinWeldingDetectionReqType* WDRq = &dDecBody->WeldingDetectionReq;
	struct dinWeldingDetectionResType* WDRs = &dEncBody->WeldingDetectionRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;


	addToTrace("[Flex][WD] Welding Detection Req Msg");
	DC_Status_Update(me, &WDRq->DC_EVStatus,"WDDq");
	//# todo: check the request content, and fill response parameters

	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->WeldingDetectionRes_isUsed = 1u;		//# EDj for Encode, Din, WeldingDetectionRes
	init_dinWeldingDetectionResType(WDRs);

	dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][WD]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	if((me->State != EVSE_State_WDRq) && (me->State != EVSE_State_PDRq)){
		WDRs->ResponseCode = dinresponseCodeType_FAILED_SequenceError;
		xsprintfln_user(strTmp, "[Flex][WD] SequenceError EVSE_State=  %d",me->State);
#if (SenssionIDCheck == 1)
	}else if(SessionIdCheckRq(me)){
		WDRs->ResponseCode = dinresponseCodeType_FAILED_UnknownSession;
		addToTrace("[Flex][WD] UnknownSession");
#endif
	}else{
		//WDRs.ResponseCode = dinresponseCodeType_FAILED;	//check sessionID  if same
		//addToTrace("[Flex][WD] ");
	}


	WDRs->EVSEPresentVoltage.Multiplier = me->EVSE.NowVoltX;
	WDRs->EVSEPresentVoltage.Value = me->EVSE.NowVolt;
	WDRs->EVSEPresentVoltage.Unit = dinunitSymbolType_V;
	WDRs->EVSEPresentVoltage.Unit_isUsed = 1;

	EVSESetStatus(me, &WDRs->DC_EVSEStatus);
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][WD] WeldingDetection done");
	evse_enterState(me,EVSE_State_WDRq);
}
void stateFunctionSSpRq(evse* const me){
	projectExiConnector* exi = me->projectExiConnector;
	struct dinBodyType* dDecBody = &exi->dinDocDec.V2G_Message.Body;
	struct dinBodyType* dEncBody = &exi->dinDocEnc.V2G_Message.Body;
	struct dinSessionStopReqType* SSpRq = &dDecBody->WeldingDetectionReq;
	struct dinSessionStopResType* SSpRs = &dEncBody->WeldingDetectionRes;

	struct dinMessageHeaderType* dDecHeader = &exi->dinDocDec.V2G_Message.Header;
	struct dinMessageHeaderType* dEncHeader = &exi->dinDocEnc.V2G_Message.Header;

	//#####################################################################

	addToTrace("[Flex][SS] Session Stop Req is Used");


	projectExiConnector_prepare_DinExiDocument(exi);
	dEncBody->SessionStopRes_isUsed = 1u;		//# EDk for Encode, Din, SessionStopRes
	init_dinSessionStopResType(SSpRs);		// send nothing

	//copy session id
    dEncHeader->SessionID.bytesLen = dDecHeader->SessionID.bytesLen;
	my_memcpy(dEncHeader->SessionID.bytes,dDecHeader->SessionID.bytes,dDecHeader->SessionID.bytesLen);	//copy
	xsprintfln_user(strTmp, "[Flex][SS]dEncHeader.SessionID = %llu",*((uint64_t*)dEncHeader->SessionID.bytes));

	SSpRs->ResponseCode = dinresponseCodeType_OK;	//no other to do
	encode_only_Din(me);
	tcp_transmit(me->tcp);
	addToTrace("[Flex][SS] SessionStop done");		//here need to stop all reset all
	evse_enterState(me,EVSE_State_SAPRq);
}

//***********************************************************************************************************************
//evse
//***********************************************************************************************************************

//state change


uint8_t evse_isTooLong(evse* const me){
    // The timeout handling function.
    return (HAL_GetTick() - me->cyclesInState > (5000)); // 100*33ms=3.3s (100)
}
/******* The statemachine dispatcher *******************/

void evse_enterState(evse* const me,uint16_t n){
	xsprintfln_user(strTmp, "[EVSE]from %d entering %d", me->State, n);

	me->State= n;
	me->cyclesInState = HAL_GetTick();		//reset timeout
}

void evseStateMachine_Reset_StateTimeout(evse* const me) {
	me->cyclesInState = HAL_GetTick();		//reset timeout
}
void evseStateMachine_Stop(evse* const me) {
	addToTrace("[EVSE]Stop fsmEvse");
	me->State = EVSE_State_SAPRq;	//0
	me->cyclesInState = HAL_GetTick();			//reset timeout
	EVSE_set_volt_curr(me,0,0);
	TimeEvent_arm(me->TimerContractor,250, 0,Contractor_Off);
}

void evseStateMachine_ReInit(evse* const me) {
	addToTrace("[EVSE]re-initializing fsmEvse");
	memset(&me->EVCC, 0, sizeof(me->EVCC));
	memset(&me->EVSE, 0, sizeof(me->EVSE));
	memset(&me->LIMIT, 0, sizeof(me->LIMIT));
	me->State = EVSE_State_SAPRq;	//0
	me->cyclesInState = HAL_GetTick();			//reset timeout
	EVSE_set_volt_curr(me,0,0);
	TimeEvent_arm(me->TimerContractor,250, 0,Contractor_Off);
	//simulatedPresentVoltage =0;
	//rxdata clear
	//tcp_Disconnect();
}

void evseStateMachine_Init(evse* const me){
	uint8_t evseid[LEN_OF_EVSEID] = {0xdc,0x44,0x27,0x1f,0xd9,0x1b};

	projectExiConnector* exi = me->projectExiConnector;
	addToTrace("[EVSE]Initializing evseStateMachine");
	memset(&me->EVCC, 0, sizeof(me->EVCC));	//reset all variable
	memset(&me->EVSE, 0, sizeof(me->EVSE));
	memset(&me->LIMIT, 0, sizeof(me->LIMIT));
	memset(me->evccid, 0, sizeof(me->evccid));
			//collect from EV car
	memcpy(me->evseid, evseid, sizeof(evseid));
	//EVCC ={0};
	exi->sessionId.u64t = 0x0000000048af31ad;
	me->State = EVSE_State_SAPRq;		//0
	me->cyclesInState = HAL_GetTick();
	EVSE_set_volt_curr(me,0,0);
	TimeEvent_arm(me->TimerContractor,250, 0,Contractor_Off);
	if(TimeEvent_new_Timer(&me->TimerContractor)){
		addToTrace("[Timer] Failed Initializing timer for Timer_Contractor");
	}
}

void evseStateMachine_Counter(evse* const me){
	//evse_cyclesInState += 1; // for timeout handling, count how long we are in a state
	 if (evse_isTooLong(me) && me->State != EVSE_State_SAPRq){
		 addToTrace("[EVSE] State too long");
		 evse_enterState(me,EVSE_State_SAPRq);
		 EVSE_set_volt_curr(me,0,0);
		 TimeEvent_arm(me->TimerContractor,500, 0,Contractor_Off);
	 }
}


void evseStateMachine_Task(evse* const me) {
    // run the state machine:
	//evse_cyclesInState += 1; // for timeout handling, count how long we are in a state
	AppCanBusTask_timerTaskReadSystem();			//force to canbus to read
    EVSE_now_update(me);			//update current voltage
    User_EVSEIsolationStatusCheck(me);
    decodeTCP(me);
}

/*** functions ********************************************************/
void addV2GTPHeader(evse* const me, const uint8_t *exiBuffer, uint8_t exiBufferLen) {
	// takes the bytearray with exidata, and adds a header to it, according to the Vehicle-to-Grid-Transport-Protocol
	// V2GTP header has 8 bytes
	// 1 byte protocol version
	// 1 byte protocol version inverted
	// 2 bytes payload type
	// 4 byte payload length
	me->tcp->tx.Payload[0] = 0x01; // version
	me->tcp->tx.Payload[1] = 0xfe; // version inverted
	me->tcp->tx.Payload[2] = 0x80; // payload type. 0x8001 means "EXI data"
	me->tcp->tx.Payload[3] = 0x01; //
	me->tcp->tx.Payload[4] = (uint8_t)(exiBufferLen >> 24); // length 4 byte.
	me->tcp->tx.Payload[5] = (uint8_t)(exiBufferLen >> 16);
	me->tcp->tx.Payload[6] = (uint8_t)(exiBufferLen >> 8);
	me->tcp->tx.Payload[7] = (uint8_t)exiBufferLen;
	if (exiBufferLen+8 < TCP_PAYLOAD_LEN) {
	  my_memcpy(&me->tcp->tx.Payload[8], exiBuffer, exiBufferLen);
	  me->tcp->tx.PayloadLen = 8 + exiBufferLen; /* 8 byte V2GTP header, plus the EXI data */
	  //showAsHex(tcpPayload, tcpPayloadLen, "tcpPayload");
	} else {
	  addToTrace("[EVSE]Error: EXI does not fit into tcpPayload.");
	}
}


void encode_only_appHand(evse* const me) {
	projectExiConnector* exi = me->projectExiConnector;
	exi->global_streamEncPos = 0;
	projectExiConnector_encode_appHandExiDocument(exi);		//evse added
	addV2GTPHeader(me,exi->global_streamEnc.data, exi->global_streamEncPos);
	showAsHex(me->tcp->tx.Payload, me->tcp->tx.PayloadLen, "responding");			//debug mode
}

void encode_only_Din(evse* const me) {
	projectExiConnector* exi = me->projectExiConnector;
	exi->global_streamEncPos = 0;
	projectExiConnector_encode_DinExiDocument(exi);
	addV2GTPHeader(me,exi->global_streamEnc.data, exi->global_streamEncPos);		//
	showAsHex(me->tcp->tx.Payload, me->tcp->tx.PayloadLen, "responding");			//Debug mode
}

void routeDecoderInputData(evse* const me) {
  projectExiConnector* exi = me->projectExiConnector;
  exi->global_streamDec.data = &me->tcp->rxdata[V2GTP_HEADER_SIZE];
  exi->global_streamDec.size = me->tcp->rxdataLen - V2GTP_HEADER_SIZE;
  //showAsHex(global_streamDec.data, global_streamDec.size, "decoder will see");
  //connMgr_ApplOk(10);
}

/************ public interfaces *****************************************/
/* The init function for the PEV charging state machine. */


uint8_t SessionIdCheckRq(evse* const me){
	  projectExiConnector* exi = me->projectExiConnector;
	  uint8_t err=0;
	  if(memcmp(exi->sessionId.u8t, exi->dinDocDec.V2G_Message.Header.SessionID.bytes, SESSIONID_LEN)==0){
		  err=1;
	  }else{
		  err=0;
	  }
  	  return err;

}


dinDC_EVSEStatusCodeType User_EVSEStatusCheck(evse* const me){
	//if(btn){
	//	EVSE.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Shutdown;//
	//}
	//else{
	me->EVSE.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Ready;//
	//}
	return me->EVSE.EVSEStatusCode;
}

void Emergency_Stop_Check(evse* const me){
	//if(btn){
	//	EVSE.EVSEStatusCode = dinDC_EVSEStatusCodeType_EVSE_Shutdown;//
	//}
}

//dinisolationLevelType_Invalid = 0,
//dinisolationLevelType_Valid = 1,
//dinisolationLevelType_Warning = 2,
//dinisolationLevelType_Fault = 3
dinisolationLevelType User_EVSEIsolationStatus = 0;
dinisolationLevelType User_EVSEIsolationStatusCheck(evse* const me){
	User_EVSEIsolationStatus = dinisolationLevelType_Valid;
	return User_EVSEIsolationStatus;
}

//	dinEVSEProcessingType_Finished = 0, dinEVSEProcessingType_Ongoing = 1
dinEVSEProcessingType User_CCRs_EVSEProcessing = 0;
dinEVSEProcessingType User_CCRs_EVSEProcessingCheck(evse* const me){		//CableCheckRes
	User_CCRs_EVSEProcessing = dinEVSEProcessingType_Ongoing;
	User_CCRs_EVSEProcessing = dinEVSEProcessingType_Finished;
	return User_CCRs_EVSEProcessing;
}

dinEVSEProcessingType User_CPDRs_EVSEProcessing = 0;		//ChargeParameterDiscovery
dinEVSEProcessingType User_CPDRs_EVSEProcessingCheck(evse* const me){
	User_CPDRs_EVSEProcessing = dinEVSEProcessingType_Ongoing;
	User_CPDRs_EVSEProcessing = dinEVSEProcessingType_Finished;
	return User_CPDRs_EVSEProcessing;
}

dinEVSEProcessingType User_CARs_EVSEProcessing = 0;		//ContractAuthenticationRes
dinEVSEProcessingType User_CARs_EVSEProcessingCheck(evse* const me){//with RFID card or app or however.
	User_CARs_EVSEProcessing = dinEVSEProcessingType_Ongoing;
	User_CARs_EVSEProcessing = dinEVSEProcessingType_Finished;
	return User_CARs_EVSEProcessing;
}



