/**
 * @file	LPC_DHTxx.c
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

#include "Modules/Devices/DHTxx.h"

SFPResult lpc_dhtxxRead(SFPFunction *msg) {
	if (SFPFunction_getArgumentCount(msg) != 1) return SFP_ERR_ARG_COUNT;

	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT)
		return SFP_ERR_ARG_TYPE;

	uint8_t dht_data = SFPFunction_getArgument_int32(msg, 0);

	if (dht_data >= LPC_PIN_COUNT) return SFP_ERR_ARG_VALUE;

	uint8_t port = 0;
	uint8_t pinNum = LPC_PIN_IDS[dht_data];
	if (pinNum > 23) {	// if not PIO0_0 to PIO0_23
		port = 1;
		pinNum -= 24;
	}
	uint8_t data[5] = {0, 0, 0, 0, 0}; // 40 bits

    /* Remove all pullup and pulldown */
    *LPC_PIN_REGISTERS[dht_data] &= ~LPC_PIN_MODE_MASK;

    /* Begin transaction with DHT11 : Set pin as output, HIGH */
    LPC_GPIO->DIR[port] |= (1 << pinNum);
    LPC_GPIO->SET[port] = (1 << pinNum);

    uint32_t startTimeUs = Time_getSystemTime_us();
    uint32_t passedTimeUs = 0;

    /* -- Start condition --
     * ¯¯¯¯¯¯¯¯\________/¯¯¯¯¯¯¯¯
     * | ~25ms | ~25ms  | ~30us |
     */
    /* Pull high for ~25ms */
    while ((Time_getSystemTime_us()-startTimeUs) <= 25000);
    LPC_GPIO->CLR[port] = (1 << pinNum);
    startTimeUs = Time_getSystemTime_us();
    /* Pull low for ~25ms */
    while ((Time_getSystemTime_us()-startTimeUs) <= 25000);
    /* Pull high for ~40us */
    LPC_GPIO->SET[port] = (1 << pinNum);
    startTimeUs = Time_getSystemTime_us();
    while ((Time_getSystemTime_us()-startTimeUs) <= 30);

    /* -- Response from sensor --
     * \_______/¯¯¯¯¯¯\______/¯¯¯¯¯¯\______/¯¯¯¯¯¯¯¯¯¯¯¯\___...
     * | 80us  | 80us | 50us | 27us | 50us |  70us      | .....
     * | start burst  | Answer : 0  | Answer : 1        | .....
     */
    /* Input, no pullup nor pulldown */
    LPC_GPIO->DIR[port] &= ~(1 << pinNum);

    uint8_t i;
    uint8_t j = 0;
    /* Store the last pin state. */

    uint32_t cnt = 0;
    uint32_t cnt_compare = 0;

    startTimeUs = Time_getSystemTime_us();
    passedTimeUs = 0;
    uint8_t error = 0;
    while (!(LPC_GPIO->PIN[port] & (1 << pinNum))) {
    	if ((passedTimeUs=Time_getSystemTime_us()-startTimeUs) >= 1000) {
    		error = 1;
    		break;
    	}
    }

    startTimeUs = Time_getSystemTime_us();
    passedTimeUs = 0;
    while ((LPC_GPIO->PIN[port] & (1 << pinNum))) {
    	if ((passedTimeUs=Time_getSystemTime_us()-startTimeUs) >= 1000) {
    		error = 1;
    		break;
    	}
    }
    if (!error) {
		for (i = 0; i < 40; i++) {
			cnt = 0;
			cnt_compare = 0;
			while (!(LPC_GPIO->PIN[port] & (1 << pinNum)))
				cnt++;
			while ((LPC_GPIO->PIN[port] & (1 << pinNum)))
				cnt_compare++;

			data[j/8] <<= 1;
			if (cnt < cnt_compare)
				data[j/8] |= 1;
			j++;
		}
    }

	SFPFunction *outFunc = SFPFunction_new();

	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;

	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, UPER_FID_DHTXXREAD);
	SFPFunction_setName(outFunc, UPER_FNAME_DHTXXREAD);
	SFPFunction_addArgument_int32(outFunc, dht_data);
	SFPFunction_addArgument_int32(outFunc, data[0]);
	SFPFunction_addArgument_int32(outFunc, data[1]);
	SFPFunction_addArgument_int32(outFunc, data[2]);
	SFPFunction_addArgument_int32(outFunc, data[3]);
	SFPFunction_addArgument_int32(outFunc, data[4]);
	SFPFunction_send(outFunc, &stream);
	SFPFunction_delete(outFunc);

	return SFP_OK;
}
