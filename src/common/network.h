#ifndef COMM_NETWORK_H
#define COMM_NETWORK_H
/***************************************************************************
 * Class for sending and receiving data over a network: non-blocking.
 *
 * Copyright Andrew 2013 Usable under the terms of the GPL 3.0 or greater
 ***************************************************************************/
#include <notrap.h>

struct DataSender {
	int bytesSent;
	uint8_t *data;
	int totalLen;
	NTPSock *sock;
};

int COMMSendData(struct DataSender *sender);


struct DataRecvr {
	int bytesRecvd;
	uint8_t *buf;
	int totalLen;
	NTPSock *sock;
};

int COMMRecvData(struct DataRecvr *recvr);


#endif

