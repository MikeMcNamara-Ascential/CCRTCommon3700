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

/*********************** Platform Specific Includes ************************/

/******************** Platform Specific Definitions ************************/

#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int

/* Can be cast to UINT8, UINT16, UINT32 or UINT8* depending on the type, so make sure
   pointer size is appropriate for platform */
#define ETAG_DATATYPE	UINT32


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

#define PLATFORM_MUTEX_TYPE HANDLE  /* Native system mutex type */

#define EWOULDBLOCK WSAEWOULDBLOCK /* Native socket system "would block" value */

#define MUTEX_TIMEOUT    INFINITE    /* Maximum time to wait for the mutex before timing out */

#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET		64		/* Number of supported connections per socket */

/*************************** Forward Declaration ******************************/

/********************************* Typedefs ***********************************/

/************************ Public Function Prototypes **************************/

extern void platformInitLogFile(void);
extern void platformCloseLogFile(void);
extern void platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);
extern void platformSocketLibInit(void);
extern void platformSocketLibCleanup();
extern void platformSocketInitTargetData(void);
extern UINT32 platformGetCurrentTickCount();
extern UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen );
extern void platformSetSockaddrIn(struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );
extern void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );
extern void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

#ifdef EIP_QOS
extern INT32 platformSetDSCPValue(SOCKET s, UINT8 bDSCPValue);
#endif

#ifdef EIP_ACD
extern BOOL platformIsDuplicateIPAddress(UINT32 lIPAddress);
#endif

/********************************** Macros ************************************/

#define platformSleep(x)    Sleep(x)    /* Sleep for x number of milliseconds */

/* Redefinition of system mutex handling functions */

#define platformInitMutex(strMutexName)         CreateMutex (NULL, TRUE, strMutexName)
#define platformDiscardMutex(xEvent)            CloseHandle(xEvent)
#define platformReleaseMutex(xEvent)            ReleaseMutex(xEvent)
#define platformWaitMutex(xMutex, lWait)        WaitForSingleObject( xMutex, lWait )

/* Redefinition of normal BSD style socket functions to named macros */

#define platformIoctl(sock,cmnd,argp)           ioctlsocket(sock, cmnd, (unsigned long*)argp)  
#define platformSetSockBufSize( p1,p2,p3,p4,p5) setsockopt(p1,p2,p3,p4,p5)
#define platformSetSockOpt(s,l,o,d,dl)          setsockopt(s,l,o,d,dl)
#define platformCloseSocket(sock)               closesocket(sock)
#define platformGetLastSocketError(socket)      platformGetLastError()
#define platformGetLastError()                  platformGetLastError()
#define platformAccept(s, a, l)                 accept(s, a, (int*)l)
#define platformBind(s, a, l)                   bind(s, a, l)
#define platformConnect(s,a,al)                 connect(s,a,al)
#define platformListen(s, i)                    listen(s, i)
#define platformRecv(s,b,l,f)                   recv(s,b,l,f)
#define platformRecvFrom(s,b,l,f,a,al)          recvfrom(s,b,l,f,a,al)
#define platformSelect(n, i, o, e, t)           select(n, i, o, e, t)
#define platformSend(s,b,l,f)                   send(s,(const char*)b,l,f)
#define platformSendTo(s,b,l,f,a,al,p)            sendto(s,(const char*)b,l,f,a,al)
#define platformSocket(x,y,z)                   socket(x,y,z)
#define platformGetInternetAddress(addr)        addr.s_addr
#define platformInetNtoa(in)                    inet_ntoa(in)


#endif /* #ifndef PLATFORM_H */
