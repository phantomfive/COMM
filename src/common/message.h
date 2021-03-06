#ifndef COMM_MESSAGE_H
#define COMM_MESSAGE_H

#include <notrap/notrap.h>

/*
 * Class to deal with internal message types.
 * We have two types of messages currently, 
 * a string and binary message.
 * 
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater.
 */

//-------------------------------------------------------------------------
// Section for data structures
//-------------------------------------------------------------------------
//Available types, see diagrams below.
//This header is not available externally to clients, so it's ok to define
//them with a simple name
#define TYPE_BINARY 0x1
#define TYPE_STRING 0x2

//Avaliable usages. For the 'usage' field in messages.
//The initial use for this is to distinguish messages for internal use
//(for example, keepalive messages) from messages that get sent to the
//user.
#define USAGE_INTERNAL 0x1
#define USAGE_USER     0x2


//-------------------------------------------------------------------------
// Message Types: Protocol definition 
//-------------------------------------------------------------------------

/*
TYPE_STRING format

+-------+---------------------------------------------------------+
| bytes | description                                             |
+-------+---------------------------------------------------------+
| 0-3   | uint32_t total message size (including these four bytes)|
| 4-7   | uint32_t type: one of TYPE_*                            |
| 8-11  | uint32_t usage: one of USAGE_*                          |
| 12-15 | uint32_t correlationId                                  |
| 16-19 | uint32_t code that is passed to the user                |
| 20-23 | uint32_t size of string param 1, including null byte    |
| 24-27 | uint32_t size of string param 2                         |
| 28-31 | uint32_t size of string param 3                         |
| 32-35 | uint32_t size of string param 4                         |
| 36-39 | uint32_t size of error message (strings can be zero len)|
| 40    | uint8_t  true on error, 0 if not error                  |
|       |                                                         |
| 41-   |   +-------------------------------------------------+   |
|       |   | String param 1                                  |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
|       |   | String param 2                                  |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
|       |   | String param 3                                  |   |
|       |   | (For an empty string set to length 1, since you |   |
|       |   |  need to send the null character as well)       |   |
|       |   +-------------------------------------------------+   |
|       |   | String param 4                                  |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
|       |   | Error string                                    |   |
|       |   +-------------------------------------------------+   |
+-------+---------------------------------------------------------+

TYPE_BINARY format.
Note that string messages and binary messages
are identical until byte 24.

+-------+---------------------------------------------------------+
| bytes | description                                             |
+-------+---------------------------------------------------------+
| 0-3   | uint32_t total message size (including these four bytes)|
| 4-7   | uint32_t type: one of TYPE_*                            |
| 8-11  | uint32_t usage: one of USAGE_*                          |
| 12-15 | uint32_t correlationId                                  |
| 16-19 | uint32_t code that is passed to the user                |
| 20-23 | uint32_t size of the string parameter                   |
| 24-27 | uint32_t size of the binary parameter                   |
| 28-31 | uint32_t unused                                         |
| 32-35 | uint32_t unused                                         |
| 36-39 | uint32_t size of the error message                      |
| 40    | uint8_t true on error, false if no error                |
| 41-   |   +-------------------------------------------------+   | 
|       |   | String param                                    |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
|       |   | Binary param                                    |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
|       |   | Error string                                    |   |
|       |   |                                                 |   |
|       |   |                                                 |   |
|       |   +-------------------------------------------------+   |
+-------+---------------------------------------------------------+

*/




/* StringMessage, inherits from Message */
struct Message {
	const void *context;
	uint32_t type;      //We keep the type in two places, because it needs
	                    //to be accessed a lot
	
	//This contains our message, ready to be sent one byte at a time
	uint8_t  data[0];   //allocated at runtime to hold all the params
};


//------------------------------------------------------------------------
// Section for Methods
//------------------------------------------------------------------------

/* Frees the message and sets it to NULL. If it's already NULL,
 * returns harmlessly */
void COMMFreeMessage(struct Message **msg);

/*Packs these fields into a struct Message for sending across a wire.
 *
 *If errMsg is not NULL, isError will be set to true. 
 *Returns NULL if there's not enough memory 
 *(or just crashes, depending on your OS).*/
struct Message *COMMNewStringMessage(uint32_t usage,
                                     uint32_t correlationId,
                                     uint32_t code,
                                     const char *param1,
                                     const char *param2,
                                     const char *param3,
                                     const char *param4,
                                     const char *errMsg,
                                     void *context);

/*Same comment as above*/
struct Message *COMMNewBinaryMessage(uint32_t usage, 
                                     uint32_t correlationId,
                                     uint32_t code,
                                     const char *stringParam,
                                     uint32_t binaryParamSize,
                                     uint8_t  *binaryParam,
                                     const char *errMsg,
                                     void *context);

/*These are convenience methods for getting fields from the struct,
 *since they stored in a format optimized for sending over the wire.
 *The first one works on binary messages too, set index to 1.
 *
 *COMMgetStringParam() has the first index starting at 1.
 *
 * If the error is set to FALSE, COMMgetErrMessage will return a string
 * of length 0.
 *
 *Return NULL on error.*/
const char *COMMgetStringParam(const struct Message *msg, int index);
const uint8_t*COMMgetBinaryParam(const struct Message *msg, uint32_t *sizeOut);
const char *COMMgetErrMessage(const struct Message  *msg);

/*These are similar, but return false if the message is the wrong type,
 *otherwise put the desired information into the 'out' parameter */ 
BOOL COMMgetSize(const struct Message *msg, uint32_t *sizeOut);
BOOL COMMgetType(const struct Message *msg, uint32_t *typeOut);
BOOL COMMgetUsage(const struct Message *msg, uint32_t *usageOut);
BOOL COMMgetCorrelationId(const struct Message *msg, uint32_t *cidOut);
BOOL COMMgetCode(const struct Message *msg, uint32_t *codeOut);
BOOL COMMgetIsErr(const struct Message *msg, BOOL *errOut);


#endif

