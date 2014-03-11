/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** WINDATATYPES.h
**
** COPYRIGHT (c) 2001-2009 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Windows Platform dependant datatype definitions for external applications.
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

#ifndef WINDATATYPES_H
#define WINDATATYPES_H

#define INT8            char  
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define UINT64			unsigned __int64
#ifdef EIP_64BIT
#define ETAG_DATATYPE	UINT64
	#ifndef SOCKET
		#define SOCKET	UINT64
	#endif
#else
#define ETAG_DATATYPE	UINT32
	#ifndef SOCKET
		#define SOCKET	UINT32
	#endif
#endif
#ifndef BOOL
#define	BOOL			INT32
#endif /* BOOL */


#endif /* #ifndef WINDATATYPES_H */
