/**
 * @file	UserFunctions.c
 * @author  Paul Rathgeb <paul.rathgeb@skynet.be>
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Rathgeb
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

#include "UserFunctions.h"

/*
 * This function allow to register custom function to SFP.
 * Usage : SFPServer_addFunctionHandler(server, FNAME, FID, callback);
 */
void SFPServer_addUserFunctions(SFPServer *server) {
	SFPServer_addFunctionHandler(server, "dummy", 1000, dummyFunction);
}

/* 
 * This is a dummy function, used as demonstration
 */
SFPResult dummyFunction(SFPFunction *msg) {
	/* Check for arg count */
	if (SFPFunction_getArgumentCount(msg) != 1) return SFP_ERR_ARG_COUNT;
	/* Check for arg type */
	if (SFPFunction_getArgumentType(msg, 0) != SFP_ARG_INT) return SFP_ERR_ARG_TYPE;
	/* Get the argument */
	uint32_t dummyData = SFPFunction_getArgument_int32(msg, 0);

	/* Do some stupid things */
	dummyData += 1;

	/* Start a new frame */
	SFPFunction *outFunc = SFPFunction_new();
	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;
	/* Then fill the frame */
	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, 1000); /* Put the Function ID */
	SFPFunction_setName(outFunc, "dummy"); /* Put the function name */
	SFPFunction_addArgument_int32(outFunc, dummyData); /* Put the values you want to send back */  
	/* Send it */
	SFPFunction_send(outFunc, &stream);
	/* End finalize */
	SFPFunction_delete(outFunc);

	return SFP_OK;
}
