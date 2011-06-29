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
//    $Log: /Core/ServicesLayer/Servers/Launcher/LicenseTimer.h $
// 
// 1     3/03/05 1:04a Cward
// Initial Version
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef LicenseTimer_h
#define LicenseTimer_h
//==============================================================================
#include "AtomicThread.h"
#include "BepSync.h"
#include "Bepos.h"
#include "Launcher.h"

// Forward declaration of the Launcher class for the compiler
class Launcher;

/**
 * LicenseTimer.
 * This class monitors time since last system boot and
 * notifies Launcher when time exceeds the valid time
 *
 * @author Greg Pattison
 * @see AtomicThread
 */
class LicenseTimer : public AtomicThread

{
public:
    /**
     * Class Constructor
     * 
     * @param laucherComponent
     *               Pointer to the Launcher object
     */
    LicenseTimer(Launcher *laucherComponent);
    /**
     * Class Constructor
     * 
     * @param laucherComponent
     *               Pointer to the Launcher Object
     * @param timeAllowed
     *               Time(in seconds) that is compared against the last system
     *               boot time
     */
    LicenseTimer(Launcher *laucherComponent, INT32 timeAllowed);
    /**
     * Class destructor
     */
    virtual ~LicenseTimer(void);
    /**
     * Function to be executed when thread is run
     *
     * @return Status
     */
    virtual void* Execute(void);
    /**
     * Signal thread to stop monitoring data.
     *
     * @return EOK if successfully told thread to stop.
     */
    INT32 StopThread(void);
    /**
     * Waits until the thread signals that it is done with its current
     * loop and is safe to be deleted
     */
    virtual INT32 WaitUntilDone(void);

private:
    /**
     * This method is used to check if the thread should
     * still run.  It returns the boolean value of the member
     * variable that is used to control the main while-loop
     * in the thread execution method.
     * 
     * @return the boolean value of the member variable assigned to determine if
     *         the thread should continue to run.
     */
    bool KeepRunningThread(void);
    /**
     * Mutex used to ensure data within class is not accessed by
     * more than one thread at a time
     */
    BepMutex m_dataMutex;
    /**
     * Boolean used to end main while-loop in thread execution method.
     */
    bool m_stopThread;
    /**
     * Used to hold the pointer to the Launcher 
     * Object that instantiated the class
     */
    Launcher *m_launcher;
    /**
     * Time (in seconds) that is compared against the time
     * since the system was last booted.
     */
    INT32 m_timeAllowed;
};
#endif
