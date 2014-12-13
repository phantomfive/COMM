#ifndef COMM_SOCK_H
#define COMM_SOCK_H

#include "network.h"

typedef struct COMMSock *sock;

void (*COMMnetRecv_cb)(COMMSock *sock, uint8_t *data, uint32_t len,
                       int result, void *context);

void (*COMMnetSent_cb *cb)(COMMSock *sock, uint8_t *data, uint32_t len,
                            int result, void *context);

void COMMSockSend(COMMSock *sock, const uint8_t*data, uint32_t len,
                  int timeoutSeconds, COMMnetSent_cb *cb, void *context);

void COMMnetRecv(COMMSock *sock, const uint8_t*data, uint32_t len,
                 int timeoutSeconds, COMMnetRecv_cb *cb, void *context);





#endif

