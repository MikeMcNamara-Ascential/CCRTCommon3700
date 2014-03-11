/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.h
**
** COPYRIGHT (c) 2003 Pyramid Solutions, Inc.
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

#ifndef EXTERN
    #ifdef __cplusplus         
        #define EXTERN extern "C"
    #else         
        #define EXTERN extern
    #endif
#endif

#include <vxworks.h>
#include <taskLib.h>
#include <semLib.h>
#include <inetlib.h>
#include <ioLib.h>
#include <tickLib.h>
#include <sysLib.h>
#include <errnoLib.h>
#include <sigLib.h>
#include <time.h>
#include <netinet\tcp.h>
#include <stdio.h>
#include <sockLib.h>
#include <selectLib.h>
#include <netdb.h>



/************************ Platform specific defines *************************/

#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define BOOL			unsigned char
#define ETAG_DATATYPE	UINT32


#ifndef SO_DONTLINGER
#define SO_DONTLINGER   (int)(~SO_LINGER)
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define IS_BYTE(x)	( (((UINT16)x & 0xFF00)==0) ? TRUE : FALSE )
#define IS_WORD(x)	( (((UINT32)x & 0xFFFF0000)==0) ? TRUE : FALSE )

#define PLATFORM_MIN(x,y) ((x < y) ? x : y)
#define PLATFORM_MAX(x,y) ((x > y) ? x : y)

#if !defined(_T)
#define _T
#endif

#define ET_IP_CALLBACK

#if _BYTE_ORDER == _BIG_ENDIAN
#define EIP_BIG_ENDIAN
#endif

/*
** Socket porting defines
*/
#define SOCKET			UINT32
#define INVALID_SOCKET	ERROR

/*
** Platform specific Mutex and Event typedefs
*/

typedef SEM_ID PLATFORM_MUTEX_TYPE; /* Native system mutex type */


#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET		64

/*
** Maximum time to wait for the mutex before timing out. Has no use in UNIX
*/

#define MUTEX_TIMEOUT    WAIT_FOREVER    /* Maximum time to wait for the mutex before timing out */

/*
** Public Function prototypes
*/
EXTERN void platformSocketInitTargetData(void);

/* Logging is done through printf, so there isn't a need for "file initialization" */
#define platformInitLogFile
#define platformCloseLogFile
EXTERN void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);

EXTERN UINT32 platformAddrFromPath( const char* szPath, INT32 nLen );

EXTERN void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress);
EXTERN void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );
EXTERN void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

#ifdef EIP_QOS
EXTERN INT32 platformSetDSCPValue( SOCKET s, UINT8 bDSCPValue );
#endif

/*
** Macro definitions
*/

#define platformSleep(x)    					taskDelay(((x*sysClkRateGet())+999)/1000)

#define platformGetCurrentTickCount() 			tickGet()

#define platformInitMutex(strMutexName)			semMCreate(SEM_Q_FIFO)
#define platformDiscardMutex(xMutex)    		semDelete(xMutex)        
#define platformReleaseMutex(xEvent)            semGive(xEvent)
#define platformWaitMutex(xMutex, lWait)        semTake(xMutex,lWait)

#define platformGetLastError() 					errnoGet()

#define platformIoctl(sock,cmnd,argp) 			ioctl(sock, cmnd, argp)
#define platformSetSockOpt(s,l,o,d,dl)			setsockopt(s,l,o,(char *)d,dl)
#define platformGetLastSocketError(socket)		platformGetLastError()
#define platformCloseSocket(sock) 				close(sock)
#define platformSetSockBufSize(p1,p2,p3,p4,p5)	setsockopt(p1,p2,p3,p4,p5)
#define platformSocketLibInit()
#define platformSocketLibCleanup()	

#define platformAccept(s, a, l)					accept(s, a, (int *)l)
#define platformBind(s, a, l)					bind(s, a, l)
#define platformConnect(s,a,al)					connect(s,a,al)
#define platformListen(s, i)					listen(s, i)
#define platformRecv(s,b,l,f)					recv(s,b,l,f)
#define platformRecvFrom(s,b,l,f,a,al)			recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)			select(n, i, o, e, t)
#define platformSend(s,b,l,f)					send(s,(char *)b,l,f)
#define platformSendTo(s,b,l,f,a,al,p)			sendto(s,(char *)b,l,f,a,al)
#define platformSocket(x,y,z)					socket(x,y,z)

#define platformGetInternetAddress(addr)		addr.s_addr

#define platformInetNtoa(in)					inet_ntoa(in)

#ifdef EIP_ACD
#define platformIsDuplicateIPAddress(IPaddress)	FALSE	/* No ACD support */
#endif


#endif /* #ifndef PLATFORM_H */

