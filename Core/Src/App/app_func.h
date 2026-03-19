/*
 * app_func.h
 *
 *  Created on: 7 Mar 2020
 *      Author: zuoha
 */

#ifndef INC_APP_FUNC_H_
#define INC_APP_FUNC_H_



void ROM_TEST(void);
uint64_t rom_read(uint32_t number);
void rom_write(uint8_t number,uint64_t wdata);
void Process(int command,int data);

uint32_t my_add_read(void);
uint32_t my_add_write(uint32_t number);

int xsprintfln_user(char *buf, const char *fmt, ...);



//#define reg_my_addr		1
#define reg_direction	1
#define reg_srate 		2
#define reg_threshold	3
#define reg_window_time 4
#define reg_gforce		5

//#define my_add_read 		rom_read(1)
//#define my_add_write(x)		rom_write(1,x)
//#define last_add_read		rom_read(2)
//#define last_add_write(x)	rom_write(2,x)
//#define dir_read			rom_read(3)
//#define	dir_write(x)		rom_read(3,x)
//#define	threshold_read		rom_read(4)
//#define threshold_write(x)	rom_read(4,x)



int isNumber(char *s);

#endif /* INC_APP_FUNC_H_ */
