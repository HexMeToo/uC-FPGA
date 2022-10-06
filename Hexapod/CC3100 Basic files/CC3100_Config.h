#ifndef CC3100_CONFIG_H_
#define CC3100_CONFIG_H_

#define SL_STOP_TIMEOUT     0xFF
#define BUF_SIZE            1450        /* Buffer for received data */
#define PREAMBLE            1           /* Preamble value 0- short, 1- long */
#define SOFT_START_TIME     100
#define KANALECZEK  2

extern _u8 g_Status;

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent);
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse);
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);

static _i32 configureSimpleLinkToDefaultState();
static _i32 initializeAppVariables();
void Start_Device (void);
void Stop_Device (void);

#endif /* VAR_H_ */
