/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/

#include "commList.h"

#include "network.h"


typedef struct COMMnet_struct {

	COMM_List *sockList;
	BOOL inErrState;
	const char *lastError;

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

	int currentAmountRead;
	int currentAmountWritten;

	//for listening sockets
	BOOL isListening;
	uint16_t port;
	COMMnetAccept_cb *accept_cb;
	void *acceptContext;

}COMMSock;

//---------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------

static BOOL setLastError(COMMnet *net, const char *msg) {
	net->lastError  = msg;
	net->inErrState = TRUE;
	return FALSE;
}

//allocate a sock and set some initial defaults
//returns NULL if there's no memory
static COMMSock *allocNewSock(COMMnet *net) {
	//alloc space for the sock
	COMMSock *rv = (COMMSock*)NTPmalloc(sizeof(COMMSock));
	if(rv==NULL) {
		setLastError(net, "No memory");
		return NULL;
	}

	//allocate space for the queues
	rv->sendQueue = allocCOMM_List(1000);
	rv->recvQueue = allocCOMM_List(1000);
	if(rv->sendQueue==NULL || rv->recvQueue==NULL) {
		freeCOMM_List(&rv->sendQueue);
		freeCOMM_List(&rv->recvQueue);
		setLastError(net, "Out of memory");
		return NULL;
	}

	//add it to our socket list
	if(!COMM_ListPushBack(net->sockList,rv)) {
		setLastError(net, "Socket list is full, too many sockets open");
		COMMnetCloseSock(&rv);
		return NULL;
	}

	//set some reasonable defaults
	rv->net = net;
	rv->port = 0;
	rv->isListening = FALSE;
	rv->currentAmountWritten = 0;
	rv->currentAmountRead    = 0;
	return rv;
}


static void doSend(COMMSock *sock, int index) {
	//call NTP send
	DataChunk *buf = (DataCHunk*)COMM_ListObjectAtIndex(sock->sendQueue, 0);
	char *bufferStart = (char*)buf->data + sock->currentAmountWritten;
	int lenToWrite = buf->len - sock->currentAmountWritten;
	int bytesWritten = NTPSend(sock->sock, bufferStart, lenToWrite);

	//handle sock error
	if(bytesWritten<=0) {
		setLastError(net, NTPSockErr(sock->sock));
		buf->sendCb(sock->net, sock, bytesWritten, buf->data, buf->context);
		COMMnetCloseSock(&sock);
		return;
	}

	//adjust send pointer
	sock->currentAmountWritten += bytesWritten;

	//if not entire section written, return and try again next time
	if(sock->currentAmountWritten < buf->len) return;

	//notify user and pop send stack
	buf->sendCb(sock->net, sock, sock->currentAmountWritten, buf->data, buf->context);
	sock->currentAmountWritten = 0;
	COMM_ListRemoveAtIndex(sock->buf, NULL, 0);
	NTPFree(buf);
}

static void doRecv(COMMSock *sock, int index) {
	//call NTP recv
	DataChunk* buf = (DataChunk*)COMM_ListObjectAtIndex(sock->recvQueue,0);
	char * bufferStart = (char*)buf->data + sock->currentAmountRead;
	int lenToRead = buf->len - sock->currentAmountRead;
	int bytesRead = NTPRecv(sock->sock, bufferStart, lenToRead);

	//handle sock error
	if(bytesRead<=0) {
		setLastError(net, NTPSockErr(sock->sock));
		buf->recvCb(sock->net, sock, -1, buf->data, buf->context);
		COMMnetCloseSock(&sock);
		return;
	}

	//adjust recv pointer
	sock->currentAmountRead += bytesRead;

	//if not entire section received, return and try again next time
	if(sock->currentAmountRead < buf->len) return;

	//pass result on to user, pop from stack
	buf->recvCb(sock->net, sock, sock->currentAmountRead, buf->data, buf->context);
	sock->currentAmountRead = 0;
	COMM_ListRemoveAtIndex(sock->buf, NULL, 0);
	NTPFree(buf);
}

static void doAccept(COMMSock *listenSock, int index) {
	COMMSock *rv; //it's called rv even though it's not
	              //returned, it's passed on to the end user
	NTPSock *newSock = NTPAccept(listenSock->sock);
	if(newSock==NULL) {
		//This is probably a temporary error
		setLastError(net, NTPSockErr(listenSock->sock));
		listenSock->accept_cb(listenSock->net, NULL, listenSock->port, listenSock->context);
		return;
	}
	rv = allocNewSock(listenSock->net);
	if(rv==NULL) {
		NTPDisconnect(&newSock);
		listenSock->accept_cb(listenSock->net, NULL, listenSock->port, listenSock->context);
		return;
	}
	
	rv->sock = newSock;
	listenSock->accept_cb(listenSock->net, newSock, listenSock->port, listenSock->context);
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

const char *COMMnetGetLastErr(COMMnet *net) {
	return net->lastError;
}

//Select seems simple enough but functions using it
//always seem to end up so lengthy
BOOL COMMrunNetwork(COMMnet *net) {
	int i;
	COMMSock *sock;
	int size;

	NTP_FD_SET readSet;
	NTP_FD_SET writeSet;
	NTP_ZERO_SET(&readSet);
	NTP_ZERO_SET(&writeSet);

	if(net->inErrState) return FALSE;

	//add all sockets into set for doing a select
	//to see which ones need attention.
	//Go through the list in reverse because if it's an error we could remove
	//elements.
	size = COMM_ListSize(net->sockList);
	for(i=size-1;i>=0;i--) {
		COMM_ListObjectAtIndex(net->sockList, &sock, i);

		status = NTPSockStatus(sock->sock);
		if(status==NTPSOCK_CONNECTING) continue;
		if(status==NTPSOCK_ERROR) {handleSockErr(sock, i); continue;}

		//add to write set or read set
		if(sock->isListening || COMM_ListSize(sock->recvQueue)>0) {
			NTP_FD_ADD(sock->sock, &readSet);
		}
		if(COMM_ListSize(sock->sendQueue)>0) {
			NTP_FD_Add(sock-sock, &writeSet);
		}
	}
	

	//now actually do the select
	result = NTPSelect(readSet, writeSet, 1);
	if(result<0) {
		return setLastError(net, NTPSockErr(NULL));
	}
	else if(result==0) {
		//timed out, no problem
		return TRUE;
	}

	//go through all sockets and do the read/write or whatever.
	//Some of these can be removed while going through the list
	//so go through it backwards
	for(i=size-1;i>=0;i--) {
		COMM_ListObjectAtIndex(net->sockList, &sock, i);
		
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
	
	return TRUE;
}


COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port) {
	COMMSock *rv = allocNewSock(net);

	//connect the socket, and if not error, return it.
	//The user will have to wait for the connect to succeed or fail.
	rv->sock = NTPConnectTCP(dest, port);
	if(rv->sock==NULL) {
		setLastError(net, NTPSockErr(rv->sock));
		COMMnetCloseSock(&rv);
		return NULL;
	}

	return rv; //SUCCESS
}


COMMSock *COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context) {

	//alloc memory for the new socket 
	COMMSock *rv = allocNewSock(net);
	if(rv==NULL) {
		setLastError(net, "Error no memory, COMMnetListen");
		return NULL;
	}

	//Start a new socket listening on the port
	rv->isListening = TRUE;
	rv->port = port;
	rv->sock = NTPListen(port);
	if(rv->sock==NULL){
		COMM_ListRemoveAtIndex(net->sockList, NULL, COMM_ListSize(net->sockList)-1);
		return NULL;
	}

	return rv; //SUCCESS
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


