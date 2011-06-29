//*************************************************************************
// Description:
//   This class will launch the system as specified by the configuration file.
//	 It will also wait for a terminate, abort, or reset message to be send and
// 	 then it will command all of its children to terminate, abort, or reset.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/Launcher/Launcher.cpp $
// 
// 8     5/08/06 7:41p Cward
// Changes that cause the launcher to report executable files that could
// not be found to the Log files
// added processing for m_missingTasks, so that missing tasks are recorded to
// the disk based log file.
//
// 7     2/15/06 7:59p Cward
// Added system print messages and Logger messages about rebooting when
// time has expired withou a valid license.
// Added Log and print to screen that QNX needs to be rebooted
// when system is disabled. c.516
//
// 6     7/20/05 9:15p Cward
// Added new TaskMon support
//
// 5     3/11/05 3:37p Cward
// Fixed OrderToLaunch problems, including duplicate values, and missing
// values
//
// 4     3/03/05 11:46p Cward
// Updated for 05030302. Needed quick fix for new licensing
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     2/02/05 11:54p Cward
// Updated for the new License
//
// 1     7/21/04 10:18a Cward
//
// 5     6/24/04 12:58a Cward
// Removed pound if 0 blocks. UPdated the header block comments to the new
// standard.
//
// 4     5/19/04 10:25p Cward
// Removed the capability of generating a ccrt key
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     11/04/03 10:34a Khykin
// Moved the class to the Library directory so that it may be used for
// system
// testing to launch up the necessary processes.
//
// 2     10/24/03 1:36p Khykin
// Updated launcher with security code.
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 2     6/27/03 7:51a Khykin
// Increased the timeout time when waiting for a response to a register
// message.
//
// 1     6/12/03 4:03p Khykin
// Initial checkin.
//
// 2     5/29/03 6:32a Khykin
// Updating shutdown logic.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:52  tbochene
//
//    PooBag
//
//
// 2     2/08/03 9:56a Khykin
// Updated for system shutdown.
//
// 1     1/27/03 7:13a Khykin
// Initial Checkin
//
// 1     1/18/03 8:09a Khykin
// Initial Checkin From DCTC
//
// 18    10/31/02 1:01p Tbochene
// Updating Log messages
//
// 17    10/23/02 1:27p Khykin
// Update the Launcher to kill processes that do not respond
// to the terminate pulse.
//
// 16    9/26/02 2:02p Tbochene
// Removed compiler warnings
//
// 15    8/23/02 8:36a Kh
// Synchronizing the pit machine with VSS.
//
// 12    5/24/02 3:06p Kh
// Adding the ability to delay between tasks.
//
// 11    4/29/02 6:04p Kh
// Updated to warn the user if a file could not be found.
//
// 8     3/19/02 12:32p Kh
// Updating for changes to BepServer.
//
// 7     3/06/02 2:05p Kh
// Making a test for the launcher and updating to use the defined USR_ROOT
// environment.
//
// 6     12/20/01 2:32p Kh
// Updating the Register command
//
// 5     12/18/01 3:52p Kh
// Updating the Register command
//
// 4     12/14/01 5:45p Kh
// Adding the reset to allow for subscription synchronization.
//
// 3     12/07/01 3:02p Kh
// Adding the reloading of the configuration after the system is l
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include <unistd.h>
#include "Launcher.h"
#include "Krnl.h"

#define INVALID_LICENSE_TIME_ALLOWED 7200

//------------------------------------------------------------------------------
// CCRT Key File That Conatins The Encrypted Mac Address
// Note: This is hardcoded here so people can not figure out how we do it
//      If it is placed in a header file, it is possible that discovery
//      could happen if the API is sold
//------------------------------------------------------------------------------
//#define CCRT_KEY_FILE   "/usr/local/bin/Key.ccrt"
//#define CCRT_INTERFACE  "en0"

Launcher::Launcher() : BepComponent(), m_interTaskDelayTime(500), m_taskStartupTime(5000),
m_taskTerminateTime(5000), m_terminateOnFail(true), m_enableSystemShutdown(true),
m_foundAllTasks(false),
m_faultServer( NULL), m_licenseTimer(NULL),m_systemDisabled(false), m_ndb(NULL)
{
	m_taskTable.clear();
	m_taskNameList.clear();
    m_missingTasks.clear();
};

Launcher::~Launcher()
{	// if processes not terminated and system launched
	if ( (GetStatus() != BEP_TERMINATE_STATUS) &&
		 (GetStatus() != BEP_INITIALIZED_STATUS) )
		Terminate();

	// Destroy fault server interface object
	if ( m_faultServer != NULL)
	{
		m_faultServer->Close();
		delete m_faultServer;
		m_faultServer = NULL;
	}

	if (m_licenseTimer != NULL)
	{	// Need to stop the monitor before deleting it
		m_licenseTimer->StopThread();
		m_licenseTimer->WaitUntilDone();
		if (m_licenseTimer != NULL)	 delete m_licenseTimer;
	}
	// Get rid of the NDB comm object
	if (m_ndb != NULL)	delete m_ndb;
	m_ndb = NULL;
}


void Launcher::Initialize(const XmlNode *document)
{
	try
	{
		if (document != NULL)	 // if valid document
		{
			// set up the base BepComponent
			BepComponent::Initialize(document);

			// get the setup node
			const XmlNode *setup = document->getChild("Setup");

			// set up the system communication
			try
			{
				m_comm.Initialize(setup->getChild("Communication"));
			}
			catch (BepException &e)
			{
				throw BepException("Communication Configuration Error: %s", e.what());
			}

			try
			{
				m_interTaskDelayTime = atoi(setup->getChild("InterTaskDelayTime")->getValue().c_str());
			}
			catch (BepException &e)
			{
				m_interTaskDelayTime = 500;
				Log(LOG_DEV_DATA, "InterTaskDelyTime Not Specified, Defaulting to .5 seconds\n");
			}

			try
			{
				m_taskStartupTime = atoi(setup->getChild("TaskStartupTime")->getValue().c_str());
			}
			catch (BepException &e)
			{
				m_taskStartupTime = 0;
				Log(LOG_DEV_DATA, "TaskStartupTime Not Specified, Defaulting to 0 seconds\n");
			}

			try
			{
				m_taskTerminateTime = atoi(setup->getChild("TaskTerminateTime")->getValue().c_str());
			}
			catch (BepException &e)
			{
				m_taskTerminateTime = 0;
				Log(LOG_DEV_DATA, "TaskTerminateTime Not Specified, Defaulting to 0 seconds\n");
			}

			try
			{
				m_terminateOnFail = (setup->getChild("TerminateOnError")->getValue() == "Yes");
			}
			catch (XmlException &e)
			{
				m_terminateOnFail = true;
			}

			try
			{
				m_enableSystemShutdown = (setup->getChild("EnableSystemShutdown")->getValue() == "Yes");
			}
			catch (XmlException &e)
			{
				m_enableSystemShutdown = true;
			}

			// register the component
			BepComponent::Register();

			RegisterWithTaskMon();

			m_foundAllTasks = GetTasksToLaunch(document);

        }
		else
			throw BepException("Launcher Invalid configuration file");

		SetStatus(BEP_INITIALIZED_STATUS);	// set status to indicate all systems initialized
	}
	catch (XmlException &e)
	{
		SetStatus(BEP_TERMINATE_STATUS);
		throw BepException("Launcher Initialize: %s", e.what());
	}
	catch (BepException &e)
	{
		SetStatus(BEP_TERMINATE_STATUS);
		throw BepException("Launcher Initialize: %s", e.what());
	}
	catch (...)
	{
		SetStatus(BEP_TERMINATE_STATUS);
		throw BepException("Launcher Initialize: Unknown Error");
	}

};

void Launcher::Run(volatile bool *terminateFlag)
{
	Log(LOG_DEV_DATA, "Validating license information...\n");
	try
	{
		LaunchSystem();		// launch the system
		ValidateLicense();	// Validate the CCRT License
		ProcessRequest();	// process the system requests
	}
	catch (BepException &e)
	{
		Log(LOG_ERRORS, "Launcher Run Exception: %s\n", e.what());
	}
	catch (...)
	{
		Log(LOG_ERRORS, "Launcher Run Unknown Exception\n");
	}
	Log(LOG_DEV_DATA, "Shutting down the system...\n");
	Shutdown();
};

void Launcher::ValidateLicense(void)
{
	// Check License...
	if (!m_lic.Valid())
	{
		string response;
		string faultTag = INVALID_LICENSE_NOTIFICATION_TAG;
		string faultDescription = "System License not valid - Please contact Burke Porter";
		string processName = "core/Launcher";
		INT32 faultLevel = 3;

		// Invalid license, notify, set fault, and start license timer
		Log(LOG_DEV_DATA, "License is not valid, system allowed to run temporarily\n");
		printf("\n\n\n\tInvalid License, Please Contact Burke E. Porter Machinery For Licensing Assistance: (616) 459-9531\n\n\n");

		try
		{
			// Log the fault in all faults node
			if ( m_faultServer != NULL)
			{	// Log the sending of the fault
				Log(LOG_ERRORS,"Fault '%s': severity %d sent to fault server\n",
					faultTag.c_str(), faultLevel);
				// Send fault to fault server
				m_faultServer->SetFault( faultTag,
										 faultDescription,
										 processName,
										 response,
										 true,
										 faultLevel);
			}
			// Setup and write an xmlnode to a system tag for invalid license notification
			XmlNode licenseNode(INVALID_LICENSE_NOTIFICATION_TAG,"1");
			// create a communication object to send the tag with
			IBepCommunication comm2;
			// connect to the task
			comm2.Initialize("core/SystemMonitor", "Client");
			// write the tag to client
			INT32 status = comm2.Write(&licenseNode,response,true);
			// Verify status of write
			if (status == BEP_STATUS_SUCCESS)
			{
				Log(LOG_DEV_DATA, "Launcher Sent License Notification Tag\n");
			}
			else
			{
				Log(LOG_ERRORS, "Launcher Could Not Send License Notification Tag\n");
			}
		}
		catch (XmlException &ex)
		{
			Log(LOG_ERRORS, "XmlException: %s\n", ex.what());
		}
		catch (BepException &ex)
		{
			Log(LOG_ERRORS, "BepException: %s\n", ex.what());
		}
		catch (...)
		{
			Log(LOG_ERRORS, "Unknown exception setting License violation notification\n");
		}

		// Start thread to monitor license
		if (m_licenseTimer == NULL)
		{
			m_licenseTimer = new LicenseTimer(this,INVALID_LICENSE_TIME_ALLOWED);
			m_licenseTimer->Resume(false);
		}
	}
}

void Launcher::DisableSystem(void)
{
	string faultTag = INVALID_LICENSE_FATAL_FAULT_TAG;
	string faultDescription = "Invalid License - System Disabled, Contact Burke Porter";
	string processName = "core/Launcher";
	string response;
	string currentRollsPosition;
	string rollsTag = ROLLS_DOWN_DATA_TAG;
	INT32 faultLevel = 9;
	INT32 status = BEP_STATUS_ERROR;

	m_ndb->Read(ROLLS_DOWN_DATA_TAG, response, true);
	m_ndb->GetNext(rollsTag, currentRollsPosition, response);

	printf("DEBUG - Launcher::DisableSystem() Rolls Down Tag = %s\n",currentRollsPosition.c_str());

	if (currentRollsPosition == "1" || m_systemDisabled ||
		(BposSecondsSinceBootUp() > (INVALID_LICENSE_TIME_ALLOWED + 600)))
	{
		try
		{
			// Log the fault in all faults node
			if ( m_faultServer != NULL)
			{
				if (!m_systemDisabled)
				{
					Log(LOG_ERRORS,"Fault '%s': severity %d sent to fault server\n",
						faultTag.c_str(), faultLevel);
					// Send fault to faultserver
					m_faultServer->SetFault( faultTag,
											 faultDescription,
											 processName,
											 response,
											 true,
											 faultLevel);
				}
				// create xml node for sending the tag
				XmlNode licenseNode(INVALID_LICENSE_FATAL_FAULT_TAG,"1");
				// create a communication object to send the tag with
				IBepCommunication commSystem;
				// connect to the task
				commSystem.Initialize("core/SystemMonitor", "Client");
				// write that tag to the server
				status = commSystem.Write(&licenseNode,response,true);
				// Verify status of write
				if (status == BEP_STATUS_SUCCESS)
				{
					Log(LOG_DEV_DATA, "Launcher Sent License Fatal Fault Tag\n");
				}
				else
				{
					Log(LOG_ERRORS, "Launcher Could Not Send License Fatal Fault Tag\n");
				}

			}
		}
		catch (XmlException &ex)
		{
			Log(LOG_ERRORS, "XmlException: %s\n", ex.what());
		}
		catch (BepException &ex)
		{
			Log(LOG_ERRORS, "BepException: %s\n", ex.what());
		}
		catch (...)
		{
			Log(LOG_ERRORS, "Unknown exception setting License violation notification\n");
		}

		try
		{
			// create a communication object to send the command with
			IBepCommunication commICM;
			// connect to the task
			commICM.Initialize("mgr/ICM", "Client");
			// create xmlnode for sending command
			XmlNode disableNode("LicenseViolation","TRUE");
			// Send the command
			status = commICM.Command(&disableNode,response,true);
			// if successfully commanded
			if (status == BEP_STATUS_SUCCESS)
			{
				try
				{
					Log(LOG_DEV_DATA, "Launcher Sent License Violation Command\n");
					// Kill the machine component
					// create a communication object to send the command with
					IBepCommunication commMotor;
					// connect to the task
					commMotor.Initialize("core/MotorController", "Client");
					// send the terminate pulse
					status = commMotor.Terminate(response);
					if (BEP_STATUS_SUCCESS == status)
					{
						Log(LOG_DEV_DATA,"License Fatal Fault - Disabled system\n");
						m_systemDisabled = true;
					}
					else if (true == m_systemDisabled)
					{
						status = BEP_STATUS_SUCCESS;
					}
				}
				catch (BepException ex)
				{
					if (true == m_systemDisabled) status = BEP_STATUS_SUCCESS;
					else status	= BEP_STATUS_ERROR;
					Log(LOG_ERRORS, "Exception\n");
				}
				catch (...)
				{
					status = BEP_STATUS_ERROR;
					Log(LOG_ERRORS, "Unknown Exception Occurred\n");
				}
			}
		}
		catch (XmlException &ex)
		{
			Log(LOG_ERRORS, "XmlException: %s\n", ex.what());
			status = BEP_STATUS_ERROR;
		}
		catch (BepException &ex)
		{
			Log(LOG_ERRORS, "BepException: %s\n", ex.what());
			status = BEP_STATUS_ERROR;
		}
		catch (...)
		{
			Log(LOG_ERRORS, "Unknown exception setting License violation notification\n");
			status = BEP_STATUS_ERROR;
		}

        // added for clarity
        printf("\nQNX System must be re-booted.\n");
        printf("\n%s\n",faultDescription.c_str());
        Log(LOG_ERRORS,"\nQNX System must be re-booted.\n");
        Log(LOG_ERRORS,"\n%s\n",faultDescription.c_str());

		if (BEP_STATUS_SUCCESS != status)
		{
			Log(LOG_ERRORS, "\n\nCould Not Disable the system\n"
				"\n\n\n\t\tTerminating system because of violation\n");
			// unable to disable the system, therefore kill the system
			pthread_create(NULL,NULL, LicenseViolationTerminate,(void *)this);
		}
	}
}

void * Launcher::LicenseViolationTerminate(void *ptr)
{
	INT32 status = BEP_STATUS_FAILURE;
	string response;
	Launcher *threadPointer = (Launcher *)ptr;
	try
	{
		// Create a communication channel to the main launcher thread to slay the system
		IBepCommunication commLauncher;
		// connect to the task
		commLauncher.Initialize("core/Launcher", "Client");
		// send the terminate pulse
		status = commLauncher.Terminate(response);
	}
	catch (...)
	{
		status =  BEP_STATUS_ERROR;
	}
	// Check status of the communication
	if (BEP_STATUS_SUCCESS != status)
	{	// Unable to communicate with the main Launcher
		//   Thread, so just kill the system.
		threadPointer->Terminate();
	}
	return(NULL);
}

/**
 * Launch the system based on the component initialization.
 * @since Version 1.0
 */
void Launcher::LaunchSystem(void)
{	// if there are tasks to load
	if (!m_taskTable.empty())
	{
		TASKTABLEITR iter = m_taskTable.begin();

		for (; iter != m_taskTable.end();)
		{	// send a prompt to indicate what tasks are being launched
			Log(LOG_FN_ENTRY, "Launching: %s, %s\n", (*iter).second.process.c_str(), (*iter).second.arguments.c_str());
			// launch the task
			(*iter).second.processID = BposCreateTask(ROUND_ROBIN, BposGetPriority(0), 0, 0, (*iter).second.process.c_str(), (*iter).second.arguments.c_str());
			if ((*iter).second.processID == BP_ERR_KRNL_FAIL)
			{	// verify that the server starts
				Log(LOG_ERRORS, "Error Launching %s: %s\n", (*iter).second.process.c_str(), strerror( errno ) );
				(*iter).second.status = BEP_FAIL_STATUS;	// set the status to fail
				SetStatus(BEP_FAIL_STATUS);					// set the task status to fail
			}
			else
			{
				UINT32 status = BEP_STATUS_ERROR;
				if ((*iter).second.registeredName != "")
				{
					Log(LOG_DEV_DATA, "Looking For: %s\n", (*iter).second.registeredName.c_str());
					// give up to m_taskStartupTime mili-seconds for task detection
					for (UINT32 jj = 0; (jj < (UINT32)(m_taskStartupTime/100)) && (status == BEP_STATUS_ERROR); jj++)
					{
						status = BposIsTaskAlive((*iter).second.registeredName.c_str());
						if (status == BEP_STATUS_ERROR)	 // if error, delay
							BposSleep(100);
					}
				}
				else
				{
					Log(LOG_DEV_DATA, "No Name Registered, Must Be Good\n");
					status = BEP_STATUS_SUCCESS;
				}
				// determine if the server started
				if (status == BEP_STATUS_SUCCESS)
				{

                    // this is the first line actually logged, after the logger task is launched
					(*iter).second.status = BEP_RUNNING_STATUS;	// set the component status to running
					Log(LOG_DEV_DATA, "  STARTED!\n");						// log the result
				}
				else
				{
					(*iter).second.status = BEP_FAIL_STATUS;	// set the component status to fail
					SetStatus(BEP_FAIL_STATUS);					// set the task status to fail
					Log(LOG_ERRORS, "  COULD NOT START!\n");					// log he result
				}
				if (++iter != m_taskTable.end())
					BposSleep(m_interTaskDelayTime);

			}

		}
		// if status == BEP_STATUS_ERROR then tell everyone that there was an error starting the system
		if (GetStatus() != BEP_FAIL_STATUS)		 // if all is good
		{
			if (Register() == BEP_SUCCESS_RESPONSE)	 // command all servers to register
			{
				SetStatus(BEP_RUNNING_STATUS);		// set the task status to indicate all tasks launched
				Log(LOG_DEV_DATA, "Finished Launching Tasks\n");
				if (m_foundAllTasks != true)
				{
					Log(LOG_ERRORS, "Warning: Could Not Find All Tasks\n");
                    if(m_missingTasks.size() > 0)
                    {
                        TASKNAMELISTITR locItr = m_missingTasks.begin();
                        for(; locItr != m_missingTasks.end(); locItr++)
                        {
                            // log names of unfound files
                            Log(LOG_ERRORS, "    Missing: %s\n",locItr->second.c_str());
                        }
                    }
                    SetStatus(BEP_SOFTWAREFAIL_STATUS);
                }
            }
			else
			{
				SetStatus(BEP_SOFTWAREFAIL_STATUS);
				Log(LOG_ERRORS, "Error Could Not Register All Of The Tasks\n");
			}
		}
		else
		{	// send a prompt indicating the system is wacked
			Log(LOG_ERRORS, "Errors Launching The System\n");
		}
	}
	else
		throw BepException("No Tasks To Launch In The Configuration File");

	if (m_terminateOnFail == false)		 // if launcher should not terminate tasks on failure
		SetStatus(BEP_RUNNING_STATUS);	// set the status to all tasks launched

	Log(LOG_FN_ENTRY, "Launcher Finished Launching The System\n");
}

/**
 * Wait for the requests of others.
 * @since Version 1.0
 */
void Launcher::ProcessRequest(void)
{
	int chid;							// chid of the requesting process
	IBepCommunication handler;			// handler for incomming requests
	std::string message;				// buffer to hold messages

	if (GetStatus() == BEP_RUNNING_STATUS)	 // if the system is running process requests
	{
		// if all the tasks are launched and everything is normal
		while ((GetStatus() != BEP_TERMINATE_STATUS) && (GetStatus() != BEP_SHUTDOWN_STATUS))
		{
			Log(LOG_DEV_DATA, "Launcher Waiting For A Request\n");
			message.clear();						// clear out the message buffer
			chid = m_comm.WaitForRequest();			// wait for a request
			handler.Initialize(m_comm);				// initialize the handler
			handler.HandleRequest(message, this);	// handle the request
			Log(LOG_DEV_DATA, "Launcher Request Handled %s\n", message.c_str());
		}
	}
	else
		throw BepException("Process Request Error, Status: %s", GetStatus().c_str());
};

/**
 * Reloads the configuration file and resets the
 * base components objects.
 * @since Version 1.0
 */
void Launcher::ReloadConfiguration(void)
{
	if (!m_taskTable.empty())
	{	// send a terminate command to all of the launched processes
		IBepCommunication comm;						// create a communication object to send the command with
		std::string response;						// create a response buffer to get the response with
		TASKTABLEITR iter = m_taskTable.begin();	// create an iterator to iterate through the list
		int status;									// the status of the termiante
		std::string name;							// the name of the registered task

		for (; iter != m_taskTable.end();iter++)	 // walk through the list backwards
		{
			Log(LOG_DEV_DATA, "ReloadingConfiguration: %s\n", (*iter).second.process.c_str());
			name = (*iter).second.registeredName;			// get the name of the task to reload config
			comm.Initialize(name, "Client");				// connect to the task
			status = comm.ReloadConfiguration(response);	// send the reload configuration pulse
			// if successfully commanded
			if (status == BEP_STATUS_SUCCESS)
			{
				Log(LOG_DEV_DATA, "Launcher Commanded Reload Config: %s\n", name.c_str());
			}
			else Log(LOG_ERRORS, "Launcher Could Not Command Reload Config: %s\n", name.c_str());
		}
	}
};

/**
 * Method used to abort the current processing based
 * on the needs of the component.
 * @since Version 1.0
 */
void Launcher::Abort(void)
{
	if (!m_taskTable.empty())
	{	// send an abort command to all of the launched processes
		IBepCommunication comm;						// create a communication object to send the command with
		std::string response;						// create a response buffer to get the response with
		TASKTABLEITR iter = m_taskTable.begin();		// create an iterator to iterate through the list
		int status;									// the status of the termiante
		std::string name;							// the name of the registered task

		for (; iter != m_taskTable.end();iter++) // walk through the list backwards
		{
			Log(LOG_DEV_DATA, "Aborting: %s\n", (*iter).second.process.c_str());
			name = (*iter).second.registeredName;			// get the name of the task to abort
			comm.Initialize(name, "Client");				// connect to the task
			status = comm.Abort(response);					// send the abort pulse
			// if successfully commanded
			if (status == BEP_STATUS_SUCCESS)
			{
				Log(LOG_DEV_DATA, "Launcher Aborted: %s\n", name.c_str());
			}
			else Log(LOG_ERRORS, "Launcher Could Not Abort: %s\n", name.c_str());
		}
	}
};

/**
 * Method used to set all of the objects in the component
 * to their original states.  This would includes all
 * member variables such as buffers, lists, and stored
 * data.
 * @since Version 1.0
 */
void Launcher::Reset(void)
{
	if (!m_taskTable.empty())
	{	// send a reset command to all of the launched processes
		IBepCommunication comm;						// create a communication object to send the command with
		std::string response;						// create a response buffer to get the response with
		TASKTABLEITR iter = m_taskTable.begin();		// create an iterator to iterate through the list
		int status;									// the status of the termiante
		std::string name;							// the name of the registered task

		for (; iter != m_taskTable.end();iter++) // walk through the list backwards
		{
			try
			{
				Log(LOG_DEV_DATA, "Reseting: %s\n", (*iter).second.process.c_str());
				name = (*iter).second.registeredName;			// get the name of the task to reset
				comm.Initialize(name, "Client");				// connect to the task
				status = comm.Reset(response);					// send the reset pulse
				// if successfully commanded
				if (status == BEP_STATUS_SUCCESS)
				{
					Log(LOG_DEV_DATA, "Launcher Reset: %s\n", name.c_str());
				}
				else Log(LOG_ERRORS, "Launcher Could Not Reset: %s\n", name.c_str());
			}
			catch (XmlException &e)
			{
				Log(LOG_ERRORS, "XmlException Resetting: %s\n", name.c_str());
			}
			catch (BepException &e)
			{
				Log(LOG_ERRORS, "BepException Resetting: %s\n", name.c_str());
			}
		}
	}
};

/**
 * Method used to command all of the components to register
 * all of their subscriptions and anything else that should be
 * done once the system is up.
 * @since Version 2.2
 */
const std::string Launcher::Register(void)
{
	std::string overallStatus=BEP_SUCCESS_RESPONSE;	// the overall status of the method

	if (!m_taskTable.empty())
	{	// send a register command to all of the launched processes
		IBepCommunication comm;						// create a communication object to send the command with
		std::string name;							// the name of the registered task
		std::string tag;							// the name of the registered task
		std::string status;							// the status of the register
		std::string response;						// create a response buffer to get the response with

		// walk throught the task list and command them to Register
		for (TASKTABLEITR iter = m_taskTable.begin() ;iter != m_taskTable.end(); iter++)
		{
			try
			{
				name = tag = status = response = "";					// reset the vars
				if ((*iter).second.status == BEP_RUNNING_STATUS)
				{
					// if there is a registered name and the task needs to be registered
					if (((*iter).second.registeredName != "") && (*iter).second.registerTask)
					{
						Log(LOG_DEV_DATA, "Commanding %s to Register\n", (*iter).second.process.c_str());
						name = (*iter).second.registeredName;			// get the name of the task to register
						comm.Initialize(name, "Client", IsDebugOn());	// connect to the task
						comm.SetTimeout(5000);							// set to a five second timeout
						tag = BEP_REGISTER;
						// register and check if successfully
						if (comm.Register(tag, status, response, true) == BEP_STATUS_SUCCESS)
						{
							Log(LOG_DEV_DATA, "Launcher Commanded %s to Register\n", name.c_str());
						}
						else
						{	// since the logger does not comply only set if not the logger
							if (name != DATA_LOGGER_NAME)	 overallStatus = BEP_FAILURE_RESPONSE;
							Log(LOG_ERRORS, "Launcher Could NOT Command %s to Register\n", name.c_str());
						}
					}
					else
					{
						Log(LOG_ERRORS, "Warning: %s Is not being registered\n", (*iter).second.process.c_str());
					}
				}
				else
					Log(LOG_ERRORS, "Not Registering %s, Due To Status %s\n",
						(*iter).second.registeredName.c_str(), (*iter).second.status.c_str());
			}
			catch (XmlException &e)
			{
				if (name != DATA_LOGGER_NAME)	 overallStatus = BEP_FAILURE_RESPONSE;
				Log(LOG_ERRORS, "XmlException Registering: %s\n", name.c_str());
			}
			catch (BepException &e)
			{
				if (name != DATA_LOGGER_NAME)	 overallStatus = BEP_FAILURE_RESPONSE;
				Log(LOG_ERRORS, "BepException Registering: %s\n", name.c_str());
			}
		}
		Log(LOG_ERRORS, "All Tasks Registered: %s\n", overallStatus.c_str() );

		BepComponent::Register();

		// Set up a fault server
		std::string faultServerName( FAULT_SERVER_NAME);

		// Close existing interface
		if ( m_faultServer != NULL)	 m_faultServer->Close();

		try
		{
			// Create new interface if needed
			if ( m_faultServer == NULL)	 m_faultServer = new IFaultServer;
			// Initialize interface
			m_faultServer->Initialize(faultServerName);
		}
		catch ( BepException &err)
		{
			Log( LOG_FN_ENTRY, "\tError initializing fault server interface: %s\n", err.GetReason());
		}
		// Create a named data broker object for tag communication
		if (m_ndb == NULL)	m_ndb = new INamedDataBroker(IsDebugOn());

	}
	return(overallStatus);
};

/**
 * Method used to terminate the component.  This performs
 * any cleanup that the destructor would not and sets
 * the state of the component to terminate.
 * @since Version 1.0
 */
void Launcher::Terminate(void)
{
	if (GetStatus() != BEP_TERMINATE_STATUS)
	{
		SetStatus(BEP_TERMINATE_STATUS);	// set the status to terminated
		Log(LOG_FN_ENTRY, "Terminating The System\n");
		if (!m_taskTable.empty())
		{	// remove the background thread for invalid license if
			//   it exists
			if (m_licenseTimer != NULL)
			{	// Need to stop the monitor before deleting it
				m_licenseTimer->StopThread();
				m_licenseTimer->WaitUntilDone();
				if (m_licenseTimer != NULL)	 delete m_licenseTimer;
				m_licenseTimer = NULL;
			}

			// send a terminate command to all of the launched processes
			IBepCommunication comm;						// create a communication object to send the command with
			std::string response;						// create a response buffer to get the response with
			TASKTABLEITR iter = m_taskTable.begin();	// create an iterator to iterate through the list
			int status;									// the status of the termiante

			// load up the tasks to shut down
			if (m_taskTable.size())
			{	// retrieve all of the registered names
				Log(LOG_DEV_DATA, "Terminating %d Tasks\n", m_taskTable.size());
				std::string registeredName[m_taskTable.size()];
				int processID[m_taskTable.size()];
				// load the names so the tasks can be terminated in reverse order from creation
				for (int ii = (m_taskTable.size() - 1); iter != m_taskTable.end();iter++, ii--)
				{
					registeredName[ii] = (*iter).second.registeredName;
					processID[ii] = (*iter).second.processID;
					Log(LOG_DEV_DATA, "Loading: %s\n", registeredName[ii].c_str());
				}
				// walk through the list backwards to terminate the tasks
				Log(LOG_DEV_DATA, "There are %d Tasks to terminate\n", m_taskTable.size());
				for (UINT32 jj = 0; jj < m_taskTable.size(); jj++)
				{
					try
					{
						Log(LOG_DEV_DATA, "Terminating: %s, ", registeredName[jj].c_str());
						// connect to the task
						comm.Initialize(registeredName[jj], "Client", IsDebugOn());
						status = comm.Terminate(response);				// send the terminate pulse
						// if successfully commanded
						if (status == BEP_STATUS_SUCCESS)
						{
							for (UINT32 kk = 0; (kk < (UINT32)m_taskTerminateTime/100) && (status == BEP_STATUS_SUCCESS); kk++)
							{
								status = BposIsTaskAlive(registeredName[jj].c_str());
								Log(LOG_DEV_DATA, "Status of task %s is %s\n", registeredName[jj].c_str(), status == BEP_STATUS_SUCCESS ? "Up" : "Down");
								if (status == BEP_STATUS_SUCCESS)		 // if up, delay
									BposSleep(100);
							}
							if (status == BEP_STATUS_SUCCESS)			 // if the server is still alive
							{
								Log(LOG_ERRORS, "Launcher Could Not Terminate: %s\n", registeredName[jj].c_str());

								// look up the pid
								TASKNAMELISTITR taskNameIter = m_taskNameList.find(registeredName[jj]);
								if (taskNameIter != m_taskNameList.end())
								{
									Log(LOG_DEV_DATA, "Launcher Had To Kill: %d, %s\n", processID[jj], (*taskNameIter).second.c_str());
									if (processID[jj] != BP_ERR_KRNL_FAIL)
									{
										int retry = 0;
										int status;
										do
										{
											status = kill(processID[jj], SIGKILL);
											if (status == -1) BposSleep(100);
										}while ((retry++ < 3) && (status == -1));
										if (status == -1)
											Log(LOG_ERRORS, "ComponentProxy Error Terminating %s: %s\n", (*taskNameIter).second.c_str(), strerror(errno));
									}
									Log(LOG_DEV_DATA, "Launcher Killed %s\n", (*taskNameIter).second.c_str());
								}
								else
									Log(LOG_ERRORS, "%s Not In Termination List\n", registeredName[jj].c_str());
							}
							else										// else successfull termination
								Log(LOG_DEV_DATA, "Launcher Terminated: %s\n", registeredName[jj].c_str());
						}
						else Log(LOG_ERRORS, "Launcher Could Not Command %s To Terminate\n", registeredName[jj].c_str());
					}
					catch (XmlException &e)
					{
						Log(LOG_ERRORS, "%s XmlException Terminating: %s\n", (*iter).second.registeredName.c_str(), e.what());
					}
					catch (BepException &e)
					{
						Log(LOG_ERRORS, "%s BepException Terminating: %s\n", (*iter).second.registeredName.c_str(), e.what());
					}
					catch (...)
					{
						Log(LOG_ERRORS, "Unknown Error Terminating: %s\n", (*iter).second.registeredName.c_str());
					}
				}
			}
		}
		Log(LOG_DEV_DATA, "All Servers Terminated\n");
	}

	Log(LOG_FN_ENTRY, "System Terminated.\n");

};

/**
 * Method used to shutdown the system.  It will first
 * terminate the servers on the system and then perform
 * a system shutdown.
 * @since Version 2.5
 */
void Launcher::Shutdown(void)
{
	Log(LOG_ALL, "Shutdown commanded Terminating and Shutting Down the system\n");
	// if a shutdown received
	if ( (GetStatus() == BEP_SHUTDOWN_STATUS) )
	{
		// if shutdown enabled in the config
		if (m_enableSystemShutdown)
		{
			printf("Performing System Shutdown\n");
			Terminate();
			system("shutdown -S system");
		}
		else
			printf("Not Performing System Shutdown: Disabled\n");
	}
	// else terminate the system
	else
		Terminate();

};


/**
 * Request to read a data tag
 *
 * @param node   The data to read.
 * @return Value of tag
 * @since Version 2.0
 */
const std::string Launcher::Read(const XmlNode *node)
{
	return(std::string("Not Supported"));
};

/**
 * Request to write a data tag
 *
 * @param node   The write to perform.
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Launcher::Write(const XmlNode *node)
{
	return(std::string("Not Supported"));
};

/**
 * Request to Perform an action
 *
 * @param node   The command to perform.
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Launcher::Command(const XmlNode *node)
{
	string status = BEP_SUCCESS_RESPONSE;

	Log(LOG_DEV_DATA, "Command Received: %s, %s\n", node->getName().c_str(), node->getValue().c_str());

	string tag = node->getName();

	// if the command is a register command, send it to the managed processes
	if (tag == SYSTEM_RELOAD_DATA_TAG)			 ReloadConfiguration();
	// else if the command is to shutdown the system
	else if (tag == SYSTEM_SHUTDOWN_DATA_TAG)
	{	// shutdown enabled and a shutdown received
		if (m_enableSystemShutdown && (!node->getValue().compare("1")))
			SetStatus(BEP_SHUTDOWN_STATUS);
	}

	return(status);
};

/**
 * Request to be inserted into the list to be notified
 * of a change in a data tag
 *
 * @param tag      Tag for who is requesting the subscription informaiton.
 * @param process  The process name of who wants the data
 *               requested.
 * @return Status of operation
 *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
 *         BEP_FAILURE_RESPONSE == Error adding subscriber
 * @since Version 2.0
 */
const std::string Launcher::Subscribe(const std::string &tag, const std::string &Process)
{
	return(std::string("Not Supported"));
};

/**
 * Request to be removed from the list to be notified
 * of a change in a data tag
 *
 * @param tag     Tag for who is requesting to be unsubscribed.
 * @param Process The process name of who wanted the data
 * @return Status of operation
 *         BEP_SUCCESS_RESPONSE == Subscriber successfully removed
 *         BEP_FAILURE_RESPONSE == Error removing subscriber
 * @since Version 2.0
 */
const std::string Launcher::Unsubscribe(const std::string &tag, const std::string &Process)
{
	return(std::string("Not Supported"));
};

/**
 * Handle the subscribed for data by the process.
 *
 * @param tag    Tag that was subscribed for
 * @param value  New data value of the tag
 * @return Status of the updating of the published tag
 *         Success, Fail
 * @since Version 2.1
 */
const std::string Launcher::Publish(const std::string &tag, const std::string &value)
{
	return(std::string("Not Supported"));
};

bool Launcher::GetTasksToLaunch(const XmlNode *document)
{
	bool foundAllTasks = true;
	std::string taskkey;

	// if there are tasks to launch, retrieve them from the config file
	if (document->getChildren().getNode("TasksToLaunch")->getChildren().size())
	{
		TASKINFO task;	// the task to add
		const XmlNode *taskList = document->getChildren().getNode("TasksToLaunch");
		const XmlNode *taskInfo;
		Log(LOG_DEV_DATA, "Launcher Managing %d Tasks\n", taskList->getChildren().size());
		std::string ftpEnvironment = getenv("FTP_ROOT");		// variable to hold the ftp path
		std::string programEnvironment = getenv("USR_ROOT");	// variable to hold the user path
		for (UINT32 ii = 0; ii < taskList->getChildren().size(); ii++)
		{
			taskInfo = taskList->getChildren().getNode(ii);
			// retrieve the process to launch
			task.processName = taskInfo->getAttributes().getNode("Name")->getValue();
			task.process = programEnvironment + taskInfo->getAttributes().getNode("Path")->getValue() + task.processName;
			// check for the existence of the file
			if (access(task.process.c_str(), F_OK) == BEP_STATUS_SUCCESS)
			{	// retrieve the registered name
				task.registeredName = taskInfo->getAttributes().getNode("RegisteredName")->getValue();
				try
				{
					task.registerTask = (taskInfo->getAttributes().getNode("Register")->getValue() == "No")? false : true;
				}
				catch (...)
				{	// always default to sending a task the register message
					task.registerTask = true;
				}

				// set the status
				task.status = BEP_INITIALIZED_STATUS;

				/* ******** set up the arguments list ******** */
				// add the process to the arguments list
				task.arguments.erase();		//clear out the arguments

				// retrieve the configuratio file to pass if needed
				if (!taskInfo->getAttributes().getNode("ConfigFile")->getValue().empty())
				{
					task.arguments += " -f" + ftpEnvironment;
					task.arguments += taskInfo->getAttributes().getNode("ConfigPath")->getValue() +
									  taskInfo->getAttributes().getNode("ConfigFile")->getValue();
				}
				try
				{	// set the demo status, Demo Mode Flag (-d)
					if (taskInfo->getAttributes().getNode("DemoMode")->getValue() == "On")
						task.arguments += " -d";
				}
				catch (XmlException &e)
				{
				}

				try
				{	// set the console status, Console Mode Flag (-c)
					if (taskInfo->getAttributes().getNode("ConsoleMode")->getValue() == "On")
						task.arguments += " -c";
				}
				catch (XmlException &e)
				{
				}

				try
				{	// set the address value, Address Value (-a)
					if (!taskInfo->getAttributes().getNode("AddressValue")->getValue().empty())
						task.arguments += " -a" + taskInfo->getAttributes().getNode("AddressValue")->getValue();
				}
				catch (XmlException &e)
				{
				}

				try
				{	// set the port value, Port Value (-p)
					if (!taskInfo->getAttributes().getNode("PortValue")->getValue().empty())
						task.arguments += " -p" + taskInfo->getAttributes().getNode("PortValue")->getValue();
				}
				catch (XmlException &e)
				{
				}

				try
				{	// set the verbose mode value, Verbosity Level Value (-v)
					if (!taskInfo->getAttributes().getNode("VerbosityLevel")->getValue().empty())
						task.arguments += " -v" + taskInfo->getAttributes().getNode("VerbosityLevel")->getValue();
				}
				catch (XmlException &e)
				{
				}

				// get task key
				try
				{
					taskkey = taskInfo->getAttributes().getNode("OrderToLaunch")->getValue();
				}
				catch (...)
				{
					Log(LOG_ERRORS, "Missing OrderToLaunch: %s\n", task.process.c_str());
					throw BepException("Launcher Task Missing OrderToLaunch");
				}

				// check to see if task key already exists in map
				if (m_taskTable.find(taskkey) == m_taskTable.end())
				{
					// put the information into the table
					m_taskTable.insert(TABLEVAL(taskkey, task));
					Log(LOG_DEV_DATA, "Process: %s\nArguments: %s\n",
						task.process.c_str(), task.arguments.c_str());
					// store the registered name and the program name in case the process will have to be killed
					m_taskNameList.insert(TASKNAMELISTVAL(task.registeredName,
														  taskInfo->getAttributes().getNode("Name")->getValue()));
				}
				else
				{
					// duplicate task key... let everyone know
					foundAllTasks = false;
					Log(LOG_ERRORS, "Duplicate OrderToLaunch: %s %s\n", taskkey.c_str(), task.process.c_str());
					// do not let the system run
					throw BepException("Launcher Duplicate Task Order Error");
				}

			}
			else
			{
				foundAllTasks = false;
                // because the logger object is not running yet, and we do want these in the
                // disk based log file, push the filename into a list. retrieve it later.
                m_missingTasks.insert(TASKNAMELISTVAL(task.process, task.process));
				Log(LOG_ERRORS, "Error Finding File: %s\n", task.process.c_str());
			}
		}
	}
	else
		throw BepException("Launcher No Tasks To Launch");

	return(foundAllTasks);
}

