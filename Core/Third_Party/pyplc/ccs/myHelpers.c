

#include "ccs32_globals.h"

char strTmp[STR_TMP_SIZE];
char mySerialPrintOutputBuffer[MY_SERIAL_PRINTBUFFERLEN] = {'\0'};

/* Helper functions */

void addToTrace(char * s) {
    McuXFormat_xsnprintf(mySerialPrintOutputBuffer,(MY_SERIAL_PRINTBUFFERLEN-1), "[%ld] %s\r\n", HAL_GetTick(), s);
    mySerialPrint(); /* print to the serial line */
   // canbus_addStringToTextTransmitBuffer(mySerialPrintOutputBuffer); /* print to the CAN */
}

void showAsHex(uint8_t *data, uint16_t len, char *description) {
   if(len>200)return;
   char strHex[5];
   int i;
   McuXFormat_xsprintf(strTmp, "%s (%d bytes): ", description, len);
   for (i=0; i<len; i++) {
       McuXFormat_xsprintf(strHex, "%02x ", data[i]);
       strcat(strTmp, strHex);
   }
   addToTrace(strTmp);
}

void sanityCheck(char *hint) {
    /* todo: check the canaries, config registers, maybe stack, ... */
}

void my_memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}
void my_memset(void *dest, int src, size_t n) {
    unsigned char *d = dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = src;
    }
}
