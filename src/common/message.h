#ifndef COMM_MESSAGE_H
#define COMM_MESSAGE_H

/*
 * Class to deal with internal message types.
 * We have two types of messages currently, a string message and a 
 * binary message. These will both be a 'subclass' of type message.
 * 
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater.
 */


/* The base class */
struct Message {
	// Indicates whether the message is binary or a string message. 
   //
	// It might seem natural to make 'type' an enum, however,
	// we need to be able to send this over the wire, so
	// we want to be specific in the exact size of the field */
	uint32_t type;

//Available types.
#define TYPE_BINARY 0x1
#define TYPE_STRING 0x2
};


//Avaliable usages. For the 'usage' field in messages.
//The initial use for this is to distinguish messages for internal use
//(for example, keepalive messages) from messages that get sent to the
//user.
#define USAGE_INTERNAL 0x1
#define USAGE_USER     0x2

/* StringMessage, inherits from Message */
struct StringMessage {
	
	//Fields that get sent across the wire
	uint32_t type;                //same as parent class
	uint32_t usage;               //one of the USAGE_* constants
	uint32_t correlationId;       //The message and its response share this id
	uint32_t code;                //This and the rest are all passed to the user
	uint32_t param1Size;
	uint32_t param2Size;
	uint32_t param3Size;
	uint32_t param4Size;
	uint8_t  *params;

	//used locally, not sent to the other side
	void *context;
};


/* BinaryMessage, inherits from Message */
struct BinaryMessage {
	
	//Fields that get sent across the wire
	uint32_t type;                //same as parent class
	uint32_t usage;               //one of the USAGE_* constants
	uint32_t correlationId;       //The message and its response share this id
	uint32_t code;                //This and the rest are all passed to the user
	uint32_t stringParamSize;
	uint32_t binaryParamSize;
	uint8_t  *params;

	//used locally, not sent to the other side
	void *context;
};

void freeMessage(Message **msg);

struct StringMessage *newStringMessage(uint32_t type,
                                       uint32_t usage,
                                       uint32_t correlationId,
                                       uint32_t code,
                                       const char *param1,
                                       const char *param2,
                                       const char *param3,
                                       const char *param4,
                                       void *context);

struct BinaryMessage *newBinaryMessage(uint32_t type, 
                                       uint32_t usage, 
                                       uint32_t correlationId,
                                       uint32_t code,
                                       const char *stringParam,
                                       uint32_t binaryParamSize,
                                       uint8_t  *binaryParam
                                       void *context);




#endif
