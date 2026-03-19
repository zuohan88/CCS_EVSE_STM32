/*
 * app_func.c
 *
 *  Created on: 7 Mar 2020
 *      Author: zuoha
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_func.h"
#include "serial.h"
#include "flash_if.h"
#include "ccs32_globals.h"
#include "cdc_serial.h"
#include "usbd_cdc_acm_if.h"
#include "evseStateMachine.h"
#include "evsePredefine.h"
#include "projectExiConnector.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "TM/accel.h"
//#include "TM/Low/FlexiTimer.h"
//#include "TM/app_func.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define len 100						//whole page is 2048 divide by 8 byte = 256
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */







//**********************************************************************************************************************
//ROM read write
//**********************************************************************************************************************
#define address 0x080BF800UL

uint64_t rom_read(uint32_t number){
		uint64_t fdata=*(volatile uint64_t *)(FLASH_PAGE_ADDRESS+(number*DOUBLEWORD_BYTES_N));
	return fdata;
}


void rom_write(uint8_t number,uint64_t wdata){
	uint64_t data[len];
	uint32_t i;
	for(i=0;i<len;i++)
	{
		data[i]=*(volatile uint64_t *)(FLASH_PAGE_ADDRESS+(i*DOUBLEWORD_BYTES_N));
	}
	data[number]=wdata;				//replace
	FLASH_If_Erase_Page_Bank_2(); //127 is last page whole page deleted
	FLASH_If_Write_Bank_2(0, data, len);// read back whole page
}

void ROM_TEST(void){
	FLASH_If_Init();
	uint64_t a = rom_read(0);
	uint8_t result = 0;
	uint64_t b = 50;
	rom_write(0,b);
	rom_write(1,100);
	rom_write(2,300);
	a = rom_read(1);
	a = rom_read(2);
	__NOP();
	__NOP();
}

void Process(int command,int data){
	 switch(command)									// parameter?
	 {
	 	 case 1:
	 		 break;
	 	 case 2:
	      	 break;
	 	 default:
	 		 break;
	 }

}



int isNumber(char *s){
    for (int i = 0; i < strlen(s); i++)
        if (isdigit((unsigned char)s[i]) == false)
            return false;
    return true;
}


int isAllNum(char *s){
    for (int i = 0; i < strlen(s); i++)
        if (isalnum((unsigned char)s[i]) == false)
            return false;
    return true;
}

int xsprintfln_user(char *buf, const char *fmt, ...)
{
	va_list args;
	int res;

	va_start(args,fmt);
	res = xsprintf(buf, fmt, args);
	va_end(args);
	addToTrace(buf);
	return res;

}

