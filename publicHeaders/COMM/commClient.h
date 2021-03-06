#ifndef COMMCLIENT_H
#define COMMCLIENT_H

#include <notrap/notrap.h>

typedef struct COMM_client COMM_client;
typedef int COMM_CLIENT_STATUS;

#define COMM_CLIENT_STATUS_CONNECTING 1
#define COMM_CLIENT_STATUS_CONNECTED  2


#define COMM_CLIENT_ERR_NO_MEM  -1
#define COMM_CLIENT_ERR_CONNECT -2
#define COMM_CLIENT_ERR_RECV    -3
#define COMM_CLIENT_ERR_SEND    -4

//--------------------------------------------------------------------------
// Callbacks for receiving messages.
// Use these to receive messages, and send a response.
//--------------------------------------------------------------------------

typedef BOOL (*COMM_message_received_cb)(int code, const char *param1, 
                                                   const char *param2,
                                                   const char *param3,
                                                   const char *param4,
                                                   char **response1,
                                                   char **response2,
                                                   char **response3,
                                                   char **response4,
                                                   char errorMessageOut[1000],
                                                   void *context);

typedef BOOL (*COMM_binary_message_received_cb)(int code, const char *param1,
                                                const char *binaryPayload,
                                                const int   binarySize,
                                                char  **response1,
                                                char  **binaryResponse,
                                                int   binaryResponseSize,
                                                char errorMessageOut[1000],
                                                void *context);



//-------------------------------------------------------------------------
// Callbacks for starting, connecting, and disconnecting a COMM client.
//-------------------------------------------------------------------------
COMM_client *COMM_initClient(COMM_message_received_cb *messageReceivedCallback,
                     COMM_binary_message_received_cb *bMessageReceivedCallback,
                     void *context);

void COMM_connect(COMM_client *obj,const char *serverName, uint16_t serverPort);

void COMM_disconnect(COMM_client **obj);

COMM_CLIENT_STATUS COMM_runClientLoop(COMM_client *obj);

#endif

