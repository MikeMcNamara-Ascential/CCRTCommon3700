/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** MBSNOTFY.H
**
** COPYRIGHT (c) 2007 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Sends notification events the calling application 
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

#ifndef MBSNOTFY_H
#define MBSNOTFY_H



#define MAX_MBUS_EVENTS				24

typedef struct mbsEvent
{
    INT32  nEvent;				/* Event type */
    INT32  nParam1;				/* Event parameter 1 */    
    INT32  nParam2;				/* Event parameter 1 */    
} mbsEvent;

extern MBS_EventCallbackType* gfnmbsLogEvent;

extern void mbus_notifyInit();
extern void mbus_notifyEvent( INT32 nEvent, INT32 nParam1, INT32 nParam2);
extern void mbus_notifyService();


#endif
