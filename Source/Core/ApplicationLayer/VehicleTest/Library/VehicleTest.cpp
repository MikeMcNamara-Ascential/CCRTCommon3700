//******************************************************************************
// Description:
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/VehicleTest/Library/VehicleTest.cpp $
// 
// 1     11/02/07 10:29a Derickso
// Updated destructor to terminate all running test components.
// 
// 7     5/08/06 7:18p Cward
// Changed printf statements to Log statements.
//
// 6     4/13/06 3:33a Cward
// Added PreTestInit and try catch blocks around getting member variables
//
// 2     3/20/06 10:50a Bmeinke
// Added PreTestInit() method to allow sub classes to perform tasks just
// prior to sequencing a new test
//
// 2     2/15/06 3:49p Bmeinke
// Use seperate try/catch blocks whil reading m_suffixExtension and
// m_useVinAsTestLogSuffix inside InitializeLogFileIndex()
// Do not add a "."+m_suffixExtension to the suffix if m_suffixExtension
// is empty inside GetLogSuffix()
//
// 5     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 4     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 3     5/09/05 5:00p Gpattison
// Moved the calling of the base class initialize to the top of the
// Initialize() method so that it can be run first.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     9/01/04 9:30p Cward
// Added option to allow components to be terminated at the end of the
// test sequence
//
// 1     7/21/04 10:03p Cward
// Moved files from Upper level directory to Library
//
// 1     12/21/03 6:34p Cward
//
// 3     10/24/03 10:50a Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 3     10/10/03 3:41p Khykin
// Updated with Brians latest changes.
//
// 2     10/10/03 9:14a Khykin
// Added the update of the StartVehicleTest tag when commanded to
// StartVehicleTest and when the test is complete.
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
// 1     1/18/03 7:47a Khykin
// Initial Checkin
//
// 13    10/21/02 11:13a Khykin
// Updated for setting the Sequencers ability to perform a retest based on
// the type of test that was sequenced.
//
// 12    9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 11    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 9     5/24/02 1:49p Kh
// Updating due to debugging.
//
// 8     3/26/02 8:14p Kh
// Adding the incrementing of the logger test.
//
// 7     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
//
// 6     2/06/02 12:20a Kh
// Adding the initialization of m_dataTags.
//
// 5     12/20/01 10:19a Kh
// Updating the Vehicle test to be a BEP Server.
//
// 4     12/04/01 11:29a Kh
// Updating the Vehicle test to be a BEP Server.
//
// 3     11/30/01 8:16a Kh
// Updating the Vehicle test to be a BEP Server.
//
// 2     8/17/01 2:36p Kh
// Checking in for weekly backup.
//
// 1     7/06/01 5:34p Kh
// Checking in the weeks work.  Not functional just written and in the
// process of debug.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "VehicleTest.h"

VehicleTest::VehicleTest() : BepServer(), m_semCommandTest(0), m_semVehicleBuild(1), m_faultServer(NULL),
    m_maxTestNumber(1000), m_sequencerComm(NULL), m_fault(""), m_terminateComponentsOnTestComplete(false),
    m_useVinAsTestLogSuffix(false), m_testCommandInProgress(false)
{
    SetStatus(BEP_RUNNING_STATUS);      // set the status to running
    m_vehicleBuild = NULL;              // set the vehicle build to NULL
    m_testNumber = 0;                   // reset the test number count
}

/**
 * Destructor for the Vehicle Test Class.
 * @since Version 1.0
 */
VehicleTest::~VehicleTest()
{
    Log(LOG_FN_ENTRY, "Vehicle Test Terminating\n");
    // Instruct the test sequencer to terminate all components
    m_testSequencer.Terminate();
    // if the vehicle build is still loaded, delete it
    if(m_vehicleBuild != NULL)  delete m_vehicleBuild;
    // if a fault server interface was created, delete it
    if(m_faultServer != NULL)   delete m_faultServer;
    // if a test sequencer comm obj exists, delete it
    if(m_sequencerComm)         delete m_sequencerComm;

    Log(LOG_FN_ENTRY, "Vehicle Test Terminating done\n");
}

void VehicleTest::Initialize(const std::string &fileName)
{
    try
    {
        Log(LOG_FN_ENTRY, "VehicleTest::Initialize(%s)\n", fileName.c_str());
        SetConfigFile(fileName);    // Set name of configuration file
        const XmlNode *document = m_parser.ReturnXMLDocument(fileName); // Parse configuration file
        if(document)
        {
            Initialize(document);       // initialize server
            Log(LOG_FN_ENTRY, "VehicleTest::Initialize(%s) done\n", fileName.c_str());
        }
        else Log(LOG_ERRORS, "VehicleTest::Initialize() Failed to parse configuration file\n");
    }
    catch(BepException &BepErr)
    {
        Log(LOG_ERRORS, "VehicleTest::Initialize(%s) failed: %s\n", fileName.c_str(), BepErr.what());
        throw;
    }
    catch(...)
    {
        Log(LOG_ERRORS, "VehicleTest::Initialize(%s) failed: for an unknown reason\n", fileName.c_str());
        throw;
    }
}

void VehicleTest::Initialize(const XmlNode *document)
{
    // Initialize the base component
    Log(LOG_FN_ENTRY, "Calling BepServer::Initialize(document)\n");
    BepServer::Initialize(document);

    Log(LOG_FN_ENTRY, "VehicleTest::Initialize(document)\n");
    const XmlNode *setup = document->getChild("Setup"); // get reference to the setup node

    // create the channel for getting component responses
    try
    {
        Log(LOG_DEV_DATA, "VehicleTest Initialize SequencerComm");
        m_sequencerComm = new IBepCommunication();
        m_sequencerComm->Initialize(setup->getChild("SequencerComm"));
    }
    catch(XmlException &e)
    {
        Log(LOG_ERRORS, "VehicleTest SequencerComm Error: %s\n", e.Reason().c_str());
        throw BepException("VehicleTest SequencerComm Error: %s", e.Reason().c_str());
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "VehicleTest SequencerComm Error: %s\n", e.Reason().c_str());
        throw BepException("VehicleTest SequencerComm Error: %s", e.Reason().c_str());
    }

    // initialize the sequencer
    try
    {
        Log(LOG_DEV_DATA, "VehicleTest Initialize Sequencer");
        m_testSequencer.Initialize(setup->getChild("Sequencer"));
        m_testSequencer.SetSequencerCommObj(m_sequencerComm);
    }
    catch(XmlException &e)
    {
        throw BepException("Error getting Sequencer info: %s", e.what());
    }

    // initialize the selector
    try
    {
        Log(LOG_DEV_DATA, "VehicleTest Initialize selector");
        m_testSelector.Initialize(setup->getChild("Selector"));
    }
    catch(XmlException &e)
    {
        throw BepException("Error getting Selector info: %s", e.what());
    }

    // Store the flag indicating if test components terminate at end of test sequence
    try
    {
        Log(LOG_DEV_DATA, "VehicleTest Initialize component termination");
        TerminateComponentsOnTestComplete(!setup->getChild("TerminateTestComponentsAtEnd")->getValue().compare("Yes"));
    }
    catch(XmlException &e)
    {
        TerminateComponentsOnTestComplete(false);
    }
    // Get the max index number allowed for the test logs
    try
    {
        Log(LOG_DEV_DATA, "VehicleTest Set maximum index number");
        SetMaxIndexNumber(BposReadInt(document->getChild("Setup/MaxLogFileIndex")->getValue().c_str()));
    }
    catch (XmlException &excpt)
    {
        SetMaxIndexNumber(10000);
    }

    try
    {
    // Store the log index file name
        Log(LOG_DEV_DATA, "VehicleTest Initialize the log file index");
    InitializeLogFileIndex(document);
}
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Initialize log file index Error: %s\n", e.Reason().c_str());
        throw BepException("Initialize log file index Error: %s", e.Reason().c_str());
    }

}

void VehicleTest::InitializeLogFileIndex(const XmlNode *document)
{
    Log(LOG_DEV_DATA, "VehicleTest::InitializeLogFileIndex() - Enter\n");
    try
    {
        SetIndexFileName(document->getChild("Setup/LogIndexFile")->getValue());
    }
    catch (XmlException &excpt)
    {
        SetIndexFileName("/Configuration/Machine/NextTestLogIndex.dat");
    }

    //Set default test log suffix variables
    m_useVinAsTestLogSuffix = false;
    m_vinSuffixLength = 8;
    m_suffixExtension = "log";
    try
    {
        // Get the suffix extension for the test logs
        m_suffixExtension = document->getChild("Setup/SuffixExtension")->getValue().c_str();
        Log(LOG_DEV_DATA, "VehicleTest::InitializeLogFileIndex log suffix extension: %$s\n",
            m_suffixExtension.c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Use vin as log suffix Error\n" );
    }

    try
    {
        // Get the setting which specified whether to use sequenced numbers or vin numbers as log suffixes
        m_useVinAsTestLogSuffix = (document->getChild("Setup/UseVinAsTestLogSuffix")->getValue().compare("Yes") == 0);

        if(m_useVinAsTestLogSuffix)
        {   // Get the length of the end of a vin to use for the test logs
            m_vinSuffixLength = BposReadInt(document->getChild("Setup/VinSuffixLength")->getValue().c_str());
            Log(LOG_DEV_DATA, "VehicleTest::InitializeLogFileIndex Use vin length %i for log suffix\n",
                m_vinSuffixLength );
        }
    //Log the results from the suffix init
    Log(LOG_DEV_DATA, "VehicleTest::InitializeLogFileIndex Use vin as log suffix: %s : %d\n",
        document->getChild("Setup/UseVinAsTestLogSuffix")->getValue().c_str(), m_useVinAsTestLogSuffix);
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Use vin as log suffix Error\n" );
    }
    // Get the log file number from the file
    IncrementLogFileIndex(true);
    Log(LOG_DEV_DATA, "VehicleTest::InitializeLogFileIndex() - Exit\n");
}

void VehicleTest::IncrementLogFileIndex(bool initializing) /*= false*/
{
    FILE *indexFile = NULL;
    Log(LOG_FN_ENTRY, "VehicleTest::IncrementLogFileIndex() - Enter - using %s", GetIndexFileName().c_str());
    // Check if the index number should be read from the file
    if (initializing)
    {   // Attempt to open the data file
        if ((indexFile = fopen(GetIndexFileName().c_str(), "r")) != NULL)
        {   // Log file open, get the next index number to use
            fscanf(indexFile, "%d", &m_testNumber);
            // Close the log file
            fclose(indexFile);
            Log(LOG_DEV_DATA, "Start with log index: %d", m_testNumber);
            // NOTE:  Log file number is decremented so we do not skip a number when we start.
            m_testNumber--;
        }
        else
        {   // NOTE:  Leave m_testNumber alone, whatever it was from instantiation.
            //        This could be the first time this feature is used on the system.
            Log(LOG_ERRORS, "IncrementLogFileIndex() - cannot open %s", GetIndexFileName().c_str());
        }
    }
    // Update the test log number and range check it
    m_testNumber++;
    if ((m_testNumber > GetMaxIndexNumber()) || (m_testNumber < 0)) m_testNumber = 0;
    // Write the new index number to the index file
    if ((indexFile = fopen(GetIndexFileName().c_str(), "w")) != NULL)
    {   // Write the index number to file and close the file
        fprintf(indexFile, "%d", m_testNumber);
        fclose(indexFile);
    }
    else
    {
        Log(LOG_ERRORS, "Cannot open %s to update the index number", GetIndexFileName().c_str());
    }
    Log(LOG_FN_ENTRY, "VehicleTest::IncrementLogFileIndex() - Exit: index Number: %d", m_testNumber);
}

void VehicleTest::SetIndexFileName(const string &name)
{
    m_dataFile = getenv("USR_ROOT") + name;
}

const string& VehicleTest::GetIndexFileName(void)
{
    return(m_dataFile);
}

void VehicleTest::SetMaxIndexNumber(const INT32 &maxNumber)
{
    m_maxTestNumber = maxNumber;
}

const INT32& VehicleTest::GetMaxIndexNumber(void)
{
    return(m_maxTestNumber);
}

const std::string VehicleTest::GetLogSuffix()
{
    char buffer[128];

    std::string suffix;
    std::string sequence;
    try
    {
        //Get the sequenced test number
        sequence = itoa(m_testNumber, buffer, 10);

        // If we have a file extension from our config file
        if( m_suffixExtension.empty() == false)
        {
        //Defaults the suffix to be just the sequenced test number with extension
        suffix = sequence;
        suffix += "." + m_suffixExtension;
    }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Converting sequenced test number to string Error: %s\n", e.Reason().c_str());
        throw BepException("Converting sequenced test number to string Error: %s", e.Reason().c_str());
    }

    if(m_useVinAsTestLogSuffix)
    {   //Add the vin into the suffix as requested

        Log(LOG_DEV_DATA, "Using vin as log suffix\n");

        std::string         response, value;
        const std::string   vinTag(GetDataTag( "Vin"));   // vin tag to check

        //Read current vin from the named data broker
        if(m_ndb.Read( vinTag, response, true) == BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "Reading vin tag: %s, response: %s\n",
                vinTag.c_str(), response.c_str());

            //Get the value from the data broker
            if(m_ndb.GetByTag(vinTag, value, response) == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Reading vin tag: %s, value: %s\n",
                    vinTag.c_str(), value.c_str());

                //Get the last digits from the vin value
                UINT32 lastDigitCount = m_vinSuffixLength;
                if(value.size() >= lastDigitCount)
                {
                    try
                    {
                        //Combine the vin with the sequence number for the following format
                        // format: TestLog.[VIN].[SequenceNumber].log
                        suffix = ".";
                        suffix += (value.substr(value.size()-lastDigitCount, lastDigitCount));
                        suffix += ".";
                        suffix += sequence;
                        suffix += "." + m_suffixExtension;
                    }
                    catch(BepException &e)
                    {
                        Log(LOG_ERRORS, "Error converting last digits of vin to string: %s error:\n",
                            value.c_str(), e.what());
                    }
                }
                else
                {
                    //Default to just the sequence number if the vin is not valid length
                    Log(LOG_ERRORS, "Vin is not a valid length(<%d) vin:%s (using sequence number)", lastDigitCount, value.c_str());
                    suffix = sequence;
                    suffix += "." + m_suffixExtension;
                }
            }
            else
            {
                Log(LOG_ERRORS, "Error reading vin tag: %s, value: %s\n",
                    vinTag.c_str(), value.c_str());
            }
        }
        else
        {
            Log(LOG_ERRORS, "Error reading vin tag: %s, response: %s\n",
                vinTag.c_str(), response.c_str());
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Using incremental index as log suffix\n");
    }

    Log(LOG_DEV_DATA, "Log suffix: %s", suffix.c_str());
    return(suffix);
}

void VehicleTest::Run(volatile bool *terminateFlag)
{
    std::string driveCurve = "";        // the current drive curve
    std::string test = "";              // the test that was commanded
    std::string testStatus = "";        // the status of the commanded test
    const XmlNode *vehicleBuild = NULL; // the current vehicle build that is being used
    std::string localizedDriveCurve;    // the localized drive curve
    std::string rewriteDIR;             // directory to load drivecurve from

    while(GetStatus() != BEP_TERMINATE_STATUS)
    {
        try
        {   // signal the end of the test
            Log(LOG_DEV_DATA, "VehicleTest Priority: %d\n", BposGetPriority(0));
            // Ensure we only command test end one time
            CommandTest(TEST_END);
            if (TerminateComponentsOnTestComplete())
            {
                m_testSequencer.RemoveTestComponents();
            }
            testStatus = "";                    // reset the test variables
            test = GetCommandedTest();          // wait for a test to be commanded

            testStatus = BEP_TESTING_RESPONSE;  // update the status to testing
            Log(LOG_DEV_DATA, "VehicleTest New Test Priority: %d\n", BposGetPriority(0));
            Write(GetDataTag("OverallPassFail"), testStatus);
            ClearFault();                       // if there is a currently active fault, clear it

            Log(LOG_DEV_DATA, "Starting Test: %s, Status: %s, %d\n", test.c_str(), GetStatus().c_str(), BposGetPriority(0));

            std::string vehicleBuildStr = "";
            vehicleBuild = GetVehicleBuild();
            vehicleBuild->writeNode(vehicleBuildStr);

            Log(LOG_DEV_DATA, "Vehicle build: %s", vehicleBuildStr.c_str());

            if(test != TEST_TERMINATE)
            {   // determine which drive curve to use
                if(test == TEST_START)          // if the beginning of a new test
                {
                    // load the vehicle build and determine which drive curve to load
                    vehicleBuild = GetVehicleBuild();
                    if(vehicleBuild != NULL)
                        driveCurve = m_testSelector.SelectDriveCurve(vehicleBuild, rewriteDIR);
                    Write(GetDataTag("DriveCurveRootDir"), rewriteDIR);
                    m_testSequencer.PerformRetest(true);
                }
                else if(test != TEST_RETEST)    // else if not begining of new test
                {
                    Log(LOG_DEV_DATA, "Loading Commanded Test: %s\n", test.c_str());
                    driveCurve = m_testSelector.SelectDriveCurve(test);
                    // if a LossCompensation test is being sequenced, do not perform a retest
                    if(test == TEST_LOSS_COMPENSATION)  m_testSequencer.PerformRetest(false);
                    // else perform a retest
                    else                                m_testSequencer.PerformRetest(true);

                }

                // if a valid drive curve exists, run through the sequencing process
                if(driveCurve != "")
                {
                    Log(LOG_DEV_DATA, "Have DriveCurve, Will Test: %s\n", driveCurve.c_str());
                    // publish the drive curve that is being used
                    Write(GetDataTag("DriveCurve"), driveCurve);

#if 0
                    // start a new logger test file if a new test
                    if(test != TEST_RETEST)
                    {
                        //The following only accepted int32 and program must allow alphanumeric vin
                        //NewTestNumberLog(GetLogSuffix());

                        //This is what NewTestNumberLog was calling
                        SendToDaemon(ROLL_NEW_TEST_NUMBER, GetLogSuffix());

                        //Always increment the log file counter
                        IncrementLogFileIndex();
                    }
#else
                    PreTestInit( test, driveCurve);
#endif
                    string rootDIR = (rewriteDIR != "") ? rewriteDIR : getenv("FTP_ROOT");
                    // prepend the FTP_ROOT to the drive curve for loading
                    localizedDriveCurve = rootDIR + driveCurve;
                    Log(LOG_FN_ENTRY, "Loading The Drive Curve: %s\n", localizedDriveCurve.c_str());
                    std::string msgResponse;

                    // load the drive curve
                    const XmlNode *document = m_driveCurveParser.ReturnXMLDocument(localizedDriveCurve);

                    // sequence the test
                    Log(LOG_DEV_DATA, "Loading Drive Curve %s\n", driveCurve.c_str());
                    m_testSequencer.LoadDriveCurve(document, rewriteDIR);
                    Log(LOG_DEV_DATA, "Sequencing Test Type: %s\n", test.c_str());

                    // update the test status, and notify subscribers
                    // NOTE: only do this right before the sequencing to prevent race conditions
                    Write(GetDataTag("TestInProgress"), "1");
                    Write(GetDataTag("ForegroundTestInProgress"), "1");

                    //set test starting bool false - protection no longer needed
                    m_testCommandInProgress = false;

                    testStatus = m_testSequencer.SequenceTest(test);
                    Log(LOG_DEV_DATA, "Test Complete: %s\n", testStatus.c_str());
                    // notify all of the subscribers for the completed test
                    Write(GetDataTag("OverallPassFail"), testStatus);
                    Log(LOG_DEV_DATA, "Notification Complete\n");
                    // if a software fail occurred, report it on the machine fault list
                    if(testStatus == BEP_SOFTWAREFAIL_STATUS)
                    {
                        char buffer[128];       // buffer to create messages with
                        throw BepException(CreateMessage(buffer, sizeof(buffer), "Error Curve: %s, Reason: %s", driveCurve.c_str(), m_testSequencer.GetFailureString().c_str()));
                    }
                    // if a hardware fail occurred, report it on the machine fault list
                    else if(testStatus == BEP_HARDWAREFAIL_STATUS)
                        throw BepException("Hardware Failure Sequencing: %s", driveCurve.c_str());
                }
                else                        // if not we are done
                {
                    Log(LOG_ERRORS, "VehicleTest Error: No Drive Curve Specified\n");
                    throw BepException("No Drive Curve Avaliable For Specified Vehicle");
                }
            }
            else
            {
                SetStatus(BEP_TERMINATE_STATUS);
                Log(LOG_ERRORS, "Terminate Received By Run(): %s\n", GetStatus().c_str());
            }
        }
        catch(BepException &e)
        {   // report the fault to the fault server
            std::string fault = "Vehicle Test: " + e.Reason();
            SetFault(fault);
            Log(LOG_ERRORS, "VehicleTest Exception: %s\n", e.what());
        }
        catch(...)
        {   // report the fault to the fault server
            std::string fault = "VehicleTest Unknown Exception";
            SetFault(fault);
            Log(LOG_ERRORS, "VehicleTest Unknown Exception\n");
        }
    }
    Log(LOG_DEV_DATA, "VehicleTestRunner Calling BepServer::Run(): %s\n", GetStatus().c_str());
    BepServer::Run();   // perform the base class cleanup
    Log(LOG_FN_ENTRY, "VehicleTestRunner Terminated\n");
}

const std::string VehicleTest::Command(const XmlNode *node)
{    // currently only support Start, Retest, and Continue
    std::string status;

    Log(LOG_FN_ENTRY, "VehicleTest::Command(%s)\n", node->ToString().c_str());

    // if a specific test was commanded
    if(node->getName() == GetDataTag("CommandTest"))
    {
        if((status = CommandTest(node->getValue())) != BEP_SUCCESS_RESPONSE)
        {   // if not a command, it must be something else
            Log(LOG_ERRORS, "VehicleTest: Received Unsuported Request: %s, %s\n",
                node->getName().c_str(), node->getValue().c_str());
        }
    }
    // else if commanded to start a vehicle test
    else if( node->getName() == GetDataTag( "StartVehicleTest"))
    {
        if(Read(GetDataTag("TestInProgress")) != "1")
        {
            // indicate that initialization is in progress
            Log(LOG_DEV_DATA, "Updating the PhaseStatus: %s\n", TEST_STATUS_NOT_STARTED);
            SetData(PHASE_STATUS_TAG, TEST_STATUS_NOT_STARTED);
            // start the test, we have a new build
            if((status = CommandTest(TEST_START)) != BEP_SUCCESS_RESPONSE)
            {
                Log(LOG_ERRORS, "ERROR! Unable To Start A New Vehicle Test\n");
            }
        }
        else
        {
            Log(LOG_ERRORS, "Error, StartTest is being commanded during a test\n");
        }
    }
    // else it is an unsupported command
    else
        Log(LOG_ERRORS, "VehicleTest Unsupported Command %s, %s\n",
            node->getName().c_str(), node->getValue().c_str());

    return(status);
}

const std::string VehicleTest::Publish(const XmlNode *node)
{
    std::string status;

    status = SetData(node);
    // if a StartLossCompensation signal
    if(node->getName() == GetDataTag("StartLossCompensation"))
    {   // check the status to determine if new test should be started
        if(node->getValue() == "1")
            CommandTest(TEST_LOSS_COMPENSATION);
    }
    else if(node->getName() == GetDataTag("RetainersDown"))
    {   // if the retainers were lovered and we are in the middle of a test, abort
        if((GetStatus() == BEP_TESTING_STATUS) && (node->getValue() == "1"))
        {
            m_testSequencer.Abort();
        }
    }
    else if(!node->getName().compare(ABORT_DATA_TAG) && atob(node->getValue().c_str()))
    {
        Abort();
    }
    else
    {
        Log(LOG_DEV_DATA, "VehicleTest Publish Received: %s\n", node->getName().c_str());
    }

    return(status);
}

void VehicleTest::Abort(void)
{
    try
    {
        SetStatus(BEP_ABORT_STATUS);    // set the status to Termiante
        m_testSequencer.Abort();        // terminante the testing framework
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Aborting: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Error Aborting\n");
    }
}

void VehicleTest::Terminate(void)
{
    try
    {
        Log(LOG_FN_ENTRY, "VehicleTest Received Terminate\n");
        m_testSequencer.Terminate();    // terminate the vehicle components
        BepServer::Terminate();         // terminate the server
        CommandTest(TEST_TERMINATE);    // command the test to terminate
        Log(LOG_FN_ENTRY, "VehicleTest Received Terminate done\n");
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Terminating: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Error Terminating\n");
    }
}

void VehicleTest::Reset(void)
{   // currently not implemented
    try
    {
        Log(LOG_FN_ENTRY, "VehicleTest Received Reset Command\n");
        m_testSequencer.Reset();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Resetting: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Error Resetting\n");
    }
}

void VehicleTest::ReloadConfig(void)
{   // not implemented
    try
    {
        Log(LOG_FN_ENTRY, "VehicleTest Received ReloadConfig Command\n");
        BepServer::ReloadConfiguration();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Reloading The Configuration: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Error Reloading The Configuration\n");
    }
}

const std::string VehicleTest::Register(void)
{
    Log(LOG_FN_ENTRY, "Registering the VehicleTest\n");

    try
    {
        // command the base class to register
        BepServer::Register();

        // initialize the fault server if it has not already been initialized
        if(!m_faultServer)
        {
            Log(LOG_DEV_DATA, "Creating a new IFaultServer\n");
            m_faultServer = new IFaultServer();
            m_faultServer->Initialize(FAULT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT, IsDebugOn());
        }

        // command the TestSequencer to register
        m_testSequencer.Register();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Registering: %s\n", e.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "Error Registering\n");
    }

    return(BEP_SUCCESS_RESPONSE);
}

INT32 VehicleTest::ReadNewVehicleBuild()
{
    XmlParser           vehBldParser;
    std::string         response, bldString;
    const std::string   rootTag(GetDataTag( "VehicleBuild"));   // root tag used for parsing
    INT32               status = BEP_STATUS_ERROR;              // status of the operation

    try
    {
        // Read current vehicle build
        status = m_ndb.Read( rootTag, response, true);
        if(status == BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "VehicleBuild Response: %s\n", response.c_str());

            // Get the build
            XmlNode vehicleBuildData(rootTag, "");
            m_ndb.GetNext(vehicleBuildData, response);
            Log(LOG_DEV_DATA, "Next String Received: %s\n", vehicleBuildData.ToString().c_str());


            // if the result is not a software fail
            if(vehicleBuildData.getValue() != BEP_SOFTWAREFAIL_RESPONSE)
            {   // Set the vehicle build
                SetVehicleBuild( &vehicleBuildData);
            }
            else
            {
                Log(LOG_ERRORS, "Software Failure Reading The Vehicle Build\n");
                status = BEP_STATUS_ERROR;
            }
        }
        else
        {
            Log(LOG_ERRORS, "VehicleTest Error Reading Vehicle Build: %d\n", status);
        }
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "XmlException reading/parsing build data: %s\n", err.GetReason());
        status = BEP_STATUS_ERROR;
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "BepException reading/parsing build data: %s\n", err.GetReason());
        status = BEP_STATUS_ERROR;
    }
    catch( ...)
    {
        Log( LOG_ERRORS, "Unknown exception reading/parsing build data\n");
        status = BEP_STATUS_ERROR;
    }

    return(status);
}

XmlNode *VehicleTest::GetVehicleBuild(void)
{
    return(m_vehicleBuild);
};

void VehicleTest::SetVehicleBuild(const XmlNode *vehicleBuild)
{
    Log(LOG_FN_ENTRY, "Setting The Vehicle Build\n");
    m_semVehicleBuild.CriticalSectionEnter();   // wait till can update vehicle build
    if(m_vehicleBuild != NULL)                  // if not == NULL delete it
    {
        delete m_vehicleBuild;                  // delete
        m_vehicleBuild = NULL;                  // set to null
    }
    m_vehicleBuild = vehicleBuild->Copy();              // update the vehicle build
    m_semVehicleBuild.CriticalSectionExit();    // wait till can update vehicle build
    Log(LOG_FN_ENTRY, "Vehicle Build Set\n");
};

std::string VehicleTest::GetCommandedTest(void)
{
    m_semCommandTest.CriticalSectionEnter();    // wait till test commanded
    std::string type = m_testType;              // get the test type
    m_testType = "";                            // reset the test type
    Log(LOG_DEV_DATA, "GetTestCommanded: %s\n", type.c_str());
    return(type);
};

std::string VehicleTest::CommandTest(const std::string testType)
{
    std::string status = BEP_SUCCESS_RESPONSE;      // status of the commanded test
    std::string testStatus = "0";                   // the status of the vehicle test

    Log( LOG_FN_ENTRY, "Enter VehicleTest::CommandTest(%s)\n", testType.c_str());

    Log(LOG_DEV_DATA, "VehicleTest::CommandTest - Checking the status of the machine:");
    Log(LOG_DEV_DATA, "\tTestInProgress: %s", Read(GetDataTag("TestInProgress")).c_str());
    Log(LOG_DEV_DATA, "\tm_testCommandInProgress: %d", m_testCommandInProgress);
    Log(LOG_DEV_DATA, "\ttestType: %s", testType.c_str());
    
    // check the test status so that only one test can be sequenced at a time
    if((Read(GetDataTag("TestInProgress")) != "1") && !m_testCommandInProgress)
    {
        Log(LOG_DEV_DATA, "Commanding Test %s\n", testType.c_str());

        if(testType == TEST_START)                      // if a new test commanded
        {
            // Get the latest vehicle build data
            if(ReadNewVehicleBuild() == BEP_STATUS_SUCCESS)
                m_testType = testType;
            else    // if unable to get it do not start a test
                status = BEP_INVALID_RESPONSE;
        }
        else if( (testType == TEST_RETEST) ||               // if a retest commanded
                 (testType == TEST_LOSS_COMPENSATION) ||    // if loss compensation command
                 (testType == TEST_BASE_BRAKE) ||           // if base brake command
                 (testType == TEST_TERMINATE))              // if terminate command
        {
            m_testType = testType;
        }
        else                                            // else set status to invalid
        {
            status = BEP_INVALID_RESPONSE;
        }

        // if successful command and not the end of the test
        if(status != BEP_INVALID_RESPONSE)
        {
            testStatus = "1";   //set the status to testing

            // if the test was not terminated
            if(testType != TEST_TERMINATE)
            {   // set the status to testing
                SetStatus(BEP_TESTING_STATUS);
            }
            // start the test
            m_semCommandTest.CriticalSectionExit();
            // set test command in progress to protect from commmanding a test twice
            m_testCommandInProgress = true;
        }

        if (testType == TEST_LOSS_COMPENSATION) 
        {
            Write(VIN_DATA_TAG, LOSS_COMPENSATION_VIN);
        }

        // indicate the test status
        Write(START_VEHICLE_TEST_DATA_TAG, testStatus);

    }
    else    // if testing check the other conditions
    {
        if(testType == TEST_END)                        // if the end of the test
        {
            SetStatus(TEST_END);                        // flag the vehicle test as complete
            m_testType = testType;                      // set the test to test end
        }
        else if(testType == TEST_ABORT)                 // if test abort command
            m_testType = testType;
        else if(testType == TEST_TERMINATE)             // if terminate command
            m_testType = testType;
        else
        {
            status = BEP_INVALID_RESPONSE;
            Log(LOG_ERRORS, "Received Invalid Command During Test: %s\n", testType.c_str());
        }

        // if successful command and not the end of the test
        if(status != BEP_INVALID_RESPONSE)
        {   // update the test status, and notify subscribers
            Write(GetDataTag("TestInProgress"), testStatus);
            // indicate that the test has ended
            Write(START_VEHICLE_TEST_DATA_TAG, testStatus);
        }

    }
    Log( LOG_FN_ENTRY, "Exit VehicleTest::CommandTest(%s->%s), status=%s\n",
         testType.c_str(), m_testType.c_str(), status.c_str());

    return(status);
};

void VehicleTest::SetFault(std::string &fault)
{
    // if a fault is to be set
    if(!fault.empty())
    {
        ClearFault();   // clear any fault that might currently exist
        std::string response;
        INT32 severity = 10; // any test fault at the VehicleTest level is a severity of 10
        // store the currently active fault
        m_fault = fault;
        // if there is a valid fault server interface
        if(m_faultServer)   m_faultServer->SetFault(std::string("TestFault"), m_fault, GetProcessName(), response, true, severity);
        Log(LOG_ERRORS, "VehicleTest Setting Fault: %s\n", m_fault.c_str());
    }
}

void VehicleTest::ClearFault(void)
{
    // if a fault was reported and there is a valid interface, clear it
    if(!m_fault.empty() && (m_faultServer != NULL))
    {
        std::string response;
        Log(LOG_DEV_DATA, "Vehicle Test Clearing Fault: %s\n", m_fault.c_str());
        if(m_faultServer->ClearFault(std::string("TestFault"), response, true) != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Unable to clear the fault: %s\n", m_fault.c_str());
        m_fault.erase();    // reset the fault information
    }
}

const std::string VehicleTest::GetFault(void)
{
    return(m_fault);
}


const bool& VehicleTest::TerminateComponentsOnTestComplete(void)
{
    return(m_terminateComponentsOnTestComplete);
}

void VehicleTest::TerminateComponentsOnTestComplete(const bool& terminateTestComplete)
{
    m_terminateComponentsOnTestComplete = terminateTestComplete;
}

void VehicleTest::PreTestInit(const std::string &test, const std::string &driveCurve)
{
    // start a new logger test file if a new test
    if(test != TEST_RETEST)
    {
        //The following only accepted int32 and program must allow alphanumeric vin
        //NewTestNumberLog(GetLogSuffix());

        //This is what NewTestNumberLog was calling
        SendToDaemon(ROLL_NEW_TEST_NUMBER, GetLogSuffix());

        //Always increment the log file counter
        IncrementLogFileIndex();
    }
}

