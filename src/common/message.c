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
	rv=(struct StringMessage*)NTPmalloc(allSize + sizeof(struct Message)); 
	if(rv==NULL) return NULL;

	//copy the data to the newly allocated mem
	msg->type = type;
	msg->context = context;
	NTPstrcpy((char*)rv->data + 41, param1);
	NTPstrcpy((char*)rv->data + 41 + size1, param2);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2, param3);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2 + size3, param4);
	NTPstrcpy((char*)rv->data + 41 + size1 + size2 + size3 + size4, errMsg);

	//we store this stuff in network byte order
	allSize = ntohl(allSize);
	type    = ntohl(type);
	usage   = ntohl(usage);
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
	uint32_t type = BINARY_TYPE;
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
	rv = (struct Message*)NTPmalloc(allSize+sizeof(struct Message));    
	if(rv==NULL) return NULL;                         //It seems like there are magic
	                                                   //number all over the place,
	                                                   //but it has to be that way,
	                                                   //the protocol is defined in
	                                                   //terms of byte position
	msg->type = type;
	msg->context = context;
	NTPstrcpy((char*)rv->data, stringParam + 41);
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
	NTPmemcpy(rv->data + 20, &binaryParamSize, 4);
	NTPmemcpy(rv->data + 36, &errMsgSize     , 4);
	NTPmemcpy(rv->data + 40, &err            , 1);

	return rv;
}

const BOOL getSize(const struct Message *msg, uint32_t *sizeOut) {
	NTPmemcpy(sizeOut, msg->data, 4);
	*sizeOut = ntohl(*sizeOut);
	return TRUE;
}
const BOOL getType(const struct Message *msg, uint32_t *typeOut) {
	NTPmemcpy(typeOut, msg->data + 4, 4);
	*typeOut = ntohl(*typeOut);
	return TRUE;
}
const BOOL getUsage(const struct Message *msg, uint32_t *usageOut) {
	NTPmemcpy(usageOut, msg->data + 8, 4);
	*usageOut = ntohl(*usageOut);
	return TRUE;
}
const BOOL getCorrelationId(const struct Message *msg, uint32_t *cidOut) {
	NTPmemcpy(cidOut, msg->data + 12, 4);
	*cidOut = ntohl(*cidOut);
	return TRUE;
}
const BOOL getCode(const struct Message *msg, uint32_t *codeOut) {
	NTPmemcpy(codeOut, msg->data + 16, 4);
	*codeOut = ntohl(*codeOut);
	return TRUE;
}

//Some of these methods are for internal use, but may eventually be used externally
static BOOL getBinarySize(const struct Message *msg, uint32_t *sizeOut) {
	NTPmemcpy(codeOut, msg->data + 24, 4);
	*codeOut = ntohl(*codeOut);
	return TRUE;
}

//index starts with 1
static BOOL getStringParamSize(const struct Message *msg, int index,uint32_t*sizeOut){
	NTPmemcpy(sizeOut, msg->data + 20 + ((index-1)*4), 4);
	*sizeOut = ntohl(*sizeOut);
	return TRUE;
}


const char *COMMgetStringParam(const struct Message*msg, int index) {
	uint32_t param1Size=0, param2Size=0, param3Size=0, param4Size=0;
	char *rv;

	//index validation
	if(index<1 || index>4 || (msg->type==BINARY_TYPE && index>1)) return FALSE;

	//find out the size of the string parameters
	if(!getStringParamSize(msg, 1, &param1Size)) return NULL;
	if(msg->type == TYPE_STRING) {
		if(!getStringParamSize(msg, 2, &param2Size)   ||
		   !getStringParamSize(msg, 3, &param3Size)   ||
		   !getStringParamSize(msg, 4, &param4Size)) return NULL;
	}
	
	//find the start of the requested string message and return it
	rv = msg->data + 41;
	switch(index) {
		case 4: rv += sMsg->param3Size;
		case 3: rv += sMsg->param2Size;
		case 2: rv += sMsg->param1Size;
		case 1: return rv;
	}

	return NULL;  //can't happen
}

const uint8_t*COMMgetBinaryParam(const struct BinaryMessage*msg, uint32_t*sizeOut) {
	int stringSize;
	if(!getStringParamSize(msg, 1, &stringSize) ||
	   !getBinarySize(msg, sizeOut))      return NULL;
	return (char*)msg->data + stringSize + 41;
}

const char *COMMgetErrMessage(const struct Message *msg) {
	if(msg->type == TYPE_BINARY) {
		uint32_t stringSize, binSize;
		if(!getStringParamSize(msg, 1, &stringSize) ||
		   !getBinarySize(msg, &binSize)) return NULL;
		return (char*)msg->data + 41 + stringSize + binSize;
	}
	
	if(msg->type==TYPE_STRING) {
		uint32_t param1Size, param2Size, param3Size, param4Size;
		if(!getStringParamSize(msg, 1, &param1Size)   ||
		   !getStringParamSize(msg, 2, &param2Size)   ||
			!getStringParamSize(msg, 3, &param3Size)   ||
			!getStringParamSize(msg, 4, &param4Size)) return NULL;

		return(char*)msg->data + 41 + param1Size + param2Size + param3Size + param4Size;
	}

	return NULL; //shouldn't happen
}



