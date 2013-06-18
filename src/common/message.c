
#include "message.h"

/* message.c
 * This code requires good understanding of common binary strategies
 * in C, especially pointer arithmetic, and malloc() with a size larger
 * than the structure requires.
 * For method descriptions, see the header.
 *
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

struct Message *COMMNewStringMessage(uint32_t usage,
                                     uint32_t correlationId,
                                     uint32_t code,
                                     const char *param1,
                                     const char *param2,
                                     const char *param3,
                                     const char *param4,
                                     const char *errMsg,
                                     void *context) {
	struct Message *rv;
	int allSize;
	uint32_t type = TYPE_STRING;
	int size1 = NTPstrlen(param1) + 1;  //plus 1 because we send NULL chars
	int size2 = NTPstrlen(param2) + 1;
	int size3 = NTPstrlen(param3) + 1;
	int size4 = NTPstrlen(param4) + 1;
	int errMsgSize = 1;
	uint8_t err = FALSE;
	if(errMsg!=NULL) {
		errMsgSize = NTPstrlen(errMsg) + 1;
		err = TRUE;
	}
	else {
		errMsg = "";
		err = FALSE;
	}

	//allocate
	allSize = size1 + size2 + size3 + size4 + errMsgSize + 41;
	rv=(struct Message*)NTPmalloc(allSize + sizeof(struct Message)); 
	if(rv==NULL) return NULL;

	//copy the data to the newly allocated mem
	rv->type = type;
	rv->context = context;
	NTPstrcpy((char*)rv->data + 41                                , param1);
	NTPstrcpy((char*)rv->data + 41 + size1                        , param2);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2                , param3);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2 + size3        , param4);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2 + size3 + size4, errMsg);

	//we store this stuff in network byte order
	allSize       = ntohl(allSize);
	type          = ntohl(type);
	usage         = ntohl(usage);
	correlationId = ntohl(correlationId);
	code          = ntohl(code);
	size1         = ntohl(size1);
	size2         = ntohl(size2);
	size3         = ntohl(size3);
	size4         = ntohl(size4);
	errMsgSize    = ntohl(errMsgSize);
	
	NTPmemcpy(rv->data +  0, &allSize      , 4);
	NTPmemcpy(rv->data +  4, &type         , 4);
	NTPmemcpy(rv->data +  8, &usage        , 4);
	NTPmemcpy(rv->data + 12, &correlationId, 4);
	NTPmemcpy(rv->data + 16, &code         , 4);
	NTPmemcpy(rv->data + 20, &size1        , 4);
	NTPmemcpy(rv->data + 24, &size2        , 4);
	NTPmemcpy(rv->data + 28, &size3        , 4);
	NTPmemcpy(rv->data + 32, &size4        , 4);
	NTPmemcpy(rv->data + 36, &errMsgSize   , 4);
	NTPmemcpy(rv->data + 40, &err          , 1);

	return rv;
}

struct Message *COMMNewBinaryMessage(uint32_t usage, 
                                     uint32_t correlationId,
                                     uint32_t code,
                                     const char *stringParam,
                                     uint32_t binaryParamSize,
                                     uint8_t  *binaryParam,
                                     const char *errMsg,
                                     void *context) {
	struct Message *rv;
	int stringSize = strlen(stringParam) + 1;
	int errMsgSize = 1;
	int allSize;
	uint32_t type = TYPE_BINARY;
	uint8_t err = FALSE;
	if(errMsg!=NULL) {
		err = TRUE;
		errMsgSize = NTPstrlen(errMsg) + 1; //+1 because we send NULL across
	}
	else {
		errMsg = "";
	}

	//add size for all the parameters plus null characters
	allSize = stringSize + errMsgSize + binaryParamSize + 41;
	rv = (struct Message*)NTPmalloc(sizeof(struct Message)+allSize);    
	if(rv==NULL) return NULL;                    //It seems like there are magic
	                                             //number all over the place,
	                                             //but it has to be that way,
	                                             //the protocol is defined in
	                                             //terms of byte position
	rv->type = type;
	rv->context = context;
	NTPstrcpy((char*)rv->data + 41,stringParam);
	NTPmemcpy((char*)rv->data + stringSize + 41,binaryParam,binaryParamSize);
	NTPstrcpy((char*)rv->data + stringSize + 41 + binaryParamSize, errMsg);

	//We store it in network order
	allSize         = htonl(allSize);
	type            = htonl(type);
	usage           = htonl(usage);
	correlationId   = htonl(correlationId);
	code            = htonl(code);
	stringSize      = htonl(stringSize);
	binaryParamSize = htonl(binaryParamSize);
	errMsgSize      = htonl(errMsgSize);
	NTPmemcpy(rv->data + 0 , &allSize        , 4);
	NTPmemcpy(rv->data + 4 , &type           , 4);
	NTPmemcpy(rv->data + 8 , &usage          , 4);
	NTPmemcpy(rv->data + 12, &correlationId  , 4);
	NTPmemcpy(rv->data + 16, &code           , 4);
	NTPmemcpy(rv->data + 20, &stringSize     , 4);
	NTPmemcpy(rv->data + 24, &binaryParamSize, 4);
	NTPmemcpy(rv->data + 36, &errMsgSize     , 4);
	NTPmemcpy(rv->data + 40, &err            , 1);

	return rv;
}

BOOL COMMgetSize(const struct Message *msg, uint32_t *sizeOut) {
	NTPmemcpy(sizeOut, msg->data, 4);
	*sizeOut = ntohl(*sizeOut);
	return TRUE;
}
BOOL COMMgetType(const struct Message *msg, uint32_t *typeOut) {
	NTPmemcpy(typeOut, msg->data + 4, 4);
	*typeOut = ntohl(*typeOut);
	return TRUE;
}
BOOL COMMgetUsage(const struct Message *msg, uint32_t *usageOut) {
	NTPmemcpy(usageOut, msg->data + 8, 4);
	*usageOut = ntohl(*usageOut);
	return TRUE;
}
BOOL COMMgetCorrelationId(const struct Message *msg, uint32_t *cidOut) {
	NTPmemcpy(cidOut, msg->data + 12, 4);
	*cidOut = ntohl(*cidOut);
	return TRUE;
}
BOOL COMMgetCode(const struct Message *msg, uint32_t *codeOut) {
	NTPmemcpy(codeOut, msg->data + 16, 4);
	*codeOut = ntohl(*codeOut);
	return TRUE;
}

BOOL COMMgetIsErr(const struct Message *msg, BOOL *errOut) {
	*errOut = msg->data[40];
	return TRUE;
}

//Some of these methods are for internal use, 
//but may eventually be used externally
static BOOL COMMgetBinarySize(const struct Message *msg, uint32_t *sizeOut) {
	NTPmemcpy(sizeOut, msg->data + 24, 4);
	*sizeOut = ntohl(*sizeOut);
	return TRUE;
}

//index starts with 1
static BOOL COMMgetStringParamSize(const struct Message *msg, int index,
                               uint32_t*sizeOut){
	NTPmemcpy(sizeOut, msg->data + 20 + ((index-1)*4), 4);
	*sizeOut = ntohl(*sizeOut);
	return TRUE;
}


const char *COMMgetStringParam(const struct Message*msg, int index) {
	uint32_t param1Size=0, param2Size=0, param3Size=0, param4Size=0;
	char *rv;

	//index validation
	if(index<1 || index>4 || (msg->type==TYPE_BINARY && index>1)) return FALSE;

	//find out the size of the string parameters
	if(!COMMgetStringParamSize(msg, 1, &param1Size)) return NULL;
	if(msg->type == TYPE_STRING) {
		if(!COMMgetStringParamSize(msg, 2, &param2Size)   ||
		   !COMMgetStringParamSize(msg, 3, &param3Size)   ||
		   !COMMgetStringParamSize(msg, 4, &param4Size)) return NULL;
	}
	
	//find the start of the requested string message and return it
	rv = (char*)msg->data + 41;
	switch(index) {
		case 4: rv += param3Size;
		case 3: rv += param2Size;
		case 2: rv += param1Size;
		case 1: return rv;
	}

	return NULL;  //can't happen
}

const uint8_t*COMMgetBinaryParam(const struct Message*msg, 
                                 uint32_t*sizeOut) {
	uint32_t stringSize;
	if(msg->type!=TYPE_BINARY) return NULL;

	if(!COMMgetStringParamSize(msg, 1, &stringSize) ||
	   !COMMgetBinarySize(msg, sizeOut))      return NULL;
	return (uint8_t*)msg->data + stringSize + 41;
}

const char *COMMgetErrMessage(const struct Message *msg) {
	if(msg->type == TYPE_BINARY) {
		uint32_t stringSize, binSize;
		if(!COMMgetStringParamSize(msg, 1, &stringSize) ||
		   !COMMgetBinarySize(msg, &binSize)) return NULL;
		return (char*)msg->data + 41 + stringSize + binSize;
	}
	
	if(msg->type==TYPE_STRING) {
		uint32_t param1Size, param2Size, param3Size, param4Size;
		if(!COMMgetStringParamSize(msg, 1, &param1Size)   ||
		   !COMMgetStringParamSize(msg, 2, &param2Size)   ||
			!COMMgetStringParamSize(msg, 3, &param3Size)   ||
			!COMMgetStringParamSize(msg, 4, &param4Size)) return NULL;

		return(char*)msg->data + 41 + param1Size + 
		                              param2Size + 
		                              param3Size + 
		                              param4Size;
	}

	return NULL; //shouldn't happen
}



