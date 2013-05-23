#include <stdlib.h>
#include <string.h>

#include "message.h"

/* message.c
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater.
 */

//------------------------------------------------------------------------
// Public API
//------------------------------------------------------------------------

void COMMFreeMessage(Message **msg) {
	if(msg==NULL || *msg==NULL) return;
	free(*msg);
	*msg = NULL;
}

struct StringMessage *COMMNewStringMessage(uint32_t type,
                                           uint32_t usage,
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
	int size1 = strlen(param1);
	int size2 = strlen(param2);
	int size3 = strlen(param3);
	int size4 = strlen(param4);
	int errMsgSize = 0;
	BOOL err = FALSE;
	if(errMsg!=NULL) {
		errMsgSize = strlen(errMsg);
		err = TRUE;
	}
	else {
		err = TRUE;
	}

	//add size for all the parameters plus null characters
	allSize = size1 + size2 + size3 + size4 + errMsgSize + 5;
	rv = (struct StringMessage*)malloc(sizeof(struct StringMessage) + allSize); 
	if(rv==NULL) return NULL;

	rv->context       = context;
	rv->type          = type;
	rv->usage         = usage;
	rv->correlationId = correlationId;
	rv->code          = code;
	rv->param1Size    = size1;
	rv->param2Size    = size2;
	rv->param3Size    = size3;
	rv->param4Size    = size4;
	rv->errMsgSize    = errMsgSize;
	rv->isError       = err;
	strcpy(rv->params, param1);
	strcpy(rv->params + size1 + 1, param2);
	strcpy(rv->params + size1 + size2 + 2, param3);
	strcpy(rv->params + size1 + size2 + size3 + 3, params4);
	strcpy(rv->params + size1 + size2 + size3 + 4, errMsg);

	return rv;
}

struct BinaryMessage *COMMNewBinaryMessage(uint32_t type, 
                                           uint32_t usage, 
                                           uint32_t correlationId,
                                           uint32_t code,
                                           const char *stringParam,
                                           uint32_t binaryParamSize,
                                           uint8_t  *binaryParam
                                           const char *errMsg,
                                           void *context) {
	int stringSize = strlen(stringParam);
	int errMsgSize = 0;
	int allSize;
	BOOL err = FALSE;
	if(errMsg!=NULL) {
		err = TRUE;
		errMsgSize = strlen(errMsg);
	}
	else {
		errMsg = "";
	}

	//add size for all the parameters plus null characters
	allSize = stringSize + errMsgSize + binaryParamSize + 2;
	
}

const char *COMMgetStringParam(const struct Message*msg, int index) {

}

const char *COMMgetBinaryParam(const struct BinaryMessage*msg) {

}

const char *COMMgetErrMessage(const struct Message *msg) {

}



