/**
 * @file	LPC_GPIO.h
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

#ifndef LPC_GPIO_H_
#define LPC_GPIO_H_

#include "main.h"
#include "Modules/LPC_INTERRUPT.h"
#include "Modules/LPC_PORT.h"
#include "Modules/LPC_DHTxx.h"

uint8_t const LPC_PIN_IDS[] = {
		0+20,	0+2,	24+26,	24+27,	24+20,	0+21,	24+23,	24+24,	// 8
		0+7,	24+28,	24+31,	24+21,	0+8,	0+9,	0+10,	24+29,	// 16
		24+19,	24+25,	24+16,	0+19,	0+18,	0+17,	24+15,	0+23,	// 24
		0+22,	0+16,	0+15,	24+22,	24+14,	24+13,	0+14,	0+13,	// 32
		0+12,	0+11,	// 34
};

volatile uint32_t * const LPC_PIN_REGISTERS[] = {
		&LPC_IOCON->PIO0_20, 	&LPC_IOCON->PIO0_2 , 	&LPC_IOCON->PIO1_26, 	&LPC_IOCON->PIO1_27,
		&LPC_IOCON->PIO1_20, 	&LPC_IOCON->PIO0_21, 	&LPC_IOCON->PIO1_23, 	&LPC_IOCON->PIO1_24,		// 8
		&LPC_IOCON->PIO0_7 , 	&LPC_IOCON->PIO1_28, 	&LPC_IOCON->PIO1_31, 	&LPC_IOCON->PIO1_21,
		&LPC_IOCON->PIO0_8 ,	&LPC_IOCON->PIO0_9 , 	&LPC_IOCON->SWCLK_PIO0_10, &LPC_IOCON->PIO1_29,		// 16
		&LPC_IOCON->PIO1_19, 	&LPC_IOCON->PIO1_25,	&LPC_IOCON->PIO1_16, 	&LPC_IOCON->PIO0_19,
		&LPC_IOCON->PIO0_18,	&LPC_IOCON->PIO0_17,	&LPC_IOCON->PIO1_15, 	&LPC_IOCON->PIO0_23, 		// 24
		&LPC_IOCON->PIO0_22,	&LPC_IOCON->PIO0_16,	&LPC_IOCON->SWDIO_PIO0_15,&LPC_IOCON->PIO1_22,
		&LPC_IOCON->PIO1_14, 	&LPC_IOCON->PIO1_13, 	&LPC_IOCON->TRST_PIO0_14,&LPC_IOCON->TDO_PIO0_13,	// 32
		&LPC_IOCON->TMS_PIO0_12,&LPC_IOCON->TDI_PIO0_11,// 34
};

#define LPC_PIN_FUNCTION_MASK	(BIT7 | 7)		// FUNC bits + AD bit
#define LPC_PIN_MODE_MASK		(3 << 3)

uint8_t const LPC_PIN_PRIMARY_FUNCTION[] = {	// GPIO function, All AD bits = 1
		0x80,	0x80,	0x80,	0x80,		0x80,	0x80,	0x80,	0x80,	// 8
		0x80,	0x80,	0x80,	0x80,		0x80,	0x80,	0x81,	0x80,	// 16
		0x80,	0x80,	0x80,	0x80,		0x80,	0x80,	0x80,	0x80,	// 24
		0x80,	0x80,	0x81,	0x80,		0x80,	0x80,	0x81,	0x81,	// 32
		0x81,	0x81,	// 34
};

uint8_t const LPC_PIN_SECONDARY_FUNCTION[] = {
		0x80 /* GPIO */,	0x80 /* GPIO */,		0x81 /* CT32B0 MAT2 */,	0x80 /* GPIO */,
		0x82 /* SPI1 SCK */,0x82 /* SPI1 MOSI */,	0x80 /* GPIO */,		0x81 /* CT32B0 MAT0 */,	// 8
		0x80 /* GPIO */,	0x80 /* GPIO */,		0x80 /* GPIO */,		0x82 /* SPI1 MISO */,
		0x81 /* SPI0 MISO*/,0x81 /* SPI0 MOSI */,	0x82 /* SPI0 SCK */,	0x80 /* GPIO */,	// 16
		0x80 /* GPIO */,	0x81 /* CT32B0 MAT1 */,	0x80 /* GPIO */,		0x81 /* UART TX */,
		0x81 /* UART RX */,	0x80 /* GPIO */,		0x82 /* PWM16_2 */,		0x01 /* ADC7 */,	// 24
		0x01 /* ADC6 */,	0x01 /* ADC5 */,		0x02 /* ADC4 */,		0x80 /* GPIO */,
		0x82 /* PWM16_1 */,	0x82 /* PWM16_0 */,		0x02 /* ADC3 */,		0x02 /* ADC2 */,	// 32
		0x02 /* ADC1 */,	0x02 /* ADC0 */,// 34
};


#define LPC_PIN_COUNT	34



void lpc_config_gpioInit(void);

SFPResult lpc_config_setPrimary(SFPFunction *msg);

SFPResult lpc_config_setSecondary(SFPFunction *msg);

SFPResult lpc_pinMode(SFPFunction *msg);

SFPResult lpc_digitalWrite(SFPFunction *msg);

SFPResult lpc_digitalRead(SFPFunction *msg);

SFPResult lpc_pulseIn(SFPFunction *msg);

#endif /* LPC_GPIO_H_ */
