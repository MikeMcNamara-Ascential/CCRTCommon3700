//*************************************************************************
// FILE:
//    $Header: /CMake Core/Source/Ford/ApplicationLayer/VehicleTest/FordVehicleTest.cpp 5     4/01/06 5:05p Bmeinke $
//
// FILE DESCRIPTION:
// The Vehicle Test is responsible for execuiting a test of a vehilce.  It
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
//    $Log: /CMake Core/Source/Ford/ApplicationLayer/VehicleTest/FordVehicleTest.cpp $
// 
// 5     4/01/06 5:05p Bmeinke
// Added an overloaded PreTestInit() method
// 
// 4     2/24/06 1:32p Bmeinke
// Added a Log() call to report the current software version at the
// beginning of each log file
// 
// 3     6/30/05 8:38p Bmeinke
// Needto make a copy of the vehicle build noed because the base class
// deletes it differently now
// Added support for persistent log file index number
// 
// 2     5/16/05 12:55p Bmeinke
// Call base class Register() method
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
// 
// 1     6/04/04 1:08p Bmeinke
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 2     4/12/04 12:33p Bmeinke
// Added a log statement if GetVehicleBuild() does not return NULL in
// Run()
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 2     8/06/03 2:53p Khykin
// Updating with latest changes from Flatrock.
// 
// 1     6/20/03 9:46a Khykin
// Initial checkin
// 
// 1     6/20/03 9:22a Khykin
// Initial checkin.
// 
//
//*************************************************************************
#include "GMVehicleTest.h"


GMVehicleTest::GMVehicleTest() : VehicleTest()
{
    Log(LOG_FN_ENTRY,"GMVehicleTest Constructor Done\n");
}

GMVehicleTest::~GMVehicleTest()
{
    Log(LOG_FN_ENTRY,"GM Vehicle Test Terminating\n");
    
    Log(LOG_FN_ENTRY,"GM Vehicle Test Terminating done\n");
}

void GMVehicleTest::Initialize(const std::string &fileName)
{   // Initialize the base class
    VehicleTest::Initialize(fileName);
}

void GMVehicleTest::Initialize(const XmlNode *document)
{
    Log(LOG_FN_ENTRY,"GMVehicleTest::Initialize() #2\n");

    // Initialize the base class
    VehicleTest::Initialize(document);
   
    // initialize the phase commander
    /*
    try
    {   // initialize the phase commander
        m_phaseCommander.Initialize(document->getChild("Setup/PhaseCommander"));
        // set the test result communication object
        m_phaseCommander.SetSequencerCommObj(m_sequencerComm);
    }
    catch(XmlException &e)
    {
        throw BepException("Error getting Phase Commander info: %s", e.what());
    }     
    */
    try
    {   // get drive curve
        m_driveCurve = document->getChild("Setup/Selector/CommandableDriveCurves/Brake")->getValue();
    }
    catch(XmlException &e)
    {
        m_driveCurve = "/Configuration/VehicleTest/DriveCurves/DriveCurve.xml";
        Log(LOG_ERRORS, "Error Drive Curve Selection: %s\n",  m_driveCurve.c_str());
    }

    Log(LOG_FN_ENTRY,"GMVehicleTest::Initialize() Exit #2\n");
}

const string& GMVehicleTest::GetLastTestType(void)
{
    return m_lastTestType;
}

void GMVehicleTest::SetCurrentTestType(const string &testType)
{
    m_lastTestType = testType;
}

void GMVehicleTest::Run(volatile bool *terminateFlag)
{
    string test = "";              // the test that was commanded
    string testStatus = "";        // the status of the commanded test    
    string localizedDriveCurve;    // the localized drive curve

    while(GetStatus() != BEP_TERMINATE_STATUS)
    {
        BposSleep(1000);
    }
    Log(LOG_FN_ENTRY, "GMVehicleTestRunner Terminated\n");
}

void GMVehicleTest::PreTestInit(const std::string &test, const std::string &driveCurve)
{
    VehicleTest::PreTestInit(test, driveCurve);

    // start a new logger test file if a new test
    if(test != TEST_RETEST)
    {
        // Report the software version at the beginning of each log file
        const std::string softwareVer( ReadSubscribeData( SOFTWARE_ID));
        Log( "CCRT Software Version: %s\n", softwareVer.c_str());
    }
}

const std::string GMVehicleTest::Command(const XmlNode *node)
{    // currently only support Start, Retest, and Continue
    std::string status;

    Log(LOG_DEV_DATA, "GMVehicleTest::Command(%s)\n", node->ToString().c_str());

    if(node->getName() == GetDataTag("CommandPhase"))
    {
        if((status = CommandPhase(node->getValue())) != BEP_SUCCESS_RESPONSE)
        {   // if not a command, it must be something else
            Log(LOG_ERRORS, "GMVehicleTest: Received Unsuported Phase Request: %s, %s\n", 
                node->getName().c_str(), node->getValue().c_str());
        }
        else
        {
            // update the commanded phase
            status = SetData(node);
            // if successfull, update the phase status
            if(status == BEP_SUCCESS_RESPONSE)
                SetData(PHASE_STATUS_TAG, TEST_STATUS_NOT_STARTED);
        }
    }
    else
        status = VehicleTest::Command(node);

    return(status);
}

const std::string GMVehicleTest::Publish(const XmlNode *node)
{
    std::string status;

    Log(LOG_DEV_DATA, "GMVehicleTest::Publish(%s)\n", node->ToString().c_str());

    // update the current data
    status = SetData(node);

    // if a StartLossCompensation signal
    if(node->getName() == GetDataTag("StartLossCompensation"))
    {   // check the status to determine if new test should be started
        if(node->getValue() == "1")  CommandTest(TEST_LOSS_COMPENSATION);
    }
    else if(!node->getName().compare(START_TEST_DATA_TAG))
    {
        Log(LOG_DEV_DATA, "Received %s, setting TestInProgress to %s", START_TEST_DATA_TAG, node->getValue().c_str());
        Write(GetDataTag("TestInProgress"), node->getValue());
    }
    else if(node->getName() == GetDataTag("VehicleBuild"))
    {
        // if a test is not in progress
        if(Read(GetDataTag("TestInProgress")) != "1")
        {
            // store a copy of the build	
            SetVehicleBuild(node);                          
            // indicate that initialization is in progress
            Log(LOG_DEV_DATA, "Updating the PhaseStatus: %s\n", TEST_STATUS_IN_PROGRESS);
            SetData(PHASE_STATUS_TAG, TEST_STATUS_NOT_STARTED);     
        }
        else
            Log(LOG_ERRORS, "Error, VehicleBuild is being published during a test\n");
    }
    // if the retainers were lovered and we are in the middle of a test or an abort received, abort
    else if((node->getName() == GetDataTag("RetainersDown")) || (node->getName() == GetDataTag("Abort")))
    {   // Determine if we need to Abort testing
        if((GetStatus() == BEP_TESTING_STATUS) && (node->getValue() == "1"))    
        {
            Write(GetDataTag("TestInProgress"), "0");
            Abort();
        }
#if 0
        else if(!node->getName().compare(GetDataTag("RetainersDown")) && !node->getValue().compare("0"))
        {
            string response;
            if(BEP_STATUS_SUCCESS == m_ndb.Read(MACHINE_TYPE, response, true))
            {
                string machType;
                m_ndb.GetByTag(MACHINE_TYPE, machType, response);
                // Read the loss compensation switch status from the PLC
                if(BEP_STATUS_SUCCESS == m_ndb.Read(GetDataTag("LossCompSwitchStatus"), response, true))
                {
                    string switchStat;
                    m_ndb.GetByTag(GetDataTag("LossCompSwitchStatus"), switchStat, response);
                    if(!machType.compare("3700") || (ForceTestInProgress() && !switchStat.compare("0")))
                    {
                        Write(GetDataTag("TestInProgress"), "1");
                    }
                }

            }
        }
#endif
        else if(!node->getName().compare(GetDataTag("RetainersDown")) && !node->getValue().compare("1"))
        {
            Write(GetDataTag("TestInProgress"), "0");
        }
    }
    else if(node->getName() == GetDataTag( "StartVehicleTest"))
    {
        if(Read(GetDataTag(TEST_STATUS_IN_PROGRESS)) != "1")
        {
            CommandTest(TEST_START);    // start the test, we have a new build
            // indicate that initialization is in progress
            Log(LOG_DEV_DATA, "Updating the PhaseStatus: %s\n", TEST_STATUS_NOT_STARTED);
            SetData(PHASE_STATUS_TAG, TEST_STATUS_NOT_STARTED);     
        }
        else
        {
            Log(LOG_ERRORS, "Error, StartVehicleTest is being commanded during a test\n");
        }
    }
    // if sequenced to stop the current test    
    else if((node->getName() == GetDataTag("StopCurrentTest")) && (node->getValue() == "1"))
    {
        // if not currently testing
        if(Read(GetDataTag("TestInProgress")) == "0")
            m_phaseCommander.StopCurrentTest_Reset();
    }
    // test phase commanded (inbound)
    else if((node->getName() == GetDataTag(COMMAND_PHASE_TAG)))
    {
        // if not currently testing
        if(Read(GetDataTag("TestInProgress")) == "0")
        {
            m_phaseCommander.StopCurrentTest_Reset();
            m_testPhase = node->getValue();
            CommandTest(TEST_START);
            //m_phaseCommander.SetCommandedPhase(m_testPhase);
            Log(LOG_DEV_DATA,"Selecting the TestPhase: %s\n",m_testPhase.c_str());
        }
    }

    else
        Log(LOG_DEV_DATA, "GMVehicleTest SpecialProcessing Received: %s\n", node->getName().c_str());

    return(status);
}

void GMVehicleTest::Abort(void)
{
    Log(LOG_FN_ENTRY|LOG_ERRORS, "GMVehicleTest::Abort() Received\n");
    SetStatus(BEP_ABORT_STATUS);    // set the status to Termiante
    m_testSequencer.Abort();        // terminante the testing framework
    m_phaseCommander.Abort();       // terminante the testing framework
}

void GMVehicleTest::Terminate(void)
{
    Log(LOG_FN_ENTRY, "GMVehicleTest Received Terminate\n");
    m_testSequencer.Terminate();    // terminate the vehicle components
    m_phaseCommander.Terminate();   // terminate the vehicle components
    BepServer::Terminate();         // terminate the server 
    CommandTest(TEST_TERMINATE);    // command the test to terminate  	
    Log(LOG_FN_ENTRY, "GMVehicleTest Received Terminate done\n");
}

void GMVehicleTest::Reset(void)
{   // currently not implemented
    Log(LOG_FN_ENTRY, "GMVehicleTest Received Reset Command\n");
    m_testSequencer.Reset();
    m_phaseCommander.Reset();
}

void GMVehicleTest::ReloadConfig(void)
{   // not implemented
    Log(LOG_FN_ENTRY, "GMVehicleTest Received ReloadConfig Command\n");
    BepServer::ReloadConfiguration();
}

const std::string GMVehicleTest::Register(void)
{
    Log(LOG_FN_ENTRY, "Registering the GMVehicleTest\n");
    VehicleTest::Register();
    m_phaseCommander.Register();

    return(BEP_SUCCESS_RESPONSE);
}

//-----------------------------------------------------------------------------
inline string GMVehicleTest::CommandTest(const string testType)
{
    string status = BEP_SUCCESS_RESPONSE;		// status of the commanded test
    string testStatus = "0";					// the status of the vehicle test

    Log( LOG_FN_ENTRY, "Enter GMVehicleTest::CommandTest(%s)\n", testType.c_str());

    // check the test status so that only one test can be sequenced at a time
    if(Read(GetDataTag("TestInProgress")) != "1")
    {
        Log(LOG_DEV_DATA, "Commanding Test %s\n", testType.c_str());
        testStatus = "1";   //set the status to testing

        // if the test was not terminated
        if(testType != TEST_TERMINATE)
        {   // set the status to testing
            SetStatus(BEP_TESTING_STATUS);
        }
        // start the test
        m_semCommandTest.CriticalSectionExit();
        
        // indicate the test status
        Write(START_VEHICLE_TEST_DATA_TAG, testStatus);
        Write(GetDataTag("TestInProgress"), "1");

    }
    else	// if testing check the other conditions
    {
        if(testType == TEST_END)						// if the end of the test
        {
            SetStatus(TEST_END);						// flag the vehicle test as complete
        }

        // if successful command and not the end of the test
        if(status != BEP_INVALID_RESPONSE)
        {	// update the test status, and notify subscribers
            // indicate that the test has ended
            Write(START_VEHICLE_TEST_DATA_TAG, testStatus);
        }

    }
    Log( LOG_FN_ENTRY, "Exit GMVehicleTest::CommandTest(%s), status=%s\n", testType.c_str(), status.c_str());

    return(status);
}

//-----------------------------------------------------------------------------
const string GMVehicleTest::GetDriveCurveType(const XmlNode *document)
{
    string driveCurveType;

    try
    {
        driveCurveType = document->getAttribute(TEST_TYPE_TAG)->getValue();
    }
    catch(BepException &e)
    {
        driveCurveType = GetDataTag("CommandSequence");
    }

    Log(LOG_DEV_DATA, "DriveCurveType: %s received\n", driveCurveType.c_str());

    return(driveCurveType);
}

//-----------------------------------------------------------------------------
std::string GMVehicleTest::CommandPhase(std::string phase)
{
    m_phaseCommander.SetCommandedPhase(phase);
    return(BEP_SUCCESS_RESPONSE);
}

