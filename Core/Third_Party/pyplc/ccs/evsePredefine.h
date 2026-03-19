/*
 * evsePredefine.h
 *
 *  Created on: Aug 18, 2024
 *      Author: zuoha
 */

#ifndef CCS_EVSEPREDEFINE_H_
#define CCS_EVSEPREDEFINE_H_


	//#define EVSE_State_SAPRq 		0
	//#define EVSE_State_SSRq 		1
	//#define EVSE_State_SDRq			2
	//#define EVSE_State_SPSRq 		3
	//#define EVSE_State_FlexibleRq	4
	//#define EVSE_State_CPDRq		5
	//#define EVSE_State_CCRq			6
	//#define EVSE_State_PCRq			7
	//#define EVSE_State_PDRq			8
	//#define EVSE_State_CARq			9
	//#define EVSE_State_CDRq			10
	//#define EVSE_State_WDRq			11

	enum EVSEstate{
		EVSE_State_SAPRq,
		EVSE_State_SSRq,
		EVSE_State_SDRq,
		EVSE_State_SPSRq,
		EVSE_State_FlexibleRq,
		EVSE_State_CPDRq,
		EVSE_State_CCRq,
		EVSE_State_PCRq,
		EVSE_State_PDRq,
		EVSE_State_CARq,
		EVSE_State_CDRq,
		EVSE_State_WDRq
	};





	#define LEN_OF_EVCCID 6
	#define LEN_OF_EVSEID 6


	//#define dDecBody 					dinDocDec.V2G_Message.Body
	//#define dEncBody 					dinDocEnc.V2G_Message.Body

	//#define dDecHeader					dinDocDec.V2G_Message.Header
	//#define dEncHeader					dinDocEnc.V2G_Message.Header

	//#define dDecSessionID				dinDocDec.V2G_Message.Header.SessionID.bytes
	//#define dEncSessionID				dinDocEnc.V2G_Message.Header.SessionID.bytes


	//Decoder

	//#define SDRq						dDecBody.ServiceDiscoveryReq


	//#define PDRq 						dDecBody.PowerDeliveryReq
	//#define PDRq_DCpara 				PDRq.DC_EVPowerDeliveryParameter
	//#define PDRq_DCpara_st 				PDRq_DCpara.DC_EVStatus

	//#define PCRq						dDecBody.PreChargeReq
	//#define PCRq_DCst 					PCRq.DC_EVStatus
	//#define PCRq_tvolt 					PCRq.EVTargetVoltage
	//#define PCRq_tcurr 					PCRq.EVTargetCurrent

	//#define CDRq 						dDecBody.CurrentDemandReq
	//#define CDRq_DCst 					CDRq.DC_EVStatus
	//#define CDRq_tvolt 					CDRq.EVTargetVoltage
	//#define CDRq_tcurr 					CDRq.EVTargetCurrent
	//#define CDRq_rttfs 					CDRq.RemainingTimeToFullSoC
	//#define CDRq_rttbs 					CDRq.RemainingTimeToBulkSoC

	//#define WDRq 						dDecBody.WeldingDetectionReq
	//#define WDRq_DCst					WDRq.DC_EVStatus

	//#define SPSRq	 					dDecBody.ServicePaymentSelectionReq
	//#define SPSRq_ss 					SPSRq.SelectedServiceList.SelectedService

	//#define CPDRq 						dDecBody.ChargeParameterDiscoveryReq
	//#define CPDRq_DCpara 				CPDRq.DC_EVChargeParameter
	//#define CPDRq_DCpara_st 			CPDRq_DCpara.DC_EVStatus
	//#define CPDRq_DCpara_cl 			CPDRq_DCpara.EVMaximumCurrentLimit
	//#define CPDRq_DCpara_pl 			CPDRq_DCpara.EVMaximumPowerLimit
	///#define CPDRq_DCpara_vl 			CPDRq_DCpara.EVMaximumVoltageLimit
	//#define CPDRq_DCpara_ec 			CPDRq_DCpara.EVEnergyCapacity
	//#define CPDRq_DCpara_er 			CPDRq_DCpara.EVEnergyRequest


	//#define CCRq						dDecBody.CableCheckReq
	//#define CCRq_DCst 					CCRq.DC_EVStatus
	//#define CDRq_uMxLi					CDRq.EVMaximumVoltageLimit
	//#define CDRq_iMxLi					CDRq.EVMaximumCurrentLimit
	//#define CDRq_pMxLi					CDRq.EVMaximumPowerLimit

	//encoder

	//#define SDSRs						dEncBody.ServiceDiscoveryRes

	//#define SPSRs						dEncBody.ServicePaymentSelectionRes

	//#define CPDRs						dEncBody.ChargeParameterDiscoveryRes
	//#define CPDRs_DCpara 				CPDRs.DC_EVSEChargeParameter
	//#define CPDRs_DCpara_st 			CPDRs_DCpara.DC_EVSEStatus
	//#define CPDRs_DCpara_iMaxLi 		CPDRs_DCpara.EVSEMaximumCurrentLimit
	//#define CPDRs_DCpara_pMaxLi 		CPDRs_DCpara.EVSEMaximumPowerLimit
	//#define CPDRs_DCpara_uMaxLi 		CPDRs_DCpara.EVSEMaximumVoltageLimit
	//#define CPDRs_DCpara_iMinLi 		CPDRs_DCpara.EVSEMinimumCurrentLimit
	//#define CPDRs_DCpara_uMinLi 		CPDRs_DCpara.EVSEMinimumVoltageLimit

	//#define PCRs 						dEncBody.PreChargeRes
	//#define PCRs_DCst 					PCRs.DC_EVSEStatus
	//#define PCRs_pv						PCRs.EVSEPresentVoltage

	//#define CARs						dEncBody.ContractAuthenticationRes

	//#define PDRs 						dEncBody.PowerDeliveryRes
	//#define PDRs_DCst 					dEncBody.PowerDeliveryRes.DC_EVSEStatus

	//#define CDRs						dEncBody.CurrentDemandRes
	//#define CDRs_DCst						CDRs.DC_EVSEStatus
	//#define CDRs_pv						CDRs.EVSEPresentVoltage
	//#define CDRs_pc						CDRs.EVSEPresentCurrent
	//#define CDRs_vMaxLi					CDRs.EVSEMaximumVoltageLimit
	//#define CDRs_cMaxLi					CDRs.EVSEMaximumCurrentLimit
	//#define CDRs_pMaxLi					CDRs.EVSEMaximumPowerLimit

	//#define CCRs 						dEncBody.CableCheckRes
	//#define CCRs_DCst 					CCRs.DC_EVSEStatus

	//#define WDRs						dEncBody.WeldingDetectionRes
	//#define WDRs_DCst						WDRs.DC_EVSEStatus
	//#define WDRs_pv						WDRs.EVSEPresentVoltage


	//#define APPRq 						aphsDocDec.supportedAppProtocolReq
	//#define APPRq_ap 					aphsDocDec.supportedAppProtocolReq.AppProtocol

	//#define APPRs 						aphsDocEnc.supportedAppProtocolRes

	//#define SSRs 						dEncBody.SessionSetupRes

	//#define SSpRs						dEncBody.SessionStopRes



	int32_t combineValueAndMultiplier(int32_t val, int8_t multiplier);
	uint8_t OutputValueAndMultiplier(int32_t input, int32_t* val, int8_t* multiplier);
#endif /* CCS_EVSEPREDEFINE_H_ */
