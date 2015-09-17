/**
 * @file	LPC_PORT.c
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
#include "main.h"
#include "Modules/LPC_PORT.h"

SFPResult lpc_portMode(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 2) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT || SFPFunction_getArgumentType(msg, 1) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t port = SFPFunction_getArgument_int32(msg, 0);
	uint8_t mode = SFPFunction_getArgument_int32(msg, 1);

	if (port >= LPC_PORT_COUNT) return SFP_ERR_ARG_VALUE;
	if (mode > 4 || mode == 3) return SFP_ERR_ARG_VALUE;

	uint8_t i = 0;
	uint8_t lpc_port = 0;
	uint8_t pin = 0;
	for (i = 0; i < 8; i++) {
		lpc_port = 0;
		pin = LPC_PORT_MAPPING[port][i];
		uint8_t pinNum = LPC_PIN_IDS[pin];
		if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
			lpc_port = 1;
			pinNum -= 24;
		}

		*LPC_PIN_REGISTERS[pin] &= ~LPC_PIN_MODE_MASK;	// Remove pull-up/down resistors

		if (mode == 1) {
			LPC_GPIO->DIR[lpc_port] |= (1 << pinNum);	// Set direction bit (output)
		} else {
			*LPC_PIN_REGISTERS[pin] |= (mode << 2) & LPC_PIN_MODE_MASK;// Setup resistors
			LPC_GPIO->DIR[lpc_port] &= ~(1 << pinNum);	// Clear direction bit (input)
		}
	}
	return SFP_OK;
}

SFPResult lpc_portWrite(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 2)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t port = SFPFunction_getArgument_int32(msg, 0);
	uint8_t value = SFPFunction_getArgument_int32(msg, 1);

	if (port >= LPC_PORT_COUNT) return SFP_ERR_ARG_VALUE;

	uint8_t lpc_port = 0;
	uint8_t i;
	/* NOTE : The port mapping of WeIO doesn't reflect the real port of LPC.
	 * The portWrite doesn't set the whole port in the same time, but pin per pin.
	 */
	for (i = 0; i < 8; i++) {
		lpc_port = 0;
		// Get the current pin
		uint8_t pinNum = LPC_PIN_IDS[LPC_PORT_MAPPING[port][i]];
		if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
			lpc_port = 1;
			pinNum -= 24;
		}

		// Output the value
		if (((value >> i) & 0x01) == 0) {
			LPC_GPIO->CLR[lpc_port] = (1 << pinNum);
		} else {
			LPC_GPIO->SET[lpc_port] = (1 << pinNum);
		}
	}
	return SFP_OK;
}

SFPResult lpc_portRead(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t port = SFPFunction_getArgument_int32(msg, 0);

	if (port >= LPC_PORT_COUNT)
		return SFP_ERR_ARG_VALUE;

	uint8_t lpc_port = 0;
	uint8_t i;
	uint8_t port_val = 0;

	for (i = 0; i < 8; i++) {
		lpc_port = 0;
		// Get the current pin
		uint8_t pinNum = LPC_PIN_IDS[LPC_PORT_MAPPING[port][i]];
		if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
			lpc_port = 1;
			pinNum -= 24;
		}
		if (LPC_GPIO->PIN[lpc_port] & (1 << pinNum))
			port_val |= (1 << i);
	}

	SFPFunction *outFunc = SFPFunction_new();

	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;

	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, UPER_FID_PORTREAD);
	SFPFunction_setName(outFunc, UPER_FNAME_PORTREAD);
	SFPFunction_addArgument_int32(outFunc, port);
	SFPFunction_addArgument_int32(outFunc, port_val);
	SFPFunction_send(outFunc, &stream);
	SFPFunction_delete(outFunc);

	return SFP_OK;
}
