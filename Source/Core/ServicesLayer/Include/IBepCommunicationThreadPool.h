//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/IBepCommunicationThreadPool.h 4     10/26/06 9:16a Rwiersem $
//
// FILE DESCRIPTION:
//      This is a thread pool class which handles communication with an active client.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/IBepCommunicationThreadPool.h $
// 
// 4     10/26/06 9:16a Rwiersem
// Changes for the 06102701 core release:
// 
// Added StopThreadPool().
// 
// 3     7/20/05 8:47p Cward
// made m comm public
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:10p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:37  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 12    11/21/02 8:37a Khykin
// Updating to use the devined buffer size and timeout.
//
// 11    6/18/02 12:54p Dan.erickson
// Removed erase() calls for strings.  This is a costly operation.
//
// 10    6/07/02 11:49a Dan.erickson
// Make sure message variable is cleared out in the Handle method.
//
// 9     4/25/02 4:40p Skypig13
// Updated with new changes
//    Revision 1.2  2002/04/11 19:19:34  erickson
//    Updated ThreadPoolBase to properly use muticies and updated tests for INamedDataBroker.  Data server runs and SIGSEGV when the Subscription manager receives a subscription request and attemtps to create the first threads in the  Notification Thread Pool (first ContextAllocate()).
//
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 6     4/02/02 11:05p Skypig13
// Updates for IPublish interface and efficiency
//
// 5     3/12/02 2:59p Dan.erickson
// Corrected GetName() method.  Could not cast m_comm to IPC object so
// method always returned an empty string as the name.
//
// 4     2/12/02 5:15p Dan.erickson
// Replaced THREAD_POOL_PARAM_T with ThreadPoolInfo.
//
// 3     2/07/02 6:30p Skypig13
// Updated methods to call base class virtual handlers first
//
// 2     2/05/02 1:42p Skypig13
// Removed warnings
//
// 1     2/05/02 12:34a Skypig13
// Thread pool used by a server to comminicate using IBepCommunication
//
//*************************************************************************
#ifndef IBepCommunicationThreadPool_h
#define IBepCommunicationThreadPool_h

#include "ThreadPoolBase.h" // NOTE: This must be included first for proper definition of thread pool data types!!!
#include "IBepCommunication.h"

/**
 * Class used by a server to receive messages on a communication
 * channel opened by an IBepCommunication object.
 *
 * @author Tim Bochenek
 * @version 1.1
 * @since Version 1.0
 */
class IBepCommunicationThreadPool : public ThreadPoolBase
{
public:
    /**
     * Data type definition of structure required to handle
     * IBepCommunicaitons
     * @since Version 1.0
     */
    typedef struct _IBepThreadPoolInfo
    {
        /**
         * Communication object to get messages from a channel
         * @since Version 1.0
         */
        IBepCommunication comm;
        /**
         * Id of channel to reply to
         * @since Vresion 1.0
         */
        INT32 replyId;
    } IBepThreadPoolInfo;
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    IBepCommunicationThreadPool(BepComponent *comp);

    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~IBepCommunicationThreadPool(void);

    /**
     * Method used to set up class
     *
     * @param config Configuration node used to set up class
     * @since Version 1.0
     */
    void Initialize(const XmlNode* config);

    /**
     * Checks if data response was Ok
     *
     * @param data   Data to be evaluated
     * @return If data is not a failure mode
     * @since Version 1.0
     */
    bool IsDataOk(const XmlString &data);

    /**
     * Get the named channel the object is using.
     *
     * @return Name of the channel (IPC only).
     * @since Version 1.0
     */
    const std::string GetName(void);

    /**
     * Set the priority of the thread pool.
     *
     * @param priority The new priority to use for the thread pool.
     */
    void SetPriority(int priority);

protected:
    /**
     * Dervied class method for performing setup for each thread.
     * @since Version 1.0
     */
    virtual void Allocate(ThreadPoolInfo *param);

    /**
     * Dervied class method for performing the blocking operation.
     * @since Version 1.0
     */
    virtual void Block(ThreadPoolInfo *param);

    /**
     * Derived class method for performing the work.
     *
     * @return Must return zero.
     * @since Version 1.0
     */
    virtual INT32 Handle(ThreadPoolInfo *param);

    /**
     * Dervied class method for destroying threads.
     * If any memory was dynamicaly declared, it must be deleted
     * in the derived Free method.
     * @since Version 1.0
     */
    virtual void Free(ThreadPoolInfo *param);

public:
    /**
     * Communication object that will do the talking
     * @since Version 1.0
     */
    IBepCommunication m_comm;

    /**
     * Used to terminate all threads in the thead pool.
     */
    virtual void StopThreadPool();
    
protected:
    /**
     * Component that will do some work for us (creator)
     * @since Version 1.0
     */
    BepComponent* m_component;

    /**
     * The initial priority of the thread.  This is used to return
     * the thread back to the original priority after the handling
     * of a request that might have bumped the priority of the
     * thread.
     * @since Version 1.1
     */
    int m_priority;
};

#endif // IBepCommunicationThreadPool_h
