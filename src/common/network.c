/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/

#include "commList.h"

#include "network.h"


typedef struct COMMnet_struct {

	COMM_List *sockList;

}COMMnet;


typedef struct dataChunk {
	void *data;
	int  len;
	int timeoutSeconds;

	void *context;
	COMMnetSent_cb  *sendCb;
	COMMnetRecvd_cb *recvCb;
} DataChunk;

typedef struct COMMsock_struct {
	COMMnet *net;       //put this here so we can figure out
	                    //which network we are connected to
	                    //without passing it in to every function
	NTPSock *sock;
	COMM_List *sendQueue;
	COMM_List *recvQueue;

	//for listening sockets
	BOOL isListening;
	COMMnetAccept_cb *accept_cb;
	void *acceptContext;

}COMMSock;

//---------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------

static void doSend(COMMSock *sock, int index) {

}

static void doRecv(COMMSock *sock, int index) {

}

static void doAccept(COMMSock *sock, int index) {

}

//---------------------------------------------------------------------
// public API
//---------------------------------------------------------------------

COMMnet *COMMinitNetwork() {
	COMMnet *rv = NTPmalloc(sizeof(COMMnet));
	if(rv==NULL) goto ERR_NO_MEM;

	rv->sockList = allocCOMM_List(200);
	if(rv->sockList==NULL) {
		NTPFree(rv);
		rv = NULL;
	}

	return rv;
}


void COMMshutdownNetwork(COMMnet **net) {
	if(net==NULL || *net==NULL) return;

#error "Don't forget to remove the items in the list"
	freeCOMM_List(&(*net)->sockList);

	NTPfree(*net);
	*net = NULL;
}

COMMStatus COMMrunNetwork(COMMnet *net) {
	int i;
	COMMSock *sock;
	int size;
	NTP_FD_SET readSet;
	NTP_FD_SET writeSet;
	NTP_ZERO_SET(&readSet);
	NTP_ZERO_SET(&writeSet);

	//add all sockets into set for doing a select
	//to see which ones need attention
	size = COMM_ListSize(net->sockList);
	for(i=0;i<size;i++) {
		COMM_ListObjectAtIndex(net->sockList, &sock, i);
#error "Should we check this error?"

#error "Make sure the sockets are connected before doing this"
		if(sock->isListening || COMM_ListSize(sock->recvQueue)>0) {
			NTP_FD_ADD(sock, &readSet);
		}
		if(COMM_ListSize(sock->sendQueue)>0 && sockStatusIsConnected) {
			NTP_FD_Add(sock, &writeSet);
		}
	}
	

	//now actually do the select
	result = NTPSelect(readSet, writeSet, 1);
	if(result<0) {
#error "Definitely handle this error"
	}
	else if(result==0) {
		//timed out, no problem
		return NTPSuccess;
	}

	//go through all sockets and do the read/write or whatever
	//some of these can be removed while going through the list
	//so go through it backwards
	for(i=size-1;i>=0;i--) {
		COMM_ListObjectAtIndex(net->sockList, &sock, i);
#error "Once again, should we check this error?"
		
		if(NTP_FD_ISSET(sock, readSet)) {
			if(sock->isListening) {
				doAccept(sock, i);
			}else {
				doRecv(sock, i);
			}
		}
		if(NTP_FD_ISSET(sock, writeSet)) {
			doSend(sock, i);
		}
	}

#error "Handle return value here"
}


COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port) {
	//alloc space for the sock
	COMMSock *rv = (COMMSock*)malloc(sizeof(COMMSock));
	if(rv==NULL) goto ERR_NO_MEM;
	rv->sendQueue = allocCOMM_List(1000);
	rv->recvQueue = allocCOMM_List(1000);
	if(rv->sendQueue==NULL || rv->recvQueue==NULL) goto ERR_QUEUES;

	//add it to our socket list
	if(!COMM_ListPushBack(net->sockList,rv)) goto ERR_FULL;

	//connect the socket, and if not error, return it.
	//The user will have to wait for the connect to succeed or fail.
	rv->net = net;
	rv->isListening = FALSE;
	rv->sock = NTPConnectTCP(dest, port);
	if(rv->sock==NULL) goto ERR_CONNECT;


	return rv; //SUCCESS

#error "Deal with error messages somehow"

	//error handling
ERR_CONNECT:
	COMM_ListRemoveAtIndex(net->sockList, NULL, COMM_ListSize(net->sockList)-1);
ERR_FULL:
ERR_QUEUES:
	freeCOMM_List(&rv->sendQueue);
	freeCOMM_List(&rv->recvQueue);
	NTPFree(rv);

ERR_NO_MEM:
	return NULL;
}


BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context) {

	//alloc memory for the new socket 
	COMMSock *rv = (COMMSock*)NTPMalloc(sizeof(COMMSock));
	if(rv==NULL) goto ERR_NO_MEM;
	rv->sendQueue = allocCOMM_List(0); //can be zero since we aren't sending
	rv->recvQueue = allocCOMM_List(0); //can be zero since we aren't receiving
	if(rv->sendQueue==NULL || rv->recvQueue==NULL) goto ERR_QUEUES;

	//add it to our socket list
	if(!COMM_ListPushBack(net->sockList,rv)) goto ERR_FULL;

	//Start a new socket listening on the port
	rv->net = net;
	rv->isListening = TRUE;
	rv->sock = NTPListen(port);
	if(rv->sock==NULL) goto ERR_LISTEN;

	return rv; //SUCCESS

#error "deal with error messages somehow"
	//error handling
ERR_LISTEN:
	COMM_ListRemoveAtIndex(net->sockList, NULL, COMM_ListSize(net->sockList)-1);
ERR_FULL:
ERR_QUEUES:
	freeCOMM_List(&rv->sendQueue);
	freeCOMM_List(&rv->recvQueue);
	NTPFree(rv);

ERR_NO_MEM:
	return NULL;
}


void COMMnetSendData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context) {
	
	//allocate memory for our msg
	DaTaChunk *msg = (DataChunk*)NTPMalloc(sizeof(DataChunk));
	if(msg==NULL) {
		notifySendErr(sock, data, len, cb, context, "Out of mem in COMMnetSendData");
		return;
	}
	
	//fill in the fields of the message
	msg->data = data;
	msg->len  = len;
	msg->timeoutSeconds = timeoutSeconds;
	msg->context        = context;
	msg->sendCb         = cb;
	msg->recvCb         = NULL;

	//put it on the send list
	if(!COMM_ListPushBack(sock->sendQueue, msg)) {
		notifySendErr(sock, data, len, cb, context,"No room on send queue in COMMnetSendData");
	}
}

void COMMnetRecvData(COMMSock *sock, const uint8_t *data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context)  {
	
	//allocate memory for our msg
	DataChunk *msg = (DataChunk*)NTPMalloc(sizeof(DataChunk));
	if(msg==NULL) {
		notifyRecvErr(sock, data, len, cb, context, "Out of memory in COMMnetRecvData");
		return;
	}

	//fill in the fields of the message
	msg->data           = data;
	msg->len            = len;
	msg->timeoutSeconds = timeoutSeconds;
	msg->context        = context;
	msg->sendCb         = NULL;
	msg->recvCb         = cb;

	//add the message to the recv queue
	if(!COMM_ListPushBack(sock->recvQueue, msg)) {
		notifyRecvErr(sock, data, len, cb, context, "List full in COMMnetRecvData");
	}
}


