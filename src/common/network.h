#ifndef COMM_NETWORK_H
#define COMM_NETWORK_H
/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/
#include <notrap/notrap.h>

struct DataSender {
	int bytesSent;
	uint8_t *data;
	int  totalLen;
	NTPSock *sock;
};

//Returns 1 when all is sent,
//        0 when more needs to be sent,
//        negative on error
int COMMSendData(struct DataSender *sender);


struct DataRecvr {
	int bytesRecvd;
	uint8_t *data;
	int  totalLen;
	NTPSock *sock;
};

//Returns 1 when all is read,
//        0 when more needs to be read,
//        negative on error
int COMMRecvData(struct DataRecvr *recvr);


#endif

