/*
 * tm.h
 *
 *  Created on: Mar 21, 2020
 *      Author: zuoha
 */

#ifndef INC_TM_CDC_H_
#define INC_TM_CDC_H_

#include "ringbuff.h"



//**********************************************************
//port 1 function
//**********************************************************
void CDC_init_0(void);
void CDC_process_data_0(const void* data, size_t len);
void CDC_write_0(const char* str, size_t len);
void CDC_send_string_0(const char* str);
uint8_t CDC_start_tx_dma_transfer_0(void);
void CDC_rx_check_0(void);
void CDC_reset_tx_0(void);

size_t CDC_rx_buffsize_0(void);
size_t CDC_rx_readbuff_0(char *buff,size_t buffsize);
void CDC_rx_tx_test_0(void);

void USER_CDC_TxCpltcallBack_0(uint8_t *Buf, uint32_t Len);
void USER_CDC_RxHandler_0(uint8_t* Buf, uint32_t Len);



#endif /* INC_TM_H_ */
