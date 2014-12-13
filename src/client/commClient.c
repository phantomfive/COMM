#include <notrap/notrap.h>
#include "commClient.h"


typedef struct COMM_client{
	COMM_CLIENT_STATUS status;
	NTPSock *sock;
	char errMsg[3000];
}

//----------------------------------------------------------------------
// Public API
//----------------------------------------------------------------------

COMM_client *COMM_initClient(COMM_message_received_cb *messageReceivedCallback,
                     COMM_binary_message_received_cb *bMessageReceivedCallback,
                     void *context) {


}

void COMM_connect(COMM_client*obj,const char *serverName, uint16_t serverPort) {
	obj->sock = NTPConnectTCP(serverName, serverPort);
	obj->status = COMM_CLIENT_STATUS_CONNECTING;
}

void COMM_disconnect(COMM_client **obj) {

}

COMM_CLIENT_STATUS COMM_runClientLoop(COMM_client *obj) {

	//If connecting, check status
	if(obj->status==COMM_CLIENT_STATUS_CONNECTING)
		runConnectLoop(obj);

	if(obj->status==COMM_CLIENT_STATUS_CONNECTED) {

		//Work on send queue
		runSendLoop(obj);

		//Work on recv queue
		runRecvLoop(obj);
	}

	return obj->status;
}

//----------------------------------------------------------------------
// Private methods
//----------------------------------------------------------------------

static void runConnectLoop(COMM_client *obj) {
	if(obj->sock==NULL)
		setError(COMM_CLIENT_ERR_NO_MEM, "NTPConnect returned NULL");

	else if(NTPSockStatus(obj->sock)==NTP_ERROR)
		setError(CONN_CLIENT_ERR_CONNECT, NTPSockErr(obj->sock));

	else if(NTPSockStatus(obj->sock)==NTP_CONNECTED)
		obj->status = COMM_CLIENT_STATUS_CONNECTED;
}

static void runSendLoop(COMM_client *obj) {

}

static void runRecvLoop(COMM_client *obj) {

}

