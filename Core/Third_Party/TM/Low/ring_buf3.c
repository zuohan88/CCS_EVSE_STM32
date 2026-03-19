//============================================================================
// Lock-Free Ring Buffer (LFRB) for embedded systems
// GitHub: https://github.com/QuantumLeaps/lock-free-ring-buffer
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
//
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//============================================================================
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "ring_buf3.h"


//............................................................................
void RingBuf3_ctor(RingBuf3 * const me, void* sto, uint32_t sto_size, RingBufCtr3 sto_len) {		// RingBufElement3 ---> void
    me->buf  = sto;		//put into address data into pointer
    me->element_size = sto_size;	//	how many element in a buffer
    me->buf_size  = sto_len;				//how many buffer in a array
    me->full = 0;
    atomic_store(&me->head, 0U);  // initialize head atomically
    atomic_store(&me->tail, 0U);  // initialize tail atomically
}
//............................................................................
bool RingBuf3_put(RingBuf3 * const me, void* const el) {				// RingBufElement3 ---> void //int * const sss sss address canot be changed
	RingBufCtr3 head_now = atomic_load_explicit(&me->head, memory_order_relaxed);
	RingBufCtr3 head_next = head_now + 1U;	//next address
	RingBufCtr3 tail = atomic_load_explicit(&me->tail, memory_order_acquire);
//    if(me->full){// buffer full
//    	return false;
//    }else{
//    	void* addr = me->buf + (head_now * me->element_size);
//    	memmove(addr, el, me->element_size);
//    	if (head_next == me->buf_size) {
//        	head_next = 0U;
//        }
//    	me->full = (head_next == tail);
//    	atomic_store_explicit(&me->head, head_next, memory_order_release);
//    	return true;
//    }

	if (head_next == me->buf_size) {
    	head_next = 0U;
    }
    if (head_next != tail) { // buffer NOT full?
    	void* addr = me->buf + (head_now * me->element_size);
    	memmove(addr, el, me->element_size);
        //me->buf[atomic_load_explicit(&me->head, memory_order_relaxed)] = el;	//struct can directly copy, memcpy but memcpy got problem
        atomic_store_explicit(&me->head, head_next, memory_order_release);
        return true;
    }
    else {
        return false; // buffer full
    }
}
//............................................................................
bool RingBuf3_get(RingBuf3 * const me, void *pel) {
    RingBufCtr3 tail = atomic_load_explicit(&me->tail, memory_order_relaxed);
    RingBufCtr3 head = atomic_load_explicit(&me->head, memory_order_acquire);
/*
    if(head == tail && !me->full){
    	 return false;
    }else{
    	memmove(pel, &me->buf[tail* me->element_size], me->element_size);
    	if (++tail == me->buf_size)
    	            tail = 0U;
        me->full = 0;
        atomic_store_explicit(&me->tail, tail, memory_order_release);
        return true;
    }
*/
    if (head != tail) { // buffer NOT empty?
    	memmove(pel, &me->buf[tail* me->element_size], me->element_size);
    	++tail;
        //*pel = me->buf[tail];
        if (tail == me->buf_size) {
            tail = 0U;
        }
        atomic_store_explicit(&me->tail, tail, memory_order_release);
        return true;
    }
    else {
        return false; // buffer empty
    }
}
//............................................................................
RingBufCtr3 RingBuf3_num_free(RingBuf3 * const me) {
    RingBufCtr3 head = atomic_load_explicit(&me->head, memory_order_acquire);
    RingBufCtr3 tail = atomic_load_explicit(&me->tail, memory_order_relaxed);
    if (head == tail) { // buffer empty?
        return (RingBufCtr3)(me->buf_size -1U);  //-1U
    }
    else if (head < tail) {
        return (RingBufCtr3)(tail - head -1U);	//-1U
    }
    else {
        return (RingBufCtr3)(me->buf_size + tail - head -1U);	//-1U
    }
}

//............................................................................
//void RingBuf3_process_all(RingBuf3 * const me, RingBufHandler3 handler) {		// RingBufElement3 ---> void
//    RingBufCtr3 tail = atomic_load_explicit(&me->tail, memory_order_relaxed);
//    RingBufCtr3 head = atomic_load_explicit(&me->head, memory_order_acquire);
//    while (head != tail) { // buffer NOT empty?
//        (*handler)(me->buf[tail]);
//       ++tail;
//        if (tail == me->end) {
//            tail = 0U;
//        }
//        atomic_store_explicit(&me->tail, tail, memory_order_release);
//    }
//}
/*
void ring_test(){
	RingBuf_ctor(&rb, buf, ARRAY_NELEM(buf));
	uint16_t num = RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U;
	RingBuf_put(&rb, temp);
	num = RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U;
	RingBuf_put(&rb, temp);
	num = RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U;
	if(true == RingBuf_get(&rb, &el)){
	}
*/

