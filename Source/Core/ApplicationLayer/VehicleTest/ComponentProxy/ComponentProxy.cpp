//*************************************************************************
// Description:
//  Vehicle Component Proxy that is responsible for handling the sequencing
//  of test objectives.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/ComponentProxy/ComponentProxy.cpp $
// 
// 10    1/17/07 3:57p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a component name agrgument to CommandActivate().
// - Added a call to SetMyName() in CommandActivate.  This is needed in
// the case where different drive curves call the same component a
// different name.
// - Added logic in SetMyName() to only set the name when it is different
// than the current value.
// 
// 9     10/25/06 4:12p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  Added 
//       CommandActivate() and CommandDeactivate().  These are used to
// keep the test components running when a new drive curve starts.
// 
// 8     5/08/06 7:17p Cward
// switched printf statements to log statements. Added Log function to
// allow component manager to use as well.
//
// 7     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 6     7/21/04 11:00a Cward
// Adding slash between path and file name incase a trailing slash is not
// specified for the  path in the config files
//
// 5     6/24/04 12:50a Cward
// Removed setting of the Test Objective time out value. If the value was
// smaller than the sum of the Test Steps, problems would occur. Test
// Steps should be handling timeouts individually.
//
// 4     6/09/04 11:18p Cward
// Refactored App Layer includes to remove implementation. Updated header
// comment block to new standard.
//
// 3     3/04/04 5:10p Cward
// Replaced printfs with Ilogger methods. Removed old unused code
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 6     10/24/03 10:53a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 4     8/14/03 10:37a Khykin
// Updated with make install changes.
//
// 2     8/06/03 3:03p Khykin
// Updating with latest changes from Flatrock.
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:21a Khykin
// Initial checkin.
//
// 2     4/30/03 9:17a Khykin
// Updated the checking of dynamically allocated objects to validate
// existance prior to invocation.
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
// 24    11/11/02 2:59p Khykin
// Updated fault handling.
//
// 23    10/23/02 1:28p Khykin
// Updated the ComponentProxy to kill components that do not
// respond to the terminate pulse.
//
// 22    10/21/02 11:16a Khykin
// Added support for test step timeouts in the commanding of the
// component.
//
// 21    9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 20    9/10/02 11:58a Khykin
// Removing #if 0 code.
//
// 17    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 15    5/24/02 1:50p Kh
// Updating due to debugging.
//
// 14    3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 13    3/06/02 1:45p Kh
// Updating the component proxy diagnostics.
//
// 12    11/13/01 11:17a Kh
// Updated due to command changes.
//
// 11    11/09/01 2:50p Kh
// Updating a debug print statment.
//
// 9     8/28/01 5:24p Kh
// Updating to use BposIsTaskAlive.
//
// 7     8/17/01 2:36p Kh
// Checking in for weekly backup.
//
// 5     8/02/01 4:29p Kh
// Making the parsing of the configuration file have error checking.
//
// 4     7/26/01 7:48p Kh
// Adding support for the defined system pulses.
//
// 3     7/19/01 7:44a Kh
// Adding more features to validate the component to launch.
//
// 2     7/12/01 11:38a Kh
// Tested and ready to go.
//
// 1     7/06/01 5:34p Kh
// Checking in the weeks work.  Not functional just written and in the
// process of debug.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ComponentProxy.h"
#include <errno.h>
#include <unistd.h>

#define BACKGROUND_COMPONENT    "Background"
#define FOREGROUND_COMPONENT    "Foreground"

//  Used to disable code below. See comment below if needed.
#define OBJECTIVE_TIMEOUTS_ENABLED  0

const static std::string whoName("Who");	// the tag for Who handled the message
const static std::string typeName("Type");	  // the tag for the Type
const static std::string objName("Objective");	  // the tag for the Objective


ComponentProxy::ComponentProxy(ILogger *logger) : Debuggable(), Status(), 
m_objective(NULL), m_semCommand(NULL), m_semProcess(NULL), m_semPulse(NULL), 
m_componentName(""), m_executableName(""), m_componentPID(-1), m_reuseComponents( false)
{
	SetStatus(BEP_RUNNING_STATUS);	   // set the task running
	// Save the pointer to the instantiating server so we can utilize its logging capability
	m_logger = logger;
};

ComponentProxy::~ComponentProxy(void)
{
	m_logger->Log(LOG_FN_ENTRY, "~ComponentProxy: %s, %s\n", GetStatus().c_str(), GetMyName().c_str());

	// if there is still an outstanding objective, delete it
	if(m_objective != NULL)
	{
		delete m_objective;
		m_objective = NULL;
	}

	// if the component has not been terminated
	if((GetStatus() != BEP_TERMINATE_STATUS) &&
	   (GetType() != BACKGROUND_COMPONENT) &&
	   (GetExecutableName() != ""))
	{
		if(BposIsTaskAlive(GetName().c_str()) == BEP_STATUS_SUCCESS)
		{
			KillComponent();		   // make sure the component is dead
		}
		else
		{
			m_logger->Log(LOG_ERRORS, "Component Unexpectantly Does Not Exist: %s\n", GetName().c_str());
		}
	}

	// delete the semaphores if they were allocated
	// bruit force programming begins here
	DeleteNewObjectiveSignal();
	DeleteNewProcessSignal();

	m_logger->Log(LOG_DETAILED_DATA, "m_semPulse Check: %d\n", (m_semPulse != NULL));
	if(m_semPulse != NULL)
	{
		m_semPulse->CriticalSectionExit();
		m_logger->Log(LOG_DEV_DATA, "Deleting m_semPulse\n");
		delete m_semPulse;
		m_logger->Log(LOG_DEV_DATA, "Deleted m_semPulse\n");
		m_semPulse = NULL;
	}
	m_logger->Log(LOG_FN_ENTRY, "~ComponentProxy Done\n");
};

void ComponentProxy::Initialize(const XmlNode *component, string driveCurveRootDir, 
								bool reuseComponents)
{
	m_reuseComponents = reuseComponents;
	m_driveCurveRootDir = driveCurveRootDir;
	// setup the debug status
	try
	{
		SetDebug(component->getChildren().getNode("Debug")->getValue());
		m_logger->Log(LOG_DETAILED_DATA, "Component Debug Status: %d\n", IsDebugOn());
	}
	catch(XmlException &e)
	{
		SetDebug("Off");
	}
	try
	{
		SetDebugLevel(component->getChildren().getNode("DebugLevel")->getValue());
		m_logger->Log(LOG_DETAILED_DATA, "Component Debug Level: %d\n",
					  atoi(component->getChildren().getNode("DebugLevel")->getValue().c_str()));
	}
	catch(XmlException &e)
	{
	}

	// set the component name
	try
	{
		SetMyName(component->getName());
		m_logger->Log(LOG_DEV_DATA, "Component Name: %s\n", GetMyName().c_str());
	}
	catch(XmlException &e)
	{
		m_logger->Log(LOG_ERRORS, "Error: No Component Name Set\n");
		throw BepException("No Component Name Set");
	}

	// initialize the communication semaphores in a blocked state
	CreateNewObjectiveSignal();
	CreateNewProcessSignal();
	if(m_semPulse == NULL)		m_semPulse = new BepSemaphore(1, IsDebugOn(), "PulseSemaphore");

	// connect to the results server
	m_resultsComm.Initialize(VEHICLE_TEST_RESULT_SERVER, "Client", IsDebugOn(), 2048);
};

void ComponentProxy::CommandObjective(const XmlNode *objective)
{
	m_logger->Log(LOG_FN_ENTRY, "Commanding Objective\n");

	try
	{
		// wait until component proxy is ready to process a new objective
		WaitForObjectiveDone();

		// make sure the class objective has been reset and set the new objective
		if(m_objective != NULL)
		{
			throw BepException("ComponentProxy %s did not properly reset m_objective\n", GetName().c_str());
		}
		m_objective = const_cast< XmlNode * > (objective);
		m_logger->Log(LOG_DEV_DATA, "(%d) Commanded With Objective: %s\n", pthread_self(), m_objective->ToString().c_str());

		// if the tag is blank
		if(m_objective->getChildren().getNode(0)->getName() == "")
			throw BepException("Error: Blank Tag");
		// also check for only one objective
		if(m_objective->getChildren().size() != 1)
			throw BepException("Error: Sequencing More Than One Objective\n");

		// set the status to testing if providing a valid command
		SetStatus(BEP_TESTING_STATUS);

		// increment the semaphore so the thread can process the objective
		SignalNewObjective();
	}
	catch(BepException &e)
	{
		std::string name = m_objective->getName();

		UpdateResults( BEP_STATUS_FAILURE);

		// ensure the objective is blank
		if(m_objective != NULL)
		{
			delete m_objective;
			m_objective = NULL;
		}

		m_logger->Log(LOG_ERRORS, "Command Objective Exception %s\n", name.c_str());

		// signal that the objective processing is complete
		SignalObjectiveDone();
		// free up anyone waiting for an objective
		SignalNewObjective();

		throw BepException("Component Proxy Command Objective Error: %s, %s", name.c_str(), e.what());
	}
	catch(...)
	{								   // ensure the objective is blank
		UpdateResults( BEP_STATUS_FAILURE);

		if(m_objective != NULL)
		{
			delete m_objective;
			m_objective = NULL;
		}

		// signal that the objective processing is complete
		SignalObjectiveDone();
		// free up anyone waiting for an objective
		SignalNewObjective();

		m_logger->Log(LOG_ERRORS, "Unknown Command Objective Exception\n");

		throw;
	}
};

void ComponentProxy::CommandTerminate(void)
{
	m_logger->Log("Terminate Command Received\n");
	std::string response = "";		   // the results of the objective that was processed

	try
	{
		m_semPulse->CriticalSectionEnter();

		SetStatus(BEP_TERMINATE_STATUS);	// set the proxy status to terminate
		// if the component is still alive
		if(BposIsTaskAlive(GetName().c_str()) == BEP_STATUS_SUCCESS)
		{
			m_componentCommPulse.Terminate(response);	 // Terminate the Component
			if(response == "Pulse sent")	// if pulse successfully sent
			{
				m_logger->Log(LOG_DEV_DATA, "ComponentProxy: Received Terminate Response: %s\n", response.c_str());

				INT32 status = BEP_STATUS_SUCCESS;	  // status of the command

				// if the task is up and not timed out (remember, the task could have several GUI tags to clean up)
				for(int ii = 0; (status == BEP_STATUS_SUCCESS) && (ii < 20); ii++)
				{	// if the task is still alive - wait
					status = BposIsTaskAlive(GetName().c_str());
					// if the component is still up, wait 250 ms
					if(status == BEP_STATUS_SUCCESS)	delay(250);
					m_logger->Log(LOG_DEV_DATA, "Proxy Terminating %d: %s - %d\n", ii, GetName().c_str(), status);
				}

				// if component did not terminate
				if(status == BEP_STATUS_SUCCESS)
				{
					m_logger->Log(LOG_ERRORS, "Error: Could Not Terminate: %s\n", GetName().c_str());
					KillComponent();		   // kill the component
					m_logger->Log(LOG_ERRORS, "Error: Terminated Component: %s\n", GetName().c_str());
				}
			}
			else
				throw BepException("ComponentProxy: Could Not Send Terminate To The Component %s\n", GetMyName().c_str());
		}

	}
	catch(BepException &e)
	{
		if(BposIsTaskAlive(GetName().c_str()) == BEP_STATUS_SUCCESS)
		{
			m_logger->Log(LOG_ERRORS, "Exception Killing: %s\n", GetName().c_str());
			KillComponent();		   // kill the component
			m_logger->Log(LOG_ERRORS, "Exception Killed: %s\n", GetName().c_str());
		}
		else
		{
			m_logger->Log(LOG_ERRORS, "Not Slaying: %s\n", GetName().c_str());
		}
		// free the pulse semaphore
		m_semPulse->CriticalSectionExit();
		// free up anyone waiting on the objective
		SignalNewObjective();
		throw;
	}
	catch(...)
	{
		if(BposIsTaskAlive(GetName().c_str()) == BEP_STATUS_SUCCESS)
			m_logger->Log(LOG_ERRORS, "Unknown Exception Killing: %s\n", GetName().c_str());
		KillComponent();		   // kill the component
		m_logger->Log(LOG_ERRORS, "Unknown Exception Killed: %s\n", GetName().c_str());
		// free the pulse semaphore
		m_semPulse->CriticalSectionExit();
		// free up anyone waiting on the objective
		SignalNewObjective();
		throw;
	}

	// free the pulse semaphore
	m_semPulse->CriticalSectionExit();
	SignalNewObjective();

	// start the thread so it can terminate
	m_logger->Log(LOG_FN_ENTRY, "CommandTerminate() %s done\n", GetName().c_str());
};

void ComponentProxy::CommandAbort(void)
{
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "Abort Command Received\n");
		m_semPulse->CriticalSectionEnter();

		std::string response;		   // response string
		// command the abort
		INT32 status = m_componentCommPulse.Abort(response);
		if(status != BEP_STATUS_SUCCESS)
		{
			throw BepException("%s Could Not Abort\n", GetMyName().c_str());
		}
		m_logger->Log(LOG_DEV_DATA, "Response From Abort: %s\n", response.c_str());
		// post the process semaphore
		m_semPulse->CriticalSectionExit();
	}
	catch(...)
	{
		m_semPulse->CriticalSectionExit();
		throw;
	}
};

void ComponentProxy::CommandReloadConfiguration(void)
{
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "ReloadConfig Command Received\n");
		m_semPulse->CriticalSectionEnter();

		std::string response;		   // response string
		// command the abort
		INT32 status = m_componentCommPulse.ReloadConfiguration(response);
		if(status != BEP_STATUS_SUCCESS)
		{
			throw BepException("%s Could Not ReloadConfig\n", GetMyName().c_str());
		}
		m_logger->Log(LOG_DEV_DATA, "Response From ReloadConfiguration: %s\n", response.c_str());
		// post the process semaphore
		m_semPulse->CriticalSectionExit();
	}
	catch(...)
	{
		m_semPulse->CriticalSectionExit();
		throw;
	}
};

void ComponentProxy::CommandReset(void)
{
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "Reset Command Received\n");
		m_semPulse->CriticalSectionEnter();

		std::string response;		   // response string
		// command the abort
		INT32 status = m_componentCommPulse.Reset(response);
		if(status != BEP_STATUS_SUCCESS) throw BepException("%s Could Not Reset\n", GetMyName().c_str());
		m_logger->Log(LOG_DEV_DATA, "Response From Reset: %s\n", response.c_str());
		// post the process semaphore
		m_semPulse->CriticalSectionExit();
	}
	catch(...)
	{
		m_semPulse->CriticalSectionExit();
		throw;
	}
};

void ComponentProxy::CommandActivate(const std::string &componentName)
{
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "Activate Command Received\n");
		m_semPulse->CriticalSectionEnter();

		SetMyName( componentName);

		std::string response;		   // response string
		// command the abort
		INT32 status = m_componentCommPulse.Activate(response);
		if(status != BEP_STATUS_SUCCESS) throw BepException("%s Could Not Activate\n", GetMyName().c_str());
		m_logger->Log(LOG_DEV_DATA, "Response From Activate: %s\n", response.c_str());
		// post the process semaphore
		m_semPulse->CriticalSectionExit();
	}
	catch(...)
	{
		m_semPulse->CriticalSectionExit();
		throw;
	}
}

void ComponentProxy::CommandDeactivate(void)
{
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "Deactivate Command Received\n");
		m_semPulse->CriticalSectionEnter();

		std::string response;		   // response string
		// command the abort
		INT32 status = m_componentCommPulse.Deactivate(response);
		if(status != BEP_STATUS_SUCCESS) throw BepException("%s Could Not Deactivate\n", GetMyName().c_str());
		m_logger->Log(LOG_DEV_DATA, "Response From Deactivate: %s\n", response.c_str());
		// post the process semaphore
		m_semPulse->CriticalSectionExit();
	}
	catch(...)
	{
		m_semPulse->CriticalSectionExit();
		throw;
	}
}

void ComponentProxy::LaunchComponent(const XmlNode *component)
{
	// first validate the component to launch
	std::string error("");

	if("" == component->getChild("ComponentPath")->getValue())		error = "No ComponentPath Specified";
	else if("" == component->getChild("ComponentName")->getValue())	error = "No ComponentName Specified";
	else if("" == component->getChild("ConfigPath")->getValue())	error = "No ConfigPath Specified";
	else if("" == component->getChild("ConfigFile")->getValue())	error = "No ConfigFile Specified";

	// if there is an error handle it
	if("" != error)
	{
		//      SetStatus("Invalid Component");
		SetStatus(BEP_SOFTWAREFAIL_STATUS);
		m_logger->Log(LOG_ERRORS, "LaunchComponent: Invalid Component %s\n", error.c_str());
		throw BepException("%s Error: %s", component->getName().c_str(), error.c_str());
	}
	else
	{
		// Get the component name to launch
		std::string compToSlay = component->getChild("ComponentName")->getValue();
		std::string compToLaunch = component->getChild("ComponentPath")->getValue() + std::string("/") +
								   compToSlay;
		// Get the component configuration file
		std::string configFile = component->getChild("ConfigPath")->getValue() + std::string("/") +
								 component->getChild("ConfigFile")->getValue();
		// update the component and config paths
		std::string environment = (m_driveCurveRootDir != "") ? m_driveCurveRootDir : getenv("USR_ROOT");
		compToLaunch = environment + compToLaunch;
		environment = (m_driveCurveRootDir != "") ? m_driveCurveRootDir : getenv("FTP_ROOT");
		configFile = environment + configFile;

		// create the command line string to launch the component in the background
		std::string newConfig = "-f" + configFile;

		m_logger->Log(LOG_DEV_DATA, "Launching: %s\n", compToLaunch.c_str());
		// check for the existence of the file
		if((access(compToLaunch.c_str(), F_OK) == BEP_STATUS_SUCCESS) &&
		   (access(configFile.c_str(), F_OK) == BEP_STATUS_SUCCESS))
		{							   // if the component was launched successfully
			// read the component information from the file
			XmlParser           compParser;
			const XmlNode       *componentConfig = compParser.ReturnXMLDocument(configFile);
			const std::string   compName( componentConfig->getChild("Setup/Communication/Name")->getValue());
			INT32               status = (INT32)BEP_STATUS_ERROR;
			std::string         taskName( "");

			// IF we should re-use an existing component
			if( true == m_reuseComponents)
			{
				// See if the process is still running
				status = GetProcecssPid( compName);
			}

			// If we have an exisitng component to re-use
			if( (INT32)BEP_STATUS_ERROR != status)
			{
				m_logger->Log(LOG_DEV_DATA, "Component already running, PID %d\n", status);
			}
			else
			{
				if(BposIsTaskAlive(compName.c_str()) == BEP_STATUS_SUCCESS)
				{
					SlayComponent( compName);
				}
				m_logger->Log(LOG_DEV_DATA, "Component Exists, Launching\n");

				status = SpawnComponent( compToLaunch, newConfig);
				m_logger->Log(LOG_DEV_DATA, "Component Launched, Looking For Errors\n");
			}

			if(status != (INT32)BEP_STATUS_ERROR)
			{						   // get the registered name out of the configuration file
				m_logger->Log(LOG_DEV_DATA, "Component Launched, No Errors\n");
				m_componentPID = status; // set the pid of the component
				m_logger->Log(LOG_DEV_DATA, "Component %s Launched, Verifying\n", compName.c_str());
				status = BEP_STATUS_ERROR;	  // default to fail
				// increased the timout time for checking for the component due to potential debugging info being on
				for(int jj = 0; (status != BEP_STATUS_SUCCESS) && (jj < 10); jj++)
				{
					status = BposIsTaskAlive(compName.c_str());
					if(status == (INT32)BEP_STATUS_ERROR)  delay(250);
				}
				m_logger->Log(LOG_DEV_DATA, "Launched Component Status: %d\n", status);
				if(status == BEP_STATUS_SUCCESS)
					SetExecutableName(component->getChildren().getNode("ComponentName")->getValue());
				else
					throw BepException("Could Not Launch %s", compToLaunch.c_str());
			}
			else
			{
				m_logger->Log(LOG_ERRORS, "Component Launched, Errors\n");
				//SetStatus("Invalid Component");
				SetStatus(BEP_SOFTWAREFAIL_STATUS);
				throw BepException("Could Not Launch %s", compToLaunch.c_str());
			}
		}
		else
		{
			m_logger->Log(LOG_ERRORS, "Could Not Find Component: %s, %s\n", compToLaunch.c_str(), configFile.c_str());
			throw BepException("Could Not Find: %s, or %s", compToLaunch.c_str(), configFile.c_str());
		}
	}
};

void ComponentProxy::ComponentCommInit(const XmlNode *component)
{
	m_logger->Log(LOG_FN_ENTRY, "Configuring the component communication\n");

	// Get the component configuration file
	std::string configFile = component->getChildren().getNode("ConfigPath")->getValue() + std::string("/") +
							 component->getChildren().getNode("ConfigFile")->getValue();
	// update the component and config paths
	std::string environment = (m_driveCurveRootDir != "") ? m_driveCurveRootDir : getenv("FTP_ROOT");
	configFile = environment + configFile;
	// parse the file and get the communication information
	XmlParser compParser;
	m_logger->Log(LOG_DETAILED_DATA, "MEM: Getting comm config from component config file: %d", ReturnMemArena());
	const XmlNode *commConfig = compParser.ReturnXMLDocument(configFile)->getChild("Setup/Communication");
	m_logger->Log(LOG_DETAILED_DATA, "MEM: Done getting comm config from component config file: %d", ReturnMemArena());
	XmlNode *type = const_cast<XmlNode *>(commConfig->getChild("Type"));
	type->setValue("Client");
	m_componentComm.Initialize(commConfig);			// set up the client comm
	m_logger->Log(LOG_DETAILED_DATA, "MEM: Done initializing component comm: %d", ReturnMemArena());
	m_componentCommPulse.Initialize(commConfig);	// set up the client comm for sending pulses
	m_logger->Log(LOG_DETAILED_DATA, "MEM: Done initializing component comm pulse: %d", ReturnMemArena());
	SetName(commConfig->getChild("Name")->getValue());	  // store the registered name
	m_logger->Log(LOG_FN_ENTRY, "Complete component communication configuration\n");
};

INT32 ComponentProxy::SendCommand(void)
{
	m_logger->Log(LOG_FN_ENTRY, "Sending Command\n");

	INT32 status;					   // status of the command and write commands
	std::string response = "";		   // the results of the objective that was processed

	// get the tag and value to send
	std::string tag = m_objective->getChildren().getNode(0)->getName();
	std::string value = m_objective->getChildren().getNode(0)->getValue();
	UINT32 timeout;

//  if objective timeouts are used and the timeout is less than the sum of the
//  test step timeouts, bad things happen. The CCRT Steering commitee decided to
//  remove the following code, and rely on the test steps to handle timeouts. This is
//  being removed by Macro defined above, in case there is a need at a plant that would
//  require reinserting this code.
#if (OBJECTIVE_TIMEOUTS_ENABLED)
	try
	{	// set the timeout value if it exists
		timeout = (UINT32) atoi(m_objective->getAttributes().getNode("Timeout")->getValue().c_str());
	}
	catch(...)
	{	// else default to never
		timeout = 0;
	}
#else
	timeout = 0;
#endif

	// retrieve the command that should be performed
	m_logger->Log(LOG_DEV_DATA, "ComponentProxy Component: %s, Objective: %s, Command: %s, Value: %s, Timeout: %u\n",
				  GetMyName().c_str(), m_objective->getName().c_str(), tag.c_str(), value.c_str(), timeout);

	// send the command message to the component
	m_componentComm.SetTimeout(timeout);
	status = m_componentComm.Command(tag, value, response, true);
	m_logger->Log(LOG_DEV_DATA, "ComponentProxy: Received Command Response: %s\n", response.c_str());
	std::string localTag="";
	std::string localValue="";
	std::string localDiag="";
	return(status);
};

INT32 ComponentProxy::UpdateResults(INT32 status)
{
	std::string localTag = "";		   // local tag for reading responses
	std::string localValue = "";	   // local value for reading responses
	std::string localDiag = "";		   // local diagnostic info for reading response
	// read out the information from the component command
	try
	{
		m_logger->Log(LOG_FN_ENTRY, "ComponentProxy: Updating Results(): %d\n", status);

		// get the command and verify only one command was commanded and executed
		m_componentComm.GetNext(localTag, localValue, localDiag);
		if(status == BEP_STATUS_SUCCESS)	// if successful send
		{
			// validate the response
			std::string objName = m_objective->getChildren().getNode(0)->getName();
			if(localTag != objName)
			{
				m_logger->Log(LOG_ERRORS, "Commanded Objective: %s is not as expected: %s\n", localTag.c_str(), objName.c_str());
				localValue = BEP_INVALID_RESPONSE;
				localTag = objName;
			}
			else if(localValue == "")
			{
				m_logger->Log(LOG_ERRORS, "Value of Command is blank\n");
				localValue = BEP_ERROR_RESPONSE;
			}
		}
		else
		{
			m_logger->Log(LOG_ERRORS, "Bad status passed into UpdateResults - status: %s\n", ConvertStatusToResponse(status).c_str());
			localValue = BEP_SOFTWAREFAIL_RESPONSE;
		}
		m_logger->Log(LOG_DEV_DATA, "ComponentProxy: Read Tag: %s, Value: %s, LocalDiag: %s\n",
					  localTag.c_str(), localValue.c_str(), localDiag.c_str());
	}
	catch(XmlException &e)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s XmlException: %s\n", GetMyName().c_str(), localTag.c_str(), e.Reason().c_str());
	}
	catch(BepException &e)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s BepException: %s\n", GetMyName().c_str(), localTag.c_str(), e.Reason().c_str());
	}
	catch(...)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s Unknown Exception\n", GetMyName().c_str(), localTag.c_str());
	}

	// send results to the sequencer
	try
	{								   // the results string of the message
		std::string response = "";	   // the respose string
		std::string type = GetType();  // the type
		std::string name = m_objective->getName();	  // the name of the sequenced objective
		std::string myName = GetMyName();	 // the name of the task that handled the message

		// build and send the message
		m_logger->Log(LOG_DEV_DATA, "ComponentProxy received: %s, %s, %s, %s, %s\n",
					  localTag.c_str(), localValue.c_str(), name.c_str(), type.c_str(), myName.c_str());

		m_resultsComm.Write(whoName, myName, response, false);
		m_resultsComm.Write(typeName, type, response, false);
		m_resultsComm.Write(objName, name, response, false);
		status = m_resultsComm.Write(localTag, localValue, response, true);
		// verify that every write was not successful
		if(status != BEP_STATUS_SUCCESS)
		{
			m_logger->Log(LOG_ERRORS, "Error writing to the TestResultServer - status: %s\n", ConvertStatusToResponse(status).c_str());
			throw BepException("Error Writing To The TestResultServer");
		}
		m_logger->Log(LOG_DEV_DATA, "ComponentProxy Reply Complete: %s\n", localTag.c_str());
	}
	catch(XmlException &e)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s XmlException: %s\n", GetMyName().c_str(), localTag.c_str(), e.Reason().c_str());
	}
	catch(BepException &e)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s BepException: %s\n", GetMyName().c_str(), localTag.c_str(), e.Reason().c_str());
	}
	catch(...)
	{
		m_logger->Log(LOG_ERRORS, "%s, %s Unknown Exception\n", GetMyName().c_str(), localTag.c_str());
	}

	return(status);
};

void ComponentProxy::KillComponent(void)
{
	m_logger->Log(LOG_FN_ENTRY, "Hey I Had To Kill: %d, %s, %s\n", m_componentPID,
				  GetMyName().c_str(), GetExecutableName().c_str());
	if(m_componentPID != -1)
	{
		int retry = 0;
		int status;
		do
		{
			status = kill(m_componentPID, SIGKILL);
			if(status == -1) BposSleep(100);
		}while((retry++ < 3) && (status == -1));
		if(status == -1)
			m_logger->Log(LOG_ERRORS, "ComponentProxy Error Terminating %s: %s\n", GetMyName().c_str(), strerror(errno));
	}
	m_logger->Log(LOG_FN_ENTRY, "Killed: %s\n", GetMyName().c_str());
}

void ComponentProxy::SlayComponent( const std::string &compToSlay)
{
	char buffer[128];

	m_logger->Log(LOG_ERRORS, "The component %s was running, so I will slay it\n", compToSlay.c_str());
	sprintf(buffer, "on -d -t /dev/con6 slay %s", compToSlay.c_str());
	system(buffer);
	BposSleep(250);	// give it time to die
}

INT32 ComponentProxy::SpawnComponent( const std::string &exeName, const std::string &cfgFile)
{
	INT32 status = spawnl( P_NOWAITO, exeName.c_str(), exeName.c_str(), cfgFile.c_str(), NULL);

	return( status);
}

BEP_STATUS_TYPE ComponentProxy::CreateNewObjectiveSignal(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

	// if not previously created, create the semaphore
	if(m_semCommand == NULL)
		m_semCommand = new BepSemaphore(0, IsDebugOn(), "CommandBepSemaphore");

	return(status);
}

BEP_STATUS_TYPE ComponentProxy::DeleteNewObjectiveSignal(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

	m_logger->Log(LOG_DETAILED_DATA, "m_semCommand Check: %d\n", (m_semCommand != NULL));
	if(m_semCommand != NULL)
	{
		m_semCommand->CriticalSectionExit();
		delete m_semCommand;
		m_semCommand = NULL;
	}

	return(status);
}

BEP_STATUS_TYPE ComponentProxy::WaitForNewObjective(void)
{
	return(m_semCommand->CriticalSectionEnter());
}

BEP_STATUS_TYPE ComponentProxy::SignalNewObjective()
{
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
	// if a valid semaphore to command
	if(m_semCommand != NULL)
	{	// signal a new objective is ready
		m_semCommand->CriticalSectionExit();
	}

	return(status);
}

BEP_STATUS_TYPE ComponentProxy::CreateNewProcessSignal(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

	// if not previously created, create the semaphore
	if(m_semProcess == NULL)
		m_semProcess = new BepSemaphore(0, IsDebugOn(), "ProcessBepSemaphore");

	return(status);
}

BEP_STATUS_TYPE ComponentProxy::DeleteNewProcessSignal(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

	m_logger->Log(LOG_DETAILED_DATA, "m_semProcess Check: %d\n", (m_semProcess != NULL));
	if(m_semProcess != NULL)
	{
		m_semProcess->CriticalSectionExit();
		delete m_semProcess;
		m_semProcess = NULL;
	}

	return(status);
}

BEP_STATUS_TYPE ComponentProxy::WaitForObjectiveDone(void)
{
	return(m_semProcess->CriticalSectionEnter());
}

BEP_STATUS_TYPE ComponentProxy::SignalObjectiveDone(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
	// if a valid semaphore to command
	if(m_semProcess != NULL)
	{	// signal objective processed
		m_semProcess->CriticalSectionExit();
	}

	return(status);
}


void ComponentProxy::SetName(const std::string name)
{
	m_logger->Log(LOG_DETAILED_DATA, "Setting Name: %s\n", name.c_str());
	m_componentName = name;
};

const std::string ComponentProxy::GetName(void)
{
	return(m_componentName);
};

void ComponentProxy::SetMyName(const std::string name)
{
	m_logger->Log(LOG_DETAILED_DATA, "Setting My Name: %s\n", name.c_str());
	if( name != m_myName)
	{
		m_myName = name;
	}
};

const std::string ComponentProxy::GetMyName(void)
{
	return(m_myName);
};

void ComponentProxy::SetThreadID(const pthread_t id)
{
	m_thread = id;
};

const pthread_t ComponentProxy::GetThreadID(void)
{
	return(m_thread);
};

void ComponentProxy::SetExecutableName(const std::string name)
{
	m_executableName = name;
};

const std::string ComponentProxy::GetExecutableName(void)
{
	return(m_executableName);
};

void ComponentProxy::SetType(const std::string type)
{
	m_type = type;
};

const std::string ComponentProxy::GetType(void)
{
	return(m_type);
};

void ComponentProxy::Log( UINT32 mask, const char *message)
{
	if(m_logger)	m_logger->Log(mask, message);
}

int ComponentProxy::ReturnMemArena()
{
	struct mallinfo mInfo;
	mInfo = mallinfo();
	return(mInfo.arena);
}

