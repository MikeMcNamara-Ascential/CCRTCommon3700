/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSUTIL.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
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

#define MBUS_MEMORY_POOL_SIZE        5000

#define IS_TICK_GREATER(tick1, tick2)  ( (tick1 < 0xffff && tick2 > 0xffff0000) ? TRUE : ( (tick2 < 0xffff && tick1 > 0xffff0000) ? FALSE : (tick1 > tick2) ) )
#define IS_TICK_GREATER_OR_EQUAL(tick1, tick2)  ( (tick1 < 0xffff && tick2 > 0xffff0000) ? TRUE : ( (tick2 < 0xffff && tick1 > 0xffff0000) ? FALSE : (tick1 >= tick2) ) )

#ifdef MBUS_STACK_MEMORY
extern UINT8  gmbusMemoryPool[MBUS_MEMORY_POOL_SIZE];
extern UINT8* gpmbusMemoryPool;
#endif

extern UINT8* mbus_malloc( UINT8* data, UINT16 iDataSize );
extern void mbus_free( UINT8** piData, UINT16* piDataSize );
extern void mbus_realloc( UINT8** piData, UINT16* piDataSize, UINT8* data, UINT16 iDataSize );

