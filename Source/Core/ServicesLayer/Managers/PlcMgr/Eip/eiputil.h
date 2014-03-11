/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** UTIL.H
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** General utilities.
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

#ifndef UTIL_H
#define UTIL_H

#define IS_TICK_GREATER(tick1, tick2)  ( (tick1 < 0xffff && tick2 > 0xffff0000) ? TRUE : ( (tick2 < 0xffff && tick1 > 0xffff0000) ? FALSE : (tick1 > tick2) ) )
#define IS_TICK_GREATER_OR_EQUAL(tick1, tick2)  ( (tick1 < 0xffff && tick2 > 0xffff0000) ? TRUE : ( (tick2 < 0xffff && tick1 > 0xffff0000) ? FALSE : (tick1 >= tick2) ) )
#define MAX_TICK(tick1, tick2)    ( IS_TICK_GREATER(tick1, tick2) ? tick1 : tick2 )

#define BUFFER_INTERSECT(start1, size1, start2, size2) ( !( ((start1)+(size1)) <= (start2) || (start1) >= ((start2)+(size2)) ) )

#ifdef EIP_STACK_MEMORY

#ifdef EIP_LARGE_MEMORY
#define MEMORY_POOL_SIZE        ((0xffff)*24)
#else
#define MEMORY_POOL_SIZE        (0xffff)
#endif

extern UINT8  gMemoryPool[MEMORY_POOL_SIZE];
extern UINT8* gpMemoryPool;
#endif

extern UINT8* EtIP_malloc( UINT8* data, UINT16 iDataSize );
extern void EtIP_free( UINT8** piData, UINT16* piDataSize );
extern void EtIP_realloc( UINT8** piData, UINT16* piDataSize, UINT8* data, UINT16 iDataSize );
extern BOOL utilParseNetworkPath( char* szNetworkPath, BOOL* pbLocal, char* extendedPath, UINT16* piExtendedPathLen );
extern BOOL utilBuildNetworkPath( char* szNetworkPath, UINT32 lIPAddr, EPATH* pPDU );
extern INT32 utilCheckNetworkPath(const char* networkPath );
extern INT32 utilCheckHostIPAddress( UINT32 lHostIPAddr );
extern UINT32 utilGetUniqueID();
extern BOOL utilIsIDUnique(UINT32 lVal);
extern UINT32 utilCalculatePseudoRandomDelay(UINT32 lMaxWaitTime);



#endif /* #ifndef UTIL_H */
