/*
 * app.h
 *
 *  Created on: May 3, 2024
 *      Author: zuoha
 */

#ifndef INC_APP_H_
#define INC_APP_H_


#include "ringbuff.h"
#include "McuXFormat.h"
#include "McuLib.h"
#include "McuUtility.h"


#define QCASPI_GOOD_SIGNATURE 0xAA55
#define QCASPI_HW_BUF_LEN 0xC5B

//#define LED_RED_ON 					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);	//on
//#define LED_RED_OFF 				HAL_GPIO_WritePin (GPIOC, GPIO_PIN_5, GPIO_PIN_SET);	//off
#define CP_ON 					HAL_GPIO_WritePin (CP_OFF_GPIO_Port, CP_OFF_Pin, GPIO_PIN_RESET);	//on
#define CP_OFF 					HAL_GPIO_WritePin (CP_OFF_GPIO_Port, CP_OFF_Pin, GPIO_PIN_SET);	//off

#define INPUT_1_READ			HAL_GPIO_ReadPin (INPUT_1_GPIO_Port, INPUT_1_Pin)
#define INPUT_2_READ			HAL_GPIO_ReadPin (INPUT_2_GPIO_Port, INPUT_2_Pin)
#define INPUT_3_READ			HAL_GPIO_ReadPin (INPUT_3_GPIO_Port, INPUT_3_Pin)
#define INPUT_4_READ			HAL_GPIO_ReadPin (INPUT_4_GPIO_Port, INPUT_4_Pin)

#define RELAY_POS_OFF 			HAL_GPIO_WritePin(RELAY_POS_GPIO_Port, RELAY_POS_Pin, GPIO_PIN_RESET);	//
#define RELAY_POS_ON 				HAL_GPIO_WritePin(RELAY_POS_GPIO_Port, RELAY_POS_Pin, GPIO_PIN_SET);	//
#define RELAY_POS_TOGGLE 		HAL_GPIO_TogglePin (RELAY_POS_GPIO_Port, RELAY_POS_Pin);

#define RELAY_NEG_OFF 		HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port, RELAY_NEG_Pin, GPIO_PIN_RESET);	//
#define RELAY_NEG_ON 		HAL_GPIO_WritePin(RELAY_NEG_GPIO_Port, RELAY_NEG_Pin, GPIO_PIN_SET);	//
#define RELAY_NEG_TOGGLE 	HAL_GPIO_TogglePin (RELAY_NEG_GPIO_Port, RELAY_NEG_Pin);

#define RELAY_1_OFF 			HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_SET);	//
#define RELAY_1_ON 				HAL_GPIO_WritePin(RELAY_1_GPIO_Port, RELAY_1_Pin, GPIO_PIN_RESET);	//
#define RELAY_1_TOGGLE 			HAL_GPIO_TogglePin(RELAY_1_GPIO_Port, RELAY_1_Pin);

#define RELAY_2_OFF 			HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_SET);	//
#define RELAY_2_ON 				HAL_GPIO_WritePin(RELAY_2_GPIO_Port, RELAY_2_Pin, GPIO_PIN_RESET);	//
#define RELAY_2_TOGGLE 			HAL_GPIO_TogglePin(RELAY_2_GPIO_Port, RELAY_2_Pin);

#define RELAY_3_OFF 			HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_SET);	//
#define RELAY_3_ON 				HAL_GPIO_WritePin(RELAY_3_GPIO_Port, RELAY_3_Pin, GPIO_PIN_RESET);	//
#define RELAY_3_TOGGLE 			HAL_GPIO_TogglePin(RELAY_3_GPIO_Port, RELAY_3_Pin);

#define RELAY_4_OFF 			HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_SET);	//
#define RELAY_4_ON 				HAL_GPIO_WritePin(RELAY_4_GPIO_Port, RELAY_4_Pin, GPIO_PIN_RESET);	//
#define RELAY_4_TOGGLE 			HAL_GPIO_TogglePin(RELAY_4_GPIO_Port, RELAY_4_Pin);

#define LED_1_OFF 				HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);	//
#define LED_1_ON 				HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);	//
#define LED_1_TOGGLE 			HAL_GPIO_TogglePin (LED_1_GPIO_Port, LED_1_Pin);	//

#define LED_2_OFF 				HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);	//
#define LED_2_ON 				HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);	//
#define LED_2_TOGGLE 			HAL_GPIO_TogglePin (LED_2_GPIO_Port, LED_2_Pin);	//

#define QCA_CS_OFF 				HAL_GPIO_WritePin(QCA_CS_GPIO_Port, QCA_CS_Pin, 0) //6	//2
#define QCA_CS_ON 				HAL_GPIO_WritePin(QCA_CS_GPIO_Port, QCA_CS_Pin, 1)	//6 //22

#define QCA7000_INT_READ		HAL_GPIO_ReadPin (QCA_INT_GPIO_Port, QCA_INT_Pin)


void app_init();
void app_run();

#endif /* INC_APP_H_ */
