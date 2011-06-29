//*************************************************************************
// Description:
// This file contains the functions necessary to perform a loss compensation
// test on the rolls tester.
//
//===========================================================================
// Copyright (c) 1999-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/LossCompensation/Common/LossCompensationTC.cpp $
// 
// 1     11/21/07 8:19a Derickso
// Refactored to use functions available in GenericTC for wheel speeds.
// 
// 10    2/22/07 9:01a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed #if 0 blocks.
// - Added a separator between the methods.
// 
// 9     1/17/07 3:37p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// - Added a check for MachineType in Stop().
// - Updated AnalyzeForces() to use the new calculations for determining
// encoder noise and runout errors.
// 
// 8     7/05/06 9:59p Cward
// Fix sequencing errors caused by Vehicle Present, lowering retainers, or
// turning off the loss comp switch during a Loss Comp test
//
// 7     5/08/06 7:11p Cward
// removed printf that was redundant with Log statement
//
// 6     4/13/06 3:26a Cward
// Changed hard coded unit strings with new units strings from GenericTC.
//
// 2     3/20/06 10:48a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 5     2/15/06 7:21p Cward
// Added Low torque startup. Added warmup speed tolerance. Updated
// Staggered Accelerate Test Step.
//
// 7     1/18/06 3:11p Bmeinke
// Lowered the loop delay inside LowTorqueStartup from 250 to 200 ms
//
// 6     1/17/06 1:30p Bmeinke
// Inside TestStepInitialize(), changed
// if(ReadSubscribeData(GetDataTag("MachineType")) == "1") to
// if(ReadSubscribeData(GetDataTag("MachineType3600")) == "0")
// Cleaned up the TestStepLowTorqueStartup() method
// Cleaned up the TestStepStaggeredAccelerate() method
// Changed TestStepStop() to use m_MotorController.Write() instead of
// SystemCommand() to set quick brake mode
// Cleaned up the LowTorqueStartup() method
//
// 5     1/16/06 12:30p Bmeinke
// Changed the CHECK_SENSOR_CROSS macro to a GetTestStepInfo() inside
// TestStepStaggeredAccelerate()
//
// 4     12/16/05 1:46p Bmeinke
// Added a new LowTorqueStartup() method to perform a low torque startup
// on an individual roller.
// TestStepStaggeredAccelerate() now checks to see if we want to perform a
// low torque startup before commanding speed to the individual drives.
//
// 3     12/12/05 2:52p Bmeinke
// Added TestStepLowTorqueStartup() to apply a small torque to the rollers
// before the acceleartion test step which would easily allow the rollers
// to be manually stopped in case somebody is working on the machine when
// the LC is started
//
// 2     11/17/05 4:44p Bmeinke
// Set testStatus=BEP_STATUS_SOFTWARE before logging its value inside
// TestStepStop()
// Initialize sampleNumber=0 inside TagArray()
//
// 1     11/17/05 10:35a Bmeinke
// Updating Tata to latest core
//
// 4     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 5     4/07/05 12:42a Bmeinke
// Added test step StaggeredAccelerate
//
// 4     4/04/05 12:42a Gpattiso
// Updated to ignore vehicle present becoming active during the
// acceleration phase and also to not error out if the retainers
// are lowered before zero speed is reached
//
// 3     6/24/04 12:42a Cward
// Removed pound if 0 blocks. Updated to new standard header comment
// block.
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:33p Cward
//
// 2     9/22/03 5:35p Derickso
// Updated to use GenericTC instead of TestComponent.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:52a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:44a Khykin
// Initial checkin
//
// 1     6/20/03 9:20a Khykin
// Initial checkin.
//
// 2     5/19/03 8:05a Khykin
// Updated for using thread safe CreateMessage function.
//
// 1     4/22/03 10:30a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:03  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:13a Khykin
// Initial Checkin
//
// 1     1/18/03 7:22a Khykin
// Initial Checkin
//
// 14    11/08/02 8:02a Tbochene
// Updating proper mode commands to MotorController and logic for better
// debugging
//
// 13    10/21/02 1:40p Khykin
// Updated for new prioritized prompts.
//
// 12    10/07/02 11:14a Khykin
// Adding subtest information for the values of the losses.
//
// 11    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 9     5/24/02 1:48p Kh
// Updating due to addition of system tags and debugging.
//
// 8     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
//
// 7     3/06/02 2:01p Kh
// Debugging in the system.
//
// 6     2/06/02 12:17a Kh
// Continuing work on LossCompensation component.
//
// 5     2/01/02 4:40p Kh
// Compiles now time to debug.
//
// 4     1/16/02 3:54p Russ.ross
// Checking in compiling LossCompensation files...
//
// 3     1/15/02 2:02p Russ.ross
// Compiling and running version, still needs some work...
//
// 2     1/03/02 3:52p Russ.ross
// Some updates...plus fill in the missing code...
//
// 1     1/03/02 8:32a Kh
//
// 5     12/20/01 1:41p Admin
// Checking in.
//
// 4     12/10/01 3:57p Russ.ross
// Updating with latest version...
//
// 3     12/02/01 7:10p Russ.ross
// Checking in the latest version...
//
// 2     11/20/01 8:38a Russ.ross
//
// 1     10/16/01 9:13a Russ.ross
// Checking in version of LossCompensation.cpp...
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "LossCompensationTC.h"

//=============================================================================
LossCompensationTC::LossCompensationTC() : GenericTC(), m_lcStatus(BEP_UNTESTED_STATUS)
{
}

//=============================================================================
LossCompensationTC::~LossCompensationTC()
{
}

//=============================================================================
void LossCompensationTC::Initialize(const XmlNode *config)
{   // Initialize the base component
    Log(LOG_FN_ENTRY, "Initializing The LossCompensationTC Component:\n%s\n", config->ToString().c_str());
    // call the base class initialize
    GenericTC::Initialize(config);
    // Setup the loss data
    SetupLossData();
    Log(LOG_FN_ENTRY, "Done Initializing LossCompensationTC Component\n");
    // signal that initialization is complete
    InitializationComplete();
}

//=============================================================================
void LossCompensationTC::WarmInitialize(void)
{   // Reset the testing status
    m_lcStatus = BEP_UNTESTED_STATUS;
    // Clear out the previous losses
    m_previousLosses.clear();
    // Load the loss data
    SetupLossData();
    // Clear the wheel force array
    LossCompensationTC::m_wheelForceArray.clear();
    // Complete the initialization
    GenericTC::WarmInitialize();
}

//=============================================================================
const string LossCompensationTC::CommandTestStep(const string &value)
{
    string status = BEP_ERROR_RESPONSE;     // the status of the commanded test step

    try
    {   // get the name of the sequenced test step
        string step = GetTestStepName();
        Log(LOG_FN_ENTRY, "%s Commanding: %s\n", GetComponentName().c_str(), step.c_str());

        if( step == "Initialize")               status = TestStepInitialize();
        else if( step=="LowTorqueStartup")      status = TestStepLowTorqueStartup();
        else if( step=="Accelerate")            status = TestStepAccelerate();
        else if( step=="StaggeredAccelerate")   status = TestStepStaggeredAccelerate();
        else if( step=="TwoMotorAccelerate")    status = TwoMotorAccelerate();
        else if( step=="LossCompensation")      status = TestStepLossCompensation();
        else if( step=="Stop")                  status = TestStepStop();
        else if( step=="CoastToStop")           status = CoastToStop();
        else if( step=="FinishUp")              status = TestStepFinishUp();
        else                                    status = BEP_UNSUPPORTED_RESPONSE;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "%s CommandTestStep %s Exception: %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    // set the test status
    SetLCStatus(status);

    return(status);
}

//=============================================================================
const string LossCompensationTC::TestStepInitialize(void)
{
    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepInitialize()\n");
    string tag, value, response;            // communication strings
    INT32 testStatus;                       // the status of the test

    try
    {   // display the prompts
        if(UpdatePrompts() != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Unable to Update Prompts\n");

        // if everything is not ready for testing, wait till it is
        if(LossCompStatusCheck() != BEP_STATUS_SUCCESS)
        {   // while not timed out and the machine not in the correct state
            bool done = false;
            while(TimeRemaining() && (done == false) && (!CheckAbort()))
            {   // only do this while the LossCompensationTC Switch is On
                if(ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) != "1")
                {
                    throw BepException("Loss Compensation Switch Turned Off\n");
                }

                // display the appropriate prompt for the user
                if(ReadSubscribeData(GetDataTag("VehiclePresent")) != "0")
                {
                    DisplayPrompt(GetTestStepInfo("VehiclePresentPromptBox") , 
                                  GetTestStepInfo("VehiclePresentPrompt"), 0);
                }
                else if(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
                {
                    DisplayPrompt(GetTestStepInfo("ZerospeedPromptBox") , GetTestStepInfo("ZerospeedPrompt"), 0);
                }
                else if(ReadSubscribeData(GetDataTag("RaiseRetainers")) != "1")
                {   
                    DisplayPrompt(GetTestStepInfo("RaiseRetainersPromptBox") , 
                                  GetTestStepInfo("RaiseRetainersPrompt"), 0);
                }
                else if(ReadSubscribeData(GetDataTag("StartLossCompensation")) != "1")
                {
                    DisplayPrompt(GetTestStepInfo("StartLossCompensationBox") , 
                                  GetTestStepInfo("StartLossCompensationPrompt"), 0);
                }
                else
                {
                    done = true;
                }
                // if not done wait and check again
                if(!done)   BposSleep (100);
            }

            if(CheckAbort())        // if an abort received
            {
                Log(LOG_ERRORS, "LossCompensationTC Abort Occurred During TestStepInitialize\n");
                testStatus = BEP_STATUS_ABORT;
            }
            if(!TimeRemaining())    // if timeout occurred
            {
                Log(LOG_ERRORS, "LossCompensationTC Timeout Occurred During TestStepInitialize\n");
                testStatus = BEP_STATUS_TIMEOUT;
            }
            else
                testStatus = BEP_STATUS_SUCCESS;
        }
        else
            testStatus = BEP_STATUS_SUCCESS;

        // if all is good, check the clutch
        if(testStatus == BEP_STATUS_SUCCESS)
        {
            testStatus = Engage();  // engage the system
            Log(LOG_DEV_DATA, "Machine Clutch Status: %d\n", testStatus);
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "TestStepInitialize Exception: %s\n", e.what());
        testStatus = BEP_STATUS_SOFTWARE;
    }

    // remove all of the prompts that were sent using UpdatePrompts()
    RemovePrompts();

    // update the test status
    string status;
    UpdateResult(testStatus, status);
    // write the test step information
    SendTestResult(status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepInitialize() complete: %s\n", status.c_str());
    return(status);             // return the status
}

//=============================================================================
const string LossCompensationTC::TestStepLowTorqueStartup(void)
{
    const string    trqModeTag( TORQUE_MODE);
    float           torqueCmd, maxRollerSpeed;
    float           wheelSpeeds[GetRollerCount()];
    char            cmdVal[256];
    string          status = BEP_SKIP_RESPONSE;
    INT32           testStatus;
    UINT32          rollIdx, ii;

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepLowTorqueStartup()\n");

    // Make sure this is an electric machine
    if(ReadSubscribeData(GetDataTag("MachineType3600")) == "1")
    {
        if(GetLCStatus() == BEP_PASS_RESPONSE)
        {
            testStatus = LossCompStatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                ///////////////////////////////////////////////////////
                // Step 1:
                //  Read test step parameters
                ///////////////////////////////////////////////////////

                // Read the amount of torque to apply (default to 5 ft-lbs)
                torqueCmd = GetTestStepParameter( "StartupTorqueCommand", torqueCmd, 5);

                // Read the max speed we want the rollers to go in torque mode (default to 3 MPH)
                maxRollerSpeed = GetTestStepParameter( "MaxTorqueModeSpeed", maxRollerSpeed, 3);

                Log( LOG_DEV_DATA, "Low torque startup value=%.03f ft-lbs (up to %.03f MPH)\n",
                     torqueCmd, maxRollerSpeed);

                try
                {
                    ///////////////////////////////////////////////////////
                    // Step 2:
                    //  Keep the rollers in torque mode at/below the max speed
                    //  for the desired amount of time
                    ///////////////////////////////////////////////////////
                    // Make sure all torque and speed commands are set to 0
                    for( rollIdx=LFWHEEL; rollIdx<(GetRollerCount()-1); rollIdx++)
                    {
                        m_MotorController.Write(rollerName[rollIdx]+"TorqueValue", "0", false);
                        m_MotorController.Write(rollerName[rollIdx]+"SpeedValue", "0", false);
                    }
                    m_MotorController.Write(rollerName[GetRollerCount()-1]+"TorqueValue", "0", false);
                    m_MotorController.Write(rollerName[GetRollerCount()-1]+"SpeedValue", "0", true);

                    // Command each motor to a low torque value
                    for( rollIdx=LFWHEEL; (rollIdx<GetRollerCount()) && (BEP_STATUS_SUCCESS == testStatus); rollIdx++)
                    {
                        // Send the initial torque mode command to this roller
                        m_MotorController.Write(rollerName[rollIdx]+"TorqueValue", cmdVal, false);
                        m_MotorController.Write(rollerName[rollIdx]+"MotorMode", trqModeTag, true);

                        // Restart our timeout for each roller
                        SetStartTime();
                        while( (TimeRemaining()) && (BEP_STATUS_SUCCESS == testStatus))
                        {
                            // Make sure none of the rollers exceeds our desired max speed
                            testStatus = GetWheelSpeeds( wheelSpeeds);
                            for( ii=LFWHEEL; (ii<rollIdx) && (BEP_STATUS_SUCCESS == testStatus); ii++)
                            {
                                Log( LOG_DEV_DATA, "LF: %.03f, RF: %.03f, LR: %.03f, RR: %.03f\n",
                                     wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
                                     wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);

                                // If above the max speed, command 0 torque
                                if( wheelSpeeds[ii] > maxRollerSpeed)   sprintf( cmdVal, "%.03f", 0.0);
                                // Command desired torque
                                else                                    sprintf( cmdVal, "%.03f", torqueCmd);

                                Log( LOG_DEV_DATA, "Commanding torque of %s to %s roll (@ %.03f MPH)\n",
                                     cmdVal, rollerName[ii].c_str(), wheelSpeeds[rollIdx]);

                                // Send the new torque command
                                m_MotorController.Write(rollerName[ii]+"TorqueValue", cmdVal, false);
                                m_MotorController.Write(rollerName[ii]+"MotorMode", trqModeTag, true);

                                testStatus = StatusSleep( 250);
                            }
                        }

                        //  Make sure the commanded rolls are not being restrained
                        if( BEP_STATUS_SUCCESS == testStatus)
                        {
                            testStatus = GetWheelSpeeds( wheelSpeeds);
                            for( ii=LFWHEEL; (ii<=rollIdx) && (BEP_STATUS_SUCCESS == testStatus); ii++)
                            {
                                if( wheelSpeeds[ii] < (maxRollerSpeed/2))
                                {
                                    Log( LOG_ERRORS, "%s speed below threshold (%.03f < %.03f)\n",
                                         rollerName[ii].c_str(), wheelSpeeds[ii], maxRollerSpeed/2);
                                    testStatus = BEP_STATUS_ABORT;
                                }
                            }
                        }
                    }

                    ///////////////////////////////////////////////////////
                    // Step 3:
                    //  Make sure the rolls are still turning
                    ///////////////////////////////////////////////////////
                    if(BEP_STATUS_SUCCESS == testStatus)
                    {
                        testStatus = GetWheelSpeeds( wheelSpeeds);
                        if( BEP_STATUS_SUCCESS == testStatus)
                        {
                            for( rollIdx=0; rollIdx<GetRollerCount(); rollIdx++)
                            {
                                Log( LOG_DEV_DATA, "%s speed: %.03f MPH\n", rollerName[rollIdx].c_str(), wheelSpeeds[rollIdx]);

                                // If this roller is turning too slow, assume stopped by external influence
                                if( wheelSpeeds[rollIdx] < (maxRollerSpeed/2))
                                {
                                    Log( LOG_ERRORS, "%s speed below threshold (%.03f < %.03f)\n",
                                         rollerName[rollIdx].c_str(), wheelSpeeds[rollIdx], maxRollerSpeed/2);
                                    testStatus = BEP_STATUS_ABORT;
                                }
                            }
                        }
                        else
                        {
                            Log( LOG_ERRORS, "Error reading wheel speeds: %d\n", testStatus);
                        }
                    }
                }
                catch( BepException &e)
                {
                    Log(LOG_ERRORS, "TestStepLowTorqueStartup Exception: %s\n", e.what());
                    testStatus = BEP_STATUS_SOFTWARE;
                }

                ///////////////////////////////////////////////////////
                // Step 4:
                //  Torque 0 all motors so they are in a known state
                ///////////////////////////////////////////////////////
                for( rollIdx=0; rollIdx<GetRollerCount(); rollIdx++)
                {
                    m_MotorController.Write(rollerName[rollIdx]+"TorqueValue", "0.0", false);
                    m_MotorController.Write(rollerName[rollIdx]+"MotorMode", trqModeTag, true);
                }
            }
        }
        else
        {
            testStatus = BEP_STATUS_SKIP;
        }
    }
    else
    {
        Log( LOG_ERRORS, "Low Torque Startup only supported on 3600 style electric machines\n");
        testStatus = BEP_STATUS_SKIP;
    }

    // update the test status
    UpdateResult(testStatus, status);

    // Report the results
    SendTestResult(status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepLowTorqueStartup() complete: %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string LossCompensationTC::TestStepAccelerate(void)
{
    string tag, value, response;    // communication strings
    INT32 testStatus;               // the status of the test being performed
    string status;                  // the test status returned

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepAccelerate()\n");
    if(GetLCStatus() == BEP_PASS_RESPONSE)
    {   // update the GUI with params
        int time = GetTestStepInfoInt("WarmUpTime")/1000;
        SystemWrite(GetDataTag("LossCompensationWarmupTime"), time);
        SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), time);
        SystemWrite(GetDataTag("ArmSpeed"), GetTestStepInfo("LCArmSpeed"));

        try
        {   // make sure everything is groovy with the machine
            testStatus = LossCompStatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)
            {   // command the machine into speed mode if not currently in it
                if(SystemRead(MOTOR_MODE) != LOSSCOMPENSATION_MODE)
                {
                    if(SystemCommand(MOTOR_MODE, string(LOSSCOMPENSATION_MODE)) != BEP_STATUS_SUCCESS)
                    {
                        throw BepException("TestStepAccelerate Error: Could Not Command %s, %s\n",
                                           MOTOR_MODE, LOSSCOMPENSATION_MODE);
                    }
                }

                // command the machine to arm speed
                float calibrationArmSpeed = GetTestStepInfoFloat("LCArmSpeed");

                // initialize the wheel speed targets
                string value;
                char buffer[128];
                for(UINT32 index = 0; index < GetRollerCount(); index++)
                {
                    value = "0 100";    // set the domain from 0 to 100
                    SystemWrite(GetDataTag(rollerName[index] + "WheelDomain"), value);
                    // set the value from the target - 5 to the target + 5
                    value = CreateMessage(buffer, sizeof(buffer), "%d %d", (int) (calibrationArmSpeed - 5), 
                                          (int) (calibrationArmSpeed + 5));
                    SystemWrite(GetDataTag(rollerName[index] + "WheelTarget"), value);
                }

                value = CreateMessage(buffer, sizeof(buffer), "%.2f", (calibrationArmSpeed + 3));
                if(SystemCommand(COMMAND_SPEED, value) != BEP_STATUS_SUCCESS)
                {
                    throw BepException("TestStepAccelerate Error: Could Not Send: %s, %s",
                                       COMMAND_SPEED, value.c_str());
                }
                // wait till arm speed is achieved or we timeout
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                Log(LOG_DEV_DATA, "Prompting Accelerate To Prompt Speed\n");
                //3/28/06 Added parameter to skip vehicle present to status check
                while((SpeedCheck(calibrationArmSpeed, true) != BEP_STATUS_SUCCESS) &&
                      TimeRemaining() && (testStatus == BEP_STATUS_SUCCESS))
                {   // if the machine conditions are still vaild, hang out for 1/10th of a second
                    // Check Machine Status
                    testStatus = LossCompStatusCheck();
                    BposSleep(100);
                }
                // remove all of the prompts that were sent using UpdatePrompts()
                RemovePrompts();

                // if we did not timeout or have a status error
                if(TimeRemaining())
                {   // wait the warmup amout of time before completing
                    DisplayPrompt(GetTestStepInfo("WarmupPromptBox") , GetTestStepInfo("WarmupPrompt"), 0);
                    Log(LOG_DEV_DATA, "Prompting Warmup Prompt\n");
                    string timeRemaining;
                    struct timespec currentTime, startTime;
                    double warmupTime = atol(GetTestStepInfo("WarmUpTime").c_str());
                    double elapsedTime=0;
                    bool warmupDone = false;

                    switch(testStatus)
                    {
                    case BEP_STATUS_SUCCESS:
                        // wait the warmup amout of time before completing
                        DisplayPrompt(GetTestStepInfo("WarmupPromptBox") , GetTestStepInfo("WarmupPrompt"), 0);
                        Log(LOG_DEV_DATA, "Prompting Warmup Prompt\n");

                        if(clock_gettime( CLOCK_REALTIME, &startTime) == -1)
                            throw BepException("WarmupRemaining Clock Gettime Error: %s", strerror(errno));
                        Log(LOG_DEV_DATA, "Warmup: %f\n", warmupTime);

                        // Add some tolerance for machines with large losses
                        calibrationArmSpeed -= GetParameterFloat("WarmUpSpeedTolerance");

                        while((SpeedCheck(calibrationArmSpeed, true) == BEP_STATUS_SUCCESS) &&
                              (!warmupDone) && (testStatus == BEP_STATUS_SUCCESS))
                        {   // if the machine conditions are vaild, wait, if not we are done
                            if((testStatus = LossCompStatusCheck()) == BEP_STATUS_SUCCESS)
                            {
                                if(clock_gettime( CLOCK_REALTIME, &currentTime) == -1)
                                    throw BepException("WarmupRemaining Clock Gettime Error: %s", strerror(errno));

                                // determine the amount of time that has elapsed in milli seconds
                                elapsedTime = ( currentTime.tv_sec - startTime.tv_sec ) * 1000 +
                                              ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
                                // write LCWarmupTimeRemaining
                                Log(LOG_DEV_DATA, "Warmup Time Remaining: %f > %f\n", warmupTime, elapsedTime);
                                if(elapsedTime >= warmupTime)
                                {
                                    timeRemaining = "0";
                                    SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), timeRemaining);
                                    warmupDone = true;  // if warmup complete signal done
                                }
                                else
                                {
                                    char buffer[128];
                                    timeRemaining = CreateMessage(buffer, sizeof(buffer), "%d", 
                                                                  (int) ((warmupTime - elapsedTime)/1000));
                                    SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), timeRemaining);
                                    BposSleep(100);     // else wait
                                }
                            }
                            else Log(LOG_DEV_DATA, "Warmup Saw Invalid Satus\n");
                        }
                        break;
                    case BEP_STATUS_HARDWARE:
                        Log(LOG_ERRORS, "Retaining rolls not raised (Elevators not lowered) in TestStepAccelerate\n");
                        status = BEP_SKIP_RESPONSE;
                        break;
                    case BEP_STATUS_ABORT:
                        Log(LOG_ERRORS, "LC switch turned off in  TestStepAccelerate\n");
                        status = BEP_SKIP_RESPONSE;
                        break;
                    case BEP_STATUS_FAILURE:
                        Log(LOG_ERRORS, "Vehicle present in TestStepAccelerate\n");
                        status = BEP_SKIP_RESPONSE;
                        break;
                    default:
                        Log(LOG_ERRORS, "Unknown error in TestStepAccelerate\n");
                        status = BEP_SKIP_RESPONSE;
                        break;
                    }
                    // if machine did not complete the warmup
                    if(!warmupDone)
                    {
                        status = BEP_SKIP_RESPONSE;
                        testStatus = BEP_STATUS_FAILURE;
                    }
                }
                else
                {   // if we are here it is not good, eiter test was terminated or timed out
                    testStatus = BEP_STATUS_TIMEOUT;
                    Log(LOG_DEV_DATA, "Timeout in TestStepAccelerate\n");
                }
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "TestStepAccelerate Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        UpdateResult(testStatus, status);
    }
    else status = BEP_SKIP_RESPONSE;

    // write the test step information
    SendTestResult(status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepAccelerate() complete: %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string LossCompensationTC::TestStepStaggeredAccelerate(void)
{
    string      tag, value, response;   // communication strings
    INT32       testStatus;             // the status of the test being performed
    UINT32      ii;
    float       speedDeltaForStart;     // Staggered start speed seperation
    float       wheelSpeeds[GetRollerCount()];
    float       trqStartupVal, maxTrqModeSpeed;
    const bool  checkSensorCross = GetTestStepInfoBool( "CheckSensorCross");
    string      status;                 // the test status returned

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepStaggeredAccelerate()\n");

    // Make sure this is an electric machine
    if(ReadSubscribeData(GetDataTag("MachineType3600")) == "1")
    {
        if(GetLCStatus() == BEP_PASS_RESPONSE)
        {
            testStatus = LossCompStatusCheck();
            if(testStatus == BEP_STATUS_SUCCESS)
            {
                // Make sure all torque and speed commands are set to 0
                for( ii=LFWHEEL; ii<(GetRollerCount()-1); ii++)
                {
                    m_MotorController.Write(rollerName[ii]+"TorqueValue", "0", false);
                    m_MotorController.Write(rollerName[ii]+"SpeedValue", "0", false);
                }
                m_MotorController.Write(rollerName[GetRollerCount()-1]+"TorqueValue", "0", false);
                m_MotorController.Write(rollerName[GetRollerCount()-1]+"SpeedValue", "0", true);

                // update the GUI with params
                int time = GetTestStepInfoInt("WarmUpTime")/1000;
                SystemWrite(GetDataTag("LossCompensationWarmupTime"), time);
                SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), time);
                SystemWrite(GetDataTag("ArmSpeed"), GetTestStepInfo("LCArmSpeed"));

                // wait till arm speed is achieved or we timeout
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");

                try
                {   // make sure everything is groovy with the machine
                    testStatus = LossCompStatusCheck();
                    if(testStatus == BEP_STATUS_SUCCESS)
                    {
                        // command the machine to arm speed
                        float calibrationArmSpeed = GetTestStepInfoFloat("LCArmSpeed");

                        // initialize the wheel speed targets
                        string value;
                        char buffer[128];
                        for(UINT32 index = 0; index < GetRollerCount(); index++)
                        {
                            value = "0 100";    // set the domain from 0 to 100
                            SystemWrite(GetDataTag(rollerName[index] + "WheelDomain"), value);
                            // set the value from the target - 5 to the target + 5
                            value = CreateMessage(buffer, sizeof(buffer), "%d %d", 
                                                  (int) (calibrationArmSpeed - 5), (int) (calibrationArmSpeed + 5));
                            SystemWrite(GetDataTag(rollerName[index] + "WheelTarget"), value);
                        }

                        // Check if we have a low speed startup torque value
                        trqStartupVal = GetTestStepParameter( "StartupTorqueCommand", trqStartupVal);
                        if( trqStartupVal > 0)
                        {
                            // Read the max speed we want the rollers to go in torque mode (default to 3 MPH)
                            maxTrqModeSpeed = GetTestStepParameter( "MaxTorqueModeSpeed", maxTrqModeSpeed, 3);

                            Log( LOG_DEV_DATA, "Low torque startup value=%.03f ft-lbs (up to %.03f MPH)\n",
                                 trqStartupVal, maxTrqModeSpeed);
                        }
                        else
                        {
                            Log( LOG_DEV_DATA, "No low torque startup parameter present...skipping low torque startup\n");
                        }

                        value = CreateMessage(buffer, sizeof(buffer), "%.2f", (calibrationArmSpeed + 3));

                        // Stagger the motors to start at 1/4 arm speed
                        speedDeltaForStart = calibrationArmSpeed / GetRollerCount();
                        if( speedDeltaForStart < 1) speedDeltaForStart = 1;

                        // Stagger start the motors
                        for( ii=LFWHEEL; (ii<GetRollerCount()) && (BEP_STATUS_SUCCESS == testStatus); ii++)
                        {
                            // Command the next roller to speed
                            const std::string speedValue( rollerName[ ii] + "SpeedValue");
                            const std::string motorMode( rollerName[ ii] + "MotorMode");

                            // Restart our test step timeout
                            SetStartTime();

                            // If we want to perform a low torque startup
                            if( trqStartupVal > 0)
                            {
                                INT32 timeOut = GetTestStepInfoInt( "Timeout");
                                testStatus = LowTorqueStartup( ii, trqStartupVal, maxTrqModeSpeed, timeOut);
                                if( TimeRemaining())    SetStartTime();
                            }
                            else
                            {
                                testStatus = LossCompStatusCheck();
                            }

                            if( BEP_STATUS_SUCCESS == testStatus)
                            {
                                Log(LOG_DEV_DATA, "Commanding %s to %s\n", speedValue.c_str(), value.c_str());

                                // Set the speed value first
                                m_MotorController.Write(speedValue, value, true);
                                // Command speed mode second
                                m_MotorController.Write(motorMode, SPEED_MODE, true);

                                if(GetWheelSpeeds(wheelSpeeds) == BEP_STATUS_SUCCESS)
                                {
                                    // Wait for this wheel speed to get above the staggered atsrt threshold
                                    while( (wheelSpeeds[ ii] < speedDeltaForStart) && TimeRemaining() &&
                                           (testStatus == BEP_STATUS_SUCCESS))
                                    {
                                        // if the machine conditions are still vaild, hang out for 1/10th of a second
                                        testStatus = StatusSleep(100);
                                        if(GetWheelSpeeds(wheelSpeeds) != BEP_STATUS_SUCCESS)
                                        {
                                            throw BepException("TestStepStaggeredAccelerate Error: could not read wheel speeds");
                                        }

                                        // If we need to check or sensor cross
                                        if( true == checkSensorCross)
                                        {
                                            // Check for crossed encoders
                                            for(UINT32 jj=ii+1; jj<GetRollerCount(); jj++)
                                            {
                                                // If any of the non-commanded wheels have a speed greater than
                                                // the most recently commanded wheel
                                                if( wheelSpeeds[ jj] > wheelSpeeds[ ii])
                                                {
                                                    // Report 2 individual faults to make translations
                                                    //  on the GUI easier
                                                    std::string faultName, faultDesc;
                                                    faultName = rollerName[ ii] + std::string("EncoderCross");
                                                    faultDesc = rollerName[ ii] + std::string("Encoder crossed");
                                                    SetFault( faultName, faultDesc);

                                                    faultName = rollerName[ jj] + std::string("EncoderCross");
                                                    faultDesc = rollerName[ jj] + std::string("Encoder crossed");
                                                    SetFault( faultName, faultDesc);
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    throw BepException("TestStepStaggeredAccelerate Error: could not read wheel speeds\n");
                                }
                            }
                            else
                            {
                                Log( LOG_ERRORS, "Low Torque Stertup error detected..disabling all drives\n");
                                Disengage();
                            }
                        }

                        Log(LOG_DEV_DATA, "Prompting Accelerate To Prompt Speed\n");
                        while((SpeedCheck(calibrationArmSpeed, true) != BEP_STATUS_SUCCESS) &&
                              TimeRemaining() && (BEP_STATUS_SUCCESS == testStatus))
                        {
                            // if the machine conditions are still vaild, hang out for 1/10th of a second
                            testStatus = StatusSleep( 100);
                        }

                        // if we did not timeout or have a status error
                        if( (TimeRemaining()) && (BEP_STATUS_SUCCESS == testStatus))
                        {   // wait the warmup amout of time before completing
                            DisplayPrompt(GetTestStepInfo("WarmupPromptBox") , GetTestStepInfo("WarmupPrompt"), 0);
                            Log(LOG_DEV_DATA, "Prompting Warmup Prompt\n");
                            string timeRemaining;
                            struct timespec currentTime, startTime;
                            double warmupTime = atol(GetTestStepInfo("WarmUpTime").c_str());
                            double elapsedTime=0;
                            bool warmupDone = false;

                            switch(testStatus)
                            {
                            case BEP_STATUS_SUCCESS:
                                // wait the warmup amout of time before completing
                                DisplayPrompt(GetTestStepInfo("WarmupPromptBox") , GetTestStepInfo("WarmupPrompt"), 0);
                                Log(LOG_DEV_DATA, "Prompting Warmup Prompt\n");

                                if(clock_gettime( CLOCK_REALTIME, &startTime) == -1)
                                    throw BepException("WarmupRemaining Clock Gettime Error: %s", strerror(errno));
                                Log(LOG_DEV_DATA, "Warmup: %f\n", warmupTime);

                                while((SpeedCheck(calibrationArmSpeed, true) == BEP_STATUS_SUCCESS) &&
                                      (!warmupDone) && (testStatus == BEP_STATUS_SUCCESS))
                                {   // if the machine conditions are vaild, wait, if not we are done
                                    if((testStatus = LossCompStatusCheck()) == BEP_STATUS_SUCCESS)
                                    {
                                        if(clock_gettime( CLOCK_REALTIME, &currentTime) == -1)
                                            throw BepException("WarmupRemaining Clock Gettime Error: %s", strerror(errno));

                                        // determine the amount of time that has elapsed in milli seconds
                                        elapsedTime = ( currentTime.tv_sec - startTime.tv_sec ) * 1000 +
                                                      ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
                                        // write LCWarmupTimeRemaining
                                        Log(LOG_DEV_DATA, "Warmup Time Remaining: %f > %f\n", warmupTime, elapsedTime);
                                        if(elapsedTime >= warmupTime)
                                        {
                                            timeRemaining = "0";
                                            SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), timeRemaining);
                                            warmupDone = true;  // if warmup complete signal done
                                        }
                                        else
                                        {
                                            char buffer[128];
                                            timeRemaining = CreateMessage(buffer, sizeof(buffer), "%d", 
                                                                          (int) ((warmupTime - elapsedTime)/1000));
                                            SystemWrite(GetDataTag("LossCompensationWarmupTimeRemaining"), timeRemaining);
                                            BposSleep(100);     // else wait
                                        }
                                    }
                                    else Log(LOG_DEV_DATA, "Warmup Saw Invalid Satus\n");
                                }
                                break;
                            case BEP_STATUS_HARDWARE:
                                Log(LOG_ERRORS, 
                                    "Retaining rolls not raised (Elevators not lowered) in TestStepStaggeredAccelerate\n");
                                status = BEP_SKIP_RESPONSE;
                                break;
                            case BEP_STATUS_ABORT:
                                Log(LOG_ERRORS, "LC switch turned off in  TestStepStaggeredAccelerate\n");
                                status = BEP_SKIP_RESPONSE;
                                break;
                            case BEP_STATUS_FAILURE:
                                Log(LOG_ERRORS, "Vehicle present in TestStepStaggeredAccelerate\n");
                                status = BEP_SKIP_RESPONSE;
                                break;
                            default:
                                Log(LOG_ERRORS, "Unknown error in TestStepStaggeredAccelerate\n");
                                status = BEP_SKIP_RESPONSE;
                                break;
                            }
                            // if machine did not complete the warmup
                            if(!warmupDone)
                            {
                                status = BEP_SKIP_RESPONSE;
                                testStatus = BEP_STATUS_FAILURE;
                            }
                        }
                        else if(BEP_STATUS_SUCCESS == testStatus)
                        {
                            // if we are here it is not good, eiter test was terminated or timed out
                            testStatus = BEP_STATUS_TIMEOUT;
                            Log(LOG_DEV_DATA, "Timeout in TestStepStaggeredAccelerate\n");
                        }
                    }
                }
                catch(BepException &e)
                {
                    // Make sure all torque and speed commands are set to 0
                    for( ii=LFWHEEL; ii<(GetRollerCount()-1); ii++)
                    {
                        m_MotorController.Write(rollerName[ii]+"TorqueValue", "0", false);
                        m_MotorController.Write(rollerName[ii]+"SpeedValue", "0", false);
                    }
                    m_MotorController.Write(rollerName[GetRollerCount()-1]+"TorqueValue", "0", false);
                    m_MotorController.Write(rollerName[GetRollerCount()-1]+"SpeedValue", "0", true);

                    Log(LOG_ERRORS, "TestStepStaggeredAccelerate Exception: %s\n", e.what());
                    testStatus = BEP_STATUS_SOFTWARE;
                }
                // remove all of the prompts that were sent using UpdatePrompts()
                RemovePrompts();
            }
            else
            {
                Log( LOG_ERRORS, "StatusCHeck failed: %d\n", testStatus);
            }
        }
        else
        {
            testStatus = BEP_STATUS_SKIP;
        }
        // update the test status
        UpdateResult(testStatus, status);

        // write the test step information
        SendTestResult(status, GetTestStepInfo("Description"));
    }
    else
    {
        Log( LOG_ERRORS, "Staggered Accelerate only supported on 3600 style electric machines\n");
        status = TestStepAccelerate();
    }

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepStaggeredAccelerate() complete: %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string LossCompensationTC::TwoMotorAccelerate(void)
{   // Make sure this is the correct style machine
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "LossCompensationTC::TwoMotorAccelerate() - Enter");
    // Make sure the rollers are up before continuing
    if(SystemReadBool(ROLLS_UP_DATA_TAG))
    {
        if(!SystemRead(MACHINE_TYPE).compare("3700"))
        {   // Make sure the speed set points are set to 0 and place the motor controller into speed mode
            m_MotorController.Write("LeftFrontSpeedValue", "0", false);
            m_MotorController.Write("RightFrontSpeedValue", "0", false);
            m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
            // Force to Rear Wheel Driver so the rear will spin first
            SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
            BposSleep(1000);   //Wait for the relay to switch
            float calSpeed = GetTestStepInfoFloat("LCArmSpeed");
            float calibrationArmSpeed = calSpeed + GetParameterFloat("TwoMotorAccelerateOffset");
            char buff[16];
            string armSpeed(CreateMessage(buff, sizeof(buff), "%.2f", calibrationArmSpeed));
            float wheelSpeeds[GetRollerCount()];
            // Command each roller to speed
            for(INT32 roller = RRWHEEL; roller >= LFWHEEL; roller--)
            {   // Command one roller at a time
                string rollName;
                switch(roller)
                {
                case LFWHEEL:
                case LRWHEEL:
                    rollName = rollerName[0];
                    break;

                case RFWHEEL:
                case RRWHEEL:
                    rollName = rollerName[1];
                    break;

                default:
                    Log(LOG_ERRORS, "Unknown rollername, default to %s", rollerName[0].c_str());
                    rollName = rollerName[0];
                    break;
                }
                Log(LOG_DEV_DATA, "Commanding %s roller to %s", rollName.c_str(), armSpeed.c_str());
                // Check if the drive axle should be switched
                if(RFWHEEL == roller)
                {   // Switch the drive axle - set to zero speed first so we do not trip the drives
                    m_MotorController.Write("LeftFrontSpeedValue", "0", false);
                    m_MotorController.Write("RightFrontSpeedValue", "0", true);
                    BposSleep(2000);
                    SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
                    BposSleep(1000);   //Wait for the relay to switch
                }
                m_MotorController.Write(rollName+"SpeedValue", armSpeed, true);
                // Wait for the roller to achieve arm speed before moving to the next roller
                do
                {   // Wait for the motor to spin up to speed
                    BposSleep(1000);
                    GetWheelSpeeds(wheelSpeeds);
                } while((wheelSpeeds[roller] < (calibrationArmSpeed-GetParameterFloat("WarmUpSpeedTolerance"))) && 
                        TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
                Log(LOG_DEV_DATA, "Done waiting for %s roller to reach arm speed: %s - TimeRemaining: %s, StatusCheck: %s",
                    rollerName[roller].c_str(), armSpeed.c_str(), TimeRemaining() ? "True" : "False", 
                    ConvertStatusToResponse(StatusCheck()).c_str());
            }
            // Set the result
            Log(LOG_DEV_DATA, "Determining test result");
            GetWheelSpeeds(wheelSpeeds);
            bool wheelSpeedsGood = true;
            for(UINT8 index = LFWHEEL; (index < GetRollerCount()) && wheelSpeedsGood; index++)
            {
                wheelSpeedsGood = (wheelSpeeds[index] >= calSpeed); 
                Log(LOG_DEV_DATA, "Checking %s wheel speed: %.2f > %.2f - %s", 
                    rollerName[index].c_str(), wheelSpeeds[index], calSpeed, wheelSpeedsGood ? "Good" : "Not Good");
            }
            testResult = wheelSpeedsGood ? testPass : testFail;
            // Make sure the component knows the switch is on  (there really is no switch, but the component looks for it 
            // in the status check.
            SetData(GetDataTag("LossCompensationSwitchStatus"), "1");
        }
        else
        {   // Wrong type of machine, abort the sequence
            Log(LOG_ERRORS, "Wrong machine type for 2 motor accelerate, aborting!!");
            testResult = testAbort;
        }
    }
    else
    {
        testResult = testFail;
        Log(LOG_ERRORS, "Not accelerating the rollers - rollers are not in the testing position!");
    }
    // Report the result
    SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    Log(LOG_FN_ENTRY, "LossCompensationTC::TwoMotorAccelerate() - Exit");
    return testResult;
}

//=============================================================================
const string LossCompensationTC::TestStepLossCompensation(void)
{
    INT32 testStatus;               // the status of the test being performed
    string status;                  // the test status returned

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepLossCompensation()\n");
    Log(LOG_DEV_DATA, "Current LC Status: %s", GetLCStatus().c_str());
    if(((GetLCStatus() == BEP_PASS_RESPONSE) || (GetLCStatus() == BEP_TESTING_RESPONSE)) && (!CheckAbort()))
    {   // update the GUI
        SystemWrite(GetDataTag("StartSampleSpeed"), GetTestStepInfo("StartSampleSpeed"));
        SystemWrite(GetDataTag("EndSampleSpeed"), GetTestStepInfo("StopSampleSpeed"));
        SystemWrite(GetDataTag("MachineLossMaxLeft"), GetTestStepInfo("MachineLossMaxLeft"));
        SystemWrite(GetDataTag("MachineLossMinLeft"), GetTestStepInfo("MachineLossMinLeft"));
        SystemWrite(GetDataTag("MachineLossMaxRight"), GetTestStepInfo("MachineLossMaxRight"));
        SystemWrite(GetDataTag("MachineLossMinRight"), GetTestStepInfo("MachineLossMinRight"));
        SystemWrite(GetDataTag("MachineLossMaxDelta"), GetTestStepInfo("MachineLossDiffMax"));

        try
        {   // display prompts
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // disengage the system
            testStatus = Disengage();
            if(testStatus == BEP_STATUS_SUCCESS)
            {   // sample the forces and return the indexes
                int firstTag, secondTag;
                testStatus = SampleForces(firstTag, secondTag);
                if(testStatus == BEP_STATUS_SUCCESS)
                {   // calculate the losses
                    testStatus = CalculateResults(GetParameter("IcmForceArray"), firstTag, secondTag);
                    Timestamp();    // timestamp the results
                    if(testStatus != BEP_STATUS_SUCCESS)
                        Log(LOG_ERRORS, "TestStepLossCompensation Did Not Pass: %d\n", testStatus);
                }
                else Log(LOG_ERRORS, "TestStepLossCompensation Could Not Sample The Wheel Forces\n");
            }
            else Log(LOG_ERRORS, "TestStepLossCompensation Could Not Command The Machine To Coast\n");
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "TestStepLossCompensation Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // remove all of the prompts that were sent using UpdatePrompts()
        RemovePrompts();

        // update the test status
        UpdateResult(testStatus, status);
    }
    else
    {   // disengage the system
        testStatus = Disengage();
        status = BEP_SKIP_RESPONSE;
    }
    // if an abort was received
    if(CheckAbort())    status = BEP_ABORT_RESPONSE;
    // write the test step information
    SendTestResult(status, GetTestStepInfo("Description"));
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepLossCompensation() complete: %s\n", status.c_str());
    return(status);
}

//=============================================================================
const string LossCompensationTC::TestStepStop(void)
{
    INT32 testStatus;   // the status of the test
    INT32 zeroSpeedCheckDelay = GetParameterInt("ZeroSpeedCheckDelay");

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepStop()\n");

    // Make sure this is an electric machine
    if((SystemRead(GetDataTag("MachineType3600")) == "1") ||
       (SystemRead(GetDataTag("MachineType")) == GetDataTag("MachineType3600Value")))
    {
        if(UpdatePrompts() != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Unable to Update Prompts\n");
        try
        {
            // change the motor mode to quick brake mode
            testStatus = m_MotorController.Write(MOTOR_MODE, QUICK_BRAKE_MODE, true);

            if( BEP_STATUS_SUCCESS == testStatus)
            {
                // wait for rolls to go to zerospeed
                Log(LOG_DEV_DATA, "TestStepStop Waiting For Zerospeed\n");
                while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
                {
                    BposSleep (zeroSpeedCheckDelay != 0 ? zeroSpeedCheckDelay : 1000);
                }
                // check the results
                if(!TimeRemaining())                            // if timeout
                    testStatus = BEP_STATUS_TIMEOUT;
                else if(ReadSubscribeData(GetDataTag("Zerospeed")) == "0")  // else if not at Zerospeed
                    testStatus = BEP_STATUS_FAILURE;
                else
                    testStatus = BEP_STATUS_SUCCESS;
            }
            else
            {
                testStatus =  BEP_STATUS_SOFTWARE;
                Log(LOG_ERRORS, "Could Not Command Rolls To Stop: %d\n", testStatus);
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "TestStepStop Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }
        // remove all of the prompts that were sent using UpdatePrompts()
        RemovePrompts();
    }
    else    // not 3600 so pass
        testStatus = BEP_STATUS_SUCCESS;

    // if an abort was received
    if(CheckAbort())    testStatus = BEP_STATUS_ABORT;

    // update the test status
    string status;
    UpdateResult(testStatus, status);

    // write the test step information
    SendTestResult(status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepStop() complete: %s\n", status.c_str());
    return(status);
}

//-------------------------------------------------------------------------------------------------
const string LossCompensationTC::CoastToStop(void)
{
    string result(BEP_TESTING_STATUS);
    float rollerSpeeds[GetRollerCount()];
    bool coastComplete = false;
    float fastestSpeed = 0.0;
    bool promptDisplayed = false;
    Log(LOG_FN_ENTRY, "Waiting for rollers to coast to stop");
    do
    {   // Get the roller speeds and find the fastest roller
        if(GetWheelSpeeds(rollerSpeeds) == BEP_STATUS_SUCCESS)
        {   // Find the fastest speed for this iteration
            fastestSpeed = 0.0;
            for(unsigned int index = 0; index < GetRollerCount(); index++)
            {
                if(rollerSpeeds[index] > fastestSpeed)
                {
                    fastestSpeed = rollerSpeeds[index];
                }
            }
            // if fastest speed is above threshold, do not exit
            coastComplete = fastestSpeed < GetParameterFloat("RollsDownSpeed");
            Log(LOG_DEV_DATA, "Coast down complete: %s - roller speed: %.02f, coast complete speed: %s",
                coastComplete ? "True" : "False", fastestSpeed, GetParameter("RollsDownSpeed").c_str());
            if(!coastComplete)  BposSleep(500);
            if(!coastComplete && !promptDisplayed)
            {
                promptDisplayed = true;
                DisplayPrompt(GetPromptBox("CoastToStop"), GetPrompt("CoastToStop"), GetPromptPriority("CoastToStop"));
            }
        }
    } while(!coastComplete && TimeRemaining());
    // Make sure to remove the prompt if it has been displayed
    if(promptDisplayed)
    {
        RemovePrompt(GetPromptBox("CoastToStop"), GetPrompt("CoastToStop"), GetPromptPriority("CoastToStop"));
    }
    // Return the result
    return testPass;
}

//=============================================================================
const string LossCompensationTC::TestStepFinishUp(void)
{
    string tag, value, response;            // communication strings

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepFinishUp()\n");
    try
    {   // write the test step information
        SendTestResult(testPass, GetTestStepInfo("Description"));
        // send the overall result
        SendOverallResult(testPass);
        // set the brake status test to complete
        SetStatus(BEP_COMPLETE_STATUS);

        // make sure everything is groovy
        // display prompts
        if(UpdatePrompts() != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Unable to Update Prompts\n");

        // 04/04/2005 Removed code that caused a 'Drive Curve Error' when lowering of the rolls
        // after the test was done and before the rolls reached zero speed
        // disengage the system
        while((Disengage() != BEP_STATUS_SUCCESS) && TimeRemaining())
        {
            Log(LOG_ERRORS, "ERROR: Unable disengage the roll motors\n");
            BposSleep(200);
        }

        // change the motor mode back to boost mode
        SystemCommand(MOTOR_MODE, string(BOOST_MODE));
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "TestStepFinishUp Exception: %s\n", e.what());
    }
    // remove all of the prompts that were sent using UpdatePrompts()
    RemovePrompts();

    Log(LOG_FN_ENTRY, "LossCompensationTC::TestStepFinishUp() complete\n");

    return(testPass);
}

//=============================================================================
INT32 LossCompensationTC::LowTorqueStartup( INT32 rollIdx, float torqueVal, float maxSpeed, INT32 timeOut)
{
    const std::string   driveName( rollerName[rollIdx]);
    const std::string   driveTrqValTag( driveName + "TorqueValue");
    const std::string   driveModeTag( driveName + "MotorMode");
    const std::string   trqModeTag( TORQUE_MODE);
    float               wheelSpeeds[GetRollerCount()];
    char                cmdVal[256];
    INT32               testStatus = BEP_STATUS_SUCCESS;

    Log(LOG_FN_ENTRY, "Enter LossCompensationTC::LowTorqueStartup(%d, %.03f, %.02f, %d)\n",
        rollIdx, torqueVal, maxSpeed, timeOut);

    // Make sure this is an electric machine
    if(ReadSubscribeData(GetDataTag("MachineType3600")) == "1")
    {
        memset( wheelSpeeds, 0, GetRollerCount()*sizeof( float));

        // Send the initial torque mode command to this roller, making
        //  sure the torque value gets set before the mode AND that
        //  we are commanding forward motion
        ////////////////////////////////////////////////////////////////////
        if( torqueVal < 0)  sprintf( cmdVal, "%.03f", torqueVal);
        else                sprintf( cmdVal, "-%.03f", torqueVal);
        Log( LOG_DEV_DATA, "Commanding %s to %s @ %s\n", driveTrqValTag.c_str(),
             driveModeTag.c_str(), cmdVal);

        m_MotorController.Write( driveTrqValTag, cmdVal, true);
        m_MotorController.Write( driveModeTag, trqModeTag, true);

        // Restart our timeout for each roller
        while( (timeOut > 0) && (BEP_STATUS_SUCCESS == testStatus) && (wheelSpeeds[rollIdx] < maxSpeed))
        {
            // Make sure none of the rollers exceeds our desired max speed
            testStatus = GetWheelSpeeds( wheelSpeeds);
            if(BEP_STATUS_SUCCESS == testStatus)
            {
                Log( LOG_DEV_DATA, "LF: %.03f, RF: %.03f, LR: %.03f, RR: %.03f\n",
                     wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
                     wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);

                testStatus = StatusSleep( 200);
                timeOut -= 200;

                Log( LOG_DEV_DATA, "Time Remaining: %d\n", timeOut);
            }
            else
            {
                Log( LOG_ERRORS, "Error reading wheel speed values: %d\n", testStatus);
            }
        }

        //  Make sure the commanded rolls are not being restrained
        if( BEP_STATUS_SUCCESS == testStatus)
        {
            testStatus = GetWheelSpeeds( wheelSpeeds);
            if(BEP_STATUS_SUCCESS == testStatus)
            {
                if( wheelSpeeds[rollIdx] < maxSpeed)
                {
                    Log( LOG_ERRORS, "%s speed below threshold (%.03f < %.03f)\n",
                         driveName.c_str(), wheelSpeeds[rollIdx], maxSpeed/2);
                    testStatus = BEP_STATUS_ABORT;
                }
            }
            else
            {
                Log( LOG_ERRORS, "Error reading final wheel speed values: %d\n", testStatus);
            }
        }
    }
    else
    {
        Log( LOG_ERRORS, "LowTorqueStartup only supported on 3600 style electric machines\n");
        testStatus = BEP_STATUS_SUCCESS;
    }

    Log(LOG_FN_ENTRY, "Exit LossCompensationTC::LowTorqueStartup(%d, %.03f, %.02f, %d), status=%d\n",
        rollIdx, torqueVal, maxSpeed, timeOut, testStatus);

    return( testStatus);
}

//=============================================================================
INT32 LossCompensationTC::LossCompStatusCheck(void)
{
    INT32 testStatus = GenericTC::StatusCheck();
    string faultDescription = "";
    if(testStatus == BEP_STATUS_SUCCESS)
    {
        if(ReadSubscribeData(GetDataTag("VehiclePresent")) == "0" || GetParameterBool("SkipVehiclePresentDuringAccelerate"))
        {   /* vehicle not present or not checking*/
            if(ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == "1")
            {   /* loss compensation switch on */
                if(ReadSubscribeData(GetDataTag("RaiseRetainers")) == "1")
                {
                    testStatus = BEP_STATUS_SUCCESS;
                }
                else
                {
                    testStatus  = BEP_STATUS_HARDWARE;
                    faultDescription = GetDataTag("FaultRetainers");
                }
            }
            else
            {
                testStatus  = BEP_STATUS_ABORT;
                faultDescription = GetDataTag("FaultSwitch");
            }
        }
        else
        {
            testStatus  = BEP_STATUS_FAILURE;
            faultDescription = GetDataTag("FaultVehiclePresent");
        }
        if(testStatus != BEP_STATUS_SUCCESS)
        {
            SendSubtestResult(GetTestStepName() + "StatusCheck",testStatus, faultDescription);
        }
        Log(LOG_DEV_DATA, "Machine Status: %d (%s, %s, %s)\n", testStatus, 
            ReadSubscribeData(GetDataTag("VehiclePresent").c_str()).c_str(),
            ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")).c_str(), 
            ReadSubscribeData(GetDataTag("RaiseRetainers")).c_str());
    }

    Log(LOG_FN_ENTRY, "Status Check: %d\n", testStatus);

    if(testStatus != BEP_STATUS_SUCCESS) Disengage();   // If it is not ok to go on, disable

    return(testStatus);
}

#if 0
//=============================================================================
INT32 LossCompensationTC::GetIForces(float wheelForce[])
{
    INT32 status = BEP_STATUS_SUCCESS;

    try
    {   // clear out the wheel forces
        memset(wheelForce, 0, (sizeof(float) * GetRollerCount()));

        // read out all of the wheel force info
        string value, response;
        for(UINT32 index = 0; (index < GetRollerCount()) && (status == BEP_STATUS_SUCCESS); index++)
            status = m_ndb->Read(GetDataTag(rollerName[index] + "Force") , response, (index == (GetRollerCount()-1)));

        // get the values of the wheel force
        for(UINT32 index = 0; (index < GetRollerCount()) && (status == BEP_STATUS_SUCCESS); index++)
        {
            status = m_ndb->GetByTag(GetDataTag(rollerName[index] + "Force") , value, response);
            if(status == BEP_STATUS_SUCCESS)
                wheelForce[index] = atof(value.c_str());
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "GetIForces Exception: %s\n", e.what());
        status = BEP_STATUS_ERROR;
    }

    return(status);
}

//=============================================================================
INT32 LossCompensationTC::GetISpeeds(float wheelSpeed[])
{
    INT32 status = BEP_STATUS_SUCCESS;

    try
    {   // clear out the wheel forces
        memset(wheelSpeed, 0, (sizeof(float) * GetRollerCount()));

        // read out all of the wheel force info
        string tag, response, value;
        for(UINT32 index = 0; (index < GetRollerCount()) && (status == BEP_STATUS_SUCCESS); index++)
        {
            tag = GetDataTag(rollerName[index] + "Speed");
            Log(LOG_DEV_DATA, "Reading: %s\n", tag.c_str());
            status = m_ndb->Read(tag, response, (index == (GetRollerCount()-1)));
        }
        // get the values of the wheel force
        for(UINT32 index = 0; (index < GetRollerCount()) && (status == BEP_STATUS_SUCCESS); index++)
        {
            tag = GetDataTag(rollerName[index] + "Speed");
            status = m_ndb->GetByTag(tag, value, response);
            if(status == BEP_STATUS_SUCCESS)
            {
                wheelSpeed[index] = atof(value.c_str());
                Log(LOG_DEV_DATA, "Wheel %d: %f\n", index, wheelSpeed[index]);
            }
            else
                Log(LOG_ERRORS, "Could Not Read Speed For Wheel: %s\n", tag.c_str());
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "GetISpeeds Exception: %s\n", e.what());
        status = BEP_STATUS_ERROR;
    }

    return(status);
}
#endif 

//=============================================================================
INT32 LossCompensationTC::SpeedCheck(float speed, bool faster)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    float wheelSpeed[GetRollerCount()];
    if(GetWheelSpeeds(wheelSpeed) == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "SpeedCheck %.2f, WheelSpeeds: \n", speed);
        bool testCondition=true;
#if 1
        // Determine if the wheel speeds are in range
        for(UINT32 index = 0; (index < GetRollerCount()) && (testCondition == true); index++)
        {
            testCondition = faster ? (wheelSpeed[index] >= speed) : (wheelSpeed[index] < speed);
            Log(LOG_DEV_DATA, "%.2f, %d, \n", wheelSpeed[index], testCondition);
        }
        // Determine the test result
        testStatus = testCondition ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
#else
        if(faster)  // if a check for faster than speed
        {
            for(UINT32 index = 0; (index < GetRollerCount()) && (testCondition == true); index++)
            {
                testCondition = (wheelSpeed[index] >= speed);
                Log(LOG_DEV_DATA, "%.2f, %d, \n", wheelSpeed[index], testCondition);
            }
        }
        else        // else if a check for slower than speed
        {
            for(UINT32 index = 0; (index < GetRollerCount()) && (testCondition == true); index++)
            {
                testCondition = (wheelSpeed[index] < speed);
                Log(LOG_DEV_DATA, "%.2f, %d, \n", wheelSpeed[index], testCondition);
            }
        }
        if(testCondition == true)
            testStatus = BEP_STATUS_SUCCESS;
        else
            testStatus = BEP_STATUS_FAILURE;
#endif
    }
    else
    {
        Log(LOG_ERRORS, "Could Not Read Wheel Speeds\n");
        testStatus = BEP_STATUS_ERROR;
    }
    return(testStatus);
}

//=============================================================================
INT32 LossCompensationTC::CalculateResults(const std::string &array, int firstTag, int secondTag)
{
    Log(LOG_FN_ENTRY, "Calculate Results( %s, %d, %d)\n", array.c_str(), firstTag, secondTag);
    // read in the data from the brake force array
    INT32 testStatus = ReadDataArrays(array, firstTag, secondTag, LossCompensationTC::m_wheelForceArray);
    // if all of the arrays read successfully
    INT32 testResult;
    if(testStatus == BEP_STATUS_SUCCESS)
    {   // iterate throught the wheels and evaluate
        float average;  // the force from the array that was analyzed
        for(UINT32 roller = 0; roller < GetRollerCount(); roller++)
        {   // calculate the average forces and check for runout
            testResult = AnalyzeForces(roller, average);
            if(testResult == BEP_STATUS_SUCCESS)
            {
                testResult = ValidateLoss(roller, average, m_previousLosses[roller]);
            }
            // else display fail for the roller loss analysis
            else
            {
                SystemWrite(GetDataTag(rollerName[roller] + "CurrentLossBGColor"), std::string("red"));
            }
            // update the status if there was a failure
            if(testResult != BEP_STATUS_SUCCESS)
                testStatus = testResult;
        }
    }
    else
        Log(LOG_ERRORS, "Error Reading Force Arrays\n");

    Log(LOG_FN_ENTRY, "Calculate Results() done: %d\n", testStatus);

    return(testStatus);
}

//=============================================================================
INT32 LossCompensationTC::ValidateLoss(int wheel, float loss, float previousLoss)
{
    INT32 testStatus = BEP_STATUS_NA;
    std::string color = "green";
    Log(LOG_FN_ENTRY, "Validating Loss: %d, %f, %f\n", wheel, loss, previousLoss);

    // check if the loss is greater than the max difference
    float maxLossDiff = GetTestStepInfoFloat("MachineLossDiffMax");
    if(fabs(loss - previousLoss) > maxLossDiff)
    {   // update the fault server and the system
        testStatus = BEP_STATUS_FAILURE;
        color = "red";
        SetFault(GetDataTag(rollerName[wheel] + "MaxLossDiff"), rollerName[wheel] + " Maximum Loss Difference Exceeded");
        SystemWrite(GetDataTag(rollerName[wheel] + "MaxLossDiff"), true);
    }
    else
    {   // update the fault server and the system
        ClearFault(GetDataTag(rollerName[wheel] + "MaxLossDiff"));
        SystemWrite(GetDataTag(rollerName[wheel] + "MaxLossDiff"), false);
    }

    // read the maximum and minimum allowable machine losses
    float maxLoss, minLoss;
    if(wheel % 2)   // if a right wheel
    {
        maxLoss = GetTestStepInfoFloat("MachineLossMaxRight");
        minLoss = GetTestStepInfoFloat("MachineLossMinRight");
        SystemWrite("LCMachineLossMaxRight", maxLoss);
        SystemWrite("LCMachineLossMinRight", minLoss);
    }
    else            // must be a left wheel
    {
        maxLoss = GetTestStepInfoFloat("MachineLossMaxLeft");
        minLoss = GetTestStepInfoFloat("MachineLossMinLeft");
        SystemWrite("LCMachineLossMaxLeft", maxLoss);
        SystemWrite("LCMachineLossMinLeft", minLoss);
    }
    // if the loss is greater than the max loss
    if(loss > maxLoss)
    {   // update the fault server and the system
        testStatus = BEP_STATUS_FAILURE;
        color = "red";
        SetFault(GetDataTag(rollerName[wheel] + "MachLossOutOfLimits"), 
                 rollerName[wheel] + " Greater Than Maximum Allowable Loss");
        SystemWrite(GetDataTag(rollerName[wheel] + "MaxLossDiff"), true);
    }
    // else if less than the minimum loss
    else if(loss < minLoss)
    {   // update the fault server and the system
        testStatus = BEP_STATUS_FAILURE;
        color = "red";
        SetFault(GetDataTag(rollerName[wheel] + "MachLossOutOfLimits"), rollerName[wheel] + " Less Than Minimum Allowable Loss");
        SystemWrite(GetDataTag(rollerName[wheel] + "MaxLossDiff"), true);
    }
    // else set all good
    else
    {   // update the fault server and the system
        ClearFault(GetDataTag(rollerName[wheel] + "MachLossOutOfLimits") + "MaxLossDiff");
        SystemWrite(GetDataTag(rollerName[wheel] + "MachLossOutOfLimits"), false);
    }

    // determine if there is a loss error
    Log(LOG_DEV_DATA, "Comparing: %f < %f < %f, %f - %f > %f\n", minLoss, loss, maxLoss, loss, previousLoss, maxLossDiff);

    // if a failure not detected
    if(testStatus == BEP_STATUS_NA) testStatus = BEP_STATUS_SUCCESS;

    // update the background color
    Log(LOG_DEV_DATA, "Publishing CurrentLossBGColor for: %s with value: %s\n",
        GetDataTag(rollerName[wheel] + "CurrentLossBGColor").c_str(), color.c_str());
    SystemWrite(GetDataTag(rollerName[wheel] + "CurrentLossBGColor"), color);
    // send the value of the machine loss for the roller
    char buffer[128];
    SendSubtestResultWithDetail(GetDataTag(rollerName[wheel] + "MachineLossResult"), testStatus, "Measured Machine Loss", "0000",
                                "ActualLoss", CreateMessage(buffer, sizeof(buffer), "%.2f", loss), unitsLBF,
                                "MaxLoss", CreateMessage(buffer, sizeof(buffer), "%.2f", maxLoss), unitsLBF,
                                "MinLoss", CreateMessage(buffer, sizeof(buffer), "%.2f", minLoss), unitsLBF);

    Log(LOG_FN_ENTRY, "Validate Loss: %d done\n", testStatus);

    return(testStatus);
}

//=============================================================================
INT32 LossCompensationTC::AnalyzeForces(int wheel, float &average)
{
    INT32 testStatus = BEP_STATUS_NA;   // the status of the test
    float total=0;                      // the sum of every sample difference
    float blockTotal = 0;               // the sum of every sample in the moving average block
    float sampleDiff=0;                 // the difference between sample values
    int runoutErrorCount=0;                 // the number of runout errors in the array
    int noiseErrorCount=0;              // the number of noise errors in the array
    DATAARRAY array = m_wheelForceArray[wheel]; // get the wheel force array

    float runoutDiff = GetTestStepInfoFloat("RunoutDifference");
    float runoutErrorMultiplier = GetTestStepInfoFloat("RunoutErrorMultiplier");
    float noiseDiff = GetTestStepInfoInt("NoiseDifference");
    INT32 noiseMaxError = GetTestStepInfoInt("NoiseMaxError");
    UINT32 runoutSamplesPerAverage = GetTestStepInfoInt("RunoutSamplesPerAverage"); 

    Log(LOG_FN_ENTRY, "Analyze Forces: wheel = %d, runoutDiff = %f, array.size() = %d\n", 
        wheel, runoutDiff, array.size());
    // walk through the data array and add the values
    // and check for encoder runout issues start with the
    // runoutSamplesPerAverage entry for filtering purposes

    UINT32 blockSamples = (array.size() < runoutSamplesPerAverage) ? array.size() : runoutSamplesPerAverage;
    for(UINT32 ii = 0; ii < blockSamples; ii++)
    {
        Log(LOG_DETAILED_DATA, "Adding %f to the current total %f", array[ii], blockTotal);
        blockTotal += array[ii];
    }

    total = blockTotal;
    Log(LOG_DETAILED_DATA, "Calculated total: %f", total);

    if(array.size() > runoutSamplesPerAverage)
    {
        for(UINT32 index = runoutSamplesPerAverage; index < array.size(); index++)
        {
            blockTotal = blockTotal + array[index] - array[index - runoutSamplesPerAverage];
            total += array[index];

            sampleDiff = abs(array[index] - (blockTotal / runoutSamplesPerAverage));

            if(sampleDiff > runoutDiff)
                runoutErrorCount++;

            if(sampleDiff > noiseDiff)
                noiseErrorCount++;
        }
    }
    else
    {
        Log(LOG_ERRORS, "Not enough samples to perform run out check! (%d < %d)", array.size(), runoutSamplesPerAverage);
    }

    // calculate the average force over the sampled interval
    average = total / array.size();
    Log(LOG_DEV_DATA, "Calculated raw average: %f = %f / %d", average, total, array.size());

    Log(LOG_DEV_DATA, "Encoder Noise Test Stats: \n");
    Log(LOG_DEV_DATA, "\tNoiseDifference: %02.1f\n\tNoiseMaxError: %d\n\tNoiseErrorCount: %d\n\tTotalSamples: %d\n",
        noiseDiff, noiseMaxError, noiseErrorCount, array.size());
    Log(LOG_DEV_DATA, "Encoder Runout Test Stats: \n");
    Log(LOG_DEV_DATA, "\tRunoutDifference: %02.1f\n", runoutDiff);
    Log(LOG_DEV_DATA, "\tRunoutErrorMultiplier * array.size(): %f\n\tRunoutErrorCount: %d\n\tTotalSamples: %d\n",
        runoutErrorMultiplier*array.size(), runoutErrorCount, array.size());

    // check for encoder error and set it or clear it
    if(noiseErrorCount > noiseMaxError)
    {
        // update the fault server and the system
        testStatus = BEP_STATUS_FAILURE;
        SetFault(GetDataTag(rollerName[wheel] + "EncoderNoiseError"), rollerName[wheel] + " Encoder Noise Error Detected");
        SystemWrite(GetDataTag(rollerName[wheel] + "EncoderNoiseError"), true);
        Log(LOG_ERRORS, "Wheel %d Noise Error %d > %d\n", wheel, noiseErrorCount, noiseMaxError);
    }
    else
    {   // update the fault server and the system
        testStatus = BEP_STATUS_SUCCESS;
        ClearFault(GetDataTag(rollerName[wheel] + "EncoderNoiseError"));
        SystemWrite(GetDataTag(rollerName[wheel] + "EncoderNoiseError"), false);
    }

    if(runoutErrorCount > runoutErrorMultiplier*array.size())
    {   // update the fault server and the system
        testStatus = BEP_STATUS_FAILURE;
        SetFault(GetDataTag(rollerName[wheel] + "EncoderRunoutError"), rollerName[wheel] + " Encoder Runout Error Detected");
        SystemWrite(GetDataTag(rollerName[wheel] + "EncoderRunoutError"), true);
        Log(LOG_ERRORS, "Wheel %d Runout Error %d > %f\n", wheel, runoutErrorCount, runoutErrorMultiplier * array.size());
    }
    else
    {   // update the fault server and the system
        //testStatus = BEP_STATUS_SUCCESS;
        ClearFault(GetDataTag(rollerName[wheel] + "EncoderRunoutError"));
        SystemWrite(GetDataTag(rollerName[wheel] + "EncoderRunoutError"), false);
    }

    // adjust it from the current machine losses in the ICM
    float value = atof(SystemRead(GetDataTag(rollerName[wheel] + "MachineLoss")).c_str());
    Log(LOG_DEV_DATA, "Adjusting average by current loss: %f += %f", average, value);
    average += value;
    Log(LOG_DEV_DATA, "Adjusted average: %f", average);
    // range check
    if(average < 0)     average = 0;

    // update the test result to the GUI
    SystemWrite(GetDataTag(rollerName[wheel] + "CurrentLoss"), average);
    // update the new machine loss in the system
    SystemWrite(GetDataTag(rollerName[wheel] + "MachineLoss"), average);

    Log(LOG_FN_ENTRY, "Wheel %d, Force average %f\n", wheel, average);

    return(testStatus);
}

//=============================================================================
INT32 LossCompensationTC::SampleForces(INT32 &firstTag, INT32 &secondTag)
{
    INT32 status = BEP_STATUS_SUCCESS;
    string tag, value;
    try
    {   // while we are above StartSampleSpeed, wait
        float StartSampleSpeed = GetTestStepInfoFloat("StartSampleSpeed");
        Log(LOG_DEV_DATA, "Waiting For Start Sample Speed\n");
        while(((status = LossCompStatusCheck()) == BEP_STATUS_SUCCESS) &&
              (SpeedCheck(StartSampleSpeed, true) == BEP_STATUS_SUCCESS) &&
              TimeRemaining())
        {
            BposSleep(100);
        }

        // if the system has not timed out waiting to perform the sampling and still good
        if((TimeRemaining()) && (status == BEP_STATUS_SUCCESS))
        {   // tell the ICM to tag the start sample (this returns the array indices)
            firstTag = TagArray("LCStart");
            Log(LOG_DEV_DATA, "Tagged Start Array: %d\n", firstTag);
            // wait till the end sample speed is reached or the test step times out
            float StopSampleSpeed = GetTestStepInfoFloat("StopSampleSpeed");
            Log(LOG_DEV_DATA, "Waiting For End Sample Speed\n");
            while(((status = LossCompStatusCheck()) == BEP_STATUS_SUCCESS) &&
                  (SpeedCheck(StopSampleSpeed, true) == BEP_STATUS_SUCCESS) &&
                  TimeRemaining())
            {
                BposSleep(100);
            }

            // if a timeout did not occur and still good
            if((TimeRemaining()) && (status == BEP_STATUS_SUCCESS))
            {   // tell the ICM to tag the end sample (this returns the array indicy)
                secondTag = TagArray("LCEnd");
                Log(LOG_DEV_DATA, "Tagged End Array: %d\n", secondTag);
            }
        }
        if(!TimeRemaining())
            status = BEP_STATUS_TIMEOUT;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "SampleForces Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }

    return(status);
}

//=============================================================================
INT32 LossCompensationTC::TagArray(const string arrayTag)
{
    INT32 sampleNumber = 0;

    if(SystemCommand(GetDataTag("TagIndex"), arrayTag) == BEP_STATUS_SUCCESS)
    {
        string value = GetSystemValue(GetDataTag("TagIndex"));
        if(value != "ERROR")    sampleNumber = atoi(value.c_str());
        else                    throw BepException("Could Not Tag Brake Start");
    }
    return(sampleNumber);
}

//=============================================================================
inline const string LossCompensationTC::GetLCStatus(void)
{
    return(m_lcStatus);
}

//=============================================================================
void LossCompensationTC::SetLCStatus(const string status)
{
    UpdateResult(status, m_lcStatus);
}

//=============================================================================
void LossCompensationTC::Timestamp(void)
{   // get the time and format the strings for storage
    time_t systemTime = time(NULL);
    struct tm *currentTime = localtime(&systemTime);
    // Convert the date time string to what the requestor asked for
    string time, date;
    char buffer[20];
    // get the current time
    strftime(buffer, sizeof(buffer), "%T", currentTime);
    time = buffer;
    // get the current date
    strftime(buffer, sizeof(buffer), "%m/%d/%Y", currentTime);
    date = buffer;

    // update the machine
    SystemWrite(GetDataTag("MachineLossTime"), time);
    SystemWrite(GetDataTag("MachineLossDate"), date);
    SystemWrite(GetDataTag("LossCompensationCurrentTime"), time);
    SystemWrite(GetDataTag("LossCompensationCurrentDate"), date);
}

//=============================================================================
void LossCompensationTC::SetupLossData(void)
{
    // read the machine losses, time and date and update the GUI
    string value;
    for(UINT32 index = 0; index < GetRollerCount(); index++)
    {   // read the machine losses and update the display
        value = SystemRead(GetDataTag(rollerName[index] + "MachineLoss"));
        SystemWrite(GetDataTag(rollerName[index] + "PreviousLoss"), value);
        m_previousLosses.push_back(atof(value.c_str()));
        value = "0";
        SystemWrite(GetDataTag(rollerName[index] + "CurrentLoss"), value);
        value = "white";
        SystemWrite(GetDataTag(rollerName[index] + "PreviousLossBGColor"), value);
        SystemWrite(GetDataTag(rollerName[index] + "CurrentLossBGColor"), value);
    }
    value = SystemRead(GetDataTag("MachineLossTime"));
    SystemWrite(GetDataTag("LossCompensationPreviousTime"), value);
    value = SystemRead(GetDataTag("MachineLossDate"));
    SystemWrite(GetDataTag("LossCompensationPreviousDate"), value);
}
