/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/

#include "commList.h"

#include "network.h"


typedef struct COMMnet_struct {

	COMM_List *sendList;
	COMM_List *recvList;
	COMM_List *lstnList;
	COMM_List *cnctList;

}COMMnet;


typedef struct COMMsock_struct {
	NTPSock *sock;


}COMMSock;

//---------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// public API
//---------------------------------------------------------------------

COMMnet *COMMinitNetwork() {
	COMMnet *rv = NTPmalloc(sizeof(COMMnet));
	if(rv==NULL) goto ERR_NO_MEM;

	rv->sendList = allocCOMM_List(200);
	rv->recvList = allocCOMM_List(200);
	rv->lstnList = allocCOMM_List(200);
	rv->cnctList = allocCOMM_List(200);
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
	freeCOMM_List(&(*net)->cnctList);

	NTPfree(*net);
	*net = NULL;
}

COMMStatus COMMrunNetwork(COMMnet *net) {
}


COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port) {

}


BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context) {

}


void COMMnetSendData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context) {

}

void COMMnetRecvData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context)  {

}









#endif

