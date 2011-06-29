//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/TemplateIBepCommunicationThreadPool.h 3     10/30/07 3:51p Rwiersem $
//
// FILE DESCRIPTION:
//   Thread pool for communicating via IBepCommunication
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/TemplateIBepCommunicationThreadPool.h $
// 
// 3     10/30/07 3:51p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:25p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 2     9/12/02 5:47p Tbochene
// Updated to derive from ILogger
//
// 1     9/10/02 6:22p Tbochene
// New thread pool for communicating via IBepComm
//
//
//*************************************************************************

#ifndef TemplateIBepCommunicationThreadPool_h
#define TemplateIBepCommunicationThreadPool_h

#include "IBepCommunication.h"
#include "TemplateThreadPool.h"

/**
 * Class that communicates via IBepCommunication in a thread for server
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since 1.0
 */
class CommunicationThread : public AbstractThread
{
public:
    /**
     * Default constructor
     * @since Version 1.0
     */
    CommunicationThread(void);
    /**
     * Initializing constructor
     *
     * @param config Setup for object
     * @param pool   Thread pool that is managing the thread
     * @since Version 1.0
     */
    CommunicationThread(const XmlNode* config, IThreadPool* pool = NULL);
    /**
     * Class Destructor
     * @since Version 1.0
     */
    virtual ~CommunicationThread(void);
    /**
     * Initializer method.
     */
    virtual void Initialize(void);
protected:
    /**
     * Funciton to be called when thread is created and blocked
     * @since Version 1.0
     */
    virtual void Block(void);
    /**
     * Funciton to be called when Block() returns and should do some work
     * @since Version 1.0
     */
    virtual void Handle(void);
protected:
    /**
     * Object to block waiting for requests
     * @since Version 1.0
     */
    IBepCommunication m_server;
    /**
     * Object to handle requests
     * @since Version 1.0
     */
    IBepCommunication m_worker;
    /**
     * Component used to handle request
     * @since Version 1.0
     */
    BepComponent* m_component;
    /**
     * Id of channel to reply to
     * @since Version 1.0
     */
    INT32 m_replyId;
};

/**
 * Data type definition for base class of TemplateIBepCommunicationThreadPool
 * @since Version 1.0
 */
typedef TemplateThreadPool<CommunicationThread> TemplateIBepCommunicationThreadPoolType;

/**
 * Thread pool used for server communication via IBepCommunication
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class TemplateIBepCommunicationThreadPool : public TemplateIBepCommunicationThreadPoolType
{
public:
    friend class CommunicationThread;
    /**
     * Class constructor.
     *
     * @param comp    The component that created the thread pool.
     * @param creator The thread that created the thread pool.
     */
    TemplateIBepCommunicationThreadPool(BepComponent* comp, const pthread_t& creator = pthread_self());

    virtual ~TemplateIBepCommunicationThreadPool(void);

    /**
     * Initialize the thread pool.
     *
     * @param configuration
     *               Xml Node containing the parameters for the behavior of
     *               the thread pool.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *configuration);

protected:
    /**
     * Server to listen for new connections
     * @since Version 1.0
     */
    IBepCommunication m_server;
    /**
     * Component to handle requests
     * @since Version 1.0
     */
    BepComponent* m_component;
};

#endif // TemplateIBepCommunicationThreadPool_h

