/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPNOTFY.C
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

#include "mbsinc.h"

#ifdef ET_IP_MODBUS

MBS_EventCallbackType* gfnmbsLogEvent = NULL;        /* Client application callback function */

static mbsEvent gmbusEvents[MAX_MBUS_EVENTS];	/* Events array */
static mbsEvent* gpnmbusEvents;			/* Number of outstanding events */

/*---------------------------------------------------------------------------
** mbus_notifyInit( )
**
** Initialize event array
**---------------------------------------------------------------------------
*/

void mbus_notifyInit()
{    
    gpnmbusEvents = gmbusEvents;    
}

/*---------------------------------------------------------------------------
** notifyEvent( )
**
** Notify the client about an event received
**---------------------------------------------------------------------------
*/

void mbus_notifyEvent( INT32 nEvent, INT32 nParam1, INT32 nParam2 )
{
    mbsEvent* pEvent;

#ifdef TRACE_OUTPUT

    switch( nEvent )
    {
        default:
            break;
    }

#endif /* #ifdef TRACE_OUTPUT */

    if ( gpnmbusEvents < &gmbusEvents[MAX_MBUS_EVENTS])
	{
		pEvent = gpnmbusEvents;
		pEvent->nEvent = nEvent;
		pEvent->nParam1 = nParam1;
		pEvent->nParam2 = nParam2;
		gpnmbusEvents++;
	}
}

/*---------------------------------------------------------------------------
** mbus_notifyService( )
**
** Sends all events from the event array
**---------------------------------------------------------------------------
*/

void mbus_notifyService()
{
    INT32 nEvent, nParam1, nParam2;
    mbsEvent* pEvent;

    if ( gfnmbsLogEvent )
    {
        while( gpnmbusEvents != gmbusEvents)
        {
            platformWaitMutex(ghmbusClientMutex, MUTEX_TIMEOUT);
            
            gpnmbusEvents--;
            nEvent = gmbusEvents[0].nEvent; 
            nParam1 = gmbusEvents[0].nParam1;
            nParam2 = gmbusEvents[0].nParam2;

            /* Shift the events with the higher index to fill in the void */
            for(pEvent = gmbusEvents; pEvent < gpnmbusEvents; pEvent++ )
                memcpy( pEvent, pEvent+1, sizeof(mbsEvent) );    
            
            platformReleaseMutex(ghmbusClientMutex);

            mbusDumpStr3(MBUS_TRACE_LEVEL_NOTICE, MBUS_TRACE_TYPE_MSG, 0, 0, "notifyService Event %d, Param1 %d, Param2 %d", nEvent, nParam1, nParam2);
            gfnmbsLogEvent( nEvent, nParam1, nParam2 );
        }
    }   
}

#endif /* ET_IP_MODBUS */
