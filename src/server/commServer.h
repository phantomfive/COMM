#ifndef COMM_SERVER_H
#define COMM_SERVER_H

typedef struct COMM_server_struct COMM_server;

messageReceivedCallback();

int COMM_sendMessage();

COMM_server *COMM_initializeServer( newConnectionCallback );

int COMM_beginListening(int port);

int COMM_runServerLoop(COMM_server *obj);

int COMM_shutdownServer(COMM_server **obj);

#endif
