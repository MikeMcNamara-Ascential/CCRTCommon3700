//******************************************************************************
// Description:
//  This class provides the common functionality needed by all background
//  test components.
//
//==============================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Include/BackgroundTestComponent.h $
// 
// 4     10/30/07 1:49p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 3     9/01/04 9:11p Cward
// Added Wait Unitl Done function
//
// 2     6/09/04 11:09p Cward
// Refactored App layer include to remove implementation. Updated header
// comment block to new standard
//
// 1     1/27/04 4:51p Cward
//
// 1     12/16/03 1:17p Derickso
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BackgroundTestComponent_h
#define BackgroundTestComponent_h
//==============================================================================

#include "AtomicThread.h"

/**
 * BackgroundTestComponent.
 * This class sets up the thread to allow the component
 * to run in the background
 *
 * @author Greg Pattison
 * @see AtomicThread
 * @since December 1, 2003
 */
class BackgroundTestComponent : public AtomicThread
{
public:
    /**
     * Class Constructor
     */
    BackgroundTestComponent();
    
    /**
     * Class destructor
     */
    virtual ~BackgroundTestComponent(void);

    /**
     * Funciton to be executed when thread is run.
     * <p>
     * <b>Description:</b><br>
     * The RunBackgroundComponent function will be called to perform the work required by the thread.  When the work is
     * completed, the thread will wait for approxiamtely 250ms in order for the object that created this thread to see
     * the terminate status.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RunBackgroundComponent() </li>
     *      </ul>
     * 
     * @return Status
     */
    virtual void* Execute(void);
    /**
     * Wait until it safe to delete the thread.
     * <p>
     * <b>Description:</b><br>
     * This function will wait until it is safe to delete the thread.  All work should be completed by the thread.
     * <p>
     * <b>Fcuntions Called:</b>
     *      <ul>
     *      <li> GetInitialThreadId() </li>
     *      </ul>
     * 
     * @return EOK if the thread was destroyed properly.
     */
    virtual INT32 WaitUntilDone(void);

protected:
    /**
     * Run the testing required by the thread.
     * <p>
     * <b>Description:</b><br>
     * All derived classes must implement this function.  This is the function that will accomplish the testing 
     * requirements.  RunBackgourndMonitor is really the only unique portion of a background monitor.
     *
     * @return Status of monitoring the module data.
     */
    virtual BEP_STATUS_TYPE RunBackgroundComponent(void) = 0;
};

#endif //BackgroundTestComponent
