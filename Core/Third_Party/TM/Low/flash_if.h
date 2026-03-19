/**
  ******************************************************************************
  * @file    IAP_Main/Inc/flash_if.h 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the headers of the flash_if functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


/* Error code */
enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
};

/* protection type */  
enum{
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};

/* protection update */
enum {
	FLASHIF_WRP_ENABLE,
	FLASHIF_WRP_DISABLE
};

/* Define the address from where user application will be loaded.
   Note: this area is reserved for the IAP code                  */
#define FLASH_PAGE_STEP         FLASH_PAGE_SIZE           /* Size of page : 2 Kbytes */
#define APPLICATION_ADDRESS     (uint32_t)0x08008000      /* Start user code address: ADDR_FLASH_PAGE_8 */  //8000 for L476  4000 for f103

/* Notable Flash addresses */
//#define USER_FLASH_END_ADDRESS        0x08080000  // x20000‬
#define USER_FLASH_END_ADDRESS (FLASH_START_ADRESS + FLASH_SIZE)

/* Define the user application size */
#define USER_FLASH_SIZE               ((uint32_t)0x00008000) /* Small default template application */ // size of  appllication
//#define USER_FLASH_SIZE               (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1) /* Small default template application */


/* Exported macro ------------------------------------------------------------*/
/* ABSoulute value */
#define ABS_RETURN(x,y)               ((x) < (y)) ? ((y)-(x)) : ((x)-(y))

/* Get the number of sectors from where the user program will be loaded */
#define FLASH_SECTOR_NUMBER           ((uint32_t)(ABS_RETURN(APPLICATION_ADDRESS,FLASH_START_BANK1))>>12)

/* Compute the mask to test if the Flash memory, where the user program will be
  loaded, is write protected */
#define FLASH_PROTECTED_SECTORS       (~(uint32_t)((1 << FLASH_SECTOR_NUMBER) - 1))

#define FLASH_BANK FLASH_BANK_2 // I also tried FLASH_BANK_2
#define FLASH_BANK_ADDRESS ((FLASH_BANK == FLASH_BANK_1) ? 0x08000000U : 0x08040000U)		//FLASH_BANK_2 0x0804 0000 - 0x0804 07FF.
#define FLASH_PAGE_N 127 // I also tried many other pages	//maximum 127
#define FLASH_PAGE_BYTES_N 0x800U // 2kb
#define FLASH_PAGE_ADDRESS (FLASH_BANK_ADDRESS + (FLASH_PAGE_N * FLASH_PAGE_BYTES_N))
#define DOUBLEWORD_BYTES_N 8 // 8*8 == 64 bit == sizeof(uint64_t)
#define FLASH_ADDRESS_VALUE_1 (FLASH_PAGE_ADDRESS + (DOUBLEWORD_BYTES_N * 0))
#define FLASH_ADDRESS_VALUE_2 (FLASH_PAGE_ADDRESS + (DOUBLEWORD_BYTES_N * 1))


/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_GetWriteProtectionStatus(void);
uint32_t FLASH_If_Write(uint32_t destination, uint64_t *p_source, uint32_t length);
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate);
uint32_t FLASH_If_Erase_Page(uint32_t start);
enum ResultEnum{eFAIL, eSUCCESS};
enum ResultEnum ProgramFirmwarePacket(uint32_t programming_address);
enum ResultEnum ProgramData(uint32_t programming_address,uint64_t data);

uint32_t FLASH_If_Erase_Page_Bank_2();
uint32_t FLASH_If_Write_Bank_2(uint32_t destination, uint64_t *p_source, uint32_t length);
#endif  /* __FLASH_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
