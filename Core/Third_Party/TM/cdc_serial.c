/*
 * tm.c
 *
 *  Created on: Mar 21, 2020
 *      Author: zuoha
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "usbd_cdc_acm_if.h"
#include "cdc_serial.h"
#include "main.h"
/* Private variables ---------------------------------------------------------*/

extern USBD_HandleTypeDef hUsbDevice;//hUsbDeviceFS;	//from usb_device
host host_com;			//from cdc_acm_if


#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))

#define RING_BUFFER_SIZE 		512		//sizeof(CDC_tx_ringbuff_data)
#define RING_BUFFER_TX_SIZE 	512
#define RING_BUFFER_RX_SIZE 	512
//**********************************************************
//port 1
//**********************************************************

size_t CDC_tx_dma_current_len_0;
ringbuff_t CDC_rx_ringbuff_0;
ringbuff_t CDC_tx_ringbuff_0;
uint8_t CDC_rx_ringbuff_data_0[RING_BUFFER_SIZE];
uint8_t CDC_tx_ringbuff_data_0[RING_BUFFER_SIZE];




//**********************************************************
//port 1 function
//**********************************************************

void CDC_init_0(void) {
    /* Initialize ringbuff for TX & RX */
    ringbuff_init(&CDC_tx_ringbuff_0, CDC_tx_ringbuff_data_0, RING_BUFFER_SIZE);
    ringbuff_init(&CDC_rx_ringbuff_0, CDC_rx_ringbuff_data_0, RING_BUFFER_SIZE);
}


size_t CDC_rx_buffsize_0(void){
	return ringbuff_get_full(&CDC_rx_ringbuff_0);
}


size_t CDC_rx_readbuff_0(char *buff,size_t buffsize){
	return ringbuff_read(&CDC_rx_ringbuff_0, &buff, buffsize);
}


void CDC_rx_tx_test_0(void){
	uint16_t buffsize = 0;
	char buff[100]={0};

	buffsize = ringbuff_get_full(&CDC_rx_ringbuff_0);
	if(buffsize){
		ringbuff_read(&CDC_rx_ringbuff_0, &buff, buffsize);
		CDC_write_0("RX:",3);
		CDC_write_0(buff,buffsize);
	}
}



//void CDC_process_data_1(const void* data, size_t len) {
	//ringbuff_write(&CDC_rx_ringbuff, data, len);  /* Write data to receive buffer */
//}
void CDC_write_0(const char* str, size_t len) {

	if((hUsbDevice.dev_state == USBD_STATE_CONFIGURED) && (host_com.port_0 == 1)){	// device is connected. do something
		ringbuff_write(&CDC_tx_ringbuff_0, str, len);   /* Write data to transmit buffer */
		CDC_start_tx_dma_transfer_0();
	}else{	//disconnected.do someting else.
		CDC_reset_tx_0();					//clean up the buffer
		CDC_tx_dma_current_len_0 = 0;		//
	}
}

void CDC_send_string_0(const char* str) {
	CDC_write_0(str,strlen(str));
}

void CDC_reset_tx_0(void){
	ringbuff_reset(&CDC_tx_ringbuff_0);
}

uint8_t CDC_start_tx_dma_transfer_0(void) {
    uint32_t old_primask;
    uint8_t started = 0;	    /* Must be set to 0 */
    uint8_t res = 0;

    old_primask = __get_PRIMASK();
    __disable_irq();

    if (CDC_tx_dma_current_len_0 == 0) {			 /* Check if transfer is not active */
        /* Check if something to send  */
        CDC_tx_dma_current_len_0 = ringbuff_get_linear_block_read_length(&CDC_tx_ringbuff_0);
        if (CDC_tx_dma_current_len_0 > 0) {
         	res = CDC_Transmit(0, ringbuff_get_linear_block_read_address(&CDC_tx_ringbuff_0), CDC_tx_dma_current_len_0);
      	    if(res== USBD_OK){					// if failed to send
      	    	started = 1;
      	    }else{
      	    	CDC_tx_dma_current_len_0 = 0;
      	    	//started = 0;
      	    }
        }
    }
    __set_PRIMASK(old_primask);
    return started;
}


//**********************************************************************************************************
//ISR UART TX
//**********************************************************************************************************
void USER_CDC_TxCpltcallBack_0(uint8_t *Buf, uint32_t Len){
    ringbuff_skip(&CDC_tx_ringbuff_0, Len);/* Skip sent data, mark as read *///CDC_tx_dma_current_len
    CDC_tx_dma_current_len_0 = 0;           /* Clear length variable */
    CDC_start_tx_dma_transfer_0();          /* Start sending more data */
}


//**********************************************************************************************************
//ISR UART RX
//**********************************************************************************************************
void USER_CDC_RxHandler_0(uint8_t* Buf, uint32_t Len){
	ringbuff_write(&CDC_rx_ringbuff_0, Buf, Len);  /* Write data to receive buffer */
}








