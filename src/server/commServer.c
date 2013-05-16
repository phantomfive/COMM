#ifndef COMM_SERVER_H

#include "notrap.h"

typedef struct COMM_server_struct COMM_server;
typedef struct COMM_connection_struct COMM_connection;

struct COMM_server_struct {
	COMM_connectionStatusCallback *connectionStatusCallback;
	void *connectionStatusContext;
	COMM_messageResponseReceived_cb *messageResponseCallback;
	COMM_binaryResponseReceived_cb  *binaryResponseCallback;
	int port;
	COMM_STATE state;
	COMM_ServerStatus errorState;
};


//---------------------------------------------------------------------------
// Functions for starting, stopping, and running the COMM server
//---------------------------------------------------------------------------

COMM_server *COMM_initServer(COMM_connectionStatusCallback   *css_cb,
                             void *connectionStatusContext,
                             COMM_messageResponseReceived_cb *mrr_cb,
                             COMM_binaryResponseReceived_cb  *bmrr_cb) {

	COMM_server *rv = (COMM_server*)malloc(sizeof(COMM_server));
	if(rv==NULL) return;

	rv->connectionStatusCallback = css_cb;
	rv->connectionStatusContext  = connectionStatusContext;
	rv->messageResponseCallback  = mrr_cb;
	rv->binaryResponseCallback   = bmmr_cb;
	rv->errorState = COMM_HAPPY;

	return rv;
}

int COMM_beginListening(int port) {
	
}

COMM_ServerStatus COMM_runServerLoop(COMM_server *obj) {
	if(obj->connected) {
		sendReceiveMessages(obj);
		processConnections(obj);
	}

	return obj->errorState;
}

void COMM_shutdownServer(COMM_server **obj) {
	if(obj==NULL || *obj==NULL) return;

	free(*obj);
	*obj = NULL;
}

//----------------------------------------------------------------------------
// Functions for sending messages
//----------------------------------------------------------------------------
int COMM_sendMessage(COMM_connection*client, int32_t code,
                     const char *param1,
                     const char *param2,
                     const char *param3,
                     const char *param4, void *context) {
}

int COMM_sendBinaryMessage(COMM_connection*client, int32_t code,
                           const char *param1,
                           const char *binaryPayload,
                           int binarySize,
                           void *context) {
}


#endif

