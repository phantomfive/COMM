#ifndef COMM_SERVER_H

#include "notrap.h"

typedef struct COMM_server_struct COMM_server;
typedef struct COMM_connection_struct COMM_connection;
typedef enum   COMM_ServerStatus_enum COMM_ServerStatus;


//----------------------------------------------------------------------------
// Callbacks that get called when responses are receieved, or when
// a new connection comes.
//
// We place these first because they are used when starting the library,
// which is the next section.
//---------------------------------------------------------------------------

void (*COMM_ResponseReceived_cb)(COMM_connection*client, 
                                 int code, BOOL result,
                                 const char *response1,
                                 const char *response2,
                                 const char *response3,
                                const char *response4,
                                 const char *errorMessage,
                                 void *context);

void (*COMM_binaryResponseReceived_cb)(COMM_connect*client, 
                                       int code, BOOL result,
                                       const char*response1,
                                       const char*binaryResponse,
                                       int binaryResponseSize,
                                       char *errorMessageOut,
                                       void *context);

BOOL (*COMM_connectionStatusCallback)(COMM_connection *connection, int STATUS);



//---------------------------------------------------------------------------
// Functions for starting, stopping, and running the COMM server
//---------------------------------------------------------------------------

COMM_server *COMM_initServer(COMM_connectionStatusCallback   *css_cb,
                             void *connectionStatusContext,
                             COMM_messageResponseReceived_cb *mrr_cb,
                             COMM_binaryResponseReceived_cb  *bmrr_cb);

int COMM_beginListening(int port);


COMM_ServerStatus COMM_runServerLoop(COMM_server *obj);

enum COMM_ServerStatus_enum {
	COMM_HAPPY
};

int COMM_shutdownServer(COMM_server **obj);

//----------------------------------------------------------------------------
// Functions for sending messages
//----------------------------------------------------------------------------
int COMM_sendMessage(COMM_connection*client, int code,
                     const char *param1,
                     const char *param2,
                     const char *param3,
                     const char *param4, void *context);

int COMM_sendBinaryMessage(COMM_connection*client, int code,
                           const char *param1,
                           const char *binaryPayload,
                           int binarySize,
                           void *context);


#endif

