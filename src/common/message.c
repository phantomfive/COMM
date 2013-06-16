#include "notrap.h"

#include "message.h"

/* message.c
 * This code requires good understanding of common binary strategies
 * in C, especially pointer arithmetic, and malloc() with a size larger
 * than the structure requires.
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater.
 */

//------------------------------------------------------------------------
// Public API
//------------------------------------------------------------------------

void COMMFreeMessage(struct Message **msg) {
	if(msg==NULL || *msg==NULL) return;
	free(*msg);
	*msg = NULL;
}

struct StringMessage *COMMNewStringMessage(uint32_t usage,
                                           uint32_t correlationId,
                                           uint32_t code,
                                           const char *param1,
                                           const char *param2,
                                           const char *param3,
                                           const char *param4,
                                           const char *errMsg,
                                           void *context) {
	struct StringMessage *rv;
	int allSize;
	int size1 = NTPstrlen(param1) + 1;  //plus 1 because we send NULL chars
	int size2 = NTPstrlen(param2) + 1;
	int size3 = NTPstrlen(param3) + 1;
	int size4 = NTPstrlen(param4) + 1;
	int errMsgSize = 1;
	BOOL err = FALSE;
	if(errMsg!=NULL) {
		errMsgSize = NTPstrlen(errMsg) + 1;
		err = TRUE;
	}
	else {
		err = TRUE;
	}

	//add size for all the parameters plus null characters
	allSize = size1 + size2 + size3 + size4 + errMsgSize + 5;
	rv=(struct StringMessage*)NTPmalloc(sizeof(struct StringMessage) + allSize); 
	if(rv==NULL) return NULL;

	rv->context       = context;
	rv->type          = TYPE_STRING;
	rv->usage         = usage;
	rv->correlationId = correlationId;
	rv->code          = code;
	rv->param1Size    = size1;
	rv->param2Size    = size2;
	rv->param3Size    = size3;
	rv->param4Size    = size4;
	rv->errMsgSize    = errMsgSize;
	rv->isError       = err;
	NTPstrcpy((char*)rv->params, param1);
	NTPstrcpy((char*)rv->params + size1, param2);
	NTPstrcpy((char*)rv->params + size1 + size2, param3);
	NTPstrcpy((char*)rv->params + size1 + size2 + size3, param4);
	NTPstrcpy((char*)rv->params + size1 + size2 + size3 + size4, errMsg);

	return rv;
}

struct BinaryMessage *COMMNewBinaryMessage(uint32_t usage, 
                                           uint32_t correlationId,
                                           uint32_t code,
                                           const char *stringParam,
                                           uint32_t binaryParamSize,
                                           uint8_t  *binaryParam,
                                           const char *errMsg,
                                           void *context) {
	struct BinaryMessage *rv;
	int stringSize = strlen(stringParam) + 1;
	int errMsgSize = 1;
	int allSize;
	BOOL err = FALSE;
	if(errMsg!=NULL) {
		err = TRUE;
		errMsgSize = NTPstrlen(errMsg) + 1; //+1 because we send NULL across
	}
	else {
		errMsg = "";
	}

	//add size for all the parameters plus null characters
	allSize = stringSize + errMsgSize + binaryParamSize;
	rv = (struct BinaryMessage*)NTPmalloc(sizeof(struct BinaryMessage)+allSize);
	if(rv==NULL) return NULL;
	
	rv->type            = TYPE_BINARY;
	rv->context         = context;
	rv->usage           = usage;
	rv->correlationId   = correlationId;
	rv->code            = code;
	rv->stringParamSize = stringSize; 
	rv->binaryParamSize = binaryParamSize;
	rv->errMsgSize      = errMsgSize;
	rv->isError         = err;
	NTPstrcpy((char*)rv->params, stringParam);
	NTPstrcpy((char*)rv->params + stringSize, errMsg);
	NTPmemcpy((char*)rv->params + stringSize + errMsgSize,binaryParam,binaryParamSize);

	return rv;
}

const char *COMMgetStringParam(const struct Message*msg, int index) {
	if(msg->type == TYPE_BINARY) {
		struct BinaryMessage *bMsg = (struct BinaryMessage*)msg;
		if(index!=1) return NULL;
		return (char*)bMsg->params;
	}
	
	if(msg->type == TYPE_STRING) {
		struct StringMessage *sMsg = (struct StringMessage*)msg;
		const char *rv = (char*)sMsg->params;
		switch(index) {
			case 4: rv += sMsg->param3Size;
			case 3: rv += sMsg->param2Size;
			case 2: rv += sMsg->param1Size;
			case 1: return rv;
		}
	}

	return NULL;
}

const char *COMMgetBinaryParam(const struct BinaryMessage*msg) {
	return (char*)msg->params + msg->stringParamSize + msg->errMsgSize;
}

const char *COMMgetErrMessage(const struct Message *msg) {
	if(msg->type == TYPE_BINARY) {
		struct BinaryMessage *bMsg = (struct BinaryMessage *)msg;
		return (char*)bMsg->params + bMsg->stringParamSize;
	}
	
	if(msg->type==TYPE_STRING) {
		struct StringMessage *m = (struct StringMessage *)msg;
		return (char*)m->params + m->param1Size + m->param2Size + 
		                          m->param3Size + m->param4Size;
	}

	return NULL;
}



