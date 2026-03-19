/*
 *  Insulation_Monitoring.c
 *
 *  Created on: Feb 2, 2025
 *      Author: zuoha
 */
#include <AppADC.h>
#include <AppADC.h>
#include "main.h"
#include "app.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <TimeEvent.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "serial.h"
#include "cdc_serial.h"
#include "ccs32_globals.h"
#include "usbd_cdc_acm_if.h"
#include "evseStateMachine.h"
#include "app_func.h"
#include "ring_buf3.h"
#include "ring_buf_type.h"
#include "evsePredefine.h"
#include "projectExiConnector.h"
#include "AppControl.h"
#include "AppADC.h"
#include "AppSerial.h"
#include "AppCanBus.h"

extern adc_measure adc_measured;
extern module module1 ,system1;


enum isulation_check{ISO_START, POS_ON, POS_MEA, NEG_ON, NEG_MEA, ISO_STOP};

typedef struct {
	enum isulation_check control;
	uint32_t time;
}RingBufElementInsulation;


struct Insulation{
	RingBufElementInsulation rb_array[10];
	RingBuf3 rb_buf;
	uint8_t Event_Timer;
	uint8_t sending_signal;
};
struct Insulation Insulation = {0};


#define BOARD_800V

#ifdef BOARD_800V
#define T_ACQ_DFLT              1           // Default measurement/acquisition period
#define R_PS_DFLT               271000.0     // Input series  resistance positive branch
#define R_NS_DFLT               271000.0     // Input series resistance negative branch
#define R_S_DFLT                255.0       // Current sense resistor in front of AMC3330
#define V_REF_DFLT              1.65        // Reference voltage
#define K_ADC_DFLT              2           // AMC3330 Gain Riso sensing
#define K_BATT_DFLT             2           // AMC3330 Gain Vbus sensing
#define k_DIFF2SINISO_DFLT      0.825       //Differential to single ended conversion Gain Riso measurement
#define k_DIFF2SINBATT_DFLT     1.65        //Differential to single ended conversion Gain Vbus measurement
#define R_PS_DFLT_DC            990000.0     // Input series resistance
#define R_S_DFLT_DC             910.0       // Feedback resistance
#define V_REF_DFLT_DC           0.0         // Common Mode Reference for Dif2Sing conversion Bus Monitoring
#else
#define T_ACQ_DFLT              1           // Default measurement/acquisition period
#define R_PS_DFLT               68100.0     // Input series  resistance positive branch
#define R_NS_DFLT               68100.0     // Input series resistance negative branch
#define R_S_DFLT                120.0       // Current sense resistor in front of AMC3330
#define V_REF_DFLT              1.65        // Reference voltage
#define K_ADC_DFLT              2           // AMC3330 Gain Riso sensing
#define K_BATT_DFLT             2           // AMC3330 Gain Vbus sensing
#define k_DIFF2SINISO_DFLT      0.825       //Differential to single ended conversion Gain Riso measurement
#define k_DIFF2SINBATT_DFLT     1.65        //Differential to single ended conversion Gain Vbus measurement
#define R_PS_DFLT_DC            990000.0     // Input series resistance
#define R_S_DFLT_DC             2100.0       // Feedback resistance
#define V_REF_DFLT_DC           0.0         // Common Mode Reference for Dif2Sing conversion Bus Monitoring
#endif

// --- Globals ---
double v_iso_P;            // Isolation voltage Vp
double v_iso_N;            // Isolation voltage Vn
double v_DC_P, v_DC_N;     // DC BUS voltage during Vp measurement and Vn measurement

// According ADC voltages
double v_adc_iso_pos;           // voltage at the ADC input during Vp measurement
double v_adc_iso_neg;           // voltage at the ADC input during Vn measurement
//double v_adc_DC_P, v_adc_DC_N;  // voltage at the ADC for DC BUS voltage measurement during Vp measurement and Vn measurement

// Circuit parameters and coefficients for the switched in resistor divider
double r_ps = R_PS_DFLT;
double r_ns = R_NS_DFLT;
double r_s = R_S_DFLT;
double v_ref = V_REF_DFLT;
double k_adc = K_ADC_DFLT;
double k_diff2sinISO = k_DIFF2SINISO_DFLT;

// Circuit parameters and coefficients for Bus Monitoring
double r_ps_DC = R_PS_DFLT_DC;
double r_s_DC = R_S_DFLT_DC;
double v_ref_DC = V_REF_DFLT_DC;
double k_batt = K_BATT_DFLT;
double k_diff2sinBATT = k_DIFF2SINBATT_DFLT;

// Calculated insulation resistances
double r_iso_p, r_iso_n;

void measure_voltages_VP_VDC_P(uint32_t adcRaw_VP,uint32_t  adcRaw_DC_P);
void measure_voltages_VN_V_DC_N(uint32_t adcRaw_VN , uint32_t  adcRaw_DC_N);
void calculate_riso(void);
void Insulation_Init(void);
void AppInsulation_TimerTask(void);




/*
float measure_ground_1(float data1,float data2){			//[0],[1]							//CA-IS1300G25G (gain = 8.2)
	float Voltage = (((float)(data1-data2))*3.3/4095.0);
	float Ground_resistor =(Voltage/(240*8.2))*1000240;
	return Ground_resistor;
}
*/
//data1 - data2  --> (+4096 to -4096)*3.3/4096

#define r_ps_1 		1000000.0
#define r_s_1 		240.0
#define r_ns_1		1000000.0

#define r_ps_DC_1 	1000000.0		// Input series resistance
#define r_s_DC_1	240.0

void measure_voltages_VP_VDC_P_2(int32_t adcRaw_VPP,int32_t adcRaw_VPN,int32_t  adcRaw_DC_PP,int32_t  adcRaw_DC_PN){
	double v_adc_DC_P;
	double v_adc_iso_pos = 0;
    //uint32_t adcRaw_VP = 0, adcRaw_DC_P = 0;

	v_adc_iso_pos = (((double)(adcRaw_VPP - adcRaw_VPN))*3.3/4096.0);
    v_iso_P = ((r_ps_1 + r_s_1) / r_s_1) * (( v_adc_iso_pos ) / (8.2));		//v over iso resistor

    v_adc_DC_P = (((double)(adcRaw_DC_PP - adcRaw_DC_PN))*3.3/4096.0);			//3.3 v on adc
    v_DC_P =((r_ps_DC_1 + r_s_DC_1) / r_s_DC_1) * ((v_adc_DC_P) / (8.2));	// Vdc
}

void measure_voltages_VN_VDC_N_2(int32_t adcRaw_VNP,int32_t adcRaw_VNN,int32_t  adcRaw_DC_NP,int32_t  adcRaw_DC_NN) {
	double v_adc_DC_N;
	double v_adc_iso_neg = 0;
    //adcRaw_VN = 0, adcRaw_DC_N = 0;

	v_adc_iso_neg = (((double)(adcRaw_VNP - adcRaw_VNN))*3.3/4096.0);
    v_iso_P = ((r_ns_1 + r_s_1) / r_s_1) * (( v_adc_iso_neg ) / (8.2));		//v over iso resistor

    v_adc_DC_N = (((double)(adcRaw_DC_NP - adcRaw_DC_NN))*3.3/4096.0);			//3.3 v on adc
    v_DC_P =((r_ps_DC_1 + r_s_DC_1) / r_s_DC_1) * ((v_adc_DC_N) / (8.2));	// Vdc
}

void calculate_riso_2(void) {
    r_iso_n =  ((r_s_1 + r_ns_1)*(r_s_1 + r_ps_1)*(v_DC_N*v_DC_P + v_DC_P * v_iso_N - v_DC_N * v_iso_P)) /(v_iso_P *(r_ns_1 * v_iso_N - r_ps_1 * v_iso_N + v_DC_N*r_s_1 + v_DC_N*r_ns_1));
    r_iso_p = -((r_s_1 + r_ns_1)*(r_s_1 + r_ps_1)*(v_DC_N*v_DC_P + v_DC_P * v_iso_N - v_DC_N * v_iso_P)) /(v_iso_N *(r_ns_1 * v_iso_P - r_ps_1 * v_iso_P + v_DC_P*r_s_1 + v_DC_P*r_ps_1));
}


void measure_voltages_VP_VDC_P(uint32_t adcRaw_VP,uint32_t  adcRaw_DC_P){
	double v_adc_DC_P;
	double v_adc_iso_pos = 0;
    //uint32_t adcRaw_VP = 0, adcRaw_DC_P = 0;

    v_adc_iso_pos = (double)adcRaw_VP * 3.3 / 4096 /20;				//3.3 v on adc
    v_iso_P = ((r_ps + r_s) / r_s) * (( v_adc_iso_pos - v_ref) / (k_diff2sinISO * k_adc));		//v over iso resistor

    v_adc_DC_P = (double)adcRaw_DC_P * 3.3 / 4096 /20;				//3.3 v on adc
    v_DC_P =((r_ps_DC + r_s_DC) / r_s_DC) * ((v_adc_DC_P - v_ref_DC) / (k_batt * k_diff2sinBATT));	// Vdc

}

void measure_voltages_VN_V_DC_N(uint32_t adcRaw_VN , uint32_t  adcRaw_DC_N) {
	double v_adc_DC_N;
	double v_adc_iso_neg = 0;
    //adcRaw_VN = 0, adcRaw_DC_N = 0;

    v_adc_iso_neg = (double)adcRaw_VN * 3.3 / 4096 /20;		//3.3 v on adc
    v_iso_N = ((r_ps + r_s) / r_s) * (( v_adc_iso_neg - v_ref) / (k_diff2sinISO * k_adc));

    v_adc_DC_N = (double)adcRaw_DC_N * 3.3 / 4096 /20;		//3.3 v on adc
    v_DC_N =((r_ps_DC + r_s_DC) / r_s_DC) * ((v_adc_DC_N - v_ref_DC) / (k_batt * k_diff2sinBATT));


}

void calculate_riso(void) {
    r_iso_n =  ((r_s + r_ns)*(r_s + r_ps)*(v_DC_N*v_DC_P + v_DC_P * v_iso_N - v_DC_N * v_iso_P)) /(v_iso_P *(r_ns * v_iso_N - r_ps * v_iso_N + v_DC_N*r_s + v_DC_N*r_ns));
    r_iso_p = -((r_s + r_ns)*(r_s + r_ps)*(v_DC_N*v_DC_P + v_DC_P * v_iso_N - v_DC_N * v_iso_P)) /(v_iso_N *(r_ns * v_iso_P - r_ps * v_iso_P + v_DC_P*r_s + v_DC_P*r_ps));
}

void AppInsulation_Init(void){
	TimeEvent_new_Timer(&Insulation.Event_Timer);
	RingBuf3_ctor(&Insulation.rb_buf, Insulation.rb_array,sizeof(RingBufElementInsulation), ARRAY_NELEM(Insulation.rb_array));
	HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port,RELAY_NEG_Pin, GPIO_PIN_RESET);
}
void AppInsulation_TimerTask(void){
	RingBufElementControl tempRBA ={0};
	tempRBA.control = Insulation.sending_signal;
	RingBuf3_put(&Insulation.rb_buf, &tempRBA);
}


void AppInsulation_StartTask(void){
	RingBufElementControl tempRBA ={0};
	Insulation.sending_signal =ISO_START;
	tempRBA.control = Insulation.sending_signal;
	RingBuf3_put(&Insulation.rb_buf, &tempRBA);
}

void AppInsulation_StopTask(void){
	RingBufElementControl tempRBA ={0};
	Insulation.sending_signal =ISO_STOP;
	tempRBA.control = Insulation.sending_signal;
	RingBuf3_put(&Insulation.rb_buf, &tempRBA);
}

//IEC 615578-8
void AppInsulation_Task(void){
	RingBufElementInsulation tempRB ={0};

	//if(adc_measured.ground_test_1 < 100){}
	//if(adc_measured.ground_test_2 < 100){}
	//EVSE.IsolationStatus;

	if(true == RingBuf3_get(&Insulation.rb_buf, &tempRB)){
		switch (tempRB.control) {
			case ISO_START:		//start
				HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port,RELAY_NEG_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_RESET);
				Insulation.sending_signal = POS_ON;
				TimeEvent_arm(Insulation.Event_Timer,500, 0,AppInsulation_TimerTask);
				break;
			case POS_ON:		//turnon POS
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_SET);
				Insulation.sending_signal = POS_MEA;
				TimeEvent_arm(Insulation.Event_Timer,1000, 0,AppInsulation_TimerTask);

				break;
			case POS_MEA:		//measure POS
				measure_voltages_VP_VDC_P(adc_measured.AdcRawGroundPP,adc_measured.AdcRawGroundPN);
				HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port,RELAY_NEG_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_RESET);;
				Insulation.sending_signal = NEG_ON;
				TimeEvent_arm(Insulation.Event_Timer,500, 0,AppInsulation_TimerTask);
				break;
			case NEG_ON: 	//check right
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_SET);
				Insulation.sending_signal = NEG_MEA;
				TimeEvent_arm(Insulation.Event_Timer,1000, 0,AppInsulation_TimerTask);
				break;
			case NEG_MEA:			//done
				measure_voltages_VN_V_DC_N(adc_measured.AdcRawGroundNP,adc_measured.AdcRawGroundNN);
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port,RELAY_NEG_Pin, GPIO_PIN_RESET);
				calculate_riso();
				break;
			case ISO_STOP:
				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port,RELAY_POS_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port,RELAY_NEG_Pin, GPIO_PIN_RESET);
				break;
			default:
				break;
		}
	}
}

void AppInsulation_test(void){

	measure_voltages_VP_VDC_P_2(adc_measured.AdcRawGroundPP,adc_measured.AdcRawGroundPN,adc_measured.AdcRawHVP,adc_measured.AdcRawHVN);
	measure_voltages_VN_VDC_N_2(adc_measured.AdcRawGroundNP,adc_measured.AdcRawGroundNN,adc_measured.AdcRawHVP,adc_measured.AdcRawHVN);
	calculate_riso_2();

}
