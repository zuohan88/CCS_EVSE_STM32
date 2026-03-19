#ifndef CCS32_GLOBALS_H_
#define CCS32_GLOBALS_H_
/* global include file, which includes all other include files */


#include <string.h> /* memcpy */
#include <stdio.h>
#include <stdlib.h> /* abs */
#include <stdint.h>


#include "configuration.h"
//#include "connMgr.h"

//#include "homeplug.h"
#include "myHelpers.h"

#include "McuXFormat.h"

#include "main.h"

//#include "qca7000.h"
//#include "tcp.h"
//#include "udp.h"
//#include "udpChecksum.h"



/* temporary stubs */
//#define publishStatus(x, y)
//#define log_v(x, ...)

//#define canbus_addToBinaryLogging(x,y,z)

//#define canbus_addStringToTextTransmitBuffer(x)

#define iAmEvse 1
#define iAmPev 0

#define u8(x) *((int8_t *)&x)
#define u16(x) *((int16_t *)&x)
#define m16(a,b) ( (((uint16_t)a)<<8) | (uint16_t)b )
#define m32(a,b,c,d) ( (((uint32_t)a)<<24) | (((uint32_t)b)<<16) | (((uint32_t)c)<<8) | (uint32_t)d )




#endif
