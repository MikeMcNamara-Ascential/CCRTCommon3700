//******************************************************************************
// Description:
//  This class provides the common functionality needed by all background
//  data monitors.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/BackgroundMonitor.cpp $
// 
// 7     10/25/06 2:54p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added an inter-message gap variable to the constructor
//     - Added a member variable to tell if the thread was joined
//     - Added a member variable to tell if the printf's should be called
// 
// 6     5/08/06 6:40p Cward
// Removed informational printf statements
//
// 5     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     1/05/05 3:45p Bmeinke
// Fixed a bug in StopMonitor() so that the m_sendMessage cond-var only
// gets Release()'d if it was successfully locked
//
// 4     10/14/04 6:38p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     9/29/04 10:00p Cward
// Modified print statmment in Wait til done to be more palatable to the
// customer
//
// 2     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 1     6/09/04 11:48p Cward
// Refactored App layer includes to remove implementation
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "BackgroundMonitor.h"


BackgroundMonitor::BackgroundMonitor(IProtocolFilter *vehicleComm, 
                                     const INT32 interMessageGap) :
    AtomicThread(), m_lastSendTime(0), m_threadJoined(false),
    m_interMessageGap(interMessageGap),m_enablePrintfs(true)
{   // Save the pointer to the vehicle communication object
    m_vehicleComm = vehicleComm;
}

BackgroundMonitor::~BackgroundMonitor(void)
{   // Instruct monitor to stop if we have not already
    StopMonitor();
    WaitUntilDone();
}

void* BackgroundMonitor::Execute(void)
{   // Keep sending the message until told to stop
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Setup to start sending messages
    m_sendMessage.Acquire();
    m_sendMessage.Signal( true);
    m_sendMessage.Release();

    m_threadJoined = false;

    // Initialize the "time we sent the last message" to now
    ClockTime( CLOCK_REALTIME, NULL, &m_lastSendTime);

    // Start sending messages
    while (SendMessage())
    {   // ASSUMPTION:  Module set up just completed, so we do not have to send right away
        // Do not wait for the entire time in the event the Test Component terminates we catch it

        // Initialize the "time we sent the last message" to now
        ClockTime( CLOCK_REALTIME, NULL, &m_lastSendTime);

        status = MonitorData();
        // Determine the status of the send
        if (BEP_STATUS_SUCCESS != status)
        {   // Message was not sent properly
            if(m_enablePrintfs == true)
            {
                printf("Error monitoring module - status: %s\n", ConvertStatusToResponse(status).c_str());
            }
        }
    }
    // Let the instantiating object know the thread is complete
    return NULL;
}

INT32 BackgroundMonitor::StopMonitor()
{
    INT32       err;
    if ( (err=m_sendMessage.Acquire()) == EOK)
    {   // Tell thread to stop
        m_sendMessage.Signal( false);
        m_sendMessage.Release();
    }
    else
    {
        if(m_enablePrintfs == true)
        {
            printf( "\n\t*********************************************************************************************\n");
            printf( "\tError locking m_sendMessage in BackgroundMonitor::StopMonitor(): %s\n", strerror( err));
            printf( "\n\t*********************************************************************************************\n\n");
        }
    }
    return( err);
}

INT32 BackgroundMonitor::WaitUntilDone(void)
{
    INT32       err;
    
    if(false == m_threadJoined)
    {
        if ((err = pthread_join(GetInitialThreadId(),NULL)) != EOK)
        {
            if(m_enablePrintfs == true)
            {
                printf("Thread previously joined - Thread Number = %d: %s\n", GetInitialThreadId(), strerror(err));
            }
        }
        else
        {
            m_threadJoined = true;
            err = EOK;
        }
    }
    return( err);
}

const bool BackgroundMonitor::SendMessage(void)
{
    int         loopCount = 0;
    bool        retVal = m_sendMessage.GetValue();
    bool            timeToSend = false;
    const uint64_t  gapTime = mSEC_nSEC( GetInterMessageGap());
    uint64_t        currTime;
    uint64_t        timeBeforeSend;
    uint64_t    timeOut = mSEC_nSEC(100LL);;// = mSEC_nSEC( GetInterMessageGap() / 10);
    int err;

    // Sample the current time
    ClockTime( CLOCK_REALTIME, NULL, &currTime);
    // Calculate how long ago we sent the last message
    timeBeforeSend = currTime - m_lastSendTime;
    // If we haven't sent a message within the last "gap time"
    if( timeBeforeSend > gapTime)
    {   // Return immediately
        timeToSend = true;
    }
    else
    {   // Calculate how long we need to wait before sending the next message
        timeBeforeSend = gapTime - timeBeforeSend;
    }

    // If less than 100ms before we need to send again
    if( timeBeforeSend < timeOut)
    {   // Just delay for the amount of time before we need to send the next message
        timeOut = timeBeforeSend;
    }

    // Loop until told to stop or until interval expires
    while ( (retVal == true) && (timeToSend == false))// && (loopCount < 100))
    {   // Timeout after 100ms so we can check if we need to stop
        // sending our "monitor" message more frequently
//      TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &timeOut, NULL);
        delay(nSEC_mSEC(timeOut));
        // Always check to see if someone wants us to stop sending our "monitor" message
        if ( (err=m_sendMessage.Acquire()) == EOK)
        {   // Check if told to stop
            retVal = m_sendMessage.GetValue();
            m_sendMessage.Release();
        }
        else
        {
            if(m_enablePrintfs == true)
            {
                printf( "\n\t*********************************************************************************************\n");
                printf( "\tError locking m_sendMessage in BackgroundMonitor::SendMessage(): %s\n", strerror( err));
                printf( "\n\t*********************************************************************************************\n\n");
            }
        }

        // Just a little sanity check
        if( loopCount >= (int)10)
        {
            timeToSend = true;
        }
        loopCount++;

        // If less than 100ms before we need to send again
        if( timeBeforeSend < timeOut)
        {   // Just delay for the amount of time before we need to send the next message
            timeOut = timeBeforeSend;
            timeBeforeSend = 0;
            delay(nSEC_mSEC(timeOut));
            timeToSend = true;
        }
        else
        {
            timeBeforeSend -= timeOut;
        }
    }
    return( retVal);
}

inline const INT32& BackgroundMonitor::GetInterMessageGap(void)
{
    return m_interMessageGap;
}

void BackgroundMonitor::SetInterMessageGap( INT32 mssgGap)
{
    m_interMessageGap = mssgGap;
}

