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
#ifndef RING_BUF_3_H
#define RING_BUF_3_H

#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>

//! Ring buffer counter/index
//
// @attention
// The following RingBufCtr type is assumed to be "atomic" in a target CPU,
// meaning that the CPU needs to write the whole RingBufCtr in one machine
// instruction. An example of violating this assumption would be an 8-bit
// CPU, which writes uint16_t (2-bytes) in 2 machine instructions. For such
// 8-bit CPU, the maximum size of RingBufCtr would be uint8_t (1-byte).
//
// Another case of violating the "atomic" writes to RingBufCtr type would
// be misalignment of a RingBufCtr variable in memory, which could cause
// the compiler to generate multiple instructions to write a RingBufCtr value.
// Therefore, it is further assumed that all RingBufCtr variables in the
// following RingBuf struct *are* correctly aligned for "atomic" access.
//In practice, most C compilers should provide such natural alignment
// (by inserting some padding into the ::RingBuf struct, if necessary).
//
typedef uint16_t RingBufCtr3;

//! Ring buffer element type
//
// @details
// The type of the ring buffer elements is not critical for the lock-free
// operation and does not need to be "atomic". For example, it can be
// an integer type (uint16_t, uint32_t, uint64_t), a pointer type,
// or even a struct type. However, the bigger the type the more RAM is
// needed for the ring buffer and more CPU cycles are needed to copy it
// into and out of the buffer memory.
//
//typedef uint8_t RingBufElement;

//typedef struct {
//	uint32_t adc_data[20];
//	uint32_t last_tick;

//}RingBufElement3;


//! Ring buffer struct
typedef struct {
    void *buf; //!< pointer to the start of the ring buffer
    uint32_t element_size;
    RingBufCtr3 buf_size;      //!< index of the end of the ring buffer
    RingBufCtr3 full;
    //! atomic index to where next element will be inserted
    _Atomic(RingBufCtr3) head;

    //! atomic index to where next element will be removed
    _Atomic(RingBufCtr3) tail;
} RingBuf3;

void RingBuf3_ctor(RingBuf3 * const me, void* sto, uint32_t sto_size, RingBufCtr3 sto_len);		// RingBufElement3 ---> void
RingBufCtr3 RingBuf3_num_free(RingBuf3 * const me);
bool RingBuf3_put(RingBuf3 * const me, void* const el)  ;
bool RingBuf3_get(RingBuf3 * const me, void *pel);

//! Ring buffer callback function for RingBuf_process_all()
//
// @details
// The callback processes one element and runs in the context of
// RingBuf_process_all().
//
//typedef void (*RingBufHandler3)(RingBufElement3 const el);

//void RingBuf3_process_all(RingBuf3 * const me, RingBufHandler3 handler);

#endif // RING_BUF_H
