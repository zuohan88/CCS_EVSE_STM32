//============================================================================
#include <AppADC.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "app.h"
#include "ccs32_globals.h"
//#include "ring_buf_type.h"
#include "ring_buf3.h"



double measure_Vin(int32_t data);
double measure_MCP9700A(int32_t data);
double measure_temp_MCU(int32_t data1,int32_t data2);
double measure_high_voltage(int32_t data1,int32_t data2);
double measure_high_current(int32_t data1,int32_t data2);
double measure_ground_1(int32_t data1,int32_t data2);
double measure_ground_2(int32_t data1,int32_t data2);
double measure_pt1000_1(int32_t data);
double measure_pt1000_2(int32_t data);
double measure_pilot_adc_high_speed(AppADC* const me, int32_t data);


OPAMP_HandleTypeDef hopamp2;
OPAMP_HandleTypeDef hopamp3;
//OPAMP_HandleTypeDef hopamp4;
//OPAMP_HandleTypeDef hopamp6;

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
ADC_HandleTypeDef hadc4;
ADC_HandleTypeDef hadc5;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim17;		//PWM


//private
// need to struct all later
//global for int




extern AppADC AppADC1;




//extern adc_measure 	adc_measured;




void AppADC_Init(AppADC* const me){
	  //RingBuf_ctor(&adc_rb1, adc_buf1, ARRAY_NELEM(adc_buf1));
	  RingBuf3_ctor(&me->adc_rb1, me->adc_buf1,sizeof(RingBufElementADC), ARRAY_NELEM(me->adc_buf1));

	  HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc3,ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc4,ADC_SINGLE_ENDED);
	  HAL_ADCEx_Calibration_Start(&hadc5,ADC_SINGLE_ENDED);
	  HAL_Delay(10);

	  HAL_OPAMP_Start(&hopamp2);
	  HAL_OPAMP_Start(&hopamp3);
	 // HAL_OPAMP_Start(&hopamp4);
	 // HAL_OPAMP_Start(&hopamp6);

	  //if(HAL_OK != HAL_OPAMP_Start(&hopamp2))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_Start(&hopamp3))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_Start(&hopamp4))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_Start(&hopamp6))Error_Handler();

	  //if(HAL_OK != HAL_OPAMP_SelfCalibrate(&hopamp2))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_SelfCalibrate(&hopamp3))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_SelfCalibrate(&hopamp4))Error_Handler();
	  //if(HAL_OK != HAL_OPAMP_SelfCalibrate(&hopamp6))Error_Handler();

	  //HAL_TIM_Base_Start_IT(&htim2);
	  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	  //pilot_ctrl(1,100);
	  AppADC_PilotCtrl(1,0);
	  RELAY_1_OFF;
	  RELAY_2_OFF;
	  RELAY_3_OFF;
	  RELAY_4_OFF;
	  HAL_ADC_Start_DMA (&hadc1, (uint32_t*)me->adc_data, 13);	//13
	  HAL_ADC_Start_DMA (&hadc2, (uint32_t*)&me->adc_data[13], 3);		//1
	  me->adc_measured.CP_State = StateA;
	  me->adc_measured.Previous_CP_State = StateA;
}

void AppADC_task(AppADC* const me){		//run in main loop
	RingBufElementADC adc_rb_temp;
	if(true == RingBuf3_get(&me->adc_rb1, &adc_rb_temp)){

	    switch (adc_rb_temp.command) {
	    case 1:
	    	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)me->adc_data, 13);
	    	me->adc_measured.V12 = measure_Vin(adc_rb_temp.adc_data[11]);
	    	me->adc_measured.temperture_MCP9700A = measure_MCP9700A(adc_rb_temp.adc_data[12]);
		//	adc_measured.temperture_MCU = measure_temp_MCU((float)adc_rb_temp.adc_data[9],(float)adc_rb_temp.adc_data[8]);
	    	me->adc_measured.AdcRawHVP = adc_rb_temp.adc_data[6];
	    	me->adc_measured.AdcRawHVN = adc_rb_temp.adc_data[7];
	    	me->adc_measured.output_voltage = measure_high_voltage(adc_rb_temp.adc_data[6],adc_rb_temp.adc_data[7]);	//this okay
	    	me->adc_measured.output_current = measure_high_current(adc_rb_temp.adc_data[4],adc_rb_temp.adc_data[5]);	//floating
	    	me->adc_measured.AdcRawGroundPP = adc_rb_temp.adc_data[0];
	    	me->adc_measured.AdcRawGroundPN = adc_rb_temp.adc_data[2];
	    	me->adc_measured.AdcRawGroundNP = adc_rb_temp.adc_data[1];
	    	me->adc_measured.AdcRawGroundNN = adc_rb_temp.adc_data[3];
	    	me->adc_measured.ground_test_1 = measure_ground_1(adc_rb_temp.adc_data[0],adc_rb_temp.adc_data[2]);	//IPLP_Cuurent	this port 1 this not okay
	    	me->adc_measured.ground_test_2 = measure_ground_2(adc_rb_temp.adc_data[1],adc_rb_temp.adc_data[3]);	//IPLP_VOLT		this port 2 this okay
	        break;
	    case 2:
	    	HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&me->adc_data[13], 3);
			me->adc_measured.pt1000_1 = measure_pt1000_1(adc_rb_temp.adc_data[15]);		//[10]
			me->adc_measured.pt1000_2 = measure_pt1000_2(adc_rb_temp.adc_data[14]);
			me->adc_measured.pilot_voltage = measure_pilot_adc_high_speed(me,adc_rb_temp.adc_data[13]);
	    	break;

	    case 3:
	    	break;
	    default:
	    	break;
	    }

	    me->adc_measured.rate = adc_rb_temp.last_tick - me->adc_measured.last_tick;
	    me->adc_measured.last_tick = adc_rb_temp.last_tick;
	}
}

//**************************************************************************************
//ISR
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	RingBufElementADC adc_rb_temp;
	AppADC* const me = &AppADC1;		//bobian

	if(hadc->Instance == ADC1)
	{
		adc_rb_temp.adc_data[0] = me->adc_data[0];
		adc_rb_temp.adc_data[1] = me->adc_data[1];
		adc_rb_temp.adc_data[2] = me->adc_data[2];
		adc_rb_temp.adc_data[3] = me->adc_data[3];
		adc_rb_temp.adc_data[4] = me->adc_data[4];
		adc_rb_temp.adc_data[5] = me->adc_data[5];
		adc_rb_temp.adc_data[6] = me->adc_data[6];
		adc_rb_temp.adc_data[7] = me->adc_data[7];
		adc_rb_temp.adc_data[8] = me->adc_data[8];
		adc_rb_temp.adc_data[9] = me->adc_data[9];
		adc_rb_temp.adc_data[10] = me->adc_data[10];
		adc_rb_temp.adc_data[11] = me->adc_data[11];
		adc_rb_temp.adc_data[12] = me->adc_data[12];
		adc_rb_temp.last_tick = HAL_GetTick();
		adc_rb_temp.command = 1;
		RingBuf3_put(&me->adc_rb1, &adc_rb_temp);

	}
	if(hadc->Instance == ADC2)
	{
		adc_rb_temp.adc_data[13] = me->adc_data[13];
		adc_rb_temp.adc_data[14] = me->adc_data[14];
		adc_rb_temp.adc_data[15] = me->adc_data[15];
		adc_rb_temp.last_tick = HAL_GetTick();
		adc_rb_temp.command = 2;
		RingBuf3_put(&me->adc_rb1, &adc_rb_temp);
	}
	if(hadc->Instance == ADC3)
	{

	}
	if(hadc->Instance == ADC4)
	{

	}
	if(hadc->Instance == ADC5)
	{

	}
}
//**************************************************************************************


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM2){

	}

}
#define resolution 	4095
#define Vref		3.3
#define Ra 			100
#define Rb 			20
double measure_Vin(int32_t data){			//(vadc= Vin (20/120)  //digital= Vadc/3.3*4095--> vdac= digital*3300/4095
	//int32_t vadc = adc_data[12]*3300/4095;		//		adc_data[11]
	return (data/206.8);
}

double measure_MCP9700A(int32_t data){		//[12]
	double voltage = 3.3;	// Typically 5V on Arduino (This is the reference voltage used by your boards A/D Converter).
	double tmp = data;
  tmp = tmp*voltage/4095;
  tmp = (tmp/0.01) - .5;
  return (tmp - 32.0) * (5.0/9.0);
}
#define AVG_SLOPE (4.3F)
#define V_AT_25C  (1.43F)
#define V_REF_INT (1.2F)


//

double measure_temp_MCU(int32_t data1,int32_t data2){		//this one got error,now ignore it 1st not important
	double V_Ref = (double)((V_REF_INT * 4095.0)/data1);			//[9],[8]
	double V_Sense = (double)(data2 * V_Ref) / 4095.0;
	double Temperature = (((V_AT_25C - V_Sense) * 1000.0) /AVG_SLOPE) + 25.0;		//?
	return Temperature;
}
//channel 6 [4]= IDCM_Curr1
//channel 7 [5]= IDCM_Curr2
//channel 8 [6]= IDCM_Volt1
//channel 9 [7]= IDCM_Volt2
double measure_high_voltage(int32_t data1,int32_t data2){	//AMC1300,CA-IS1300G25G (gain = 8.2) [6],[7]
	double voltage = (((double)(data1-data2))*3.3/4095.0);			//maximum input -+250mV												//input 0-50mV  output mid around 1.4425 (swing 1.017)
	voltage =(voltage/(240*8.2))*1000240;
	return voltage;															//8.2 (250mV)or 41(50mV) input voltage

}

double measure_high_current(int32_t data1,int32_t data2){	//[4],[5]							//normally 1.44 41	//50mV input max
	double current = (((double)(data1 - data2))*3.3 / 4095.0);		//CA-IS1300G05G	(gain = 41)
	current =current*41;
	return current;		//gain =41(-+50mV)

}
//channel 1 [0]= IPLP_Curr1
//channel 2 [1]= IPLP_Volt1
//channel 3 [2]= IPLP_Curr2
//channel 4 [3]= IPLP_Volt2
double measure_ground_1(int32_t data1,int32_t data2){			//[0],[1]							//CA-IS1300G25G (gain = 8.2)
	double Voltage = (((double)(data1-data2))*3.3/4095.0);
	double Ground_resistor =(Voltage/(240*8.2))*1000240;
	return Ground_resistor;
}
double measure_ground_2(int32_t data1,int32_t data2){									//CA-IS1300G25G (gain = 8.2) [2][3]
	double Voltage = (((double)(data1-data2))*3.3/4095.0);
	double Ground_resistor =(Voltage/(240*8.2))*1000240;
	return Ground_resistor;
}

//const float vt_factor = 1.88;	//1.88
//const float offset = 0;
//float temp_c;
//float voltage = sensorvalue * (5.0/ 1023.0);
// temp_c = ( ( ( voltage * 100) / vt_factor ) + offset );

const double Vs = 3.3;
const double R1 = 10000.0;
const double amp = 4.0;
double measure_pt1000_1(int32_t data){					//[10]
	double Vo = ((double)data*3.3)/4095.0;	//mV
	Vo = Vo/amp;
	double R2 = R1/((Vs/Vo)-1);

	//y = 0.25974025974x − 259.74025974
	double temp_c = (0.25974*R2) - 259.74;

	if(R2>2000.0 || R2 < 900.0){
		temp_c = -1.0;
		//no sensor
	}

	return temp_c;
}

double measure_pt1000_2(int32_t data){			//[14]
	double Vo = ((double)data*3.3)/4095.0;	//mV
	Vo = Vo/amp;
	double R2 = R1/((Vs/Vo)-1);
	double temp_c = (0.25974*R2) - 259.74;

	if(R2 > 2000.0 || R2 < 900.0){
		temp_c = -1.0;
		//no sensor
	}

	return temp_c;
}


#define buf_size 50
double measure_pilot_adc_high_speed(AppADC* const me, int32_t data){		//[13]
	static double buffer[buf_size];
	static uint32_t pointer = 0;
	double buffer_max = -15.0f;
	double Vadc = 0;
	double Vpilot = 0;
	//const float resistor[3] = {40.2,10,15};		//resistor a,b,c	(a from vin to ADC, B from 3.3 , C from gnd
	//const float AB = resistor[0]*resistor[1];
	//const float BC = resistor[1]*resistor[2];
	//const float CA = resistor[2]*resistor[0];

	Vadc = (double)(data*3.3/4095.0);
	//Vpilot = fabsf(((Vadc*(AB+BC+CA))-(3.3*CA))/BC);
	Vpilot=((Vadc-1.72)*7.693);
	//Vpilot = (((Vadc*(AB+BC+CA))-(3.3*CA))/BC);
	if(pointer < buf_size){
		buffer[pointer++] = Vpilot;
	}
	if(pointer >= buf_size){
		pointer = 0;
	}
	for(uint32_t i=0;i<buf_size;i++){
		if(buffer[i]>buffer_max)
			buffer_max = buffer[i];
	};
	Vpilot = buffer_max;
	//Vadc = (float)(buffer_max*3.3/4095.0);
	//Vpilot = fabsf(((Vadc*(AB+BC+CA))-(3.3*CA))/BC);
	//adc_measured.Previous_State = adc_measured.State;
	if(Vpilot > 10.0f){										//12v
		me->adc_measured.CP_State = StateA;
	}else if(Vpilot < 10.0f && Vpilot > 8.0f){				//9v
		me->adc_measured.CP_State = StateB;
	}else if(Vpilot < 7.0f && Vpilot > 5.0f){				//6v
		me->adc_measured.CP_State = StateC;
	}else if(Vpilot < 4.0f && Vpilot > 2.0f){				//3v
		me->adc_measured.CP_State = StateD;
	}else if(Vpilot < 1.0f && Vpilot > -1.0f ){				//0v
		me->adc_measured.CP_State = StateE;
	}else if(Vpilot < -11.0f && Vpilot > -13.0f ){				//-12v
		me->adc_measured.CP_State = StateF;
	}
	return Vpilot;			//1.77 is gnd, 0.1 is -12v, 3.2 is 12v, 0.6
}

void AppADC_PilotCtrl(uint8_t on,uint32_t PWM){
	 TIM17->CCR1 = PWM;		//5% only for DC system or turn off

	 if(on){
		 CP_ON;
	 }
	 else{
	 	 CP_OFF;
	 }
}
