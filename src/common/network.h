#ifndef COMM_NETWORK_H
#define COMM_NETWORK_H
/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/
#include <notrap.h>
#include "commSock.h"

typedef struct COMMnet_struct COMMnet;

//---------------------------------------------------------------------
// Callback methods. Need to be declared before the API
//---------------------------------------------------------------------
/**Called when a socket listening on a port accepts a socket.
 * If there is some kind of error, then acceptedSock will be NULL.
 * Usually this is an out-of-memory or interrupt error, or 
 * if COMMshutdownNetwork() is called.*/
void (*COMMnetListen_cb)(COMMnet*net,const COMMSock*acceptedSock,
                         uint16_t port, void*context);

/**Called when a message is sent successfully.
 * If there is a send error, this will get called and bytesSent will be
 * a negative number.*/
void (*COMMnetSent_cb)(COMMnet *net, const COMMSock *sock, int bytesSent, 
                       uint8_t*data,void *context);

/**Called when a message is received successfully.
 * If there is a receive error, this will get called and bytesRecvd will
 * be a negative number. */
void (*COMMnetRecvd_cb)(COMMnet *net,const COMMSock *sock, int bytesRecvd,
                       uint8_t*data,void*context);



//---------------------------------------------------------------------
// public API
//---------------------------------------------------------------------

/*Only fails if there is no memory*/
COMMnet *COMMinitNetwork();
void COMMshutdownNetwork(COMMnet **net);

/*Runs the network for one loop. Call frequently.*/
BOOL COMMrunNetwork(COMMnet *net);

const char *COMMnetGetLastErr(COMMnet *net);


//need to connect
COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port);


//need to listen
BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context);

//close the sock, set *sock to NULL
void COMMnetCloseSock(COMMSock **sock);

//Absolutely need these
void COMMnetSendData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context);

void COMMnetRecvData(COMMSock *sock, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context);









#endif

