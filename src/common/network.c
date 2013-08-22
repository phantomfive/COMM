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
	NTPSock *sock;
	
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


void COMMnetSendData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context) {

	//arg, this is the hardest part.
	//Have to figure out how to send and receive.
	//Should it put the message on a queue? Or should it only allow one
	//send at a time?

}

void COMMnetRecvData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context)  {

}









#endif

