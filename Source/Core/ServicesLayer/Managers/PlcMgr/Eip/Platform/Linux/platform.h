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

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/timeb.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>


/************************ Platform specific defines *************************/

#define INT8            signed char
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

/*
** Socket porting defines
*/
#define SOCKET			UINT32
#define INVALID_SOCKET	(-1)

/*
** Platform specific Mutex and Event typedefs
*/

typedef pthread_mutex_t* PLATFORM_MUTEX_TYPE;


#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET		64

/********************************* Typedefs ***********************************/

/*
** Number of milliseconds between executions of the EADK main task
*/

#define TIMER_INTERVAL		10		/* Main task should be executed once per TIMER_INTERVAL milliseconds */

/*
** Maximum time to wait for the mutex before timing out. Has no use in UNIX
*/

#define MUTEX_TIMEOUT		0


/*
** Public Function prototypes
*/

extern UINT32 platformGetCurrentTickCount(void);
extern void platformSleep(int ticks);
extern PLATFORM_MUTEX_TYPE platformInitMutex(char *strMutexName);
extern void platformDiscardMutex(PLATFORM_MUTEX_TYPE xMutex);

extern void platformInitLogFile(void);
extern void platformCloseLogFile(void);
extern void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);
extern UINT32 platformAddrFromPath( const char* szPath, INT32 nLen );
extern void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );
extern void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );
extern void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

/*
** Macro definitions
*/

#define platformIoctl(sock,cmnd,argp) ioctl(sock, cmnd, argp)
#define platformSetSockOpt(s,l,o,d,dl)		setsockopt(s,l,o,d,dl)
#define platformGetLastError() errno
#define platformGetLastSocketError(socket) platformGetLastError()
#define platformCloseSocket(sock) close(sock)
#define platformSetSockBufSize( p1,p2,p3,p4,p5) 0  
#define platformSocketLibInit()
#define platformSocketLibCleanup()	

#define platformInetNtoa(in)				inet_ntoa(in)

#define platformAccept(s, a, l)				accept(s, a, l)
#define platformBind(s, a, l)				bind(s, a, l)
#define platformConnect(s,a,al)				connect(s,a,al)
#define platformListen(s, i)				listen(s, i)
#define platformRecv(s,b,l,f)				recv(s,b,l,f)
#define platformRecvFrom(s,b,l,f,a,al)		recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)		select(n, i, o, e, t)
#define platformSend(s,b,l,f)				send(s,b,l,f)
#define platformSendTo(s,b,l,f,a,al,p)		sendto(s,b,l,f,a,al)
#define platformSocket(x,y,z)				socket(x,y,z)
#define platformGetInternetAddress(addr)	addr.s_addr

/* No ACD support */
#ifdef EIP_ACD
#define platformIsDuplicateIPAddress(IPaddress)			FALSE
#endif

#define platformReleaseMutex(xMutex) pthread_mutex_unlock( xMutex )
#define platformWaitMutex(xMutex, lWait) pthread_mutex_lock( xMutex )

#endif /* #ifndef PLATFORM_H */

