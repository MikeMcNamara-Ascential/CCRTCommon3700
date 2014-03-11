/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** SOCKET.H
**
** COPYRIGHT (c) 2001,2002 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Using Winsock to read and write TCP and UDP packets 
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


#ifndef NETBURNER_SOCKET_H
#define NETBURNER_SOCKET_H

#define CONNECT_TIMEOUT		100					  /* Number of ticks to wait for the connection established event */

#define   socketGetBroadcasts()
#define   socketClass1Recv( lClass1Socket, lHostIp )	0
#define   socketClass1Init( bBroadcast, lBufferSize, lHostIPAddress, bConsumeMcast)	0

extern void	socketCheckIncomingSession();
extern void socketPostListen(SOCKET* pWaitSocket, UINT32 lClaimedHostIPAddr);


#ifdef ET_IP_SCANNER
/* forward declaration */
struct tagConnection;

extern INT32 socketJoinMulticastGroup(struct tagConnection* pConnection);
extern INT32 socketDropMulticastGroup(struct tagConnection* pConnection);
#endif

#endif  /* NETBURNER_SOCKET_H */
