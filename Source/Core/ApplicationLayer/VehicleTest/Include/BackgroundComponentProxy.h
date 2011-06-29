//******************************************************************************
// Description:
//  Proxy for a test component.  It provides the methods to handle a backgound
//  test objective.  It will command the objective, update the results and then
//  die.
//
//==============================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/BackgroundComponentProxy.h $
// 
// 5     10/25/06 4:15p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  This is used
// to       keep the test components running when a new drive curve
// starts.
// 
// 4     7/21/04 11:18a Cward
// Updated forward class references and removed freind class references.
//
// 3     6/09/04 11:20p Cward
// Refactored App layer includes to remove implementation. Updated header
// comment block to new standard
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
// 1     6/20/03 9:18a Khykin
// Initial checkin.
//
// 1     4/22/03 10:24a Khykin
// Initial Working Checkin.
//
// 1     1/27/03 11:09a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 1     8/24/01 3:24p Kh
// Initial Checkin.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BACKGROUNDCOMPONENTPROXY_H
#define BACKGROUNDCOMPONENTPROXY_H
//==============================================================================
using namespace std;

#include "ComponentProxy.h"

class BackgroundComponentManager;

/**
 * Component Proxy Class that provides the methods to
 * command objectives and send update results to the
 * sequencer.
 *
 * @author Kevin Hykin
 * @version 1.0
 */
class BackgroundComponentProxy : public ComponentProxy
{
public:
    /**
     * Vehicle Component Manager constructor.
     *
     * @param logger  Logger interface so data can be logged to file instead of console.
     * @since Version 1.0
     */
    BackgroundComponentProxy(ILogger *logger);
    
    /**
     * Vehicle Component Manager destructor.
     * @since Version 1.0
     */
    virtual ~BackgroundComponentProxy(void);
    
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

    /**
     * Tell the Background Component Proxy who its component
     * manager is.  This is so the proxy can tell its
     * manager when it has completed its objective.
     *
     * @param bcm    Background Component Manager reference.
     * @since Version 1.0
     */
    void SetManager(BackgroundComponentManager *bcm);

    /**
     * After the Background Component Proxy has completed
     * the objective that was given, this method will
     * allow it to remove itself from the component managers
     * list of managed background components.  Based on
     * the success or failure of this method, the decision
     * can be made if the background component proxy should
     * be deleted.
     *
     * @return The status of the removal.
     *         BEP_STATUS_SUCCESS or BEP_STATUS_ERROR
     */
    const INT32 RemoveSelf(void);

private:
    
    /**
     * Reference to the Background Component Manager.
     * @since Version 1.0
     */
    BackgroundComponentManager *m_backgroundCM;
};
#endif //BACKGROUNDCOMPONENTPROXY_H

