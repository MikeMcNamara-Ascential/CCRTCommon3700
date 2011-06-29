//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/HeartBeatThread.h 3     10/30/07 3:36p Rwiersem $
//
// FILE DESCRIPTION:
//   Heartbeat thread class
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/HeartBeatThread.h $
// 
// 3     10/30/07 3:36p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 3     8/12/03 2:48p Khykin
// Updated with new directory update changes.
//
// 2     8/06/03 3:31p Khykin
// Updated with latest changes from Flatrock.
//
// 6     6/17/03 10:03a Tbochene
// Eleminated nyquist problem with heartbeat
//
// 5     6/04/03 1:34p Tbochene
// Updated to use server reference instead of data referances (causes
// segmentation faults)
//
// 4     5/30/03 2:09p Tbochene
// Updated control data to be passed in and update logic
//
// 2     5/28/03 12:53p Tbochene
// Updated heartbeat tags and reflected heartbeat tags as well as
// constructor and initializers
//
//*************************************************************************
#ifndef HeartBeatThread_h
#define HeartBeatThread_h
//==============================================================================

#include "ILogger.h"
#include "AtomicThread.h"
#include "XmlNode.h"
#include "BepDefs.h"
#include "BepServer.h"
#include "INamedDataBroker.h"
/**
 * Heartbeat thread class
 *
 * @author Tim Bocehenk
 * @version Version 1.1
 */
class HeartBeatThread : public AtomicThread, public ILogger
{
public:
    /**
     * Class consructor.
     */
    HeartBeatThread(void);

    /**
     * Class consructor.
     *
     * @param document Reference to a parsed configuration document.
     * @param server   Server that will provide data
     * @since Version 1.0
     */
    HeartBeatThread(const XmlNode *document, BepServer* server = NULL);

    /**
     * Class desructor.
     * @since Version 1.0
     */
    virtual ~HeartBeatThread(void);

    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @param server   Server that will provide data
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document, BepServer* server = NULL);

protected:
    /**
     * Funciton to be executed when thread is run
     *
     * @return Status
     * @since Version 1.0
     */
    virtual void* Execute(void);

    /**
     * Return the amount of time in milliseconds to delay
     * between sampling the heartbeat bit
     *
     * @return Amount of time in ms to delay
     * @since Version 1.0
     */
    const UINT32 GetSamplingDelay(void);

    /**
     * Toggle the heartbeat bit at the configured rate.
     */
    void UpdateHeartbeat(void);

private:
    /**
     * Interval which the heartbeat status shuld be checked
     * @since Version 1.0
     */
    UINT32 m_heartbeatSampleDelay;
    /**
     * Communication object used to read and write data
     * @since Version 1.0
     */
    INamedDataBroker* m_hbComm;
    /**
     * Tag of reflected state
     * @since Version 1.1
     */
    std::string m_reflectedHbTag;
    /**
     * Tag of our state
     * @since Version 1.1
     */
    std::string m_HbTag;
    /**
     * Provider of data we are interested in monitoring
     * or modifying
     * @since Version 1.1
     */
    BepServer* m_server;
};
#endif // HeartBeatThread_h

