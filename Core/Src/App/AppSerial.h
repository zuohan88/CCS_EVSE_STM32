/*
 * AppSerial.h
 *
 *  Created on: Feb 9, 2025
 *      Author: zuoha
 */

#ifndef SRC_APP_APPSERIAL_H_
#define SRC_APP_APPSERIAL_H_

void mySerialPrint(void);


void AppSerial_SerialTask(void);
void AppSerial_CDCTask(void);

void AppSerial_CheckUSB(void);

#endif /* SRC_APP_APPSERIAL_H_ */
