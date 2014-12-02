/**
 * @file	LPC_UART.h
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
 * UART doesn't follow the same logic than other buses, like SPI or I2C.
 * This is because UART is initialized by the CDC module, and act as a USB bridge.
 * This small interface enable to configure the UART without reconfiguing the
 * whole USB bridge, in the cases when UART pins have been put to their primary
 * function.
 *
 */

#ifndef LPC_UART_H_
#define LPC_UART_H_

#include "main.h"

SFPResult lpc_uart_init(SFPFunction *msg);

#endif // LPC_UART_H_
