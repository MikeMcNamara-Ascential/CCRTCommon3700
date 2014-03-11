/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDATATYPES.h
**
** COPYRIGHT (c) 2008 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant datatypes so other libraries don't need to include
** the TCP/IP stack headers
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

#ifndef EIPDATATYPES_H
#define EIPDATATYPES_H

/*********************** Platform Specific Includes ************************/

/******************** Platform Specific Definitions ************************/
#define EIP_BIG_ENDIAN

#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define BOOL			UINT8
#define BYTE            unsigned char
#define SOCKET			UINT32
#define ETAG_DATATYPE	UINT32


#define TRUE			1
#define FALSE			0

#ifndef ET_IP_CALLBACK
#define ET_IP_CALLBACK
#endif

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

#define PLATFORM_MUTEX_TYPE UINT32  /* Native system mutex type */

#define MUTEX_TIMEOUT    0xFFFFFFFF    /* Maximum time to wait for the mutex before timing out */


#endif /* #ifndef PLATFORM_H */
