//******************************************************************************
// Description:
//	This class provides creates a thread for measuring operation time for
//  a system that does not have a valid license.  It is design for use only
//  in conjunction with the Launcher
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
//     $Log: /Core/ServicesLayer/Servers/Launcher/LicenseTimer.cpp $
// 
// 1     3/03/05 1:04a Cward
// Initial Version
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "LicenseTimer.h"


LicenseTimer::LicenseTimer(Launcher *laucherComponent) : AtomicThread(),
m_stopThread(false)
{
    m_timeAllowed = 60;
    m_launcher = laucherComponent;
}

LicenseTimer::LicenseTimer(Launcher *laucherComponent, INT32 timeAllowed) :
AtomicThread(),m_stopThread(false)
{
    m_timeAllowed = timeAllowed;
    m_launcher = laucherComponent;
}

LicenseTimer::~LicenseTimer(void)
{
}

void* LicenseTimer::Execute(void)
{   
    do
    {
        // printf("\nTime since boot up: %e\n",BposSecondsSinceBootUp());
        if (BposSecondsSinceBootUp() > m_timeAllowed)
        {
            m_launcher->DisableSystem();
        }
        BposSleep(1000);
    }
    while (KeepRunningThread());

    // Let the instantiating object know the thread is complete
    return NULL;
}

INT32 LicenseTimer::StopThread()
{
    errno = m_dataMutex.Acquire();
    if (errno != EOK)
    {
        m_launcher->Log(LOG_ERRORS,
                        "ERROR - Unable to acquire thread data mutex: %s\n",
                        strerror(errno));
    }
    m_stopThread = true;
    m_dataMutex.Release();
    return(errno);
}

INT32 LicenseTimer::WaitUntilDone(void)
{
    INT32       err;
    err = pthread_join(GetInitialThreadId(),NULL);
    if (err == EOK)
    {
        m_launcher->Log(LOG_DEV_DATA,
                        "Thread done, joined successfully - Thread Number = %d\n",
                        GetInitialThreadId());
    }
    else
    {
        m_launcher->Log(LOG_DEV_DATA,
                        "Thread previously joined - Thread Number = %d\n",
                        GetInitialThreadId());
    }
    return( err);
}

bool LicenseTimer::KeepRunningThread(void)
{
    bool stopThread = false;
    m_dataMutex.Acquire();
    stopThread = m_stopThread;
    m_dataMutex.Release();
    return(!stopThread);
}

