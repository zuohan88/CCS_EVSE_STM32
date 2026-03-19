/*
 * evsePredefine.c
 *
 *  Created on: Aug 18, 2024
 *      Author: zuoha
 */

#include "stdint.h"
#include "inttypes.h"
int32_t combineValueAndMultiplier(int32_t val, int8_t multiplier) {
  while (multiplier>0) { val = val*10; multiplier--; }
  while (multiplier<0) { val = val/10; multiplier++; }
  return val;
}

uint8_t OutputValueAndMultiplier(int32_t input, int32_t* val, int8_t* multiplier) {
  *multiplier = 0;

  if(input == 0) return 0;
  while(input%10 == 0){
    *multiplier  += 1;
    input /= 10;
  }
  *val = input;
  return 1;
}

//************************************************************************************************

