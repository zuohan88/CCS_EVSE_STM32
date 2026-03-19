
/* Interface header for myHelpers.c */
#ifndef MYHELPERS_H_
#define MYHELPERS_H_
/* Global Defines */

#define STR_TMP_SIZE 1000
#define MY_SERIAL_PRINTBUFFERLEN 1000

/* Global Variables */

extern char strTmp[STR_TMP_SIZE];
extern char mySerialPrintOutputBuffer[MY_SERIAL_PRINTBUFFERLEN];

/* Global Functions */

extern void addToTrace(char * s);
extern void showAsHex(uint8_t *data, uint16_t len, char *description);
extern void sanityCheck(char *hint);
extern void mySerialPrint(void);

void my_memcpy(void *dest, const void *src, size_t n);
void my_memset(void *dest, int src, size_t n) ;

#endif /* INC_APP_FUNC_H_ */
