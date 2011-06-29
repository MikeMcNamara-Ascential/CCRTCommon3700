//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentProxy/ForegroundComponentProxy.cpp 4     10/25/06 4:13p Rwiersem $
//
// FILE DESCRIPTION:
// Vehicle Component Proxy that is responsible for handling the sequencing
// of test objectives.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentProxy/ForegroundComponentProxy.cpp $
// 
// 4     10/25/06 4:13p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  This is used
// to       keep the test components running when a new drive curve
// starts.
// 
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     3/04/04 5:11p Cward
// Added logger parameter to constructor.
//
// 1     12/21/03 6:34p Cward
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 2     8/06/03 3:04p Khykin
// Update with the latest changes from Flatrock.
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:22a Khykin
// Initial checkin.
//
// 2     4/30/03 9:18a Khykin
// Updated debugging statements to include TID.
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
// 9     10/21/02 11:16a Khykin
// Removed commented out code that was unnecessary.
//
// 8     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 6     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 5     3/06/02 1:47p Kh
// Updating the diagnostics.
//
// 4     12/20/01 10:19a Kh
// Updating the proxy to indicate when ready to be terminated.
//
// 3     11/13/01 1:57p Kh
// Updating diagnostic printout information.
//
// 2     11/09/01 2:47p Kh
// Updating for changes to the base component.
//
// 1     8/24/01 3:24p Kh
// Initial Checkin.
//
//*************************************************************************
#include "ForegroundComponentProxy.h"
#include <errno.h>
#include <unistd.h>

ForegroundComponentProxy::ForegroundComponentProxy(ILogger *logger) : ComponentProxy(logger)
{
    SetType("Foreground");  // set the component type to foreground
};

ForegroundComponentProxy::~ForegroundComponentProxy(void)
{
    m_logger->Log(LOG_FN_ENTRY, "~ForegroundComponentProxy: %s, %s\n", GetStatus().c_str(), GetMyName().c_str());
};

void ForegroundComponentProxy::Initialize(const XmlNode *component, string driveCurveRootDir, 
                                          bool reuseComponents)
{
    char buffer[128];
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: ForegroundComponentProxy::Initialize()1 - %d\n", ReturnMemArena()));
    ComponentProxy::Initialize(component, driveCurveRootDir, reuseComponents); // initialize the base component proxy
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: Done initializing base class ComponentProxy - %d\n", ReturnMemArena()));
    m_logger->Log(LOG_FN_ENTRY, "ForegroundComponentProxy::Initialize()\n");

    // launch the vehicle component
    LaunchComponent(component);
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: ForegroundComponentProxy::Initialize()2 - %d\n", ReturnMemArena()));
    // connect to the launched component
    ComponentCommInit(component);
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: ForegroundComponentProxy::Initialize()3 - %d\n", ReturnMemArena()));
    // register the component
    XmlNode registerMessage("Register", "0");
    std::string response;
    m_componentComm.Register(&registerMessage, response, true);
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: Done registering component comm - %d\n", ReturnMemArena()));

    m_logger->Log(LOG_FN_ENTRY, "Component Proxy Initialized\n");
    SetStatus(BEP_RUNNING_STATUS);  // set the task running
    m_logger->Log(LOG_DETAILED_DATA, CreateMessage(buffer, sizeof(buffer), "MEM: ForegroundComponentProxy::Initialize()4 - %d\n", ReturnMemArena()));
};

void ForegroundComponentProxy::ProcessObjective(void)
{
    std::string testStatus; // the local testing status
    INT32       cmdStatus;  // command status

    // while the process is running (not terminated)
    testStatus = GetStatus();
    while(testStatus != BEP_TERMINATE_STATUS)
    {
        try
        {   // always reset the objective to null
            if(m_objective != NULL)
            {
                m_logger->Log(LOG_DEV_DATA, "%s Objective %s Deleted\n",
                              m_objective->getAttributes().getNode("Component")->getValue().c_str(),
                              m_objective->getValue().c_str());
                delete m_objective;
                m_objective = NULL;
            }
            // signal complete processing objective and wait for a new command
            SignalObjectiveDone();

            m_logger->Log(LOG_DEV_DATA, "(%d) ForegroundComponentProxy %s: Waiting For A Command\n", pthread_self(), GetName().c_str());
            // wait for the next commanded objective
            WaitForNewObjective();

            m_logger->Log(LOG_DEV_DATA, "(%d) ForegroundComponentProxy %s: Command Received\n", pthread_self(), GetName().c_str());

            // if an objective to execute or the status is not running send a message
            if((GetStatus() == BEP_TESTING_STATUS) && (m_objective != NULL))
            {   // send the command to the component
                cmdStatus = SendCommand();
                // update the results of the command
                if(UpdateResults(cmdStatus) != BEP_STATUS_SUCCESS)
                    throw BepException("Could Not Update Results For: %s", m_objective->getChildren().getNode(0)->getName().c_str());
            }
            m_logger->Log(LOG_DEV_DATA, "ForegroundComponentProxy %s Command: %s\n", GetName().c_str(), GetStatus().c_str());
        }
        catch(XmlException &e)
        {
            m_logger->Log(LOG_ERRORS, "%s Process Objective XmlException: %s\n", GetName().c_str(), e.Reason().c_str());
            if(m_objective != NULL)
            {
                XmlNode *result = const_cast<XmlNode *> (m_objective->getAttributes().getNode("Result"));
                result->setValue(e.Reason());
            }
            SetStatus(BEP_TERMINATE_STATUS);
        }
        catch(BepException &e)
        {
            m_logger->Log(LOG_ERRORS, "%s Process Objective BepException: %s\n", GetName().c_str(), e.Reason().c_str());
            if(m_objective != NULL)
            {
                XmlNode *result = const_cast<XmlNode *> (m_objective->getAttributes().getNode("Result"));
                result->setValue(BEP_SOFTWAREFAIL_STATUS);
            }
            SetStatus(BEP_TERMINATE_STATUS);

        }
        catch(...)
        {
            m_logger->Log(LOG_ERRORS, "%s Process Objective Unknown Command Exception\n", GetName().c_str());
            if(m_objective != NULL)
            {
                XmlNode *result = const_cast<XmlNode *> (m_objective->getAttributes().getNode("Result"));
                result->setValue("Unknown Command Exception");
            }
            SetStatus(BEP_TERMINATE_STATUS);
        }

        testStatus = GetStatus();   // update the current status
    }

    m_logger->Log(LOG_FN_ENTRY, "ForegroundComponentProxy %s: Terminating\n", GetName().c_str());
};
