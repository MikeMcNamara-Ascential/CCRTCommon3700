/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPMBSCM.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Connection Manager proxy for Modbus devices. 
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

#ifndef EIPMBSCM_H
#define EIPMBSCM_H

extern void eipmbsconnmgrParseRequest( REQUEST* pRequest );
extern BOOL eipmbsParseCIPModbusFORequest(SESSION* pSession, REQUEST* pRequest, 
										  CONNECTION* pConnection, FWD_OPEN* pFwdOpen, BOOL bLargeFwdOpen);

extern void eipmbsconnmgrTranslateReadRsp(CONNECTION* pConnection, MBSResponse* pResponse);
extern void eipmbsconnmgrTranslateWriteRsp(CONNECTION* pConnection, MBSResponse* pResponse);
extern void eipmbsParseIOPacket(CONNECTION* pConnection, UINT8 *pData, UINT16 iLen);
extern BOOL eipmbsIsDataConnPoint( UINT32 iInstance );
extern void eipmbsioSendReadRequest(CONNECTION* pConnection, CONNECTION* pMulticastProducer);

#define MODBUS_TCP_TO_SL_PORT_NUMBER	1

#endif
