//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/ForegroundComponentProxy.h 4     10/25/06 4:19p Rwiersem $
//
// FILE DESCRIPTION:
// Proxy for a test component.  It provides the methods to sequence a test
// objective and then update the results from the test.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/ForegroundComponentProxy.h $
// 
// 4     10/25/06 4:19p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  This is used
// to       keep the test components running when a new drive curve
// starts.
// 
// 3     7/21/04 11:18a Cward
// Updated forward class references and removed freind class references.
//
// 2     3/04/04 5:12p Cward
// added logger
//
// 1     12/21/03 6:34p Cward
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:43a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 1     4/22/03 10:25a Khykin
// Initial Working Checkin.
//
// 1     1/27/03 11:10a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 1     8/24/01 3:24p Kh
// Initial Checkin.
//
//*************************************************************************
#ifndef FOREGROUNDCOMPONENTPROXY_H
#define FOREGROUNDCOMPONENTPROXY_H
//==============================================================================
using namespace std;

#include "ComponentProxy.h"

    
/**
 * Component Proxy Class that provides the methods to
 * command objectives and send update results to the
 * sequencer.
 *
 * @author Kevin Hykin
 * @version 1.0
 */
class ForegroundComponentProxy : public ComponentProxy
{
public:
    /**
     * Vehicle Component Manager constructor.
     *
     * @param logger  Logging interface to use for data logging
     * @since Version 1.0
     */
    ForegroundComponentProxy(ILogger *logger);
    
    /**
     * Vehicle Component Manager destructor.
     * @since Version 1.0
     */
    virtual ~ForegroundComponentProxy(void);
    
    /**
     * Initialization method that launches the component
     * that will be managed, connects to it for communication,
     * and connects to the results server.
     *
     * @param component The component information necessary to launch a component
     *                  and connect to it as a client.
     * @param reuseComponents
     *                 Flag indicating if the components should be killed/relaunched (false) or
     *                 if they should be reused from one test to another (true)
     */
    virtual void Initialize(const XmlNode *component, string driveCurveRootDir, 
                            bool reuseComponents=false);
    
    /**
     * Method that the thread waits on until an objective
     * is commanded.  It will send the request to the
     * component, wait for the response, clear the objective
     * and then send the response back to the sequencer.
     * @since Version 1.0
     */
    virtual void ProcessObjective(void);
};

#endif //FOREGROUNDCOMPONENTPROXY_H

