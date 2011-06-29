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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Include/BackgroundMonitor.h $
// 
// 7     10/30/07 1:28p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added Javadoc comments.
// 
// 6     10/25/06 3:04p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a member variable to tell if the thread was joined
//     - Added a member variable to tell if the printf's should be called
// 
// 5     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 4     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 3     6/09/04 10:43p Cward
// Refactored App layer includes remove implementation. Also updated
// header comments to new standard
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:32p Cward
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BackgroundMonitor_h
#define BackgroundMonitor_h
//==============================================================================
#include "ProtocolFilter.h"
#include "AtomicThread.h"

/**
 * BackgroundMonitor.
 * This class uses the provided ProtocolFilter for sending messages to
 * the vehicle module periodically.  This class assumes that module communications have
 * already been established.
 *
 * @author Dan Erickson
 * @see AtomicThread
 * @see ProtocolFilter
 * @since November 17, 2003
 */
class BackgroundMonitor : public AtomicThread
{
    friend class BackgroundMonitorTest;
    friend class SendModuleKeepAliveTest;

public:
    /**
     * Class Constructor.
     *
     * @param vehicleComm
     *               Communication object to use.
     * @param interMessageGap
     *               Time delay between messages (in ms).
     */
    BackgroundMonitor(IProtocolFilter *vehicleComm, const INT32 interMessageGap);
    /**
     * Class destructor.
     * <p>
     * <b>Description:</b><br>
     * The monitor will be instructed to stop.  This will halt the loop periodically sending messages to the module.
     * Once the loop is commanded to halt, the object will wait until all loop processing and post processing is complete 
     * prior to destroying the object.
     */
    virtual ~BackgroundMonitor(void);
    /**
     * Funciton to be executed when thread is run.
     * <p>
     * <b>Description:</b><br>
     * The loop will continue to execute until <i>SendMessage()</i> returns <b>false</b>.  The time of the last message
     * transmitted to the module will also be tracked.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ClockTime() </li>
     *      <li> SendMessage() </li>
     *      <li> MonitorData() </li>
     *      </ul>
     */
    virtual void* Execute(void);
    /**
     * Signal thread to stop monitoring data.
     *
     * @return EOK if successfully told thread to stop.
     */
    INT32 StopMonitor(void);
    /**
     * Wait until the thread signals that it is done with its current
     * message and is safe to be deleted.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInitialThreadId() </li>
     *      </ul>
     * 
     * @return Returns EOK if the thread was properly joined and deleted.
     */
    virtual INT32 WaitUntilDone(void);

protected:
    /**
     * Monitor data from the module.
     * <p>
     * <b>Description:</b><br>
     * Do the actual work of monitoring data from the module.  This is really the only unique portion of a background
     * monitor.  This function must be implemented in all derived classes to handle the requirements of that object.
     * <p>
     *
     * @return Status of monitoring the module data.
     */
    virtual BEP_STATUS_TYPE MonitorData(void) = 0;
    /**
     * Determine if the message should be sent.
     * <p>
     * <b>Description:</b><br>
     * This function will loop until it is time to send the next message to the module.  A simple delay is not used since
     * the thread would be tied up waiting until the message gap time expired.  By waiting in small increments for the 
     * complete time to expire, the thread can exit quickly when an abort condition occurs or when it is time to kill
     * the thread.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInterMessageGap() </li>
     *      <li> ClockTime() </li>
     *      </ul>
     * 
     * @return Flag indicating if a message should be sent to the module.
     *      <ul>
     *      <li> True - Send the message to the module. </li>
     *      <li> False - The message should not be sent to the module. </li>
     *      </ul>
     */
    const bool SendMessage(void);
    /**
     * Get the time to delay between monitor messages.  This data is passed in to the constructor when the object is created.
     *
     * @return Time delay in milliseconds between messages.
     */
    inline const INT32 &GetInterMessageGap(void);
    /**
     * Store the intermessage gap time.
     * 
     * @param mssgGap The amount of time in milliseconds between consecutive transmit operations.
     */
    void SetInterMessageGap( INT32 mssgGap);
    /**
     * Communication object to use for talking to the vehicle.
     */
    IProtocolFilter *m_vehicleComm;
    /**
     * Flag to indicate if keep alive message needs to be sent.
     */
    BepCondVar<bool>    m_sendMessage;
    /**
     * Timestamp of when we sent the last message
     */
     uint64_t m_lastSendTime;
    
     /**
      * Flag indicating if the thread has already been joined. This will prevent
      * a message being printed as a result of trying to join a thread that has 
      * already been joined.
      */
     bool   m_threadJoined;
    
private:
    /**
     * Time to delay between keep alive messages.
     */
    INT32 m_interMessageGap;
    /**
     * Flag to tell if the printf's should be enabled.
     */ 
    bool m_enablePrintfs;
};

#endif //BackgroundMonitor
