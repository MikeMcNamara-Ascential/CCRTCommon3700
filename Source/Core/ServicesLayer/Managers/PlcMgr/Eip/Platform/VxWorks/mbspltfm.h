/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSPLTFM.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Modbus Platform dependant declarations and definitions.
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

#ifndef MBUS_PLATFORM_H
#define MBUS_PLATFORM_H

/* Include EtherNet/IP platform header for base */
#include "platform.h"

/* Serial port handle type (not currently used) */
#define PLATFORM_PORT_HANDLE		HANDLE

extern void mbus_platformInitLogFile(void);
extern void mbus_platformCloseLogFile(void);
extern void mbus_platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);


#endif /* #ifndef MBUS_PLATFORM_H */
