/*
 * tm.h
 *
 *  Created on: Mar 21, 2020
 *      Author: zuoha
 */

#ifndef INC_TM_SC_H_
#define INC_TM_SC_H_

#include "Low/ringbuff.h"





//**********************************************************
//port 1 function
//**********************************************************
void usart_init_1(void);
void usart_process_data_1(const void* data, size_t len);
void uasrt_write_1(const char* str, size_t len);
void usart_send_string_1(const char* str);
uint8_t usart_start_tx_dma_transfer_1(void);
void usart_rx_check_1(void);
size_t usart_read_buff_1(void);
size_t usart_read_char_buff_1(char *b);

void USER_UART_IRQHandler(UART_HandleTypeDef *huart);

uint8_t UART_Handle_TO_CDC_CH(UART_HandleTypeDef *handle);
void ComPort_Config(uint8_t cdc_ch);

#endif /* INC_TM_H_ */
