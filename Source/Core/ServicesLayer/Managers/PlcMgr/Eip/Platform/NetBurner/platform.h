/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.h
**
** COPYRIGHT (c) 2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant declarations and definitions.
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>----0<major>         Customization: <none>----0<major>
**
*****************************************************************************
*****************************************************************************
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#include "predef.h" 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <ucos.h>
#include <ip.h>
#include <udp.h>
#include <tcp.h>
#include <multicast.h>
#include <errno.h>

//#define TRACE_OUTPUT
//#define TRACE_DEBUGGER_OUTPUT

#define EIP_BIG_ENDIAN

#define EIP_CUSTOM_SOCKET
#define SYNCHRONOUS_CONNECTION

extern OS_CRIT   gCriticalSection;
extern OS_FIFO   gMulticastFifo;
extern int       gnMulticastCount;

#define htons	HTONS
#define ntohs	NTOHS
#define htonl	HTONL
#define ntohl	NTOHL

/********************* Platform specific definitions **************************/

#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define SOCKET			UINT32
#define ETAG_DATATYPE	UINT32

#define ET_IP_CALLBACK

#define in_addr         IN_ADDR

#define IS_BYTE(x)                ( (HIBYTE((UINT16)x)==0) ? TRUE : FALSE )
#define IS_WORD(x)                ( (HIWORD((UINT32)x)==0) ? TRUE : FALSE )

#define PLATFORM_MUTEX_TYPE OS_CRIT*

#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET		64

#if !defined(_T)
#define _T    
#endif

#define PLATFORM_MIN(x,y) ((x < y) ? x : y)
#define PLATFORM_MAX(x,y) ((x > y) ? x : y)

#define MUTEX_TIMEOUT    0    /* Maximum time to wait for the mutex before timing out */

#define INADDR_NONE             0xffffffff

#define AF_INET                 2     
#define IPPROTO_IP              0 
#define IP_MULTICAST_TTL        3
#define IP_MULTICAST_IF         2
#define INADDR_BROADCAST		INADDR_NONE

#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)

#ifndef INVALID_SOCKET	
#define INVALID_SOCKET   (UINT32)(~0)
#endif

typedef struct IN_ADDR {
 union {
 struct{
 unsigned char s_b1,
 s_b2,
 s_b3,
 s_b4;
 } S_un_b;
 struct {
 unsigned short s_w1,
 s_w2;
 } S_un_w;
 unsigned long S_addr;
 } S_un;
} IN_ADDR;

typedef struct sockaddr_in
{
	short sin_family;
	unsigned short sin_port;
	struct IN_ADDR sin_addr;
	char sin_zero[8];
} sockaddr_in;

typedef struct sockaddr
{
	u_short sa_family;
	char    sa_data[14];
} sockaddr;

extern INT32 gSocketError;

/* include this now that the "native types" have been defined */
#include "eipnbsoc.h"


/*************************** Forward Declaration ******************************/

/********************************* Typedefs ***********************************/

/************************ Public Function Prototypes **************************/

extern void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);

extern void platformSocketInitTargetData(void);

extern UINT32 platformGetCurrentTickCount();
extern OS_CRIT* platformInitMutex(char *strMutexName);

extern INT32 platformGetHostName(char* szName, INT32 nNameLen);

extern UINT32 platformAddrFromPath( const char* szPath, INT32 nLen );
extern void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );

extern INT32 platformSend(SOCKET lSockFd, UINT8* buf, INT32 len, INT32 lFlags);
extern INT32 platformSendTo(SOCKET lSockFd, UINT8* buf, INT32 len, INT32 iDummy1, struct sockaddr* pto, INT32 iDummy2, WORD iPort); 
extern INT32 platformRecv(SOCKET lSockFd, const char* buf, INT32 len, INT32 lFlags);
extern void platformCloseSocket(SOCKET lSocket );

extern void TCPConnectTask(void * pd);
extern void UdpReadTask(void * pd);
extern void BroadcastReadTask(void * pd);
extern void MulticastReadTask(void * pd);

/********************************** Macros ************************************/
#define platformSleep(x)    OSTimeDly(x)

#define platformInitLogFile(void) 
#define platformCloseLogFile(void)

#define platformIoctl(sock,cmnd,argp) 
#define platformSetSockBufSize( p1,p2,p3,p4,p5) 
#define platformSetSockOpt(s,l,o,d,dl)   0 
#define platformGetLastSocketError(socket) gSocketError;
#define platformGetLastError()		0

extern INT8* platformInetNtoa( IN_ADDR IPAddress );

#define platformReleaseMutex(xMutex) OSCritLeave( xMutex)
#define platformWaitMutex(xMutex, lWait) OSCritEnter(xMutex, lWait )
#define platformDiscardMutex(xMutex) 

#define platformAccept(s, a, l)                accept(s, a, NULL, 0)
#define platformBind(s, a, l)                bind(s, a, l)
#define platformConnect(s,a,al)                connect(s,a,al)
#define platformListen(s, i)                listen(1, s, i)
#define platformRecvFrom(s,b,l,f,a,al)        recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)        select(n, i, o, e, t)
#define platformSocket(x,y,z)                socket(x,y,z)
#define platformGetInternetAddress(addr)    addr.S_un.S_addr
#define platformCopySockAddrInToBuffer(pBuffer, pSockAddr)	memcpy(pBuffer, pSockAddr, sizeof(struct sockaddr_in))
#define platformCopySockAddrInFromBuffer(pSockAddr, pBuffer)	memcpy(pSockAddr, pBuffer, sizeof(struct sockaddr_in))

/* No ACD support */
#define platformIsDuplicateIPAddress(IPaddress)			FALSE

#define platformSocketLibInit()
#define platformSocketLibCleanup()	

#endif /* #ifndef PLATFORM_H */
