#include "network.h"

int COMMSendData(struct DataSender *s) {
	NTP_FD_SET set;
	NTP_FD_ADD(s->sock, &set);
	
	int result = NTPSelect(NULL, &set, 0);
	if(result<0)
		return -1;
	if(result==0)
		return 0;
	
	int lenToSend = s->totalLen - s->bytesSent;
	int bytesSent = NTPSend(s->sock, s->data + s->bytesSent, lenToSend);
	if(bytesSent<0)
		return -1;
	
	s->bytesSent += bytesSent;

	if(s->bytesSent >= s->totalLen)
		return 1;

	return 0;
}

int COMMRecvData(struct DataRecvr *r) {
	NTP_FD_SET set;
	NTP_FD_ADD(r->sock, &set);

	int result = NTPSelect(&set, NULL, 0);
	if(result<0)
		return -1;
	if(result==0)
		return 0;
	
	int lenToRecv = r->totalLen - r->bytesRecvd;
	int bytesRecvd = NTPRecv(r->sock, r->data + r->bytesRecvd, lenToRecv);
	if(bytesRecvd<0)
		return -1;
	
	r->bytesRecvd += bytesRecvd;

	if(r->bytesRecvd >= r->totalLen)
		return 1;
	
	return 0;
}

