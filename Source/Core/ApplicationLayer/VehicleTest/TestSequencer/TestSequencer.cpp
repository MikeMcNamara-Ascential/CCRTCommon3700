//*************************************************************************
// Description:
//  The Sequencer is responsible for sequencing a vehicle test.  It will launch
//  a set of VehicleComponentManagers and then use them to command test objectives
//  in the system.  At the end of each Objective the sequencer will evaluate the
//  oveall Pass or Failure of the test and either sequence the phase again or
//  continue.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/VehicleTest/TestSequencer/TestSequencer.cpp $
// 
// 1     11/02/07 10:30a Derickso
// Updated destructor to terminate all running components.
// 
// 11    10/25/06 4:25p Rwiersem
// Changes for the 06102701 core release:
// 
// - Added a "reuseComponents" variable.  This is used to keep the test 
//   components running when a new drive curve starts.
// 
// 10    5/08/06 7:18p Cward
// Changed printf statements to Log statements.
//
// 9     4/13/06 3:34a Cward
// Init vars
//
// 2     2/15/06 3:50p Bmeinke
// Initialize some uninitialized variables
//
// 8     7/20/05 8:40p Cward
// Fixed Initialization Bug
//
// 7     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 6     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 5     9/29/04 10:19p Cward
// Added wait for zero spreed after abort processing
//
// 4     9/01/04 9:30p Cward
//
// 3     6/09/04 11:30p Cward
// Refactored App layer inlcudes to remove implementation Updated header
// comment block to the new standard
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:33p Cward
//
// 5     10/24/03 11:10a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 4     8/14/03 10:41a Khykin
// Updated with make install changes.
//
// 2     8/06/03 3:11p Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/24/03 7:11a Khykin
// Added more verbosity in error handling.
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:22a Khykin
// Initial checkin.
//
// 3     5/28/03 5:33p Khykin
// Adding retest question timeout parameter.
//
// 2     4/30/03 9:22a Khykin
// Added the localization of the drive curve, update the logging, and
// added the checking of machine status prior to sequencing a new test.
//
// 1     4/22/03 10:33a Khykin
// Initial Working Checkin.
//
// 3     4/07/03 9:20a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:12  tbochene
//
//    PooBag
//
//
// 2     2/08/03 6:51a Khykin
// Added fault handling for the case were only a backround objective is
// sequenced.
//
// 1     1/27/03 11:16a Khykin
// Initial Checkin
//
// 17    11/11/02 2:59p Khykin
// Updated fault handling.
//
// 16    11/07/02 6:15a Khykin
// Updated sequencer to handle invalid drive curve tags more gracefully.
//
// 15    10/28/02 12:45p Khykin
// Updated to reset the background test status when retest is selected.
//
// 14    10/25/02 6:12a Khykin
// Updated the test sequencer for managing background tasks that report
// results asynchronously.
//
// 13    10/21/02 11:18a Khykin
// Updated for the ability to have the VehicleTest indicate if a retest
// can be performed. (This was to support no retests on LossCompensation
// Tests)
//
// 12    9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 11    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 9     5/24/02 1:50p Kh
// Updating due to debugging.
//
// 8     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 7     3/06/02 1:34p Kh
// Making changes for logging and for test phases.
//
// 6     11/20/01 10:45a Kh
// Updating for changes to the vehicle component.
//
// 5     8/28/01 11:08a Kh
// Updating for NTO 6.1
//
// 4     8/17/01 2:36p Kh
// Checking in for weekly backup.
//
// 2     8/02/01 4:35p Kh
// Checking in incremental development file. (Not Debugged or Tested)
//
// 1     7/06/01 5:34p Kh
// Checking in the weeks work.  Not functional just written and in the
// process of debug.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TestSequencer.h"

TestSequencer::TestSequencer() : BepComponent(),
m_driveCurve(NULL), m_currentPhase(NULL), m_ndb(NULL), m_prompt(NULL),
m_retestPossible(true), m_overallTestResultPromptBox(1), m_reuseComponents( false), 
m_interObjectiveDelay(100)
{
}

TestSequencer::~TestSequencer()
{	// if a drive curve was loaded
	Log(LOG_FN_ENTRY, "~TestSequencer Terminating\n");
	// Ensure the components have been terminated
	m_compManager.TerminateAll(true);
	// clean up the interfaces
	if(m_ndb != NULL)		delete m_ndb;
	if(m_prompt != NULL)	delete m_prompt;
	Log(LOG_FN_ENTRY, "~TestSequencer Terminating Done\n");
}

void TestSequencer::Initialize(const XmlNode *configFile)
{	// initialize the base component
	BepComponent::Initialize(configFile);

	try
	{	// set the retest type
		m_retestType = configFile->getChild("RetestType")->getValue();
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "TestSequencer RetestType Not Set\n");
		m_retestType = "None";
	}

	try
	{	// set the maximum allowed retests
		m_maxRetests = atoi(configFile->getChild("MaxRetests")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "TestSequencer MaxRetests Not Set\n");
		m_maxRetests = 1;
	}

	try
	{	// set the retest timeout value
		m_retestTimeout = atoi(configFile->getChild("RetestQuestionTimeout")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "TestSequencer Retest Timeout Not Set\n");
		m_retestTimeout = 10000;
	}
	// Get the prompt box for the RetestYesOrNo prompt
	try
	{	// set the prompt box that the RetestYesOrNo will be placed in
		m_retestYesOrNoPromptBox = atoi(configFile->getChild("RetestYesOrNoPromptBox")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		m_retestYesOrNoPromptBox = 2;
	}
	// Get the prompt box for the overall test result prompt
	try
	{	// set the prompt box that the overall test result will be placed in
		m_overallTestResultPromptBox = atoi(configFile->getChild("OverallTestResultPromptBox")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		m_overallTestResultPromptBox = 1;
	}
	// Get the prompt box for the Loading parameters prompt
	try
	{	// set the prompt box that the LoadingParameters will be placed in
		m_loadingParametersPromptBox = atoi(configFile->getChild("LoadingParametersPromptBox")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		m_loadingParametersPromptBox = 1;
	}
	// Set the command objective delay time
	try
	{
		SetInterObjectiveDelay(atoi(configFile->getChild("InterObjectiveCommandDelay")->getValue().c_str()));
	}
	catch(XmlException &e)
	{
		SetInterObjectiveDelay(100);
	}

	try
	{
		m_reuseComponents = atob( configFile->getChild("ReuseComponents")->getValue().c_str());
	}
	catch(XmlException &e)
	{
		m_reuseComponents = false;
	}
	// Determine if PLC result buttons are in use
	bool plcButtons = false;
	try
	{
		plcButtons = atob(configFile->getChild("UsePlcResultButtons")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not determine if PLC result buttons are in use, defaulting to no PLC result buttons: %s", 
			excpt.GetReason());
		plcButtons = false;
	}
	Log(LOG_DEV_DATA, "PLC Buttons in use: %s", plcButtons ? "True" : "False");
	UsePlcResultButtons(&plcButtons);
	if(UsePlcResultButtons())
	{
		string buttonTag("YesPB");
		try
		{
			buttonTag = configFile->getChild("PlcPassButtonTag")->getValue().c_str();
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Pass button tag not defined, using YesPB - %s", excpt.GetReason());
			buttonTag = "YesPB";
		}
		PlcPassButtonTag(&buttonTag);
		try
		{
			buttonTag = configFile->getChild("PlcFailButtonTag")->getValue().c_str();
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Fail button tag not defined, using NoPB - %s", excpt.GetReason());
			buttonTag = "NoPB";
		}
		PlcFailButtonTag(&buttonTag);
	}
	// initialize the component managers with the logging interface
	m_compManager.SetLoggingInterface((ILogger *) this);
	m_backCompManager.SetLoggingInterface((ILogger *) this);

	// ensure that the interfaces are null
	if(m_ndb != NULL)		delete m_ndb;
	if(m_prompt != NULL)	delete m_prompt;
};

const std::string TestSequencer::Register(void)
{
	Log(LOG_FN_ENTRY, "Registering the TestSequencer\n");
	// set up the interfaces for reporting results
	if(m_ndb == NULL)			//  the ndb interface to null
	{
		m_ndb = new INamedDataBroker(IsDebugOn());
	}
	if(m_prompt == NULL)		//  the prompt interface to null
	{
		m_prompt = new IPromptServer();
		m_prompt->Initialize(PROMPT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, IsDebugOn());
	}
	return(BEP_SUCCESS_RESPONSE);
}

void TestSequencer::LoadDriveCurve(const XmlNode *driveCurve, string driveCurveRootDir)
{
	// prepend the FTP_ROOT to the drive curve for loading
	Log(LOG_FN_ENTRY, "Loading The Drive Curve: %s\n", driveCurve->getName().c_str());
	std::string msgResponse;
	try
	{	// prompt driver that parameters are being loaded
		m_prompt->DisplayPrompt(m_loadingParametersPromptBox, "LoadingTestParameters", msgResponse);
		// reference the drive curve
		m_driveCurve = driveCurve->getChild("Test");
		// set debug status
		SetDebug(m_driveCurve->getAttributes().getNode("Debug")->getValue());
		// initialize the component manager and backup component manager
		const XmlNode *components = driveCurve->getChild("Components");
		Log(LOG_DEV_DATA, "Initializing The Component Manager\n");
		Log(LOG_DETAILED_DATA, "MEM: TestSequencer::LoadDriveCurve()1 - %d\n",ReturnMemArena());
		m_compManager.Initialize(components, driveCurveRootDir, m_reuseComponents);
		Log(LOG_DETAILED_DATA, "MEM: TestSequencer::LoadDriveCurve()2 - %d\n",ReturnMemArena());
		Log(LOG_DEV_DATA, "Initializing The Background Component Manager\n");
		m_backCompManager.Initialize(components,  driveCurveRootDir, m_reuseComponents);
		Log(LOG_DETAILED_DATA, "MEM: TestSequencer::LoadDriveCurve()3 - %d\n",ReturnMemArena());
		m_prompt->RemovePrompt(m_loadingParametersPromptBox, "LoadingTestParameters", msgResponse);
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "Error loading the components: %s\n", e.what());
		m_prompt->RemovePrompt(m_loadingParametersPromptBox, "LoadingTestParameters", msgResponse);
		throw BepException("Error Loading Drive Curve: %s error: %s\n",
						   driveCurve->getName().c_str(), e.what());
	}

	Log(LOG_FN_ENTRY, "Drive Curve Loaded: %s\n", driveCurve->getName().c_str());
};

const std::string TestSequencer::SequenceTest(const std::string type)
{
	Log(LOG_FN_ENTRY, "Sequencing Test All Set Up\n");

	// before the test starts, get a reference to the overall pass/fail and set it to fail
	XmlNode *result = const_cast<XmlNode *> (m_driveCurve->getAttributes().getNode("Result"));
	std::string testStatus="Pass";			// set the overall test status to pass
	m_backgroundObjectiveStatus = "Pass";	// set the background test status to pass
	int retestNumber = 0;					// the number of retests that have been performed
	m_currentPhaseNumber=0;					// reset the current phase number
	std::string response;					// response buffer
	bool atZeroSpeed=false;					// at zerospeed flag
	std::string msgResponse;				// response for ndb read

	// retest loop
	do
	{
		std::string currentTestStatus;							// the status of the current test
		Log(LOG_ERRORS, "Resetting The Abort Status\n");
		m_ndb->Write(ABORT_DATA_TAG, "0", response, true);
		SetStatus(BEP_TESTING_STATUS);							// set the test status to testing
		INT32 totalPhases = m_driveCurve->getChildren().size();	// get total number of phases to run
		Log(LOG_DEV_DATA, "TotalPhases: %d\n", totalPhases);
		bool retested=false, incycleRetest=false;				// the retest flags

		// set the overall status of the test to Fail
		result->setValue("Fail");
		// clear out the failure string
		SetFailureString("");
		// Clear the abort flag at the beginning of the test sequence
		Log(LOG_ERRORS, "Clearing the Abort status at the beginning of the sequence");
		m_ndb->Write(ABORT_DATA_TAG, "0", response, true);
		BposSleep(500);	  // Wait for the abort flag to be reset
		// perform vehicle test as long as there are more test steps to perform and can test
		for(;(m_currentPhaseNumber < totalPhases) && CanTest(testStatus) &&
		   (GetStatus() == BEP_TESTING_STATUS) ; m_currentPhaseNumber++)
		{
			try
			{	// walk through the phases until complete
				Log(LOG_DEV_DATA, "Current Test Phase: %d, Total: %d\n", m_currentPhaseNumber, totalPhases);
				try
				{	// get the test phase information
					m_currentPhase = m_driveCurve->getChildren().getNode(m_currentPhaseNumber);
				}
				catch(XmlException &e)
				{	// if the phase does not exist set it to NULL
					m_currentPhase = NULL;
				}

				if(m_currentPhase)	// if there is a phase to sequence
				{   // Reset the in-cycle retest counter
					if(!m_retestType.compare("Incycle") && !incycleRetest)
					{
						retestNumber = 0;
					}
					Log(LOG_DEV_DATA, "Sequencing: %s\n", m_currentPhase->ToString().c_str());
					// if there are any tests to run
					if(ResetPhase(incycleRetest))				// reset the phase in the drive curve
					{
						// sequence them
						Log(LOG_DEV_DATA, "Sequencing The Phase: %s\n", m_currentPhase->getName().c_str());
						if(SequencePhase(incycleRetest))	// if there were any foreground tests sequenced
						{
							Log(LOG_DEV_DATA, "Processing The Results\n");
							currentTestStatus = ProcessResults();
							Log(LOG_DEV_DATA, "Current Test Status: %s\n", currentTestStatus.c_str());

							if(PerformIncycleRetest(currentTestStatus, retestNumber))
							{
								Log(LOG_DEV_DATA, "Performing Retest Phase %s, Result: %s, Overall: %s\n", m_currentPhase->getName().c_str(),
									currentTestStatus.c_str(), testStatus.c_str());
								currentTestStatus = "Pass";			// set the current test status to Pass
								incycleRetest = true;				// flag that an incycle retest has been performed
								retestNumber++;						// increment the retest counter
							}
							else
							{
								incycleRetest = false;
								UpdateCurrentStatus(currentTestStatus, testStatus);
								Log(LOG_DEV_DATA, "Phase %s, Result: %s, Overall: %s\n", m_currentPhase->getName().c_str(),
									currentTestStatus.c_str(), testStatus.c_str());
							}
						}
						else
							Log(LOG_DEV_DATA, "No Foregound Test Sequenced For: %s\n", m_currentPhase->getName().c_str());
					}
					else
						Log(LOG_DEV_DATA, "Nothing To Sequence In: %s\n", m_currentPhase->getName().c_str());
				}
			}
			catch(BepException &e)
			{
				Log(LOG_ERRORS, "TestSequencer: Phase: %s, Reason: %s\n", m_currentPhase->getName().c_str(), e.what());
				// update the failure string
				char buffer[128];		// buffer to create messages with
				SetFailureString(CreateMessage(buffer, sizeof(buffer), "TestSequencer: Phase: %s, Reason: %s\n", m_currentPhase->getName().c_str(), e.what()));
				SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
				testStatus = BEP_SOFTWAREFAIL_STATUS;	// set the sequence status to software fail
			}
			catch(...)
			{
				Log(LOG_ERRORS, "TestSequencer: Unknown Exception\n");
				// update the failure string
				char buffer[128];		// buffer to create messages with
				SetFailureString(CreateMessage(buffer, sizeof(buffer), "TestSequencer: Software Failure Occurred Sequencing Phase: %s\n", m_currentPhase->getName().c_str()));
				SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
				testStatus = BEP_SOFTWAREFAIL_STATUS;	// set the sequence status to software fail
			}

			// check if software failure occurred, if so, abort
			if(testStatus == BEP_SOFTWAREFAIL_STATUS)
			{
				Log(LOG_ERRORS, "Software Failure Occurred Sequencing The Test\n");
				try
				{
					m_compManager.AbortAll();				// if an issue detected, abort all of the test
				}
				catch(...)
				{
					Log(LOG_ERRORS, "Unable To Abort All Components\n");
				}
				SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
			}
		}

		if(testStatus == BEP_FATALFAIL_STATUS)
		{
			Log(LOG_ERRORS, "Fatal failure detected, aborting test\n");
			try
			{
				m_compManager.AbortAll();				// if an issue detected, abort all of the test
			}
			catch(...)
			{
				Log(LOG_ERRORS, "Unable To Abort All Components\n");
			}
			SetStatus(BEP_FATALFAIL_STATUS);		// set the component status to a software fail
		}


		// signal that the end of the Foreground Tests have been completed
		m_ndb->Write(FOREGROUNDTEST_IN_PROG_DATA_TAG, "0", response, true);

		// Even after the whole test is complete, there might be background objectives still running
		if(m_backCompManager.NumberOfObjectivesLeft() && (GetStatus() == BEP_TESTING_STATUS))
		{	// if there are background objectives left, wait for them and update the overall results
			Log(LOG_DEV_DATA, "Processing Background Objectives Left: %d\n", m_backCompManager.NumberOfObjectivesLeft());
			// process the results
			std::string backgroundStatus = ProcessBackgroundResults();
			// update the status
			UpdateCurrentStatus(backgroundStatus, m_backgroundObjectiveStatus);
			Log(LOG_DEV_DATA, "Background Objectives Finished\n");
		}
		else if(GetStatus() == BEP_TESTING_STATUS)
			Log(LOG_DEV_DATA, "Background Objectives Already Complete\n");

		// update the overall test status
		UpdateCurrentStatus(m_backgroundObjectiveStatus, testStatus);

		// last check is to determine if the Status is Testing or if there is another
		// reason for the completion of testing
		if(GetStatus() != BEP_TESTING_STATUS)
			UpdateCurrentStatus(GetStatus(), testStatus);

		// update the GUI with the pass fail message
		DisplayOverallTestPrompt(testStatus);

		// If an Abort occurred, reset the status
		if(testStatus == BEP_ABORT_RESPONSE)
		{
			// check if we are at zerospeed
			if(m_ndb->Read(ZEROSPEED_TAG_TO_PLC, response, true) == BEP_STATUS_SUCCESS)
			{
				string tag, value;
				m_ndb->GetNext(tag, value, response);
				if(value != "1")	// not at zerospeed
				{
					// Make sure no other prompts are displayed
					string response;
					m_prompt->ClearPromptBox(1, response);
					m_prompt->ClearPromptBox(2, response);
					// prompt to driver to stop
					m_prompt->DisplayPrompt(1, "BrakeToStop", msgResponse);
					atZeroSpeed = false;
					while(atZeroSpeed == false)
					{
						// check if we are at zerospeed
						if(m_ndb->Read(ZEROSPEED_TAG_TO_PLC, response, true) == BEP_STATUS_SUCCESS)
						{
							string tag, value;
							m_ndb->GetNext(tag, value, response);
							if(value == "1")
							{
								atZeroSpeed = true;
							}
							else
							{
								// not yet at zero speed, sleep a little
								BposSleep(100);
							}
						}
						else
						{
							// should never get here, something is wrong ... get out of the loop
							break;
						}
					}
					// zerospeed reached
					// clear prompt
					m_prompt->RemovePrompt(1, "BrakeToStop", msgResponse);
				}
			}

			Log(LOG_ERRORS, "Aborting The TestComponents\n");
			try
			{
				m_compManager.AbortAll();				// if an issue detected, abort all of the test
			}
			catch(...)
			{
				Log(LOG_ERRORS, "Unable To Abort All Components\n");
			}
			Log(LOG_ERRORS, "Resetting The Abort Status\n");
			m_ndb->Write(ABORT_DATA_TAG, "0", response, true);
		}
		// else if not aborted, update the PLC with complete
		// Note: this is implemented because in some plants, when the test is complete
		// the retainers are automatically lowered.
		else
		{
			// Send test complete to the PLC
			m_ndb->Write(string("CompleteLamp"),"1",response,true);
		}

		// check to determine if completed testing, if not ask for retest
		if(TestComplete(testStatus, retestNumber) == false)
		{
			retested = true;						// indicate that a retest is necessary
			testStatus="Pass";						// set the overall test status to pass
			currentTestStatus="Pass";				// the status of the current test
			m_backgroundObjectiveStatus = "Pass";	// the status of the background tests
			retestNumber++;							// increment the number of retests performed
            char startRetestVal[11];
            sprintf(startRetestVal,"%d",retestNumber);
            m_ndb->Write(string("StartRetest"),startRetestVal,response,true);   // set the StartRetest tag to the current retestNumber
		}
	} while((GetStatus() != BEP_COMPLETE_STATUS) && (GetStatus() != BEP_SOFTWAREFAIL_STATUS));

    m_ndb->Write(string("StartRetest"),"0",response,true);           // clear the StartRetest tag
	result->setValue(testStatus);		// set the overall result value
	return(testStatus);
};

void TestSequencer::RemoveTestComponents(void)
{
	Log(LOG_FN_ENTRY, "RemoveTestComponents() - Removing all test components\n");
	m_compManager.RemoveComponents();
	Log(LOG_FN_ENTRY, "RemoveTestComponents() - Removing all background test components\n");
	m_backCompManager.RemoveComponents();
	Log(LOG_FN_ENTRY, "RemoveTestComponents() - Done removing all test components\n");
}

const INT32 TestSequencer::SequencePhase(bool incycleRetest)
{
	Log(LOG_FN_ENTRY, "SequencePhase: %s\n", m_currentPhase->getName().c_str());
	int foregroundObjectivesSequenced = 0;
	const XmlNode *objective;	// the objective to sequence
	const XmlNode *foreground;	// the foreground objectives to sequence
	const XmlNode *background;	// the background objectives to sequence

	// sequence the foreground objectives
	try
	{
		foreground = m_currentPhase->getChildren().getNode("ForegroundObjectives");
		foregroundObjectivesSequenced = foreground->getChildren().size();
	}
	catch(XmlException &e)
	{	// if it does not exist, set it to null
		foreground = NULL;
	}

	if(foreground != NULL)
	{
		const XmlNodeMap&   fgObjs = foreground->getChildren();
		XmlNodeMapCItr      objItr;
		for( objItr=fgObjs.begin(); objItr!=fgObjs.end(); objItr++)
		{
			// get the objective to sequence and command it
			objective = (XmlNode*)objItr->second;
			Log(LOG_DEV_DATA, "Commanding Foreground Objective: %s\n", objective->getName().c_str());
			m_compManager.CommandObjective(objective->Copy());
			BposSleep(GetObjectiveCommandDelay());
		}
	}

	// sequence the background tests if not in an Incycle retest
	if(!incycleRetest)
	{
		try
		{
			background = m_currentPhase->getChildren().getNode("BackgroundObjectives");
		}
		catch(XmlException &e)
		{	// if it does not exist, set it to null
			background = NULL;
		}
		if(background != NULL)
		{
			for(UINT32 jj=0; jj < background->getChildren().size(); jj++)
			{	// get the test to sequence and command it
				objective = background->getChildren().getNode(jj);
				Log(LOG_DEV_DATA, "Commanding Background Objective: %s\n", objective->getName().c_str());
				m_backCompManager.CommandObjective(objective->Copy());
			}
		}
	}
	Log(LOG_FN_ENTRY, "SequencePhase Complete\n");

	return(foregroundObjectivesSequenced);	// return the number of foreground objective sequenced
};

/**
 * Process the results of the sequenced test objectives.
 * This method will wait until all of the objectives
 * in this test phase have reported their status.
 *
 * @param objectives The test phase that was sequenced.
 * @return The status of the test objectives.
 */
const std::string TestSequencer::ProcessResults(void)
{
	INT32 chid;								// the channel ID of the process that sent a message
	std::string status;						// the status of the currentPhase
	std::string testStatus;					// the status of the last test performed
	std::string message;					// message receive from the client
	IBepCommunication commHandler;			// create a new handler
	bool done = false;						// done flag, set to true when all message received
	uint64_t timeout = mSEC_nSEC( 1000LL);	// the timeout value in nS

	Log(LOG_FN_ENTRY, "ProcessResults\n");

	try
	{	// wait for the commanded messages to be received as long as we are not terminated
		while((GetStatus() != BEP_TERMINATE_STATUS) && (!done))
		{	// wait for a request to be received
			chid = m_sequencerComm->WaitForRequestWithTimeout(timeout);

			// if not a timeout event
			if(chid != -1)
			{
				Log(LOG_DEV_DATA, "Message Received: %d\n", chid);
				commHandler.Initialize(*m_sequencerComm);	// initialize it with the server
				// handle the request
				message.clear();	// clear out the message buffer

				if(commHandler.HandleRequest(message, this) == BEP_STATUS_SUCCESS)
				{
					if(chid > 0)	// if from a client (not a pulse)
					{
						Log(LOG_DEV_DATA, "Received Message: %s\n", message.c_str());
						// if a write, update the drive curve
						if(commHandler.GetRequestType(message) == BEP_WRITE)
						{	// validate the mesage and update the drive curve
							testStatus = UpdateTestStatus(&commHandler);
							// if there was a software failure, set the status
							if(testStatus == BEP_SOFTWAREFAIL_RESPONSE)
								SetStatus(BEP_SOFTWAREFAIL_STATUS);
						}
						else
							Log(LOG_ERRORS, "Unexpected Message: %s\n", message.c_str());

						// check the test phase status
						if((status = TestPhaseStatus()) != BEP_TESTING_RESPONSE)
							done = true;
						Log(LOG_DEV_DATA, "ProcessResults status: %s\n", status.c_str());
					}
				}
				else Log(LOG_ERRORS, "Wacked Message Received %s\n", message.c_str());
			}
			else
				Log(LOG_DEV_DATA, "ProcessResults Wakeup Event, Check %s Testing Status: %s\n",
					m_currentPhase->getName().c_str(), GetStatus().c_str());
		}
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "ProcessResults Exception: %s\n", e.Reason().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "ProcessResults Exception: %s\n", e.Reason().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	return(status);
};

const std::string TestSequencer::ProcessBackgroundResults(void)
{
	INT32 chid;								// the channel ID of the process that sent a message
	std::string status;						// the status of the background tests
	std::string objStatus;					// status of the received objective
	std::string message;					// message receive from the client
	IBepCommunication commHandler;			// communication handler use to process messages
	uint64_t timeout = mSEC_nSEC( 1000LL);	// the timeout value in nS

	Log(LOG_FN_ENTRY, "Processing Background Results: %d\n", m_backCompManager.NumberOfObjectivesLeft());
	// wait for the commanded messages to be received as long as we
	// are Testing
	try
	{	// as long as there are outstanding objectives and we are not terminated, getem
		while(m_backCompManager.NumberOfObjectivesLeft() && (GetStatus() != BEP_TERMINATE_STATUS))
		{	// wait for the response until the timeout time hits
			chid = m_sequencerComm->WaitForRequestWithTimeout(timeout);	// wait for a request to be received
			if(chid != -1)
			{
				commHandler.Initialize(*m_sequencerComm);	// initialize it with the server
				// handle the request
				message.clear();	// clear out the message buffer
				if(commHandler.HandleRequest(message, this) == BEP_STATUS_SUCCESS)
				{
					if(chid > 0)	// if from a client (not a pulse)
					{
						if(commHandler.GetRequestType(message) == "Write")
							objStatus = UpdateTestStatus(&commHandler);	// validate mssg and update the drive curve
						else
							Log(LOG_ERRORS, "Unexpected Message: %s\n", message.c_str());

						// check the test status
						UpdateCurrentStatus(objStatus, status);
					}
					else
						Log(LOG_DEV_DATA, "Waiting for the NumberOfObjectivesLeft %d == 0\n", m_backCompManager.NumberOfObjectivesLeft());
				}
				else
					Log(LOG_ERRORS, "Unknown Error Receiving Background Results: %s\n", message.c_str());
			}
			else
				Log(LOG_DEV_DATA, "ProcessBackgroundResults Timeout, Check For End Of Testing: %d\n", m_backCompManager.NumberOfObjectivesLeft());
		}
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "ProcessBackgroundResults XmlException: %s\n", e.what());
		status = "Exception";
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "ProcessBackgroundResults BepException: %s\n", e.what());
		status = "Exception";
	}
	Log(LOG_FN_ENTRY, "Processing Background Results Complete: %d\n", m_backCompManager.NumberOfObjectivesLeft());
	return(status);
};


const std::string TestSequencer::UpdateTestStatus(IBepCommunication *commHandler)
{	// check the message to determine if it is a results message
	std::string status = "";	// the status of the message that was received
	std::string tag;			// the message tag
	std::string value;			// the value of the tag
	std::string message;		// the remainder of the message
	// determine the type of objective
	tag = "Type";
	commHandler->GetByTag(tag, value, message);
	Log(LOG_DEV_DATA, "UpdateTestStatus Received: %s\n", value.c_str());
	if(value == "Foreground")
		status = UpdateForegroundObjectiveStatus(commHandler);
	else if(value == "Background")
		status = UpdateBackgroundObjectiveStatus(commHandler);
	else
		throw BepException("UpdateTestStatus Invalid Message Type: %s, %s", tag.c_str(), value.c_str());

	return(status);
}


const std::string TestSequencer::UpdateForegroundObjectiveStatus(IBepCommunication *commHandler)
{
	std::string status = BEP_FAIL_RESPONSE;	// the status of the message that was received
	std::string testType;					// the type of test that was performed
	std::string tag;						// the message tag
	std::string value;						// the value of the tag
	std::string message;					// the remainder of the message
	const XmlNode *objective;				// the objective to update

	if(m_currentPhase != NULL)				// check if we are actually still sequencing foreground objectives
	{
		try
		{
			Log(LOG_DEV_DATA, "CurrentPhase: %s\n", m_currentPhase->ToString().c_str());
			const XmlNode *foreground = m_currentPhase->getChildren().getNode("ForegroundObjectives");

			// get the specified objective
			tag = "Objective";
			commHandler->GetByTag(tag, value, message);
			objective = foreground->getChildren().getNode(value);	// get the specified node
			// get the person who was sequenced
			tag = "Who";
			commHandler->GetByTag(tag, value, message);
			// if the sequenced objective was from the person who was sequenced
			if(value == objective->getAttributes().getNode("Component")->getValue())
			{	// get the command that was sequenced
				tag = objective->getChildren().getNode(0)->getName();
				commHandler->GetByTag(tag, value, message);
				// update the objectives result
				XmlNode *result = const_cast<XmlNode *> (objective->getAttributes().getNode("Result"));
				result->setValue(value);	// record the test result
				status = value;				// set the command status
				Log(LOG_DEV_DATA, "%s Updated With: %s\n", tag.c_str(), value.c_str());
			}
			else
			{
				Log(LOG_ERRORS, "We have a problem, Expected: %s, Received: %s\n",
					value.c_str(), objective->getAttributes().getNode("Component")->getValue().c_str());
			}
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "UpdateTestStatus Foreground Test Exception: %s\n", e.Reason().c_str());
			status = BEP_SOFTWAREFAIL_RESPONSE;
		}
	}
	else	// else could not possibly have sequenced another foregound test.
	{
		Log(LOG_ERRORS, "Still Reporting Foreground Tests When None Sequenced!\n");
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}

	return(status);
};

const std::string TestSequencer::UpdateBackgroundObjectiveStatus(IBepCommunication *commHandler)
{
	std::string status = BEP_FAIL_RESPONSE;	// the status of the message that was received
	std::string testType;					// the type of test that was performed
	std::string tag;						// the message tag
	std::string value;						// the value of the tag
	std::string message;					// the remainder of the message
	const XmlNode *objective;				// the objective to update
	const XmlNode *backgroundObjectives;			// test phase background test pointer

	try
	{	// if in the background, we must search all of the drive curve for the correct objective
		Log(LOG_DEV_DATA, "Processing A Background Objective\n");
		// get the objective that was sequenced
		std::string objectiveName;	// test objective name
		tag = "Objective";
		commHandler->GetByTag(tag, objectiveName, message);
		Log(LOG_DEV_DATA, "Objective Name: %s\n", objectiveName.c_str());
		// get Who sequenced it
		std::string objectiveWho;	// Who sequenced the test objective
		tag = "Who";
		commHandler->GetByTag(tag, objectiveWho, message);
		Log(LOG_DEV_DATA, "Objective Who: %s\n", objectiveWho.c_str());
		// walk through the drive curve to update the background test
		bool updateComplete = false;	// update status flag
		for(UINT32 ii = 0; (ii < m_driveCurve->getChildren().size()) && (!updateComplete); ii++)
		{
			try
			{	// get the next background test phase (if it exists for the current phase)
				backgroundObjectives = m_driveCurve->getChildren().getNode(ii)->getChildren().getNode("BackgroundObjectives");
				// get the sequenced objective
				objective = backgroundObjectives->getChildren().getNode(objectiveName);
				// if the objective was sequenced by the correct component, check it
				if(objectiveWho == objective->getAttributes().getNode("Component")->getValue())
				{	// get the components sequenced objective
					tag = objective->getChildren().getNode(0)->getName();
					// if the sequence objective is what was returned, process it
					if(commHandler->GetByTag(tag, value, message) == BEP_STATUS_SUCCESS)
					{
						Log(LOG_DEV_DATA, "Tag: %s, Result: %s\n", tag.c_str(), value.c_str());
						XmlNode *result = const_cast<XmlNode *> (objective->getAttributes().getNode("Result"));
						result->setValue(value);	// record the test result
						updateComplete = true;		// indicate background objective updated
						status = value;				// set the command status
					}
				}
				else
					Log(LOG_DEV_DATA, "Checked Phase: %s, Objective: %s\n",
						m_driveCurve->getChildren().getNode(ii)->getName().c_str(), objectiveWho.c_str());
			}
			catch(BepException &e)
			{	// if the node does not exist thats ok, just dont do anything
				Log(LOG_DEV_DATA, "Checked Phase: %s, Objective: %s, %s\n",
					m_driveCurve->getChildren().getNode(ii)->getName().c_str(), objectiveWho.c_str(), e.Reason().c_str());
			}
		}
		// if unable to update the result, it is very bad
		if(updateComplete == false)
		{
			Log(LOG_ERRORS, "UpdateTestStatus Invalid Background Message Sent To Sequencer: %s, %s\n",
				objectiveName.c_str(), objectiveWho.c_str());
			status = BEP_SOFTWAREFAIL_RESPONSE;
		}
		// else update the current state of the background components
		else
			UpdateCurrentStatus(value, m_backgroundObjectiveStatus);
	}
	catch(XmlException &e)
	{	// if the node does not exist thats ok, just dont do anything
		Log(LOG_ERRORS, "UpdateTestStatus Background Test XmlException: %s\n", e.Reason().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &e)
	{	// must ignore everything while trying to update the drivecurve
		Log(LOG_ERRORS, "UpdateTestStatus Background Test BepException: %s\n", e.Reason().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}

	return(status);
};

const std::string TestSequencer::TestPhaseStatus(void)
{
	Log(LOG_FN_ENTRY, "TestSequencer::TestPhaseStatus()\n");

	std::string         status = "";	// the results of the test phase
	std::string         result;			// the result of the objective
	const XmlNode       *objective;		// reference to the current objective
	const XmlNode       *foreground = m_currentPhase->getChildren().getNode("ForegroundObjectives");
	const XmlNodeMap&   fgObjs = foreground->getChildren();
	XmlNodeMapCItr      objItr;

	// look at all of the results to determine if any are still outstanding
	for( objItr=fgObjs.begin(); objItr!=fgObjs.end() && (status != BEP_TESTING_RESPONSE); objItr++)
	{
		// look at objectives and determine if they are complete
		objective = (XmlNode*)objItr->second;
		result = objective->getAttributes().getNode("Result")->getValue();
		Log(LOG_DEV_DATA, "TestPhaseStatus '%s': %s\n", objective->getName().c_str(), result.c_str());
		if(result == BEP_SEQUENCED_STATUS)	// if not complete, still testing
		{
			Log(LOG_DEV_DATA, "Still Testing\n");
			status = BEP_TESTING_RESPONSE;
		}
		else // finished testing, set the results
			UpdateCurrentStatus(result, status);
	}
	Log(LOG_FN_ENTRY, "TestSequencer::TestPhaseStatus Results: %s\n", status.c_str());
	return(status);
};

void TestSequencer::UpdateCurrentStatus(const std::string &result, std::string &overallResult)
{	// Order of results in importance:
	// Abort, FatalFail, Hardware Fail, Software Fail, Fail, Pass
	Log(LOG_FN_ENTRY, "UpdateCurrentStatus: %s, %s\n", result.c_str(), overallResult.c_str());
	string response;

	if(overallResult == "")										// if a blank entry just update
	{
		overallResult = result;
	}
	else if(result == BEP_ABORT_RESPONSE)						// if an abort condition update
	{
		overallResult = BEP_ABORT_RESPONSE;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_FATALFAIL_RESPONSE) &&				// if is a fatal fail, update
			(overallResult != BEP_ABORT_RESPONSE))				// if not previous abort
	{
		overallResult = result;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_HARDWAREFAIL_RESPONSE) &&			// if a hardware fail, update
			(overallResult != BEP_ABORT_RESPONSE) &&			// if not previous abort
			(overallResult != BEP_FATALFAIL_RESPONSE))			// and not previous fatal fail
	{
		overallResult = result;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_SOFTWAREFAIL_RESPONSE) &&			// if a software fail, update
			(overallResult != BEP_ABORT_RESPONSE) &&			// if not previous abort
			(overallResult != BEP_FATALFAIL_RESPONSE) &&		// and not previous fatal fail
			(overallResult != BEP_HARDWAREFAIL_RESPONSE))		// and not previous hardware fail
	{
		overallResult = result;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_FAIL_RESPONSE) &&					// if a failure, update
			(overallResult != BEP_ABORT_RESPONSE) &&			// if not previous abort
			(overallResult != BEP_FATALFAIL_RESPONSE) &&		// and not previous fatal fail
			(overallResult != BEP_HARDWAREFAIL_RESPONSE) &&		// and not previous hardware fail
			(overallResult != BEP_SOFTWAREFAIL_RESPONSE))		// and not previous software fail, update (pass or fail)
	{
		overallResult = result;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_TIMEOUT_RESPONSE) &&					// if a timeout, update
			(overallResult != BEP_ABORT_RESPONSE) &&			// if not previous abort
			(overallResult != BEP_FATALFAIL_RESPONSE) &&		// and not previous fatal fail
			(overallResult != BEP_HARDWAREFAIL_RESPONSE) &&		// and not previous hardware fail
			(overallResult != BEP_SOFTWAREFAIL_RESPONSE) &&		// and not previous software fail
			(overallResult != BEP_FAIL_RESPONSE))				// and not previously fail, update (pass or fail)
	{
		overallResult = result;
		if((GetStatus() == BEP_TESTING_STATUS))	 m_ndb->Write("FaultLamp", "1", response, true);
	}
	else if((result == BEP_PASS_RESPONSE) &&					// if a pass, update
			(overallResult != BEP_ABORT_RESPONSE) &&			// if not previous abort
			(overallResult != BEP_FATALFAIL_RESPONSE) &&		// and not previous fatal fail
			(overallResult != BEP_HARDWAREFAIL_RESPONSE) &&		// and not previous hardware fail
			(overallResult != BEP_SOFTWAREFAIL_RESPONSE) &&		// and not previous software fail
			(overallResult != BEP_FAIL_RESPONSE) &&				// and not previously fail
			(overallResult != BEP_TIMEOUT_RESPONSE))			// and not previously timeout, update (pass or fail)
	{
		overallResult = result;
	}
	else
		Log(LOG_DEV_DATA, "Not Updating The Status: %s, %s\n", result.c_str(), overallResult.c_str());

	m_ndb->Write("CurrentTestStatus", overallResult, response, true);
	m_currentOverallTestStatus = overallResult;
	Log(LOG_FN_ENTRY, "UpdateCurrentStatus Done: %s\n", overallResult.c_str());
};

INT32 TestSequencer::ResetPhase(bool incycleRetest)
{
	const XmlNode *objective = NULL;
	XmlNode *result;
	INT32 objectivesReset=0;

	Log(LOG_FN_ENTRY, "TestSequencer::ResetPhase\n");

	// get each objective and reset the result to untested ""
	// reset foreground tests
	const XmlNode *foreground;
	try
	{
		foreground = m_currentPhase->getChildren().getNode("ForegroundObjectives");
	}
	catch(XmlException &e)
	{
		foreground = NULL;
	}

	if(foreground != NULL)
	{	// walk through the foregound objective list and reset the results
		for(UINT32 jj=0; jj < foreground->getChildren().size(); jj++)
		{
			try
			{
				objective = foreground->getChildren().getNode(jj);
				result = const_cast<XmlNode *> (objective->getAttributes().getNode("Result"));
				result->setValue(BEP_SEQUENCED_STATUS);	// indicate the test will be sequenced
				objectivesReset++;				// keep count of the objectives reset
				Log(LOG_DEV_DATA, "Foreground Objective: %s\n", objective->getName().c_str());
			}
			catch(XmlException &e)
			{
				if( NULL != objective)
				{
					Log(LOG_ERRORS, "Objective Not Set Up Correctly: %s, %s\n", objective->getName().c_str(), e.Reason().c_str());
				}
				else
				{
					Log(LOG_ERRORS, "Objective Not Set Up Correctly: <NULL>, %s\n", e.Reason().c_str());
				}
				throw;
			}
		}
	}

	// reset background tests, if they exist and we are not in an Incycle retest
	if(!incycleRetest)
	{
		const XmlNode *background;
		try
		{
			background = m_currentPhase->getChildren().getNode("BackgroundObjectives");
		}
		catch(XmlException &e)
		{
			background = NULL;
		}
		if(background != NULL)
		{	// walk through the foregound objective list and reset the results
			for(UINT32 jj=0; jj < background->getChildren().size(); jj++)
			{
				try
				{
					objective = background->getChildren().getNode(jj);
					result = const_cast<XmlNode *> (objective->getAttributes().getNode("Result"));
					result->setValue(BEP_SEQUENCED_STATUS);	// indicate the test will be sequenced
					objectivesReset++;				// keep count of the objectives reset
					Log(LOG_DEV_DATA, "Background Objective: %s\n", objective->getName().c_str());
				}
				catch(XmlException &e)
				{
					Log(LOG_ERRORS, "Objective Not Set Up Correctly: %s, %s\n", objective->getName().c_str(), e.Reason().c_str());
					throw;
				}
			}
		}
	}

	Log(LOG_FN_ENTRY, "TestSequencer::ResetPhase Finished Resetting\n");

	return(objectivesReset);	// return the number of foreground objectives sequenced
};

const std::string TestSequencer::PromptRetest(void)
{
	// clear the keypress value, and look for the response
	std::string msgResponse;
	std::string driverResponse;
	std::string tag, value, response;

	// change the mode of the InputServer to TestResult
	BEP_STATUS_TYPE inputServerStatus = BEP_STATUS_ERROR;
	INT32 attempts = 20;
	do
	{	// Command input server to TestResult state
		m_ndb->Write(INPUT_SERVER_STATE, INPUT_SERVER_TESTRESULT_STATE, msgResponse, true);
		// Get the current state from the input server
		m_ndb->Read(INPUT_SERVER_STATE, msgResponse, true);
		// Check if the InputServer is in TestResult state
		m_ndb->GetNext(tag, driverResponse, msgResponse);
		if(INPUT_SERVER_TESTRESULT_STATE == driverResponse)
			inputServerStatus = BEP_STATUS_SUCCESS;
		else
			BposSleep(100);
		// Keep trying while time remaining not in TestResult state
	} while(attempts-- && (BEP_STATUS_SUCCESS != inputServerStatus));
	// Check if the input server is in test result state
	if(inputServerStatus == BEP_STATUS_SUCCESS)
	{	// Clear the button tags before reading
		m_ndb->Write(PENDANT_KEY_PRESS, PENDANT_CLEAR_KEY_PRESS, msgResponse, true);
		if(UsePlcResultButtons())
		{
			m_ndb->Write(PlcPassButtonTag(), "0", msgResponse, true);
			m_ndb->Write(PlcFailButtonTag(), "0", msgResponse, true);
		}
		// prompt the driver to retest or not
		BposSleep(1000);  // Wait a second to prevent overlap from previous button press
		m_prompt->DisplayPrompt(m_retestYesOrNoPromptBox, "RetestYesOrNo", msgResponse);
		// look for the response
		int timeout = (m_retestTimeout / 250);	// allow the driver X seconds to enter a value
		bool done = false;	// flag to indicate the retest selection is complete
		// while no timeout, driver has not responded, and the
		while((timeout--) && !done && CanRetest(GetStatus()) && (StatusCheck() == BEP_STATUS_SUCCESS))
		{
			Log(LOG_DEV_DATA, "All Good, Checking The Status Of The KeyPress\n");
			// Determine where the input will come from
			if(UsePlcResultButtons())
			{
				string yesButton;
				string noButton;

				// Read both PLC buttons
				if(BEP_STATUS_SUCCESS == m_ndb->Read(PlcPassButtonTag(), msgResponse, true))
				{
					m_ndb->GetByTag(PlcPassButtonTag(), yesButton, msgResponse);
				}
				else
				{
					yesButton = "-1";
				}
				if(BEP_STATUS_SUCCESS == m_ndb->Read(PlcFailButtonTag(), msgResponse, true))
				{
					m_ndb->GetByTag(PlcFailButtonTag(), noButton, msgResponse);
				}
				else
				{
					noButton = "-1";
				}
				Log(LOG_DEV_DATA, "Yes button: %s, No button: %s", yesButton.c_str(), noButton.c_str());
				driverResponse = !yesButton.compare("1") ? BEP_PASS_RESPONSE : !noButton.compare("1") ? BEP_FAIL_RESPONSE : BEP_UNAVAILABLE_RESPONSE;
			}
			else
			{
				tag = PENDANT_KEY_PRESS;
				if(m_ndb->Read(tag, msgResponse, true) == BEP_STATUS_SUCCESS)
					m_ndb->GetNext(tag, driverResponse, msgResponse);
				else
					Log(LOG_ERRORS, "Unable to get the KeyPress information: %s\n", msgResponse.c_str());
			}

			// if not a valid response, check the machine status and if good wait
			if((driverResponse != "Pass") && (driverResponse != "Fail"))
			{
				Log(LOG_DEV_DATA, "Read KeyPress: %s\n", driverResponse.c_str());
				tag = ROLLS_DOWN_DATA_TAG;
				if(m_ndb->Read(tag, msgResponse, true) == BEP_STATUS_SUCCESS)
				{	// if the rolls are down, exit
					m_ndb->GetNext(tag, value, msgResponse);
					Log(LOG_DEV_DATA, "Retainer Status: %s\n", value.c_str());
					if(value == "1")
					{
						driverResponse = BEP_FAIL_RESPONSE;
						done = true;
					}
					else
					{
						BposSleep(250);
					}
				}
				else
				{
					done = true;
					Log(LOG_ERRORS, "Unable to get the RollsDown information: %s\n", msgResponse.c_str());
				}
			}
			else
			{
				done = true;
			}
		}

		// Clear the prompt box
		m_prompt->DisplayPrompt(m_retestYesOrNoPromptBox, "Blank", msgResponse);

		if(driverResponse == "Pass")
		{
			driverResponse = "Yes";
			//clear fault lamp
			m_ndb->Write("FaultLamp", "0", response, true);
		}
		else
		{
			driverResponse = "No";
		}
		// change the mode of the InputServer back to Normal
		m_ndb->Write(INPUT_SERVER_STATE, INPUT_SERVER_NORMAL_MODE, msgResponse, true);
	}
	else	// else we have a problem, dont retest
	{
		Log(LOG_ERRORS, "Error, Unable to command pendant mode\n");
		driverResponse = "No";
	}

	Log(LOG_FN_ENTRY, "PromptRetest: %s\n", driverResponse.c_str());

	return(driverResponse);
};


// *********************************
// BepComponent Method Definitions
// *********************************
void TestSequencer::ReloadConfiguration(void)
{	// send a reload config to all of the components
	Log(LOG_FN_ENTRY, "To ReloadConfig for the components, start a new test\n");
};

void TestSequencer::Abort(void)
{	// Send an abort to all of the components
	try
	{
		SetStatus(BEP_ABORT_STATUS);		// set the test status
		UpdateCurrentStatus(BEP_ABORT_RESPONSE, m_currentOverallTestStatus);
		m_compManager.AbortAll();
		Log(LOG_FN_ENTRY, "TestSequencer::Abort sent to all components\n");
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "Error aborting the test components: %s\n", e.what());
	}
	catch(...)
	{
		Log(LOG_ERRORS, "Error aborting the test components\n");
	}
};

void TestSequencer::Reset(void)
{	// send a reset command to all of the components
	Log(LOG_FN_ENTRY, "To Reset the components, start a new test\n");
};

void TestSequencer::Terminate(void)
{	// open a client connection to the sequencer
	try
	{
		m_compManager.TerminateAll(true);
		SetStatus(BEP_TERMINATE_STATUS);		// set the test status
		Log(LOG_FN_ENTRY, "TestSequencer::Terminate All Components Terminated\n");
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "Error terminating the test components: %s\n", e.what());
	}
	catch(...)
	{
		Log(LOG_ERRORS, "Error terminating the test components\n");
	}
};

const std::string TestSequencer::Read(const std::string &tag, const INT32 rate /* = 0 */)
{	// not supported
	Log(LOG_FN_ENTRY, "TestSequencer Read Received: %s\n", tag.c_str());
	return(std::string("NotSupported"));
};

const std::string TestSequencer::Read(const XmlNode *node, const INT32 rate	/* = 0 */)
{	// not supported
	Log(LOG_FN_ENTRY, "TestSequencer Read Received: %s\n", node->getName().c_str());
	return(std::string("NotSupported"));
};

const std::string TestSequencer::Write(const std::string &tag, const std::string &value)
{	// each component proxy will write the results to the sequencer
	Log(LOG_FN_ENTRY, "TestSequencer Write Received: %s, %s\n", tag.c_str(), value.c_str());
	return(value);
};

const std::string TestSequencer::Write(const XmlNode *node)
{	// each component proxy will write the results to the sequencer
	Log(LOG_FN_ENTRY, "TestSequencer Write Received: %s, %s\n", node->getName().c_str(), node->getValue().c_str());
	return(node->getValue());
};

const std::string TestSequencer::Command(const std::string &tag, const std::string &value)
{	// not supported
	Log(LOG_FN_ENTRY, "TestSequencer Command Received: %s, %s\n", tag.c_str(), value.c_str());
	return(std::string("NotSupported"));
};

const std::string TestSequencer::Command(const XmlNode *node)
{	// not supported
	Log(LOG_FN_ENTRY, "TestSequencer Command Received: %s, %s\n", node->getName().c_str(), node->getValue().c_str());
	return(std::string("NotSupported"));
};

void TestSequencer::DisplayResults(void)
{
	Log(LOG_DEV_DATA, "Sequencer Test Results:\n%s\n", m_driveCurve->ToString().c_str());
}

void TestSequencer::PerformRetest(bool status)
{
	Log(LOG_DEV_DATA, "Setting Perform Retest: %d\n", status);
	m_retestPossible = status;
}

const std::string TestSequencer::GetFailureString(void)
{
	return(m_failString);
}


void TestSequencer::DisplayOverallTestPrompt(const std::string testStatus)
{
	Log(LOG_FN_ENTRY, "DisplayOverallTestPrompt(%s)\n", testStatus.c_str());
	std::string testResult("TestFailed");
	std::string bgColor("red");
	if(testStatus == "Pass")
	{
		testResult = "TestPassed";
		bgColor = "green";
	}
	else if(testStatus == "Abort")
	{
		testResult = "TestAborted";
		bgColor = "yellow";
	}
	else if(testStatus == "Timeout")
	{
		testResult = "TestTimeout";
		bgColor = "orange";
	}
	else if(testStatus == "SoftwareFail")
	{
		testResult = "TestSoftwareFail";
		bgColor = "red";
	}

	std::string msgResponse;
	m_prompt->DisplayPrompt(m_overallTestResultPromptBox, testResult, msgResponse);
	m_prompt->SetBackgroundColor(m_overallTestResultPromptBox, bgColor, msgResponse);
}

bool TestSequencer::TestComplete(const std::string &testStatus, const int retestNumber)
{
	bool complete = false;
    std::string response;					// response buffer

	if((m_retestPossible) &&				/* if a retest can be performed */
	   (testStatus != "Pass") &&			/* and if the test failed */
	   (m_retestType == "ReRun") &&			/* and if ReRun retests have been enabled */
	   (retestNumber < m_maxRetests) &&		/* and if less than the maximum number of retests performed */
	   (CanRetest(testStatus)) &&				/* and can still continue the test */
	   (PromptRetest() == "Yes"))			/* and if the operator selects retest */
	{
		Log(LOG_DEV_DATA, "Notifying subscribers vehicle test being resequenced\n");
        m_ndb->Write(BEP_INTERMEDIATE_OVERALL_RESULT,testStatus,response,true);
		Log(LOG_DEV_DATA, "ReSequencing Vehicle Test\n");
		m_compManager.ResetAll();			// reset all of the components
		m_currentPhaseNumber=0;				// reset the current phase number
		SetStatus(BEP_TESTING_STATUS);		// set the test status to testing
		// reset the prompts and background colors
		std::string msgResponse;
		m_prompt->DisplayPrompt(1, "Blank", msgResponse);
		m_prompt->SetBackgroundColor(1, "white", msgResponse);
		m_prompt->DisplayPrompt(2, "Blank", msgResponse);
		m_prompt->SetBackgroundColor(2, "white", msgResponse);
	}
	else
	{
		complete = true;					// indicate that the test is complete
		SetStatus(BEP_COMPLETE_STATUS);		// set the test status to complete for testing complete
	}

	return(complete);
}

void TestSequencer::SetFailureString(const std::string reason)
{
	if(reason.empty())	m_failString.erase();
	else				m_failString = reason;
}

bool TestSequencer::PerformIncycleRetest(const std::string &currentTestStatus, const int retestNumber)
{
	bool retest = false;

	// if retest enabled and a failure occured and not previously retested
	if((m_retestPossible) &&								/* if a retest can be performed */
	   (currentTestStatus != "Pass") &&						/* and if the current phase has failed */
	   (currentTestStatus != "Skip") &&						/* and if the current phase has not been skipped */
	   (currentTestStatus != "Untested") &&
	   (m_retestType == "Incycle") &&						/* and if Incycle retests have been enabled */
	   (retestNumber < m_maxRetests) &&						/* and if less than the maximum number of retests performed */
	   (CanTest(currentTestStatus)) &&						/* can still continue the test */
	   (PromptRetest() == "Yes"))							/* and if the operator selects retest */
	{
		// if yes is selected, set up to resequence the test
		Log(LOG_DEV_DATA, "Retest Selected %s, %d\n", currentTestStatus.c_str(), retestNumber);
		// If the test is AnalyzeBrakeTest, go back 1 extra step to repeat whole brake test
		if(m_currentPhase->getChildren().getNode("ForegroundObjectives")->getChildren().getNode(0)->getChildren().getNode(0)->getName()== "AnalyzeBrakeTest")
		{
			Log(LOG_DEV_DATA, "AnalyzeBrakeTest Special Case");
			m_currentPhaseNumber--;
		}
		m_currentPhaseNumber--;

		retest = true;
	}
	else if(currentTestStatus != "Pass")
		Log(LOG_DEV_DATA, "Not Performing Retest %d, %s, %s, %d, %d, Re<Max %d, CanTest %d\n",
			m_retestPossible, currentTestStatus.c_str(), m_retestType.c_str(), retestNumber, m_maxRetests,
			(retestNumber < m_maxRetests),
			CanTest(currentTestStatus));

	return(retest);
}

bool TestSequencer::CanTest(const std::string &testStatus)
{
	bool canTest = false;
	// determine if a test can still be performed
	// (i.e. no aborts, fatal fails, or hardware fails)
	if((testStatus != BEP_ABORT_RESPONSE) &&			// test status is not abort
	   (testStatus != BEP_FATALFAIL_RESPONSE) &&		// and test status is not fatal fail
	   (testStatus != BEP_HARDWAREFAIL_RESPONSE) &&		// and test status is not hardware fail
	   (testStatus != BEP_SOFTWAREFAIL_RESPONSE))		// and test status is not software fail
	{
		canTest = true;
	}

	return(canTest);
}

bool TestSequencer::CanRetest(const std::string &testStatus)
{
	bool canTest = false;
	// determine if a retest can still be performed
	// (i.e. no hardware or software fails)
	if((testStatus != BEP_HARDWAREFAIL_RESPONSE) &&		// and test status is not hardware fail
	   (testStatus != BEP_SOFTWAREFAIL_RESPONSE))		// and test status is not software fail
	{
		canTest = true;
	}

	return(canTest);
}

INT32 TestSequencer::StatusCheck(void)
{
	INT32 status = BEP_STATUS_SUCCESS;
	string response;
	// if the retainers are down
	if(m_ndb->Read(ROLLS_UP_DATA_TAG, response, true) == BEP_STATUS_SUCCESS)
	{	// return status failure
		string tag, value;
		m_ndb->GetNext(tag, value, response);
		if(value != "1")	status = BEP_STATUS_FAILURE;
		else				status = BEP_STATUS_SUCCESS;

	}
	else
		status = BEP_STATUS_ERROR;

	Log(LOG_DEV_DATA, "Sequencer StatusCheck(): %d\n", status);

	return(status);
}

void TestSequencer::SetSequencerCommObj(IBepCommunication *sequencerComm)
{
	Log(LOG_DEV_DATA, "Initialized the commuincation handler for: %s\n", GetProcessName().c_str());
	if(sequencerComm != NULL)
		m_sequencerComm = sequencerComm;
	else
		throw BepException("Null Sequencer Comm Passed To Sequencer\n");
}

inline const INT32& TestSequencer::GetObjectiveCommandDelay(void)
{
	return m_interObjectiveDelay;
}

inline void TestSequencer::SetInterObjectiveDelay(const INT32& delayTime)
{
	m_interObjectiveDelay = delayTime;
}

int TestSequencer::ReturnMemArena()
{
	struct mallinfo mInfo;
	mInfo = mallinfo();
	return(mInfo.arena);
}

//-----------------------------------------------------------------------------
inline const bool& TestSequencer::UsePlcResultButtons(const bool *plcButtons /*= NULL*/)
{
	if(plcButtons != NULL)	m_usePlcResultButtons = *plcButtons;
	return m_usePlcResultButtons;
}

//-----------------------------------------------------------------------------
inline const string& TestSequencer::PlcPassButtonTag(const string *passButtonTag /*= NULL*/)
{
	if(passButtonTag != NULL)  m_plcPassButtonTag = *passButtonTag;
	return m_plcPassButtonTag;
}

//-----------------------------------------------------------------------------
inline const string& TestSequencer::PlcFailButtonTag(const string *failButtonTag /*= NULL*/)
{
	if(failButtonTag != NULL)  m_plcFailButtonTag = *failButtonTag;
	return m_plcFailButtonTag;
}
