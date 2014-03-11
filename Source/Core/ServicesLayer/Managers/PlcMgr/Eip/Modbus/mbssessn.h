/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSSESSN.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Session collection declaration
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


#ifndef MBS_SESSION_H
#define MBS_SESSION_H

#define MBUS_INVALID_SESSION_INDEX		(-1)
#define MBUS_MAX_SESSIONS				10
#define MBUS_INACTIVITY_TIMEOUT		30000	/* Amount of time without any session activity before it is closed */


typedef enum mbusOpenSessionState
{
	MBUS_SESSION_NON_EXISTENT					= 0x0,		/* Session is not instantiated    */
	MBUS_SESSION_LOGGED							= 0x1,		/* Open Session request has just arrived */
	MBUS_SESSION_WAITING_FOR_TCP_CONNECTION		= 0x2,		/* Waiting for the TCP connection to be established to the target */
	MBUS_SESSION_ESTABLISHED					= 0x3		/* Session is opened     */
}
embus_OpenSessionState;

typedef struct mbus_MbapHeader
{
	UINT16	TcpTransId;
	UINT16	PrtocolId;
	UINT16	Length;
	UINT8	UnitId;

} mbus_MbapHeader;

#define MBUS_HEADER_SIZE		7

typedef struct  mbus_Session       /* Session object data */
{
    INT32	nSessionId;		 /* Unique session identifier */
	SOCKET	lSocket;            /* TCP socket identifier */    
    UINT32	lState;            /* One of the values of the OpenSessionState enumeration */
    UINT32	lWaitingForOpenSessionTimeout;  /* When the session should time out if there is no response */
    UINT32	lHostIPAddr;        /* Host IPAddress in UINT32 format for the opened session */
    UINT32	lClientIPAddr;        /* Network peer IP address TCP session is opened with */
	BOOL	bIO;							/* Session used for I/O data */
	BOOL	bOnlySingleSessionSupported;	/* Remember if device only supports a single TCP connection (I/O and expicit data share session) */

    UINT8*	pPartialRecvPacket;                /* Temporary store any partial packets we received for this session */
    UINT16	iPartialRecvPacketSize;            /* Size of the partial recv packet */
    UINT32	lPartialRecvPacketTimeoutTick;     /* When the partial recv packet should be discarded as old */

    UINT8* pPartialSendPacket;                /* Temporary store any partial packets we need to send for this session */
    UINT16 iPartialSendPacketSize;            /* Size of the partial send packet */

        
} MBUS_SESSION;

extern void mbus_sessionInit(void);
extern MBUS_SESSION* mbus_sessionNew( UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming, BOOL bIO);
extern void mbus_sessionRemove( MBUS_SESSION* pSessionRemove, BOOL bRelogOutgoingRequests );
extern void mbus_sessionRemoveAll(void);
extern MBUS_SESSION* mbus_sessionFindOutgoing(UINT32 lIPAddress, UINT32 lHostIPAddr, UINT8 bIOStatus);
extern void  mbus_sessionService( MBUS_SESSION* pSession );
extern MBUS_SESSION* mbus_sessionGetBySessionId( INT32 nId );
extern BOOL mbus_sessionIsIdle(INT32 nSessionId, INT32 nRequestIdExclude );
extern void mbus_sessionResetTimeout( MBUS_SESSION* pSession );

extern MBUS_SESSION* gpnmbsSessions;						/* Number of the open sessions */
extern MBUS_SESSION  gmbsSessions[MBUS_MAX_SESSIONS];	/* Modbus TCP Session array */
extern INT32    gnmbusSessionIndex;							/* Id to be assigned to the next opened session */

#endif
