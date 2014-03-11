/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSRQST.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Request array used for scheduling Modbus activity
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************
*****************************************************************************
*/

#ifndef MBSRQST_H
#define MBSRQST_H

#define	MAX_MBUS_REQUESTS		100
#define MBUS_INDEX_END			0x7fffffff            /* Should wrap at this point */
#define MBUS_REQUEST_INDEX_BASE			0x200		/* Request IDs will start with this number. Used to separate request IDs from the returned errors */
#define MBUS_INVALID_REQUEST_INDEX		(-1)
#define MBUS_TRANSACTION_ID_BASE		1

#define	MBUS_EXCEPTION_RESPONSE_MASK	0x80
#define MBUS_MODBUS_PROTOCOL	0

typedef enum mbus_RequestState
{
	MBUS_REQUEST_LOGGED					= 1,	/* Request has just arrived */
	MBUS_REQUEST_WAITING_FOR_SESSION		= 2,	/* Waiting for the the TCP connection to be established to the target */    
	MBUS_REQUEST_WAITING_FOR_RESPONSE	= 3,	/* Waiting for the response    */
	MBUS_REQUEST_RESPONSE_RECEIVED		= 4		/* Response received    */
} mbus_RequestState;

/* Request configuration data */
typedef struct mbus_RequestCfg
{
	UINT8  bFunction;				/* Function code */
	UINT8* pData;					/* Request and response data */
	UINT16 iDataSize;				/* Data size in bytes */
} MBUS_REQUEST_CFG;

/* Structure used to pass around information when parsing the request */
typedef struct mbus_Request
{
    INT32  nIndex;						/* Unique request identifier */
	UINT32 lRequestTimeoutTick;			/* When the request should time out on the lack of response */
	INT32  nState;						/* From the RequestState enumeration */
	BOOL   bIncoming;					/* Request can be originated by another device (incoming) or by the client application (outgoing) */
	BOOL   bDeleteOnSend;				/* Whether the request should be immediately deleted on send */
	UINT32 lIPAddress;					/* IP address of the target device for the outgoing request */
	UINT32 lHostIPAddr;					/* Host IPAddress in UINT32 format for the opened session */
	UINT8 bUnitID;						/* Modbus ID */
	BOOL  bIO;							/* Determines if this is an I/O data request */

	MBUS_REQUEST_CFG cfg;

	UINT8  bGeneralError;				/* General request status. 0 indicates a success */

	INT32  nSessionId;					/* Unique session identifier */

	UINT16 TcpTransID;					/* Transaction ID uses for TCP */
	PLATFORM_PORT_HANDLE hPort;			/* Handle to COM Port */

} MBUS_REQUEST;

extern MBUS_REQUEST gmbusRequests[MAX_MBUS_REQUESTS];        /* Requests array */
extern INT32   gnmbusIndex;                        /* Index to be assigned to the next request */
extern MBUS_REQUEST* gpnmbusRequests;

extern UINT8 gmbusMessageBuffer[MBUS_MAX_PACKET_SIZE];   /* Buffer storing incoming and outgoing packets */

extern void mbus_requestInit(void);
extern void mbus_requestService(MBUS_REQUEST* pRequest);
extern void mbus_requestRemove(MBUS_REQUEST* pRequest);
extern void mbus_requestRemoveAll(void);
extern INT32 mbus_requestNewMessage(MBSRequest* pRequest, BOOL bIO);
extern void mbus_requestLogTCPResponse(MBUS_REQUEST* pRequest);
extern void mbus_requestLogSerialResponse(MBUS_REQUEST* pRequest, UINT16 iMsgSize);
extern MBUS_REQUEST* mbus_requestGetByRequestId( INT32 nRequestId );

#endif
