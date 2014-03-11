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

#define PLATFORM_PORT_HANDLE		HANDLE

extern void mbus_platformInitLogFile(void);
extern void mbus_platformCloseLogFile(void);
extern void mbus_platformWriteLog(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf, BOOL bTimeStamp);

#ifdef ET_IP_MODBUS_SERIAL
extern PLATFORM_PORT_HANDLE platformInitSerialPort(UINT32 lPort);
extern void platformSerialLibInit(void);
extern void	platformSerialInitTargetData();
extern UINT32 platformSerialSendMessage(PLATFORM_PORT_HANDLE hPort, UINT8* Message, UINT16 Size);
extern UINT8 platformSerialPeekMessage(PLATFORM_PORT_HANDLE hPort);
extern INT32 platformSerialGetMessage(PLATFORM_PORT_HANDLE hPort, UINT8* Message);


#define NUM_MSGS		8
#define MSG_BUF_SIZE	512
#define MAX_CHAR_DELAY	25	// 3.5 chars at 9600 baud = 3.6 msecs 

typedef struct COMPortData
{
	PLATFORM_PORT_HANDLE hCOMPort;
	UINT8 InData[NUM_MSGS][MSG_BUF_SIZE];
	UINT8 OutMsgIndex;

} COMPortData;

#endif /* ET_IP_MODBUS_SERIAL */

#endif /* #ifndef MBUS_PLATFORM_H */
