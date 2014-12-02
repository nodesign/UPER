/**
 * @file	LPC_INTERRUPT.c
 * @author  Giedrius Medzevicius <giedrius@8devices.com>
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
#include "Modules/LPC_GPIO/LPC_INTERRUPT.h"

static volatile SFPFunctionType LPC_INTERRUPT_FUNCTION_TYPE[LPC_INTERRUPT_COUNT];
static uint32_t LPC_INTERRUPT_DOWNTIME[LPC_INTERRUPT_COUNT];

SFPResult lpc_attachInterrupt(SFPFunction *func) {
	if (SFPFunction_getArgumentCount(func) != 4)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(func, 0) != SFP_ARG_INT
			|| SFPFunction_getArgumentType(func, 1) != SFP_ARG_INT
			|| SFPFunction_getArgumentType(func, 2) != SFP_ARG_INT
			|| SFPFunction_getArgumentType(func, 3) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t p_intID = SFPFunction_getArgument_int32(func, 0);	// interrupt ID
	uint8_t p_pin = SFPFunction_getArgument_int32(func, 1);	// pin ID
	uint8_t p_mode = SFPFunction_getArgument_int32(func, 2);	// interrupt mode
	uint32_t p_downtime = SFPFunction_getArgument_int32(func, 3); // down time

	if (p_pin >= LPC_PIN_COUNT || p_intID >= LPC_INTERRUPT_COUNT || p_mode > 4) return SFP_ERR_ARG_VALUE;

	NVIC_DisableIRQ(p_intID);	// Disable interrupt. XXX: Luckily FLEX_INTx_IRQn == x, so it can be used this way, otherwise BE AWARE!

	LPC_SYSCON->PINTSEL[p_intID] = LPC_PIN_IDS[p_pin]; 	// select which pin will cause the interrupts

	// XXX: using SI/CI ENF and ENR registers could probably save few instructions
	switch (p_mode) {
		case 0: {	// LOW level mode
			LPC_GPIO_PIN_INT ->ISEL |= (1 << p_intID);	// Set PMODE=level sensitive
			LPC_GPIO_PIN_INT ->IENR |= (1 << p_intID);	// Enable level interrupt.
			LPC_GPIO_PIN_INT ->IENF &= ~(1 << p_intID);	// Set active level LOW.
			break;
		}
		case 1: {	// HIGH level mode
			LPC_GPIO_PIN_INT ->ISEL |= (1 << p_intID);	// Set PMODE=level sensitive
			LPC_GPIO_PIN_INT ->IENR |= (1 << p_intID);	// Enable level interrupt.
			LPC_GPIO_PIN_INT ->IENF |= (1 << p_intID);	// Set active level HIGH.
			break;
		}
		case 2: {	// Edge CHANGE mode
			LPC_GPIO_PIN_INT ->ISEL &= ~(1 << p_intID);	// Set PMODE=edge sensitive
			LPC_GPIO_PIN_INT ->IENR |= (1 << p_intID);	// Enable rising edge.
			LPC_GPIO_PIN_INT ->IENF |= (1 << p_intID);	// Enable falling edge.
			break;
		}
		case 3: {	// RISING edge mode
			LPC_GPIO_PIN_INT ->ISEL &= ~(1 << p_intID);	// Set PMODE=edge sensitive
			LPC_GPIO_PIN_INT ->IENR |= (1 << p_intID);	// Enable rising edge.
			LPC_GPIO_PIN_INT ->IENF &= ~(1 << p_intID);	// Disable falling edge.
			break;
		}
		case 4: {	// FALLING edge mode
			LPC_GPIO_PIN_INT ->ISEL &= ~(1 << p_intID);	// Set PMODE=edge sensitive
			LPC_GPIO_PIN_INT ->IENR &= ~(1 << p_intID);	// Disable rising edge.
			LPC_GPIO_PIN_INT ->IENF |= (1 << p_intID);	// Enable falling edge.
			break;
		}
	}

	LPC_INTERRUPT_FUNCTION_TYPE[p_intID] = SFPFunction_getType(func);
	LPC_INTERRUPT_DOWNTIME[p_intID] = p_downtime;

	LPC_GPIO_PIN_INT->RISE = (1 << p_intID);	// Clear rising edge (sort of) flag
	LPC_GPIO_PIN_INT->FALL = (1 << p_intID);	// Clear falling edge (sort of) flag
	NVIC_SetPriority(p_intID, 3); // set lowest priority
	NVIC_EnableIRQ(p_intID);	// Enable interrupt. XXX: Luckily FLEX_INTx_IRQn == x, so it can be used this way, otherwise BE AWARE!

	return SFP_OK;
}

SFPResult lpc_detachInterrupt(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1)
		return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT) return SFP_ERR_ARG_TYPE;

	uint8_t p_intID = SFPFunction_getArgument_int32(msg, 0);	// interrupt ID

	NVIC_DisableIRQ(p_intID);	// Disable interrupt. XXX: Luckily FLEX_INTx_IRQn == x, so it can be used this way, otherwise BE AWARE!
	LPC_GPIO_PIN_INT->CIENR = (1 << p_intID);	// Disable rising edge or level interrupt
	LPC_GPIO_PIN_INT->CIENF = (1 << p_intID);	// Disable falling edge interrupt
	LPC_GPIO_PIN_INT->RISE = (1 << p_intID);	// Clear rising edge (sort of) flag
	LPC_GPIO_PIN_INT->FALL = (1 << p_intID);	// Clear falling edge (sort of) flag

	return SFP_OK;
}

void GPIO_EnableInterruptCallback(void* ptr) {
	GPIO_EnableInterrupt((uint8_t)(uint32_t)ptr);
}

void GPIO_EnableInterrupt(uint8_t intID) {
	LPC_GPIO_PIN_INT->RISE = (1<<intID);	// Clear rising edge (sort of) flag
	LPC_GPIO_PIN_INT->FALL = (1<<intID);	// Clear falling edge (sort of) flag
	NVIC_EnableIRQ(intID);	// Enable ISR
}

static inline void GPIO_SEND_INT(uint8_t intID, uint32_t intStatus) {
	SFPFunction *func = SFPFunction_new();
	if (func != NULL) {
		SFPFunction_setType(func, LPC_INTERRUPT_FUNCTION_TYPE[intID]);
		SFPFunction_setID(func, UPER_FID_INTERRUPT);
		SFPFunction_setName(func, UPER_FNAME_INTERRUPT);
		SFPFunction_addArgument_int32(func, intID);
		SFPFunction_addArgument_int32(func, intStatus);
		SFPFunction_send(func, &stream);
		SFPFunction_delete(func);
	}
}

static void GPIO_InterruptHandler(uint8_t intID) {
	NVIC_DisableIRQ(intID);		// Disable ISR

	uint8_t intBit = (1 << intID);

	if (LPC_GPIO_PIN_INT->IST & intBit) {

		uint32_t interruptValues = 0;
		uint8_t i;
		for (i=0; i<LPC_INTERRUPT_COUNT; i++) {
			uint8_t port = 0;
			uint8_t pinNum = LPC_SYSCON->PINTSEL[i];
			if (pinNum > 23) {
				port = 1;
				pinNum -= 24;
			}

			if (LPC_GPIO->PIN[port] & (1 << pinNum))
				interruptValues |= (1 << i);
		}


		uint8_t interruptEvent = 0xFF;

		if ((LPC_GPIO_PIN_INT->ISEL & intBit)) {	// if LEVEL mode
			if (LPC_GPIO_PIN_INT->IENR & intBit) {	// if LEVEL interrupts are enabled
				if (LPC_GPIO_PIN_INT->IENF & intBit) {	// HIGH mode
					interruptEvent = 1;
				} else {								// LOW mode
					interruptEvent = 0;
				}
			}
		} else {	// EDGE mode
			if ((LPC_GPIO_PIN_INT->RISE & intBit) && (LPC_GPIO_PIN_INT->IENR & intBit)) {	// Rising edge interrupt
				interruptEvent = 3;
			}
			if ((LPC_GPIO_PIN_INT->FALL & intBit) && (LPC_GPIO_PIN_INT->IENF & intBit)) {	// Falling edge interrupt
				if (interruptEvent == 3)
					interruptEvent = 2;				// Edge CHANGE (RISE+FALL)
				else
					interruptEvent = 4;				// Falling edge
			}
		}

		GPIO_SEND_INT(intID, (interruptValues << 8) | interruptEvent);

		Time_addTimer(LPC_INTERRUPT_DOWNTIME[intID], GPIO_EnableInterruptCallback, (void*)(uint32_t)intID);
		return;
	}

	GPIO_EnableInterrupt(intID);
}

void FLEX_INT0_IRQHandler() {
	GPIO_InterruptHandler(0);
}

void FLEX_INT1_IRQHandler() {
	GPIO_InterruptHandler(1);
}

void FLEX_INT2_IRQHandler() {
	GPIO_InterruptHandler(2);
}

void FLEX_INT3_IRQHandler() {
	GPIO_InterruptHandler(3);
}

void FLEX_INT4_IRQHandler() {
	GPIO_InterruptHandler(4);
}

void FLEX_INT5_IRQHandler() {
	GPIO_InterruptHandler(5);
}

void FLEX_INT6_IRQHandler() {
	GPIO_InterruptHandler(6);
}

void FLEX_INT7_IRQHandler() {
	GPIO_InterruptHandler(7);
}
