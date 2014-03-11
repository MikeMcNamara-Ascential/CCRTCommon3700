/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSTCP.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handles Modbus/TCP specifics
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

#ifndef MBSTCP_H
#define MBSTCP_H

#define MODBUS_PORT					502
#define MBUS_SMALL_BUFFER_SIZE		4096      /* Reserve buffer size in bytes for shared TCP use */ 

#define MBUS_ADU_OVERHEAD					3
#define MBUS_PARTIAL_TCP_PACKET_TIMEOUT		1000

#ifndef SOCKET_ERROR	
#define SOCKET_ERROR   (-1)
#endif

extern BOOL mbus_tcpStartConnection( MBUS_SESSION* pSession );
extern BOOL mbus_tcpContinueConnection( MBUS_SESSION* pSession, BOOL* bTCPConnectionSuccessfullyCompleted );

extern BOOL mbus_tcpSendPartial(MBUS_SESSION* pSession);
extern INT32 mbus_tcpRecv(MBUS_SESSION* pSession );


#endif
