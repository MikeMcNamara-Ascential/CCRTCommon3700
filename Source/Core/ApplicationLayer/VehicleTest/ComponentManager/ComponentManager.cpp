//*************************************************************************
// Description:
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentManager/ComponentManager.cpp $
// 
// 12    10/30/07 3:26p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed the logic for checking if all components have been removed in
// TerminateAll().
// 
// 11    1/17/07 3:53p Rwiersem
// Changes for the 07011901 core release:
// 
// - In the exception handling of CommandTerminate(), only delete the
// component proxy if the component is unable to be removed.
// - Only send CommandAbort to activated components in AbortAll().
// - Only send CommandReloadConfiguration to activated components in
// ReloadConfigurationAll().
// - Only send CommandReset to activated components in ResetAll().
// - Moved the try-catch block inside the compnent list loop so the pulse
// will be sent to the remaining components if an exception is caught.
// - Added exception handling in TerminateAll() in case a component exits
// abnormally.
// - Pass the short component name as an argument to CommandActivate() in
// AddComponents().
// - In the exception handling of AddComponents(), only delete the
// component proxy if the component is unable to be removed.
// 
// 10    10/25/06 4:10p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize() and added a 
//       "forceTerminate" variable to TerminateAll().  Added
// CommandDeactivate() and GetComponentIpcName().  These are used       to
// keep the test components running when a new drive curve starts.
// 
// 9     5/08/06 7:16p Cward
// switched printf statements to Log statements.
//
// 8     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 7     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 6     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 5     6/24/04 12:46a Cward
// Removed pound if 0 block.
//
// 4     6/09/04 11:14p Cward
// Refactored App layer includes to remove implementation. Update the
// header comment block to new standard
//
// 3     3/04/04 5:07p Cward
// added m_logger to Forward Component Proxy instantiation
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 8     10/24/03 10:52a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 3:41p Khykin
// Updated with Brians latest changes.
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 6     8/14/03 10:37a Khykin
// Updated with make install changes.
//
// 3     8/12/03 2:45p Khykin
// Updated with new directory update changes.
//
// 2     8/06/03 2:57p Khykin
// Updating with latest changes from Flatrock.
//
// 2     6/24/03 7:09a Khykin
// Updated to better handle a component sig seg for displaying the status
// to the user.
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
//    Revision 1.1.1.1  2003/02/15 00:03:10  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:16a Khykin
// Initial Checkin
//
// 13    11/11/02 2:59p Khykin
// Updated diagnostic information.
//
// 12    9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 11    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 10    3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 9     3/06/02 1:48p Kh
// Updating the handling of bad processes.
//
// 8     12/20/01 10:19a Kh
// Updating the adding of the thread to the list.
//
// 7     11/13/01 4:10p Kh
//
// 6     8/28/01 6:56p Kh
// Checking in for OS conversion back to 6.0.
//
// 5     8/17/01 2:36p Kh
// Checking in for weekly backup.
//
// 3     8/02/01 4:29p Kh
// Working On Issues Detected During Testing.
//
// 1     8/02/01 4:23p Kh
// Component Manager responsible for managing all of the components.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================

#include "ComponentManager.h"
#include "INamedDataBroker.h"

// Forward declaration of the component proxy thread.
extern "C"  void *ComponentProxyThread(void *arg);

////////////////////////////////////////////////////////////////////////////
//  Component List here
////////////////////////////////////////////////////////////////////////////

ComponentList::ComponentList() : COMPONENTLIST()
{
}

ComponentList::~ComponentList()
{
    Clear();
}

int ComponentList::AddComponent( const std::string &compName, ComponentProxy *compProxy)
{
    int status;
    // Only add the component to the list if it does not already exist
    if( (status=Lock()) == EOK)
    {
        if (FindComponent(compName) == NULL)
        {
        COMPONENTLIST::insert( COMPONENTVAL( compName, compProxy));
        }
    else
    {
        status = BEP_STATUS_ERROR;
    }
        Unlock();
    }

    return( status);
}

int ComponentList::RemoveComponent( const std::string &compName)
{
    int     status;

    if( (status=Lock()) == EOK)
    {
        ComponentProxy *proxy = FindComponent(compName);
        if (proxy != NULL) delete proxy;
        proxy = NULL;
        COMPONENTLIST::erase( compName);

        Unlock();
    }

    return( status);
}

ComponentProxy* ComponentList::FindComponent( const std::string &compName)
{
    ComponentProxy* compProxy=NULL;
    COMPONENTCITR   itr;
    int             status;

    if( (status=Lock()) == EOK)
    {
        if( (itr=COMPONENTLIST::find( compName)) != COMPONENTLIST::end())
        {
            compProxy = itr->second;
        }

        Unlock();
    }

    return( compProxy);
}

int ComponentList::Clear()
{
    int             status;

    if( (status=Lock()) == EOK)
    {
        for (COMPONENTITR iter = begin(); iter != end(); iter++)
        {
            ComponentProxy *proxy = iter->second;
            if (proxy != NULL) delete proxy;
            proxy = NULL;
        }
        COMPONENTLIST::clear();

        Unlock();
    }

    return( status);
}

int ComponentList::Lock()
{
    return( m_listLock.Acquire());
}

int ComponentList::Unlock()
{
    return( m_listLock.Release());
}


////////////////////////////////////////////////////////////////////////////
//  Component Manager starts here
////////////////////////////////////////////////////////////////////////////


ComponentManager::ComponentManager(): m_logger(NULL), m_reuseComponents (false)
{
};

ComponentManager::~ComponentManager()
{   // remove all of the proxies and processes
    Log(LOG_FN_ENTRY, "~ComponentManager()\n");
    // if there are any components to terminate
    if (m_compList.size() > 0)
    {
        Log(LOG_DEV_DATA, "Terminating Remaining Components()\n");
        TerminateAll();
    }
    Log(LOG_FN_ENTRY, "~ComponentManager() done\n");
};

void ComponentManager::Initialize(const XmlNode *initInfo, string driveCurveRootDir, 
                                  bool reuseComponents)
{
    m_reuseComponents = reuseComponents;
    m_driveCurveRootDir = driveCurveRootDir;
    LaunchComponents(initInfo);
}

void ComponentManager::LaunchComponents(const XmlNode *compList)
{
    try
    {
        Log(LOG_DEV_DATA, "Launching Components\n");
        RemoveComponents();         // kill all components
        Log(LOG_DEV_DATA, "Components Removed\n");
        Log(LOG_DETAILED_DATA, "MEM: ComponentManager::LaunchComponents()1 - %d\n", ReturnMemArena());
        AddComponents(compList);    // launch the new ones
        Log(LOG_DEV_DATA, "Components Added\n");
    }
    catch (BepException &e)
    {
        try
        {
            RemoveComponents();
        }
        catch (...)
        {
        }
        throw BepException("Launching Components Exception: %s\n", e.what());;
    }
};

void ComponentManager::CommandObjective(const XmlNode *objective)
{
    // Get the short, convenient name for this component
    std::string shortName = objective->getAttributes().getNode("Component")->getValue();
    // Get the unique name for this component
    std::string compName = m_compNameMap[ shortName];
    
    Log(LOG_FN_ENTRY, "Commanding %s\n", objective->getName().c_str());
    // get the component proxy
    ComponentProxy *cp = LookupComponent(compName);
    // command the component proxy to send the command
    if (cp == NULL) // ensure that it is valid
    {
        throw BepException("ComponentProxy NULL Could Not Command %s->%s",
                           shortName.c_str(), compName.c_str());
    }
    cp->CommandObjective(objective);
};

void ComponentManager::CommandReset(const std::string componentName)
{   // command the component to Reset
    Log(LOG_FN_ENTRY, "CommandReset %s\n", componentName.c_str());
    // get the component proxy
    ComponentProxy *cp = LookupComponent(componentName);
    // command the component proxy to send the command
    if (cp == NULL) // ensure that it is valid
    {
        throw BepException("ComponentProxy NULL Could Not Command Reset");
    }
    cp->CommandReset();
};

void ComponentManager::CommandTerminate(const std::string componentName)
{   // command the component to Termiante
    Log(LOG_FN_ENTRY, "CommandTerminate %s\n", componentName.c_str());
    ComponentProxy *cp = NULL;
    struct sigevent unblockEvent;
    uint64_t    joinTime = mSEC_nSEC( 1000LL);

    SIGEV_UNBLOCK_INIT( &unblockEvent);

    try
    {   // get the component proxy
        cp = LookupComponent(componentName);
        // command the component proxy to send the command
        if (cp == NULL) // ensure that it is valid
        {
            throw BepException("CommandTerminate ComponentProxy NULL For: %s", componentName.c_str());
        }
        cp->CommandTerminate();

        INT32 status;                       // status flag
        INT32 retry = 3;                    // set the retry counter to 3
        do
        {   // try to join the thread
            Log(LOG_DEV_DATA, "Waiting for %s thread to die: %d\n", componentName.c_str(), (int) cp->GetThreadID());

            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_JOIN, &unblockEvent, &joinTime, NULL);
            status = pthread_join(cp->GetThreadID(), NULL);
            // if timed out joining
            if (status == ETIMEDOUT)
            {
                // cancel and kill the thread
                Log(LOG_DEV_DATA, "Join for %s, %d timed out, attempting to cancel and kill\n", componentName.c_str(), (int) cp->GetThreadID());
                pthread_cancel( cp->GetThreadID());
                Log(LOG_DEV_DATA, "Canceled\n");
                BposSleep(100); // give time to end
                Log(LOG_DEV_DATA, "Trying Again\n");
            }
            // else if an interupt happened joining, reset the counter
            else if (status == EINTR)
            {
                retry = 3;
                Log(LOG_DEV_DATA, "Interrupted attempting to join for %s, %d timed out, attempting to cancel and kill\n", componentName.c_str(), cp->GetThreadID());
            }
            // while the thead is not joined and a full three seconds has not passed
        } while ((status != EOK) && (retry--));

        if (status != EOK)
        {
            // cancel and kill the thread
            Log(LOG_ERRORS, "Could not join thread %s (%d) timed out, attempting to cancel and kill\n", componentName.c_str(), (int) cp->GetThreadID());
            pthread_cancel( cp->GetThreadID());
            Log(LOG_DEV_DATA, "Cancelled\n");
            pthread_kill( cp->GetThreadID(), SIGTERM);
            Log(LOG_DEV_DATA, "Killed\n");
            BposSleep(100); // give time to end

            joinTime = mSEC_nSEC( 5000LL);
            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_JOIN, &unblockEvent, &joinTime, NULL);
            if ( (status = pthread_join(cp->GetThreadID(), NULL)) != EOK)
            {
                throw BepException("Could Not Join %s Thread: %s", cp->GetName().c_str(), strerror(errno));
            }
            else
            {
                Log(LOG_DEV_DATA, "Forced thread %s (%d) to die\n", componentName.c_str(), (int) cp->GetThreadID());
            }
        }
        else
            Log(LOG_DEV_DATA, "Thread %s (%d) terminated successfully\n", componentName.c_str(), (int) cp->GetThreadID());

        int tid = cp->GetThreadID();
        // must delete the component proxy
        Log(LOG_DEV_DATA, "Thread %s is dead: %d\n", componentName.c_str(), tid);
        // Delete the component proxy from the map
        if (m_compList.RemoveComponent(componentName) != EOK)
        {
            Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", componentName.c_str());
            if (cp != NULL)
                delete cp;
            else
                Log(LOG_ERRORS, "ComponentProxy for %s is NULL", componentName.c_str());
            cp = NULL;
        }
        Log(LOG_DEV_DATA, "ComponentProxy For Thread %s is dead\n", componentName.c_str());
    }
    catch (XmlException &e)
    {   // delete the component if it exists
        if (m_compList.RemoveComponent(componentName) != EOK)
        {
            Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", componentName.c_str());
            if (cp != NULL)
                delete cp;
            else
                Log(LOG_ERRORS, "ComponentProxy for %s is NULL", componentName.c_str());
        cp = NULL;
        }
        throw BepException("CommandTerminate, XmlException: %s\n", e.Reason().c_str());
    }
    catch (BepException &e)
    {
        if (m_compList.RemoveComponent(componentName) != EOK)
        {
            Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", componentName.c_str());
            if (cp != NULL)
                delete cp;
            else
                Log(LOG_ERRORS, "ComponentProxy for %s is NULL", componentName.c_str());
            cp = NULL;
        }
        throw BepException("CommandTerminate, BepException: %s\n", e.Reason().c_str());
    }
    catch (...)
    {
        if (m_compList.RemoveComponent(componentName) != EOK)
        {
            Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", componentName.c_str());
            if (cp != NULL)
                delete cp;
            else
                Log(LOG_ERRORS, "ComponentProxy for %s is NULL", componentName.c_str());
            cp = NULL;
        }
        throw BepException("CommandTerminate, Unknown exception\n");
    }
    Log(LOG_FN_ENTRY, "CommandTerminate(%s) complete\n", componentName.c_str());
};

void ComponentManager::CommandAbort(const std::string componentName)
{   // command the component to Abort
    Log(LOG_FN_ENTRY, "CommandAbort %s\n", componentName.c_str());
    // get the component proxy
    ComponentProxy *cp = LookupComponent(componentName);
    // command the component proxy to send the command
    if (cp == NULL) // ensure that it is valid
        throw BepException("ComponentProxy NULL Could Not Command Abort");
    cp->CommandAbort();
};

void ComponentManager::CommandDeactivate(const std::string componentName)
{   // command the component to Abort
    Log(LOG_FN_ENTRY, "CommandingDeactivate %s\n", componentName.c_str());
    // get the component proxy
    ComponentProxy *cp = LookupComponent(componentName);
    // command the component proxy to send the command
    if(cp == NULL) // ensure that it is valid
        throw BepException("ComponentProxy NULL Could Not Command Deactivate");
    cp->CommandDeactivate();
};

void ComponentManager::CommandReloadConfiguration(const std::string componentName)
{   // command the component to ReloadConfiguration
    Log(LOG_FN_ENTRY, "CommandReloadConfiguration %s\n", componentName.c_str());
    // get the component proxy
    ComponentProxy *cp = LookupComponent(componentName);
    // command the component proxy to send the command
    if (cp == NULL) // ensure that it is valid
        throw BepException("ComponentProxy NULL Could Not Command ReloadConfiguration");
    cp->CommandReloadConfiguration();
};

INT32 ComponentManager::AbortAll(void)
{
    // Abort fault
    std::string fault;
    int errorNumber = 0;
    ComponentProxy *cp;     // component proxy reference
    char    buffer[ 256];
    ComponentNameMapItr_t iter;  //iterator to walk through list of active components

    for (iter=m_compNameMap.begin(); iter != m_compNameMap.end(); iter++)
    {
        try
        {
            // Get the short name for this component
            std::string compName = (*iter).second;
            cp = LookupComponent(compName);        // retrieve the component proxy
            if (cp != NULL)             // if a valid component
                cp->CommandAbort();     // command an Abort
        }
        catch (BepException &e)
        {   // record the error
            errorNumber++;     // increment the count
            fault += CreateMessage(buffer, sizeof(buffer), "Error%d: %s, ", errorNumber, e.what());
            Log(LOG_ERRORS, buffer);
        }
    }

#if 0
    COMPONENTITR iter;      // iterator to walk through the list of components
    // Lock the component list while we abort all the components
    if( (errorNumber=m_compList.Lock()) == EOK)
    {
        errorNumber = 0;
    // walk through the list and send an abort message
    // to all of the components
        for(iter=m_compList.begin(); iter != m_compList.end(); iter++)
        {
            try
            {
                cp = (*iter).second;        // retrieve the component proxy
                if (cp != NULL)             // if a valid component
                    cp->CommandAbort();     // command an Abort
            }
            catch (BepException &e)
            {   // record the error
                errorNumber++;     // increment the count
                fault += CreateMessage(buffer, sizeof(buffer), "Error%d: %s, ", errorNumber, e.what());
                Log(LOG_ERRORS, buffer);
            }
        }

        m_compList.Unlock();
    }
    else
    {
        fault = strerror( errorNumber);
        Log( LOG_ERRORS, "Error locking component list in AbortAll(): %s\n", fault.c_str());
    }
#endif 
    // if one or more faults occurred during the abort sequence
    if (errorNumber)
        throw BepException("ComponenetManager Error(s) during AbortAll: %s\n", fault.c_str());

    return(BEP_STATUS_SUCCESS);
};

INT32 ComponentManager::ReloadConfigurationAll(void)
{
    ComponentProxy *cp;     // component proxy reference
    int status = BEP_STATUS_SUCCESS;
    ComponentNameMapItr_t iter;  //iterator to walk through list of active components

    for (iter=m_compNameMap.begin(); iter != m_compNameMap.end(); iter++)
    {
        // Get the short name for this component
        std::string compName = (*iter).second;
        cp = LookupComponent(compName);        // retrieve the component proxy
        if (cp != NULL)                         // if a valid component
            cp->CommandReloadConfiguration();   // command to Reload its Config File
    }


#if 0

    COMPONENTITR iter;      // iterator to walk through the list of components
    // Lock the component list while we abort all the components
    if( (status=m_compList.Lock()) == EOK)
    {
    // walk through the list and send an abort message
    // to all of the components
        for(iter=m_compList.begin(); iter != m_compList.end(); iter++)
        {
            cp = (*iter).second;                    // retrieve the component proxy
            if (cp != NULL)                         // if a valid component
                cp->CommandReloadConfiguration();   // command to Reload its Config File
    }

        m_compList.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking component list in ReloadConfigurationAll(): %s\n", strerror( status));
        status = BEP_STATUS_ERROR;
    }

#endif 

    return( status);
};

INT32 ComponentManager::ResetAll(void)
{
    INT32 status=BEP_STATUS_SUCCESS;    // status of the command
    ComponentProxy *cp;                 // component proxy reference
    ComponentNameMapItr_t iter;  //iterator to walk through list of active components

    // create an iterator to walk through the list of active components
    for ( iter=m_compNameMap.begin(); iter!=m_compNameMap.end(); iter++)
    {
        //Moved Try Catch inside loop - this way all components get pulse
        try
        {
            // walk through the list and send an abort message
            // to all of the components
            // Get the short name for this component
            std::string compName = (*iter).second;
            cp = LookupComponent(compName);        // retrieve the component proxy
            Log(LOG_DEV_DATA, "Resetting: %s\n", cp->GetMyName().c_str());
            if (cp != NULL)             // if a valid component
                cp->CommandReset();     // command to Reset the component
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "ResetAll BepException: %s\n", e.Reason().c_str());
            status = BEP_STATUS_ERROR;
        }
    }

#if 0

    COMPONENTITR iter;

        // create an iterator to walk through the list of components
    if( (status=m_compList.Lock()) == EOK)
    {
        status = BEP_STATUS_SUCCESS;
        for ( iter=m_compList.begin(); iter!=m_compList.end(); iter++)
        {
            //Moved Try Catch inside loop - this way all components get pulse
        try
        {
        // walk through the list and send an abort message
        // to all of the components
            cp = (*iter).second;        // retrieve the component proxy
            Log(LOG_DEV_DATA, "Resetting: %s\n", cp->GetMyName().c_str());
            if (cp != NULL)             // if a valid component
                cp->CommandReset();     // command to Reset the component
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "ResetAll BepException: %s\n", e.Reason().c_str());
            status = BEP_STATUS_ERROR;
        }
        }

        m_compList.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking component list in ResetAll(): %s\n", strerror( status));
        status = BEP_STATUS_ERROR;
    }
#endif 

    return(status);
};

INT32 ComponentManager::TerminateAll(bool forceTerminate)
{
    INT32   status = BEP_STATUS_SUCCESS;
    bool    done = false;

    Log(LOG_FN_ENTRY, "Entering Component Manager TerminateAll()\n");
    Log(LOG_DETAILED_DATA, "MEM: ComponentManager::TerminateAll()1 - %d\n", ReturnMemArena());
    if( (status=m_compList.Lock()) == EOK)
    {
        // Make sure we have components before we attempt to remove them
        if (m_compList.size() > 0)
        {
            // create an iterator to walk through the list of components
            COMPONENTITR iter = m_compList.begin();
            // walk through the list and terminate everyone
            Log(LOG_DEV_DATA, "Terminating %d old components\n", (INT32)m_compList.size());
            while( false == done)
            {
                Log(LOG_DETAILED_DATA, "MEM: ComponentManager::TerminateAll()2 while != Empty - %d\n", ReturnMemArena());
                try
                {
                    // If not using activate/deactivate OR if shutting down
                    if( (false == m_reuseComponents) || (true == forceTerminate))
                    {
                        iter = m_compList.begin();
                        const std::string compName(iter->first);

                        // walk through the list and remove all of the components
                        Log(LOG_DEV_DATA, "Terminating: %s\n", compName.c_str());
                        CommandTerminate(compName);
                        Log(LOG_DEV_DATA, "Terminated: %s\n", compName.c_str());
                    }
                    else
                    {
                        const std::string compName(iter->first);
                        iter++;
                        
                        // walk through the list and remove all of the components
                        Log(LOG_DEV_DATA, "Deactivating: %s\n", compName.c_str());
                        try
                        {
                            CommandDeactivate(compName.c_str());
                        }
                        catch ( BepException &err)
                        {
                            int taskId = BposGetTaskId( compName.c_str());
                            if ( -1 == taskId)
                            {
                                Log( LOG_ERRORS, "Component %s is no longer running!!!\n", compName.c_str());
                                // Advance the iterator because CommandTerminate() will remove the
                                //  component from the list and we need to prcess the next item
                                iter++;
                                CommandTerminate(compName);
                            }
                            else
                            {
                                throw;
                            }
                        }
                        Log(LOG_DEV_DATA, "Deactivated: %s\n", compName.c_str());
                    }
                    // Give some time before killing the next component
                    BposSleep(100);
                }
                catch (XmlException &e)
                {
                    Log(LOG_ERRORS, "TerminateAll XmlException: %s\n", e.Reason().c_str());
                    status = BEP_STATUS_ERROR;
                    BposSleep(10);
                }
                catch (BepException &e)
                {
                    Log(LOG_ERRORS, "TerminateAll BepException: %s\n", e.Reason().c_str());
                    status = BEP_STATUS_ERROR;
                    BposSleep(10);
                }
                catch (...)
                {
                    Log(LOG_ERRORS, "TerminateAll Unknown Exception\n");
                    status = BEP_STATUS_ERROR;
                    BposSleep(10);
                }
                // If not using activate/deactivate OR if shutting down
                if ( (false == m_reuseComponents) || (true == forceTerminate))
                {
                    // Leave loop when no more components are in the list
                    done = m_compList.empty();
                }
                else
                {
                    // Leave loop when we have processed all components in the list
                    done = (iter == m_compList.end());
                }
            }
            Log(LOG_DETAILED_DATA, "Done terminating the components. m_comList.size == %d", m_compList.size());
            if( false == m_reuseComponents)
            {
                // Clear the component map
                m_compList.Clear();
            }
            Log(LOG_DETAILED_DATA, "Done clearing the component list. m_comList.size == %d", m_compList.size());
            Log(LOG_DETAILED_DATA, "MEM: ComponentManager::TerminateAll()3 After clear()- %d\n", ReturnMemArena());
        }
    else
    {   // No components to terminate
        Log(LOG_DEV_DATA, "No components to terminate\n");
    }

        m_compList.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "Error locking component list in TerminateAll(): %s\n", strerror( status));
        status = BEP_STATUS_ERROR;
    }
    return(status);
};

ComponentProxy *ComponentManager::LookupComponent(const std::string &component)
{
    ComponentProxy *cp;
    if( (cp = m_compList.FindComponent( component)) == NULL)
    {
        throw BepException("Could Not Find DriveCurve Component Named: %s", component.c_str());
    }

    return(cp);
};

void ComponentManager::AddComponents(const XmlNode *compList)
{
    ComponentProxy *cp=NULL;    // component proxy reference
    int status;
    std::string compName;
    std::string name;
    bool        reuseComponent = false;
    XmlNodeMap components;
    components.DeepCopy(compList->getChildren());

    Log(LOG_FN_ENTRY, "ComponentManager::AddComponents() enter\n");

    if( (status=m_compList.Lock()) == EOK)
    {
        m_compNameMap.clear();
        for (XmlNodeMapCItr iter = components.begin(); iter != components.end(); iter++)
        {
            const XmlNode *compNode = iter->second;
            try
            {
                Log(LOG_DETAILED_DATA, "MEM: ComponentManager::AddComponents()1 - %d\n", ReturnMemArena());
                const std::string shortComponentName( compNode->getName());
                //name = compNode->getName();
                compName = compNode->getChild("ComponentName")->getValue();

                GetComponentIpcName( compNode, name);
                // Add a name mapping entry for this component
                m_compNameMap.insert( ComponentNameMapVal_t( shortComponentName, name));

                // Assume we need to launch a new component
                reuseComponent = false;

                // See if we have a ComponentProxy already running for this component
                try
                {
                    cp = LookupComponent(name);
                }
                catch( ...)
                {
                    cp = NULL;
                }
                if( NULL == cp)
                {
                // create a new foreground component proxy
                    Log(LOG_DEV_DATA, "Adding: %s\n", compName.c_str());
                    cp = new ForegroundComponentProxy(m_logger);
                    if(m_compList.AddComponent(name, cp) != EOK)
                    {
                        Log(LOG_ERRORS, "Could not add component: %s - deleting component", name.c_str());
                        if( NULL != cp)
                        {
                            delete cp;
                            cp = NULL;
                        }
                    }
                }
                else
                {
                    // Say we can re-activate the existing component
                    reuseComponent = true;
                }
                Log(LOG_DETAILED_DATA, "MEM: ComponentManager::AddComponents()2 - %d\n", ReturnMemArena());

                // If we have a component proxy for this component
                if(NULL != cp)
                {
                    // If this is a new component to launch
                    if( false == reuseComponent)
                    {
                        // Try to initialize the component
                        Log(LOG_DEV_DATA, "Initializing Component: %s\n", compName.c_str());
                        cp->Initialize(iter->second, m_driveCurveRootDir, m_reuseComponents);
                        Log(LOG_DETAILED_DATA, "MEM: ComponentManager::AddComponents()3 - %d\n", ReturnMemArena());
                        // create a thread to manage the component proxy
                        pthread_t id;           // the id of the thread
                        pthread_attr_t attr;    // the attribute of the thread
                        pthread_attr_init( &attr );
                        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
                        Log(LOG_DEV_DATA, "Launching Component: %s\n", compName.c_str());
                        if (pthread_create( &id, &attr, ComponentProxyThread, (void *) cp) != EOK)
                        {
                            throw BepException("Could Not Create Thread: %s",
                                               iter->second->getChild("ComponentName")->getValue().c_str());
                        }
                        cp->SetThreadID(id);    // Set the threads ID
                    }
                    else
                    {
                        // Reuse the existing component and proxy
                        cp->CommandActivate( shortComponentName);
                    }
                BposSleep(100);
            }
            }
            catch (XmlException &e)
            {//Remove component from list and delete cp
                Log(LOG_ERRORS, "Manager Caught XmlException: %s\n", e.what());
                if (m_compList.RemoveComponent(name) != EOK)
                {
                    Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", name.c_str());
                    if (cp != NULL)
                        delete cp;
                    else
                        Log(LOG_ERRORS, "ComponentProxy for %s is NULL", name.c_str());
                    cp = NULL;
                }
                m_compList.Unlock();
                throw BepException("AddComponents XmlException %s", e.Reason().c_str());
            }
            catch (BepException &e)
            {//Remove component from list and delete cp
                Log(LOG_ERRORS, "Manager Caught BepException: %s\n", e.what());
                if (m_compList.RemoveComponent(name) != EOK)
                {
                    Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", name.c_str());
                    if (cp != NULL)
                        delete cp;
                    else
                        Log(LOG_ERRORS, "ComponentProxy for %s is NULL", name.c_str());
                    cp = NULL;
                }
                m_compList.Unlock();
                throw BepException("AddComponents BepException %s", e.Reason().c_str());
            }
            catch (...)
            {//Remove component from list and delete cp
                if (m_compList.RemoveComponent(name) != EOK)
                {
                    Log(LOG_DEV_DATA, "Could not remove ComponentProxy for %s", name.c_str());
                    if (cp != NULL)
                        delete cp;
                    else
                        Log(LOG_ERRORS, "ComponentProxy for %s is NULL", name.c_str());
                    cp = NULL;
                }
                m_compList.Unlock();
                throw;
            }
        }
        m_compList.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "Error locking component list in AddComponents(): %s\n", strerror( status));
    }
    components.clear(true);
    Log(LOG_FN_ENTRY, "ComponentManager::AddComponents() exit\n");
};

void ComponentManager::GetComponentIpcName( const XmlNode *compNode, std::string &ipcName)
{
    // Get the path to the component executable
    std::string compPath( compNode->getChild("ComponentPath")->getValue());
    if( compPath[ 0] != '/')    compPath = std::string("/") + compPath;
    if( compPath[compPath.size()-1] != '/') compPath.append( "/");
    compPath = std::string("$(FTP_ROOT)") + compPath;

    // Get the path to the component config file
    std::string cfgPath( compNode->getChild( "ConfigPath")->getValue());
    if( cfgPath[ 0] != '/') cfgPath = std::string("/") + compPath;
    if( cfgPath[cfgPath.size()-1] != '/')   cfgPath.append( "/");
    cfgPath = std::string("$(FTP_ROOT)") + cfgPath;

    // Build the full path to the config file
    std::string cfgFile( cfgPath + compNode->getChild( "ConfigFile")->getValue());
    // Build the full path to the executable
    std::string compQxe( compPath + compNode->getChild( "ComponentName")->getValue());

    // Expand environment variables in the paths
    FixUpPathString( cfgFile);
    FixUpPathString( compQxe);

    // Create the "special" IPC channel name that this component will use to register
    ipcName = CreateUniqueIpcName( compQxe, cfgFile);
}

INT32 ComponentManager::RemoveComponents(void)
{
    Log(LOG_FN_ENTRY, "RemoveComponents()\n");
    // terminate all until a speed issue is recoginzed and then
    // code a more efficient algorythm
    INT32 status = TerminateAll();
    // Need to wait for all components to die before moving on
    BposSleep(2000);
    return(status);
};

void ComponentManager::SetLoggingInterface(ILogger *loggingInterface)
{
    m_logger = loggingInterface;
};

void ComponentManager::Log( UINT32 mask, const char *message, ...)
{
    if(m_logger)    
    {
        char    logMssg[1024];
        va_list vaArgs;

        // parse out the varaible argument message
        // clear the print buffer
        memset(logMssg, 0 , sizeof(logMssg));
        // format the input into a String
        va_start( vaArgs, message);
        // Limit the size of the message so that it fits into the buffer (else SIGSEGV)
        vsnprintf(logMssg, sizeof(logMssg)-1, (char *)message, vaArgs);
        // remove left overs
        va_end( vaArgs);

        m_logger->Log(mask, logMssg);
    }
};

int ComponentManager::ReturnMemArena()
{
    struct mallinfo mInfo;
    mInfo = mallinfo();
    return(mInfo.arena);
}

//extern "C" void *ComponentProxyThread(void *arg);
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
    void *ComponentProxyThread(void *arg)
    {
        ComponentProxy *cp;
        string logMessage;
        try
        {
            cp = (ComponentProxy *) arg;
            if (cp == NULL)
                throw BepException("Invalid Component Proxy Installed In Thread");
            // process the objective that has been commanded
            char buff[64];
            logMessage = "Component Thread Started: " + cp->GetName() + itoa(cp->GetThreadID(), buff, 10);
            cp->Log(LOG_FN_ENTRY, logMessage.c_str());
            cp->ProcessObjective();
            logMessage = "Component Thread Terminated: " + cp->GetName();
            cp->Log(LOG_FN_ENTRY, logMessage.c_str());
        }
        catch (BepException &e)
        {
            logMessage = cp->GetName() + "Thread BepException: " + e.Reason();
            cp->Log(LOG_ERRORS, logMessage.c_str());
        }
        catch (...)
        {
            logMessage = cp->GetName() + "Thread Unknown Exception";
            cp->Log(LOG_ERRORS, logMessage.c_str());
        }
        return(NULL);
    };
};


