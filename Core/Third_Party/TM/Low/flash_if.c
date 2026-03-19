/**
  ******************************************************************************
  * @file    IAP_Main/Src/flash_if.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the memory related operation functions.
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

/** @addtogroup STM32F1xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*  1M0 flash 1 * 1024 * 1024 */
#define FLASH_START_ADRESS    0x08000000
//#define FLASH_PAGE_NBPERBANK  256
//#define FLASH_BANK_NUMBER     2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */




void FLASH_If_Init(void)
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
  //__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
  //FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR| FLASH_FLAG_OPTVERR |
  //FLASH_FLAG_BSY | FLASH_FLAG_ECCC | FLASH_FLAG_ECCD | FLASH_FLAG_ECCC2 | FLASH_FLAG_ECCD2  );

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);//FLASH_FLAG_OPERR
    /* Unlock the Program memory */
  HAL_FLASH_Lock();
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  start: start of user flash area
  * @retval FLASHIF_OK : user flash area successfully erased
  *         FLASHIF_ERASEKO : error occurred
  */
uint32_t FLASH_If_Erase(uint32_t start)
{
  uint32_t NbrOfPages = 0;
  uint32_t PageError = 0;
  FLASH_EraseInitTypeDef pEraseInit;
  HAL_StatusTypeDef status = HAL_OK;

  /* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();

  /* Get the sector where start the user flash area */
//  NbrOfPages = (USER_FLASH_END_ADDRESS - start)/FLASH_PAGE_SIZE;		//number of page to be delete		//start address need to modify
  NbrOfPages = (FLASH_START_ADRESS + FLASH_SIZE);		//FLASH_START_ADRESS ? FLASH_PAGE_SIZE=2k			//this kill all data
  NbrOfPages = (NbrOfPages - start) / FLASH_PAGE_SIZE;

  pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  //pEraseInit.Page = start;
  pEraseInit.Page = FLASH_PAGE_NB - NbrOfPages;									//1st page
  pEraseInit.NbPages = NbrOfPages;
  status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  if (status != HAL_OK)
  {
    /* Error occurred while page erase */
    return FLASHIF_ERASEKO;
  }

  return FLASHIF_OK;
}

//*********************************
//erase page only
//*********************************
uint32_t FLASH_If_Erase_Page(uint32_t start)		//page number,erase 1 page only
{
  uint32_t NbrOfPages = 1;
  uint32_t PageError = 0;
  FLASH_EraseInitTypeDef pEraseInit;
  HAL_StatusTypeDef status = HAL_OK;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  pEraseInit.Page = start;

  pEraseInit.NbPages = NbrOfPages;
  status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  if (status != HAL_OK)
  {
    /* Error occurred while page erase */
    return FLASHIF_ERASEKO;
  }

  return FLASHIF_OK;
}

/* Public functions ---------------------------------------------------------*/
/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  destination: start address for target location
  * @param  p_source: pointer on buffer with data to write
  * @param  length: length of data buffer (unit is 32-bit word)
  * @retval uint32_t 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint64_t *pp_source=0;
uint32_t FLASH_If_Write(uint32_t destination, uint64_t *p_source, uint32_t length)
{

  uint32_t status = FLASHIF_OK;
  uint32_t i = 0;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
  //__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
  //FLASH_FLAG_SIZERR | FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR| FLASH_FLAG_OPTVERR |
  //FLASH_FLAG_BSY | FLASH_FLAG_ECCC | FLASH_FLAG_ECCD | FLASH_FLAG_ECCC2 | FLASH_FLAG_ECCD2  );


  /* DataLength must be a multiple of 64 bit */
  //flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len)
  for (i = 0; (i < length/2) && (destination <= (USER_FLASH_END_ADDRESS-8)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
	pp_source = (uint64_t *)(p_source+(2*i));
	//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, 0xffffffff) == HAL_OK)
//	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, *pp_source) == HAL_OK)

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, *((uint64_t *)(p_source+2*i))) == HAL_OK)
    {
     /* Check the written value */
      if (*(uint64_t*)destination != *(uint64_t *)(p_source+2*i))
      {
        /* Flash content doesn't match SRAM content */
        status = FLASHIF_WRITINGCTRL_ERROR;
        break;
      }
      /* Increment FLASH destination address */
      destination += 8;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      status = FLASHIF_WRITING_ERROR;
      break;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return status;
}

/**
  * @brief  Returns the write protection status of application flash area.
  * @param  None
  * @retval If a sector in application area is write-protected returned value is a combinaison
            of the possible values : FLASHIF_PROTECTION_WRPENABLED, FLASHIF_PROTECTION_PCROPENABLED, ...
  *         If no sector is write-protected FLASHIF_PROTECTION_NONE is returned.
  */
uint32_t FLASH_If_GetWriteProtectionStatus(void)
{
  uint32_t ProtectedPAGE = FLASHIF_PROTECTION_NONE;
  FLASH_OBProgramInitTypeDef OptionsBytesStruct1,OptionsBytesStruct2,OptionsBytesStruct3,OptionsBytesStruct4;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();


  OptionsBytesStruct1.WRPArea = OB_WRPAREA_BANK1_AREAA;//Flash Bank 1 Area A */
  OptionsBytesStruct2.WRPArea = OB_WRPAREA_BANK1_AREAB;//Flash Bank 1 Area B */
  OptionsBytesStruct3.WRPArea = OB_WRPAREA_BANK2_AREAA;//Flash Bank 2 Area A */
  OptionsBytesStruct4.WRPArea = OB_WRPAREA_BANK2_AREAB;//Flash Bank 2 Area B */


  /* Check if there are write protected sectors inside the user flash area ***/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct1);
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct2);
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct3);
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct4);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();



  /* check WRP */
  if(OptionsBytesStruct1.WRPEndOffset > OptionsBytesStruct1.WRPStartOffset)
  {
    /* check if area is inside the WRP Range */
    if((OptionsBytesStruct1.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APPLICATION_ADDRESS)
    {
       ProtectedPAGE|= FLASHIF_PROTECTION_WRPENABLED;
    }
  }

  if(OptionsBytesStruct2.WRPEndOffset > OptionsBytesStruct2.WRPStartOffset)
  {
    /* check if area is inside the WRP Range */
    if((OptionsBytesStruct2.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APPLICATION_ADDRESS)
    {
       ProtectedPAGE|= FLASHIF_PROTECTION_WRPENABLED;
    }
  }
  if(OptionsBytesStruct3.WRPEndOffset > OptionsBytesStruct3.WRPStartOffset)
  {
    /* check if area is inside the WRP Range */
    if((OptionsBytesStruct3.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APPLICATION_ADDRESS)
    {
       ProtectedPAGE|= FLASHIF_PROTECTION_WRPENABLED;
    }
  }
  if(OptionsBytesStruct4.WRPEndOffset > OptionsBytesStruct4.WRPStartOffset)
  {
    /* check if area is inside the WRP Range */
    if((OptionsBytesStruct4.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APPLICATION_ADDRESS)
    {
       ProtectedPAGE|= FLASHIF_PROTECTION_WRPENABLED;
    }
  }

  if(OptionsBytesStruct2.RDPLevel == 1)
  {
    ProtectedPAGE|= FLASHIF_PROTECTION_RDPENABLED;
  }

  return ProtectedPAGE;
}

/**
  * @brief  Configure the write protection status of user flash area.
  * @param  protectionstate : FLASHIF_WRP_DISABLE or FLASHIF_WRP_ENABLE the protection
  * @retval uint32_t FLASHIF_OK if change is applied.
  */
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate)
{
  FLASH_OBProgramInitTypeDef OptionsBytesStruct1;
  HAL_StatusTypeDef retr;
  
  /* Unlock the Flash to enable the flash control register access *************/
  retr = HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  retr|= HAL_FLASH_OB_Unlock();
  
  OptionsBytesStruct1.OptionType = OPTIONBYTE_WRP;
  OptionsBytesStruct1.WRPArea = OB_WRPAREA_BANK1_AREAA;
  if( protectionstate == FLASHIF_WRP_ENABLE)
  {
    /* Enable the WRP protection for all flash BANK1 */
	OptionsBytesStruct1.WRPStartOffset = 0x00;
    OptionsBytesStruct1.WRPEndOffset = FLASH_PAGE_NB - 1;

  }
  else
  {
    /* Remove all WRP protection */
    OptionsBytesStruct1.WRPEndOffset = 0x00;
    OptionsBytesStruct1.WRPStartOffset = 0xFF;
  }
  retr|= HAL_FLASHEx_OBProgram(&OptionsBytesStruct1);
  
  OptionsBytesStruct1.RDPLevel = OB_RDP_LEVEL_0;
  OptionsBytesStruct1.OptionType = OPTIONBYTE_WRP;
  OptionsBytesStruct1.WRPArea = OB_WRPAREA_BANK1_AREAB;
  OptionsBytesStruct1.WRPEndOffset = 0x00;
  OptionsBytesStruct1.WRPStartOffset = 0xFF;
  retr|= HAL_FLASHEx_OBProgram(&OptionsBytesStruct1);

  return (retr == HAL_OK ? FLASHIF_OK: FLASHIF_PROTECTION_ERRROR);
}


//#pragma optimize=medium

#define FIRMWARE_PACKET_SIZE 50
enum ResultEnum ProgramFirmwarePacket(uint32_t programming_address)
{
  uint32_t address = 0;
  static uint64_t decripted_firmware_data[FIRMWARE_PACKET_SIZE]={10};
  uint64_t *programmed_data = (uint64_t *)programming_address;
  uint16_t i;
  uint32_t err;

//here I decripted the data and store in decripted_firmware_data :-)

  address = programming_address;

  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  for (i = 0; i < FIRMWARE_PACKET_SIZE; ++i)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, decripted_firmware_data[i]) != HAL_OK)
    {
    	err = HAL_FLASH_GetError();

    	if(err & HAL_FLASH_ERROR_OP){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_PROG){//
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_WRP){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_PGA){	//FLASH Programming Alignment error flag
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_SIZ){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_PGS){ //Programming Sequence error;
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_MIS){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_FAST){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_RD){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_OPTV){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_ECCC){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_ECCD){
    		__NOP();
    	}
    	if(err & FLASH_OPTR_DBANK){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_ECCC2){
    		__NOP();
    	}
    	if(err & HAL_FLASH_ERROR_ECCD2){
    		__NOP();
    	}




      /* Lock the Flash to disable the flash control register access (recommended
         to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();
      return eFAIL;
    }
    else
    {
      if (*(programmed_data + i) != decripted_firmware_data[i])
      {
        /* Lock the Flash to disable the flash control register access (recommended
           to protect the FLASH memory against possible unwanted operation) *********/
        HAL_FLASH_Lock();

        return eFAIL;
      }
    }
    address += sizeof(uint64_t);
  }

  return eSUCCESS;
}


enum ResultEnum ProgramData(uint32_t programming_address,uint64_t data)
{
  uint32_t address = 0;
  static uint64_t decripted_firmware_data[FIRMWARE_PACKET_SIZE]={0};
  uint64_t *programmed_data = (uint64_t *)programming_address;
  uint16_t i;

//here I decripted the data and store in decripted_firmware_data :-)

  address = programming_address;

  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);


  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data) != HAL_OK)
  {
      HAL_FLASH_GetError();
      /* Lock the Flash to disable the flash control register access (recommended
         to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();
      return eFAIL;
  }
  else
  {
	  if (*(programmed_data ) != data)
      {
        /* Lock the Flash to disable the flash control register access (recommended
           to protect the FLASH memory against possible unwanted operation) *********/
        HAL_FLASH_Lock();

        return eFAIL;
      }
  }

  return eSUCCESS;
}

void deleteFlash(void){

	int value_1 = 1000;
	int value_2 = 10;

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_FLASH_CLK_ENABLE();

	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	FLASH_EraseInitTypeDef flash_erase_init =
	{
	.TypeErase = FLASH_TYPEERASE_PAGES,
	.Banks = FLASH_BANK,
	.Page = FLASH_PAGE_N,
	.NbPages = 1
	};
	uint32_t flash_page_erase_error = 0;
	// ret is always HAL_OK_:
	uint32_t ret = HAL_FLASHEx_Erase(&flash_erase_init, &flash_page_erase_error);

	// This works fine (ret == HAL_OK) only the first time after changing FLASH_PAGE_N,
	// even though I comment the HAL_FLASHEx_Erase() above:
	ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_ADDRESS_VALUE_1, (uint64_t)(value_1));
	// This always fails (ret == HAL_ERROR):
	ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_ADDRESS_VALUE_2, (uint64_t)(value_2));

	// value_1_read is == value_1:
	uint64_t value_1_read = ((__IO uint64_t)FLASH_ADDRESS_VALUE_1);
	// value_2_read is wrong:
	uint64_t value_2_read = ((__IO uint64_t)FLASH_ADDRESS_VALUE_2);

	HAL_FLASH_Lock();
}

uint32_t FLASH_If_Erase_Page_Bank_2()		//page number,erase 1 page only
{
  uint32_t NbrOfPages = 1;
  uint32_t PageError = 0;
  FLASH_EraseInitTypeDef pEraseInit;
  HAL_StatusTypeDef status = HAL_OK;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  pEraseInit.Page = FLASH_PAGE_N;
  pEraseInit.Banks = FLASH_BANK;
  pEraseInit.NbPages = NbrOfPages;
  status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
  if(status != HAL_OK){
	  __NOP();

  }
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  if (status != HAL_OK)
  {
    /* Error occurred while page erase */
    return FLASHIF_ERASEKO;
  }

  return FLASHIF_OK;
}


uint32_t FLASH_If_Write_Bank_2(uint32_t destination, uint64_t *p_source, uint32_t length)
  {

    uint32_t status = FLASHIF_OK;
    uint32_t i = 0;
    uint32_t address = FLASH_PAGE_ADDRESS + (DOUBLEWORD_BYTES_N * destination);
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* DataLength must be a multiple of 64 bit */
    //flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len)
    for (i = 0; (i < length) && (address <= (USER_FLASH_END_ADDRESS-8)); i++)
    {
      /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
         be done by word */

  	//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, 0xffffffff) == HAL_OK)
  //	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination, *pp_source) == HAL_OK)
    	//FLASH_ADDRESS_VALUE_2 (FLASH_PAGE_ADDRESS + (DOUBLEWORD_BYTES_N * 1))
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *((uint64_t *)(p_source+i))) == HAL_OK)
      {
       /* Check the written value */
        if (*(uint64_t*)address != *(uint64_t *)(p_source+i))
        {
          /* Flash content doesn't match SRAM content */
          status = FLASHIF_WRITINGCTRL_ERROR;
          break;
        }
        /* Increment FLASH destination address */
        address += 8;
      }
      else
      {
        /* Error occurred while writing data in Flash memory */
        status = FLASHIF_WRITING_ERROR;
        break;
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return status;
  }

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
