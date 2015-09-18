# Summary
The communication through the USB CDC stream suffers of [latencies](https://github.com/nodesign/weio/wiki/WeIO-timings). Some applications where the timings are short and must be very precise cannot be implemented from software side (from the IDE), due to these latencies.

To help users writing specific drivers or custom functions without going into the core code of UPER, this directory has been created. 

This directory contains two files :

* UserFunctions.c
* UserFunctions.h

# How to write a custom function
To explains how to write a custom function, here's a short example. 
In this example, a LED will be turned ON when the argument is 1, and OFF when it is at 0.

## Writing the function
All custom functions must have an argument of type SFPFunction* and a return value of type SFPResult, as shown below :

*SFPResult* **functionName**(_SFPFunction *msg_)

In this example, my custom function will be **dummyFunction**.

`````
SFPResult dummyFunction(SFPFunction *msg) {

    return SFP_OK;
}
`````

**msg** contains all the arguments for this function. The arguments can be verified with the following :

```
if (SFPFunction_getArgumentCount(msg) != x) // Where x is the number of expected arguments

if (SFPFunction_getArgumentType(msg, n) != SFP_ARG_INT) // Where n is the argument number. 
```

The value can be read with the function :

```
uint32_t dummyData = SFPFunction_getArgument_int32(msg, n); // Where n is the argument number.
```

Once all the arguments have been read, the processing can start. In my case, I have to test the argument, and control a GPIO :

```
LPC_GPIO->DIR[1] = 1 << 14; // Set GPIO1_14 as output
if (dummyData)
    LPC_GPIO->CLR[1] = 1 << 14; // Clear GPIO1_14
else
    LPC_GPIO->SET[1] = 1 << 14; // Set GPIO1_14
```

Once the processing is finished, a status can be sent back. Here, I just want to send back the result :

```
    /* Start a new frame */
	SFPFunction *outFunc = SFPFunction_new();
	if (outFunc == NULL) return SFP_ERR_ALLOC_FAILED;
	
	/* Then fill the frame */
	SFPFunction_setType(outFunc, SFPFunction_getType(msg));
	SFPFunction_setID(outFunc, 250); /* Put the Function ID */
	SFPFunction_setName(outFunc, "dummy"); /* Put the function name */
	SFPFunction_addArgument_int32(outFunc, dummyData); /* Put the values you want to send back */  
	/* Send it */
	SFPFunction_send(outFunc, &stream);
	/* End finalize */
	SFPFunction_delete(outFunc);
```

And that's it. The complete function can be shown [here](https://github.com/ks156/UPER/blob/newlib/UserFunctions/UserFunctions.c#L46-L71)

The argument for SFPFunction_setID and SFPFUnction_setName will be described later.

## Linking the custom function to SFP
The custom function must be registered in the SFP server. This registration is done in the function *SFPServer_addUserFunctions* this way :

```
	SFPServer_addFunctionHandler(server, fname, fid, callback);
```

Arguments are : 

* server : This is the instance of the SFP server. 
* fname : The function name (const char *)
* fid : ID of the function (uint32_t)
* callback function (SFPCallbackFunction)

For now, *fname* is not used. However, I recommand to use it for a potential future use.

__*fid*__ must be a unique identifier. ID must be comprised between 200 and 250. 0..199 and 251..255 are reserved and must not be used for a custom function.

In this example, fname will be "dummy", and fid 200 :

```
SFPServer_addFunctionHandler(server, 'dummy', 200, dummyFunction);
```

Your custom function must be public. Don't forget to add the definition in UserFunctions.h :

```
SFPResult dummyFunction(SFPFunction *msg);
```

