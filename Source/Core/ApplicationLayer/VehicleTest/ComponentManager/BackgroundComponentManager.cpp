//*************************************************************************
// Description:
//  A Vehicle Component Manager is responsible for launching a vehicle
//  component, handling the commanding of test objectives, updating the results,
//  and notifying the sequencer when a test objective has been completed.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentManager/BackgroundComponentManager.cpp $
// 
// 8     10/25/06 4:09p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize() and added a 
//       "forceTerminate" variable to TerminateAll().  These are used to
// keep       the test components running when a new drive curve starts.
// 
// 7     5/08/06 7:16p Cward
// switched printf statements to Log statements.
//
// 6     4/13/06 3:29a Cward
// Initialize the BackgroundComponentProxy to NULL side CommandObjective
// and only delete/remove it if it is not NULL
//
// 2     2/15/06 3:47p Bmeinke
// Initialize the BackgroundComponentProxy to NULL side CommandObjective
// and only delete/remove it if it is not NULL
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     6/24/04 12:46a Cward
// Removed pound if 0 blocks. Updated header comment block to the new
// standard.
//
// 3     3/04/04 5:07p Cward
// Added m_logger to Background Component Proxy instantiation.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:45a Khykin
// Initial checkin
//
// 1     6/20/03 9:21a Khykin
// Initial checkin.
//
// 2     6/05/03 7:01a Khykin
// Updated BackgroundComponentProxy threads to be detached.
//
// 1     4/22/03 10:33a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:20a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:10  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:16a Khykin
// Initial Checkin
//
// 7     9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 6     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 5     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 4     12/20/01 10:19a Kh
// Updating the background component thread definition.
//
// 3     8/28/01 6:56p Kh
// Checking in for OS conversion back to 6.0.
//
// 2     8/24/01 3:55p Kh
// Updating changes.
//
// 1     8/02/01 4:22p Kh
// The background component manager is responsible for managing background
// objectives.  (This has not been tested yet)
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "BackgroundComponentManager.h"
#include <typeinfo>


// Forward declaration of the component proxy thread.
extern "C"  void *BackgroundComponentProxyThread(void *arg);

BackgroundComponentManager::BackgroundComponentManager() : ComponentManager(), m_compInfo(NULL)
//, m_proxySem(1)

{
};

BackgroundComponentManager::~BackgroundComponentManager()
{
    Log(LOG_FN_ENTRY, "~BackgroundComponentManager()\n");
    // remove all of the proxies and processes
    if(NumberOfObjectivesLeft() > 0)
    {
        Log(LOG_DEV_DATA, "Terminating Remaining Background Objectives\n");
        if(TerminateAll() != BEP_STATUS_SUCCESS)
            throw BepException("Could Not Terminate All Of the Background Component Proxies\n");
    }
    Log(LOG_FN_ENTRY, "~BackgroundComponentManager() done\n");
};

void BackgroundComponentManager::Initialize(const XmlNode *initInfo, string driveCurveRootDir,
                                            bool reuseComponents)
{
    ComponentManager::Initialize(initInfo, driveCurveRootDir, reuseComponents);
    // Save pointer to component info.
    // NOTE:  Component info is valid until next drive curve is loaded.
    //        it is stored in VehicleTest in an XmlParser object
    m_compInfo = initInfo;
    m_driveCurveRootDir = driveCurveRootDir;
    Log(LOG_FN_ENTRY, "BackgroundComponentManger Initialized\n");
};

void BackgroundComponentManager::CommandObjective(const XmlNode *objective)
{
    BackgroundComponentProxy *cp = NULL;
    char buffer[128];       // buffer to create messages with

    try
    {   // create a background component proxy
        cp = new BackgroundComponentProxy(m_logger);

        // initialize the component proxy with the correct component information
        std::string componentName;
        const XmlNode *component;
        try
        {
            componentName = objective->getAttributes().getNode("Component")->getValue();
            component = m_compInfo->getChildren().getNode(componentName);
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, CreateMessage(buffer, sizeof(buffer), "Could Not Find Component To Command: %s\n", e.Reason().c_str()));
            throw BepException("Could Not Find Component To Command: %s\n", componentName.c_str());
        }
        // MUST! tell background proxy who is the manager before initialization
        cp->SetManager(this);       // tell him I'm his boss
        cp->Initialize(component,m_driveCurveRootDir,false);  // initialize the proxy

        // launch a thread and install the component
        pthread_t id;
        pthread_attr_t attr;
        pthread_attr_init( &attr );
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
        if(pthread_create( &id, &attr, BackgroundComponentProxyThread, (void *) cp) != EOK)
        {
            throw BepException("Could Not Create Background Thread: %s",
                                    objective->getName().c_str());
        }
        cp->SetThreadID(id);    // add my thead ID to the list

        // command the objective
        Log(LOG_DEV_DATA, CreateMessage(buffer, sizeof(buffer), "Commanding %s\n", objective->getName().c_str()));
        cp->CommandObjective(objective);
    }
    catch(XmlException &e)
    {
        if( NULL != cp)
        {
            // if there was a problem, delete the component proxy
        // if I am the one that removed the proxy entry
        if(cp->RemoveSelf() == BEP_STATUS_SUCCESS)
            delete cp;                              // delete the proxy
        }
        throw;
    }
    catch(BepException &e)
    {
        if( NULL != cp)
        {
            // if there was a problem, delete the component proxy
        // if I am the one that removed the proxy entry
        if(cp->RemoveSelf() == BEP_STATUS_SUCCESS)
            delete cp;                              // delete the proxy
        }
        throw;
    }
    catch(...)
    {
        if( NULL != cp)
        {
            // if there was a problem, delete the component proxy
        // if I am the one that removed the proxy entry
        if(cp->RemoveSelf() == BEP_STATUS_SUCCESS)
            delete cp;                              // delete the proxy
        }
        throw;
    }
};

INT32 BackgroundComponentManager::TerminateAll(bool forceTerminate)
{
    Log(LOG_FN_ENTRY, "Entering Background Component TerminateAll()\n");
    return ComponentManager::TerminateAll(forceTerminate);
};

INT32 BackgroundComponentManager::NumberOfObjectivesLeft(void)
{
    INT32   size=0;
    ShowList();
    size = m_compList.size();   // get the number of components in the list
    return(size);
};

const INT32 BackgroundComponentManager::RemoveProxy(const std::string &component)
{
    // create an iterator to walk through the list of components
    INT32 status = BEP_STATUS_ERROR;
    if( m_compList.RemoveComponent( component) == EOK)
    {
        status = BEP_STATUS_SUCCESS;
    }
    return(status);
};

const std::string BackgroundComponentManager::AddProxy(BackgroundComponentProxy *cp)
{
    std::string name = "";      // that name that was used
    COMPONENTITR iter;          // iterator to use with the component list
    char tempName[80];  // scratch pad name to look for
    char buffer[128];       // buffer to create messages with

    int         status;
    uint32_t    ii=0;
    if( (status=m_compList.Lock()) == EOK)
    {
        try
        {
            while( ii < (uint32_t)-1)
            {
                sprintf(tempName, "%s%d", cp->GetName().c_str(), ii);
                Log(LOG_DEV_DATA, CreateMessage(buffer, sizeof(buffer), "AddProxy Looking For: %s\n", tempName));
                iter = m_compList.find(tempName);   // look for the name in the list
                if(iter == m_compList.end())        // of not in the list
                {
                    name = tempName;                // accept the name
                    // add it to the list
                    Log(LOG_DEV_DATA, CreateMessage(buffer, sizeof(buffer), "Adding Component: %s\n", name.c_str()));
                    m_compList.AddComponent(name, cp);
                    break;
                }

                ii++;
            }
        }
        catch(XmlException &e)
        {
            Log(LOG_ERRORS, CreateMessage(buffer, sizeof(buffer), "AddProxy XmlException: %s\n", e.Reason().c_str()));
            m_compList.Unlock();
            throw;
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, CreateMessage(buffer, sizeof(buffer), "AddProxy BepException: %s\n", e.Reason().c_str()));
            m_compList.Unlock();
        throw;
    }
        m_compList.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, CreateMessage(buffer, sizeof(buffer), "Error locking component list in AddProxy: %s\n", strerror( status)));
    }

    return(name);
};

void BackgroundComponentManager::LaunchComponents(const XmlNode *compList)
{
    Log(LOG_FN_ENTRY, "The BackgroundComponentManger can not launch components!\n");
}

void BackgroundComponentManager::ShowList(void)
{
    // walk through the list and display the components
    COMPONENTITR iter = m_compList.begin();
    char buffer[128];       // buffer to create messages with
    int status;

    if( (status=m_compList.Lock()) == EOK)
    {
    for(;iter != m_compList.end(); iter++)
        Log(LOG_DEV_DATA, CreateMessage(buffer, sizeof(buffer), "Component In List: %s\n", (*iter).first.c_str()));

        m_compList.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, CreateMessage(buffer, sizeof(buffer), "Error locking component list in ShowList: %s\n", strerror( status)));
    }
};

extern "C"
{
    /**
     * Component Proxy Thread that will be started to manage
     * the component proxy that was handed in.
     *
     * @param arg    The arg is really a component proxy that will be
     *               used in each specific instance of the component
     *               proxy thread.
     * @return Nothing.
     * @since Version 1.0
     */
    void *BackgroundComponentProxyThread(void *arg)
    {
        BackgroundComponentProxy *cp;
        string logMessage;
        try
        {
            cp = (BackgroundComponentProxy *) arg;
            if(cp == NULL)
                throw BepException("Invalid Component Proxy Installed In Thread");
            // process the objective that has been commanded
            char buff[64];
            logMessage = "BackgroundComponent Thread Started: " + cp->GetName() + itoa(cp->GetThreadID(), buff, 10);
            cp->Log(LOG_FN_ENTRY, logMessage.c_str());
            cp->ProcessObjective();
            logMessage = "BackgroundComponent Thread Terminated: " + cp->GetName();
            cp->Log(LOG_FN_ENTRY, logMessage.c_str());
        }
        catch(BepException &e)
        {
            logMessage = cp->GetName() + "Thread BepException: " + e.Reason();
            cp->Log(LOG_ERRORS, logMessage.c_str());
        }
        catch(...)
        {
            logMessage = cp->GetName() + "Thread Unknown Exception";
            cp->Log(LOG_ERRORS, logMessage.c_str());
        }

        cp->RemoveSelf();   // remove the proxy from the list
        return(NULL);
    };
};


