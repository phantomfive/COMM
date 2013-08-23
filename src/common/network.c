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



typedef struct COMMsock_struct {
	COMMnet *net;       //put this here so we can figure out
	                    //which network we are connected to
	                    //without passing it in to every function
	NTPSock *sock;
	COMM_List *sendQueue;
	COMM_List *recvQueue;

	BOOL isListening;

}COMMSock;

//---------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------

static COMMStatus checkConnectingSockets(COMMnet *net) {
	//go through the list of sockets, and
	//query if it is done


	//if it is done, move it to the 


}

//---------------------------------------------------------------------
// public API
//---------------------------------------------------------------------

COMMnet *COMMinitNetwork() {
	COMMnet *rv = NTPmalloc(sizeof(COMMnet));
	if(rv==NULL) goto ERR_NO_MEM;

	rv->sendList = allocCOMM_List(200);
	rv->recvList = allocCOMM_List(200);
	rv->lstnList = allocCOMM_List(200);
	if(rv->sendList==NULL || rv->recvList==NULL || rv->lstnList==NULL)
		goto ERR_LISTS;

	//success
	return rv;

ERR_LISTS:
	freeCOMM_List(&rv->sendList);
	freeCOMM_List(&rv->recvList);
	freeCOMM_List(&rv->lstnList);
	NTPfree(rv);

ERR_NO_MEM:
	return NULL;
}


void COMMshutdownNetwork(COMMnet **net) {
	if(net==NULL || *net==NULL) return;

#error "Don't forget to remove the items in the list"
	freeCOMM_List(&(*net)->sendList);
	freeCOMM_List(&(*net)->recvList);
	freeCOMM_List(&(*net)->lstnList);

	NTPfree(*net);
	*net = NULL;
}

COMMStatus COMMrunNetwork(COMMnet *net) {
	COMMStatus rv;
	NTP_FD_SET readSet;
	NTP_FD_SET writeSet;
	NTP_ZERO_SET(&readSet);
	NTP_ZERO_SET(&writeSet);

	//add all sockets into set for doing a select
	//to see which ones need attention
	

	//go through all sockets and do the read/write or whatever
}


COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port) {
	//alloc space for the sock

	//connect the socket, and if not error, return it.
	//The user will have to wait for the connect to succeed or fail.


	//error handling

}


BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context) {

	//alloc memory for the new socket 

	//Start a new socket listening on the port

	//add it to our socket list

	//error handling
}


void COMMnetSendData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context) {

	//each socket needs to have a send queue and a recv queue.
	//So here we'll add it to the send queue
}

void COMMnetRecvData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context)  {
	//each socket needs to have a send queue and a recv queue
	//So here we'll add it to the recv queue
}









#endif

