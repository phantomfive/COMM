/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/

#include "commList.h"

#include "network.h"

COMMnet *COMMinitNetwork() {
}

void COMMshutdownNetwork(COMMnet **net) {
}

BOOL COMMrunNetwork(COMMnet *net) {

}

const char *COMMnetGetLastErr(COMMnet *net) {
}

COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port) {

}


BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context) {
}

void COMMnetCloseSock(COMMSock **sock) {

}

void COMMnetRecvData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context) {
	COMMSockRecv(sock, data, len, timeoutSeconds, cb, context);
}

void COMMnetSendData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context) {
	COMMSockSend(sock, data, len, timeoutSeconds, cb, context);
}
