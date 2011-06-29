//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/BackgroundComponentManager.h 5     10/25/06 4:15p Rwiersem $
//
// FILE DESCRIPTION:
// A Vehicle Component Manager is responsible for launching a vehicle
// component, handling the commanding of test objectives, updating the results,
// and notifying the sequencer when a test objective has been completed.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/BackgroundComponentManager.h $
// 
// 5     10/25/06 4:15p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize() and added a 
//       "forceTerminate" variable to TerminateAll().  These are used to
// keep       the test components running when a new drive curve starts.
// 
// 4     7/21/04 11:18a Cward
// Updated forward class references and removed freind class references.
//
// 3     3/04/04 5:12p Cward
// added logger
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
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
// 3     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 2     1/04/02 10:24a Kh
// Overloading LaunchComponents so the Background Component can not launch
// components.
//
// 1     8/24/01 3:24p Kh
// Initial Checkin.
//
//*************************************************************************
#ifndef BACKGROUNDCOMPONENTMANAGER_H
#define BACKGROUNDCOMPONENTMANAGER_H
//==============================================================================
using namespace std;

#include "ComponentManager.h"
#include "BackgroundComponentProxy.h"
#include "BepDefs.h"
#include "BepSemaphore.h"
#include <semaphore.h>
#include <errno.h>


/**
 * Component Manager class.  This class is responsible
 * for commanding vehicle components in the background
 * of a vehicle test.
 *
 * @author Kevin Hykin
 * @version 1.0
 */
class BackgroundComponentManager : public ComponentManager
{
public:
    /**
     * Vehicle Component Manager constructor.
     * @since Version 1.0
     */
    BackgroundComponentManager(void);
    
    /**
     * Vehicle Component Manager destructor.
     * @since Version 1.0
     */
    virtual ~BackgroundComponentManager();
    
    /**
     * Initialization method that sets up the ComponentManager
     * to manage a list of test components.
     *
     * @param compList Initialization information for the ComponentManager.
     * @param reuseComponents
     *                 Flag indicating if the components should be killed/relaunched (false) or
     *                 if they should be reused from one test to another (true)
     */
    virtual void Initialize(const XmlNode *compList, string driveCurveRootDir, 
                            bool reuseComponents=false);
    
    /**
     * Method that will be used to command the objective
     * that should be executed and then it wake up the manager.
     *
     * @param objective The objective that needs to be commanded.
     * @since Version 1.0
     */
    virtual void CommandObjective(const XmlNode *objective);

    /**
     * Command all of the components to Termiante.
     * 
     * @param forceTerminate
     *               Flag indicating if we should truly terminate all components instead of
     *               deactivating them (in order to properly handle system shutdown)
     * 
     * @return The status of the command.
     */
    virtual INT32 TerminateAll(bool forceTerminate=false);
    
    /**
     * Get the number of objectives that have been sequenced
     * and have not completed.
     *
     * @return Number of objectives still executing.
     */
    INT32 NumberOfObjectivesLeft(void);
    
    /**
     * Remove the specified component from the background
     * component list.
     *
     * @param component The name of the component to remove.
     * @return The success or failure of the removal.
     *         BEP_STATUS_SUCCESS or BEP_STATUS_ERROR
     * @since Version 1.0
     */
    const INT32 RemoveProxy(const std::string &component);
    
    /**
     * Add a component to the list for management.
     *
     * @param cp     The component proxy to add.
     * @return The name that was used in the list for the component
     *         proxy. This is due to the chance that multiple
     *         background objectives can be entered for the same
     *         component.
     * @since Version 1.0
     */
    const std::string AddProxy(BackgroundComponentProxy *cp);

protected:
    /**
     * Overloaded method to prevent the background component manager from
     * launching components.
     *
     * @param compList List of components.
     * @since Version 1.0
     */
    virtual void LaunchComponents(const XmlNode *compList);
    
    /**
     * Debug utility to show the current components in the
     * list.
     * @since Version 1.0
     */
    void ShowList(void);
    
    /**
     * Reference to the list of component that is being
     * managed.
     * @since Version 1.0
     */
    const XmlNode *m_compInfo;

    /**
     * Semaphore to wait on when waiting for a component to
     * complete a process.
     * @since Version 1.0
     */
    //BepSemaphore m_proxySem;

    /**
     * The type of component that is running.
     * @since Version 1.2
     */
    std::string m_driveCurveRootDir;
};


#endif  // BACKGROUNDCOMPONENTMANAGER                               
