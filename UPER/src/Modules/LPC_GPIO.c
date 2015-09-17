/**
 * @file	LPC_GPIO.c
 * @author  Giedrius Medzevicius <giedrius@8devices.com>
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

#include "Modules/LPC_GPIO.h"

void lpc_config_gpioInit() {
	uint8_t pin;
	for (pin=0; pin<LPC_PIN_COUNT; pin++)
		*LPC_PIN_REGISTERS[pin] = (*LPC_PIN_REGISTERS[pin] & ~LPC_PIN_FUNCTION_MASK) | LPC_PIN_PRIMARY_FUNCTION[pin];
}

SFPResult lpc_config_setPrimary(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT) return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);

	if (pin >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;

	*LPC_PIN_REGISTERS[pin] = (*LPC_PIN_REGISTERS[pin] & ~LPC_PIN_FUNCTION_MASK) | LPC_PIN_PRIMARY_FUNCTION[pin];

	return SFP_OK;
}

SFPResult lpc_config_setSecondary(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT) return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);

	if (pin >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;

	*LPC_PIN_REGISTERS[pin] = (*LPC_PIN_REGISTERS[pin] & ~LPC_PIN_FUNCTION_MASK) | LPC_PIN_SECONDARY_FUNCTION[pin];

	return SFP_OK;
}


SFPResult lpc_pinMode(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 2) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT || SFPFunction_getArgumentType(msg, 1) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);
	uint8_t mode = SFPFunction_getArgument_int32(msg, 1);

	if (pin >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;
	if (mode > 4 || mode == 3) return SFP_ERR_ARG_VALUE;

	uint8_t port = 0;
	uint8_t pinNum = LPC_PIN_IDS[pin];
	if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
		port = 1;
		pinNum -= 24;
	}

	*LPC_PIN_REGISTERS[pin] &= ~LPC_PIN_MODE_MASK;	// Remove pull-up/down resistors

	if (mode == 1) {
		LPC_GPIO->DIR[port] |= (1 << pinNum);	// Set direction bit (output)
	} else {
		*LPC_PIN_REGISTERS[pin] |= (mode << 2) & LPC_PIN_MODE_MASK;		// Setup resistors
		LPC_GPIO->DIR[port] &= ~(1 << pinNum);	// Clear direction bit (input)
	}

	return SFP_OK;
}

SFPResult lpc_digitalWrite(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 2)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT || SFPFunction_getArgumentType(msg, 1) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);
	uint8_t value = SFPFunction_getArgument_int32(msg, 1);

	if (pin >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;

	uint8_t port = 0;
	uint8_t pinNum = LPC_PIN_IDS[pin];
	if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
		port = 1;
		pinNum -= 24;
	}

	if (value == 0) {
		LPC_GPIO->CLR[port] = (1 << pinNum);
	} else {
		LPC_GPIO->SET[port] = (1 << pinNum);
	}

	return SFP_OK;
}

SFPResult lpc_digitalRead(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);

	if (pin >= LPC_PIN_COUNT)
		return SFP_ERR_ARG_VALUE;

	uint8_t port = 0;
	uint8_t pinNum = LPC_PIN_IDS[pin];
	if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
		port = 1;
		pinNum -= 24;
	}

	uint8_t val = 0;

	if (LPC_GPIO->PIN[port] & (1 << pinNum))
		val = 1;

	SFPFunction *outFunc = SFPFunction_new();

	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;

	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, UPER_FID_DIGITALREAD);
	SFPFunction_setName(outFunc, UPER_FNAME_DIGITALREAD);
	SFPFunction_addArgument_int32(outFunc, pin);
	SFPFunction_addArgument_int32(outFunc, val);
	SFPFunction_send(outFunc, &stream);
	SFPFunction_delete(outFunc);

	return SFP_OK;
}

SFPResult lpc_pulseIn(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 3) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT
			|| SFPFunction_getArgumentType(msg, 1) != SFP_ARG_INT
			|| SFPFunction_getArgumentType(msg, 2) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t pin = SFPFunction_getArgument_int32(msg, 0);
	uint8_t levelMask = (SFPFunction_getArgument_int32(msg, 1) == 0 ? 0 : 1);
	uint32_t timeout = SFPFunction_getArgument_int32(msg, 2);

	if (pin >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;

	uint8_t port = 0;
	uint8_t pinNum = LPC_PIN_IDS[pin];
	if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
		port = 1;
		pinNum -= 24;
	}

	levelMask <<= pinNum; // shift BIT0 to pin place

	uint32_t startTimeUs = Time_getSystemTime_us();
	uint32_t passedTimeUs = 0;

	while ((LPC_GPIO->PIN[port] & (1 << pinNum)) == levelMask) {	// Wait while signal is on
		if ((passedTimeUs=Time_getSystemTime_us()-startTimeUs) >= timeout)
			break;
	}

	while ((LPC_GPIO->PIN[port] & (1 << pinNum)) != levelMask) { // Wait while signal is off
		if ((passedTimeUs=Time_getSystemTime_us()-startTimeUs) >= timeout)
			break;
	}

	uint32_t signalStartTime = Time_getSystemTime_us();

	while ((LPC_GPIO->PIN[port] & (1 << pinNum)) == levelMask) {	// Wait while signal is on
		if ((passedTimeUs=Time_getSystemTime_us()-startTimeUs) >= timeout)
			break;
	}
	uint32_t signalDuration = Time_getSystemTime_us()-signalStartTime;

	SFPFunction *outFunc = SFPFunction_new();

	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;

	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, UPER_FID_PULSEIN);
	SFPFunction_setName(outFunc, UPER_FNAME_PULSEIN);
	SFPFunction_addArgument_int32(outFunc, (passedTimeUs < timeout ? signalDuration : 0));
	SFPFunction_send(outFunc, &stream);
	SFPFunction_delete(outFunc);

	return SFP_OK;
}
