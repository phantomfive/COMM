#include <stdio.h>
#include <CuTest.h>
#include "../src/common/message.h"

//Here we're just creating a string message and making sure all the fields are
//accessible correctly
void testCreateStringMessage(CuTest *tc) {
	uint32_t usage         = USAGE_INTERNAL;
	uint32_t correlationId = 22;
	uint32_t code          = 55;
	char *param1 = "This is our first parameter";
	char *param2 = "Einstein was a great genius, believe it or not";
	char *param3 = "When he spoke, the world bent his way.";
	char *param4 = "Except when he talked about world peace.";
	char *errMsg = NULL;
	void *context= (void*)88;
	uint32_t out;
	BOOL err;
	struct Message *msg = COMMNewStringMessage(usage, correlationId,
	                                 code, param1, param2, param3, param4,
	                                 errMsg, context);
	CuAssert(tc, "strMsg not null", msg!=NULL);
	CuAssert(tc, "check type",  COMMgetType         (msg, &out) && out==TYPE_STRING  );
	CuAssert(tc, "check usage", COMMgetUsage        (msg, &out) && out==usage        );
	CuAssert(tc, "check cid",   COMMgetCorrelationId(msg, &out) && out==correlationId);
	CuAssert(tc, "check code",  COMMgetCode         (msg, &out) && out==code         );
	CuAssert(tc, "check param1", NTPstrcmp(param1, COMMgetStringParam((struct Message*)msg,1))==0);
	CuAssert(tc, "check param2", NTPstrcmp(param2, COMMgetStringParam((struct Message*)msg,2))==0);
	CuAssert(tc, "check param3", NTPstrcmp(param3, COMMgetStringParam((struct Message*)msg,3))==0);
	CuAssert(tc, "check param4", NTPstrcmp(param4, COMMgetStringParam((struct Message*)msg,4))==0);
	CuAssert(tc, "check err is 0 len",NTPstrlen(   COMMgetErrMessage((struct Message*)msg))==0);
	CuAssert(tc, "check isError", COMMgetIsErr(msg, &err) && err==FALSE);
	CuAssert(tc, "check context", msg->context==context);
	COMMFreeMessage((struct Message**)&msg);
	CuAssert(tc, "check message null", msg==NULL);

	//now test it again, but this time make errormsg not NULL
	errMsg = "What an  unfortunate error to come upon at this time";
	msg = COMMNewStringMessage(usage, correlationId,
	                           code, param1, param2, param3, param4, errMsg, context);
	CuAssert(tc, "strMsg not null", msg!=NULL);
	CuAssert(tc, "check type",  COMMgetType         (msg, &out) && out==TYPE_STRING  );
	CuAssert(tc, "check usage", COMMgetUsage        (msg, &out) && out==usage        );
	CuAssert(tc, "check cid",   COMMgetCorrelationId(msg, &out) && out==correlationId);
	CuAssert(tc, "check code",  COMMgetCode         (msg, &out) && out==code         );
	CuAssert(tc, "check param1", NTPstrcmp(param1, COMMgetStringParam(msg,1))==0);
	CuAssert(tc, "check param2", NTPstrcmp(param2, COMMgetStringParam(msg,2))==0);
	CuAssert(tc, "check param3", NTPstrcmp(param3, COMMgetStringParam(msg,3))==0);
	CuAssert(tc, "check param4", NTPstrcmp(param4, COMMgetStringParam(msg,4))==0);
	CuAssert(tc, "check errmsg", NTPstrcmp(errMsg, COMMgetErrMessage(msg))==0);
	CuAssert(tc, "check isError", COMMgetIsErr(msg,&err) && err==TRUE);
	CuAssert(tc, "check context", msg->context==context);
	COMMFreeMessage((struct Message**)&msg);
	CuAssert(tc, "check message null", msg==NULL);

	//ok, I guess that's all
}

void testCreateBinaryMessage(CuTest *tc) {
	uint32_t usage = USAGE_USER;
	uint32_t correlationId = 12380;
	uint32_t code  = 119945;
	uint32_t out;
	char *   stringParam = "Every parameter should be a string";
	char *   binaryParam = "Hey, even ascii is binary";
	uint32_t binaryParamSize = strlen(binaryParam) + 1;
	char *   errMsg = NULL;
	BOOL err;
	void *context = (void*)1234;
	struct Message *msg = COMMNewBinaryMessage(usage, correlationId,
	                       code, stringParam, binaryParamSize, (uint8_t*)binaryParam,
	                       errMsg, context);
	CuAssert(tc, "binMsg not null", msg!=NULL);
	CuAssert(tc, "check type", msg->type==TYPE_BINARY);
	CuAssert(tc, "check type2", COMMgetType(msg, &out) && out==TYPE_BINARY);
	CuAssert(tc, "check usage", COMMgetUsage(msg, &out) && usage == out);
	CuAssert(tc, "check corr ID", COMMgetCorrelationId(msg, &out) && out==correlationId);
	CuAssert(tc, "check str",NTPstrcmp(stringParam, COMMgetStringParam((struct Message*)msg,1))==0);
	CuAssert(tc, "check bin", NTPstrcmp(binaryParam, (char*)COMMgetBinaryParam(msg, &out))==0);
	CuAssert(tc, "check bin size", binaryParamSize == out);
	CuAssert(tc, "check err", COMMgetIsErr(msg, &err) && !err);
	CuAssert(tc, "check err is 0 len",NTPstrlen(COMMgetErrMessage((struct Message*)msg))==0);
	CuAssert(tc, "check context", msg->context==context);
	COMMFreeMessage((struct Message**)&msg);
	CuAssert(tc, "check message null", msg==NULL);
	
	//Now check with errMsg not null
	errMsg = "Yet another error. How tiring.";
	msg = COMMNewBinaryMessage(usage, correlationId,
	          code, stringParam, binaryParamSize, (uint8_t*)binaryParam, errMsg, context);
	CuAssert(tc, "binMsg not null", msg!=NULL);
	CuAssert(tc, "check type", msg->type==TYPE_BINARY);
	CuAssert(tc, "check type2", COMMgetType(msg, &out) && out==TYPE_BINARY);
	CuAssert(tc, "check usage", COMMgetUsage(msg, &out) && usage==out);
	CuAssert(tc, "check corr ID", COMMgetCorrelationId(msg, &out) && out==correlationId);
	CuAssert(tc, "check str",NTPstrcmp(stringParam, COMMgetStringParam((struct Message*)msg,1))==0);
	CuAssert(tc, "check bin", NTPstrcmp(binaryParam, (char*)COMMgetBinaryParam(msg, &out))==0);
	CuAssert(tc, "check bin size", binaryParamSize == out);
	CuAssert(tc, "check err", COMMgetIsErr(msg, &err) && !err);
	CuAssert(tc, "check err str",NTPstrcmp(errMsg,COMMgetErrMessage((struct Message*)msg))==0);
	CuAssert(tc, "check context", msg->context==context);
	COMMFreeMessage((struct Message**)&msg);
	CuAssert(tc, "check message null", msg==NULL);
	
}

CuSuite *getMessageSuite(void) {
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testCreateStringMessage);
	SUITE_ADD_TEST(suite, testCreateBinaryMessage);
	return suite;
}
