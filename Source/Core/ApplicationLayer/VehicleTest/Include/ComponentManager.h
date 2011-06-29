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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/ComponentManager.h $
// 
// 7     10/25/06 4:16p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize() and added a 
//       "forceTerminate" variable to TerminateAll().  Added
// CommandDeactivate() and GetComponentIpcName().  These are used       to
// keep the test components running when a new drive curve starts.
// 
// 6     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 5     7/21/04 11:18a Cward
// Updated forward class references and removed freind class references.
//
// 4     6/09/04 11:21p Cward
// Refactored App layer includes to remove implimentation. Updated the
// header comment block to new standard.
//
// 3     3/04/04 5:12p Cward
// added logger
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 2     8/06/03 10:07a Khykin
// Updated with changes from Flatrock.
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
// 1     1/27/03 11:09a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 6     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 5     3/06/02 1:54p Kh
// Updating includes.
//
// 4     8/24/01 3:24p Kh
//
// 3     8/17/01 2:35p Kh
// Checking in for weekly backup.
//
// 2     7/26/01 7:48p Kh
// Completed testing, ready to go.
//
// 1     7/06/01 5:35p Kh
// Just checking in for the weekend.  Will debug next week.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H
//==============================================================================
using namespace std;

#include "GeneralUtilities.h"   // general system utilities
#include "BepException.h"       // base exception definition
#include "Status.h"
#include "Debuggable.h"
#include "BepDefs.h"
#include "SystemSetup.h"
#include "Bepos.h"
#include "XmlParser.h"
#include "XmlNode.h"
#include "Message.h"
#include "CmdLineProcessor.h"
#include "NotificationList.h"
#include "IBepCommunication.h"
#include "ForegroundComponentProxy.h"
#include "BackgroundComponentProxy.h"
#include "ILogger.h"
#include <map>
#include <string>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

/**
 * The list of components and thier references needed to command them.
 */
typedef map< std::string, ComponentProxy *, less<string> >  COMPONENTLIST;
/**
 * An iterator used to navagate through the components to find
 * the reference to use to command the component.
 */
typedef COMPONENTLIST::iterator                     COMPONENTITR;
/**
 * A constant iterator used to navagate through the components to find
 * the reference to use to command the component.
 */
typedef COMPONENTLIST::const_iterator               COMPONENTCITR;
/**
 * A table value operator used for accessing component list references.
 */
typedef COMPONENTLIST::value_type                   COMPONENTVAL;

typedef std::map< std::string, std::string>     ComponentNameMap_t;
typedef ComponentNameMap_t::iterator            ComponentNameMapItr_t;
typedef ComponentNameMap_t::const_iterator      ComponentNameMapCItr_t;
typedef ComponentNameMap_t::value_type          ComponentNameMapVal_t;

/**
 * Object used to maintain a list of component (proxies) that the ComponentManager
 * manages
 *
 * @author Brian Meinke
 * @version 1.0
 * @since Dec 03, 2003
 */
class ComponentList : public COMPONENTLIST
{
public:
	/**
	 * Constructor
	 */
	ComponentList();

	/**
	 * Destructor
	 */
	~ComponentList();

	/**
	 * Adds a component proxy named compName to the list
	 *
	 * @param compName  Name of the component being added to the list
	 * @param compProxy Reference to the component proxy used to manage the component
	 *
	 * @return EOK if component successfully added
	 */
	int AddComponent( const std::string &compName, ComponentProxy *compProxy);
	/**
	 * Removes a component proxy named compName from the list
	 *
	 * @param compName  Name of the component being removed from the list
	 *
	 * @return EOK if component successfully removed
	 */
	int RemoveComponent( const std::string &compName);

	/**
	 * Attempts to find a component named compName in the component list
	 *
	 * @param compName Name of the component to find
	 *
	 * @return Pointer to the component if found. NULL if no component is found
	 */
	ComponentProxy* FindComponent( const std::string &compName);

	/**
	 * Clears all entries from the list
	 *
	 * @return EOK if the list was successfully cleared
	 */
	int Clear();

	/**
	 * Locks the list while operating on it
	 *
	 * @return EOK if successfully locked
	 */
	int Lock();

	/**
	 * Unlocks the list when done operating on it
	 *
	 * @return EOK if successfully locked
	 */
	int Unlock();

private:
	mutable BepMutex    m_listLock;
};

/**
 * Component Manager class.  This class is responsible
 * for launching vehicle components and providing the
 * methods to be able to comand thoes components to
 * perform the specified test objectives.
 *
 * @author Kevin Hykin
 * @version 1.1
 */
class ComponentManager
{
public:
	/**
	 * Vehicle Component Manager constructor.
	 * @since Version 1.0
	 */
	ComponentManager(void);

	/**
	 * Vehicle Component Manager destructor.
	 * @since Version 1.0
	 */
	virtual ~ComponentManager();

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
	 * Command the specified component to Abort.
	 *
	 * @param componentName
	 *               The name of the component to send the command to.
	 * @since Version 1.0
	 */
	void CommandAbort(const std::string componentName);

	/**
	 * Command the specified component to deactivate.
	 *
	 * @param componentName
	 *               The name of the component to send the command to.
	 */
	void CommandDeactivate(const std::string componentName);
    
	/**
	 * Command the specified component to Reload its
	 * configuration.
	 *
	 * @param componentName
	 *               The name of the component to send the command to.
	 * @since Version 1.0
	 */
	void CommandReloadConfiguration(const std::string componentName);

	/**
	 * Command the specified component to Reset.
	 *
	 * @param componentName
	 *               The name of the component to send the command to.
	 * @since Version 1.0
	 */
	void CommandReset(const std::string componentName);

	/**
	 * Command the specified component to Terminate.
	 *
	 * @param componentName
	 *               The name of the component to send the command to.
	 * @since Version 1.0
	 */
	void CommandTerminate(const std::string componentName);

	/**
	 * Command all of the components to Abort.
	 *
	 * @return The status of the command.
	 * @since Version 1.0
	 */
	INT32 AbortAll(void);

	/**
	 * Command all of the components to Reload their
	 * configuration files.
	 *
	 * @return The status of the command.
	 * @since Version 1.0
	 */
	INT32 ReloadConfigurationAll(void);

	/**
	 * Command all of the components to Reset.
	 *
	 * @return The status of the command.
	 * @since Version 1.0
	 */
	INT32 ResetAll(void);

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
	 *  Debug function to return memory usage
	 */
	int ReturnMemArena(void);
	/**
	 * Set the logging interface for the manger to log test
	 * information
	 *
	 * @param loggingInterface The interface to use.
	 * @since Version 1.1
	 */
	void SetLoggingInterface(ILogger *loggingInterface);

	/**
	 * Sends a message to the logger using the loggine interface.
	 *
	 * @param mask    Logger verbosity mask
	 * @param message The message to send
	 * @since Version 1.1
	 */
	void Log( UINT32 mask, const char *message, ...);
	/**
	 * Remove all of the components currently being managed.
	 * It will look up each component, and then use the
	 * component proxy to command the component to terminate.
	 *
	 * @return The success or failure of the removal.
	 * @since Version 1.0
	 */
	virtual INT32 RemoveComponents(void);

protected:
	/**
	 * Method that launches the components that will be managed.
	 * Each time the method is called, it will deterimine what
	 * is running, and then determine what needs to be started
	 * and terminated to perform the next test.
	 *
	 * @param compList The list of components that need to be managed.  The
	 *                 initalize method will manage a component proxy that
	 *                 will launch the requested component and become an
	 *                 interface to that component.
	 */
	virtual void LaunchComponents(const XmlNode *compList);

	/**
	 * Retrieve a component proxy reference. This is used
	 * to look up a component proxy by component name and
	 * then allows the requestor to command the component.
	 *
	 * @param component The component name to return the proxy of.
	 * @return The component proxy that is connected with the
	 *         specified component.
	 * @since Version 1.0
	 */
	ComponentProxy *LookupComponent(const std::string &component);

	/**
	 * Add a list of components to the component list.  For each
	 * component specified, a component proxy will be created and
	 * a thread launched.
	 *
	 * @param compList The component list that should be launched and added to
	 *                 the system.
	 * @since Version 1.0
	 */
	void AddComponents(const XmlNode *compList);

	/**
	 * Gets a components re-usable IPC channel name from the component's 
	 * drive curve node (name,config file, paths, etc)
	 * 
	 * @param compNode The XML node in the drive curve that describes the runtime component
	 * @param ipcName  The name that the component will register for IPC messages
	 */
	void GetComponentIpcName( const XmlNode *compNode, std::string &ipcName);

	/**
	 * Component list that contains each ComponentProxy
	 * reference which can be looked up by name.
	 * @since Version 1.0
	 */
	ComponentList m_compList;
	/**
	 * Reference to the logging interface for logging purposes.
	 * @since Version 1.1
	 */
	ILogger *m_logger;

	/**
	 * Flag indicating if we want to re-use existing test components instead of
	 * always killing/relaunching them
	 */
	bool m_reuseComponents;

	/**
	 * Mapping of "nice" component name to a unique IPC channel name
	 */
	ComponentNameMap_t m_compNameMap;

	/**
	 * The type of component that is running.
	 * @since Version 1.2
	 */
	std::string m_driveCurveRootDir;
};

#endif
