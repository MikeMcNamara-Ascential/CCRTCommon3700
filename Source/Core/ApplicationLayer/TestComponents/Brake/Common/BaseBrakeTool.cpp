//*************************************************************************
// Description:
//
// This file contains the functions necessary to perform a base brake test.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Brake/Common/BaseBrakeTool.cpp $
// 
// 1     11/01/07 2:05p Derickso
// Updated ForceThread to accept a BaseBrakeTool instead of a void *.
// 
// 1     10/01/07 4:08p Derickso
// Added force multipliers to satisfy max brake force testing for Asia.
// 
// 18    2/22/07 8:48a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added m_calcBrakeForce = BEP_UNTESTED_RESPONSE in WarmInitialize().
// 
// 17    1/17/07 3:08p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made ForceThread() a member of the class.
// - Added m_launchForceThread.
// - Added WarmInitialize() and Deactivate().
// - Moved the delay in ForceThread() outside the try block.  So that is
// always gets called.
// 
// 16    10/25/06 3:45p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Changed the test result name to "ElapsedTime" in
// TestStepAccelerate()
// 
// 15    4/13/06 3:18a Cward
// TestStepAccelerate() calls SendTestResultWithDetail() to report arm
// speed and acceleration time Changed hard coded unit strings with new
// units strings from GenericTC.h
//
// 2     3/20/06 10:43a Bmeinke
// TestStepAccelerate() calls SendTestResultWithDetail() to report arm
// speed and acceleration time
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 14    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 4     8/24/05 8:42a Bmeinke
// In ValidateTandemBalances(), check if size of the brake force vector > 4
// instead of roller count > 4 for Tata
//
// 3     8/23/05 10:47a Bmeinke
// Added SetBrakeForceValue() method in order to be able to compute brake
// balance values for tandem axle testing
//
// 13    8/17/05 8:12p Cward
// Corrected format error in Analyze Brake Forces method.
//
// 12    5/25/05 8:55p Cward
// Updated AnalyzeValveCross and updated the structure of the Input Server
//
// 11    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 11     4/12/05 5:29p Rwiersema
// Added methods for new Coast test.  Also added more description for
// test reporting in AnalyzeDrags() and AnalyzeBrakeForces()
//
// 10    3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 9     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 8     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 8     1/31/05 9:08a Bmeinke
// Changed m_updateForceSem from a BepSemaphore to a BepCondVar
// Inside Brake(): always insert a "safe" brake start tag in case the
// brake force never reaches the "StartSamplingAtMinForce" value
//
// 7     1/17/05 9:57a Bmeinke
// In AverageForces() make sure the m_wheelForcesArray contains a force
// array for the desired wheel before trying to get a reference to it
// (will SIGSEG if the desired array does not exist)
//
// 6     1/05/05 4:37p Bmeinke
// Inside the Brake() method, changed baseBrakeSampleCount =
// m_component->GetParameterBool() to baseBrakeSampleCount =
// m_component->GetParameterInt()
//
// 5     1/05/05 3:48p Bmeinke
// Fixed the Brake() method so that we will sample 'n' samples above min
// force IFF the baseBrakeSampleCount > 0 instead of >=0
//
// 4     12/28/04 12:59p Bmeinke
// Changed the logic inside Brake() to handle the 2 different methods of
// starting/stopping brake sampling
//
// 3     12/27/04 7:41p Bmeinke
// Clear m_dragForces at the beginning of AnalyzeDrags()
// Clear m_brakeForceat the beginning of AnalyzeBrakeForces()
//
// 2     12/22/04 7:56p Bmeinke
// Added support for writing/clearing drag forces to the ICM in order to
// remove the  drag forces from the calculated brake force values (needed
// in order to report accurate brake force data to Ford DCL). Set the ICM
// drag forces to 0 at the start of Drag() and write the drag force values
// to the ICM at the end of AnalyzeDrag() if the IncludeDragInBrakeForce
// parameter is false.
// Modified the Brake() method so we can start sampling brake force IFF we
// achieve min brake force and end sampling after 'n' samples instead of
// basing the brake force sampling on vehicle speed (sampling from start
// speed to end speed is still supported)
// Removed 'IncludeDragInBrakeForce compensation' in lieu of the new ICM
// drag compensation
//
// 7     9/29/04 10:09p Cward
// clear drag force and brake force lists before adding new forcesl.
// Primarialy for handling a retest.
//
// 6     9/01/04 8:58p Cward
// Added parameters to allow retaining test information when the component
// terminates and to ignore machine conditions.
//
// 5     8/18/04 11:09p Cward
//
// 4     6/24/04 12:40a Cward
// Removed pound if 0 block. Updated to new standard comment header
// block.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 4     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 3     11/13/03 7:04p Derickso
// Removed vehicle present requirement.
// updated to have base brake test step remove the prompts it displays.
//
// 2     10/27/03 3:59p Derickso
// Latest working BaseBrakeTool.
// Includes debugging from PIT testing.
//
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "BaseBrakeTool.h"

//-------------------------------------------------------------------------------------------------
BaseBrakeTool::BaseBrakeTool(GenericTC *tc) : m_calcBrakeForce(BEP_UNTESTED_RESPONSE),
m_dragStart(0), m_dragEnd(0), m_brakeStart(0), m_brakeEnd(0),
m_readForces(1), m_semReadCurrentForce(1), m_forceThreadID(-1), m_updateForceFlag(STOP),
m_forceValue(0), m_forceFile(-1), m_forceFileOpen(false),
m_launchForceThread(true)
{
    if(tc == NULL)
        throw BepException();

    m_component = tc;
    m_dragTestStats.reserve(MAXWHEELS);
    m_dragTestStats.resize(MAXWHEELS);
    m_brakeTestStats.reserve(MAXWHEELS);
    m_brakeTestStats.resize(MAXWHEELS);
}

//-------------------------------------------------------------------------------------------------
BaseBrakeTool::~BaseBrakeTool()
{
    // reset the system tags that are specified in the component list
    if(!m_component->GetParameterBool("KeepTestStatusOnExit"))  m_component->ResetSystemTags();
    // terminate the force updates to the gui
    TerminateForceUpdates();
    CloseForceFile();
    m_component->Log(LOG_FN_ENTRY, "Base Brake Tool Destructor complete\n");
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::WarmInitialize()
{
    m_component->Log(LOG_FN_ENTRY, "Enter BaseBrakeTool::WarmInitialize()\n");

    m_dragStart = 0;
    m_dragEnd = 0;
    m_brakeStart = 0;
    m_brakeEnd = 0;
    m_forceValue = 0;
    m_calcBrakeForce = BEP_UNTESTED_RESPONSE;

    m_updateForceFlag = STOP;

    if(m_launchForceThread == true)
    {
        // Set to disble before launching so thread does not die
        DisableForceUpdates();
        LaunchForceThread();
    }

    m_component->Log(LOG_FN_ENTRY, "Exit BaseBrakeTool::WarmInitialize()\n");
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::Deactivate()
{
    m_component->Log(LOG_FN_ENTRY, "Enter BaseBrakeTool::Deactivate()\n");
    CloseForceFile();
    TerminateForceUpdates();

    m_dragStart = 0;
    m_dragEnd = 0;
    m_brakeStart = 0;
    m_brakeEnd = 0;
    m_forceValue = 0;

    m_component->Log(LOG_FN_ENTRY, "Exit BaseBrakeTool::Deactivate()\n");
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepInitialize(void)
{
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepInitialize()\n");
    string tag, value, response;            // communication strings
    INT32 testStatus = BEP_STATUS_FAILURE;  // the status of the test
    string status;
    if(m_forceThreadID == -1 || m_component->GetParameterBool("AlwaysStartGUIForceThread"))
    {
        m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepInitialize - Launching GUI Force Thread()\n");
        //Set to disble before launching so thread does not die
        DisableForceUpdates();
        LaunchForceThread();
    }
    // Check if test should be run
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            // tell the operator we are initializing for a base brake test
            if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // if this is a retest or the initial test, reset the system tags
            if((m_component->IsRetest() && (m_component->GetPreviousOverallResult() != testPass)) || !m_component->IsRetest())
            {
                m_component->ResetSystemTags();
            }
            // check for zerospeed, if not, prompt to stop
            testStatus = m_component->StatusCheck();
            if((testStatus != BEP_STATUS_SUCCESS) && (testStatus != BEP_STATUS_ABORT))
            {
                if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1")
                {   // display the zerospeed prompt in box2
                    std::string prompt = m_component->GetTestStepInfo("ZerospeedPrompt");
                    m_component->DisplayPrompt("2" , prompt);
                    while((m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1") && m_component->TimeRemaining())
                        BposSleep(100);
                    // remove the prompt
                    m_component->DisplayPrompt("2" , "Blank");
                }
                // if time remaining
                if(m_component->TimeRemaining())
                {
                    m_component->Log(LOG_DEV_DATA, "TestStepInitialize Zerospeed Seen\n");
                    // set the machine into the correct conditions for testing
                    testStatus = Engage();
                    // if a timeout occurred, set the status to error
                    if(!m_component->TimeRemaining())
                    {
                        m_component->Log(LOG_ERRORS, "Error TestStepInitialize Timeout\n");
                        testStatus = BEP_STATUS_TIMEOUT;
                    }
                }
                else    // else indicate timeout
                {
                    m_component->Log(LOG_ERRORS, "Timeout Waiting For Zerospeed\n");
                    testStatus = BEP_STATUS_TIMEOUT;
                }
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepInitialize Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        //Remove prompts
        m_component->RemovePrompts();

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepInitialize(): %s, %d\n", status.c_str(), testStatus);

    return(status);             // return the status
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::DisplaySpeedTimeGraph (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::DisplaySpeedTimeGraph()\n");
    if (!m_component->ShortCircuitTestStep())
    {
        try
        {
            //Set up Lower / upper limit lines if available
            m_component->SystemWrite(m_component->GetDataTag("GraphLowerSeries"), m_component->GetParameter("SpeedGraphLowerSeries"));
            m_component->SystemWrite(m_component->GetDataTag("GraphUpperSeries"), m_component->GetParameter("SpeedGraphUpperSeries"));
            //display graph
            m_component->SystemWrite(m_component->GetDataTag("DisplayDriverMonitor"),string("speedtime"));

            // update the test status
            m_component->UpdateResult(testStatus, status);
        }
        catch (BepException &e)
        {
            m_component->Log(LOG_ERRORS, "DisplaySpeedTimeGraph Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::DisplaySpeedTimeGraph(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

const string BaseBrakeTool::RemoveSpeedTimeGraph (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::RemoveSpeedTimeGraph()\n");
    if (!m_component->ShortCircuitTestStep())
    {
        try
        {
            //remove graph
            m_component->SystemWrite(m_component->GetDataTag("DisplayDriverMonitor"),string("drivermonitor")); 
            // update the test status
            m_component->UpdateResult(testStatus, status);
        }
        catch (BepException &e)
        {
            m_component->Log(LOG_ERRORS, "RemoveSpeedTimeGraph Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::RemoveSpeedTimeGraph(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

const string BaseBrakeTool::TestStepAccelerate (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    float armSpeed = -1;
    double elapsedTime = -1;
    double prevElapsedTime = -1;
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAccelerate()\n");
    // Check if test should be run
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {   // if the brake test was not a previous pass
            if(GetBrakeTestingStatus() != "Pass")
            {   // make sure everything is groovy with the machine
                testStatus = StatusCheck();
                if(testStatus == BEP_STATUS_SUCCESS)
                {   // set up the display (speed gauge)
                    testStatus = m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), m_component->GetTestStepInfo("SpeedTarget"));

                    if(testStatus == BEP_STATUS_SUCCESS)
                    {   // wait till arm speed is achieved or we timeout
                        armSpeed = fabs(m_component->GetTestStepInfoFloat("ArmSpeed"));
                        // Display Prompts
                        if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                            m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                        float speed=0;

                        //multiple acceleration points parameters
                        char buff[10];
                        int intermediateSpeedPoint = 0;
                        string speedPtTargetParam = "SpeedPoint";
                        float prevSpeedTarget;
                        float speedPtTarget = fabs(m_component->GetParameterFloat(speedPtTargetParam +
                                                                             CreateMessage( buff, sizeof(buff), "%02d", intermediateSpeedPoint)));
                        int numAccelPts = m_component->GetParameterInt("NumberOfAccelerationPoints");
                        bool recordAccelPtsDone = false;

                        if (m_component->GetParameterBool("DisplaySpeedTimeGraph"))
                        {
                            //Set up Lower / upper limit lines if available
                            m_component->SystemWrite(m_component->GetDataTag("GraphLowerSeries"), m_component->GetParameter("SpeedGraphLowerSeries"));
                            m_component->SystemWrite(m_component->GetDataTag("GraphUpperSeries"), m_component->GetParameter("SpeedGraphUpperSeries"));
                            //display graph
                            m_component->SystemWrite("DisplayDriverMonitor","speedtime"); 
                            //set start parameter
                            m_component->SystemWrite(m_component->GetDataTag("StartSpeedGraph"), 1);
                        }
                        if(m_component->GetParameterBool("WaitForAccelStart"))
                        {// wait for zero speed to be broken
                            if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "0")
                            {
                                bool done = false;
                                while((m_component->StatusCheck() == BEP_STATUS_SUCCESS) && m_component->TimeRemaining() && !done)
                                {
                                    if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1")
                                        BposSleep(250);
                                    else
                                        done = true;
                                }
                                m_component->SetStartTime();
                                m_component->SetSecondaryStartTime();
                                prevElapsedTime = m_component->GetElapsedTime();
                            }
                        }
                        do
                        {   // read the current speed and update the display
                            speed = fabs(GetSpeed());
                            if(speed < armSpeed)   // if not at speed yet
                            {
                                // check the machine status and wait if ok
                                testStatus = StatusCheck();
                                if(testStatus == BEP_STATUS_SUCCESS)
                                    BposSleep(100);
                            }
                            if(m_component->GetParameterBool("CheckAccelerationPoints") && !recordAccelPtsDone)
                            {
                                if(speed >= speedPtTarget)
                                {
                                    elapsedTime = m_component->GetElapsedTime();
                                    //Report acceleration zone elapse time
                                    char buff[ 256];
                                    string units;
                                    float reportRangeLow;
                                    float reportRangeHigh;
                                    if(m_component->GetParameterBool("ConvertToKPH"))
                                    {
                                        units = unitsKPH;
                                        reportRangeLow = prevSpeedTarget * 1.609344;
                                        reportRangeHigh = speedPtTarget * 1.609344;
                                    }
                                    else
                                    {
                                        units = unitsMPH;
                                        reportRangeLow = prevSpeedTarget;
                                        reportRangeHigh = speedPtTarget;
                                    }
                                    string accelMessage = string(CreateMessage( buff, sizeof(buff), "%.2f", reportRangeLow)) + string("-") +
                                                          string(CreateMessage( buff, sizeof(buff), "%.2f", reportRangeHigh));
                                    m_component->SendSubtestResultWithDetail(speedPtTargetParam + CreateMessage( buff, sizeof(buff), "%02d", intermediateSpeedPoint), 
                                                                             status, m_component->GetTestStepInfo("Description"), "0000",
                                                                             "AccelerationZone", accelMessage.c_str(), units,
                                                                             "ElapsedTime", CreateMessage( buff, sizeof(buff), "%.02f", (elapsedTime - prevElapsedTime)/1000.0), "seconds");
                                    if(intermediateSpeedPoint < numAccelPts)
                                    {
                                        //Proceed to next accel zone
                                        prevSpeedTarget = speedPtTarget;
                                        prevElapsedTime = elapsedTime;
                                        intermediateSpeedPoint++;
                                        speedPtTarget = m_component->GetParameterFloat(speedPtTargetParam +
                                                                                       CreateMessage( buff, sizeof(buff), "%02d", intermediateSpeedPoint));
                                        m_component->SetSecondaryStartTime();
                                    }
                                    else
                                        recordAccelPtsDone = true;
                                }
                            }
                        }while((speed < armSpeed) && m_component->TimeRemaining() && testStatus == BEP_STATUS_SUCCESS);

                        elapsedTime = m_component->GetElapsedTime();

                        if(speed >= armSpeed)      // if reached speed
                        {
                            SetBrakeTestingStatus(BEP_TESTING_RESPONSE);    // flag that the brakes can be tested
                            testStatus = BEP_STATUS_SUCCESS;                // indicate that the test passed
                        }
                        else if(!m_component->TimeRemaining()) // else if timed out
                            testStatus = BEP_STATUS_TIMEOUT;
                    }
                    else
                        m_component->Log(LOG_ERRORS, "Error, Could Not Set Up Display %d\n", testStatus);

                    // remove the speed target from the gauge
                    m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), string("0 0"));
                }
                else
                    m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTool::TestStepAccelerate %d\n", testStatus);
            }
            else
                testStatus = BEP_STATUS_SUCCESS;    // flag that the test passed
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepAccelerate Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        if (m_component->GetParameterBool("DisplaySpeedTimeGraph"))
        {
            //remove graph
            m_component->SystemWrite("DisplayDriverMonitor","drivermonitor"); 
            //clear start parameter
            m_component->SystemWrite(m_component->GetDataTag("StartSpeedGraph"), 0);
        }

        // update the test status
        m_component->UpdateResult(testStatus, status);
        char buff[ 256];
        m_component->SendTestResultWithDetail(status, m_component->GetTestStepInfo("Description"), "0000",
                                              "Armspeed", CreateMessage( buff, sizeof(buff), "%.2f", armSpeed), unitsMPH,
                                              "ElapsedTime", CreateMessage( buff, sizeof(buff), "%.02f", elapsedTime/1000.0), "seconds");
        if(m_component->GetParameterBool("CheckAccelerationElapseTime"))
        {
            string status = elapsedTime/1000.0 > m_component->GetParameterFloat("AccelerationTimeLimit") ? testFail : testPass;
            m_component->SendTestResultWithDetail(status, m_component->GetTestStepInfo("Description"), "0000",
                                                  "Armspeed", CreateMessage( buff, sizeof(buff), "%.2f", armSpeed), unitsMPH,
                                                  "ElapsedTime", CreateMessage( buff, sizeof(buff), "%.02f", elapsedTime/1000.0), "seconds",
                                                  "AccelerationTimeLimit", CreateMessage( buff, sizeof(buff), "%.02f", m_component->GetParameterFloat("AccelerationTimeLimit")), "seconds");

        }
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAccelerate(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepDrag (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepDrag()\n");
    // Check if test should be run
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {
                // make sure everything is groovy with the machine
                testStatus = StatusCheck();
                if(testStatus == BEP_STATUS_SUCCESS)
                {
                    // display driver prompts
                    if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                        m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                    // set up the display (speed gauge)
                    if(m_component->GetParameterBool("DisplayBrakeTargetSpeeds"))
                        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), m_component->GetParameter("BrakeTargetSpeedRange"));
                    // perform Drag
                    testStatus = Drag(m_dragStart, m_dragEnd);
                }
                else
                    m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTool::TestStepDrag %d\n", testStatus);
            }
            else
            {
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                         testStatus = BEP_STATUS_SKIP;
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepDrag Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // fail the brake testing so nothing else is executed
        if(testStatus != BEP_STATUS_SUCCESS)
            SetBrakeTestingStatus(BEP_FAILURE_RESPONSE);    // flag the brake test failed

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepDrag(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepAnalyzeDrag (void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAnalyzeDrag()\n");
    // Check if test should be run
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            // if data sampled correctly
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {
                // display driver prompts if desired
                if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                    m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                testStatus = AnalyzeDrags(m_dragStart, m_dragEnd);
            }
            else
            {   // if previous pass, mark passed else skip
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                    testStatus = BEP_STATUS_SKIP;
                m_component->Log(LOG_DEV_DATA, "Not Performing Drag Analysis due status: %s\n", GetBrakeTestingStatus().c_str());
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepAnalyzeDrag Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));

        if(status == BEP_PASS_RESPONSE)
        {
            m_component->SystemWrite(m_component->GetDataTag("TestResultBox1"), colorGreen);
        }
        else m_component->SystemWrite(m_component->GetDataTag("TestResultBox1"), colorRed);
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAnalyzeDrag(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepBaseBrake (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBaseBrake()\n");
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {
                // make sure everything is groovy with the machine
                testStatus = BrakeTestStatusCheck(false);
                m_component->Log(LOG_FN_ENTRY, "TestStepBaseBrake current status: %d\n", testStatus);
                if(testStatus == BEP_STATUS_SUCCESS)
                {   // set up the display (brake force gauge)
                    UpdateTarget();
                    // if gauge set up
                    if(testStatus == BEP_STATUS_SUCCESS)
                    {   // display driver prompts
                        if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                            m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                        // variables to hold the index values for analyzing
                        m_brakeStart = m_brakeEnd = 0;
                        testStatus = Brake(m_brakeStart, m_brakeEnd);
                        // Remove the prompts we displayed
                        m_component->RemovePrompts();
                    }
                    else
                        m_component->Log(LOG_ERRORS, "Error, TestStepBaseBrake Could Not Set Up Display %d\n", testStatus);
                    // Determine if the brake force meter should be disabled
                    if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))  UpdateTarget(false);
                }
                else
                    m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTool::TestStepBaseBrake %d\n", testStatus);
            }
            else
            {
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                    testStatus = BEP_STATUS_SUCCESS;
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepBaseBrake Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // set the brake gauge to inactive if we are not displaying meter until zero speed
        if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
            m_component->SystemWrite(m_component->GetDataTag("BrakeActive"), 0);

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBaseBrake(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

const string BaseBrakeTool::BrakeToThreshhold (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::BrakeToThreshhold()\n");
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {
                // make sure everything is groovy with the machine
                // Declutch
                int retries = 3;
                testStatus = Disengage();
                while( (BEP_STATUS_SUCCESS != testStatus) && (retries > 0))
                {
                    testStatus = Disengage();
                    retries--;
                }
                testStatus = BrakeTestStatusCheck(false);
                m_component->Log(LOG_FN_ENTRY, "TestStepBaseBrake current status: %d\n", testStatus);
                if(testStatus == BEP_STATUS_SUCCESS)
                {   // set up the display (brake force gauge)
                    UpdateTarget();
                    // if gauge set up
                    if(testStatus == BEP_STATUS_SUCCESS)
                    {   // display driver prompts
                        if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                            m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                        // variables to hold the index values for analyzing
                        testStatus = WaitForBrakeThreshhold();
                        // Remove the prompts we displayed
                        m_component->RemovePrompts();
                    }
                    else
                        m_component->Log(LOG_ERRORS, "Error, TestStepBaseBrake Could Not Set Up Display %d\n", testStatus);
                    // Determine if the brake force meter should be disabled
                    if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))  UpdateTarget(false);
                }
                else
                    m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTool::TestStepBaseBrake %d\n", testStatus);
            }
            else
            {
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                    testStatus = BEP_STATUS_SUCCESS;
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepBaseBrake Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // set the brake gauge to inactive if we are not displaying meter until zero speed
        if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
            m_component->SystemWrite(m_component->GetDataTag("BrakeActive"), 0);

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBaseBrake(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

const string BaseBrakeTool::TestStepAnalyzeBaseBrake (void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAnalyzeBaseBrake()\n");
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            // if data sampled correctly
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {
                // display driver prompts if desired
                if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                    m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                testStatus = AnalyzeBrakeForces(m_brakeStart, m_brakeEnd);
            }
            else
            {   // if previous pass, mark passed else skip
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                    testStatus = BEP_STATUS_SUCCESS;
                m_component->Log(LOG_DEV_DATA, "Not Performing Brake Analysis due status: %s\n", GetBrakeTestingStatus().c_str());
            }

        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepAnalyzeBaseBrake Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));

        if(status == BEP_PASS_RESPONSE)
        {
            m_component->SystemWrite(m_component->GetDataTag("TestResultBox2"), colorGreen);
        }
        else m_component->SystemWrite(m_component->GetDataTag("TestResultBox2"), colorRed);
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepAnalyzeBaseBrake(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepBalance (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBalance()\n");
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {
            if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {   // display driver prompts
                if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                    m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // perform the test
                testStatus = ValidateBalances();
            }
            else
            {
                if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                         testStatus = BEP_STATUS_SKIP;
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepBalance Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));

        if(status == BEP_PASS_RESPONSE)
        {
            m_component->SystemWrite(m_component->GetDataTag("TestResultBox3"), colorGreen);
        }
        else m_component->SystemWrite(m_component->GetDataTag("TestResultBox3"), colorRed);
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBalance(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepAccelerateToParkBrakeSpeed(void)
{
    string testStatus(testError);
    string armSpeed( m_component->GetParameter("ParkBrakeArmSpeed"));

    m_component->Log(LOG_FN_ENTRY, "Enter BaseBrakeTool::TestStepAccelerateToParkBrakeSpeed(%s)", armSpeed.c_str());
    // Disable the brake meter while we accelerate
    UpdateTarget( false);
    // Copy the dynamic park brake parameters to test step info
    m_component->SetTestStepInfoValue( "ArmSpeed", armSpeed);
    testStatus = TestStepAccelerate();
    m_component->Log(LOG_FN_ENTRY, "Exit BaseBrakeTool::TestStepAccelerateToParkBrakeSpeed(%s), status=%s", 
                     armSpeed.c_str(), testStatus.c_str());
    // Return the status
    return( testStatus);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepDynamicParkBrake(void)
{
    INT32   testStatus = BEP_STATUS_SUCCESS;        // the status of the test being performed
    int     retries = 3;
    char    buff[ 256];
    float   minForce, maxForce, target;
    string  testResult;

    m_component->Log( LOG_FN_ENTRY, "Enter BaseBrakeTool::TestStepDynamicParkBrake()\n");
    if(!m_component->ShortCircuitTestStep())
    {
        minForce = m_component->GetVehicleParameter("BrakeForces/ParkBrakeMinForce", float(9999.9));
        maxForce = m_component->GetVehicleParameter("BrakeForces/ParkBrakeMaxForce", float(0.0));
        target = (minForce + maxForce) / 2;

        m_component->SetTestStepInfoValue( "BrakeDomain", CreateMessage( buff, sizeof( buff), "0 %.02f", 2*target));
        m_component->SetTestStepInfoValue( "BrakeTarget", CreateMessage( buff, sizeof( buff), "%.02f %.02f", minForce, maxForce));

        try
        {
            // set up the display (brake force gauge)
            UpdateTarget();
            // if gauge set up
            if(testStatus == BEP_STATUS_SUCCESS)
            {   // display driver prompts
                m_component->UpdatePrompts();
                // variables to hold the index values for analyzing
                m_dynParkBrakeStart = m_dynParkBrakeStop = 0;
                // Declutch
                testStatus = Disengage();
                while( (BEP_STATUS_SUCCESS != testStatus) && (retries > 0))
                {
                    testStatus = Disengage();
                    retries--;
                }
                // If we are declutched
                if( BEP_STATUS_SUCCESS == testStatus)
                {
                    testStatus = DynamicParkBrake(m_dynParkBrakeStart, m_dynParkBrakeStop, "DynamicParkBrake");
                }
                else
                {
                    m_component->Log(LOG_ERRORS, "Could not declutch for DynamicParkBrake\n");
                }
                // Remove the prompts we displayed
                m_component->RemovePrompts();
                // Determine if the brake force meter should be disabled
                if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
                {
                    UpdateTarget(false);
                }
            }
            else
            {
                m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTool::TestStepDynamicParkBrake %d", 
                                 testStatus);
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepDynamicParkBrake Exception: %s", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }
        // set the brake gauge to inactive if we are not displaying meter until zero speed
        if(!m_component->GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
        {
            m_component->SystemWrite(m_component->GetDataTag("BrakeActive"), 0);
        }

        // update the test status
        m_component->UpdateResult(testStatus, testResult);
        m_component->SendTestResult(testResult, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_DEV_DATA, "Skipping test step - %s", m_component->GetTestStepName().c_str());
        testResult = testSkip;
    }
    m_component->Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::TestStepDynamicParkBrake(): %s, %d", 
                      testResult.c_str(), testStatus);
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const string &tagPrefix)
{
    INT32 testStatus = BEP_STATUS_NA;
    float wheelForce[4];
    // Log the entry and begin the test
    m_component->Log( LOG_FN_ENTRY, "Enter BaseBrakeTool::DynamicParkBrake()\n");
    try
    {   // update the the brake force until the speed is below the min sample speed
        float startSampleSpeed = fabs(m_component->GetParameterFloat("ParkBrakeStartSampleSpeed"));
        float endSampleSpeed = fabs(m_component->GetParameterFloat("ParkBrakeEndSampleSpeed"));
        bool  startSamplingAtMinForce = m_component->GetParameterBool("ParkBrakeStartSamplingAtMinimumForce");
        float minimumStartForce = m_component->GetParameterFloat("ParkBrakeMinimumStartForce");
        int   forceSampleCount = m_component->GetParameterInt("ParkBrakeForceSampleCount");
        int   validForceSamples;
        float speed=0;
        INT32 safeBrakeStart = 0;

        brakeStart = brakeEnd = 0;  // reset the brake array indexes
        validForceSamples = 0;  // No base brake samples taken yet
        // Insert a "safe" start tag in case the operator does not ever hit the brake pedal
        safeBrakeStart = m_component->TagArray( tagPrefix+"Start");
        do
        {
            speed = fabs(GetSpeed());

            // If we want base brake sampling to start/stop based on force/sample count
            if((0 < forceSampleCount) && (true == startSamplingAtMinForce))
            {   // check if minimum force for sampling is reached
                GetIForces(wheelForce);
                if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                {
                    validForceSamples++;
                    // If this is the first sample above min force
                    if(validForceSamples == 1)
                    {   // Tag the start
                        brakeStart = m_component->TagArray(tagPrefix+"StartForce");
                        m_component->Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d", brakeStart);
                        m_component->StartParkBrakeTimer();
                    }
                    // Wait for the desired number of samples above minimum before we end
                    else if(validForceSamples >= forceSampleCount)
                    {   // Tag the end
                        brakeEnd = m_component->TagArray(tagPrefix+"StopForce");
                        m_component->Log(LOG_DEV_DATA,"Base park Brake sample count reached @ %d", brakeEnd);
                    }
                }
            }
            else
            {   // If we want to "re-start" sampling base brake at minimum force value AND we haven't reached min force yet
                if((true == startSamplingAtMinForce) && (0 == validForceSamples))
                {   // check if minimum force for sampling is reached
                    GetIForces(wheelForce);
                    if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                    {   // Tag the start
                        brakeStart = m_component->TagArray(tagPrefix+"StartForce");
                        m_component->Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d", brakeStart);
                        validForceSamples = 1;
                    }
                }
                // If we have dropped below start speed and we have not tagged start yet
                if((speed <= startSampleSpeed) && (brakeStart == 0))
                {
                    brakeStart = m_component->TagArray(tagPrefix+"StartSpeed");
                    m_component->Log(LOG_DEV_DATA,"Start sample speed reached @ %d", brakeStart);
                }
                // If we have dropped below stop speed and we have not tagged stop yet
                else if((speed <= endSampleSpeed) && (brakeEnd == 0))
                {
                    brakeEnd = m_component->TagArray(tagPrefix+"EndSpeed");
                    m_component->Log(LOG_DEV_DATA,"End sample speed reached @ %d", brakeEnd);
                }
            }
            // only need to check every 1/10th of a second
            if(brakeEnd == 0)    BposSleep(100);
            // check the system status
            testStatus = BrakeTestStatusCheck(false);
        } while((m_component->TimeRemaining()) &&  (brakeEnd == 0) && (testStatus == BEP_STATUS_SUCCESS));

        // Make sure we have a valid brake start tag
        if( brakeStart == 0)    brakeStart = safeBrakeStart;
        // if brakeEnd has not been set yet, set to brakeStart
        if(brakeEnd == 0) brakeEnd = brakeStart;
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "DynamicParkBrake Exception: %s", e.what());
        testStatus = BEP_STATUS_FAILURE;
    }
    // Log the exit and return the status
    m_component->Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::DynamicParkBrake( %d, %d), status=%d", 
                      brakeStart, brakeEnd, testStatus);
    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::AnalyzeDynamicParkBrake(void)
{
    INT32 status = BEP_STATUS_FAILURE;      // the status of the test being performed
    string testResult;
    // Log the entry and determine if this step should be performed
    m_component->Log( LOG_FN_ENTRY, "Enter BaseBrakeTool::AnalyzeDynamicParkBrake()");
    if(!m_component->ShortCircuitTestStep())
    {
        try
        {   // if data sampled correctly
            // display driver prompts if desired
            m_component->UpdatePrompts();

            status = AnalyzeParkBrakeForces(m_dynParkBrakeStart, m_dynParkBrakeStop);
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepAnalyzeDynamicParkBrake Exception: %s", e.what());
            status = BEP_STATUS_SOFTWARE;
        }
        m_component->RemovePrompts();
        // update the test status
        m_component->UpdateResult(status, testResult);
        m_component->SendTestResult(testResult, m_component->GetTestStepInfo("Description"));
        m_component->SystemWrite(m_component->GetDataTag("TestResultBox2"), 
                                 !testResult.compare(testPass) ? colorGreen : colorRed);
    }
    else
    {   // Skip the test
        m_component->Log(LOG_DEV_DATA, "Skipping test step - %s", m_component->GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the reuslt
    m_component->Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::AnalyzeDynamicParkBrake(): %s, %d", 
                      testResult.c_str(), status);
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd)
{
    INT32           testStatus;
    INT32           wheelStatus;
    WHEELDATAARRAY  wheelForceArray;
    float           force;        // the current brake force
    char            buffer[256];    // data array for printing
    string          faultCode;  // Code to report to external systems for a failure condition
    string          faultDesc;  // Description of what the test failed for
    int             roller;
    // Log the entry
    m_component->Log( LOG_FN_ENTRY, "Enter BaseBrakeTool::AnalyzeParkBrakeForces(%d,%d)", brakeStart, brakeEnd);
    // Clear out previous drag force results
    m_parkBrakeForce.clear();
    // read in the data from the brake force array
    testStatus = m_component->ReadDataArrays(m_component->GetParameter("IcmForceArray"), brakeStart, brakeEnd, 
                                             wheelForceArray);
    if(BEP_STATUS_SUCCESS == testStatus)
    {   // the parking brake only uses the rear two rolls
        for(roller = LRWHEEL; roller <= RRWHEEL; roller++)
        {   // calculate the average forces and validate the results
            const DATAARRAY &forceArray = wheelForceArray[roller];
            if( forceArray.size() > 0)
            {
                force = 0;
                DATAARRAY::const_iterator   itr;
                for( itr=forceArray.begin(); itr!=forceArray.end(); itr++)
                {
                    force += *itr;
                }
                force /= forceArray.size();
				force = fabs(force);
                m_component->Log(LOG_DEV_DATA, "BaseBrakeTool::AnalyzeParkBrakeForces() - step1: Force - %.2f", force);
                wheelStatus = ValidateParkBrakeForce(roller, force);
            }
            else    // if there is a problem averaging the forces display -1
            {
                m_component->Log( LOG_ERRORS, "Force array for roller %d has zero length", roller);
                // write the value to -1
                force = -1;
                // update the result and the background color of the result on the GUI
                m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), "-1");
                m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), "Red");
            }
            // place the normalized value into the brake force array
            m_parkBrakeForce.push_back(force);

            m_component->Log(LOG_DEV_DATA, "BaseBRakeTool::AnalyzeParkBrakeForces() - step2");
            // Set the fault codes and descriptions
            const string faultTag(rollerName[roller]+"ParkBrakeForceFault");
            const string testResultTag(rollerName[roller]+"ParkBrakeForceTest");
            if( BEP_STATUS_SUCCESS == wheelStatus)
            {
                faultCode = "0000";
                faultDesc = rollerName[roller] + " Park Brake Force Test Result";
            }
            else
            {
                faultCode = m_component->GetFaultCode(faultTag);
                faultDesc = m_component->GetFaultDescription(faultTag);
            }
            // send the test results to the TestResultServer
            memset(buffer, 0 , sizeof(buffer));       // clear the data array
            m_component->SendSubtestResultWithDetail(testResultTag, wheelStatus, faultDesc, faultCode,
                                                     "ParkBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", force), unitsLBF,
                                                     "MaxParkBrakeForce", m_component->GetParameter("ParkBrakeMaxForce"), unitsLBF,
                                                     "MinParkBrakeForce", m_component->GetParameter("ParkBrakeMinForce"), unitsLBF);

            if (wheelStatus != BEP_STATUS_SUCCESS)
            {
                testStatus = wheelStatus;
            }
        }
    }
    else
    {
        m_component->Log( LOG_ERRORS, "Error reading force array: %d", testStatus);
    }
    // Log the exit and return the result
    m_component->Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::AnalyzeParkBrakeForces(%d,%d), status=%d", 
                      brakeStart, brakeEnd, testStatus);
    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateParkBrakeForce(INT32 roller, float average)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");
    m_component->Log( LOG_FN_ENTRY, "Enter BaseBrakeTool::ValidateParkBrakeForce(%d, %.2f)", roller, average);

    // read the parameters to compare the average too
    float minForce = m_component->GetVehicleParameter("BrakeForces/ParkBrakeMinForce", float(0.0));
    float maxForce = m_component->GetVehicleParameter("BrakeForces/ParkBrakeMaxForce", float(0.0));

    m_component->Log( LOG_DEV_DATA, "ValidateParkBrakeForce(%d, %f), min: %f, max: %f", roller, average, minForce, maxForce);

    // if brake force is in the valid limits set ok
    if( ((average <= maxForce) || (maxForce <= 0)) && (average >= minForce))
    {
        testStatus = BEP_STATUS_SUCCESS;
        color = "Green";
    }

    // update the result and the background color of the result on the GUI
    m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), average);
    m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), color);

    m_component->Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::ValidateParkBrakeForce(%d, %.2f), status=%d", 
                      roller, average, testStatus);
    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
const std::string BaseBrakeTool::TestStepBrakeToStop(void)
{
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepBrakeToStop()\n");
    //std::string status = BEP_NONE;    // set test step status to no response
    INT32 testStatus = BEP_STATUS_FAILURE;      // the status of the test being performed
    string status;

    if(!m_component->ShortCircuitTestStep())
    {
        try
        {   // Just look for good system status
            testStatus = m_component->StatusCheck();
            m_component->Log(LOG_DEV_DATA, "TestStepBrakeToStop current status: %d\n", testStatus);
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                bool done = false;  // status of the braking
                // check if at zerospeed
                if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1")
                {
                    // if not, put up the question and wait
                    if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                        m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                    while((m_component->StatusCheck() == BEP_STATUS_SUCCESS) && m_component->TimeRemaining() && !done)
                    {
                        if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1")
                            BposSleep(250);
                        else
                            done = true;
                    }
                }
                // else flag done
                else
                    done = true;

                // update the status of the test
                // NOTE: Do not use our StatusCheck() method since time lags in
                //       the machine resetting can cause false failures.
                if(m_component->StatusCheck() != BEP_STATUS_SUCCESS)
                    m_component->UpdateResult(m_component->StatusCheck(), testStatus);
                else if( (!m_component->TimeRemaining())||(!done) )
                    testStatus = BEP_STATUS_TIMEOUT;
                else
                    testStatus = BEP_STATUS_SUCCESS;

                // Ensure that the motor mode is Boost mode
                m_component->SystemWrite(MOTOR_MODE, string(BOOST_MODE));
            }
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "BaseBrakeTool::TestStepBrakeToStop Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }
        // update the test status
        m_component->UpdateResult(testStatus, status);
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
        m_component->RemovePrompts();   // remove the prompts from the screen
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    return(status);     // return the status
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepFinishUp (void)
{
    string tag, value, response;            // communication strings
    INT32 testStatus = BEP_STATUS_NA;       // status of the test
    string status;

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepFinishUp()\n");

    if(!m_component->ShortCircuitTestStep())
    {
        // this always runs
        try
        {
            // display finishup prompt
            if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // make sure the vhicle is at zerospeed
            while(m_component->TimeRemaining() && (m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1"))
                BposSleep (100);                // wait a tenth of a second
            // once stopped, enable the system again
            testStatus = Engage();

            // if a timeout did not occur
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                if(m_component->TimeRemaining())
                    testStatus = BEP_STATUS_SUCCESS;
                else
                    testStatus = BEP_STATUS_FAILURE;
            }
            else
                testStatus = BEP_STATUS_FAILURE;
        }
        catch(BepException &e)
        {
            m_component->Log(LOG_ERRORS, "TestStepFinishUp Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        m_component->UpdateResult(testStatus, status);
        // write the test step information
        m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));
    }
    else
    {   // Skip the test
        m_component->Log(LOG_FN_ENTRY, "Skipping test step - %s\n", m_component->GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }
    // send the overall test result - always
    // NOTE:  If this is not sent, the third retry after an initial pass will turn the status box blue
    //        indicating the
    SendOverallResult(status);
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepFinishUp(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
const string BaseBrakeTool::TestStepCoast(void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepCoast()\n");
    try
    {
        if(GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
        {
            // make sure everything is groovy with the machine
            testStatus = StatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                // set up the display (speed gauge)
                testStatus = m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), m_component->GetTestStepInfo("SpeedTarget"));
                // display driver prompts
                if(m_component->UpdatePrompts() != BEP_STATUS_SUCCESS)
                    m_component->Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // perform Drag
                testStatus = Coast();
                // remove the speed target from the gauge
                m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), string("0 0"));
            }
            else
                m_component->Log(LOG_ERRORS, "Invalid Machine Status For BaseBrakeTC::TestStepCoast %d\n", testStatus);
        }
        else
        {
            if(GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
            else                                    testStatus = BEP_STATUS_SKIP;
        }
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "TestStepCoast Exception: %s\n", e.what());
        testStatus = BEP_STATUS_SOFTWARE;
    }

    // fail the brake testing so nothing else is executed
    if(testStatus != BEP_STATUS_SUCCESS)
        SetBrakeTestingStatus(BEP_FAILURE_RESPONSE);    // flag the brake test failed

    // update the test status
    string status;
    m_component->UpdateResult(testStatus, status);
    m_component->SendTestResult(status, m_component->GetTestStepInfo("Description"));

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::TestStepCoast(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

//-----------------------------------------------------------------------------
string BaseBrakeTool::AnalyzeStoppingDistance(void)
{   // Log the entry and determine if this test should be performed
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeStoppingDistance() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!m_component->ShortCircuitTestStep())
    {
        if((BEP_TESTING_RESPONSE == GetBrakeTestingStatus()) && (BEP_STATUS_SUCCESS == m_component->StatusCheck()))
        {   // Get the current distances
            m_component->GetWheelDistances(m_brakeStoppingDistance[1]);
            // Calculate the stopping distance - the initial distance reading comes from when we tag the force 
            // array with the start of the brake test.
            WHEELINFO stoppingDistance;
            m_component->GetTotalDistances(stoppingDistance, m_brakeStoppingDistance[0], m_brakeStoppingDistance[1]);
            // Convert the pulse counts into a physical distance - change to feet since this should be short  5280 feet/mile
            if (m_component->GetParameterFloat("ConversionFactorFromMiles") == 0)
            {
                stoppingDistance.lfWheel = m_component->ConvertPulsesToMiles(stoppingDistance.lfWheel) * 5280.0;
                stoppingDistance.rfWheel = m_component->ConvertPulsesToMiles(stoppingDistance.rfWheel) * 5280.0;
                stoppingDistance.lrWheel = m_component->ConvertPulsesToMiles(stoppingDistance.lrWheel) * 5280.0;
                stoppingDistance.rrWheel = m_component->ConvertPulsesToMiles(stoppingDistance.rrWheel) * 5280.0;
            }
            else
            {
                stoppingDistance.lfWheel = m_component->ConvertPulsesToMiles(stoppingDistance.lfWheel) * m_component->GetParameterFloat("ConversionFactorFromMiles");
                stoppingDistance.rfWheel = m_component->ConvertPulsesToMiles(stoppingDistance.rfWheel) * m_component->GetParameterFloat("ConversionFactorFromMiles");
                stoppingDistance.lrWheel = m_component->ConvertPulsesToMiles(stoppingDistance.lrWheel) * m_component->GetParameterFloat("ConversionFactorFromMiles");
                stoppingDistance.rrWheel = m_component->ConvertPulsesToMiles(stoppingDistance.rrWheel) * m_component->GetParameterFloat("ConversionFactorFromMiles");
            }
            // Make sure each wheel is within the correct stopping distance
            float frontMaxDistance = m_component->GetVehicleParameter("FrontMaximumStoppingDistance", float(0.0));
            float frontMinDistance = m_component->GetVehicleParameter("FrontMinimumStoppingDistance", float(0.0));
            float rearMaxDistance = m_component->GetVehicleParameter("RearMaximumStoppingDistance", float(0.0));
            float rearMinDistance = m_component->GetVehicleParameter("RearMinimumStoppingDistance", float(0.0));
            string lfResult = ((frontMinDistance < stoppingDistance.lfWheel) && 
                               (stoppingDistance.lfWheel < frontMaxDistance)) ? testPass : testFail;
            string rfResult = ((frontMinDistance < stoppingDistance.rfWheel) && 
                               (stoppingDistance.rfWheel < frontMaxDistance)) ? testPass : testFail;
            string lrResult;
            string rrResult;
            if(GetNumberOfWheelsToAnalyze() > 2)
            {
                lrResult = ((rearMinDistance < stoppingDistance.lrWheel) && 
                            (stoppingDistance.lrWheel < rearMaxDistance)) ? testPass : testFail;
                rrResult = ((rearMinDistance < stoppingDistance.rrWheel) && 
                            (stoppingDistance.rrWheel < rearMaxDistance)) ? testPass : testFail;
            }
            else
            {
                lrResult = testSkip;
                rrResult = testSkip;
            }
            m_component->Log(LOG_DEV_DATA, "Stopping distance results:\n\tLF: %.2f [%.2f %.2f]\t\tRF: %.2f [%.2f %.2f]\n"
                             "\tLR: %.2f [%.2f %.2f]\t\tRR: %.2f [%.2f %.2f]",
                             stoppingDistance.lfWheel, frontMinDistance, frontMaxDistance, 
                             stoppingDistance.rfWheel, frontMinDistance, frontMaxDistance,
                             stoppingDistance.lrWheel, rearMinDistance, rearMaxDistance, 
                             stoppingDistance.rrWheel, rearMinDistance, rearMaxDistance);
            // Determine the overall result
            testResult = (!lfResult.compare(testPass) && !rfResult.compare(testPass) && 
                          ((!lrResult.compare(testPass) && !rrResult.compare(testPass)) ||
                           (GetNumberOfWheelsToAnalyze() == 2))) ? testPass : testFail;
            m_component->Log(LOG_DEV_DATA, "Overall: %s, LF: %s    RF: %s    LR: %s    RR: %s",
                             testResult.c_str(), lfResult.c_str(), rfResult.c_str(), lrResult.c_str(), rrResult.c_str());
            // Report the results
            SendBrakeStoppingDistanceResult(m_component->GetTestStepName(), 
                                            testResult, m_component->GetTestStepInfo("Description"), "0000",
                                            stoppingDistance, lfResult, rfResult, lrResult, rrResult);
            // Publish the values so they can be displayed on the screen
            m_component->SystemWrite(m_component->GetDataTag("LFBrakeDistanceTag"), stoppingDistance.lfWheel);
            m_component->SystemWrite(m_component->GetDataTag("LFBrakeDistanceColorTag"), 
                                     !lfResult.compare(testPass) ? string("green") : string("red"));
            m_component->SystemWrite(m_component->GetDataTag("RFBrakeDistanceTag"), stoppingDistance.rfWheel);
            m_component->SystemWrite(m_component->GetDataTag("RFBrakeDistanceColorTag"), 
                                     !rfResult.compare(testPass) ? string("green") : string("red"));
            m_component->SystemWrite(m_component->GetDataTag("LRBrakeDistanceTag"), stoppingDistance.lrWheel);
            m_component->SystemWrite(m_component->GetDataTag("LRBrakeDistanceColorTag"), 
                                     !lrResult.compare(testPass) ? string("green") : string("red"));
            m_component->SystemWrite(m_component->GetDataTag("RRBrakeDistanceTag"), stoppingDistance.rrWheel);
            m_component->SystemWrite(m_component->GetDataTag("RRBrakeDistanceColorTag"), 
                                     !rrResult.compare(testPass) ? string("green") : string("red"));
        }
        else
        {   // Something is not right, do not calculate the distance
            m_component->Log(LOG_ERRORS, "Not analyzing stopping distance - Testing Status: %s, StatusCheck: %s",
                             GetBrakeTestingStatus().c_str(), ConvertStatusToResponse(m_component->StatusCheck()).c_str());
            testResult = testFail;

        }
    }
    else
    {
        m_component->Log(LOG_FN_ENTRY, "Skipping test step %s", m_component->GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeStoppingDistance() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
INT32 BaseBrakeTool::Coast(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;

    float speed=0;          // speed read from the machine
    bool done = false;      // flag to indicate if done

    // Get the coast speed
    float coastSpeed = m_component->GetTestStepInfoFloat("CoastEndSpeed");

    // Tag the array so we can see that a coas occured
    m_component->TagArray("CoastStart");
    m_component->SystemWrite(m_component->GetDataTag("NeutralLamp"),true);

    do
    {   // get the wheel with the slowest speed
        speed = GetSpeed();
        // else check the status of the component and delay
        if(speed < coastSpeed)  done = true;
        else
        {
            testStatus = m_component->StatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)    BposSleep(100);
        }
    }while( (!done) && (testStatus == BEP_STATUS_SUCCESS) && m_component->TimeRemaining());

    m_component->SystemWrite(m_component->GetDataTag("NeutralLamp"),false);
    // if for some reason, the speed never came down before the system had an error or a
    if(done)                    testStatus = BEP_STATUS_SUCCESS;
    else if(!m_component->TimeRemaining())   testStatus = BEP_STATUS_TIMEOUT;
    else                        testStatus = BEP_STATUS_FAILURE;

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::Drag(INT32 &dragStart, INT32 &dragEnd)
{
    INT32 testStatus = BEP_STATUS_FAILURE;

    // Wait for drag speed to declutch
    float dragSpeed = m_component->GetTestStepInfoFloat("DragSpeed");
    float speed=0;          // speed read from the machine
    int retries = 3;        // number of retries attempted

    int icmFd;
    if( -1 != (icmFd = open( "/dev/Icm/Block/Drag", O_RDWR)) )
    {
        const UINT32 rollCount = m_component->GetRollerCount();
        float drags[ rollCount];
        memset( drags, 0, sizeof( drags));
        write( icmFd, drags, sizeof( drags));
        close( icmFd);
    }
    else
    {
        m_component->Log( LOG_ERRORS, "Error clearing drag forces to the ICM: %s\n", strerror( errno));
    }

    do
    {   // get the wheel with the slowest speed
        speed = GetSpeed();
        m_component->Log( LOG_DEV_DATA, "BaseBrakeTool::Drag - speed: %.2f  dragSpeed: %.2f", speed, dragSpeed);

        // if at or below the drag speed, disengage the drive
        if(speed <= dragSpeed)
        {
            m_component->Log( LOG_DEV_DATA, "BaseBrakeTool::Drag - will try to Disengage");
            testStatus = Disengage();
            // if not successful, retry
            if(testStatus != BEP_STATUS_SUCCESS)    retries--;
            // else we are done
            else                                    retries = 0;
        }
        // else check the status of the component and delay
        else
        {
            testStatus = m_component->StatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)    BposSleep(100);
        }
    }while( (testStatus == BEP_STATUS_SUCCESS) && retries && m_component->TimeRemaining());

    // if successfully declutched, perform the drag sampling
    if(testStatus == BEP_STATUS_SUCCESS)
    {   // delay some drag delay time for the declutching (config file)
        BposSleep(m_component->GetTestStepInfoInt("DragDelay"));
        // tag the array for drag start
        dragStart = m_component->TagArray("DragStart");
        // get the number of samples to wait for measuring the drag
        INT32 dragSamples = m_component->GetTestStepInfoInt("DragSamples");
        // set the drag End based on the number of samples to take
        dragEnd = dragStart + dragSamples;
        // ensure that all of the samples are taken (samples * 20mS)
        BposSleep(dragSamples * 20);
        // if the test timed out
        if(!m_component->TimeRemaining())   testStatus =  BEP_STATUS_TIMEOUT;
    }
    else
        m_component->Log(LOG_ERRORS, "Drag Test Unable to disengage the rollers: %d\n", testStatus);

    return(testStatus);
}


//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::WaitForBrakeThreshhold()
{
    INT32 testStatus = BEP_STATUS_NA;
    float wheelForce[4];
    bool threshholdReached = false;

    try
    {   // update the the brake force until the speed is below the min sample speed
        float minimumStartForce = m_component->GetTestStepInfoFloat("MinimumThreshholdForce");
        do
        {
            // check if minimum force for sampling is reached
            GetIForces(wheelForce);
            if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
            {
                threshholdReached = true;
            }

            // only need to check every 1/10th of a second
            if(!threshholdReached) BposSleep(100);

            // check the system status
            testStatus = BrakeTestStatusCheck(false);

        } while((m_component->TimeRemaining()) &&  
                (m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) != "1") && 
                (testStatus == BEP_STATUS_SUCCESS)
                && !threshholdReached);
        m_component->Log(LOG_DEV_DATA, "WaitForBrakeThreshhold reached ? %s\n", threshholdReached  ? "True" : "False");
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Brake Exception: %s\n", e.what());
        testStatus = BEP_STATUS_FAILURE;
    }
    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::Brake(INT32 &brakeStart, INT32 &brakeEnd)
{
    INT32 testStatus = BEP_STATUS_NA;
    float wheelForce[m_component->GetRollerCount()];

    try
    {   // update the the brake force until the speed is below the min sample speed
        float startSampleSpeed = m_component->GetTestStepInfoFloat("StartBrakeSampleSpeed");
        float endSampleSpeed = m_component->GetTestStepInfoFloat("EndBrakeSampleSpeed");
        bool  startSamplingAtMinForce = m_component->GetParameterBool("StartSamplingAtMinimumForce");
        float minimumStartForce = m_component->GetParameterFloat("MinimumStartForce");
        int   baseBrakeSampleCount = m_component->GetParameterInt("BaseBrakeSampleCount");
        int   validBaseBrakeSamples;
        float speed=0;
        INT32 safeBrakeStart = 0;

        // set up the display (speed gauge)
        if(m_component->GetParameterBool("DisplayBrakeTargetSpeeds"))
            m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), m_component->GetParameter("BrakeTargetSpeedRange"));

        brakeStart = brakeEnd = 0;  // reset the brake array indexes
        validBaseBrakeSamples = 0;  // No base brake samples taken yet
        // Insert a "safe" start tag in case the operator does not ever hit the brake pedal
        safeBrakeStart = m_component->TagArray( "SafeBrakeStart");
        bool stopDistanceStarted = false;
        do
        {
            speed = GetSpeed();             // get the wheel with slowest speed
            // Ideally this is where we will start, but just in case we get enough smaples before this, we will also
            // Set the starting distance when we start braking
            if((speed < m_component->GetParameterFloat("BrakeStopdistanceStartSpeed")) && !stopDistanceStarted)
            {   // Get the current distances and set the started flag to true
                m_component->GetWheelDistances(m_brakeStoppingDistance[0]);
                stopDistanceStarted = true;
            }

            // If we want base brake sampling to start/stop based on force/sample count
            if((0 < baseBrakeSampleCount) && (true == startSamplingAtMinForce))
            {
                // check if minimum force for sampling is reached
                GetIForces(wheelForce);
                if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                {
                    validBaseBrakeSamples++;
                    // If this is the first sample above min force
                    if(validBaseBrakeSamples == 1)
                    {   // Tag the start
                        brakeStart = m_component->TagArray("BaseBrakeStartForce");
                        m_component->Log(LOG_DEV_DATA,"Minimum brake force reached @ %d\n", brakeStart);
                        m_component->StartBrakeTimer();
                        m_component->GetWheelDistances(m_brakeStoppingDistance[0]);

                        // store the brake pedal force if specified in the configuration file
                        ProcessBrakePedalForce();
                    }
                    // Wait for the desired number of samples above minimum before we end
                    else if(validBaseBrakeSamples >= baseBrakeSampleCount)
                    {   // Tag the end
                        brakeEnd = m_component->TagArray("BaseBrakeStopForce");
                        m_component->Log(LOG_DEV_DATA,"Base Brake sample count reached @ %d\n", brakeEnd);
                    }
                }
            }
            else
            {   // If we want to "re-start" sampling base brake at minimum force value AND we haven't reached min force yet
                if((true == startSamplingAtMinForce) && (0 == validBaseBrakeSamples))
                {
                    // check if minimum force for sampling is reached
                    GetIForces(wheelForce);
                    if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                    {   // Tag the start
                        brakeStart = m_component->TagArray("BaseBrakeStartForce");
                        m_component->Log(LOG_DEV_DATA,"Minimum brake force reached @ %d\n", brakeStart);
                        validBaseBrakeSamples = 1;
                        m_component->GetWheelDistances(m_brakeStoppingDistance[0]);

                        // store the brake pedal force if specified in the configuration file
                        ProcessBrakePedalForce();
                    }
                }

                // If we have dropped below start speed and we have not tagged start yet
                if((speed <= startSampleSpeed) && (brakeStart == 0))
                {
                    brakeStart = m_component->TagArray("BaseBrakeStartSpeed");
                    m_component->Log(LOG_DEV_DATA,"Start sample speed reached @ %d\n", brakeStart);
                    m_component->GetWheelDistances(m_brakeStoppingDistance[0]);
                }
                // If we have dropped below stop speed and we have not tagged stop yet
                else if((speed <= endSampleSpeed) && (brakeEnd == 0))
                {
                    brakeEnd = m_component->TagArray("BaseBrakeEndSpeed");
                    m_component->Log(LOG_DEV_DATA,"End sample speed reached @ %d\n", brakeEnd);

                    // clear the display (speed gauge)
                    if(m_component->GetParameterBool("DisplayBrakeTargetSpeeds"))
                        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), string("0 0"));
                }
            }

            // only need to check every 1/10th of a second
            if(brakeEnd == 0)    BposSleep(100);

            // check the system status
            testStatus = BrakeTestStatusCheck(false);

        } while((m_component->TimeRemaining()) &&  (brakeEnd == 0) && (testStatus == BEP_STATUS_SUCCESS));

        // Make sure we have a valid brake start tag
        if( brakeStart == 0)    brakeStart = safeBrakeStart;

        // if brakeEnd has not been set yet, set to brakeStart
        if(brakeEnd == 0) brakeEnd = brakeStart;

        m_component->Log(LOG_DEV_DATA, "Brake( %d, %d)\n", brakeStart, brakeEnd);
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Brake Exception: %s\n", e.what());
        testStatus = BEP_STATUS_FAILURE;
    }

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
string BaseBrakeTool::ExcessiveBrakeForceCheck(void)
{
    string  testResult = BEP_TESTING_STATUS;
    string  testResultCode = "0000";
    string  testDescription = m_component->GetTestStepInfo("Description");
    float   brakeForceLimit = m_component->GetParameterFloat("ExcessiveBrakeForceLimit");
    INT32   wheelIndex, dataPoint, totalSamples, badCount=0;
    INT32   consecSamples = m_component->GetParameterInt("NumberOfSamplesAboveLimit");

    // Determine if this step should be performed
    m_component->Log(LOG_FN_ENTRY, "%s::%s - Enter\n", m_component->GetComponentName().c_str(), m_component->GetTestStepName().c_str());
    if((brakeForceLimit != 0.0) && !m_component->ShortCircuitTestStep())
    {   // Tag the end of testing
        INT32 brakeTestEnd = m_component->TagArray("BrakeTestEnd");
        INT32 brakeStart = GetBrakeStart();

        // Load the brake force data
        BEP_STATUS_TYPE readDataStatus = (BEP_STATUS_TYPE)ReadDataForWheelForceArray(m_component->GetParameter("IcmForceArray"),
                                                                                     brakeStart,
                                                                                     brakeTestEnd);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == readDataStatus)
        {   // How many smaple do we have for analysis
            totalSamples = brakeTestEnd - brakeStart + 1;
            // Check each wheel for excesive brake force
            for(wheelIndex = 0; ((UINT16)wheelIndex < m_component->GetWheelCount()) && (BEP_TESTING_STATUS == testResult); wheelIndex++)
            {
                // Reset the "bad" sample counter because we are starting a new wheel
                badCount = 0;

                // Check all force values in the array until we determine if excessive brake force was used.
                for(dataPoint = 0; (dataPoint < totalSamples) && (BEP_TESTING_STATUS == testResult); dataPoint++)
                {   // Check for n consecutive samples above limit
                    // NOTE: We will assume the current data point is over the limit.  As soon as we see
                    //       one sample below the limit, we will exit the loop and start checking the next point.
                    bool excessiveBrakeForce = false;
                    // If this force sample is above the limit
                    if(GetWheelForceValue(wheelIndex, dataPoint) > brakeForceLimit)
                    {   // Increment the "bad" counter
                        badCount++;
                        if( badCount > consecSamples)
                        {
                            excessiveBrakeForce = true;
                        }
                    }
                    else
                    {   // Reset "bad" counter to 0
                        badCount = 0;
                    }
                    // Update the test result to determine if we need to keep checking
                    testResult = excessiveBrakeForce ? testFail : BEP_TESTING_STATUS;
                }
                m_component->Log(LOG_DEV_DATA, "%s excessive brake force check: %s\n", rollerName[wheelIndex].c_str(),
                                 testFail == testResult ? "Fail" : "Pass");
            }
            // Determine the result of the test
            testResult = ( (testFail == testResult) ? testResult : testPass);
            testResultCode = ( (testPass == testResult) ? "0000" : m_component->GetFaultCode("ExcessiveBrakeForce"));
            testDescription = ( (testPass == testResult) ? testDescription : m_component->GetFaultDescription("ExcessiveBrakeForce"));
            m_component->Log(LOG_DEV_DATA, "Excessive Brake Force: %s\n", testResult.c_str());
        }
        else
        {   // Error reading brake force data
            m_component->Log(LOG_ERRORS, "Error reading brake force data array - status: %s\n", ConvertStatusToResponse(readDataStatus).c_str());
            testResult = testFail;
            testResultCode = m_component->GetFaultCode("WheelForceDataReadError");
            testDescription = m_component->GetFaultDescription("WheelForceDataReadError");
        }
        // Report the results
        m_component->SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Skip the test step
        m_component->Log(LOG_FN_ENTRY, "Skipping %s -- ExcessiveBrakeForceLimit: %s\n",
                         m_component->GetTestStepName().c_str(), m_component->GetParameter("ExcessiveBrakeForceLimit").c_str());
        testResult = testSkip;
    }
    // Log the function exit and return the result
    m_component->Log(LOG_FN_ENTRY, "%s::%s - Exit, result: %s\n",
                     m_component->GetComponentName().c_str(), m_component->GetTestStepName().c_str(), testResult.c_str());
    return !m_component->GetParameterBool("FailForExcessiveBrakeForce") ? testResult : testPass;
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::StatusCheck(void)
{
    m_component->Log(LOG_DEV_DATA, "Entering BaseBrakeTool Status Check\n");

    INT32 status = m_component->StatusCheck();

    if((status == BEP_STATUS_SUCCESS) && !m_component->GetParameterBool("IgnoreMachineConditions"))
    {
        if(/*(m_component->ReadSubscribeData(m_component->GetDataTag("VehiclePresent")) == "1") && */      /* vehicle present */
           (m_component->ReadSubscribeData(MACHINE_CLUTCH) == "1") &&                   /* rollers clutched */
           (m_component->ReadSubscribeData(MACHINE_DECLUTCH) == "0") &&                 /* and not declutched */
           (m_component->ReadSubscribeData(string(MOTOR_MODE)) == string(BOOST_MODE)) &&                                   /* and in boost mode */
           (m_component->ReadSubscribeData(m_component->GetDataTag("RetainersUp")) == "1"))    /* retainers raised or elevators lowered */
        {
            status = BEP_STATUS_SUCCESS;
        }
        else
            status = BEP_STATUS_ABORT;
        // Log the status for the user
        m_component->Log(LOG_DEV_DATA, "BaseBrakeTool Status Check: %s\n\tVehicle Present: %s\n\tClutch: %s"
                         "\n\tDeclutch: %s\n\tMotor Mode: %s\n\tRetainers Up: %s\n",
                         ConvertStatusToResponse(status).c_str(),
                         m_component->ReadSubscribeData(m_component->GetDataTag("VehiclePresent")).c_str(),
                         m_component->ReadSubscribeData(string(MACHINE_CLUTCH)).c_str(),
                         m_component->ReadSubscribeData(string(MACHINE_DECLUTCH)).c_str(),
                         m_component->ReadSubscribeData(string(MOTOR_MODE)).c_str(),
                         m_component->ReadSubscribeData(m_component->GetDataTag("RetainersUp")).c_str());
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::BrakeTestStatusCheck(bool checkTestStatus) /* = true */
{
    INT32 status = BEP_STATUS_SUCCESS;

    // if we need to check the overall test status
    if(checkTestStatus) status = m_component->StatusCheck();

    if(status == BEP_STATUS_SUCCESS)
    {
        if(/*(m_component->ReadSubscribeData(m_component->GetDataTag("VehiclePresent")) == "1") && */  /* vehicle present      */
           (m_component->ReadSubscribeData(MACHINE_CLUTCH) == "0") &&           /* rollers not clutched */
           (m_component->ReadSubscribeData(MACHINE_DECLUTCH) == "1") &&     /* and are declutched   */
           (m_component->ReadSubscribeData(m_component->GetDataTag("RetainersUp")) == "1"))        /* retainers raised or elevators lowered */
        {
            status = BEP_STATUS_SUCCESS;
        }
        else
            status = BEP_STATUS_FAILURE;

        m_component->Log(LOG_DEV_DATA, "Base Brake Status Check: %d (%s, %s, %s, %s)\n", status,
                         m_component->ReadSubscribeData(m_component->GetDataTag("VehiclePresent")).c_str(),
                         m_component->ReadSubscribeData(MACHINE_CLUTCH).c_str(),
                         m_component->ReadSubscribeData(MACHINE_DECLUTCH).c_str(),
                         m_component->ReadSubscribeData(m_component->GetDataTag("RetainersUp")).c_str());
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::AnalyzeDrags(INT32 dragStart, INT32 dragEnd)
{
    INT32 status = BEP_STATUS_SUCCESS;
    // read in the data from the brake force array
    INT32 testStatus = m_component->ReadDataArrays(m_component->GetParameter("IcmForceArray"), dragStart, dragEnd, m_wheelForceArray);

    // iterate throught the wheels and evaluate
    float average;          // the force from the array that was analyzed
    std::string color;      // the status of the result "Red" / "Green"
    char buffer[128];       // character buffer array for formating
    string faultCode;       // Code to report for failed values
    string faultDesc;       // Description for why the test failed
    const UINT32 rollCount = m_component->GetWheelCount();
    float icmDrags[ rollCount];

    // Clear out previous drag force results
    m_dragForce.clear();

    for(UINT32 roller = 0; (roller < rollCount); roller++)
    {   // calculate the average forces and validate the results
        testStatus = AverageForces(roller, average, m_dragTestStats[roller]);
        
        // determine whether the minimum drag parameter will be used
        bool useMinParameter = m_component->GetParameterBool("CheckMinDragForce");
        if(testStatus == BEP_STATUS_SUCCESS)
        {   // analyze the data
            if (useMinParameter)
            {
                testStatus = ValidateMinDrag(roller, average);
                if (testStatus == BEP_STATUS_SUCCESS)
                {
                    testStatus = ValidateDrag(roller, average);
                }
            }
            else
            {
                testStatus = ValidateDrag(roller, average);
            }
            
            color = (testStatus == BEP_STATUS_SUCCESS) ? "Green" : "Red";
            icmDrags[ roller] = average;
            // Add the offset back in
            m_dragTestStats[roller].minValueIndex += dragStart;
            m_dragTestStats[roller].maxValueIndex += dragStart;
        }
        else
        {   // set the value to fail
            average = -1;
            color = "Red";
            icmDrags[ roller] = 0;
        }
        // place the value into the drag force array and status array
        m_dragForce.push_back(average);

        // update the system
        m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "DragValue"), average);
        m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "DragBGColor"), color);
        // Update the fault data based on the results
        if (m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")) == "1" &&
             m_component->ReadSubscribeData(m_component->GetDataTag("FrontAxleTestSelected")) == "0")
        {
            string faultTag = rollerName[roller+2]+"DragFault";
            faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
            faultDesc = (color == "Green" ? rollerName[roller+2] + " Brake Drag Test Result" : m_component->GetFaultDescription(faultTag));
            // send the test results to the TestResultServer
            memset(buffer, 0, sizeof(buffer));
            string param(roller+2 < 2 ? "FrontMaxDragForce" : "RearMaxDragForce");
            float maxDragForce = m_component->GetVehicleParameter("BrakeForces/" + param, float(0.0));
            if (useMinParameter) 
            {
                // Add minimum test
                string minParameter(roller+2 < 2 ? "FrontMinDragForce" : "RearMinDragForce");
                float minDragForce = m_component->GetVehicleParameter("BrakeForces/" + minParameter, float(0.0));

                m_component->SendSubtestResultWithDetail(rollerName[roller + 2] + "DragTest", testStatus, faultDesc, faultCode,
                                                         "DragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", average), unitsLBF,
                                                         "MaxDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", maxDragForce), unitsLBF,
                                                         "MinDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", minDragForce), unitsLBF);
            }
            else
            {
                m_component->SendSubtestResultWithDetail(rollerName[roller + 2] + "DragTest", testStatus, faultDesc, faultCode,
                                                         "DragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", average), unitsLBF,
                                                         "MaxDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", maxDragForce), unitsLBF);
            }
            
            m_component->SendSubtestResultWithDetail(rollerName[roller+2] + "DragTestStats", testStatus, faultDesc, faultCode,
                                                     "MinDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller+2].minValue), unitsLBF,
                                                     "MaxDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller+2].maxValue), unitsLBF,
                                                     "AverageDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller+2].averageValue), unitsLBF,
                                                     "MinDragIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_dragTestStats[roller+2].minValueIndex), "",
                                                     "MaxDragIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_dragTestStats[roller+2].maxValueIndex), "");
            // Write the results to the system for display
            m_component->SystemWrite(rollerName[roller+2]+"DragMinValue", m_dragTestStats[roller+2].minValue);
            m_component->SystemWrite(rollerName[roller+2]+"DragMaxValue", m_dragTestStats[roller+2].maxValue);
            m_component->SystemWrite(rollerName[roller+2]+"DragAverageValue", m_dragTestStats[roller+2].averageValue);
            m_component->SystemWrite(rollerName[roller+2]+"DragMinValueIndex", m_dragTestStats[roller+2].minValueIndex);
            m_component->SystemWrite(rollerName[roller+2]+"DragMaxValueIndex", m_dragTestStats[roller+2].maxValueIndex);
        }
        else
        {
            string faultTag = rollerName[roller]+"DragFault";
            faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
            faultDesc = (color == "Green" ? rollerName[roller] + " Brake Drag Test Result" : m_component->GetFaultDescription(faultTag));
            // send the test results to the TestResultServer
            memset(buffer, 0, sizeof(buffer));
            string param(roller < 2 ? "FrontMaxDragForce" : "RearMaxDragForce");
            float maxDragForce = m_component->GetVehicleParameter("BrakeForces/" + param, float(0.0));
            if (useMinParameter)
            {
                string minParameter(roller < 2 ? "FrontMinDragForce" : "RearMinDragForce");
                float minDragForce = m_component->GetVehicleParameter("BrakeForces/" + minParameter, float(0.0));

                m_component->SendSubtestResultWithDetail(rollerName[roller] + "DragTest", testStatus, faultDesc, faultCode,
                                                     "DragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", average), unitsLBF,
                                                     "MaxDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", maxDragForce), unitsLBF,
                                                     "MinDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", minDragForce), unitsLBF);
            }
            else
            {
                m_component->SendSubtestResultWithDetail(rollerName[roller] + "DragTest", testStatus, faultDesc, faultCode,
                                                     "DragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", average), unitsLBF,
                                                     "MaxDragForce", CreateMessage(buffer, sizeof(buffer), "%.2f", maxDragForce), unitsLBF);
            }
            
            m_component->SendSubtestResultWithDetail(rollerName[roller] + "DragTestStats", testStatus, faultDesc, faultCode,
                                                     "MinDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller].minValue), unitsLBF,
                                                     "MaxDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller].maxValue), unitsLBF,
                                                     "AverageDragValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_dragTestStats[roller].averageValue), unitsLBF,
                                                     "MinDragIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_dragTestStats[roller].minValueIndex), "",
                                                     "MaxDragIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_dragTestStats[roller].maxValueIndex), "");
            // Write the results to the system for display
            m_component->SystemWrite(rollerName[roller]+"DragMinValue", m_dragTestStats[roller].minValue);
            m_component->SystemWrite(rollerName[roller]+"DragMaxValue", m_dragTestStats[roller].maxValue);
            m_component->SystemWrite(rollerName[roller]+"DragAverageValue", m_dragTestStats[roller].averageValue);
            m_component->SystemWrite(rollerName[roller]+"DragMinValueIndex", m_dragTestStats[roller].minValueIndex);
            m_component->SystemWrite(rollerName[roller]+"DragMaxValueIndex", m_dragTestStats[roller].maxValueIndex);
        }
        // update the result of the check
        m_component->UpdateResult(testStatus, status);
    }

    // Update the drag forces to the ICM (for brake force compensation)
    bool includeDrag = m_component->GetParameterBool("IncludeDragInBrakeForce");
    if( false == includeDrag)
    {
        m_component->Log( LOG_DEV_DATA, "Updating ICM drags: \n");
        for( UINT32 ii=0; ii<rollCount; ii++)
        {
            m_component->Log( LOG_DEV_DATA, "\tDrag[%d] = %.2f\n", ii, icmDrags[ ii]);
        }
        int icmFd;
        if( -1 != (icmFd = open( "/dev/Icm/Block/Drag", O_RDWR)) )
        {
            write( icmFd, icmDrags, sizeof( icmDrags));
            close( icmFd);
            m_component->Log( LOG_DEV_DATA, "Drag forces updated to ICM\n");
        }
        else
        {
            m_component->Log( LOG_ERRORS, "Error updating drag forces to the ICM: %s\n", strerror( errno));
        }
    }
    else
    {
        m_component->Log( LOG_DEV_DATA, "NOT updating drag forces to ICM due to IncludeDragInBrakeForce\n");
    }

    m_component->Log(LOG_FN_ENTRY, "Analyze Drag Force done: %d\n", status);

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::AnalyzeBrakeForces(INT32 brakeStart, INT32 brakeEnd)
{
    INT32 status = BEP_STATUS_SUCCESS;
    INT32 testStatus;
    // iterate throught the wheels and evaluate
    float force;             // the current brake force
    float modifiedForce;     // The brake force with the multiplier applied.
    float modifiedMinForce;  // The minimum required brake force with multiplier applied
    char buffer[256];        // data array for printing
    string faultCode;        // Code to report to external systems for a failure condition
    string faultDesc;        // Description of what the test failed for

    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeBrakeForces() - Reading IcmForceArray\n");
    // Clear out previous drag force results
    m_brakeForce.clear();
    // read in the data from the brake force array
    testStatus = m_component->ReadDataArrays(m_component->GetParameter("IcmForceArray"), brakeStart, brakeEnd, m_wheelForceArray);
    //m_component->Log(LOG_DEV_DATA,"Wheel Count: %d",m_component->GetWheelCount());
    for(UINT32 roller = 0; (roller < m_component->GetWheelCount()); roller++)
    {   // calculate the average forces and validate the results
        testStatus = AverageForces(roller, force, m_brakeTestStats[roller]);
        if(testStatus == BEP_STATUS_SUCCESS)
        {   // place the normalized value into the brake force array
            m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeBrakeForces() - step1: Force - %.2f\n", force);
            m_brakeForce.push_back(force);
            m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeBrakeForces() - step2: m_brakeForce - %.2f\n", m_brakeForce.back());
            testStatus = ValidateBrakeForce(roller, force);
            m_brakeTestStats[roller].minValueIndex += brakeStart;
            m_brakeTestStats[roller].maxValueIndex += brakeStart;
        }
        else    // if there is a problem averaging the forces display -1
        {
            m_brakeForce.push_back(-1); // write the value to -1
            // update the result and the background color of the result on the GUI
            m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ForceValue"), string("-1"));
            m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ForceBGColor"), string("Red"));
        }
        m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool::AnalyzeBrakeForces() - step3\n");
        // Set the fault codes and descriptions
        string faultTag = rollerName[roller]+"ForceFault";
        faultCode = ((BEP_STATUS_TYPE)testStatus == BEP_STATUS_SUCCESS ? "0000" : m_component->GetFaultCode(faultTag));
        faultDesc = ((BEP_STATUS_TYPE)testStatus == BEP_STATUS_SUCCESS ? rollerName[roller] + " Brake Force Test Result" : m_component->GetFaultDescription(faultTag));
        // send the test results to the TestResultServer
        memset(buffer, 0 , sizeof(buffer));       // clear the data array
        modifiedForce = ApplyForceMultiplier(force, m_component->GetParameterFloat("BrakeForceMultiplier"));
        float minForce = (roller < (UINT32)2) ? m_component->GetTestStepInfoFloat("FrontMinBrakeForce") : m_component->GetTestStepInfoFloat("RearMinBrakeForce");
        modifiedMinForce = ApplyForceMultiplier(minForce, m_component->GetParameterFloat("BrakeForceMultiplier"));
        string minParamName;
        string maxParamName;
        if (m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")) == "1" &&
             m_component->ReadSubscribeData(m_component->GetDataTag("FrontAxleTestSelected")) == "0")
        {
            if (roller+2 < (UINT32)2)
            {
                minParamName = "BrakeForces/FrontMinBrakeForce";
                maxParamName = "BrakeForces/FrontMaxBrakeForce";
            }
            else if(roller+2 < (UINT32)4)
            {
                minParamName = "BrakeForces/RearMinBrakeForce";
                maxParamName = "BrakeForces/RearMaxBrakeForce";
            }
            else
            {
                minParamName = "BrakeForces/TandemMinBrakeForce";
                maxParamName = "BrakeForces/TandemMaxBrakeForce";
            }
            m_component->SendSubtestResultWithDetail(rollerName[roller+2] + "ForceTest", testStatus, faultDesc, faultCode,
                                                 "BrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", force), unitsLBF,
                                                 "MaxBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", m_component->GetVehicleParameter(maxParamName, float(0.0))), unitsLBF,
                                                 "MinBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", m_component->GetVehicleParameter(minParamName, float(0.0))), unitsLBF,
                                                 "MinBrakeForceWithMultiplier", CreateMessage(buffer, sizeof(buffer), "%.2f", modifiedMinForce), unitsLBF,
                                                 "BrakeForceWithMultiplier", CreateMessage(buffer, sizeof(buffer), "%.2f", modifiedForce), unitsLBF);
            m_component->SendSubtestResultWithDetail(rollerName[roller+2] + "BrakeTestStats", testStatus, faultDesc, faultCode,
                                                 "MinBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller+2].minValue), unitsLBF,
                                                 "MaxBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller+2].maxValue), unitsLBF,
                                                 "AverageBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller+2].averageValue), unitsLBF,
                                                 "MinBrakeIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_brakeTestStats[roller+2].minValueIndex), "",
                                                 "MaxBrakeIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_brakeTestStats[roller+2].maxValueIndex), "");

            // Write the results to the system for display
            m_component->SystemWrite(rollerName[roller+2]+"BrakeMinValue", m_brakeTestStats[roller+2].minValue);
            m_component->SystemWrite(rollerName[roller+2]+"BrakeMaxValue", m_brakeTestStats[roller+2].maxValue);
            m_component->SystemWrite(rollerName[roller+2]+"BrakeAverageValue", m_brakeTestStats[roller+2].averageValue);
            m_component->SystemWrite(rollerName[roller+2]+"BrakeMinValueIndex", m_brakeTestStats[roller+2].minValueIndex);
            m_component->SystemWrite(rollerName[roller+2]+"BrakeMaxValueIndex", m_brakeTestStats[roller+2].maxValueIndex);
        }
        else
        {
            if (roller < (UINT32)2)
            {
                minParamName = "BrakeForces/FrontMinBrakeForce";
                maxParamName = "BrakeForces/FrontMaxBrakeForce";
            }
            else if(roller < (UINT32)4)
            {
                minParamName = "BrakeForces/RearMinBrakeForce";
                maxParamName = "BrakeForces/RearMaxBrakeForce";
            }
            else
            {
                minParamName = "BrakeForces/TandemMinBrakeForce";
                maxParamName = "BrakeForces/TandemMaxBrakeForce";
            }
            m_component->SendSubtestResultWithDetail(rollerName[roller] + "ForceTest", testStatus, faultDesc, faultCode,
                                                 "BrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", force), unitsLBF,
                                                 "MaxBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", m_component->GetVehicleParameter(maxParamName, float(0.0))), unitsLBF,
                                                 "MinBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", m_component->GetVehicleParameter(minParamName, float(0.0))), unitsLBF,
                                                 "MinBrakeForceWithMultiplier", CreateMessage(buffer, sizeof(buffer), "%.2f", modifiedMinForce), unitsLBF,
                                                 "BrakeForceWithMultiplier", CreateMessage(buffer, sizeof(buffer), "%.2f", modifiedForce), unitsLBF);
            m_component->SendSubtestResultWithDetail(rollerName[roller] + "BrakeTestStats", testStatus, faultDesc, faultCode,
                                                 "MinBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller].minValue), unitsLBF,
                                                 "MaxBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller].maxValue), unitsLBF,
                                                 "AverageBrakeValue", CreateMessage(buffer, sizeof(buffer), "%.2f", m_brakeTestStats[roller].averageValue), unitsLBF,
                                                 "MinBrakeIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_brakeTestStats[roller].minValueIndex), "",
                                                 "MaxBrakeIndex", CreateMessage(buffer, sizeof(buffer), "%d", m_brakeTestStats[roller].maxValueIndex), "");

            // Write the results to the system for display
            m_component->SystemWrite(rollerName[roller]+"BrakeMinValue", m_brakeTestStats[roller].minValue);
            m_component->SystemWrite(rollerName[roller]+"BrakeMaxValue", m_brakeTestStats[roller].maxValue);
            m_component->SystemWrite(rollerName[roller]+"BrakeAverageValue", m_brakeTestStats[roller].averageValue);
            m_component->SystemWrite(rollerName[roller]+"BrakeMinValueIndex", m_brakeTestStats[roller].minValueIndex);
            m_component->SystemWrite(rollerName[roller]+"BrakeMaxValueIndex", m_brakeTestStats[roller].maxValueIndex);
        }
        // update the result of the check
        m_component->UpdateResult(testStatus, status);
    }

    m_component->Log(LOG_FN_ENTRY, "Analyze Brake Force done: %d\n", status);

    return(status);
}

//-----------------------------------------------------------------------------
void BaseBrakeTool::SendBrakeStoppingDistanceResult(string testName, 
                                                    string testResult, string faultDesc, string faultCode,
                                                    WHEELINFO stoppingDistance, 
                                                    string lfResult, string rfResult, string lrResult, string rrResult)
{
    char buff[128];
    memset(buff,0,sizeof(buff));
    m_component->SendSubtestResultWithDetail("Lf"+testName, lfResult, faultDesc, faultCode,
                                             "StoppingDistance", CreateMessage(buff, sizeof(buff), "%.2f", stoppingDistance.lfWheel), unitsFEET,
                                             "MinDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("FrontMinimumStoppingDistance", float(0.0))), unitsFEET,
                                             "MaxDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("FrontMaximumStoppingDistance", float(0.0))), unitsFEET);
    m_component->SendSubtestResultWithDetail("Rf"+testName, rfResult, faultDesc, faultCode,
                                             "StoppingDistance", CreateMessage(buff, sizeof(buff), "%.2f", stoppingDistance.rfWheel), unitsFEET,
                                             "MinDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("FrontMinimumStoppingDistance", float(0.0))), unitsFEET,
                                             "MaxDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("FrontMaximumStoppingDistance", float(0.0))), unitsFEET);
    m_component->SendSubtestResultWithDetail("Lr"+testName, lrResult, faultDesc, faultCode,
                                             "StoppingDistance", CreateMessage(buff, sizeof(buff), "%.2f", stoppingDistance.lrWheel), unitsFEET,
                                             "MinDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("RearMinimumStoppingDistance", float(0.0))), unitsFEET,
                                             "MaxDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("RearMaximumStoppingDistance", float(0.0))), unitsFEET);
    m_component->SendSubtestResultWithDetail("Rr"+testName, rrResult, faultDesc, faultCode,
                                             "StoppingDistance", CreateMessage(buff, sizeof(buff), "%.2f", stoppingDistance.rrWheel), unitsFEET,
                                             "MinDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("RearMinimumStoppingDistance", float(0.0))), unitsFEET,
                                             "MaxDistance", CreateMessage(buff, sizeof(buff), "%.2f", m_component->GetVehicleParameter("RearMaximumStoppingDistance", float(0.0))), unitsFEET);
}

//-----------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateDrag(INT32 roller, float average)
{
    INT32 testStatus;
    string param(roller < 2 ? "FrontMaxDragForce" : "RearMaxDragForce");
    switch(roller)
    {
    case LFWHEEL:
    case RFWHEEL:
        param = "FrontMaxDragForce";
        break;

    case LRWHEEL:
    case RRWHEEL:
        param = "RearMaxDragForce";
        break;

    case LTWHEEL:
    case RTWHEEL:
        param = "TandemMaxDragForce";
        break;

    default:
        param = "RearMaxDragForce";
        break;
    }
    float maxDragForce = m_component->GetVehicleParameter("BrakeForces/" + param, float(0.0));

    m_component->Log(LOG_FN_ENTRY, "ValidateDrag(%d, %f), max: %f\n", roller, average, maxDragForce);

    if(average > maxDragForce)     testStatus = BEP_STATUS_FAILURE;
    else                            testStatus = BEP_STATUS_SUCCESS;

    return(testStatus);
}

//-----------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateMinDrag(INT32 roller, float average)
{
    INT32 testStatus;
    string param(roller < 2 ? "FrontMinDragForce" : "RearMinDragForce");
    switch(roller)
    {
    case LFWHEEL:
    case RFWHEEL:
        param = "FrontMinDragForce";
        break;

    case LRWHEEL:
    case RRWHEEL:
        param = "RearMinDragForce";
        break;

    case LTWHEEL:
    case RTWHEEL:
        param = "TandemMinDragForce";
        break;

    default:
        param = "RearMinDragForce";
        break;
    }
    float minDragForce = m_component->GetVehicleParameter("BrakeForces/" + param, float(0.0));

    m_component->Log(LOG_FN_ENTRY, "ValidateDrag(%d, %f), min: %f\n", roller, average, minDragForce);

    if(average < minDragForce)     testStatus = BEP_STATUS_FAILURE;
    else                            testStatus = BEP_STATUS_SUCCESS;

    return(testStatus);
}

//-----------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateBrakeForce(INT32 roller, float average)
{
    m_component->Log(LOG_FN_ENTRY, "Entering BaseBrakeTool::ValidateBrakeForce\n");
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");
    string minParamName;
    string maxParamName;
    switch(roller)
    {
    case LFWHEEL:
    case RFWHEEL:
        minParamName = "FrontMinBrakeForce";
        maxParamName = "FrontMaxBrakeForce";
        break;

    case LRWHEEL:
    case RRWHEEL:
        minParamName = "RearMinBrakeForce";
        maxParamName = "RearMaxBrakeForce";
        break;

    case LTWHEEL:
    case RTWHEEL:
        minParamName = "TandemMinBrakeForce";
        maxParamName = "TandemMaxBrakeForce";
        break;

    default:
        m_component->Log(LOG_DEV_DATA, "BaseBrakeTool::ValidateBrakeForce() Error, roller number: %d\n", roller);
        minParamName = "RearMinBrakeForce";
        maxParamName = "RearMaxBrakeForce";
        break;
    }
    // Get the parameters from the vehicle build info
    float minBrakeForce = m_component->GetVehicleParameter("BrakeForces/" + minParamName, float(0.0));
    float maxBrakeForce = m_component->GetVehicleParameter("BrakeForces/" + maxParamName, float(0.0));
    // Make sure the parameters are valid
    if(minBrakeForce == 0.0)
    {
        minBrakeForce = m_component->GetTestStepInfoFloat(minParamName);
    }
    if(maxBrakeForce == 0.0)
    {
        maxBrakeForce = m_component->GetTestStepInfoFloat(maxParamName);
    }
    m_component->Log(LOG_FN_ENTRY, "ValidateBrakeForce(%d, %f), min: %f, max: %f\n", roller, average, minBrakeForce, maxBrakeForce);

    // if brake force is in the valid limits set ok
    if((average <= maxBrakeForce) && (average >= minBrakeForce))
    {
        testStatus = BEP_STATUS_SUCCESS;
        color = "Green";
    }

    // update the result and the background color of the result on the GUI
    m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ForceValue"), average);
    m_component->SystemWrite(m_component->GetDataTag(rollerName[roller] + "ForceBGColor"), color);

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateBalances(void)
{
    m_component->Log(LOG_FN_ENTRY, "Validating Balances\n");

    INT32 status = BEP_STATUS_SUCCESS;          // test status
    m_component->Log(LOG_DEV_DATA, "Brake forces -- \n");
    for(UINT32 index = 0; index < m_brakeForce.size(); index++)
    {
        m_component->Log(LOG_DEV_DATA, "\t\tindex: %d -- %.2f\n", index, m_brakeForce[index]);
    }
    INT32 testStatus;
    if (m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")) == "1" &&
         m_component->ReadSubscribeData(m_component->GetDataTag("FrontAxleTestSelected")) == "0") 
    {
        testStatus = RearBalance();          // perform the rear balance test
    }
    else
    {
        testStatus = FrontBalance();          // perform the front balance test
    }
    m_component->UpdateResult(testStatus, status);          // update the result of the check

    if((m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine"))).compare("1")) //if NOT a single axle
    {
        testStatus = RearBalance();                 // perform the rear balance test
        m_component->UpdateResult(testStatus, status);          // update the result of the check

        testStatus = FrontToRearBalance();          // perform the front to rear balance test
        m_component->UpdateResult(testStatus, status);          // update the result of the check

        testStatus = ValidateTandemBalances();      // perform the tandem balance tests
        m_component->UpdateResult(testStatus, status);          // update the result of the check
    }

    m_component->Log(LOG_FN_ENTRY, "Validating Balances: %d done\n", testStatus);

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::FrontBalance(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    std::string color("Red");

    m_component->Log(LOG_FN_ENTRY, "Checking Front Balance\n");

    // load the parameters and convert to %
    float balance = m_component->GetVehicleParameter("BrakeForces/FrontSideToSideBalance", float(0.0)) / 100.0;
//  float balance = m_component->GetTestStepInfoFloat(axle + "SideToSideBalance") / 100.0;
    m_component->Log(LOG_DEV_DATA, "Balance: %f, values: %f, %f\n", balance, m_brakeForce[LFWHEEL], m_brakeForce[RFWHEEL]);

    // perform the side to side comparisons of the wheels
    // balance = left wheel / left wheel + right wheel
    float sum = m_brakeForce[LFWHEEL] + m_brakeForce[RFWHEEL];
    float percentBalance = 0.0;

    m_component->Log(LOG_DEV_DATA, "Sum: %f\n", sum);

    if(sum > 0) // if sum > 0 calculate the percentage
    {
        // validate it the balance
        percentBalance = (m_brakeForce[LFWHEEL] / sum);
        m_component->Log(LOG_DEV_DATA, "Percent Balance: %f\n", percentBalance);
        if((percentBalance <= (.5 + balance)) && (percentBalance >= (.5 - balance)))
        {
            testStatus = BEP_STATUS_SUCCESS;
            color = "Green";
        }
        // convert the balnce for display XX.XX%
        percentBalance *= 100;
    }
    else    // fail the test
    {
        m_component->Log(LOG_ERRORS, "Error Sums Were Zero\n");
        percentBalance = -1;
    }
    m_component->Log(LOG_DEV_DATA, "Sum Calc Complete: %d, %s\n", testStatus, color.c_str());

    // update the GUI with value and status
    m_component->SystemWrite(m_component->GetDataTag("FrontBalanceValue"), percentBalance);
    m_component->SystemWrite(m_component->GetDataTag("FrontBalanceBGColor"), color);
    // Update the fault data to report with the test result
    string faultTag = "FrontBalanceFault";
    string faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
    string faultDesc = (color == "Green" ? "LF to RF Balance Test Result" : m_component->GetFaultDescription(faultTag));
    // send the test results to the TestResultServer
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    m_component->SendSubtestResultWithDetail(m_component->GetDataTag("FrontBalanceTest"), testStatus, faultDesc, faultCode,
                                             "Value", CreateMessage(buffer, sizeof(buffer),"%.2f", percentBalance), "Percent",
                                             "MaxPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 + balance) * 100), "Percent",
                                             "MinPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 - balance) * 100), "Percent");

    m_component->Log(LOG_FN_ENTRY, "Front Balance: %f, %s\n", percentBalance, color.c_str());

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::RearBalance(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");

    m_component->Log(LOG_FN_ENTRY, "Checking Rear Balance\n");

    // load the parameters and convert to %
    float balance = m_component->GetVehicleParameter("BrakeForces/RearSideToSideBalance", float(0.0)) / 100.0;
//  float balance = m_component->GetTestStepInfoFloat(axle + "SideToSideBalance") / 100.0;
    m_component->Log(LOG_DEV_DATA, "Balance: %f, values: %f, %f\n", balance, m_brakeForce[LRWHEEL], m_brakeForce[RRWHEEL]);

    float sum = m_brakeForce[LRWHEEL] + m_brakeForce[RRWHEEL];
    float percentBalance = 0;
    if(sum > 0) // if sum > 0 calculate the percentage
    {
        // validate it the balance
        percentBalance = m_brakeForce[LRWHEEL] / sum;
        if((percentBalance <= (.5 + balance)) && (percentBalance >= (.5 - balance)))
        {
            testStatus = BEP_STATUS_SUCCESS;
            color = "Green";
        }
        // convert the balnce for display XX.XX%
        percentBalance *= 100;
    }
    else    // fail the test
        percentBalance = -1;

    // update the GUI with value and status
    m_component->SystemWrite(m_component->GetDataTag("RearBalanceValue"), percentBalance);
    m_component->SystemWrite(m_component->GetDataTag("RearBalanceBGColor"), color);
    // Update the fault data to be reported with the test results
    string faultTag = "RearBalanceFault";
    string faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
    string faultDesc = (color == "Green" ? "LR to RR Balance Test Result" : m_component->GetFaultDescription(faultTag));
    // send the test results to the TestResultServer
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    m_component->SendSubtestResultWithDetail(m_component->GetDataTag("RearBalanceTest"), testStatus, faultDesc, faultCode,
                                             "Value", CreateMessage(buffer, sizeof(buffer),"%.2f", percentBalance), "Percent",
                                             "MaxPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 + balance) * 100), "Percent",
                                             "MinPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 - balance) * 100), "Percent");
    m_component->Log(LOG_FN_ENTRY, "Rear Balance: %f, %s\n", percentBalance, color.c_str());

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::FrontToRearBalance(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");

    m_component->Log(LOG_FN_ENTRY, "Checking Front To Rear Balance\n");

    // load the parameters and convert to %
    float min = m_component->GetVehicleParameter("BrakeForces/BalanceFrontToRearMin", float(0.0)) / 100.0;
    float max = m_component->GetVehicleParameter("BrakeForces/BalanceFrontToRearMax", float(0.0)) / 100.0;
    if(min == 0)  min = m_component->GetTestStepInfoFloat("BalanceFrontToRearMin") / 100.0;
    if(max == 0)  max = m_component->GetTestStepInfoFloat("BalanceFrontToRearMax") / 100.0;

    float frontSum = m_brakeForce[LFWHEEL] + m_brakeForce[RFWHEEL];
    float rearSum = m_brakeForce[LRWHEEL] + m_brakeForce[RRWHEEL];
    float percentBalance = 0;
    if((frontSum > 0) && (rearSum > 0))    // if sum > 0 calculate the percentage
    {
        // validate the balance
        float numerator = m_component->GetParameterBool("FrontToRearBalanceUseFront") ? frontSum : rearSum;
        percentBalance = numerator / (frontSum + rearSum);
        if((percentBalance >= min) && (percentBalance <= max))
        {
            testStatus = BEP_STATUS_SUCCESS;
            color = "Green";
        }
        // convert the balnce for display XX.XX%
        percentBalance *= 100;
    }
    else    // fail the test
        percentBalance = -1;

    // update the GUI with value and status
    m_component->SystemWrite(m_component->GetDataTag("FrontToRearBalanceValue"), percentBalance);
    m_component->SystemWrite(m_component->GetDataTag("FrontToRearBalanceBGColor"), color);
    // Update the fault data to be reported with the test result
    string faultTag = "FrontToRearBalanceFault";
    string faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
    string faultDesc = (color == "Green" ? "Front To Rear Balance Test Result" : m_component->GetFaultDescription(faultTag));
    // send the test results to the TestResultServer
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    m_component->SendSubtestResultWithDetail(m_component->GetDataTag("FrontToRearBalanceTest"), testStatus, faultDesc, faultCode,
                                             "Value", CreateMessage(buffer, sizeof(buffer),"%.2f", percentBalance), "Percent",
                                             "MaxPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", max * 100), "Percent",
                                             "MinPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", min * 100), "Percent");

    m_component->Log(LOG_FN_ENTRY, "Front To Rear Balance: %f, %s\n", percentBalance, color.c_str());

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::ValidateTandemBalances(void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;

    // 08/23/2005 - BRM
    // Need to calculate tandem balance on a 2-axle machine testing
    //  tandem axle vehicles
    // if (m_component->GetRollerCount() > 4)
    if(m_brakeForce.size() > 4)
    {
        if(TandemBalance() != BEP_STATUS_SUCCESS)
            testStatus = BEP_STATUS_FAILURE;
        if(TandemToRearBalance() != BEP_STATUS_SUCCESS)
            testStatus = BEP_STATUS_FAILURE;
    }
    else
        m_component->Log(LOG_ERRORS, "Not Performing Tandem Balances\n");

    return(testStatus);
}

INT32 BaseBrakeTool::TandemBalance(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");

    // load the parameters and convert to %
    float balance = m_component->GetTestStepInfoFloat("TandemSideToSideBalance") / 100;

    float sum = m_brakeForce[LTWHEEL] + m_brakeForce[RTWHEEL];
    float percentBalance = 0;
    if(sum > 0)    // if sum > 0 calculate the percentage
    {
        // validate it the balance
        percentBalance = m_brakeForce[LTWHEEL] / sum;
        if((percentBalance <= (.5 + balance)) ||
           (percentBalance >= (.5 - balance)))
        {
            testStatus = BEP_STATUS_SUCCESS;
            color = "Green";
        }
        // convert the balnce for display XX.XX%
        percentBalance *= 100;
    }
    else    // fail the test
        percentBalance = -1;

    // update the GUI with value and status
    m_component->SystemWrite(m_component->GetDataTag("TandemBalanceValue"), percentBalance);
    m_component->SystemWrite(m_component->GetDataTag("TandemBalanceBGColor"), color);

    // send the test results to the TestResultServer
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    // Update the fault data to be reported with the test result
    string faultTag = "TandemBalanceFault";
    string faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultTag));
    string faultDesc = (color == "Green" ? "LT to RT Balance Test Result" : m_component->GetFaultDescription(faultTag));
    m_component->SendSubtestResultWithDetail(m_component->GetDataTag("TandemBalanceTest"), testStatus, faultDesc, faultCode,
                                             "Value", CreateMessage(buffer, sizeof(buffer),"%.2f", percentBalance), "Tandem Side To Side Percent Balance",
                                             "MaxPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 + balance) * 100), "Maximum Allowed Balance Percent",
                                             "MinPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", (.5 - balance) * 100), "Minimum Allowed Balance Percent");
    return(testStatus);
}

INT32 BaseBrakeTool::TandemToRearBalance(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");

    // load the parameters and convert to %
    float min = m_component->GetTestStepInfoFloat("BalanceTandemToRearMin") / 100;
    float max = m_component->GetTestStepInfoFloat("BalanceTandemToRearMax") / 100;

    float rearSum = m_brakeForce[LRWHEEL] + m_brakeForce[RRWHEEL];
    float tandemSum = m_brakeForce[LTWHEEL] + m_brakeForce[RTWHEEL];
    float percentBalance = 0;
    if((rearSum > 0) && (tandemSum > 0))   // if sums > 0 calculate the percentage
    {
        // validate it the balance
        percentBalance = tandemSum / (rearSum + tandemSum);
        if((percentBalance <= max) && (percentBalance >= min))
        {
            testStatus = BEP_STATUS_SUCCESS;
            color = "Green";
        }
        // convert the balnce for display XX.XX%
        percentBalance *= 100;
    }
    else    // fail the test
        percentBalance = -1;

    // update the GUI with value and status
    m_component->SystemWrite(m_component->GetDataTag("TandemToRearBalanceValue"), percentBalance);
    m_component->SystemWrite(m_component->GetDataTag("TandemToRearBalanceBGColor"), color);

    // send the test results to the TestResultServer
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    // Update the fault data to be reported with the test result
    string faultTag = "TandemToRearBalanceFault";
    string faultCode = (color == "Green" ? "0000" : m_component->GetFaultCode(faultCode));
    string faultDesc = (color == "Green" ? "Tandem To Rear Balance Test Result" : m_component->GetFaultDescription(faultTag));
    // Report the test result
    m_component->SendSubtestResultWithDetail(m_component->GetDataTag("TandemToRearBalanceTest"), testStatus, faultDesc, faultCode,
                                             "Value", CreateMessage(buffer, sizeof(buffer),"%.2f", percentBalance), "Tandem To Rear Percent Balance",
                                             "MaxPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", max * 100), "Maximum Allowed Balance Percent",
                                             "MinPercentBalance", CreateMessage(buffer, sizeof(buffer),"%.2f", min * 100), "Minimum Allowed Balance Percent");
    // Return the status of the test
    return(testStatus);
}


//-------------------------------------------------------------------------------------------------
#if COMPONENT_TESTING
INT32 BaseBrakeTool::GetISpeeds(float wheelSpeed[])
{
    INT32 testStatus = BEP_STATUS_SUCCESS;

    m_component->Log(LOG_FN_ENTRY, "GetISpeeds():Roller Count = %d\n", m_component->GetWheelCount());
    try
    {   // clear out the wheel forces
        memset(wheelSpeed, 0, (sizeof(float) * m_component->GetRollerCount()));

        // read out all of the wheel force info
        std::string tag, response, value;
        for(UINT32 index = 0; (index < m_component->GetRollerCount()) && (testStatus == BEP_STATUS_SUCCESS); index++)
        {
            tag = m_component->GetDataTag(rollerName[index] + "Speed");
            m_component->Log(LOG_DEV_DATA, "Reading: %s\n", tag.c_str());
            testStatus = m_ndb->Read(tag, response, (index == (GetRollerCount()-1)));
        }
        // get the values of the wheel force
        for(UINT32 index = 0; (index < m_component->GetRollerCount()) && (testStatus == BEP_STATUS_SUCCESS); index++)
        {
            tag = m_component->GetDataTag(rollerName[index] + "Speed");
            testStatus = m_ndb->GetByTag(tag, value, response);
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                wheelSpeed[index] = atof(value.c_str());
                m_component->Log(LOG_DEV_DATA, "Wheel %d: %f\n", index, wheelSpeed[index]);
            }
            else
                m_component->Log(LOG_ERRORS, "Could Not Read Speed For Wheel: %s\n", tag.c_str());
        }
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "GetISpeeds Exception: %s\n", e.what());
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}
#else
INT32 BaseBrakeTool::GetISpeeds(float wheelSpeed[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    m_component->Log(LOG_FN_ENTRY, "GetISpeeds():Roller Count = %d\n", m_component->GetRollerCount());

    try
    {
        testStatus = m_component->GetWheelSpeeds(wheelSpeed);
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "GetISpeeds Exception: %s\n", e.what());
        testStatus = BEP_STATUS_ERROR;
    }
    return(testStatus);
}
#endif

//-------------------------------------------------------------------------------------------------
#if COMPONENT_TESTING
INT32 BaseBrakeTool::GetIForces(float wheelForce[])
{
    INT32 testStatus = BEP_STATUS_SUCCESS;

    try
    {   // clear out the wheel forces
        memset(wheelForce, 0, (sizeof(float) * m_component->GetRollerCount()));

        // read out all of the wheel force info
        string tag, value, response;
        for(UINT32 index = 0; (index < m_component->GetRollerCount()) && (testStatus == BEP_STATUS_SUCCESS); index++)
            testStatus = m_ndb->Read(m_component->GetDataTag(rollerName[index] + "Force") , response, (index == (GetRollerCount()-1)));

        // get the values of the wheel force
        for(UINT32 index = 0; (index < m_component->GetRollerCount()) && (testStatus == BEP_STATUS_SUCCESS); index++)
        {
            testStatus = m_ndb->GetByTag(m_component->GetDataTag(rollerName[index] + "Force") , value, response);
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                wheelForce[index] = atof(value.c_str());
                m_component->Log(LOG_DEV_DATA, "Wheel %d, Force: %f\n", index, wheelForce[index]);
            }
        }
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "GetIForces Exception: %s\n", e.what());
        testStatus = BEP_STATUS_ERROR;
    }

    return(testStatus);
}
#else
INT32 BaseBrakeTool::GetIForces(float wheelForce[])
{
    INT32 testStatus = BEP_STATUS_FAILURE;  // overall status
    BEP_STATUS_TYPE openStatus = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt
    m_component->Log(LOG_FN_ENTRY, "BaseBrakeTool: GetIForces():Roller Count = %d\n", m_component->GetRollerCount());
    try
    {   // clear out the wheel forces
        m_readForces.CriticalSectionEnter();
        memset(wheelForce, 0, (sizeof(float) * m_component->GetRollerCount()));
        if(!IsForceFileOpen())
        {   // Open the force file
            openStatus = OpenForceFile();
            if(BEP_STATUS_SUCCESS != openStatus)
            {   // error opening force file
                m_component->Log(LOG_ERRORS, "Error opening force file, not reading forces");
            }
        }
        // Ensure force file was opened properly
        if(IsForceFileOpen())
        {
            do
            {   // read the force block
                lseek(m_forceFile, 0, SEEK_SET);
                UINT32 bytes = read(m_forceFile, (void *) wheelForce, (sizeof(float) * m_component->GetRollerCount()));
                m_component->Log(LOG_EXTRA_DETAILED_DATA, "ForceThread - SingleAxleMachine: %s, bytes: %d\n", m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")).c_str(), bytes);
                if(bytes != (sizeof(float) * m_component->GetRollerCount()) && ((m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")) == "1") && bytes != 16))
                {   // if interrupted during a read, read again
                    if(errno == EINTR)
                    {
                        m_component->Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", m_component->GetParameter("CurrentForces").c_str());
                    }
                    // else display the error report
                    else
                    {
                        m_component->Log(LOG_ERRORS, "Error Reading The File Informaiton: %d, %s\n", bytes, strerror(errno));
                    }
                }
                else
                {
                    testStatus = BEP_STATUS_SUCCESS;
                    if(m_component->GetRollerCount() == 4 || (m_component->ReadSubscribeData(m_component->GetDataTag("SingleAxleMachine")) == "1"))
                        m_component->Log(LOG_DEV_DATA, "BaseBrakeTool: Read forces: %.2f, %.2f, %.2f, %.2f\n",
                                         wheelForce[0], wheelForce[1], wheelForce[2], wheelForce[3]);
                    else
                        m_component->Log(LOG_DEV_DATA, "Read forces: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                                         wheelForce[0], wheelForce[1], wheelForce[2], wheelForce[3], wheelForce[4], wheelForce[5]);
                }
                // if the file could not be read, give up the time slice
                if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
            }while( (testStatus != BEP_STATUS_SUCCESS) && (retry--));
        }
        else
        {   // Force file is not open for reading
            m_component->Log(LOG_ERRORS, "Force file is not open, cannot read wheel force values");
        }
        m_readForces.CriticalSectionExit();
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "GetIForces Exception: %s\n", e.what());
        testStatus = BEP_STATUS_ERROR;
        // try to get the semaphore
        m_readForces.CriticalSectionTryEnter();
        // post it no matter the result
        m_readForces.CriticalSectionExit();
    }

    return(testStatus);
}
#endif

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::SpeedCheck(float speed, bool faster)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    float wheelSpeed[m_component->GetRollerCount()];
    if(GetISpeeds(wheelSpeed) == BEP_STATUS_SUCCESS)
    {
        m_component->Log(LOG_DEV_DATA, "SpeedCheck %.2f, WheelSpeeds: ", speed);
        bool testCondition=true;
        if(faster) // if a check for faster than speed
        {
            for(UINT32 index = 0; (index < m_component->GetWheelCount()) && (testCondition == true); index++)
            {
                testCondition = (wheelSpeed[index] >= speed);
                m_component->Log(LOG_DEV_DATA, "%.2f, %d, ", wheelSpeed[index], testCondition);
            }
        }
        else        // else if a check for slower than speed
        {
            for(UINT32 index = 0; (index < m_component->GetWheelCount()) && (testCondition == true); index++)
            {
                testCondition = (wheelSpeed[index] < speed);
                m_component->Log(LOG_DEV_DATA, "%.2f, %d, ", wheelSpeed[index], testCondition);
            }
        }
        if(testCondition == true)
            testStatus = BEP_STATUS_SUCCESS;
        else
            testStatus = BEP_STATUS_FAILURE;
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Could Not Read Wheel Speeds\n");
        testStatus = BEP_STATUS_ERROR;
    }


    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
float BaseBrakeTool::GetSpeed (void)
{
    float wheelSpeed[m_component->GetRollerCount()];    // wheel array
    float speed;                        // lowest speed value

    if(GetISpeeds(wheelSpeed) == BEP_STATUS_SUCCESS)
    {
        // find the lowest speed wheel and return it
        speed = wheelSpeed[0];
        for(UINT32 index = 1; index < m_component->GetWheelCount(); index++)
        {
            if(m_component->GetParameterBool("TwoAxleOnTandem") && ((index == 2) || (index == 3)))
            {
                m_component->Log(LOG_DETAILED_DATA, "Two axle vehicle running rear on tandem rollers, not evaluating rear speed"); 
            }
            else
            {
                if(speed > wheelSpeed[index])
                    speed = wheelSpeed[index];
            }
        }
    }
    else
        throw BepException("GetSpeed Can Not Read Wheel Speeds\n");

    m_component->Log(LOG_DEV_DATA, "Update Speed Value: %f\n WheelCount: %d AxleCount: %s\n", speed, m_component->GetWheelCount(), m_component->GetDataTag( "VehicleAxleCount").c_str());

    return(speed);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::UpdateTarget(bool status) /* = true*/
{
    INT32 testStatus, overallStatus = BEP_STATUS_SUCCESS;

    if(status) // if instructed to enable the target
    {
        m_component->Log(LOG_DEV_DATA, "UpdateTarget() enabling the force target and updating\n");
        std::string value;
        // update the domain on the radial gauge
        value = m_component->GetTestStepInfo("BrakeDomain");
        if(!value.empty())
        {
            testStatus = m_component->SystemWrite(m_component->GetDataTag("BrakeDomain"), value);
            if(testStatus != BEP_STATUS_SUCCESS)
                overallStatus = testStatus;
        }
        // update the domain target the radial gauge
        value = m_component->GetTestStepInfo("BrakeTarget");
        if(!value.empty())
        {
            testStatus = m_component->SystemWrite(m_component->GetDataTag("BrakeTarget"), value);
            if(testStatus != BEP_STATUS_SUCCESS)
                overallStatus = testStatus;
        }
        // update the radial gauge state to active
        testStatus = m_component->SystemWrite(m_component->GetDataTag("BrakeActive"), 1);
        if(testStatus != BEP_STATUS_SUCCESS)
            overallStatus = testStatus;
        // enable the force updates
        testStatus = EnableForceUpdates();
        if(testStatus != BEP_STATUS_SUCCESS)
            overallStatus = testStatus;
    }
    else    // else just disable the target
    {
        m_component->Log(LOG_DEV_DATA, "BaseBrakeTool: UpdateTarget() disabling the force target and updating\n");
        // update the radial gauge state to inactive
        testStatus = m_component->SystemWrite(m_component->GetDataTag("BrakeActive"), 0);
        if(testStatus != BEP_STATUS_SUCCESS)
        {
            m_component->Log(LOG_DEV_DATA, "BaseBrakeTool: m_component->SystemWrite(m_component->GetDataTag(BrakeActive), 0); != BEP_STATUS_SUCCESS \n");
            overallStatus = testStatus;
            m_component->Log(LOG_DEV_DATA, "overallStatus = %d \n",overallStatus);
        }
        // disable the force thread
        testStatus = DisableForceUpdates();
        if(testStatus != BEP_STATUS_SUCCESS)
            overallStatus = testStatus;

    }

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::AverageForces(int wheel, float &average, BrakeDataStats &dataStats)
{
    INT32 testStatus = BEP_STATUS_NA;           // the status of the test

    // Make sure we have data for this wheel
    if( m_wheelForceArray.size() > (UINT32)wheel)
    {
        const DATAARRAY &array = m_wheelForceArray[wheel];  // get the wheel force array

        m_component->Log(LOG_FN_ENTRY, "Average Forces, Array Size: %d\n", array.size());

        // walk through the data array and add the values
        float total=0;
        dataStats.minValue = 999.9;
        dataStats.maxValue = 0.0;
        for(UINT32 index = 0;index < array.size(); index++)
        {   // Determine if this is the min value in the array
            if(array[index] < dataStats.minValue)
            {   // Store the new value
                dataStats.minValue = array[index];
                dataStats.minValueIndex = index;
            }
            // Determine if this is the max value in the array
            if(array[index] > dataStats.maxValue)
            {   // Store the new value
                dataStats.maxValue = array[index];
                dataStats.maxValueIndex = index;
            }
            total += array[index];
            m_component->Log(LOG_DETAILED_DATA, "Wheel: %d, Sample: %d, total: %f\n", wheel, index, total);
        }
        // calculate the average force over the sampled interval
        if(array.size() > 0)
        {
            average = total / array.size();
            if(average < 0) average = 0;   // range check
            testStatus = BEP_STATUS_SUCCESS;
        }
        else
        {
            average = -1;
            testStatus = BEP_STATUS_FAILURE;
        }
    }
    else
    {
        m_component->Log(LOG_ERRORS, "No array for wheel %d\n", wheel);
        average = -1;
        testStatus = BEP_STATUS_FAILURE;
    }

    dataStats.averageValue = average;
    m_component->Log(LOG_FN_ENTRY, "Status: %d, Wheel: %d, Average Force: %f\n", testStatus, wheel, average);

    return(testStatus);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::Disengage(void)
{
    string tag, value, response;        // communication strings
    INT32 status = BEP_STATUS_FAILURE;

    m_component->Log(LOG_FN_ENTRY, "Disengaging System\n");

    try
    {   // command the motor mode to Torque Mode

        if(m_component->SystemCommand(MOTOR_MODE, string(TORQUE_MODE)) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               MOTOR_MODE, TORQUE_MODE);
        m_component->Log(LOG_DEV_DATA, "Motor Mode Is Torque Mode\n");

        // command the motor to 0 Torque
        if(m_component->SystemCommand(COMMAND_TORQUE, 0) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               COMMAND_TORQUE, "0");
        m_component->Log(LOG_DEV_DATA, "Torque Commanded To 0\n");

        // declutch the rollers
        if((m_component->ReadSubscribeData(MACHINE_CLUTCH) != "0") || (m_component->ReadSubscribeData(MACHINE_DECLUTCH) != "1"))
        {
            value = "0";    // disable clutch
            m_component->GetNamedDataBroker()->Write(MACHINE_CLUTCH, value, response, false);
            value = "1";    // enable declutch
            if(m_component->GetNamedDataBroker()->Write(MACHINE_DECLUTCH, value, response, true) 
               != BEP_STATUS_SUCCESS)
            {
                throw BepException("Could Not Declutch The Rollers\n");
            }
        }
        m_component->Log(LOG_DEV_DATA, "Clutches disengaged\n");

        // give time for the system to obtain the correct state
        int count = 20;
        while((BrakeTestStatusCheck() != BEP_STATUS_SUCCESS) && (count-- > 0))
        {
            m_component->Log(LOG_DEV_DATA, "Waiting For System To Disengage\n");
            BposSleep(100);
        }

        m_component->Log(LOG_DEV_DATA, "System disengaged\n");

        if(count)   status = BEP_STATUS_SUCCESS;
        else        status = BEP_STATUS_FAILURE;
    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Disengage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }
    catch(...)
    {
        m_component->Log(LOG_ERRORS, "Disengage Unknown Exception\n");
        status = BEP_STATUS_FAILURE;
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::Engage()
{
    string tag, value, response;        // communication strings
    INT32 status = BEP_STATUS_FAILURE;

    m_component->Log(LOG_FN_ENTRY, "Engaging System\n");

    try
    {   // Clutch the rollers
        if((m_component->ReadSubscribeData(MACHINE_CLUTCH) != "1") || (m_component->ReadSubscribeData(MACHINE_DECLUTCH) != "0"))
        {
            // make sure machine at Zerospeed
            if(m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) == "1")
            {   // then clutch
                value = "1";    // enable clutch
                m_component->GetNamedDataBroker()->Write(MACHINE_CLUTCH, value, response, false);
                value = "0";    // disable declutch
                if(m_component->GetNamedDataBroker()->Write(MACHINE_DECLUTCH, value, response, true) 
                   != BEP_STATUS_SUCCESS)
                {
                    throw BepException("Could Not Declutch The Rollers\n");
                }
            }
            else
                m_component->Log(LOG_ERRORS, "Error, Can Not Engage, Not At Zerospeed\n");
        }

        // command the motor mode to Boost Mode
        if(m_component->SystemCommand(MOTOR_MODE, string(BOOST_MODE)) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               MOTOR_MODE, BOOST_MODE);

        // set Boost to enable
        if(m_component->SystemWrite(BOOST_MODE, "1") != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               BOOST_MODE, "1");

        // give time for the system to obtain the correct state
        int count = 20;
        while((StatusCheck() != BEP_STATUS_SUCCESS) && (count-- > 0))
        {
            m_component->Log(LOG_DEV_DATA, "Waiting For System To Engage\n");
            BposSleep(100);
        }

        if(count)       status = BEP_STATUS_SUCCESS;
        else            status = BEP_STATUS_FAILURE;

    }
    catch(BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Engage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
const std::string BaseBrakeTool::GetBrakeTestingStatus(void)
{
    m_component->Log(LOG_DEV_DATA, "GetBrakeTestingStatus(): %s\n", m_calcBrakeForce.c_str());
    return(m_calcBrakeForce);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetBrakeTestingStatus(const std::string status)
{
    m_component->Log(LOG_DEV_DATA, "SetBrakeTestingStatus(): %s\n", status.c_str());
    m_component->UpdateResult(status, m_calcBrakeForce);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::ReadDataForWheelForceArray(const string &array, int startIdx, int endIdx)
{
    return(m_component->ReadDataArrays(array,startIdx,endIdx,m_wheelForceArray));
}

//-------------------------------------------------------------------------------------------------
float BaseBrakeTool::GetDragForceValue(const int &wheel)
{
    return(m_dragForce[wheel]);
}

//-------------------------------------------------------------------------------------------------
float BaseBrakeTool::GetBrakeForceValue(const int &wheel)
{
    return(m_brakeForce[wheel]);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetBrakeForceValue(const int &wheel, float value)
{
    // Dont SIGSEG
    if( (int)m_brakeForce.size() <= wheel)  // casting to remove warning
    {
        m_brakeForce.reserve( wheel+1);
        m_brakeForce.resize( wheel+1);
    }
    m_brakeForce[wheel] = value;
}

//-------------------------------------------------------------------------------------------------
float BaseBrakeTool::GetWheelForceValue(const int &wheel, const int &trial)
{
    return(m_wheelForceArray[wheel][trial]);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::GetWheelForceArray(const int &wheel, vector< float > &wheelArray)
{
    UINT32 cnt = 0;         // Used as a loop counter variable
    wheelArray.clear();
    for(cnt = 0; cnt < m_wheelForceArray[wheel].size(); cnt++)
    {
        wheelArray.push_back(m_wheelForceArray[wheel][cnt]);
    }
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetBrakeStart(const int &start)
{
    m_brakeStart = start;
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::GetBrakeStart(void)
{
    return(m_brakeStart);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetBrakeEnd(const int &end)
{
    m_brakeEnd = end;
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::GetBrakeEnd(void)
{
    return(m_brakeEnd);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SendOverallResult(const std::string result) /* = BEP_PASS_RESPONSE*/
{
    m_component->SetOverallResult(result);                              // set the overall result
    SetBrakeTestingStatus(m_component->GetOverallResult());             // set the overall brake testing status flag
    m_component->SendOverallResult(m_component->GetOverallResult());    // send the overall result
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::LaunchForceThread(void)
{   // create a thread to manage the component proxy
    pthread_attr_t attr;            // the attribute of the thread
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
//    if (pthread_create( &m_forceThreadID, &attr, BaseBrakeTool::ForceThread, (void *) this) != EOK)
    if(pthread_create( &m_forceThreadID, &attr, BaseBrakeTool::ForceThread, this) != EOK)
    {
        m_forceThreadID = -1;
        throw BepException("Could Not Create Force Thread");
    }
}

//-------------------------------------------------------------------------------------------------
//void* BaseBrakeTool::ForceThread(void *arg)
void* BaseBrakeTool::ForceThread(void *arg)
{
    BaseBrakeTool *brake = (BaseBrakeTool *)arg;
    // ndb interface to update the system
    INamedDataBroker ndb(brake->m_component->IsDebugOn());
    // tag to update
    std::string tag(BRAKE_FORCE_VALUE);
    std::string value, response;
    // set up the force array
    float wheelForce[brake->m_component->GetRollerCount()];
    char buffer[128];
    brake->m_component->Log(LOG_DEV_DATA, "Force Thread Started, Roller Count:%d\n",brake->m_component->GetRollerCount());
    do
    {
        // block untill it is time to start force reads
        brake->StartForceReads();

        // update the force thread while state is SAMPLE
        while(brake->GetForceReadStatus() == SAMPLE)
        {
            try
            {
                float force = 0;
                if(brake->GetIForces(wheelForce) == BEP_STATUS_SUCCESS)
                {
                    for(UINT32 index = 0; index < brake->m_component->GetWheelCount(); index++)
                    {   // if looking for maximum and is greater
                        if(force < wheelForce[index])
                            force = wheelForce[index];
                    }
                    // if system should update the force on the GUI
                    brake->SetForce(force);
                    memset(buffer, 0, sizeof(buffer));
                    value = CreateMessage(buffer, sizeof(buffer),"%.2f", force);
                    ndb.Write(tag, value, response,true);
                }
                else
                    brake->m_component->Log(LOG_DEV_DATA, "Force Thread Error Reading Forces\n");
            }
            catch(BepException &e)
            {
                brake->m_component->Log(LOG_DEV_DATA, "Force Thread Exception: %s\n", e.what());
            }
            catch(...)
            {
                brake->m_component->Log(LOG_DEV_DATA, "Force Thread Unknown Exception\n");
            }
            // update approx 10 times a second - moved out of try statement 9/27/06 JPS
            BposSleep(100);
        }
    }while(brake->GetForceReadStatus() != TERMINATE);

    brake->m_forceThreadID = -1;

    return(NULL);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::Reset(void)
{
    m_component->Log(LOG_FN_ENTRY, "Base Brake Resetting\n");
    // if the brake test did not pass, reset the status
    if(GetBrakeTestingStatus() != BEP_PASS_RESPONSE)
        m_calcBrakeForce = BEP_UNTESTED_RESPONSE;
}

//-------------------------------------------------------------------------------------------------
float BaseBrakeTool::GetForce(void)
{
    m_semReadCurrentForce.CriticalSectionEnter();
    float force = m_forceValue;
    m_semReadCurrentForce.CriticalSectionExit();
    return(force);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetForce(float force)
{
    m_semReadCurrentForce.CriticalSectionEnter();
    m_forceValue = force;
    m_semReadCurrentForce.CriticalSectionExit();
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::StartForceReads(void)
{
    SetForce(0.0);  // set the force to 0 when waiting
    int err;
    FORCETHREADSTATE    forceState = STOP;
    uint64_t            timeOut = mSEC_nSEC( 100LL);
    if( (err=m_updateForceSem.Acquire()) == EOK)
    {
        // Wait here until told to sample or terminate
        while( STOP == forceState)
        {
            m_updateForceSem.Wait( SAMPLE, timeOut);
            forceState = m_updateForceSem.GetValue();
        }

        m_updateForceSem.Release();
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Error locking m_updateForceSem in StartForceReads: %s\n", strerror( err));
    }
}

//-------------------------------------------------------------------------------------------------
FORCETHREADSTATE BaseBrakeTool::GetForceReadStatus(void)
{
    int err;
    FORCETHREADSTATE    forceState = STOP;

    if( (err=m_updateForceSem.Acquire()) == EOK)
    {
        forceState = m_updateForceSem.GetValue();
        m_updateForceSem.Release();
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Error locking m_updateForceSem in GetForceReadStatus: %s\n", strerror( err));
    }
    return( forceState);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::EnableForceUpdates(void)
{
    INT32 status = BEP_STATUS_SUCCESS;
    m_component->Log(LOG_DEV_DATA, "EnableForceUpdates()\n");
    int err;

    if( (err=m_updateForceSem.Acquire()) == EOK)
    {
        m_updateForceSem.Signal( SAMPLE);
        m_updateForceSem.Release();
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Error locking m_updateForceSem in EnableForceUpdates: %s\n", strerror( err));
        status = BEP_STATUS_ERROR;
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::DisableForceUpdates(void)
{
    INT32 status = BEP_STATUS_SUCCESS;
    m_component->Log(LOG_DEV_DATA, "DisableForceUpdates()\n");
    int err;

    if( (err=m_updateForceSem.Acquire()) == EOK)
    {
        m_updateForceSem.Signal( STOP);
        m_updateForceSem.Release();
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Error locking m_updateForceSem in DisableForceUpdates: %s\n", strerror( err));
        status = BEP_STATUS_ERROR;
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
INT32 BaseBrakeTool::TerminateForceUpdates(void)
{
    INT32 status = BEP_STATUS_SUCCESS;
    m_component->Log(LOG_DEV_DATA, "TerminateForceUpdates()\n");
    int err;

    if( (err=m_updateForceSem.Acquire()) == EOK)
    {
        m_updateForceSem.Signal( TERMINATE);
        m_updateForceSem.Release();
        // Wait a little for the force thread to die
        BposSleep( 250);
    }
    else
    {
        m_component->Log(LOG_ERRORS, "Error locking m_updateForceSem in TerminateForceUpdates: %s\n", strerror( err));
        status = BEP_STATUS_ERROR;
    }

    return(status);
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE BaseBrakeTool::OpenForceFile(void)
{
    INT32 attempts = 3;
    m_component->Log(LOG_DEV_DATA, "File Open: %s\n", m_component->GetParameter("CurrentForces").c_str());
    do
    {
        m_forceFile = open(m_component->GetParameter("CurrentForces").c_str(), O_RDONLY);
        SetForceFileOpen(m_forceFile != -1);
    } while(!IsForceFileOpen() && attempts--);

    m_component->Log(LOG_ERRORS, "Force File Opened: %s", IsForceFileOpen() ? "Success" : "Failure");
    return(IsForceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE BaseBrakeTool::CloseForceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsForceFileOpen())
    {   // Force file is open, close it
        do
        {
            closeStatus = close(m_forceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                m_component->Log(LOG_DEV_DATA, "Force File closed");
                SetForceFileOpen(false);
                m_forceFile = -1;
            }
            else
            {
                m_component->Log(LOG_DEV_DATA, "Error closing Force File: %s", strerror(errno));
            }
        } while(IsForceFileOpen() && closeAttempts--);
    }
    else
    {
        m_component->Log(LOG_DEV_DATA, "Force file not open, not attempting to close");
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return(closeStatus);
}

//-------------------------------------------------------------------------------------------------
bool& BaseBrakeTool::IsForceFileOpen(void)
{
    return(m_forceFileOpen);
}

//-------------------------------------------------------------------------------------------------
void BaseBrakeTool::SetForceFileOpen(bool fileOpen)
{
    m_forceFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
void BaseBrakeTool::ProcessBrakePedalForce(void)
{
    // if selected, store the brake pedal force result
    if(m_component->GetParameterBool("StoreBrakePedalForce"))
    {
        // report the test result (there is no evaluation)
        m_component->SendSubtestResultWithDetail("BrakePedalForce",BEP_STATUS_SUCCESS,"Brake Pedal Force","0000",
                                                 "BrakePedalForce",m_component->ReadSubscribeData("BrakePedalForce"),"LBF");
    }
}
//-----------------------------------------------------------------------------
UINT32 BaseBrakeTool::GetNumberOfWheelsToAnalyze(void)
{
    UINT32 numWheelsToAnalyze=4;

    // if we are testing on a tandem axle machine
    if(m_component->GetParameterBool("TandemAxleMachine") == true)
    {
        // if we are running a front axle test or a single rear axle
        if((m_component->ReadSubscribeData("FrontAxleTestSelected") == "1" && m_component->ReadSubscribeData("NumberOfFrontAxles") == "1") ||
           (m_component->ReadSubscribeData("NumberOfRearAxles") == "1") ||
           (!m_component->ReadSubscribeData("NumberOfFrontAxles").compare("2") && m_component->GetParameterBool("OnlyAccelerateFrontRollers")))
        {
            m_component->Log(LOG_DEV_DATA,"Front axle test or Single rear axle test - only analyzing front wheels");
            numWheelsToAnalyze = 2;
        }
        else
        {
            m_component->Log(LOG_DEV_DATA,"Rear axle test - analyzing all four wheels");
            numWheelsToAnalyze = 4;
        }
    }
    else    // else we are not testing on a tandem axle machine
    {
        m_component->Log(LOG_DEV_DATA,"Non-tandem axle machine:  analyzing all four wheels");
        numWheelsToAnalyze = 4;
    }

    return(numWheelsToAnalyze);
}

