/*
 * tm.c
 *
 *  Created on: Mar 21, 2020
 *      Author: zuoha
 */
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "myHelpers.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_usart5_rx;
DMA_HandleTypeDef hdma_usart5_tx;

#define DMA_TX_BUFFER_SIZE 		2048      	//128 or 1024 and above
#define RING_TX_BUFFER_SIZE 	4096		//sizeof(usart_tx_dma_ringbuff_data_1)

#define DMA_RX_BUFFER_SIZE 		128      	//128 or 1024 and above
#define RING_RX_BUFFER_SIZE 	128		//sizeof(usart_tx_dma_ringbuff_data_1)

//**********************************************************
//port 1
//**********************************************************
uint8_t usart_rx_dma_buffer_5[DMA_RX_BUFFER_SIZE];		//this is for DMA
ringbuff_t usart_rx_dma_ringbuff_5;
uint8_t usart_rx_dma_ringbuff_data_5[RING_RX_BUFFER_SIZE];		//this is for ring buffer

ringbuff_t usart_tx_dma_ringbuff_5;
uint8_t usart_tx_dma_ringbuff_data_5[RING_TX_BUFFER_SIZE];
size_t usart_tx_dma_current_len_5;


#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))


//**********************************************************
//port 1 function
//**********************************************************
void usart_init_1(void) {
    /* Initialize ringbuff for TX & RX */
    ringbuff_init(&usart_tx_dma_ringbuff_5, usart_tx_dma_ringbuff_data_5, RING_TX_BUFFER_SIZE);
    ringbuff_init(&usart_rx_dma_ringbuff_5, usart_rx_dma_ringbuff_data_5, RING_RX_BUFFER_SIZE);

    HAL_UART_Receive_DMA(&huart5, usart_rx_dma_buffer_5, DMA_RX_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);   // enable idle line interrupt
    //__HAL_UART_CLEAR_IDLEFLAG(&huart2);				//too fast at starting
    __HAL_DMA_ENABLE_IT (&hdma_usart5_rx, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT (&hdma_usart5_rx, DMA_IT_TC);  // enable DMA Tx cplt interruptDMA_IT_HT
    //ringbuff_reset(&usart_rx_dma_ringbuff_1);		//got bug at beginning of __HAL_DMA_GET_COUNTER(&hdma_usart2_rx) = 0
}

static size_t old_pos_1;
void usart_rx_check_reset_1(void){
	old_pos_1 =0;
}

void usart_rx_check_1(void) {
    //static size_t old_pos;
    size_t pos;

    /* Calculate current position in buffer */
    pos = ARRAY_LEN(usart_rx_dma_buffer_5) - __HAL_DMA_GET_COUNTER(huart5.hdmarx);//LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);__HAL_DMA_GET_COUNTER(&hdma_usart5_rx);
    if (pos != old_pos_1) {                       /* Check change in received data */
        if (pos > old_pos_1) {                    /* Current position is over previous one */
            /* We are in "linear" mode */
            /* Process data directly by subtracting "pointers" */
            usart_process_data_1(&usart_rx_dma_buffer_5[old_pos_1], pos - old_pos_1);
        } else {
            /* We are in "overflow" mode */
            /* First process data to the end of buffer */
            usart_process_data_1(&usart_rx_dma_buffer_5[old_pos_1], ARRAY_LEN(usart_rx_dma_buffer_5) - old_pos_1);
            /* Check and continue with beginning of buffer */
            if (pos > 0) {
                usart_process_data_1(&usart_rx_dma_buffer_5[0], pos);
            }
        }
    }
    old_pos_1 = pos;                              /* Save current position as old */

    /* Check and manually update if we reached end of buffer */
    if (old_pos_1 == ARRAY_LEN(usart_rx_dma_buffer_5)) {
        old_pos_1 = 0;
    }
}

void usart_process_data_1(const void* data, size_t len) {
    ringbuff_write(&usart_rx_dma_ringbuff_5, data, len);  /* Write data to receive buffer */
    usart_start_tx_dma_transfer_1();
}

void uasrt_write_1(const char* str, size_t len) {
    ringbuff_write(&usart_tx_dma_ringbuff_5, str, len);   /* Write data to transmit buffer */
    usart_start_tx_dma_transfer_1();
}

void usart_send_string_1(const char* str) {
	uasrt_write_1(str, strlen(str));   /* Write data to transmit buffer */
}


uint8_t usart_start_tx_dma_transfer_1(void) {
    uint32_t old_primask;
    uint8_t started = 0;

    /* Check if DMA is active */
    /* Must be set to 0 */
    old_primask = __get_PRIMASK();
    __disable_irq();
    /* Check if transfer is not active */
    if (usart_tx_dma_current_len_5 == 0) {			//no data in dma 1

        /* Check if something to send  */
        usart_tx_dma_current_len_5 = ringbuff_get_linear_block_read_length(&usart_tx_dma_ringbuff_5);
        if (usart_tx_dma_current_len_5 > 0) {
      	    HAL_UART_Transmit_DMA(&huart5, (uint32_t)ringbuff_get_linear_block_read_address(&usart_tx_dma_ringbuff_5), usart_tx_dma_current_len_5);
      	    started = 1;
        }
    }
    __set_PRIMASK(old_primask);
    return started;
}
//**********************************************************************************************************
size_t usart_read_buff_1(void){
	return ringbuff_get_full(&usart_rx_dma_ringbuff_5);
}

size_t usart_read_char_buff_1(char *b){
	return ringbuff_read(&usart_rx_dma_ringbuff_5, b, 1);
}



//**********************************************************************************************************
//ISR UART TX
//**********************************************************************************************************
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)		//TC
{
	//usart_rx_check();
	if (huart->Instance == UART5){
	    ringbuff_skip(&usart_tx_dma_ringbuff_5, usart_tx_dma_current_len_5);/* Skip sent data, mark as read */
	    usart_tx_dma_current_len_5 = 0;           /* Clear length variable */
	    usart_start_tx_dma_transfer_1();          /* Start sending more data */
	}


}

//**********************************************************************************************************
//ISR UART RX
//**********************************************************************************************************

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART5){
		usart_rx_check_1();

	}


}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == UART5){
		usart_rx_check_1();
	}


}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(huart->Instance == UART5)                                   //Determine whether it is serial port 1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE))   //Judging whether it is idle interruption
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart5);                     //Clear idle interrupt sign (otherwise it will continue to enter interrupt)
            usart_rx_check_1();

        }
    }



}



void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance == UART5)
	{

//		__HAL_UART_CLEAR_OREFLAG(&huart1);
		if(huart->ErrorCode == HAL_UART_ERROR_DMA)
		{
		//#define  HAL_UART_ERROR_NONE             ((uint32_t)0x00000000U)    /*!< No error                */
		//#define  HAL_UART_ERROR_PE               ((uint32_t)0x00000001U)    /*!< Parity error            */
		//#define  HAL_UART_ERROR_NE               ((uint32_t)0x00000002U)    /*!< Noise error             */
		//#define  HAL_UART_ERROR_FE               ((uint32_t)0x00000004U)    /*!< Frame error             */
		//#define  HAL_UART_ERROR_ORE              ((uint32_t)0x00000008U)    /*!< Overrun error           */
		//#define  HAL_UART_ERROR_DMA              ((uint32_t)0x00000010U)    /*!< DMA transfer error      */
		}
		//HAL_UART_DMAStop(&huart1);
		//HAL_UART_DMAResume(&huart1);
		usart_rx_check_reset_1();
		HAL_UART_Receive_DMA(&huart5, usart_rx_dma_buffer_5, DMA_RX_BUFFER_SIZE);		//MANY BUG HERE
		HAL_UART_DMAResume(&huart5);											//MANY BUG HERE
	}

 }

void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{

}

//**********************************************************************************************************
//UART RX
//**********************************************************************************************************





