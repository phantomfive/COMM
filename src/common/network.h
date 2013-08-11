#ifndef COMM_NETWORK_H
#define COMM_NETWORK_H
/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/
#include <notrap.h>

struct COMMnet;

//---------------------------------------------------------------------
// Callback methods. Need to be declared before the API
//---------------------------------------------------------------------
void (*COMMnetListen_cb)(COMMnet *net, COMMSock *acceptedSock, void *context);
void (*COMMnetSent_cb)(COMMnet *net, int bytesSent, void *context);
void (*COMMnetRecvd_cb)(COMMnet *net,int bytesRecvd,uint8_t*data,void*context);



//---------------------------------------------------------------------
// public API
//---------------------------------------------------------------------

/*Only fails if there is no memory*/
COMMnet *COMMinitNetwork();
void COMMshutdownNetwork(COMMnet **net);

/*Runs the network for one loop. Call frequently.*/
COMMStatus COMMrunNetwork(COMMnet *net);



//need to connect
COMMSock *COMMnetConnect(COMMnet *net, const char *dest, uint16_t port);


//need to listen
BOOL COMMnetListen(COMMnet *net, uint16_t port, COMMnetAccept_cb *cb,
                        void *context);


//Absolutely need these
void COMMnetSendData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetSent_cb*cb, void *context);

void COMMnetRecvData(COMMnet *net, const uint8_t*data, uint32_t len,
                     int timeoutSeconds, COMMnetRecvd_cb*cb, void*context);









#endif

