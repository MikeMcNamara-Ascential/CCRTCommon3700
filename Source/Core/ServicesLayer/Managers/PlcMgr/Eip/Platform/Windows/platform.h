/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.h
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
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

#ifndef ET_IP_API
#define ET_IP_API extern "C" __declspec(dllexport)
#endif

#ifndef ET_IP_CALLBACK
#define ET_IP_CALLBACK __stdcall
#endif

#define WIN32_LEAN_AND_MEAN    

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/* remove "nonstandard extension used : nameless struct/union" warning */
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#include <process.h>
#include <nb30.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <direct.h>
/***************************************************************************************
* iphlpapi.h was retrieved from a Microsoft SDK not part of the standard 
* Visual Studio installation.  It in included in the EtherNet/IP stack installation
* for convenience.  While this version has been tested for Windows 2000/XP, there 
* may be a more recent version available.
* It is assumed here that iphlpapi.h (and it's associated header/library files)
* have been copied to a directory included in the Visual Studio include path
****************************************************************************************/
#include <iphlpapi.h>
#pragma warning( pop ) 

#include <sys/timeb.h>
#include <time.h>
#include <tchar.h>

/* Datatypes defined for Windows.  Separate header file so that it can be included
   in applications with out all of the other parts of the platform files that
   shouldn't be needed at the "application" layer */
#include "windatatypes.h"
#ifdef EIP_LARGE_CONFIG_OPT
#include "hashtable.h"
#endif


#define IS_BYTE(x)                ( (HIBYTE((UINT16)x)==0) ? TRUE : FALSE )
#define IS_WORD(x)                ( (HIWORD((UINT32)x)==0) ? TRUE : FALSE )

#define PLATFORM_MIN(x,y) min(x,y)
#define PLATFORM_MAX(x,y) max(x,y)

#define PLATFORM_MUTEX_TYPE HANDLE

#define EWOULDBLOCK		WSAEWOULDBLOCK

#define MUTEX_TIMEOUT    INFINITE    /* Maximum time to wait for the mutex before timing out */

#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET		64

#ifdef EIP_LARGE_CONFIG_OPT
typedef struct hashTable* PLATFORM_HASH_ID;
typedef struct hashNode PLATFORM_HASH_NODE;

#define CNXN_HASH_TABLE_SIZE_LOG2 9     /* 2^9 (512) entries */
#endif

/********************************* Typedefs ***********************************/

typedef struct _ASTAT_ {
   ADAPTER_STATUS adapt;
   NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

/************************ Public Function Prototypes **************************/

extern void platformInitLogFile(void);
extern void platformCloseLogFile(void);
extern void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);

extern void platformSocketLibInit(void);
extern void platformSocketLibCleanup();
extern void platformSocketInitTargetData(void);

#ifdef EIP_QOS
extern INT32 platformSetDSCPValue(SOCKET s, UINT8 bDSCPValue);
#endif

extern UINT32 platformGetCurrentTickCount();
extern UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen );
extern void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );
extern void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );
extern void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

/********************************** Macros ************************************/
#define platformInitMutex(strMutexName)			CreateMutex (NULL, TRUE, NULL)
#define platformReleaseMutex(xEvent)			ReleaseMutex(xEvent)
#define platformDiscardMutex(xEvent)			CloseHandle(xEvent)
#define platformWaitMutex(xMutex, lWait)		WaitForSingleObject( xMutex, lWait )

#define platformSleep(x)						Sleep(x)

#define platformIoctl(sock,cmnd,argp)			ioctlsocket(sock, cmnd, (unsigned long*)argp)  
#define platformSetSockBufSize( p1,p2,p3,p4,p5)	setsockopt(p1,p2,p3,p4,p5)
#define platformSetSockOpt(s,l,o,d,dl)			setsockopt(s,l,o,(const char *)d,dl)
#define platformCloseSocket(sock)				closesocket(sock)
#define platformGetLastSocketError(socket)		platformGetLastError()
#define platformGetLastError()					WSAGetLastError() 

#define platformInetNtoa(in)					inet_ntoa(in)
#define platformAccept(s, a, l)					accept(s, a, (int*)l)
#define platformBind(s, a, l)					bind(s, a, l)
#define platformConnect(s,a,al)					connect(s,a,al)
#define platformListen(s, i)					listen(s, i)
#define platformRecv(s,b,l,f)					recv(s,b,l,f)
#define platformRecvFrom(s,b,l,f,a,al)			recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)			select(n, i, o, e, t)
#define platformSend(s,b,l,f)					send(s,(const char*)b,l,f)
#define platformSendTo(s,b,l,f,a,al,p)			sendto(s,(const char*)b,l,f,a,al)
#define platformSocket(x,y,z)					socket(x,y,z)
#define platformGetInternetAddress(addr)		addr.s_addr

/* No ACD support */
#ifdef EIP_ACD
#define platformIsDuplicateIPAddress(IPaddress)			FALSE
#endif

#endif /* #ifndef PLATFORM_H */
