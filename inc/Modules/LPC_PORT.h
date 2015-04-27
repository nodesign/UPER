/**
 * @file	LPC_PORT.h
 * @author  Paul Rathgeb <paul.rathgeb@skynet.be>
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 UAB 8devices
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 */

#ifndef LPC_PORT_H_
#define LPC_PORT_H_

#include "main.h"
#include "Modules/LPC_GPIO.h"

#define LPC_PORT_COUNT	4

uint8_t const LPC_PORT_MAPPING[4][8] = {
		{20, 19, 13, 12, 14, 1,  8,  21}, // PORT 0
		{5,  11, 4,  0, 18, 16, 27, 6},  // PORT 1
		{3,  9,  29, 28, 22, 7,  17, 2},  // PORT 2
		{33, 32, 31, 30, 26, 25, 24, 23}  // PORT 3
};

SFPResult lpc_portMode(SFPFunction *msg);

SFPResult lpc_portWrite(SFPFunction *msg);

SFPResult lpc_portRead(SFPFunction *msg);

#endif // LPC_PORT_H_
