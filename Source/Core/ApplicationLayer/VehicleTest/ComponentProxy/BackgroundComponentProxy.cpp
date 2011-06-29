//******************************************************************************
// Description:
//  Vehicle Component Proxy that is responsible for handling the sequencing
//  of a test objective.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentProxy/BackgroundComponentProxy.cpp $
// 
// 6     10/25/06 4:11p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  This is used
// to       keep the test components running when a new drive curve
// starts.
// 
// 5     4/13/06 3:30a Cward
// deleted unused removeStatus
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     6/09/04 11:16p Cward
// refactored App layer includes to remove implementation. Updated the
// header comment block to new standard.
//
// 2     3/04/04 5:08p Cward
// Replaced printfs with ILogger methods
//
// 1     12/21/03 6:34p Cward
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 2     8/06/03 2:59p Khykin
// Updating with latest changes from Flatrock.
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:21a Khykin
// Initial checkin.
//
// 1     4/22/03 10:33a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:20a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:11  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:16a Khykin
// Initial Checkin
//
// 5     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 3     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 2     12/20/01 10:19a Kh
// Changing the location of where the component removes itself fro
//
// 1     8/24/01 3:24p Kh
// Initial Checkin.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "BackgroundComponentProxy.h"
#include "BackgroundComponentManager.h"
#include <errno.h>
#include <unistd.h>

BackgroundComponentProxy::BackgroundComponentProxy(ILogger *logger) : ComponentProxy(logger)
{
    m_backgroundCM = NULL;
    SetType("Background");  // set the component proxy type to background
}

BackgroundComponentProxy::~BackgroundComponentProxy(void)
{
    m_logger->Log(LOG_FN_ENTRY, "~BackgroundComponentProxy: %s, %s\n", GetStatus().c_str(), GetName().c_str());
}

void BackgroundComponentProxy::Initialize(const XmlNode *component, string driveCurveRootDir,
                                          bool reuseComponents)
{
    m_logger->Log(LOG_FN_ENTRY, "BackgroundComponentProxy::Initialize()\n");
    ComponentProxy::Initialize(component, driveCurveRootDir, reuseComponents); // initialize the base component proxy

    // connect to the launched component
    ComponentCommInit(component);

    // set my name to what I was configured with, register and then
    // update my name with what I was added as
    SetName(component->getChildren().getNode("ComponentName")->getValue());
    if(m_backgroundCM != NULL)  // if running normally
    {
        std::string name = m_backgroundCM->AddProxy(this);  // add myself to the managers list
        SetName(name);                                      // set my name to what I was added to the list as
    }
    else
        throw BepException("Component %s: Invalid Component Manager", component->getName().c_str());

    SetStatus(BEP_RUNNING_STATUS);                          // set my status to running
    m_logger->Log(LOG_FN_ENTRY, "Component Proxy Initialized\n");
}

void BackgroundComponentProxy::ProcessObjective(void)
{
    INT32       cmdStatus=BEP_STATUS_ERROR;     // command status
    INT32       updateStatus=BEP_STATUS_ERROR;  // update status

    if(m_objective != NULL)
    {
        m_logger->Log(LOG_DEV_DATA, "%s Objective %s Deleted\n",
                      m_objective->getAttributes().getNode("Component")->getValue().c_str(),
                      m_objective->getValue().c_str());
        m_objective->clear();
        delete m_objective;
        m_objective = NULL;
    }

    // signal that objective processing is complete
    SignalObjectiveDone();

    m_logger->Log(LOG_DEV_DATA, "Background ComponentProxy %s: Waiting For A Command\n", GetName().c_str());
    // wait for the next objective
    WaitForNewObjective();

    m_logger->Log(LOG_DEV_DATA, "Background ComponentProxy %s: Command Received\n", GetName().c_str());

    // if an objective to execute or the status is not running send a message
    if((GetStatus() == BEP_TESTING_STATUS) && (m_objective != NULL))
    {   // send the command to the component
        cmdStatus = SendCommand();
        // update the results of the command
        updateStatus = UpdateResults(cmdStatus);
    }
    else
    {
        m_logger->Log(LOG_ERRORS, "Error, Background ComponentProxy Command: %s\n", GetStatus().c_str());
    }

    if( (cmdStatus != BEP_STATUS_SUCCESS) &&
        (updateStatus != BEP_STATUS_SUCCESS) )
    {
        throw BepException("Process Objective Error: %d, %d", cmdStatus, updateStatus);
    }
}

void BackgroundComponentProxy::SetManager(BackgroundComponentManager *bcm)
{
    m_backgroundCM = bcm;
}

const INT32 BackgroundComponentProxy::RemoveSelf(void)
{
    INT32 status = BEP_STATUS_SUCCESS;

    if(m_backgroundCM != NULL)  // remove the proxy from the manged list if in one
    {
        m_logger->Log(LOG_DEV_DATA, "Removing: %s\n", GetName().c_str());
        status = m_backgroundCM->RemoveProxy(GetName());
    }
    return(status);
}
