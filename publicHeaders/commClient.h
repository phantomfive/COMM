#ifndef COMMCLIENT_H
#define COMMCLIENT_H

#include "commErrors.h"

typedef struct COMM_struct COMM_client;
typedef int COMM_CLIENT_STATUS;

//--------------------------------------------------------------------------
// Callbacks for receiving messages.
// Use these to receive messages, and send a response.
//--------------------------------------------------------------------------

BOOL (*COMM_message_received_cb)(int code, const char *param1, 
                                           const char *param2,
                                           const char *param3,
                                           const char *param4,
                                           char **response1,
                                           char **response2,
                                           char **response3,
                                           char **response4,
                                           char errorMessageOut[1000]
                                           void *context);

BOOL (*COMM_binary_message_received_cb)(int code, const char *param1,
                                                  const char *binaryPayload,
                                                  const int   binarySize,
                                                  char  **response1,
                                                  char  **binaryResponse,
                                                  int   binaryResponseSize,
                                                  char errorMessageOut[1000]
                                                  void *context);



//-------------------------------------------------------------------------
// Callbacks for starting, connecting, and disconnecting a COMM client.
//-------------------------------------------------------------------------
COMM_client *COMM_initClient(COMM_message_received_cb *messageReceivedCallback,
                     COMM_binary_message_received_cb *bMessageReceivedCallback,
                     void *context);

void COMM_connect(const char *serverName, uint16_t serverPort);

void COMM_disconnect(COMM_client **obj);

COMM_CLIENT_STATUS COMM_runClientLoop(COMM_client *obj);

#endif

