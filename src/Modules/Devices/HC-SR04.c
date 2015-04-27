/**
 * @file	HC-SR04.c
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

#include "Modules/Devices/HC-SR04.h"

SFPResult hcsr04Read(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 2) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;
	if (SFPFunction_getArgumentType(msg, 1) != SFP_ARG_INT)
			return SFP_ERR_ARG_TYPE;

	uint8_t trigger = SFPFunction_getArgument_int32(msg, 0);
	uint8_t pulse = SFPFunction_getArgument_int32(msg, 1);
	uint32_t startTime = 0;
	uint32_t distance;

	if (trigger >= LPC_PIN_COUNT)
		return SFP_ERR_ARG_VALUE;
	if (pulse >= LPC_PIN_COUNT)
		return SFP_ERR_ARG_VALUE;

	uint8_t trigger_port = 0;
	uint8_t trigger_pinNum = LPC_PIN_IDS[trigger];
	if (trigger_pinNum > 23) {	// if not PIO0_0 to PIO0_23
		trigger_port = 1;
		trigger_pinNum -= 24;
	}

	uint8_t pulse_port = 0;
	uint8_t pulse_pinNum = LPC_PIN_IDS[pulse];
	if (pulse_pinNum > 23) {	// if not PIO0_0 to PIO0_23
		pulse_port = 1;
		pulse_pinNum -= 24;
	}

	/* Set the trigger as output, LOW */
	LPC_GPIO->CLR[trigger_port] = (1 << trigger_pinNum);
	*LPC_PIN_REGISTERS[trigger] &= ~LPC_PIN_MODE_MASK;	// Remove pull-up/down resistors
	LPC_GPIO->DIR[trigger_port] |= (1 << trigger_pinNum);

	/* Set the pulse as input, no resistors */
	*LPC_PIN_REGISTERS[pulse] &= ~LPC_PIN_MODE_MASK;	// Remove pull-up/down resistors
	*LPC_PIN_REGISTERS[pulse] |= (2 << 2) & LPC_PIN_MODE_MASK;;
	LPC_GPIO->DIR[pulse_port] &= ~(1 << pulse_pinNum);

	/* Send the trigger signal for 10us*/
	startTime = Time_getSystemTime_us();
	LPC_GPIO->SET[trigger_port] = (1 << trigger_pinNum);
	while ((Time_getSystemTime_us()-startTime) <= 10);
	LPC_GPIO->CLR[trigger_port] = (1 << trigger_pinNum);


	/* Wait for an answer */
	startTime = Time_getSystemTime_us();
	while (!(LPC_GPIO->PIN[pulse_port] & (1 << pulse_pinNum))) {
	    if ((Time_getSystemTime_us()-startTime) >= 25000) { // Timeout : 25ms
	   		break;
	   	}
	}

	/* Measure the pulse */
	startTime = Time_getSystemTime_us();
	while ((LPC_GPIO->PIN[pulse_port] & (1 << pulse_pinNum))) {
		if ((Time_getSystemTime_us()-startTime) >= 25000) { // Timeout : 25ms
		    break;
		}
    }

	distance = (Time_getSystemTime_us() - startTime) / 58;

	SFPFunction *outFunc = SFPFunction_new();

	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;

	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, UPER_FID_HCSR04);
	SFPFunction_setName(outFunc, UPER_FNAME_HCSR04);
	SFPFunction_addArgument_int32(outFunc, (distance < 25000 ? distance : 0));
	SFPFunction_send(outFunc, &stream);
	SFPFunction_delete(outFunc);

	return SFP_OK;
}
