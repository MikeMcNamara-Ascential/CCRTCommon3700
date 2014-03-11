/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** PLATFORM.h
**
** COPYRIGHT (c) 2001,2002 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependent declarations and definitions.
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

/*********************** Platform Specific Includes ************************/
#define _POSIX_SOURCE
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include "macros.h"
#include <ngdev.h>
#include <ngos/netsock.h>
#include <ngudp.h>
#include <ngos.h>
#include <ngeth.h>
#include <ngip.h>
#include <ngip/ethernet.h> /* ETHERMTU */
#include <ngping.h>
#include <ngos/errno.h>         /* errno */
#include <ngsocket.h>
#include <ngtcp.h>
#include <ngipcmd.h>

#include <ngresolv.h>
#include <ngnetdb.h>
#include <ngresolv/bsd_glob.h>


/******************** Platform Specific Definitions ************************/
/* This adds support for the Ethernet Link object Attribute 6 */
#define EIP_INTERFACE_CONTROL_SUPPORT

#define EIP_BIG_ENDIAN

#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define BOOL	        UINT8
#define BYTE            unsigned char
#define SOCKET		    int
#define ETAG_DATATYPE   UINT32

#define ET_IP_CALLBACK

#define TRUE			1
#define FALSE			0

#define INVALID_SOCKET		(-1)

/* IS_BYTE macro should return TRUE if x is a single byte and FALSE if its a 16 bit value */
#define IS_BYTE(x)	( (((UINT16)x & 0xFF00)==0) ? TRUE : FALSE )
/* IS_WORD macro should return TRUE if x is a single word (16 bit value) and FALSE if its a 32 bit value */
#define IS_WORD(x)	( (((UINT32)x & 0xFFFF0000)==0) ? TRUE : FALSE )

#if !defined(_T)
#define _T
#endif

/* MIN and MAX macros return the higher or lower of two values */

#define PLATFORM_MIN(x,y) ((x < y) ? x : y)
#define PLATFORM_MAX(x,y) ((x > y) ? x : y)

#define PLATFORM_MUTEX_TYPE int  /* Native system mutex type */

#define EWOULDBLOCK NG_EWOULDBLOCK /* Native socket system "would block" value */

#define MUTEX_TIMEOUT    0xFFFFFFFF    /* Maximum time to wait for the mutex before timing out */

#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET 32

/*************************** Forward Declaration ******************************/

/********************************* Typedefs ***********************************/

/************************ Public Function Prototypes **************************/

extern void platformInitLogFile(void);
extern void platformCloseLogFile(void);
extern void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);

#define platformSocketLibInit()
#define platformSocketLibCleanup()
extern void platformSocketInitTargetData(void);

#ifdef EIP_QOS
extern INT32 platformSetDSCPValue(SOCKET s, UINT8 bDSCPValue);
#endif

extern UINT32 platformGetCurrentTickCount();
extern UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen );
extern INT32 platformSetSockOpt(int s, int level, int optname, const void *optval, int optlen);
extern void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );
extern void platformCopySockAddrInToBuffer(UINT8* pBuffer, struct sockaddr_in* pSockAddr);
extern void platformCopySockAddrInFromBuffer(struct sockaddr_in* pSockAddr, UINT8* pBuffer);
extern UINT16 platformFree (void* ppMem);
extern UINT16* platformAlloc(UINT32 dwLength);

/* ACD Support */
#ifdef EIP_ACD
extern BOOL platformIsDuplicateIPAddress(UINT32 IPAddress);
#endif

/********************************** Macros ************************************/

#define platformSleep(x) /* Sleep for x number of milliseconds */

/* Redefinition of system mutex handling functions */

/* Claim register are not being using for the EtherNet/IP stack within the LEASH architecture
 * because all API calls are on a single context (thread).  If the EtherNet/IP stack is
 * used independent of the LEASH architecture and uses more than one context, the claim
 * register API would map here
 */
UINT32 platformInitMutex(char* strMutexName);
#define platformDiscardMutex(xEvent)
#define platformReleaseMutex(xEvent)
#define platformWaitMutex(xMutex, lWait)

/* Redefinition of normal BSD style socket functions to named macros */
#define platformIoctl(sock,cmnd,argp)           ioctlsocket(sock, cmnd, (int*)argp)
#define platformSetSockBufSize( p1,p2,p3,p4,p5) platformSetSockOpt(p1,p2,p3,p4,p5)
#define platformCloseSocket(sock)               closesocket(sock)
#define platformGetLastSocketError(socket)      platformGetLastError()
#define platformGetLastError()                  ngOSGetErrno()
#define platformAccept(s, a, l)                 accept(s, a, (int*)l)
#define platformBind(s, a, l)                   bind(s, a, l)
#define platformConnect(s,a,al)                 connect(s,a,al)
#define platformListen(s, i)                    listen(s, i)
#define platformRecv(s,b,l,f)                   recv(s,b,l,f)
#define platformRecvFrom(s,b,l,f,a,al)          recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)           selectsocket(n, i, o, e, t)
#define platformSend(s,b,l,f)                   send(s,(const char*)b,l,f)
#define platformSendTo(s,b,l,f,a,al,p)          sendto(s,(const char*)b,l,f,a,al)
#define platformSocket(x,y,z)                   socket(x,y,z)
#define platformGetInternetAddress(addr)        addr.s_addr
#define platformInetNtoa(in)                    inet_ntoa(in)

#define atol(x)									atoi(x)

#endif /* #ifndef PLATFORM_H */
