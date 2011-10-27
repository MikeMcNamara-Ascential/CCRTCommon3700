//*************************************************************************
// FILE DESCRIPTION:
// The Mercedes Vehicle Test is responsible for execuiting a test of a vehilce.  It
// creates a Selector to choose the appropriate DriveCurve to sequence based
// on the vehicle build and then creates a Sequencer to run the test.
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
//    $Log: /CCRT/Common/ApplicationLayer/VehicleTest/MercedesVehicleTest.cpp $
// 
// 2     2/22/05 3:30p Derickso
// Updated to only start a vehicle test if we have valid data.
// Added new log file index increment feature.
// 
// 1     11/01/04 3:11p Derickso
// Created.
// 
//*************************************************************************

#include "MercedesVehicleTest.h"
#include "/usr/include/malloc.h"

MercedesVehicleTest::MercedesVehicleTest() : VehicleTest(), m_testResult(NULL)
{
}

MercedesVehicleTest::~MercedesVehicleTest()
{	// Clear the data tags map
	m_resetSystemTags.clear(true);
	m_resetOverallResults.clear(true);
	// Delete the TestResultServer comm object
	if (m_testResult != NULL) delete m_testResult;
	m_testResult = NULL;
}

void MercedesVehicleTest::Initialize(const XmlNode *document)
{
	// Call the base class to iniialize first
	VehicleTest::Initialize(document);

	// Save the system data tags that need to be reset
	try
	{
		m_resetSystemTags.DeepCopy(document->getChild("Setup/ResetDataTags")->getChildren());
	}
	catch (XmlException &e)
	{
		Log(LOG_ERRORS, "Setup/ResetDataTags Does not exist\n");
		m_resetSystemTags.clear(true);
	}
	// Save the overall result tags that need to be reset
	try
	{
		m_resetOverallResults.DeepCopy(document->getChild("Setup/ResetOverallResults")->getChildren());
	}
	catch (XmlException &e)
	{
		Log(LOG_ERRORS, "Setup/ResetOverallResults Does not exist\n");
		m_resetOverallResults.clear(true);
	}

	if (m_testResult == NULL)	 //  the test result interface to null
	{
		m_testResult = new ITestResultServer();
		m_testResult->Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
	}
    Log(LOG_DETAILED_DATA, "MEM: End Initialize() - %d\n",ReturnMemArena());
}

void MercedesVehicleTest::Run(volatile bool *terminateFlag)
{
	std::string driveCurve = "";		// the current drive curve
	std::string test = "";				// the test that was commanded
	std::string testStatus = "";		// the status of the commanded test
	std::string localizedDriveCurve;	// the localized drive curve

    Log(LOG_DETAILED_DATA, "MEM: Start Run()1 - %d\n",ReturnMemArena());
	while (GetStatus() != BEP_TERMINATE_STATUS)
	{
		try
		{	// signal the end of the test
			Log(LOG_DEV_DATA, "MercedesVehicleTest Priority: %d\n", BposGetPriority(0));
			CommandTest(TEST_END);      
			testStatus = "";					// reset the test variables
			m_testSequencer.RemoveTestComponents();	// Remove the test components after a test has completed
			Log(LOG_DETAILED_DATA, "MEM: Run()2 Start While Loop - %d\n",ReturnMemArena());
            do
			{	// wait for a test to be commanded
				// or command to remove or load test components
                test = GetCommandedTest();
				if (LOAD_COMPONENTS == test)
				{
					// Write tag to the system to prevent the Logger from rotating during test component loading
                    driveCurve = LoadComponents(test);
				}
				else if (REMOVE_COMPONENTS == test)
				{
					// Write tag to the system to prevent the Logger from rotating during test component removing
                    m_testSequencer.RemoveTestComponents();
				}
			} while ((test == LOAD_COMPONENTS) || (test == REMOVE_COMPONENTS) || (test == BEP_NO_DATA));
            Log(LOG_DETAILED_DATA, "MEM: Run()3 End While Loop - %d\n",ReturnMemArena());
			testStatus = BEP_TESTING_RESPONSE;	// update the status to testing
			Log(LOG_DEV_DATA, "MercedesVehicleTest New Test Priority: %d\n", BposGetPriority(0));
			Write(GetDataTag("OverallPassFail"), testStatus);
			ClearFault();						// if there is a currently active fault, clear it

			Log(LOG_DEV_DATA, "Starting Test: %s, Status: %s, %d\n", test.c_str(), GetStatus().c_str(), BposGetPriority(0));

			if (test != TEST_TERMINATE)
			{	// determine which drive curve to use
				if (test == TEST_START)			 // if the beginning of a new test
				{
					m_testSequencer.PerformRetest(true);
				}
				else if (test != TEST_RETEST)	 // else if not begining of new test
				{
					Log(LOG_DEV_DATA, "Loading Commanded Test: %s\n", test.c_str());
					driveCurve = m_testSelector.SelectDriveCurve(test);
					LoadComponents(test);
					// if a LossCompensation test is being sequenced, do not perform a retest 
					if (test == TEST_LOSS_COMPENSATION)
					{
						m_testSequencer.PerformRetest(false);
					}
					// else perform a retest
					else
						m_testSequencer.PerformRetest(true);

				}

				// if a valid drive curve exists, run through the sequencing process
				if (driveCurve != "")
				{
                    Log(LOG_DEV_DATA, "Have DriveCurve, Will Test: %s\n", driveCurve.c_str());
					// publish the drive curve that is being used
					Write(GetDataTag("DriveCurve"), driveCurve);

					// start a new logger test file if a new test
					if (test != TEST_RETEST)
					{
						//The following only accepted int32 and program must allow alphanumeric vin
						//NewTestNumberLog(VehicleTest::GetLogSuffix()); 
						
						//This is what NewTestNumberLog was calling
						SendToDaemon(ROLL_NEW_TEST_NUMBER, VehicleTest::GetLogSuffix());

						//Always increment the log file counter	
						VehicleTest::IncrementLogFileIndex();
					}
                    Log(LOG_DEV_DATA, "Sequencing Test Type: %s\n", test.c_str());

					// update the test status, and notify subscribers
					// NOTE: only do this right before the sequencing to prevent race conditions
					Write(GetDataTag("TestInProgress"), "1");
					Write(GetDataTag("ForegroundTestInProgress"), "1");

					testStatus = m_testSequencer.SequenceTest(test);
					Log(LOG_DEV_DATA, "Test Complete: %s\n", testStatus.c_str());
					// notify all of the subscribers for the completed test
					Write(GetDataTag("OverallPassFail"), testStatus);
					Log(LOG_DEV_DATA, "Notification Complete\n");
					// if a software fail occurred, report it on the machine fault list
					if (testStatus == BEP_SOFTWAREFAIL_STATUS)
					{
						char buffer[128];		// buffer to create messages with
						throw BepException(CreateMessage(buffer, sizeof(buffer), "Error Curve: %s, Reason: %s", driveCurve.c_str(), m_testSequencer.GetFailureString().c_str()));
					}
					// if a hardware fail occurred, report it on the machine fault list
					else if (testStatus == BEP_HARDWAREFAIL_STATUS)
						throw BepException("Hardware Failure Sequencing: %s", driveCurve.c_str());
                    Log(LOG_DETAILED_DATA, "MEM: Run()6 - %d\n",ReturnMemArena());
				}
				else						// if not we are done
				{
					Log(LOG_ERRORS, "MercedesVehicleTest Error: No Drive Curve Specified\n");
					throw BepException("No Drive Curve Avaliable For Specified Vehicle");
				}
			}
			else
			{
				SetStatus(BEP_TERMINATE_STATUS);
				Log(LOG_ERRORS, "Terminate Received By Run(): %s\n", GetStatus().c_str());
			}
		}
		catch (BepException &e)
		{	// report the fault to the fault server
			std::string fault = "Vehicle Test: " + e.Reason();
			SetFault(fault);
			Log(LOG_ERRORS, "MercedesVehicleTest Exception: %s\n", e.what());
		}
		catch (...)
		{	// report the fault to the fault server
			std::string fault = "MercedesVehicleTest Unknown Exception";
			SetFault(fault);
			Log(LOG_ERRORS, "MercedesVehicleTest Unknown Exception\n");
		}
	}
	Log(LOG_DEV_DATA, "MercedesVehicleTestRunner Calling BepServer::Run(): %s\n", GetStatus().c_str());
	BepServer::Run();	// perform the base class cleanup
	Log(LOG_FN_ENTRY, "MercedesVehicleTestRunner Terminated\n");
}

const std::string MercedesVehicleTest::Command(const XmlNode *node)
{	 // currently only support Start, Retest, and Continue
	std::string status;
	UINT16 count;

	Log(LOG_FN_ENTRY, "MercedesVehicleTest::Command(%s)\n", node->ToString().c_str());

	// if a specific test was commanded
	if (node->getName() == GetDataTag("CommandTest"))
	{
		if ((status = CommandTest(node->getValue())) != BEP_SUCCESS_RESPONSE)
		{	// if not a command, it must be something else
			Log(LOG_ERRORS, "MercedesVehicleTest: Received Unsuported Request: %s, %s\n", 
				node->getName().c_str(), node->getValue().c_str());
		}
	}
	// else if commanded to start a vehicle test
	else if ( node->getName() == GetDataTag( "StartVehicleTest"))
	{
		if (Read(GetDataTag("TestInProgress")) != "1")
		{
			// indicate that initialization is in progress
			Log(LOG_DEV_DATA, "Updating the PhaseStatus: %s\n", TEST_STATUS_NOT_STARTED);
			SetData(PHASE_STATUS_TAG, TEST_STATUS_NOT_STARTED);     
			// start the test, we have a new build
			if ((status = CommandTest(TEST_START)) != BEP_SUCCESS_RESPONSE)
			{
				Log(LOG_ERRORS, "ERROR! Unable To Start A New Vehicle Test\n");
			}
		}
		else
		{
			Log(LOG_ERRORS, "Error, StartTest is being commanded during a test\n");
		}
	}
	else if (node->getName() == LOAD_VEHICLE_TEST_DATA_TAG)
	{
		if (Read(GetDataTag("TestInProgress")) != "1")
		{
			Log(LOG_DEV_DATA, "MercedesVehicleTest::Command() - Processing Load Vehicle Data, No Test In Progress\n");
			Log(LOG_DEV_DATA, "MercedesVehicleTest::Command() - Queue Size = %d\n", m_commandQueue.size());
			// if another load is in the queue remove and place at end
			for (count = 0; count < m_commandQueue.size(); count++)
			{
				Log(LOG_DEV_DATA, "MercedesVehicleTest::Command(), Top of Queue = %s\n",
					m_commandQueue.front().c_str());
				if (LOAD_COMPONENTS == m_commandQueue.front())
				{
					m_commandQueue.pop();
				}
				else
				{	// Put front into back and check the next
					m_commandQueue.push(m_commandQueue.front());
					m_commandQueue.pop();
				}
			}

			m_commandQueue.push(LOAD_COMPONENTS);
			// Load the components
			m_semCommandTest.CriticalSectionExit();     
		}
		else
		{
			Log(LOG_ERRORS, "Error, Load Vehicle Components is being commanded during a test\n");
		}
	}
	else if (node->getName() == REMOVE_VEHICLE_TEST_DATA_TAG)
	{
		if (Read(GetDataTag("TestInProgress")) != "1")
		{
			Log(LOG_DEV_DATA, "MercedesVehicleTest::Command() - Processing Remove Vehicle Data, No Test In Progress\n");

			Log(LOG_DEV_DATA, "MercedesVehicleTest::Command() - Queue Size = %d\n", m_commandQueue.size());
			// if another remove is in the queue, then remove and add to the end
			// otherwise just add to the end of the queue
			// if another load is in the queue remove and place at end
			for (count = 0; count < m_commandQueue.size(); count++)
			{
				Log(LOG_DEV_DATA, "MercedesVehicleTest::Command(), Top of Queue = %s\n",
					m_commandQueue.front().c_str());
				if (REMOVE_COMPONENTS == m_commandQueue.front())
				{	// Remove from queue
					m_commandQueue.pop();
				}
				else
				{	// Put front into back and check the next
					m_commandQueue.push(m_commandQueue.front());
					m_commandQueue.pop();
				}
			}
			m_commandQueue.push(REMOVE_COMPONENTS);
			// Remove the components
			m_semCommandTest.CriticalSectionExit();     
		}
		else
		{
			Log(LOG_ERRORS, "Error, Unload Components is being commanded during a test\n");
		}
	}
	// else it is an unsupported command
	else
		Log(LOG_ERRORS, "MercedesVehicleTest Unsupported Command %s, %s\n", 
			node->getName().c_str(), node->getValue().c_str());

	return(status);
}

const std::string MercedesVehicleTest::Publish(const XmlNode *node)
{
	std::string status;

	status = SetData(node);
	// if a StartLossCompensation signal
	if (node->getName() == GetDataTag("StartLossCompensation"))
	{	// check the status to determine if new test should be started
		if (node->getValue() == "1")
			CommandTest(TEST_LOSS_COMPENSATION);
	}
	else if (node->getName() == GetDataTag("RetainersDown"))
	{	// if the retainers were lovered and we are in the middle of a test, abort
		if ((GetStatus() == BEP_TESTING_STATUS) && (node->getValue() == "1"))
			m_testSequencer.Abort();
	}
	else
		Log(LOG_DEV_DATA, "MercedesVehicleTest Publish Received: %s\n", node->getName().c_str());

	return(status);
}

string MercedesVehicleTest::LoadComponents(string testType)
{
	string response;
	string description("Clear Overall Result");
	string name("DCXComponent");
	std::string driveCurve = "";		// the current drive curve
	std::string localizedDriveCurve;	// the localized drive curve
	XmlNodeMapItr iter;

	Log(LOG_DEV_DATA, "MercedesVehicleTest::LoadComponents() - Enter\n");
	// Reset Overall Test result GUI display
	for (iter = m_resetOverallResults.begin(); iter != m_resetOverallResults.end(); iter++)
	{
		m_testResult->WriteTestResult(iter->second->getName().c_str(),
									  iter->second->getValue().c_str(),
									  name, description, response, true);
	}
	// Reset data tags
	for (iter = m_resetSystemTags.begin(); iter != m_resetSystemTags.end(); iter++)
	{
		m_ndb.Write(iter->second->getName(), iter->second->getValue(), response, true);
	}
	// Check if we need to load a regular or commanded drive curve
	driveCurve = (LOAD_COMPONENTS != testType) ? m_testSelector.SelectDriveCurve(testType) : LoadDriveCurve();
	// prepend the FTP_ROOT to the drive curve for loading
	localizedDriveCurve = getenv("FTP_ROOT") + driveCurve;
	Log(LOG_FN_ENTRY, "Loading The Drive Curve: %s\n", localizedDriveCurve.c_str());
	// load the drive curve
	const XmlNode *driveCurveDoc = m_driveCurveParser.ReturnXMLDocument(localizedDriveCurve);
	// sequence the test
	Log(LOG_DEV_DATA, "Loading Drive Curve %s\n", driveCurve.c_str());
	Log(LOG_DETAILED_DATA, "MEM: LoadComponents()1 Enter m_testSequencer.LoadDriveCurve() - %d\n",ReturnMemArena());
	m_testSequencer.LoadDriveCurve(driveCurveDoc, getenv("USR_ROOT"));
	Log(LOG_DETAILED_DATA, "MEM: LoadComponents()2 Exit m_testSequencer.LoadDriveCurve() - %d\n",ReturnMemArena());
	return(driveCurve);
}

string MercedesVehicleTest::LoadDriveCurve(void)
{
	std::string driveCurve = "";		// the current drive curve
	std::string localizedDriveCurve;	// the localized drive curve

	Log(LOG_DEV_DATA, "MercedesVehicleTest::LoadDriveCurve() - Enter");
	// load the vehicle build and determine which drive curve to load
	ReadNewVehicleBuild();

	if (GetVehicleBuild() != NULL)
    {
		string rootDir(getenv("USR_ROOT"));
        driveCurve = m_testSelector.SelectDriveCurve(GetVehicleBuild(), rootDir);
    }
	// Return the drive curve name
	return(driveCurve);
}

inline std::string MercedesVehicleTest::GetCommandedTest(void)
{
	string type = BEP_NO_DATA;

	if (m_commandQueue.empty())
	{
		do
		{
			Log(LOG_DEV_DATA, "MercedesVehicleTest::GetCommandedTest() - No more commands in queue, ensuring\n"
				"\tm_semCommandTest is set to 0 to wait for next command");
		} while (BEP_STATUS_FAILURE != m_semCommandTest.CriticalSectionTryEnter());

		Log(LOG_DEV_DATA, "MercedesVehicleTest::GetCommandedTest() - Waiting for Command");
		// Wait for Command
		m_semCommandTest.CriticalSectionEnter();
	}


	// Ensuer that the stack is populated once the critical section was entered
	if (m_commandQueue.empty())
	{
		Log(LOG_ERRORS, "ERROR MercedesVehicleTest::GetCommandedTest() m_commandQueue empty");
		type = BEP_NO_DATA;
	}
	else
	{
		// Pop the next command off the stack
		type = m_commandQueue.front();	// get the command type
		m_commandQueue.pop();		// remove from stack	
	}
	Log(LOG_DEV_DATA, "GetTestCommanded: %s\n", type.c_str());
	return(type);
}

inline std::string MercedesVehicleTest::CommandTest(const std::string testType)
{
	std::string status = BEP_SUCCESS_RESPONSE;		// status of the commanded test
	std::string testStatus = "0";					// the status of the vehicle test

	// check the test status so that only one test can be sequenced at a time
	if (Read(GetDataTag("TestInProgress")) != "1")
	{
		Log(LOG_DEV_DATA, "Commanding Test %s\n", testType.c_str());

		if (testType == TEST_START)						 // if a new test commanded
		{
			// Get the latest vehicle build data
			if (ReadNewVehicleBuild() == BEP_STATUS_SUCCESS)
				m_commandQueue.push(testType);
			else	// if unable to get it do not start a test
				status = BEP_INVALID_RESPONSE;
		}
		else if ( (testType == TEST_RETEST) ||				 // if a retest commanded
				  (testType == TEST_LOSS_COMPENSATION) ||	 // if loss compensation command
				  (testType == TEST_BASE_BRAKE) ||			 // if base brake command
				  (testType == TEST_TERMINATE))				 // if terminate command
		{
			m_commandQueue.push(testType);
		}
		else											// else set status to invalid
		{
			status = BEP_INVALID_RESPONSE;              
		}

		// if successful command and not the end of the test
		if (status != BEP_INVALID_RESPONSE)
		{
			testStatus = "1";	//set the status to testing

			// if the test was not terminated
			if (testType != TEST_TERMINATE)
			{	// set the status to testing
				SetStatus(BEP_TESTING_STATUS);              
			}
			// start the test
			m_semCommandTest.CriticalSectionExit();     
		}

		// indicate the test status
		Write(START_VEHICLE_TEST_DATA_TAG, testStatus);

	}
	else	// if testing check the other conditions
	{
		if (testType == TEST_END)						 // if the end of the test
		{
			SetStatus(TEST_END);						// flag the vehicle test as complete
		}
		else if (testType == TEST_ABORT);				 // if test abort command
		else if (testType == TEST_TERMINATE);				 // if terminate command
		else
		{
			status = BEP_INVALID_RESPONSE;
			Log(LOG_ERRORS, "Received Invalid Command During Test: %s\n", testType.c_str());
		}

		// if successful command and not the end of the test
		if (status != BEP_INVALID_RESPONSE)
		{	// update the test status, and notify subscribers
			Write(GetDataTag("TestInProgress"), testStatus);
			// indicate that the test has ended
			Write(START_VEHICLE_TEST_DATA_TAG, testStatus);
		}

	}   

	return(status);
};

int MercedesVehicleTest::ReturnMemArena()
{
    struct mallinfo mInfo;
    mInfo = mallinfo();
    return(mInfo.arena);
}

//struct mallinfo {
//	int	arena;	/* size of the arena */
//	int	ordblks;/* number of big blocks in use */
//	int	smblks;	/* number of small blocks in use */
//	int	hblks;	/* number of header blocks in use */
//	int	hblkhd;	/* space in header block headers */
//	int	usmblks;/* space in small blocks in use */
//	int	fsmblks;/* memory in free small blocks */
//	int	uordblks;/* space in big blocks in use */
//	int	fordblks;/* memory in free big blocks */
//	int	keepcost;/* penalty if M_KEEP is used -- not used */
//};

