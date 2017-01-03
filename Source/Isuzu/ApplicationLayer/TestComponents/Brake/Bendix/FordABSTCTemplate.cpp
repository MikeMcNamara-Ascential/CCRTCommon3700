//******************************************************************************
// Description:
// This class extends the functionality of GenericABSTCTemplate for Ford
// specific ABS functionality
//
//==============================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/TestComponents/Brake/Common/FordABSTCTemplate.cpp $
// 
// 1     2/07/11 4:46p Mmcnamar
// backup c drive source for cross compiler
// 
// 3     9/20/10 2:42p Mmcnamar
// Not printing/logging accel rates in TestStepNeutral().
// 
// 2     7/23/10 3:24p Mmcnamar
// Multiple updates for new core, KTP, OHAP limited slip, and ABS
// security.
// 
// 1     5/10/10 2:08p Mmcnamar
// initial check-in for new core.
// 
// 29    3/29/10 4:05p Mmcnamar
// Added method ResetCommHw() and overrode method EnterDiagnosticMode().
// Updated valve cross analysis.
// 
// 28    3/17/10 12:11p Mmcnamar
// Updated valve cross check.
// 
// 26    1/26/09 10:58a Mmcnamar
// Added SBAParkBrakePawlInit(); Added min drag force paramter in
// TestStepAnalyzeDrag().
// 
// 25    11/14/08 3:40p Mmcnamar
// Updated for hybrid testing.
// 
// 24    10/21/08 10:50a Mmcnamar
// Updated dynamic park brake test for KTP.
// 
// 23    8/21/08 2:19p Mmcnamar
// Added test step "EvaluateRolltest" for results of TRW module "Execute
// Rolltest" command.
// 
// 22    6/16/08 1:58p Jwynia
// Updated for CD338HEV:  Added PedalTravelCheck(); updated EvaluateSBA();
// added ValidateSBAReductionForces();
//   
// 
// 21    5/05/08 2:58p Mmcnamar
// Update EvaluateABS(): No longer calling method FindReductionPoint() to
// locate reduction/recovery indices...using valve firing markers instead.
// Updated EvaluateSBA() for hybrids.
// 
// 20    4/09/08 4:13p Mmcnamar
// Updated for Teves Hybrid SBA brake system; Added Ford ABS security
// algorithm.
// 
// 19    1/09/08 3:41p Mmcnamar
// Update for D471: Reverted back to old FindReductionpoint() method; Will
// need separate ones for cars and trucks because the front wheels on
// trucks stop before the rear ABS valve firing is complete; 
// Update for AAI: Added check for param "DoNotWaitAllWheels" in method
// WaitForTargetSpeed() to avoid waiting for all 4 wheels to drop below 55
// mph to fire ABS valves on the Mazda.
// 
// 18    8/24/07 2:07p Mmcnamar
// Fixed bug where ABS test would fail if the brake force start index was
// equal to zero; Calling GetRollSpeed( ) instead of GetSpeed( ) in
// DynamicparkBrakeTest( ).
// 
// 17    3/28/07 2:46p Mmcnamar
// Added brake pedal check in ParkBrakePawlInit(); Added brake pedal check
// in DynamicParkBrake(); Fixed bug in dynamic park brake test where
// sample speed wasn't being used if min force wasn't reached; Updated
// ValveFiringTest() so valve firing sequence waits until speed is below
// "ABSTestStartSpeed" parameter; Added parameter "ForceMeterCenterValue"
// for SetPrompt1ForceValue().
// 
// 16    3/05/07 10:23a Mmcnamar
// Added HSSensorTest( ) for test step ABSHSSensorTest; Changed error log
// msg in TestStepNeutral from "Exception in AWDSpeedBalance" to
// "Exception in TestStepNeutral".
// 
// 15    12/27/06 12:39p Mmcnamar
// Updated for Dynamic Park Brake test: Added
// TestStepAccelerateToParkBrakeSpeed(); TestStepDynamicParkBrake();
// AnalyzeDynamicParkBrake(); TestStepParkBrakeBalance(). Overloaded
// TestStepAccelerate() to call GetRollSpeed() which returns average wheel
// speed of driven axle.
// 
// Updated AnalyzeLFValveCross(), AnalyzeRFValveCross(), and
// AnalyzeLRValveCross(), to call SendSubtestResultWithDetail() with fault
// code of 0000 if valve cross check passes in order to clear fault when
// in-cycle retest passes.
// 
// 14    8/09/06 10:11a Bmeinke
// The Log() call at the end of EvaluateABS() was missing status.c_str()
// argument
// 
// 13    4/01/06 5:01p Bmeinke
// Fixed a bug in EvaluateABS() when using the reduction discovery
// algorithm where the indices would be set to 0 if we did not find a
// reduction/recovery point which would cause a SIGSEG in the base class
// when it tried to load the brake force array (base class subtracts
// m_absStartIdx from each of the indices and starts reading the arrays
// from there which wound up being a large negative number)
// Overloaded the new GeneriCTC park brake/park pawl 'init' methods to
// allow us to start monitoring the brake switch AFTER the "Shift" and
// "Apply park brake" prompts have been displayed.
// 
// 12    2/24/06 1:23p Bmeinke
// Initialize the m_reduxRecovIndex structure to "clean" values inside
// ExecuteAbsRollTestSequence()
// Added new ABSReferenceSamples parameter. Defines how many base brake
// samples are used for calculating the reduction/recovery reference force
// Modified EvaluateABS() so that the reduction/recovery discovery
// algorithm will work when we perform individual valve control (as
// opposed to using a canned ABS internal valve firing command)
// Overloaded the new GetAbsReferenceForce() method to use
// "ABSReferenceSamples" brake force samples just prior to reduction for
// calculating reduction/recovery percentages
// Removed the StopCurrentTest check from StatusCheck and added the
// StopCurrentTest tag to the list of StatusCheckItems in the config file
// 
// 11    9/23/05 3:03p Bmeinke
// Added a new TestStep method (ReportBrakeData) to optionally add the ICM
// brake and speed log file data to the test result file
// 
// 10    9/20/05 10:17a Bmeinke
// Added a ParkBrakeStop test step that is like the BrakeToStop test step,
// but displays an actual Brake to Stop prompt (as opposed to the Brake
// Moderately in Green" prompt that the Ford BrakeToStop test step
// displays).
// Fixed reading the accelerations from the ICM in TestStepNeutral()
// Added an overloaded ComboTorqueTest() to copy parameter values into
// TestStep info before calling the base class method
// 
// 9     9/16/05 11:43a Bmeinke
// Changes to TestStepNeutral()
// 
// 8     8/31/05 5:08p Bmeinke
// Added TestStepNeutral() method to implement a neutral check
// 
// 7     8/03/05 2:41p Bmeinke
// Removed the ValidateTorqueTestDriveAxle() method and moved the park
// brake/pawl brake switch check to the new
// AnalyzeParkBrakePawlDriverInfluence() method
// 
// 6     7/08/05 12:57p Bmeinke
// Removed the Log() statement in StatusCheck() for the ParkBrake/ParkPawl
// test steps
// Removed the HydParkBrakeTest condition from StatusCheck(): test step
// does not exist
// Added code to the HighSpeedSensorTest() to modify the sensor test speed
// parameters to arm the test if we are outside of the range (but above
// the target) defined by the config file parameters
// Added new UpdateParameterValue() methods to support the
// HighSpeedSensorTest() method modification
// 
// 5     6/30/05 8:32p Bmeinke
// Inside ExecuteAbsRollTestSequence(): set priority to 20 instead of 50
// Inside EvaluateABS(): Had to add 'm_absStartIndex' to the reduction and
// recovery indices in order for GenericABSTCTemplate to properly evaluate
// valve firing
// 
// 4     6/21/05 5:28p Bmeinke
// Added brake switch checks to the AWDSpeedBalance() method
// 
// 3     6/15/05 2:45p Bmeinke
// Added short circuit check at the beginning of AWDSpeedBalance()
// 
// 2     6/14/05 2:58p Bmeinke
// Added support for new AWDSpeedBalance test
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 4     3/09/05 12:13p Bmeinke
// Added NoStoredCodeLogging
// 
// 3     12/22/04 8:01p Bmeinke
// Moved StopPumpMotor() test step handling to GenericABSTCTemplate
// Added new test steps to monitor the brake switch (for park brake and
// park pawl driver influence checks) called StartBrakeMonitor() and
// StopBrakeMonitor()
// Added a new teststep (IsolateAll()) to isolate all wheels prior to the
// valve firing test
// Fixed the copying of the force meter center value from the parameter
// section to the TestStepInfo section (was using the wrong test step info
// attribute name)
// Added calls to UpdatePrompts() to the LowSpeed and HighSpeed sensor
// test steps so that the target speed always gets sent to eCATS
// If the base class sensor test methods return a 'skip' status, we will
// return the  previous test step status. This was due to the fact that
// the base class will return 'skip' if the test has already passed. eCATS
// can still re-sequence the sensor test even if it has already passed and
// a 'skip' status gets sent to eCATS as a critical equipment fault
//  
// 
// 2     12/14/04 8:47p Bmeinke
// Fixed the typos in LowSpeedSensorTest() and HighSensorTest() when
// copying parameter values into the TestStepInfo data
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericABSTCTemplate.cpp"
#include "FordABSTCTemplate.h"

#include <math.h>       // For fabsf
#include <sys/sched.h>  // for boosting priority during valve firing
#include <pthread.h>

template <class ModuleType>
FordABSTCTemplate<ModuleType>::FordABSTCTemplate() : GenericABSTCTemplate<ModuleType>(),
m_icmAccelFile( "/dev/Icm/Block/Acceleration"), m_ppwlBrakeAppFault(0)
{
    // Nothing
}

template <class ModuleType>
FordABSTCTemplate<ModuleType>::~FordABSTCTemplate()
{
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::InitializeHook(const XmlNode *config)
{
    try
    {
        m_abs4ChannelFrontFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS4ChannelFrontFiringSequence")->getChildren());
    }
    catch(XmlException &err)
    {
        Log(LOG_ERRORS, "Front firing seuquence not specified for 4-channel system - %s\n", err.GetReason());
    }
    try
    {
        m_abs4ChannelRearFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS4ChannelRearFiringSequence")->getChildren());
    }
    catch(XmlException &err)
    {
        Log(LOG_ERRORS, "Rear firing seuquence not specified for 4-channel system - %s\n", err.GetReason());
    }
    m_zerospeedBrakeTimeFlag = false;
    m_zerospeedParkBrakeTimeFlag = false;

    // call parent initialize hook
    GenericABSTCTemplate<ModuleType>::InitializeHook(config);
}

template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::CommandTestStep(const string &value)
{
    // Status of the test step
    string  status;
    INT32   statusCheck;
    const string step(GetTestStepName());

    try
    {
        // Log it
        Log(LOG_DEV_DATA, "Enter FordABSTCTemplate::CommandTestStep value=%s step=%s\n", value.c_str(), step.c_str());

        if ( (statusCheck=StatusCheck()) != BEP_STATUS_SUCCESS)
        {
            UpdateResult( statusCheck, status);
            Log(LOG_ERRORS, "FordABSTCTemplate::CommandTestStep: StatusCheck() failed: %s\n", status.c_str());
            SendTestResult(status, GetTestStepInfo("Description"));
        }
        else if ("FrontIndividualSensorTest" == step)    status = IndividualSensorTest();
        else if ("RearIndividualSensorTest" == step)     status = IndividualSensorTest();
        else if ("IndividualSensorTest" == step)         status = IndividualSensorTest();
        // Start monitoring brake switch
        else if ("StartBrakeMonitor" == step)            status = StartBrakeSwitchMonitor();
        // Stop monitoring brake switch
        else if ("StopBrakeMonitor" == step)             status = StopBrakeSwitchMonitor();
        // Check for the brake switch in the On position
        else if (step == "PedalTravelCheck")             status = PedalTravelCheck();
        // Isolate all ABS valves
        else if ("IsolateAll" == step)                   status = IsolateAll();
        // Wait for neutral after arm speed
        else if ("NeutralCheck" == step)                 status = TestStepNeutral();
        // Prompt driver to stop at end of park brake / park pawl test
        else if ("ParkBrakeStop" == step)                status = m_baseBrakeTool->TestStepBrakeToStop();
        //MAM 3/14/12
        else if (step == "QuickStop")                     status = TestStepQuickStop(value);
        // Add brake forse/speed data to the test result file
        else if ("ReportBrakeData" == step)              status = TestStepReportBrakeData();
        else if ("RequestABSSecurity" == step)           status = RequestAbsSecurity(); //MAM 3/11/08
        else if ("RequestBendixSecurity1" == step)           status = RequestBendixSecurity1(); //MAM 8/1/14
        else if ("RequestBendixSecurity3" == step)           status = RequestBendixSecurity3(); //MAM 1/7/15
        /**
         * The following test steps are "overriden" so we can take values from our
         * Parameters and put them into the TestStepInfo that the base class wants.
         * It is illogical to me that we should have drag, base brake and ABS parameter
         * data contained in the test step info.
         */

        // Accelerate to brake test speed
        else if ("AccelerateToBrakeSpeed" == step)      status = TestStepAccelerateToBrakeSpeed();
        else if("AccelerateToABSSpeed" == step)
		{
			m_baseBrakeTool->UpdateTarget(false);
			status = m_baseBrakeTool->TestStepAccelerate();
		}
        // Perform the drag test
        else if ("DragTest" == step)                    status = TestStepDrag();
        // Perform the base brake test
        else if ("BrakeTest" == step)                   status = TestStepBaseBrake();
        // Perform the ABS reduction/recovery sequence
        else if ("ValveFiringTest" == step)             status = ValveFiringTest();
        // Perform the ABS reduction/recovery sequence
        else if ("FrontValveFiringTest" == step)        status = ValveFiringTest("Front");
        // Perform the ABS reduction/recovery sequence
        else if ("RearValveFiringTest" == step)         status = ValveFiringTest("Rear");
        // Analyze the drag test results
        else if ("AnalyzeDragTest" == step)             status = TestStepAnalyzeDrag();
        // Analyze the drag test results
        /*else if ("AnalyzeFrontDragTest" == step)        status = TestStepAnalyzeDrag("Front");
        // Analyze the drag test results
        else if ("AnalyzeRearDragTest" == step)         status = TestStepAnalyzeDrag("Rear");*/
        // Analyze the base brake results
        else if ("AnalyzeBrakeTest" == step)            status = TestStepAnalyzeBaseBrake();
        // Analyze the base brake results
        /*else if ("AnalyzeFrontBrakeTest" == step)       status = TestStepAnalyzeBaseBrake("Front");
        // Analyze the base brake results
        else if ("AnalyzeRearBrakeTest" == step)        status = TestStepAnalyzeBaseBrake("Rear");*/
        // Analyze the brake balances
        else if ("AnalyzeBalance" == step)              status = TestStepBalance();
        // Analyze the brake balances
        /*else if ("AnalyzeFrontBalance" == step)         status = TestStepBalance("Front");
        // Analyze the brake balances
        else if ("AnalyzeRearBalance" == step)          status = TestStepBalance("Rear");*/
        // Accelerate to dynamic park brake test speed
        else if ("AccelerateToParkBrakeSpeed" == step)  status = TestStepAccelerateToParkBrakeSpeed();
        // Perform dynamic park brake test
        else if ("DynamicParkBrakeTest" == step)        status = TestStepDynamicParkBrake();
        // Analyze dynamic park brake test
        else if ("AnalyzeDynamicParkBrake" == step)     status = AnalyzeDynamicParkBrake();
        // Analyze the dynamic park brake balance
        else if ("AnalyzeParkBrakeBalance" == step)     status = TestStepParkBrakeBalance();
        //MAM 3/24/10  - Reset the AVT
        else if ("ResetCommHw" == step)                 status = ResetCommHw();
        //MAM 31/31/13  - Put Gryphon in listen-only mode
        else if ("DisableCommHw" == step)               status = DisableCommHw();
        //MAM 3/25/10 - Place the module into diagnostic mode
        else if (step == "EnterDiagnosticMode")         status = EnterDiagnosticMode();
        //MAM 12/18/14
        else if (step == "WriteFingerprint")            status = WriteFingerprint();
        //MAM 12/23/14
        else if (step == "CheckPartNumber")             status = CheckPartNumber();
        else if (step == "EvaluateFrontAbs")            status = EvaluateABS("Front");
        else if (step == "EvaluateRearAbs")             status = EvaluateABS("Rear");
        else if (step == "FrontValveCycleTest")         status = ValveCycleTorqueTest("Forward", "Front");
        else if (step == "RearValveCycleTest")          status = ValveCycleTorqueTest("Forward", "Rear");
        else if (step == "AccelerateVehicleToSpeed")    status = AccelerateVehicleToSpeed();
        // Since it's not a test step we've defined, call the parent
        else                                            status = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
    }
    catch (BepException &ex)
    {
        Log(LOG_DEV_DATA, "FordABSTCTemplate::CommandTestStep(%s) Exception: %s\n", ex.GetReason());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    catch (ModuleException &ex)
    {
        Log(LOG_DEV_DATA, "FordABSTCTemplate::CommandTestStep(%s) ModuleException: %s\n", ex.GetReason());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }

    // This will be logged more than once perhaps if we called our parent's
    // CommandTestStep() method
    Log(LOG_DEV_DATA, "Enter FordABSTCTemplate::CommandTestStep(%s): %s, status = %s\n", value.c_str(), step.c_str(), status.c_str());

    // return the status
    return(status);

}

template <class VehicleModuleType>
string FordABSTCTemplate<VehicleModuleType>::PedalTravelCheck()
{
    //BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    //string testResult;             = BEP_TESTING_STATUS;
    string testResult            = testPass;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    SerialString_t               response;
    BEP_STATUS_TYPE              status;
    short travelDistance         = 0;
    bool promptDisplayed         = false;
    int lameTimer                = 6;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter PedalTravelCheck \n");
    // Determine if this test step should be performed


    status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
    if (status == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "ReadPedalTravel response = %2X %2X %2X %2X %2X\n",
            response[0], response[1], response[2], response[3], response[4]);
        travelDistance = (short)response[3]; //MAM 10/29/08
        travelDistance = travelDistance << 8;
        travelDistance = travelDistance + response[4]; //MAM 10/29/08
        Log(LOG_DEV_DATA, "travelDistance = %d\n", travelDistance);

        if (travelDistance < 1000)
        {
            Log(LOG_DEV_DATA, "Brake Off\n");
        }
        else
        {
            Log(LOG_DEV_DATA, "Brake On.  Prompting Driver to remove foot from brake\n");
            DisplayPrompt( 1, GetPrompt( "RemoveFootFromBrake"), 0);
            promptDisplayed = true;
            while (travelDistance >= 1000 && lameTimer > 0)
            {
                status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
                if (status == BEP_STATUS_SUCCESS)
                {
                    travelDistance = (short)response[3];
                    travelDistance = travelDistance << 8;
                    travelDistance = travelDistance + response[4];
                    Log(LOG_DEV_DATA, "looptravelDistance = %d\n", travelDistance);
                }
                else
                {
                    travelDistance = 0;
                    Log(LOG_DEV_DATA, "LoopReadPedalTravel response failed\n");
                    testResult = testFail;
                }
                lameTimer--;
                BposSleep(500);
            }
            Log(LOG_DEV_DATA, "After While TimeRemaining: %d, TravelDistance %d \n", TimeRemaining(), travelDistance);
            if (travelDistance >= 1000)
            {
                Log(LOG_DEV_DATA, "PedalTravelCheck loop timed out.  Driver did not remove foot from brake\n");
            }
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "ReadPedalTravel response failed\n");
        testResult = testFail;
        //MAM 10/30/08        
        testResultCode = GetFaultCode("CommunicationFailure");
        testDescription = GetFaultDescription("CommunicationFailure");
        //update test result with comm error?
    }

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit: PedalTravelCheck\n");
    return testResult;
}


template <class ModuleType>
inline string FordABSTCTemplate<ModuleType>::Setup(void)
{
    string  status;

    Log(LOG_DEV_DATA, "Enter FordABSTCTemplate::Setup()\n");

    // Determine if this is a 3 valve or 4 vavle ABS module
    if ( 3 == GetParameterInt("ABSValveCount"))
    {
        SetFourChannelSystem( false);
    }
    else
    {
        SetFourChannelSystem( true);
    }

    try
    {
        m_icmAccelFile = GetParameter("AccelFilePath");
    }
    catch (BepException &err)
    {
        m_icmAccelFile = std::string( "/dev/Icm/Block/Acceleration");
    }

    status = GenericABSTCTemplate<ModuleType>::Setup();

    Log(LOG_DEV_DATA, "Exit FordABSTCTemplate::Setup(), status=%s\n", status.c_str());

    return( status);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::StartBrakeSwitchMonitor(void) 
{
    // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    // Check if we need to skip this step
    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::StartBrakeSwitchMonitor()\n");
    if (CheckCableConnect())
    {
        // Need to perform this test step
        try
        {
            // Reset the brake siwtch on/off status
            m_vehicleModule.ClearBrakeSwitchStatus();
            // Try to start the switch monitor thread
            moduleStatus = m_vehicleModule.StartSwitchMonitor();
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            // Set the rest of the data
            Log(LOG_DEV_DATA, "StartBrakeSwitchMonitor: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("BrakeSwitchCommError"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("BrakeSwitchCommError"));
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        // Need to skip this test step
        testResult = testError;
        Log(LOG_DEV_DATA, "Cannot monitor because cable is not connected\n");
    }
    // Return the test result
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::StartBrakeSwitchMonitor(%s)\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::StopBrakeSwitchMonitor(void) 
{
    // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    bool   sawBrakeOn = false;

    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    // Check if we need to skip this step
    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::StopBrakeSwitchMonitor()\n");
    // Need to perform this test step
    try
    {
        // Try to start the switch monitor thread
        moduleStatus = m_vehicleModule.StopSwitchMonitor();
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

        m_vehicleModule.SawBrakeSwitchOn( sawBrakeOn);
        if ( sawBrakeOn)
        {
            Log( LOG_ERRORS, "Saw brake switch on during park brake / park pawl test\n");
        }

        // Set the rest of the data
        Log(LOG_DEV_DATA, "StopBrakeSwitchMonitor: %s - status: %s\n",
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("BrakeSwitchCommError"));
        testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("BrakeSwitchCommError"));
    }
    catch (ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::StopBrakeSwitchMonitor(%s)\n", testResult.c_str());
    return(testResult);
}

#if 0
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::ValidateTorqueTestDriveAxle( float leftDistance, float rightDistance, float maxDistance, const std::string &faultTag)
{
    string testStatus = BEP_PASS_STATUS;        // the current test status
    const std::string driverInfFault = GetTestStepInfo( "DIFaultTag");
    bool   sawBrakeOn = false;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::ValidateTorqueTestDriveAxle()\n");

    // Check if we saw the brake switch during the park pawl test
    m_vehicleModule.SawBrakeSwitchOn( sawBrakeOn);
    if ( false == sawBrakeOn)
    {
        // Analyze the distances that the wheels travelled
        testStatus = GenericTC::ValidateTorqueTestDriveAxle(leftDistance, rightDistance, maxDistance, faultTag);
    }
    else
    {
        Log( LOG_ERRORS, "Failed torque test drive axle for driver influence\n");
        // Send the fault info
        SendSubtestResultWithDetail( GetTestStepName(), 
                                     testFail, 
                                     GetTestStepInfo("Description"), 
                                     GetFaultCode(driverInfFault));
        testStatus = testFail;
    }
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::ValidateTorqueTestDriveAxle(), status=%s\n", testStatus.c_str());

    return( testStatus);
}
#endif

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::EvaluateABS(void) 
{
    const int                   rollCount = GetRollerCount();    
    INT32                       ii;    
    std::string                 status;
    //MAM 9/14/12
    string                      clearDidStatus;
    BEP_STATUS_TYPE             moduleStatus = BEP_STATUS_FAILURE;
    SerialString_t              response;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::EvaluateABS()\n");

    //debug
    Log( LOG_DEV_DATA, "m_absStartIndex = %d m_absEndIndex = %d\n",m_absStartIndex,m_absEndIndex);         
    //These have already been marked during the valve firing test
    Log( LOG_DEV_DATA, "LF Markers 1: %d %d %d %d\n", 
         m_reduxRecovIndex[LFWHEEL].reductionStart, m_reduxRecovIndex[LFWHEEL].reductionEnd,
         m_reduxRecovIndex[LFWHEEL].recoveryStart, m_reduxRecovIndex[LFWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "RF Markers 1: %d %d %d %d\n", 
         m_reduxRecovIndex[RFWHEEL].reductionStart, m_reduxRecovIndex[RFWHEEL].reductionEnd,
         m_reduxRecovIndex[RFWHEEL].recoveryStart, m_reduxRecovIndex[RFWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "LR Markers 1: %d %d %d %d\n", 
         m_reduxRecovIndex[LRWHEEL].reductionStart, m_reduxRecovIndex[LRWHEEL].reductionEnd,
         m_reduxRecovIndex[LRWHEEL].recoveryStart, m_reduxRecovIndex[LRWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "RR Markers 1: %d %d %d %d\n", 
         m_reduxRecovIndex[RRWHEEL].reductionStart, m_reduxRecovIndex[RRWHEEL].reductionEnd,
         m_reduxRecovIndex[RRWHEEL].recoveryStart, m_reduxRecovIndex[RRWHEEL].recoveryEnd);

    const int recoverySamples = (int)(GetParameterInt( "ABSRecoveryTime") / 20.0 + 0.5);

    for ( ii=LFWHEEL; ii<rollCount; ii++)
    {
        //give wheel a little more time to react to reduction cmd
        m_reduxRecovIndex[ii].reductionEnd += 20;   //MAM 1/31/11 - for U38x
        //MAM 1/31/11 - safety net
        if ( m_reduxRecovIndex[ii].reductionEnd > m_absEndIndex)  m_reduxRecovIndex[ii].reductionEnd = m_absEndIndex;

        // Calculate the new recovery indices based on parameter data
        m_reduxRecovIndex[ii].recoveryEnd += recoverySamples;
        if ( m_reduxRecovIndex[ii].recoveryEnd > m_absEndIndex)  m_reduxRecovIndex[ii].recoveryEnd = m_absEndIndex;
    }

    Log( LOG_DEV_DATA, "LF Markers 2: %d %d %d %d\n", 
         m_reduxRecovIndex[LFWHEEL].reductionStart, m_reduxRecovIndex[LFWHEEL].reductionEnd,
         m_reduxRecovIndex[LFWHEEL].recoveryStart, m_reduxRecovIndex[LFWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "RF Markers 2: %d %d %d %d\n", 
         m_reduxRecovIndex[RFWHEEL].reductionStart, m_reduxRecovIndex[RFWHEEL].reductionEnd,
         m_reduxRecovIndex[RFWHEEL].recoveryStart, m_reduxRecovIndex[RFWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "LR Markers 2: %d %d %d %d\n", 
         m_reduxRecovIndex[LRWHEEL].reductionStart, m_reduxRecovIndex[LRWHEEL].reductionEnd,
         m_reduxRecovIndex[LRWHEEL].recoveryStart, m_reduxRecovIndex[LRWHEEL].recoveryEnd);
    Log( LOG_DEV_DATA, "RR Markers 2: %d %d %d %d\n", 
         m_reduxRecovIndex[RRWHEEL].reductionStart, m_reduxRecovIndex[RRWHEEL].reductionEnd,
         m_reduxRecovIndex[RRWHEEL].recoveryStart, m_reduxRecovIndex[RRWHEEL].recoveryEnd);

    status = GenericABSTCTemplate<ModuleType>::EvaluateABS();

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::EvaluateABS(), status=%s\n", status.c_str());

    //MAM 9/14/12 - Update status DID if necessary
    if (GetParameterBool("SetStatusDID"))
    {

        if (testPass == status)
        {
            Log(LOG_DEV_DATA, "EvaluateABS - setting StatusDID to PASS (0x55)\n");
            moduleStatus = m_vehicleModule.GetInfo( GetDataTag("SetStatusDIDPass"), response );
            //if (BEP_PASS_RESPONSE != moduleStatus)
            if (BEP_STATUS_SUCCESS != moduleStatus)
            {
                Log(LOG_ERRORS, "EvaluateABS - Error setting StatusDID to PASS\n");
                //even though we passed the brake evaluation, we can't pass if we can't set the status DID
                //status = BEP_FAIL_RESPONSE;
                status = testFail; //set comm fault here??????????????????
            }
        }
    }

    return( status);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::EvaluateABS(std::string axle) 
{
    const int                   rollCount = GetRollerCount();    
    INT32                       ii;    
    std::string                 status;
    //MAM 9/14/12
    string                      clearDidStatus;
    BEP_STATUS_TYPE             moduleStatus = BEP_STATUS_FAILURE;
    SerialString_t              response;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::EvaluateABS()\n");

    //debug
    Log( LOG_DEV_DATA, "m_absStartIndex = %d m_absEndIndex = %d\n",m_absStartIndex,m_absEndIndex);         
    //These have already been marked during the valve firing test
    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
    {
        Log( LOG_DEV_DATA, "LF Markers 1: %d %d %d %d\n", 
             m_reduxRecovIndex[LFWHEEL].reductionStart, m_reduxRecovIndex[LFWHEEL].reductionEnd,
             m_reduxRecovIndex[LFWHEEL].recoveryStart, m_reduxRecovIndex[LFWHEEL].recoveryEnd);
        Log( LOG_DEV_DATA, "RF Markers 1: %d %d %d %d\n", 
             m_reduxRecovIndex[RFWHEEL].reductionStart, m_reduxRecovIndex[RFWHEEL].reductionEnd,
             m_reduxRecovIndex[RFWHEEL].recoveryStart, m_reduxRecovIndex[RFWHEEL].recoveryEnd);
    }
    else
    {
        Log( LOG_DEV_DATA, "LR Markers 1: %d %d %d %d\n", 
             m_reduxRecovIndex[LRWHEEL].reductionStart, m_reduxRecovIndex[LRWHEEL].reductionEnd,
             m_reduxRecovIndex[LRWHEEL].recoveryStart, m_reduxRecovIndex[LRWHEEL].recoveryEnd);
        Log( LOG_DEV_DATA, "RR Markers 1: %d %d %d %d\n", 
             m_reduxRecovIndex[RRWHEEL].reductionStart, m_reduxRecovIndex[RRWHEEL].reductionEnd,
             m_reduxRecovIndex[RRWHEEL].recoveryStart, m_reduxRecovIndex[RRWHEEL].recoveryEnd);
    }

    const int recoverySamples = (int)(GetParameterInt( "ABSRecoveryTime") / 20.0 + 0.5);

    for ( ii=LFWHEEL; ii<rollCount; ii++)
    {
        //give wheel a little more time to react to reduction cmd
        m_reduxRecovIndex[ii].reductionEnd += 20;   //MAM 1/31/11 - for U38x
        //MAM 1/31/11 - safety net
        if ( m_reduxRecovIndex[ii].reductionEnd > m_absEndIndex)  m_reduxRecovIndex[ii].reductionEnd = m_absEndIndex;

        // Calculate the new recovery indices based on parameter data
        m_reduxRecovIndex[ii].recoveryEnd += recoverySamples;
        if ( m_reduxRecovIndex[ii].recoveryEnd > m_absEndIndex)  m_reduxRecovIndex[ii].recoveryEnd = m_absEndIndex;
    }

    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
    {
        Log( LOG_DEV_DATA, "LF Markers 2: %d %d %d %d\n", 
             m_reduxRecovIndex[LFWHEEL].reductionStart, m_reduxRecovIndex[LFWHEEL].reductionEnd,
             m_reduxRecovIndex[LFWHEEL].recoveryStart, m_reduxRecovIndex[LFWHEEL].recoveryEnd);
        Log( LOG_DEV_DATA, "RF Markers 2: %d %d %d %d\n", 
             m_reduxRecovIndex[RFWHEEL].reductionStart, m_reduxRecovIndex[RFWHEEL].reductionEnd,
             m_reduxRecovIndex[RFWHEEL].recoveryStart, m_reduxRecovIndex[RFWHEEL].recoveryEnd);
    }
    else
    {
        Log( LOG_DEV_DATA, "LR Markers 2: %d %d %d %d\n", 
             m_reduxRecovIndex[LRWHEEL].reductionStart, m_reduxRecovIndex[LRWHEEL].reductionEnd,
             m_reduxRecovIndex[LRWHEEL].recoveryStart, m_reduxRecovIndex[LRWHEEL].recoveryEnd);
        Log( LOG_DEV_DATA, "RR Markers 2: %d %d %d %d\n", 
             m_reduxRecovIndex[RRWHEEL].reductionStart, m_reduxRecovIndex[RRWHEEL].reductionEnd,
             m_reduxRecovIndex[RRWHEEL].recoveryStart, m_reduxRecovIndex[RRWHEEL].recoveryEnd);
    }

    status = GenericABSTCTemplate<ModuleType>::EvaluateABS();

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::EvaluateABS(), status=%s\n", status.c_str());

    //MAM 9/14/12 - Update status DID if necessary
    if (GetParameterBool("SetStatusDID"))
    {

        if (testPass == status)
        {
            Log(LOG_DEV_DATA, "EvaluateABS - setting StatusDID to PASS (0x55)\n");
            moduleStatus = m_vehicleModule.GetInfo( GetDataTag("SetStatusDIDPass"), response );
            //if (BEP_PASS_RESPONSE != moduleStatus)
            if (BEP_STATUS_SUCCESS != moduleStatus)
            {
                Log(LOG_ERRORS, "EvaluateABS - Error setting StatusDID to PASS\n");
                status = testFail; //set comm fault here??????????????????
            }
        }

    }//

    return( status);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::FindMinForcePoint( INT32 wheelIndex, const DATAARRAY &forceData, 
                                                        INT32 &startIdx, INT32 &endIdx, float *minForce)
{
    INT32       retVal = -1;
    INT32       ii;
    //INT32       minIdx = startIdx;
    INT32       minIdx = -1;
    const INT32 samples = endIdx - startIdx;       
    float       forceSample = 999.0;    
    //MAM 8/11/08
    float       minForceVal = 999.0;   
    //DATAARRAY tempDataArray;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::FindMinForcePoint: wheel= %d, startIdx= %d, endIdx= %d, samples=%d\n", 
         wheelIndex, startIdx, endIdx, samples);

    startIdx = startIdx - m_absStartIndex;
    endIdx = startIdx + samples;

    // Make sure we have enough data to analyze    
    if ( samples > 0)
    {
        Log( LOG_DEV_DATA, "Looking for min force sample\n"); //MAM 8/11/08
        //for ( ii=startIdx; ii<samples; ii++)
        for ( ii=startIdx; ii<endIdx; ii++)
        {
            forceSample = forceData[ii];
            if ( forceSample <= minForceVal)
            {
                minForceVal = forceSample;
                minIdx = ii;                
                retVal = minIdx;

                //Log( LOG_DEV_DATA, "Found min force %2.2f at sample #%d\n", minForce[wheelIndex], minIdx);                
                Log( LOG_DEV_DATA, "Found min force %2.2f at sample #%d\n", minForceVal, minIdx);                
            }
        }        
    }
    else
    {
        Log( LOG_ERRORS, "No force samples for FindMinForcePoint()\n");
    }

    minForce[wheelIndex] = minForceVal;    //MAM 8/11/08

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::FindMinForcePoint()\n");

    //BposSleep(50); //MAM 7/24/08 - delay 50 ms
    return( retVal);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::FindMaxForcePoint( INT32 wheelIndex, const DATAARRAY &forceData, 
                                                        INT32 &startIdx, INT32 &endIdx, float *maxForce)
{
    INT32       retVal = -1;
    INT32       ii, maxIdx=0;
    const INT32 samples = endIdx - startIdx;       
    float       forceSample = 0.0;    

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::FindMaxForcePoint(%d, %d)\n", startIdx, endIdx);

    maxForce[wheelIndex] = 0.0;
    // Invalidate the markers passed in just in case the analysis fails
    maxIdx = startIdx = endIdx = 1;    

    // Make sure we have enough data to analyze    
    if ( samples > 0)
    {
        for ( ii=startIdx; ii<samples; ii++)
        {
            forceSample = forceData[ii];
            if ( forceSample >= maxForce[wheelIndex])
            {
                maxForce[wheelIndex] = forceSample;
                maxIdx = ii;                
                retVal = maxIdx;

                Log( LOG_DEV_DATA, "Found max force %2.2f at sample #%d\n", maxForce[wheelIndex], maxIdx);                
            }
        }        
    }
    else
    {
        Log( LOG_ERRORS, "No force samples for FindMaxForcePoint()\n");
    }
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::FindMaxForcePoint()\n");

    BposSleep(50); //MAM 7/24/08 - delay 50 ms
    return( retVal);
}

template <class VehicleModuleType>
float FordABSTCTemplate<VehicleModuleType>::GetAbsReferenceForce( INT32 roller)
{
    const int           refSampleCount = GetParameterInt( "ABSReferenceSamples");
    const int           startIdx = m_reduxRecovIndex[ roller].reductionStart - m_absStartIndex;
    float               refForce = 0;
    std::vector<float>  forceData;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::GetAbsReferenceForce roller = %d refSampleCount= %d startIdx = %d\n", roller, refSampleCount, startIdx);    

    // If no reference count defined OR if reference count is less than where we started reduction    
    if ( (refSampleCount <= 0) || (startIdx < refSampleCount))
    {
        Log( LOG_DEV_DATA, "NOT using ABS reference samples - using base brake as reference\n");    //MAM 1/24/12
        // Use the base class implementation
        refForce = GenericABSTCTemplate<VehicleModuleType>::GetAbsReferenceForce( roller);
    }
    else
    {
        Log( LOG_DEV_DATA, "Using samples %d through %d for ABS reference\n", startIdx-refSampleCount, startIdx);

        // Get the force for this roller
        m_baseBrakeTool->GetWheelForceArray( roller, forceData);

        for ( int ii=0; ii<refSampleCount; ii++)
        {
            refForce += forceData[ startIdx - ii];
        }
        refForce /= refSampleCount;
    }

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::GetAbsReferenceForce roller = %d, ref = %.03f\n", roller, refForce);
    return( refForce);
}


template <class VehicleModuleType>
BEP_STATUS_TYPE FordABSTCTemplate<VehicleModuleType>::AnalyzeValveCross(void)
{

    bool lfcrPass, rfcrPass, lrcrPass, rrcrPass = false;              // status of the check pass/fail
    BEP_STATUS_TYPE result = BEP_STATUS_SUCCESS;    // the result of a check
    //MAM 2/23/10
    INT32 startIndex, endIndex = 0;
    INT32           ii;
    vector<float>   forceData;
    float           minForce[4];
    INT32           minIndex[4];    
    //MAM 11/3/11
    bool            rearfirst = GetParameterBool("RearValvesFirst"); 
    //string wheelCrossMessage = "ABS Valves are crossed";  // Message used to report the results to the test result server
    //string testResult = testFail; // Result of test
    string testResultCode = "0000"; // Failure code for test
    string testDescription = "ABS Valve Cross Check";   // Description of test

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::AnalyzeValveCross()\n");

    // Determine if valves are crossed or something wrong
    if (GetParameterBool("SkipValveCrossAnalysis"))
    {
        Log(LOG_DEV_DATA, "FordABSTCTemplate: ABS valve cross skipped\n");
    }
    //MAM 8/31/11 - m_absCommFault cleared at beginning of valve firing test and set during reduction
    //or recovery if we have a module communication fault
    else if (true == m_absCommFault)
    {
        Log(LOG_DEV_DATA, "m_absCommFault true - ABS valve cross skipped\n");
    }
    else
    {
        //MAM 4/22/08
        if (GetParameterBool("SBABrakeSystem"))
        {
            Log(LOG_DEV_DATA, "FordABSTCTemplate: Performing SBA Valve Cross Check\n");
            lfcrPass = AnalyzeLFValveCrossSBA();
            rfcrPass = AnalyzeRFValveCrossSBA();
            lrcrPass = AnalyzeLRValveCrossSBA();
            rrcrPass = true;

        }
        else
        {

            for (ii=0; ii<4; ii++)
            {
                forceData.clear();
                m_baseBrakeTool->GetWheelForceArray( ii, forceData);

                startIndex = m_reduxRecovIndex[ii].reductionStart; //MAM 4/5/11                    
                endIndex = m_reduxRecovIndex[ii].reductionEnd;                    

                Log( LOG_DEV_DATA, "wheel %d: startIndex = %d endIndex = %d \n", ii, startIndex, endIndex);
                Log( LOG_DEV_DATA, "Finding min force point \n");
                minIndex[ii] = FindMinForcePoint(ii, forceData, startIndex, endIndex, minForce);

                if (-1 != minIndex[ii])
                {
                    Log( LOG_DEV_DATA, "Min force for wheel %d = %2.2f at index %d \n", ii, minForce[ii], minIndex[ii]);
                }
                else
                {
                    Log( LOG_DEV_DATA, "Failed to locate min force indices for wheel %d \n", ii);                    
                }
            }

            if (-1 != minIndex[ii])
            {
                //MAM 6/30/10
                if (GetParameterBool("LimitedSlip"))
                {
                    Log(LOG_DEV_DATA, "FordABSTCTemplate: Performing LimitedSlip Valve Cross Check\n");
                    lfcrPass = AnalyzeLFValveCrossLimSlip(minIndex);
                    rfcrPass = AnalyzeRFValveCrossLimSlip(minIndex);
                    lrcrPass = true;
                    rrcrPass = true;
                }
                //MAM 3/08/11
                //else if (GetParameterBool("RearValvesFirst"))
                //These are typically AWD 4-channel
                else if (rearfirst)
                {
                    Log(LOG_DEV_DATA, "FordABSTCTemplate: Performing RearValvesFirst Valve Cross Check\n");
                    lrcrPass = AnalyzeLRValveCrossRearFirst(minIndex);
                    rrcrPass = AnalyzeRRValveCrossRearFirst(minIndex);
                    lfcrPass = AnalyzeLFValveCrossRearFirst(minIndex);
                    rfcrPass = true;
                }
                //MAM 2/18/12 - New valve cross checks
                else
                {
                    Log(LOG_DEV_DATA, "FordABSTCTemplate: Performing FrontValvesFirst Valve Cross Check\n");
                    //Check for front cross and front-to-rear cross
                    lfcrPass = AnalyzeLFValveCrossFrontFirst(minIndex);
                    rfcrPass = AnalyzeRFValveCrossFrontFirst(minIndex);

                    //MAM 2/23/12 - Only check for LR/RR cross if 4-channel ABS
                    if ( IsFourChannelSystem() )
                    {
                        lrcrPass = AnalyzeLRValveCrossFrontFirst(minIndex);
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "3 channel ABS - NOT checking LR/RR valve cross\n");
                        lrcrPass = true;
                    }
                    rrcrPass = true;
                }

            }
            else
            {
                //MAM 11/3/11
                // Error reading brake force data
                Log(LOG_ERRORS, "Error reading brake force data \n");                

                testResultCode = GetFaultCode("WheelForceDataReadError");
                testDescription = GetFaultDescription("WheelForceDataReadError");

                // Valve cross test fails
                //testResultCode = GetFaultCode("ABSValveCross");
                //testDescription = GetFaultDescription("ABSValveCross");;

                SendSubtestResult("ABSValveCrossCheck", testFail, testDescription, testResultCode);
            }

        }

        // if no valve cross detected        
        if ((lfcrPass) && (rfcrPass) && (lrcrPass) && (rrcrPass))
        {
            result = BEP_STATUS_SUCCESS;
        }
        // else report fail
        else
        {
            result = BEP_STATUS_FAILURE;
        }
    }
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::AnalyzeValveCross() lfcrPass=%d rfcrPass=%d lrcrPass=%d rrcrPass=%d \n",
        lfcrPass, rfcrPass, lrcrPass, rrcrPass);

    return(result);
}


template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCrossNew(INT32 *minIndex)
{
    bool    lfcrPass;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: minIndex[LF]=%d minIndex[RF]=%d minIndex[LR]=%d minIndex[RR]=%d \n", 
         minIndex[LFWHEEL], minIndex[RFWHEEL], minIndex[LRWHEEL], minIndex[RRWHEEL]);


    // Check for LF crossed with RF
    if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
         (minIndex[LFWHEEL] < minIndex[LRWHEEL]) &&
         (minIndex[LFWHEEL] < minIndex[RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    // Check for LF crossed with LR
    else if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
              (minIndex[LFWHEEL] > minIndex[LRWHEEL]) &&
              (minIndex[LFWHEEL] < minIndex[RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: LF crossed with LR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultName("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
        lfcrPass = false;
    }
    //Check for LF crossed with RR        
    else if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
              (minIndex[LFWHEEL] > minIndex[LRWHEEL]) &&
              (minIndex[LFWHEEL] > minIndex[RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: LF crossed with RR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
        lfcrPass = false;
    }
    //MAM 2/9/11 - In case the data is wacked out...
    // Check for LF index > LR index
    else if (minIndex[LFWHEEL] >= minIndex[LRWHEEL])
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: LF index >= LR index - bad data!\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultName("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
        lfcrPass = false;
    }
    //Check for LF index > RR index
    else if (minIndex[LFWHEEL] >= minIndex[RRWHEEL])
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossNew: LF index >= RR index - bad data!\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
        lfcrPass = false;
    }
    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");

    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLRValveCrossRearFirst(INT32 *minIndex)
{
    bool    lrcrPass;
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;

    Log( LOG_DEV_DATA, "AnalyzeLRValveCrossRearFirst: minIndex[LR]=%d \n", minIndex[LRWHEEL]); 

    //get the force values for each wheel at the LR min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[LRWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "LR valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //LR force should be lower than other forces at the LR min index point
    if (LRforce > RRforce)
    {
        Log( LOG_DEV_DATA, "AnalyzeLRValveCrossRearFirst: LR crossed with RR\n");

        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                    GetFaultName("LeftRearValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        lrcrPass = false;
    }
    // Check for LR crossed with LF
    else if (LRforce > LFforce)
    {
        Log( LOG_DEV_DATA, "AnalyzeLRValveCrossRearFirst: LR crossed with LF\n");
        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithLeftFront"),
                                    GetFaultName("LeftRearValveCrossedWithLeftFront"),
                                    GetFaultDescription("LeftRearValveCrossedWithLeftFront"));
        lrcrPass = false;
    }
    //Check for LR crossed with RF        
    else if (LRforce > RFforce)
    {
        Log( LOG_DEV_DATA, "AnalyzeLRValveCrossRearFirst: LR crossed with RF\n");
        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightFront"),
                                    GetFaultName("LeftRearValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightFront"));
        lrcrPass = false;
    }
    else
    {
        lrcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LRValveCross", testPass, "LR Valve Cross","0000");

    }

    return( lrcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLRValveCrossFrontFirst(INT32 *minIndex)
{
    bool    lrcrPass;
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;    

    Log( LOG_DEV_DATA, "AnalyzeLRValveCrossFrontFirst: minIndex[LR]=%d \n", minIndex[LRWHEEL]); 

    //get the force values for each wheel at the LR min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[LRWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "LR valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //LR force should be lower than other forces at the LR min index point
    if (LRforce > RRforce)
    {
        Log( LOG_DEV_DATA, "AnalyzeLRValveCrossFrontFirst: LR crossed with RR\n");

        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                    GetFaultName("LeftRearValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        lrcrPass = false;
    }
    else
    {
        lrcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LRValveCross", testPass, "LR Valve Cross","0000");

    }

    return( lrcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCrossLimSlip(INT32 *minIndex)
{
    bool    lfcrPass;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossLimSlip: minIndex[LF]=%d minIndex[RF]=%d minIndex[LR]=%d minIndex[RR]=%d \n", 
         minIndex[LFWHEEL], minIndex[RFWHEEL], minIndex[LRWHEEL], minIndex[RRWHEEL]);


    // Check for LF crossed with RF
    if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
         (minIndex[LFWHEEL] < minIndex[LRWHEEL]) &&
         (minIndex[LFWHEEL] < minIndex[RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossLimSlip: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    // Check for LF crossed with LR
    else if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
              (minIndex[LFWHEEL] > minIndex[LRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossLimSlip: LF crossed with LR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultName("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
        lfcrPass = false;
    }
    //Check for LF crossed with RR        
    else if ( (minIndex[LFWHEEL] > minIndex[RFWHEEL]) &&
              (minIndex[LFWHEEL] > minIndex[RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossLimSlip: LF crossed with RR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
        lfcrPass = false;
    }

    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");

    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRFValveCrossNew(INT32 *minIndex)
{
    bool    rfcrPass;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossNew: minIndex[LF]=%d minIndex[RF]=%d minIndex[LR]=%d minIndex[RR]=%d \n", 
         minIndex[LFWHEEL], minIndex[RFWHEEL], minIndex[LRWHEEL], minIndex[RRWHEEL]);

    // Check for RF crossed with LR
    if ( (minIndex[ RFWHEEL] > minIndex[ LRWHEEL]) &&
         (minIndex[ RFWHEEL] < minIndex[ RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossNew: RF crossed with LR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
        rfcrPass = false;
    }
    // Check for RF crossed with RR
    else if ( (minIndex[ RFWHEEL] > minIndex[ LRWHEEL]) &&
              (minIndex[ RFWHEEL] > minIndex[ RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossNew: RF crossed with RR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                    GetFaultName("RightFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithRightRear"));
        rfcrPass = false;
    }
    //MAM 2/9/11 - In case the data is wacked out...
    // Check for RF index > LR index
    else if (minIndex[RFWHEEL] >= minIndex[LRWHEEL])
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossNew: RF index >= LR index - bad data!\n");
        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
        rfcrPass = false;
    }
    //Check for RF index > RR index
    else if (minIndex[RFWHEEL] >= minIndex[RRWHEEL])
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossNew: RF index >= RR index - bad data!\n");
        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                    GetFaultName("RightFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithRightRear"));
        rfcrPass = false;
    }
    else
    {
        rfcrPass = true;
        //MAM 12/14/06
        SendSubtestResultWithDetail("RFValveCross", testPass, "RF Valve Cross","0000");
    }

    return( rfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRRValveCrossRearFirst(INT32 *minIndex)
{
    bool    rrcrPass;    
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;

    Log( LOG_DEV_DATA, "AnalyzeRRValveCrossRearFirst: minIndex[RR]=%d \n", minIndex[RRWHEEL]); 

    //get the force values for each wheel at the RR min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[RRWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "RR valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //RR force should be lower than other forces at the RR min index point           
    // Check for RR crossed with LF
    if (RRforce > LFforce)
    {

        Log( LOG_DEV_DATA, "AnalyzeRRValveCrossRearFirst: RR crossed with LF\n");

        SendSubtestResultWithDetail("RRValveCross", testFail, "RR Valve Cross",
                                    GetFaultCode("RightRearValveCrossedWithLeftFront"),
                                    GetFaultName("RightRearValveCrossedWithLeftFront"),
                                    GetFaultDescription("RightRearValveCrossedWithLeftFront"));
        rrcrPass = false;
    }
    // Check for RR crossed with RF
    else if (RRforce > RFforce)
    {
        Log( LOG_DEV_DATA, "AnalyzeRRValveCrossRearFirst: RR crossed with RF\n");

        SendSubtestResultWithDetail("RRValveCross", testFail, "RR Valve Cross",
                                    GetFaultCode("RightRearValveCrossedWithRightFront"),
                                    GetFaultName("RightRearValveCrossedWithRightFront"),
                                    GetFaultDescription("RightRearValveCrossedWithRightFront"));
        rrcrPass = false;
    }
    else
    {
        rrcrPass = true;
        //MAM 12/14/06
        SendSubtestResultWithDetail("RRValveCross", testPass, "RR Valve Cross","0000");
    }

    return( rrcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRFValveCrossLimSlip(INT32 *minIndex)
{
    bool    rfcrPass;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossLimSlip: minIndex[LF]=%d minIndex[RF]=%d minIndex[LR]=%d minIndex[RR]=%d \n", 
         minIndex[LFWHEEL], minIndex[RFWHEEL], minIndex[LRWHEEL], minIndex[RRWHEEL]);

    // Check for RF crossed with LR
    if ( (minIndex[ RFWHEEL] > minIndex[ LRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossLimSlip: RF crossed with LR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
        rfcrPass = false;
    }
    // Check for RF crossed with RR
    else if ( (minIndex[ RFWHEEL] > minIndex[ LRWHEEL]) &&
              (minIndex[ RFWHEEL] > minIndex[ RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossLimSlip: RF crossed with RR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                    GetFaultName("RightFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithRightRear"));
        rfcrPass = false;
    }
    else
    {
        rfcrPass = true;
        //MAM 12/14/06
        SendSubtestResultWithDetail("RFValveCross", testPass, "RF Valve Cross","0000");
    }

    return( rfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLRValveCrossNew(INT32 *minIndex)
{
    bool    lrcrPass=true;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLRValveCrossNew: minIndex[LF]=%d minIndex[RF]=%d minIndex[LR]=%d minIndex[RR]=%d \n", 
         minIndex[LFWHEEL], minIndex[RFWHEEL], minIndex[LRWHEEL], minIndex[RRWHEEL]);

    // If a 4 valve system, compare rears against each other
    if ( (IsFourChannelSystem()) && (minIndex[ LRWHEEL] > minIndex[ RRWHEEL]))
    {
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLRValveCrossNew: LR crossed with RR\n");

        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                    GetFaultName("LeftRearValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        lrcrPass = false;
    }
    else
    {
        lrcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LRValveCross", testPass, "LR Valve Cross","0000");
    }

    return( lrcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCrossRearFirst(INT32 *minIndex)
{
    bool    lfcrPass=true;
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;

    Log( LOG_DEV_DATA, "AnalyzeLFValveCrossRearFirst: minIndex[LF]=%d \n", minIndex[LFWHEEL]); 

    //get the force values for each wheel at the LF min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[LFWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "LF valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //LF force should be lower than other forces at the LF min index point           
    // Check for LF crossed with RF
    if (LFforce > RFforce)
    {

        Log( LOG_DEV_DATA, "AnalyzeLFValveCrossRearFirst: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");
    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCrossFrontFirst(INT32 *minIndex)
{
    bool    lfcrPass=true;
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;
    float refForce = 0;   //MAM 4/20/12

    Log( LOG_DEV_DATA, "AnalyzeLFValveCrossFrontFirst: minIndex[LF]=%d \n", minIndex[LFWHEEL]); 

    //get the force values for each wheel at the LF min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[LFWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "LF valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //LF force should be lower than other forces at the LF min index point           
    // Check for LF crossed with RF
    if (LFforce > RFforce)
    {

        Log( LOG_DEV_DATA, "AnalyzeLFValveCrossFrontFirst: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    else if (LFforce > LRforce)
    {
        //MAM 4/20/12 - Verify we had a LR force reduction by comparing LR force to 1/2 the ABS ref force
        refForce = GetAbsReferenceForce( 2)*0.5;
        Log( LOG_DEV_DATA, "LFforce > LRforce: LRref = %2.2f\n", refForce);

        if (LRforce < refForce)
        {
            Log( LOG_DEV_DATA, "AnalyzeLFValveCrossFrontFirst: LF crossed with LR\n");
            SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                        GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                        GetFaultName("LeftFrontValveCrossedWithLeftTRear"),
                                        GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
            lfcrPass = false;
        }
    }
    else if (LFforce > RRforce)
    {
        //MAM 4/20/12 - Verify we had a RR force reduction by comparing RR force to 1/2 the ABS ref force
        refForce = GetAbsReferenceForce( 3)*0.5;
        Log( LOG_DEV_DATA, "LFforce > RRforce: RRref = %2.2f\n", refForce);

        if (RRforce < refForce)
        {
            Log( LOG_DEV_DATA, "AnalyzeLFValveCrossFrontFirst: LF crossed with RR\n");
            SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                        GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                        GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                        GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
            lfcrPass = false;
        }
    }
    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");
    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRFValveCrossFrontFirst(INT32 *minIndex)
{
    bool    rfcrPass=true;
    vector<float>   forceData;
    int ii;
    float CrossForce[4];
    float LFforce, RFforce, LRforce, RRforce;
    float refForce = 0;   //MAM 4/20/12

    Log( LOG_DEV_DATA, "AnalyzeRFValveCrossFrontFirst: minIndex[RF]=%d \n", minIndex[RFWHEEL]); 

    //get the force values for each wheel at the LF min force index
    for (ii=0; ii<4; ii++)
    {
        forceData.clear();
        m_baseBrakeTool->GetWheelForceArray( ii, forceData);
        CrossForce[ii] = forceData[minIndex[RFWHEEL]];
    }

    LFforce = CrossForce[0];
    RFforce = CrossForce[1];
    LRforce = CrossForce[2];
    RRforce = CrossForce[3];

    Log( LOG_DEV_DATA, "RF valve cross check force values: LF %2.2f, RF %2.2f, LR %2.2f, RR %2.2f \n",
         LFforce, RFforce, LRforce, RRforce);

    //RF force should be lower than other forces at the RF min index point           
    // Check for RF crossed with LR
    if (RFforce > LRforce)
    {
        //MAM 4/20/12 - Verify we had a LR force reduction by comparing LR force to 1/2 the ABS ref force
        refForce = GetAbsReferenceForce( 2)*0.5;
        Log( LOG_DEV_DATA, "RFforce > LRforce: LRref = %2.2f\n", refForce);

        if (LRforce < refForce)
        {
            Log( LOG_DEV_DATA, "AnalyzeRFValveCrossFrontFirst: RF crossed with LR\n");

            SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                        GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                        GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                        GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
            rfcrPass = false;
        }
    }
    else if (RFforce > RRforce)
    {
        //MAM 4/20/12 - Verify we had a RR force reduction by comparing RR force to 1/2 the ABS ref force
        refForce = GetAbsReferenceForce( 3)*0.5;
        Log( LOG_DEV_DATA, "RFforce > RRforce: RRref = %2.2f\n", refForce);

        if (RRforce < refForce)
        {
            Log( LOG_DEV_DATA, "AnalyzeRFValveCrossFrontFirst: RF crossed with RR\n");

            SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                        GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                        GetFaultName("RightFrontValveCrossedWithRightRear"),
                                        GetFaultDescription("RightFrontValveCrossedWithRightRear"));
            rfcrPass = false;
        }
    }
    else
    {
        rfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("RFValveCross", testPass, "RF Valve Cross","0000");
    }

    return( rfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCross()
{
    bool    lfcrPass;

    Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCross: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].reductionEnd, m_reduxRecovIndex[ RFWHEEL].reductionEnd, 
         m_reduxRecovIndex[ LRWHEEL].reductionEnd, m_reduxRecovIndex[ RRWHEEL].reductionEnd);

    // Check for LF crossed with RF
    if ( (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ RFWHEEL].reductionEnd) &&
         (m_reduxRecovIndex[ LFWHEEL].reductionEnd < m_reduxRecovIndex[ LRWHEEL].reductionEnd) &&
         (m_reduxRecovIndex[ LFWHEEL].reductionEnd < m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCross: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    // Check for LF crossed with LR
    else if ( (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ RFWHEEL].reductionEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ LRWHEEL].reductionEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].reductionEnd < m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCross: LF crossed with LR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultName("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
        lfcrPass = false;
    }
    // Check for LF crossed with RR
    else if ( (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ RFWHEEL].reductionEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ LRWHEEL].reductionEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].reductionEnd > m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCross: LF crossed with RR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
        lfcrPass = false;
    }
    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");

    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRFValveCross()
{
    bool    rfcrPass;

    Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCross: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].reductionEnd, m_reduxRecovIndex[ RFWHEEL].reductionEnd, 
         m_reduxRecovIndex[ LRWHEEL].reductionEnd, m_reduxRecovIndex[ RRWHEEL].reductionEnd);

    // Check for RF crossed with LR
    if ( (m_reduxRecovIndex[ RFWHEEL].reductionEnd > m_reduxRecovIndex[ LRWHEEL].reductionEnd) &&
         (m_reduxRecovIndex[ RFWHEEL].reductionEnd < m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCross: RF crossed with LR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
        rfcrPass = false;
    }
    // Check for RF crossed with RR
    else if ( (m_reduxRecovIndex[ RFWHEEL].reductionEnd > m_reduxRecovIndex[ LRWHEEL].reductionEnd) &&
              (m_reduxRecovIndex[ RFWHEEL].reductionEnd > m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCross: RF crossed with RR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                    GetFaultName("RightFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithRightRear"));
        rfcrPass = false;
    }
    else
    {
        rfcrPass = true;
        //MAM 12/14/06
        SendSubtestResultWithDetail("RFValveCross", testPass, "RF Valve Cross","0000");
    }

    return( rfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLRValveCross()
{
    bool    lrcrPass=true;

    Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCross: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].reductionEnd, m_reduxRecovIndex[ RFWHEEL].reductionEnd, 
         m_reduxRecovIndex[ LRWHEEL].reductionEnd, m_reduxRecovIndex[ RRWHEEL].reductionEnd);

    // If a 4 valve system, compare rears against each other
    if ( (IsFourChannelSystem()) && (m_reduxRecovIndex[ LRWHEEL].reductionEnd > m_reduxRecovIndex[ RRWHEEL].reductionEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLRValveCross: LR crossed with RR\n");

        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                    GetFaultName("LeftRearValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        lrcrPass = false;
    }
    else
    {
        lrcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LRValveCross", testPass, "LR Valve Cross","0000");
    }

    return( lrcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLFValveCrossSBA()
{
    bool    lfcrPass;    

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossSBA: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].recoveryEnd, m_reduxRecovIndex[ RFWHEEL].recoveryEnd, 
         m_reduxRecovIndex[ LRWHEEL].recoveryEnd, m_reduxRecovIndex[ RRWHEEL].recoveryEnd);

    //check for valve cross - RR, LR, LF, RF order    
    // Check for LF crossed with RF
    if ( (m_reduxRecovIndex[ LFWHEEL].recoveryEnd > m_reduxRecovIndex[ RFWHEEL].recoveryEnd) &&
         (m_reduxRecovIndex[ LFWHEEL].recoveryEnd > m_reduxRecovIndex[ LRWHEEL].recoveryEnd) &&
         (m_reduxRecovIndex[ LFWHEEL].recoveryEnd > m_reduxRecovIndex[ RRWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCrossSBA: LF crossed with RF\n");

        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        lfcrPass = false;
    }
    // Check for LF crossed with LR
    else if ( (m_reduxRecovIndex[ LFWHEEL].recoveryEnd < m_reduxRecovIndex[ RFWHEEL].recoveryEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].recoveryEnd < m_reduxRecovIndex[ LRWHEEL].recoveryEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].recoveryEnd > m_reduxRecovIndex[ RRWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCrossSBA: LF crossed with LR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultName("LeftFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithLeftRear"));
        lfcrPass = false;
    }
    // Check for LF crossed with RR
    else if ( (m_reduxRecovIndex[ LFWHEEL].recoveryEnd < m_reduxRecovIndex[ RFWHEEL].recoveryEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].recoveryEnd < m_reduxRecovIndex[ LRWHEEL].recoveryEnd) &&
              (m_reduxRecovIndex[ LFWHEEL].recoveryEnd < m_reduxRecovIndex[ RRWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLFValveCrossSBA: LF crossed with RR\n");
        SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                    GetFaultCode("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultName("LeftFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftFrontValveCrossedWithRightRear"));
        lfcrPass = false;
    }
    else
    {
        lfcrPass = true;
        //MAM 12/14/06        
        SendSubtestResultWithDetail("LFValveCross", testPass, "LF Valve Cross","0000");
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLFValveCrossSBA passed!\n");
    }

    return( lfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeRFValveCrossSBA()
{
    bool    rfcrPass;

    Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossSBA: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].recoveryEnd, m_reduxRecovIndex[ RFWHEEL].recoveryEnd, 
         m_reduxRecovIndex[ LRWHEEL].recoveryEnd, m_reduxRecovIndex[ RRWHEEL].recoveryEnd);

    //check for valve cross - RR, LR, LF, RF order
    // Check for RF crossed with LR
    if ( (m_reduxRecovIndex[ RFWHEEL].recoveryEnd < m_reduxRecovIndex[ LRWHEEL].recoveryEnd) &&
         (m_reduxRecovIndex[ RFWHEEL].recoveryEnd > m_reduxRecovIndex[ RRWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCrossSBA: RF crossed with LR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultName("RightFrontValveCrossedWithLeftRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithLeftRear"));
        rfcrPass = false;
    }
    // Check for RF crossed with RR
    else if ( (m_reduxRecovIndex[ RFWHEEL].recoveryEnd < m_reduxRecovIndex[ LRWHEEL].recoveryEnd) &&
              (m_reduxRecovIndex[ RFWHEEL].recoveryEnd < m_reduxRecovIndex[ RRWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeRFValveCrossSBA: RF crossed with RR\n");

        SendSubtestResultWithDetail("RFValveCross", testFail, "RF Valve Cross",
                                    GetFaultCode("RightFrontValveCrossedWithRightRear"),
                                    GetFaultName("RightFrontValveCrossedWithRightRear"),
                                    GetFaultDescription("RightFrontValveCrossedWithRightRear"));
        rfcrPass = false;
    }
    else
    {
        rfcrPass = true;     
        SendSubtestResultWithDetail("RFValveCross", testPass, "RF Valve Cross","0000");
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeRFValveCrossSBA passed!\n");
    }

    return( rfcrPass);
}

template <class ModuleType>
bool FordABSTCTemplate<ModuleType>::AnalyzeLRValveCrossSBA()
{
    bool    lrcrPass=true;

    Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLRValveCrossSBA: LF=%d RF=%d LR=%d RR=%d\n", 
         m_reduxRecovIndex[ LFWHEEL].recoveryEnd, m_reduxRecovIndex[ RFWHEEL].recoveryEnd, 
         m_reduxRecovIndex[ LRWHEEL].recoveryEnd, m_reduxRecovIndex[ RRWHEEL].recoveryEnd);

    //check for valve cross - RR, LR, LF, RF order
    if ( (m_reduxRecovIndex[ LRWHEEL].recoveryEnd < m_reduxRecovIndex[ RRWHEEL].recoveryEnd) &&
         (m_reduxRecovIndex[ RRWHEEL].recoveryEnd < m_reduxRecovIndex[ LFWHEEL].recoveryEnd) &&
         (m_reduxRecovIndex[ RRWHEEL].recoveryEnd < m_reduxRecovIndex[ RFWHEEL].recoveryEnd))
    {
        Log( LOG_ERRORS, "FordABSTCTemplate::AnalyzeLRValveCrossSBA: LR crossed with RR\n");

        SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                    GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                    GetFaultName("LeftRearValveCrossedWithRightRear"),
                                    GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        lrcrPass = false;
    }
    else
    {
        lrcrPass = true;      
        SendSubtestResultWithDetail("LRValveCross", testPass, "LR Valve Cross","0000");
        Log( LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeLRValveCrossSBA passed!\n");
    }

    return( lrcrPass);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::IsolateAll()
{
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string testResult            = BEP_TESTING_STATUS;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::IsolateAll()\n");
    // Determine if the test should be performed
    if (!ShortCircuitTestStep())
    {   // Attempt to run the motor
        try
        {
            moduleStatus = m_vehicleModule.Isolate( "All");
            // Check the status of the command
            Log(LOG_DEV_DATA, "Isolate All Valves: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
            testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
        }
        catch (ModuleException &excpt)
        {   // Something bad happened
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::IsolateAll(), status=%s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::TestStepNeutral()
{
    std::string     testResult(testPass);
    std::string     faultCode( "0000");
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    const float     neutralDecel = -1 * GetParameterFloat( "NeutralDecelRate");
    float           avgAccel = neutralDecel+1;
    int             scanDelay = GetTestStepInfoInt( "ScanDelay");
    int             icmFd = open( m_icmAccelFile.c_str(), O_RDONLY);
    int             bytesRead, errCount = 0;
    float           accelRate[ GetRollerCount()];
    float           whlDecel[4] = {0};    //MAM 8/16/11
    bool            rearWd = ReadSubscribeData( GetDataTag("DriveAxle")) == REAR_WHEEL_DRIVE_VALUE;
    //MAM 3/8/12
    bool            frontWd = ReadSubscribeData( GetDataTag("DriveAxle")) == FRONT_WHEEL_DRIVE_VALUE;
    bool            allWd = ReadSubscribeData( GetDataTag("DriveAxle")) == ALL_WHEEL_DRIVE_TAG;    

    char            temp[ 256];

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepNeutral()\n");

    // Determine if the test should be performed
    if (!ShortCircuitTestStep())
    {
        try
        {
            if ( scanDelay <= 0)     scanDelay = 100;

            // Display Prompts
            UpdatePrompts();

            status = StatusSleep( GetParameterInt( "NeutralDelay"));

            // Wait for all decel rates to fall below the neutral decel threshold
            while ( (true == TimeRemaining()) && (BEP_STATUS_SUCCESS == status) &&
                    (avgAccel > neutralDecel) )
            {
                // Set outside limits in case error reading values
                avgAccel = neutralDecel+1;

                // Read the accel rates
                bytesRead = read( icmFd, accelRate, sizeof( accelRate));
                if ( bytesRead == (int)sizeof( accelRate))
                {
                    //MAM 8/16/11 - Don't want to use the same variable for reading data and logging
                    //inside of while loop
                    whlDecel[0] = accelRate[0];
                    whlDecel[1] = accelRate[1];
                    whlDecel[2] = accelRate[2];
                    whlDecel[3] = accelRate[3];

                    status = StatusSleep( scanDelay);
                    if ( BEP_STATUS_SUCCESS == status)
                    {
                        //MAM 3/8/12 - Use all 4 wheels for awd average accel/decel rate
                        Log( LOG_DEV_DATA, "TestStepNeutral: DriveAxle= RWD %d FWD %d AWD %d\n",rearWd, frontWd, allWd);
                        // Calculate the average drive axle accel rate
                        if ( rearWd)     avgAccel = (whlDecel[2] + whlDecel[3])/2;
                        else if ( frontWd)    avgAccel = (whlDecel[0] + whlDecel[1])/2;
                        else  avgAccel = (whlDecel[0] + whlDecel[1] + whlDecel[2] + whlDecel[3])/4;

                        Log( LOG_DEV_DATA, "TestStepNeutral: Decel=%.3f (%.3f, %.3f, %.3f, %.3f), Limit: < %.3f\n",
                             avgAccel, whlDecel[0], whlDecel[1], whlDecel[2], whlDecel[3], neutralDecel);
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Error waiting for neutral decel, status=%d\n", status);
                        avgAccel = neutralDecel+1;
                    }
                }
                else
                {
                    errCount++;
                    Log(LOG_ERRORS, "%d Error reading accel data, status=%d\n", errCount, status);
                    avgAccel = neutralDecel+1;
                    if ( errCount == 3)
                    {
                        status = BEP_STATUS_SOFTWARE;
                    }
                }
            }
        }
        catch ( BepException &err)
        {
            Log( LOG_ERRORS, "Exception in TestStepNeutral: %s\n", err.GetReason());
            status = BEP_STATUS_SOFTWARE;
        }

        // If we saw the desired decel rates
        if ( (BEP_STATUS_SUCCESS == status) && (avgAccel <= neutralDecel))
        {
            testResult = testPass;
            faultCode = "000";
        }
        else
        {
            // If we simply didn't meet the accel criteria
            if ( BEP_STATUS_SUCCESS == status)       status = BEP_STATUS_FAILURE;
            // Update overall test step status
            UpdateResult( status, testResult);
            faultCode = GetFaultCode( "NeutralCheckFault");
        }

        // Remove Prompts
        RemovePrompts();

        Log( LOG_DEV_DATA, "Neutral Test: LF=%.3f RF=%.3f LR=%.3f RR=%.3f, Tolerance: %.3f\n",
             accelRate[0], accelRate[1], accelRate[2], accelRate[3], neutralDecel);
    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }

    close( icmFd);

    SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), faultCode,
                             "LeftFrontDecel", CreateMessage(temp, 256, "%.2f", accelRate[0]), "MPH/sec",
                             "RightFrontDecel", CreateMessage(temp, 256, "%.2f", accelRate[1]), "MPH/sec",
                             "LeftRearDecel", CreateMessage(temp, 256, "%.2f", accelRate[2]), "MPH/sec",
                             "RightRearDecel", CreateMessage(temp, 256, "%.2f", accelRate[3]), "MPH/sec",
                             "NeutralDecelRate", CreateMessage(temp, 256, "%.2f", neutralDecel), "MPH/sec");

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepNeutral(), status=%s\n", testResult.c_str());

    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::TestStepReportBrakeData()
{
    std::string             testResult(testPass);
    std::string             dataFilePath( GetParameter("BrakeForceLogFilePath"));
    const std::string       forceFileName( "BrakeForce.log");
    const std::string       speedFileName( "Speed.log");
    std::string             dataFileName;
    std::string             forceData, speedData;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepReportBrakeData()\n");

    // If no parameter given for path to ICM log files
    if ( dataFilePath.empty())
    {
        // Default to the standard path
        dataFilePath = std::string( "/Data/TestResults/Machine/BrakeForceData/");
    }

    /**
     * Read the brake force data
     */
    dataFileName = dataFilePath + forceFileName;
    if ( ReadIcmDataLogFile( dataFileName, forceData) <= 0)
    {
        forceData.clear();
        Log( LOG_ERRORS, "Unable to read data for %s\n", dataFileName.c_str());
    }

    /**
     * Read the speed data
     */
    dataFileName = dataFilePath + speedFileName;
    if ( ReadIcmDataLogFile( dataFileName, speedData) <= 0)
    {
        speedData.clear();
        Log( LOG_ERRORS, "Unable to read data for %s\n", dataFileName.c_str());
    }

    SendTestResultWithDetail( testResult, "Brake Force Data", "0000",
                              "BrakeData", forceData, "",
                              "SpeedData", speedData, "");

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepReportBrakeData(), status=%s\n", testResult.c_str());

    return( testResult);
}

template <class ModuleType>
int FordABSTCTemplate<ModuleType>::ReadIcmDataLogFile( const std::string &fileName, std::string &fileData)
{
    char            *data;
    int             fd;
    struct stat     fileStat;
    int             bytesRead = 0;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::ReadIcmDataLogFile(%s)\n", fileName.c_str());

    memset( &fileStat, 0, sizeof( fileStat));
    if ( (-1 != stat(fileName.c_str(), &fileStat)) && (fileStat.st_size > 0))
    {
        data = new char[fileStat.st_size+1];
        memset( data, 0, fileStat.st_size+1);

        if ( -1 != (fd = open( fileName.c_str(), O_RDONLY)))
        {
            if ((bytesRead=read( fd, data, fileStat.st_size)) > 0)
            {
                fileData = std::string(data, bytesRead);
            }
            else
            {
                Log( LOG_ERRORS, "Unable to read() %s\n", fileName.c_str());
            }

            close( fd);
        }
        else
        {
            Log( LOG_ERRORS, "Unable to open() %s\n", fileName.c_str());
        }
        delete[] data;
    }
    else
    {
        Log( LOG_ERRORS, "Unable to get stat() for %s\n", fileName.c_str());
    }

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::ReadIcmDataLogFile(%s), status=%d\n", fileName.c_str(), bytesRead);

    return( bytesRead);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::HydraulicParkBrakeInit()
{
    INT32 status;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::HydraulicParkBrakeInit()\n");
    // Wait for the driver to shift/apply park brake
    status = GenericTC::HydraulicParkBrakeInit();
    if ( BEP_STATUS_SUCCESS == status)
    {
        if ( true == GetParameterBool( "ParkBrakePedalCheck"))
        {
            status = ParkBrakePawlPedalCheck();
        }
    }
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::HydraulicParkBrakeInit(), status=%d\n", status);

    return( status);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::ElectricParkBrakeInit()
{
    INT32 status;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ElectricParkBrakeInit()\n");
    // Wait for the driver to shift/apply park brake
    status = GenericTC::ElectricParkBrakeInit();
    if ( BEP_STATUS_SUCCESS == status)
    {
        if ( true == GetParameterBool( "ParkBrakePedalCheck"))
        {
            status = ParkBrakePawlPedalCheck();
        }
    }
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::ElectricParkBrakeInit(), status=%d\n", status);

    return( status);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::ParkBrakePawlPedalCheck()
{
    INT32 status;
    bool  brakeOn = false; //MAM 3/23/07
    bool promptDisplayed = false;
    int brakeOnCount = 0;   //MAM 6/3/10
    SerialString_t  response; //MAM 8/13/11 - for pedal travel distance on hybrids
    short travelDistance = 0;
    //MAM 10/24/14
    //string testStatus(testFail);
    string faultTag;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ParkBrakePawlPedalCheck()\n");

    if (CheckCableConnect())
    {
        // Need to perform this test step
        try
        {

            //MAM 3/23/07 - Make sure brakes are not being applied at beginning of test
            Log( LOG_DEV_DATA, "Checking BrakeSwitch in ParkBrakePawlPedalCheck()\n");

            //MAM 8/13/11 - Reading pedal travel distance for SBA brake system (hybrids)            
            if (GetParameterBool("SBABrakeSystem"))
            {
                status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
                if (status == BEP_STATUS_SUCCESS)
                {
                    Log(LOG_DEV_DATA, "ReadPedalTravel response = %2X %2X %2X %2X %2X\n",
                        response[0], response[1], response[2], response[3], response[4]);
                    //travelDistance = (short)response[2];
                    travelDistance = (short)response[3]; //MAM 10/29/08
                    travelDistance = travelDistance << 8;
                    //travelDistance = travelDistance + response[3];
                    travelDistance = travelDistance + response[4]; //MAM 10/29/08
                    Log(LOG_DEV_DATA, "travelDistance = %d\n", travelDistance);

                    if (travelDistance >= 1000)
                    {
                        brakeOn = true;                        
                    }
                }
            }
            else
            {
                status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
            }

            if ( (BEP_STATUS_SUCCESS == status) && (true == brakeOn) )
            {
                Log( LOG_DEV_DATA, "BrakeSwitch ON detected in ParkBrakePawlPedalCheck()\n");
                Log( LOG_DEV_DATA, "Prompting driver to remove foot from brake\n");
                //DisplayPrompt( GetPromptBox( "KeepFootOffBrake"), GetPrompt( "KeepFootOffBrake"), GetPromptPriority( "KeepFootOffBrake"));
                DisplayPrompt( 2, GetPrompt( "KeepFootOffBrake"), 0);
                promptDisplayed = true;
            }

            while ( (brakeOnCount < 5) && (BEP_STATUS_SUCCESS == status) &&
                    (true == brakeOn) )
            {
                BposSleep(500); //delay 500 ms

                //continue to check brake switch

                if (GetParameterBool("SBABrakeSystem"))
                {
                    status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
                    if (status == BEP_STATUS_SUCCESS)
                    {
                        Log(LOG_DEV_DATA, "ReadPedalTravel response = %2X %2X %2X %2X %2X\n",
                            response[0], response[1], response[2], response[3], response[4]);
                        //travelDistance = (short)response[2];
                        travelDistance = (short)response[3]; //MAM 10/29/08
                        travelDistance = travelDistance << 8;
                        //travelDistance = travelDistance + response[3];
                        travelDistance = travelDistance + response[4]; //MAM 10/29/08
                        Log(LOG_DEV_DATA, "travelDistance = %d\n", travelDistance);

                        if (travelDistance >= 1000)
                        {
                            brakeOn = true;
                            Log(LOG_DEV_DATA, "Brake On %d\n", brakeOnCount);
                        }
                        else
                        {
                            brakeOn = false;
                            Log(LOG_DEV_DATA, "Brake Off %d\n", brakeOnCount);
                        }
                    }
                }
                else
                {
                    status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
                    if (brakeOn)
                    {
                        Log(LOG_DEV_DATA, "Brake On %d\n", brakeOnCount);
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "Brake Off %d\n", brakeOnCount);
                    }
                }

                brakeOnCount++; //500 ms * 6 counts = 3 seconds to remove foot from brake pedal

            }
            //remove warning prompt if displayed            
            if (true == promptDisplayed)
            {
                Log( LOG_DEV_DATA, "Removing prompt to remove foot from brake\n");
                RemovePrompt("2",GetPrompt("KeepFootOffBrake"), 0);
            }

            //if brake not on
            if ( (BEP_STATUS_SUCCESS == status) && (false == brakeOn) && (true == TimeRemaining()))
            {
                // Reset the brake switch on/off status
                Log( LOG_DEV_DATA, "Clearing brake switch status in ParkBrakePawlPedalCheck()\n");
                m_vehicleModule.ClearBrakeSwitchStatus();
                // Try to start the switch monitor thread
                //MAM 6/3/10 - commented out line below - checking brake switch during ParkPawlTorqueTest()
                //status = m_vehicleModule.StartSwitchMonitor();
            }
            else if ( true == brakeOn)
            { // If brake switch stayed on
                Log( LOG_ERRORS, "Saw brake switch ON in ParkBrakePawlPedalCheck()\n");
                //Set the brake switch status - should fail driver influence check 
                m_vehicleModule.SetBrakeSwitchStatusOn();
                status = BEP_STATUS_FAILURE;
            }
            else if ( false == TimeRemaining())
            {
                Log( LOG_ERRORS, "Timeout error during ParkBrakePawlPedalCheck()\n");
                status = BEP_STATUS_TIMEOUT;
            }
            else
            { //status value is fail
                Log( LOG_ERRORS, "Error reading brake switch status during ParkBrakePawlPedalCheck()\n");
                //faultTag = "CommunicationFailure";
                status = BEP_STATUS_FAILURE;
                //MAM 10/24/14
                //SetCommunicationFailure(true);
                Log( LOG_ERRORS, "Setting communication failure in ParkBrakePawlPedalCheck()\n");
                faultTag = "CommunicationFailure";
                // Send the overall test result
                SendTestResultWithDetail(testFail, 
                                         GetTestStepInfo("Description"),
                                         GetFaultCode(faultTag),
                                         GetFaultName(faultTag),
                                         GetFaultDescription(faultTag));
            }

        }
        catch (ModuleException &moduleException)
        {
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {
        // Assume base brake if no cable connect
        Log( LOG_DEV_DATA, "No Cable Connect...assuming base brake\n");
        status = BEP_STATUS_SUCCESS;
    }
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::ParkBrakePawlPedalCheck(), status=%d\n", status);

    return( status);
}

//MAM 6/3/10
//=============================================================================
//const string GenericTC::PerformPBTorqueTest(const std::string &direction)
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::PerformPBTorqueTest(const std::string &direction)
{
    string  status;         // the current test status
    const int rollerCount = GetRollerCount();
    float   torqueVal = ((GetParameterFloat("ParkBrakeApplyForce") != 0) ? GetParameterFloat("ParkBrakeApplyForce") : 350) * 0.5;
    //float   speedVal = (GetParameterFloat("ParkBrakeSpeedValue") != 0) ? GetParameterFloat("ParkBrakeSpeedValue") : 5;
    float   speedVal = GetParameterFloat("ParkBrakeSpeedValue");
    string  frontMotorMode, rearMotorMode;
    string  lfMotorValTag, rfMotorValTag;
    string  lrMotorValTag, rrMotorValTag;
    string  faultCode("0000");;
    float   wheelSpeeds[rollerCount];
    char    temp[256];
    int     testTime = GetTestStepInfoInt("TestTime");
    INT32   statusCheck;           
    //MAM 5/7/07 - if not turning rolls for driver influence check - made backward compatible: false if param not in config file
    //bool    NoDriverInfluence = GetParameterBool("DriverInfluenceDoNotTurnRolls");
    bool    DontTurnRolls = GetParameterBool("DriverInfluenceDoNotTurnRolls");


    //Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::PerformPBTorqueTest(%s)\n", direction.c_str());
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::PerformPBTorqueTest(%s) ApplyForce = %.02f \n", direction.c_str(), torqueVal);

    //max roller speed of 5 mph
    if (speedVal > 5.0)
    {
        speedVal = 5.0;
    }
    if (speedVal < -5.0)
    {
        speedVal = -5.0;
    }

    //MAM 5/7/07 - if not performing mechanical driver influence check        
    if (1 == DontTurnRolls)
    {
        Log( LOG_DEV_DATA, "Not turning rollers for driver influence check\n");
        Log( LOG_DEV_DATA, "Changing speedVal from %.02f to zero\n", speedVal);
        speedVal = 0;
    }

    Log(LOG_DEV_DATA, "PerformPBTorqueTest front roller speed = %.02f \n", speedVal);


    // Go to torque 0 first to clear any command history in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    BposSleep( 100);

    // If we want the wheels to turn forward, we need a negative torque
    if ( ((direction == "Forward") && (torqueVal > 0)) ||
         // If we want the wheels to turn in reverse, we need a positive torque value
         ((direction == "Reverse") && (torqueVal < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueVal, -torqueVal);
        torqueVal *= -1.0;
    }

    // If we want the wheels to turn forward, we need a positive speed
    if ( ((direction == "Forward") && (speedVal < 0)) ||
         // If we want the wheels to turn in reverse, we need a negative speed
         ((direction == "Reverse") && (speedVal > 0)) )
    {
        Log( LOG_DEV_DATA, "Changing speed value from %.02f to %.02f\n", speedVal, -speedVal);
        speedVal *= -1.0;
    }

    // read the current distance of the wheels
    GetWheelDistances( m_parkBrakeDistances[ 0]);

    // Tell the motor to turn for the park brake test
    Log( LOG_DEV_DATA, "Commanding Front Motors to %.2f MPH and Rear Motors %.2f LBS\n",
         speedVal, torqueVal);

    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("RightRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    m_MotorController.Write("LeftRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
    m_MotorController.Write("RightRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), true);

    // wait the specified amount of time
    SetStartTime();
    while ((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) )
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        //BposSleep(100);     // wait 1/10th of a second
        //testTime -= 100;    // decrement the time from the total time
        BposSleep(500);     // wait 1/2 of a second
        testTime -= 500;    // decrement the time from the total time
        Log(LOG_DEV_DATA, "PerformPBTorque: testTime = %d ", testTime);  //MAM 11/24/14

        //continue to check brake switch        
        //if not performing mechanical driver influence check        
        if (1 == DontTurnRolls)
        {
            //status = ParkBrakePawlInit(); //this actually checks the BOO switch
            if ( true == GetParameterBool( "ParkBrakePedalCheck"))
            { //MAM 5/12/11
                status = ParkBrakePawlPedalCheck();
            }

        }
    }

    // Make sure we clear the speed/torque command values
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while (TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // once stopped, check for pass/fail

    // if and error did not occur and not aborted
    statusCheck = GenericTC::StatusCheck();
    if ( statusCheck == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances( m_parkBrakeDistances[ 1]);

        // Analyze the distances that the wheels travelled
        status = ValidateParkBrakeTestDistances(m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1]);
    }
    else
    {
        Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
        status = ConvertStatusToResponse( statusCheck);
        faultCode = GetFaultCode("ParkBrakeFault");
    }

    // Send the test result info
    SendTestResult( GetTestStepName(), status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::PerformPBTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return( status);
}

//=============================================================================
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::ValveCycleTorqueTest(const std::string &direction, std::string axle)
{
    string      testStatus;     // the overall test status
    string      result;         // the current test status
    INT32       status = BEP_STATUS_SUCCESS;
    string testResultCode("0000");
    int         testTime;
    float       speedValue;
    float       minRearDistance, minFrontDistance;
    const int   rollerCount = GetRollerCount();
    float       wheelSpeeds[rollerCount];
    WHEELINFO   totalDistance;    
    int         valveNum;
    int         scanDelay = 3000; //default
    INT32       lfStart, rfStart, lrStart, rrStart = 0;
    char        temp[256];
    bool        brakeOn = false;
    int         driverDelayTime = GetParameterInt("ValveCycleDriverDelay");
    bool        pedalCheckFail = false;
    bool        lfrfCross/*, lflrCross, lfrrCross */= false;
    //bool        rflrCross, rfrrCross = false;
    bool        lrrrCross = false;
    bool        valveCrossDetected = false;

    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
        valveNum = 1;
    else
        valveNum = 3;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::ValveCycleTorqueTest(%s)\n", direction.c_str());

    // load up the test parameters
    testTime = (GetTestStepInfoInt("TestTime") != 0) ? GetTestStepInfoInt("TestTime") : 20000;
    speedValue = (GetTestStepInfoFloat("SpeedValue") != 0) ? GetTestStepInfoFloat("SpeedValue") : 3.0;
    minRearDistance = GetTestStepInfoFloat("MinRearDistance");
    minFrontDistance = GetTestStepInfoFloat("MinFrontDistance");

    UpdatePrompts();

    //max roller speed of 5 mph
    if (speedValue > 2.5)
    {
        speedValue = 2.5;
    }
    if (speedValue < -2.5)
    {
        speedValue = -2.5;
    }

    // If we want the wheels to turn forward, we need a positive speed
    if (((direction == "Forward") && (speedValue < 0)) ||
        // If we want the wheels to turn in reverse, we need a negative speed
        ((direction == "Reverse") && (speedValue > 0)))
    {
        Log( LOG_DEV_DATA, "Changing speed value from %.02f to %.02f\n", speedValue, -speedValue);
        speedValue *= -1.0;
    }

    // Go to torque 0 first to clear any command hiostory in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    BposSleep( 100);

    // read the current distance of the wheels
    GetWheelDistances(m_valveCycleDistances[ 0]);

    //check for brake pedal apply
    status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
    if (BEP_STATUS_SUCCESS == status)
    {
        if (false == brakeOn)
        {
            Log( LOG_DEV_DATA, "BrakeSwitch OFF detected at start of ValveCycleTorqueTest()\n");
            Log( LOG_DEV_DATA, "Prompting driver to apply brake\n");
            DisplayPrompt(1,"ApplyBrake");
            status = StatusSleep(driverDelayTime);          
        }

        if (true == brakeOn)
        {
            Log( LOG_DEV_DATA, "BrakeSwitch ON detected at start of ValveCycleTorqueTest()\n");            
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Failure reading brake switch at start of ValveCycleTorqueTest() \n");
    }

    // command the drives to the desired speed
    //SystemCommand(MOTOR_MODE, string(SPEED_MODE));
    //SystemCommand(COMMAND_SPEED, speedValue);
    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightRearMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedValue), false);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedValue), false);
    m_MotorController.Write("LeftRearSpeedValue", CreateMessage(temp, 256, "%.2f", speedValue), false);
    m_MotorController.Write("RightRearSpeedValue", CreateMessage(temp, 256, "%.2f", speedValue), true);


    // wait the specified amount of time
    //SetStartTime();
    scanDelay = GetTestStepInfoInt( "ScanDelay");
    BposSleep(scanDelay); //give motors time to react
    speedValue = 0;
    status = BEP_STATUS_SUCCESS;

    //while ((BEP_STATUS_SUCCESS == status) && (testTime > 0) && (valveNum <= 5))
    while ((BEP_STATUS_SUCCESS == status) && (testTime > 0))
    {

        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "ValveCycleTorqueTest: testTime = %d valveNum = %d speeds = LF: %f, RF: %f, LR: %f, RR: %f\n", 
            testTime, valveNum, wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL], wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);

        //check for brake pedal apply
        status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
        if (BEP_STATUS_SUCCESS == status)
        {
            if (false == brakeOn)
            {
                Log( LOG_DEV_DATA, "BrakeSwitch OFF detected during ValveCycleTorqueTest()\n");
                Log( LOG_DEV_DATA, "Prompting driver to apply brake\n");
                DisplayPrompt(1,"ApplyBrake");
                //give driver time to apply brake
                status = StatusSleep(driverDelayTime);
                //check again
                status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
                if (false == brakeOn)
                {
                    pedalCheckFail = true;
                }
            }

            if (true == brakeOn)
            {
                Log( LOG_DEV_DATA, "BrakeSwitch ON detected during ValveCycleTorqueTest()\n");            
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Failure reading brake switch during ValveCycleTorqueTest() \n");
        }

        switch (valveNum)
        {
        case 1:            
            Log(LOG_DEV_DATA, "CycleValves LF ON\n");
            status = m_vehicleModule.GetInfo( GetDataTag("CycleValves1") );
            lfStart = TagArray("ValveCycleLFStart");
            break;
        case 2:
            Log(LOG_DEV_DATA, "Check LF valve cross \n");
            //only LF wheel should have moved            
            GetWheelDistances(m_valveCycleDistances[ 1]);                        
            GetTotalDistances( totalDistance, m_valveCycleDistances[ 0], m_valveCycleDistances[ 1]);            
            Log(LOG_DEV_DATA, "The distances traveled #1: %.2f, %.2f, %.2f, %.2f \n",
                totalDistance.lfWheel, totalDistance.rfWheel, totalDistance.lrWheel, totalDistance.rrWheel);          
            if (totalDistance.rfWheel > totalDistance.lfWheel)
            {
                lfrfCross = true;
                valveCrossDetected = true;
            }

            Log(LOG_DEV_DATA, "CycleValves LF OFF\n");
            status = m_vehicleModule.GetInfo( GetDataTag("CycleValves1Stop") );
            lfStart = TagArray("ValveCycleLFStop");
            Log(LOG_DEV_DATA, "CycleValves RF \n");            
            status = m_vehicleModule.GetInfo( GetDataTag("CycleValves2") );
            rfStart = TagArray("ValveCycleRFStart");
            break;
        case 3:
            if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
            {
                Log(LOG_DEV_DATA, "Check RF valve cross \n");
                //LF & RF wheels could both be moving            
                GetWheelDistances(m_valveCycleDistances[ 1]);            
                GetTotalDistances( totalDistance, m_valveCycleDistances[ 0], m_valveCycleDistances[ 1]);            
                Log(LOG_DEV_DATA, "The distances traveled #2: %.2f, %.2f, %.2f, %.2f \n",
                    totalDistance.lfWheel, totalDistance.rfWheel, totalDistance.lrWheel, totalDistance.rrWheel);            

                Log(LOG_DEV_DATA, "CycleValves RF OFF\n");
                status = m_vehicleModule.GetInfo( GetDataTag("CycleValves2Stop") );
                lfStart = TagArray("ValveCycleRFStop");
            }
            else
            { 
                Log(LOG_DEV_DATA, "CycleValves LR \n");           
                status = m_vehicleModule.GetInfo( GetDataTag("CycleValves3") );
                lrStart = TagArray("ValveCycleLRStart");
            }
            break;
        case 4:
            if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
            { 
                Log(LOG_DEV_DATA, "Check REAR valve cross \n");
                //RR should not be moving yet            
                GetWheelDistances(m_valveCycleDistances[ 1]);            
                GetTotalDistances( totalDistance, m_valveCycleDistances[ 0], m_valveCycleDistances[ 1]);            
                Log(LOG_DEV_DATA, "The distances traveled #3: %.2f, %.2f, %.2f, %.2f \n",
                    totalDistance.lfWheel, totalDistance.rfWheel, totalDistance.lrWheel, totalDistance.rrWheel);          
                if (false == valveCrossDetected)
                {
                    if (totalDistance.rrWheel > totalDistance.lrWheel)
                    {
                        lrrrCross = true;
                        valveCrossDetected = true;
                    }
                }

                Log(LOG_DEV_DATA, "CycleValves LR OFF\n");
                status = m_vehicleModule.GetInfo( GetDataTag("CycleValves3Stop") );
                lfStart = TagArray("ValveCycleLRStop");
                Log(LOG_DEV_DATA, "CycleValves RR \n");           
                status = m_vehicleModule.GetInfo( GetDataTag("CycleValves4") );
                rrStart = TagArray("ValveCycleRRStart");
            }
            break;
        case 5:
            if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
            {
                Log(LOG_DEV_DATA, "CycleValves RR OFF\n");
                status = m_vehicleModule.GetInfo( GetDataTag("CycleValves4Stop") );
                lfStart = TagArray("ValveCycleRRStop");
            }
            break;
        default:
            break;
        }

        //delay between wheels        
        BposSleep(scanDelay);
        testTime -= scanDelay;
        valveNum++;
    }

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while ((BEP_STATUS_SUCCESS == status) && (TimeRemaining()) && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        status = StatusSleep(100);
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // if and error did not occur and not aborted
    if (status == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances(m_valveCycleDistances[ 1]);        

        // Check for roll-over / roll-under
        GetTotalDistances( totalDistance, m_valveCycleDistances[ 0], m_valveCycleDistances[ 1]);

        Log(LOG_DEV_DATA, "The distances traveled #4: %.2f, %.2f, %.2f, %.2f, Parameter MinRear: %f, MinFront: %f\n",
            totalDistance.lfWheel, totalDistance.rfWheel,
            totalDistance.lrWheel, totalDistance.rrWheel,
            minRearDistance, minFrontDistance);

        //** TO DO: ADD NEW FAULT **

        // Validate the distances for the drive axle        
        result = ValidateTorqueTestSlaveAxle( totalDistance.lrWheel, totalDistance.rrWheel,
                                              minRearDistance, "ValveCycleTestFault");
        UpdateResult( result, testStatus);

        // if the slave wheels are under the minimum allowed distance        
        result = ValidateTorqueTestSlaveAxle( totalDistance.lfWheel, totalDistance.rfWheel,
                                              minFrontDistance, "ValveCycleTestFault");
        UpdateResult( result, testStatus);
    }
    else
    {
        UpdateResult( status, testStatus);
    }

    if (pedalCheckFail)
    {
        Log( LOG_DEV_DATA, "Failing ValveCycleTorqueTest because brake pedal detected OFF\n");
        result = BEP_FAIL_STATUS;
        UpdateResult( result, testStatus);
    }

    if ((valveCrossDetected)&&(!pedalCheckFail))
    {
        //left front
        if (true == lfrfCross)
        {

            SendSubtestResultWithDetail("LFValveCross", testFail, "LF Valve Cross",
                                        GetFaultCode("LeftFrontValveCrossedWithRightFront"),
                                        GetFaultName("LeftFrontValveCrossedWithRightFront"),
                                        GetFaultDescription("LeftFrontValveCrossedWithRightFront"));
        }
        //rear        
        if (true == lrrrCross)
        {

            SendSubtestResultWithDetail("LRValveCross", testFail, "LR Valve Cross",
                                        GetFaultCode("LeftRearValveCrossedWithRightRear"),
                                        GetFaultName("LeftRearValveCrossedWithRightRear"),
                                        GetFaultDescription("LeftRearValveCrossedWithRightRear"));
        }

        Log( LOG_DEV_DATA, "Failing ValveCycleTorqueTest because valve cross detected\n");
        result = BEP_FAIL_STATUS;
        UpdateResult( result, testStatus);
    }


    SendTestResult( testStatus, GetTestStepInfo("Description"), "0000");

    // command the motor controller to boost mode
    SystemCommand(MOTOR_MODE, string(BOOST_MODE));
    RemovePrompt(1,"ApplyBrake");
    RemovePrompt(2,"ABSTest");

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::ValveCycleTorqueTest(%s), result=%s\n", direction.c_str(), testStatus.c_str());

    return(testStatus);
}

//MAM 12/10/08
//Note: We only fail here if we have issues with reading the data from the module. If the brake switch stays on,
//then we set the fault in AnalyzeParkPawlDriverInfluence().
template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::SBAParkBrakePawlInit()
{
    INT32 status;
    bool  brakeOn = false;
    bool promptDisplayed = false;
    short travelDistance = 0;
    int pedalTravelIndex = GetParameterInt("PedalTravelIndex");     //MAM 11/11/15
    string testResult = testPass;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    SerialString_t               response;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::SBAParkBrakePawlInit()\n");

    if (CheckCableConnect())
    {
        // Need to perform this test step
        try
        {

            //Make sure brakes are not being applied at beginning of test
            Log( LOG_DEV_DATA, "Checking BrakeSwitch in SBAParkBrakePawlInit()\n");
            //Read the pedal travel to verify driver's foot is off the brake
            status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
            if (status == BEP_STATUS_SUCCESS)
            {
                if (0 == pedalTravelIndex)
                {
                    pedalTravelIndex = 3;
                }
                travelDistance = (short)response[pedalTravelIndex];
                travelDistance = travelDistance << 8;            
                travelDistance = travelDistance + response[pedalTravelIndex+1];
                Log(LOG_DEV_DATA, "SBAParkBrakePawlInit: pedaltravelDistance = %d\n", travelDistance);

                if (travelDistance > 1000)
                {  //10 mm travel = "ON"
                    brakeOn = true;                    
                }
            }
            else
            {
                Log(LOG_DEV_DATA, "SBA Recovery: Error reading pedal travel!\n");             
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }        

            if ( (BEP_STATUS_SUCCESS == status) && (true == brakeOn) )
            {
                Log( LOG_DEV_DATA, "BrakeSwitch ON detected in SBAParkBrakePawlInit()\n");
                Log( LOG_DEV_DATA, "Prompting driver to remove foot from brake\n");                
                DisplayPrompt( 2, GetPrompt( "KeepFootOffBrake"), 0);
                promptDisplayed = true;
            }

            //continue to check brake switch if initially on
            while ( (true == TimeRemaining()) && (BEP_STATUS_SUCCESS == status) &&
                    (true == brakeOn) )
            {
                BposSleep(500); //delay 500 ms                
                status = m_vehicleModule.GetInfo( GetDataTag("ReadPedalTravel"), response );
                if (status == BEP_STATUS_SUCCESS)
                {
                    if (0 == pedalTravelIndex)
                    {
                        pedalTravelIndex = 3;
                    }
                    travelDistance = (short)response[pedalTravelIndex];
                    travelDistance = travelDistance << 8;            
                    travelDistance = travelDistance + response[pedalTravelIndex+1];
                    Log(LOG_DEV_DATA, "SBAParkBrakePawlInit: pedaltravelDistance = %d\n", travelDistance);
                    if (travelDistance > 1000)
                    {  //10 mm travel = "ON"
                        brakeOn = true;                        
                    }
                    else
                    {
                        brakeOn = false;
                    }
                }
                else
                {
                    Log(LOG_DEV_DATA, "SBA Recovery: Error reading pedal travel!\n");             
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                }
            } //end while

            //remove warning prompt if displayed            
            if (true == promptDisplayed)
            {
                Log( LOG_DEV_DATA, "Removing prompt to remove foot from brake\n");
                RemovePrompt("2",GetPrompt("KeepFootOffBrake"), 0);
            }

            //if brake not on
            if ( (BEP_STATUS_SUCCESS == status) && (false == brakeOn))
            {
                // Reset the brake switch on/off status
                m_vehicleModule.ClearBrakeSwitchStatus();
            }
            else if (true == brakeOn)
            { // If brake switch stayed on
                Log( LOG_ERRORS, "Saw brake switch ON during SBAParkBrakePawlInit()\n");
                //Set the brake switch status - should fail driver influence check 
                m_vehicleModule.SetBrakeSwitchStatusOn();
                //Driver influence fault is set in AnalyzeParkPawlDriverInfluence()                
            }
            else
            { //status value is fail
                Log( LOG_ERRORS, "Error reading brake switch status during SBAParkBrakePawlInit()\n");
                status = BEP_STATUS_FAILURE;
            }

        }
        catch (ModuleException &moduleException)
        {
            status = BEP_STATUS_SOFTWARE;            
        }
    }
    else
    {
        // Assume base brake if no cable connect
        Log( LOG_DEV_DATA, "No Cable Connect...assuming base brake\n");
        status = BEP_STATUS_SUCCESS;
    }

    // Report the result of the test
    SendSubtestResult("SBAParkBrakePawlInit", testResult, testDescription, testResultCode);

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::SBAParkBrakePawlInit(), status=%d\n", status);

    return( status);
}

//=============================================================================
//const std::string GenericTC::AnalyzeParkPawlTest(const std::string &value)
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::AnalyzeParkPawlTest()
{
    std::string testStatus = testFail;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::AnalyzeParkPawlTest() \n");

    //Don't check distances if we failed the BOO check
    if (0 == m_ppwlBrakeAppFault)
    {

        // If this is an electric vehicle
        if (ReadSubscribeData(GetDataTag("MachineType")) == GetDataTag("MachineType3600"))
        {
            // Analyze the distances that the wheels travelled
            testStatus = ValidateParkPawlTestDistances(m_parkPawlDistances[0], m_parkPawlDistances[1]);
        }
        // Can't check for driver influence because we don't read
        // distances on a hydraulic machine
        else
        {
            testStatus = testPass;  // set test step status to pass
            Log(LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeParkPawlTest() Not validating 360 PLC results: testStatus = testPass \n");
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "FAILED BOO CHECK - NOT CHECKING PPWL DISTANCES\n");
    }

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::AnalyzeParkPawlTest() \n");
    return( testStatus);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::AnalyzeParkBrakePawlDriverInfluence()
{
    bool            sawBrakeOn = true;
    std::string     testResult(testFail);
    std::string     faultTag( GetTestStepInfo( "DIFaultTag"));       

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::AnalyzeParkBrakePawlDriverInfluence()\n");

    m_ppwlBrakeAppFault = 0;

    // Determine if the test should be performed
    if (!ShortCircuitTestStep())
    {
        try
        {
            // Check if we saw the brake switch during the park pawl test
            (void)m_vehicleModule.SawBrakeSwitchOn( sawBrakeOn);
            // Report the status of the test
            if ( false == sawBrakeOn)
            {
                testResult = testPass;              
            }
            else
            {
                Log( LOG_ERRORS, "Saw brake switch on during park brake / park pawl test\n");
                testResult = testFail;
                m_ppwlBrakeAppFault = 1;
            }
        }
        catch ( BepException &err)
        {
            Log( LOG_ERRORS, "Exception in AnalyzeParkBrakePawlDriverInfluence: %s\n", err.GetReason());
            testResult = testSoftwareFail;
        }

        //MAM 3/31/10 - Only send the test result details with fault code if we fail
        if (testResult == testPass)
        {
            SendTestResult(testPass, GetTestStepInfo("Description"));
        }
        else
        {
            SendTestResultWithDetail(testResult, 
                                     GetTestStepInfo("Description"),
                                     GetFaultCode(faultTag),
                                     GetFaultName(faultTag),
                                     GetFaultDescription(faultTag));
        }

    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::AnalyzeParkBrakePawlDriverInfluence(), status=%s\n", testResult.c_str());

    return(testResult);
}

template <class ModuleType>
BEP_STATUS_TYPE FordABSTCTemplate<ModuleType>::WaitForTargetSpeed(float speedt,Direction direction, UINT32 wait) /*= 100 */
{
    //MAM 12/21/07 - if we want to wait for ALL wheels to be below target speed before firing valves
    //We have to use reverse logic because param is false if not in config file
    bool waitAll = true;
    bool doNotWait = GetParameterBool("DoNotWaitAllWheels");

    if (0 != doNotWait)
    {
        waitAll = false;
        Log(LOG_DEV_DATA, "NOT waiting for all wheels to be below valve firing target speed \n");
    }
    else
    {
        Log(LOG_DEV_DATA, "Waiting for all wheels to be below valve firing target speed \n");
    }

    //return(WaitForTargetSpeed( speedt, direction, true, wait));
    return(WaitForTargetSpeed( speedt, direction, waitAll, wait));
}

template <class ModuleType>
BEP_STATUS_TYPE FordABSTCTemplate<ModuleType>::WaitForTargetSpeed(float speedt,Direction direction, bool waitAll, UINT32 wait) /*= 100 */
{
    BEP_STATUS_TYPE rtnStat = BEP_STATUS_FAILURE;   // return status of the test
    bool proceed = false;           // flag to indicate that conditions have been met
    bool frontsOk = false;
    bool rearsOk = false;
    float currentFrontSpeed;        // the current average front roller speeds
    float currentRearSpeed;         // the current average rear roller speeds
    WHEELINFO   wheelSpeeds;
    //MAM 10/30/08
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    Log(LOG_FN_ENTRY, "FordABSTCTemplate::WaitForTargetSpeed targetspeed: direction: (%f, %d)\n",
        speedt, direction);

    do
    {
        if ( GetWheelSpeeds( wheelSpeeds) == BEP_STATUS_SUCCESS)
        {
            currentFrontSpeed = (wheelSpeeds.lfWheel + wheelSpeeds.rfWheel) / 2;
            currentRearSpeed = (wheelSpeeds.lrWheel + wheelSpeeds.rrWheel) / 2;

            //MAM 10/30/08 - Ping module so we don't lose diag mode in case we're waiting
            //for all wheels to fall below the target speed
            status = m_vehicleModule.GetInfo( GetDataTag("ModuleKeepAlive") );

            if ( UP == direction)
            {
                if ( currentFrontSpeed >= speedt)
                {
                    Log( LOG_DEV_DATA, "WaitForTargetSpeed() fronts above target %f front %f mph\n", 
                         speedt, currentFrontSpeed);
                    frontsOk = true;
                }
                else
                {
                    frontsOk = false;
                }

                if ( currentRearSpeed >= speedt)
                {
                    Log( LOG_DEV_DATA, "WaitForTargetSpeed() fronts above target %f front %f mph\n", 
                         speedt, currentFrontSpeed);
                    rearsOk = true;
                }
                else
                {
                    rearsOk = false;
                }
            }
            else
            {
                if ( currentFrontSpeed <= speedt)
                {
                    Log( LOG_DEV_DATA, "WaitForTargetSpeed() fronts below target %f front %f mph\n", 
                         speedt, currentFrontSpeed);
                    frontsOk = true;
                }
                else
                {
                    frontsOk = false;
                }

                if ( currentRearSpeed <= speedt)
                {
                    Log( LOG_DEV_DATA, "WaitForTargetSpeed() rears below target %f rear %f mph\n", 
                         speedt, currentRearSpeed);
                    rearsOk = true;
                }
                else
                {
                    rearsOk = false;
                }
            }

            // If we want all axles to break the limit and both axles have broken the limit
            if ( (true == waitAll) && (true == frontsOk) && (true == rearsOk))
            {
                Log(LOG_DEV_DATA, "WaitForTargetSpeed() all reached: target %f front %f rear %f mph\n", 
                    speedt, currentFrontSpeed, currentRearSpeed);
                proceed = true;
            }
            // If we want any axle to break the limit and either axle has broken the limit
            else if ( (false == waitAll) && ((true == frontsOk) || (true == rearsOk)) )
            {
                Log(LOG_DEV_DATA, "WaitForTargetSpeed() individual reached: target %f front %f rear %f mph\n", 
                    speedt, currentFrontSpeed, currentRearSpeed);
                proceed = true;
            }
            else
            {
                BposSleep(wait);
            }
        }
        else
        {
            BposSleep( wait);
        }
    } while ((!proceed) && TimeRemaining() &&
             (StatusCheck() == BEP_STATUS_SUCCESS));

    if (proceed && (StatusCheck() == BEP_STATUS_SUCCESS))
    {
        rtnStat = BEP_STATUS_SUCCESS;
    }
    else if (StatusCheck() == BEP_STATUS_ABORT)
    {
        rtnStat = BEP_STATUS_ABORT;
    }
    else
    {
        rtnStat = BEP_STATUS_FAILURE;
    }

    Log(LOG_FN_ENTRY, "WaitForTargetSpeed() done: return status = %d\n", rtnStat);

    return(rtnStat);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::StatusCheck(void)
{
    const std::string testStepName(GetTestStepName());
    INT32 status;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::StatusCheck( %s)\n", testStepName.c_str());

    if ((testStepName == "ParkBrakeTest") || (testStepName == "ParkPawlTest") ||
        (testStepName == "ParkBrakeParkPawlTest"))
    {
        if ((ReadSubscribeData(GetDataTag("VehiclePresent")) == "1") &&
            (ReadSubscribeData(GetDataTag("Clutch")) == "1") &&
            (ReadSubscribeData(GetDataTag("Declutch")) == "0"))
        {
            status = BEP_STATUS_SUCCESS;        
        }
        else
        {
            status = BEP_STATUS_ABORT;
        }
    }
    else
    {
        status = GenericABSTCTemplate<ModuleType>::StatusCheck();
    }

    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::StatusCheck( %s), %d\n", testStepName.c_str(), status);

    return(status);
}

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepAccelerateToBrakeSpeed(void)
{
    std::string status( BEP_FAIL_STATUS);
    const float armSpeed( GetParameterFloat( "ArmSpeed"));
    const float promptSpeed( GetParameterFloat( "BrakePromptSpeed"));  //MAM 7/12/10
    char speedTarget[ 128];

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAccelerateToBrakeSpeed()\n");

    sprintf( speedTarget, "%.2f %.2f", armSpeed*.9, armSpeed*1.1);

    //MAM 7/12/10 - Use "BrakePromptSpeed" parameter for driver prompt speed if available
    if (promptSpeed > 0)
    {
        CopyParamToTestStepInfo( "Prompt1Value1", promptSpeed);
    }
    else
    {
        CopyParamToTestStepInfo( "Prompt1Value1", armSpeed);
    }

    // Copy drag parameters into the test step info for the base class
    CopyParamToTestStepInfo( "ArmSpeed", armSpeed);
    //CopyParamToTestStepInfo( "Prompt1Value1", armSpeed); //MAM 7/12/10 - commented out
    CopyParamToTestStepInfo( "SpeedTarget", std::string( speedTarget));        

    // Prompt driver to accelerate
    status = m_baseBrakeTool->TestStepAccelerate();
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAccelerateToBrakeSpeed(), status=%s\n", status.c_str());

    return( status);
}

template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::TestStepAccelerateToParkBrakeSpeed(void)
{
    std::string status( BEP_FAIL_STATUS);
    const float armSpeed( GetParameterFloat( "DynamicParkBrakeArmSpeed"));  
    char speedTarget[ 128];
    INT32       engageStatus;
    int     promptDelayTime;
    INT32   dclBitStatus;   //MAM 7/11/10

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAccelerateToParkBrakeSpeed\n");

    sprintf( speedTarget, "%.2f %.2f", armSpeed*.9, armSpeed*1.1);    

    //MAM 7/11/10
    dclBitStatus = SystemWrite(GetDataTag("DCLControl"), true);
    Log( LOG_DEV_DATA, "Setting DCLControl PLC bit \n");

    // Make sure the machine is declutched
    engageStatus = Disengage();
    if ( BEP_STATUS_SUCCESS == engageStatus)
    {
        // Copy the dynamic park brake parameters to test step info
        SetTestStepInfoValue( "ArmSpeed", armSpeed);
        CopyParamToTestStepInfo( "Prompt1Value1", armSpeed);
        CopyParamToTestStepInfo( "SpeedTarget", std::string( speedTarget));

        status = TestStepAccelerate(); //MM 12/21/06

        //show "RemoveFootFromAccelerator" prompt
        promptDelayTime = GetParameterInt("ReleaseThrottlePromptDelay");
        DisplayTimedPrompt(GetParameter("ReleaseThrottlePrompt"), "1", 0, promptDelayTime);
    }
    else
    {
        Log( LOG_ERRORS, "Error %d declutching the machine\n", engageStatus);
        UpdateResult( engageStatus, status);
    }    

    //MAM 7/11/10
    dclBitStatus = SystemWrite(GetDataTag("DCLControl"), false);
    Log( LOG_DEV_DATA, "Clearing DCLControl PLC bit \n");

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAccelerateToParkBrakeSpeed(), status=%s\n", status.c_str());

    return( status);
}

template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::TestStepAccelerate (void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed
    float armSpeed = -1;
    double elapsedTime = -1;
    string status;

    Log(LOG_FN_ENTRY, "FordABSTCTemplate::TestStepAccelerate()\n");
    // Check if test should be run
    if (!ShortCircuitTestStep())
    {
        try
        {   // if the brake test was not a previous pass
            if (m_baseBrakeTool->GetBrakeTestingStatus() != "Pass")
            {   // make sure everything is groovy with the machine
                testStatus = StatusCheck();
                if (testStatus == BEP_STATUS_SUCCESS)
                {   // set up the display (speed gauge)
                    testStatus = SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedTarget"));

                    if (testStatus == BEP_STATUS_SUCCESS)
                    {   // wait till arm speed is achieved or we timeout
                        armSpeed = GetTestStepInfoFloat("ArmSpeed");
                        Log(LOG_DEV_DATA,"FordABSTCTemplate::TestStepAccelerate - armSpeed = %f\n", armSpeed); //MAM 12/6/06
                        // Display Prompts
                        if (UpdatePrompts() != BEP_STATUS_SUCCESS)
                            Log(LOG_ERRORS, "Unable to Update Prompts\n");
                        float speed=0;
                        do
                        {   // read the current speed and update the display
                            //speed = GetSpeed();
                            //MAM 12/21/06: from GenericTC - returns average of drive axle speed
                            speed = GetRollSpeed(); 
                            if (speed < armSpeed)
                            {   // if not at speed yet
                                // check the machine status and wait if ok
                                testStatus = StatusCheck();
                                if (testStatus == BEP_STATUS_SUCCESS)
                                    BposSleep(100);
                            }
                        }while ((speed < armSpeed) && TimeRemaining() && testStatus == BEP_STATUS_SUCCESS);

                        elapsedTime = GetElapsedTime();

                        if (speed >= armSpeed)
                        {      // if reached speed
                            m_baseBrakeTool->SetBrakeTestingStatus(BEP_TESTING_RESPONSE);   // flag that the brakes can be tested
                            testStatus = BEP_STATUS_SUCCESS;                // indicate that the test passed
                        }
                        else if (!TimeRemaining())  // else if timed out
                            testStatus = BEP_STATUS_TIMEOUT;
                    }
                    else
                        Log(LOG_ERRORS, "Error, Could Not Set Up Display %d\n", testStatus);

                    // remove the speed target from the gauge
                    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                }
                else
                    Log(LOG_ERRORS, "Invalid Machine Status For FordABSTCTemplate::TestStepAccelerate %d\n", testStatus);
            }
            else
                testStatus = BEP_STATUS_SUCCESS;    // flag that the test passed
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "TestStepAccelerate Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        UpdateResult(testStatus, status);

        char buff[ 256];
        SendTestResultWithDetail(status, GetTestStepInfo("Description"), "0000",
                                 "Armspeed", CreateMessage( buff, sizeof(buff), "%.2f", armSpeed), unitsMPH,
                                 "Elapsed Time", CreateMessage( buff, sizeof(buff), "%.02f", elapsedTime/1000.0), "seconds");
    }
    else
    {   // Skip the test
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    Log(LOG_FN_ENTRY, "FordABSTCTemplate::TestStepAccelerate(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::TestStepDynamicParkBrake(void)
{
    INT32   testStatus = BEP_STATUS_SUCCESS;        // the status of the test being performed
    int     retries = 3;
    char    buff[ 256];
    float   startForce, minForce, maxForce, target;
    string  testResult;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepDynamicParkBrake()\n");
    
    if (!ShortCircuitTestStep())
    {
        (void)GetTestStepParameter("DynamicParkBrakeStartForce", startForce);
        (void)GetTestStepParameter("DynamicParkBrakeRearMinForce", minForce);
        (void)GetTestStepParameter("DynamicParkBrakeRearMaxForce", maxForce);
        target = (minForce + maxForce) / 2;

        SetTestStepInfoValue( "StartSampleSpeed", GetParameter("DynamicParkBrakeStartSampleSpeed"));
        SetTestStepInfoValue( "EndSampleSpeed", GetParameter("DynamicParkBrakeEndSampleSpeed"));
        SetTestStepInfoValue( "MinimumStartForce", startForce);
        SetTestStepInfoValue( "ForceSampleCount", GetParameter("DynamicParkBrakeSampleCount"));
        SetTestStepInfoValue( "StartSamplingAtMinimumForce", GetParameter("DynamicParkBrakeSampleUseForces"));
        SetTestStepInfoValue( "BrakeDomain", CreateMessage( buff, sizeof( buff), "0 %.02f", 2*target));
        SetTestStepInfoValue( "BrakeTarget", CreateMessage( buff, sizeof( buff), "%.02f %.02f", minForce, maxForce));

        try
        {
            //MAM 10/06/08 - moved from below to here so the system is
            //disengaged before we prompt thr driver to accelerate
            // Declutch
            testStatus = Disengage();
            while ( (BEP_STATUS_SUCCESS != testStatus) && (retries > 0))
            {
                testStatus = Disengage();
                retries--;
            }

            // set up the display (brake force gauge)
            //m_baseBrakeTool->UpdateTarget();
            m_baseBrakeTool->UpdateTarget(true);

            // if disengaged
            if (testStatus == BEP_STATUS_SUCCESS)
            {
                // display driver prompts
                UpdatePrompts();
                // variables to hold the index values for analyzing
                m_dynParkBrakeStart = m_dynParkBrakeStop = 0;

                // If we are declutched
                if ( BEP_STATUS_SUCCESS == testStatus)
                {
                    testStatus = DynamicParkBrake(m_dynParkBrakeStart, m_dynParkBrakeStop, "DynamicParkBrake");
                }
                // Remove the prompts we displayed
                RemovePrompts();

                // Determine if the brake force meter should be disabled
                if (!GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
                {
                    m_baseBrakeTool->UpdateTarget(false);
                }
            }
            else
            {
                Log( LOG_ERRORS, "Could not declutch for DynamicParkBrake\n");
            }
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "TestStepDynamicParkBrake Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // set the brake gauge to inactive if we are not displaying meter until zero speed
        if (!GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
        {
            SystemWrite(GetDataTag("BrakeActive"), 0);
        }

        // update the test status
        UpdateResult(testStatus, testResult);
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {
        // Skip the test
        Log(LOG_DEV_DATA, "Skipping test step - %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepDynamicParkBrake(): %s, %d\n", 
         testResult.c_str(), testStatus);

    return(testResult);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const std::string &tagPrefix)
{
    INT32 testStatus = BEP_STATUS_NA;
    float wheelForce[4];
    const string faultTag("ParkBrakeDriverInfluenceFault");    //MAM 3/19/07
    std::string     testResult(testFail); //MAM 3/19/07
    bool  brakeOn = false;    //MAM 3/16/07

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::DynamicParkBrake()\n");

    try
    {   // update the the brake force until the speed is below the min sample speed
        float startSampleSpeed = GetTestStepInfoFloat("StartSampleSpeed");
        float endSampleSpeed = GetTestStepInfoFloat("EndSampleSpeed");
        bool  startSamplingAtMinForce = GetTestStepInfoBool("StartSamplingAtMinimumForce");
        float minimumStartForce = GetTestStepInfoFloat("MinimumStartForce");
        int   forceSampleCount = GetTestStepInfoInt("ForceSampleCount");
        int   validForceSamples;
        float speed=0;
        INT32 safeBrakeStart = 0;        

        brakeStart = brakeEnd = 0;  // reset the brake array indexes
        validForceSamples = 0;  // No base brake samples taken yet
        // Insert a "safe" start tag in case the operator does not ever hit the brake pedal
        safeBrakeStart = TagArray( tagPrefix+"Start");
        do
        {
            //speed = m_baseBrakeTool->GetSpeed();
            //MAM 03/29/07: from GenericTC - returns average of drive axle speed
            speed = GetRollSpeed();
            Log(LOG_DEV_DATA,"DynamicParkBrake speed = %.2f\n", speed);

            // If we want base brake sampling to start/stop based on force/sample count
            Log(LOG_DEV_DATA, "forceSampleCount: %d, startSamplingAtMinForce: %s", forceSampleCount, startSamplingAtMinForce ? "true" : "false"); 
            if ((0 < forceSampleCount) && (true == startSamplingAtMinForce))
            {
                // check if minimum force for sampling is reached
                Log(LOG_DEV_DATA,"gets to here snorkel 11");
                m_baseBrakeTool->GetIForces(wheelForce);
                Log(LOG_DEV_DATA,"gets to here snorkel 12");
                //if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                Log(LOG_DEV_DATA, "minimumStartForce: %f, LFwheelForce: %f, RFwheelForce: %f, LRwheelForce: %f, RRwheelForce: %f,", minimumStartForce, wheelForce[LFWHEEL], wheelForce[RFWHEEL], wheelForce[LRWHEEL], wheelForce[RRWHEEL]);
                if ((wheelForce[LRWHEEL] > minimumStartForce) && (wheelForce[RRWHEEL] > minimumStartForce) && (0 == brakeStart))
                { //MAM 10/3/08
                    validForceSamples++;
                    // If this is the first sample above min force
                    //if (validForceSamples == 1)
                    //{   // Tag the start
                    brakeStart = TagArray(tagPrefix+"StartForce");
                    Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d\n", brakeStart);
                    StartParkBrakeTimer();
                 }
                else if (validForceSamples)
                {
                    //Wait for the desired number of samples above minimum before we end
                    Log(LOG_DEV_DATA,"gets to here snorkel 13");
                    if (validForceSamples >= forceSampleCount)
                    {   // Tag the end
                        brakeEnd = TagArray(tagPrefix+"StopForce");
                        Log(LOG_DEV_DATA,"Park brake force sample count reached @ %d\n", brakeEnd);
                    }
                    else
                    {
                        validForceSamples++;
                    }                    
                }
            }
            else
            {   // If we want to "re-start" sampling base brake at minimum force value AND we haven't reached min force yet
                Log(LOG_DEV_DATA,"gets to here snorkel 0");
                if ((true == startSamplingAtMinForce) && (0 == validForceSamples))
                {
                    // check if minimum force for sampling is reached
                    Log(LOG_DEV_DATA,"gets to here snorkel 0a");
                    m_baseBrakeTool->GetIForces(wheelForce);
                    Log(LOG_DEV_DATA,"gets to here snorkel 1");
                    Log(LOG_DEV_DATA, "minimumStartForce: %f, LFwheelForce: %f, RFwheelForce: %f, LRwheelForce: %f, RRwheelForce: %f,", minimumStartForce, wheelForce[LFWHEEL], wheelForce[RFWHEEL], wheelForce[LRWHEEL], wheelForce[RRWHEEL]);
                    //if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
                    if ((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce) && (brakeStart == 0)) 
                    { //MAM 12/06/06
                        // Tag the start
                        Log(LOG_DEV_DATA,"gets to here snorkel 2");
                        brakeStart = TagArray(tagPrefix+"StartForce");
                        Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d\n", brakeStart);
                        validForceSamples = 1;
                        Log(LOG_DEV_DATA,"gets to here snorkel 0b");
                        StartParkBrakeTimer();
                        Log(LOG_DEV_DATA,"gets to here snorkel 0c");
                    }
                }
            }

            Log(LOG_DEV_DATA,"gets to here snorkel 15");
            // If we have dropped below start speed and we have not tagged start yet
            if ((speed <= startSampleSpeed) && (brakeStart == 0))
            {
                brakeStart = TagArray(tagPrefix+"StartSpeed");
                Log(LOG_DEV_DATA,"Start sample speed reached @ %d\n", brakeStart);
            }
            // If we have dropped below stop speed and we have not tagged stop yet
            else if ((speed <= endSampleSpeed) && (brakeEnd == 0))
            {
                brakeEnd = TagArray(tagPrefix+"EndSpeed");
                Log(LOG_DEV_DATA,"End sample speed reached @ %d\n", brakeEnd);
                //MAM 9/25/08 - If the driver waited too long to apply the parking brake, we could end up with
                //brakeEnd = brakeStart. If this happens, we're going to default to 5 samples
                if (brakeStart == brakeEnd)
                {
                    brakeEnd = brakeStart + 5;
                    Log(LOG_DEV_DATA,"brakeStart = brakeEnd! Adding 5 to brakeEnd %d\n", brakeEnd);
                }
            }

            // only need to check every 1/10th of a second
            //if (brakeEnd == 0)    BposSleep(100);
            //MAM 10/3/08 - check every 20 ms - same as VME
            if (brakeEnd == 0)    BposSleep(20);


            // Make sure brakes are not being applied
            testStatus = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeOn);
            if ( (BEP_STATUS_SUCCESS == testStatus) && (true == brakeOn))
            {
                Log( LOG_ERRORS, "Saw brake switch ON during DynamicParkBrake()\n");                    
                testStatus = BEP_STATUS_FAILURE;
            }
            //}

        } while ((TimeRemaining()) &&  (brakeEnd == 0) && (testStatus == BEP_STATUS_SUCCESS));

        // Make sure we have a valid brake start tag
        if ( brakeStart == 0)    brakeStart = safeBrakeStart;

        // if brakeEnd has not been set yet, set to brakeStart
        if (brakeEnd == 0) brakeEnd = brakeStart;
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "DynamicParkBrake Exception: %s\n", e.what());
        testStatus = BEP_STATUS_FAILURE;
        testResult = testSoftwareFail;
    }

    //MAM 3/19/07
    // Update overall test step status
    if ( BEP_STATUS_SUCCESS == testStatus)   testResult = testPass;

    // Report the status of the test
    if (true == brakeOn)
    {
        // Send the overall test result
        SendTestResultWithDetail(testResult, 
                                 GetFaultDescription(faultTag),
                                 GetFaultCode(faultTag),
                                 GetFaultName(faultTag));
    }
    else
    {
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::DynamicParkBrake( %d, %d), status=%d\n", 
         brakeStart, brakeEnd, testStatus);

    return(testStatus);
}


template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::AnalyzeDynamicParkBrake(void)
{
    INT32   status = BEP_STATUS_FAILURE;        // the status of the test being performed
    //int     driverDelayTime;
    std::string parkBrakeStatus( BEP_FAIL_STATUS);
    string testResult;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");

    float       elapsedParkBrakeTime, maxParkBrakeTime = 0;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::AnalyzeDynamicParkBrake()\n");
    if (!ShortCircuitTestStep())
    {
        try
        {
            // display driver prompts if desired
            //UpdatePrompts();            
            SetTestStepInfoValue("RearMinForce", GetParameter("DynamicParkBrakeRearMinForce"));
            SetTestStepInfoValue("RearMaxForce", GetParameter("DynamicParkBrakeRearMaxForce"));

            status = AnalyzeParkBrakeForces(m_dynParkBrakeStart, m_dynParkBrakeStop);

            maxParkBrakeTime = GetParameterFloat("MaxParkBrakeTime");
            elapsedParkBrakeTime = ( m_parkBrakeStopTime.tv_sec - m_parkBrakeStartTime.tv_sec ) * 1000
                          + ( m_parkBrakeStopTime.tv_nsec - m_parkBrakeStartTime.tv_nsec ) / 1000000;
            elapsedParkBrakeTime /= 1000;
            Log(LOG_DEV_DATA,"Start Time:%d\n",m_parkBrakeStartTime.tv_sec);
            Log(LOG_DEV_DATA,"Stop Time:%d\n",m_parkBrakeStopTime.tv_sec);
            Log(LOG_DEV_DATA,"Park Brake time: %.2f", elapsedParkBrakeTime);
            Log(LOG_DEV_DATA,"Max Park Brake time: %.2f", maxParkBrakeTime);
            string color = "white";
            if(elapsedParkBrakeTime > maxParkBrakeTime)
            {
                parkBrakeStatus == testFail;
                testResultCode = GetFaultCode("BrakeTimeExceedsMax");
                testDescription = GetFaultDescription("BrakeTimeExceedsMax");
                color = "Red";
            }
            else
            {
                parkBrakeStatus = testPass;
                color = "Green";
            }
            SystemWrite(GetDataTag("ParkBrakeTime"), elapsedParkBrakeTime);
            SystemWrite(GetDataTag("ParkBrakeTimeBGColor"), color);
            char temp[256];
            SendSubtestResultWithDetail("ParkBrakeTime",parkBrakeStatus, testDescription, testResultCode,
                                        "ParkBrakeTime",CreateMessage(temp, 256, "%.2f", elapsedParkBrakeTime),"s",
                                        "MaxParkBrakeTime",CreateMessage(temp, 256,"%.2f",maxParkBrakeTime),"s");
        }
        catch (BepException &e)
        {
            Log(LOG_ERRORS, "TestStepAnalyzeDynamicParkBrake Exception: %s\n", e.what());
            status = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        UpdateResult(status, testResult);
        SendTestResult(testResult, GetTestStepInfo("Description"));        
    }
    else
    {   // Skip the test
        Log(LOG_DEV_DATA, "Skipping test step - %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::AnalyzeDynamicParkBrake(): %s, %d\n", 
         testResult.c_str(), status);

    return(testResult);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd)
{
    INT32           status = BEP_STATUS_SUCCESS;        // the test status
    INT32           testStatus;
    WHEELDATAARRAY  wheelForceArray;
    float           force;        // the current brake force
    char            buffer[256];    // data array for printing
    string          faultCode;  // Code to report to external systems for a failure condition
    string          faultDesc;  // Description of what the test failed for
    int             roller;
    const int       rollerCount = GetRollerCount();

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::AnalyzeParkBrakeForces brakeStart brakeEnd (%d,%d)\n", 
         brakeStart, brakeEnd);

    // Clear out previous park brake force results
    m_parkBrakeForce.clear();

    // read in the data from the brake force array
    testStatus = ReadDataArrays(GetParameter("IcmForceArray"), brakeStart, brakeEnd, wheelForceArray);
    if (BEP_STATUS_SUCCESS == testStatus)
    {
        if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1")
        {
            roller = 0;
            Log(LOG_DEV_DATA, "Setting Roller index to 0 for Single Axle Machine");
        }
        else
            roller = 2;
        //for(roller=0; roller<rollerCount; roller++)
        //MAM 12/06/06 - only look at rear roller forces
        for (; roller<rollerCount; roller++)
        {
            // calculate the average forces and validate the results
            const DATAARRAY &forceArray = wheelForceArray[roller];
            if ( forceArray.size() > 0)
            {
                force = 0;
                DATAARRAY::const_iterator   itr;
                for ( itr=forceArray.begin(); itr!=forceArray.end(); itr++)
                {
                    if(force < *itr)
                        force = *itr;
                    //Log(LOG_DEV_DATA, "Park Brake Force Value: %.2f", *itr);
                }
                //force /= forceArray.size();
                Log(LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeParkBrakeForces() - step1: Max Force Value - %.2f\n", force);
                                                             
                testStatus = ValidateParkBrakeForce(roller, force);

                string color = "white";
                if (testStatus == BEP_STATUS_SUCCESS)
                {
                    color = "Green";
                }
                else
                {
                    color = "Red";
                }
                Log(LOG_DEV_DATA,"Writing Park Brake to GUI:: Force: %f, color: %s, roller: %d, name: %s", force, color.c_str(), roller, rollerName[roller].c_str());
                SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), force);
                SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), color);
            }
            else
            {    // if there is a problem averaging the forces display -1
                Log( LOG_ERRORS, "Force array for roller %d has zero length\n", roller);

                // write the value to -1
                force = -1;
                // update the result and the background color of the result on the GUI
                //SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), "-1");
                SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), "-1");
                SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), "Red");
            }
            // place the normalized value into the park brake force array
            m_parkBrakeForce.push_back(force);

            Log(LOG_DEV_DATA, "FordABSTCTemplate::AnalyzeParkBrakeForces() - step2\n");
            // Set the fault codes and descriptions
            const std::string faultTag(rollerName[roller]+"ParkBrakeForceFault");
            const std::string testResultTag(rollerName[roller] + "ParkBrakeForceTest");
            if ( BEP_STATUS_SUCCESS == testStatus)
            {
                faultCode = "0000";
                faultDesc = rollerName[roller] + " Park Brake Force Test Result";
            }
            else
            {
                UpdateResult(status, testStatus); //MAM 12/13/06 ????
                faultCode = GetFaultCode(faultTag);
                faultDesc = GetFaultDescription(faultTag);
            }
            // send the test results to the TestResultServer
            memset(buffer, 0 , sizeof(buffer));       // clear the data array
            SendSubtestResultWithDetail(testResultTag, testStatus, faultDesc, faultCode,
                                        "ParkBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", force), unitsLBF,
                                        "MaxParkBrakeForce", GetTestStepInfo("RearMaxForce"), unitsLBF,
                                        "MinParkBrakeForce", GetTestStepInfo("RearMinForce"), unitsLBF);
        }
       

    }
    else
    {
        Log( LOG_ERRORS, "Error reading force array: %d\n", testStatus);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::AnalyzeParkBrakeForces(%d,%d), status=%d\n", 
         brakeStart, brakeEnd, testStatus);

    return(testStatus);
}

template <class ModuleType>
INT32 FordABSTCTemplate<ModuleType>::ValidateParkBrakeForce(INT32 roller, float average)
{
    INT32 testStatus = BEP_STATUS_FAILURE;
    string color("Red");

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ValidateParkBrakeForce(%d, %.2f)\n",
         roller, average);

    // read the parameters to compare the average too
    float minForce = (roller < 2) ? GetTestStepInfoFloat("FrontMinForce") : GetTestStepInfoFloat("RearMinForce");
    float maxForce = (roller < 2) ? GetTestStepInfoFloat("FrontMaxForce") : GetTestStepInfoFloat("RearMaxForce");

    Log( LOG_DEV_DATA, "ValidateParkBrakeForce(%d, %f), min: %f, max: %f\n", roller, average, minForce, maxForce);

    // if brake force is in the valid limits set ok
    if ( ((average <= maxForce) || (maxForce <= 0)) && (average >= minForce))
    {
        testStatus = BEP_STATUS_SUCCESS;
        color = "Green";
    }

    // update the result and the background color of the result on the GUI
    //???????????????/

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::ValidateParkBrakeForce(%d, %.2f), status=%d\n",
         roller, average, testStatus);

    return(testStatus);
}

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepDrag(void)
{
    std::string status( BEP_FAIL_STATUS);
    float   dragSpeed = 999.0;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepDrag()\n");

    // Copy our parameters into the test step info for the base class
    if ( true == GetParameterBool("UseDragSpeed"))   dragSpeed = GetParameterFloat( "DragSpeed");
    CopyParamToTestStepInfo( "DragSpeed", dragSpeed);
    CopyParamToTestStepInfo( "DragDelay", GetParameter( "DragDelay"));
    CopyParamToTestStepInfo( "DragSamples", GetParameter( "DragSamples"));

    status = m_baseBrakeTool->TestStepDrag();

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepDrag(), status=%s\n", status.c_str());

    return( status);
}

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepBaseBrake(void)
{
    std::string status( BEP_FAIL_STATUS);
    std::string domain;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepBaseBrake()\n");
    m_zerospeedParkBrakeTimeFlag = true;
    // Update the TestStepInfo prompts to reflect the required center value
    SetPrompt1ForceValue();
    // Copy our parameters into the test step info for the base class
    CopyParamToTestStepInfo( "StartBrakeSampleSpeed", GetParameter( "StartBrakeSampleSpeed"));
    CopyParamToTestStepInfo( "EndBrakeSampleSpeed", GetParameter( "EndBrakeSampleSpeed"));
    CopyParamToTestStepInfo( "BrakeDomain", GetParameter( "BrakeDomain"));
    CopyParamToTestStepInfo( "BrakeTarget", GetParameter( "BrakeTarget"));
    m_zerospeedBrakeTimeFlag = true;
    // Let the base class do all the work
    status = m_baseBrakeTool->TestStepBaseBrake();

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepBaseBrake(), status=%s\n", status.c_str());

    return( status);
}

//MAM 3/25/10 - Copied from GenericTCTemplate.cpp
template <class ModuleType>
string FordABSTCTemplate<ModuleType>::EnterDiagnosticMode(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    INT32   dclBitStatus;   //MAM 9/21/11
    int scanDelay;  //NAN 10/22/14

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::EnterDiagnosticMode()\n");

    //MAM 9/21/11- Enables ClutchOnZeroSpeed
    dclBitStatus = SystemWrite(GetDataTag("DCLControl"), false);
    Log( LOG_DEV_DATA, "EnterDiagnosticMode - Clearing DCLControl PLC bit \n");
    // Check if this step should be skipped
    if (!ShortCircuitTestStep() || GetParameterBool("AlwaysEnterDiagnosticMode"))
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.EnterDiagnosticMode();
                // Determine the test result
                if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "Enter Diagnostic Mode: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                //MAM 10/22/14
                scanDelay = GetTestStepInfoInt( "ScanDelay");
                if ( scanDelay <= 0)     scanDelay = 100; //100 ms min
                BposSleep(scanDelay);
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::EnterDiagnosticMode() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::EnterDiagnosticMode()\n");
    return(testResult);
}

//MAM 12/3/14
template <class ModuleType>
string FordABSTCTemplate<ModuleType>::WriteFingerprint(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;    
    int scanDelay;
    SerialString_t              response;

    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::WriteFingerprint()\n");

    // Check if this step should be skipped
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            //MAM 3/25/10
            float speed=0;
            do
            {   // read the current speed and update the display
                //speed = GetSpeed();
                //MAM 12/21/06: from GenericTC - returns average of drive axle speed
                speed = GetRollSpeed(); 
                if (speed > 0)
                {
                    // check the machine status and wait if ok
                    //testStatus = StatusCheck();
                    //if (testStatus == BEP_STATUS_SUCCESS)
                    Log(LOG_DEV_DATA, "Wheel speed above 0 mph - Prompting Driver to stop!\n");
                    DisplayPrompt( 1, GetPrompt( "ZeroSpeed"), 0);
                    BposSleep(100);
                }
            }while ((speed > 5) && TimeRemaining());

            try
            {   // Try to enter diagnostic mode
                //moduleStatus = m_vehicleModule.WriteFingerprint();
                moduleStatus = m_vehicleModule.GetInfo( GetDataTag("WriteFingerprint"), response );
                // Determine the test result
                if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "WriteFingerprint: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                //MAM 10/22/14
                scanDelay = GetTestStepInfoInt( "ScanDelay");
                if ( scanDelay <= 0)     scanDelay = 100; //100 ms min
                BposSleep(scanDelay);
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::WriteFingerprint() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::WriteFingerprint()\n");
    return(testResult);
}

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::ValveFiringTest(void)
{
    const int   refSampleCount = GetParameterInt("ABSReferenceSamples");
    std::string status( BEP_FAIL_STATUS);
    float       startSpeed; //MAM 3/21/07
    //MAM 8/31/11
    m_absCommFault = false;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ValveFiringTest()\n");

    //MAM 10/30/08
    m_brakeApplicationFault = false;
    //MAM 3/26/10
    m_valveCrossEnd[LFWHEEL] = 0;  m_valveCrossEnd[RFWHEEL] = 0;
    m_valveCrossEnd[LRWHEEL] = 0;  m_valveCrossEnd[RRWHEEL] = 0;

    // Update the TestStepInfo prompts to reflect the required center value
    SetPrompt1ForceValue();
    CopyParamToTestStepInfo( "BrakeDomain", GetParameter( "BrakeDomain"));
    CopyParamToTestStepInfo( "BrakeTarget", GetParameter( "BrakeTarget"));

    // Make sure we have enough "pre reduction" samples for our reference
    if ( refSampleCount > 0) delay( refSampleCount*20);

    //MAM 3/21/07
    // The TRW module will not execute the ABS sequence above 55 MPH. 
    //  We wait here until all wheel speeds have dropped below this speed
    startSpeed = GetParameterFloat( "ABSTestStartSpeed");
    if ( startSpeed > 0) status = WaitForTargetSpeed( startSpeed, DOWN);
    else                status = BEP_STATUS_SUCCESS;

    // Let the base class do all the work
    status = GenericABSTCTemplate<ModuleType>::ValveFiringTest();

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::ValveFiringTest(), status=%s\n", status.c_str());

    return( status);
}
template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::ValveFiringTest(std::string axle)
{
    const int   refSampleCount = GetParameterInt("ABSReferenceSamples");
    std::string status( BEP_FAIL_STATUS);
    float       startSpeed; //MAM 3/21/07
    //MAM 8/31/11
    m_absCommFault = false;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ValveFiringTest()\n");

    //MAM 10/30/08
    m_brakeApplicationFault = false;
    //MAM 3/26/10
    if(ReadSubscribeData(GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData(GetDataTag("FrontAxleTestSelected")) == "1")
    {
        m_valveCrossEnd[LFWHEEL] = 0;  m_valveCrossEnd[RFWHEEL] = 0;
    }
    else
    {
        m_valveCrossEnd[LRWHEEL] = 0;  m_valveCrossEnd[RRWHEEL] = 0;
    }

    // Update the TestStepInfo prompts to reflect the required center value
    SetPrompt1ForceValue();
    CopyParamToTestStepInfo( "BrakeDomain", GetParameter( "BrakeDomain"));
    CopyParamToTestStepInfo( "BrakeTarget", GetParameter( "BrakeTarget"));

    // Make sure we have enough "pre reduction" samples for our reference
    if ( refSampleCount > 0) delay( refSampleCount*20);

    //MAM 3/21/07
    // The TRW module will not execute the ABS sequence above 55 MPH. 
    //  We wait here until all wheel speeds have dropped below this speed
    startSpeed = GetParameterFloat( "ABSTestStartSpeed");
    if ( startSpeed > 0) status = WaitForTargetSpeed( startSpeed, DOWN);
    else                status = BEP_STATUS_SUCCESS;

    string testResult             = BEP_TESTING_STATUS;     // Used to report test step results
    string testResultCode         = "0000";
    string testDescription        = GetTestStepInfo("Description");
    string overallReductionResult = BEP_TESTING_STATUS;
    string overallRecoveryResult  = BEP_TESTING_STATUS;
    INT32  sensorSpeedIndex       = 0;
    // Define a structure for storing the local results
    struct _valveFiringResults
    {
        string reductionResult;
        string recoveryResult;
    } valveFiringResults[GetRollerCount()];
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Verify it is ok to test -- not skip, brake testing in progress or development testing
    if((!ShortCircuitTestStep() && (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)) ||
       GetParameterBool("DeveloperTest"))
    {   // Instruct operator and update GUI
        UpdatePrompts();
        // Determine if the force meter should be updated during valve firing
        if(GetParameterBool("DisableMeterForValveFiring"))
        {
            m_baseBrakeTool->DisableForceUpdates();
            if(GetParameterBool("DeactivateMeterForValveFiring")) SystemWrite("BrakeActive",false);
        }
        else
            m_baseBrakeTool->EnableForceUpdates();
        // Determine if recovery or reduction should be performed first
        bool reductionFirst = GetParameterBool("PerformReductionBeforeRecovery");
        // Tag the start of the reduction/recovery sequence
        m_absStartIndex = TagArray("ABSStart");
        // Determine which firing sequence to use
        XmlNodeMap *firingOrder;
        if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
            firingOrder = &m_abs4ChannelFrontFiringOrder;
        else
            firingOrder = &m_abs4ChannelRearFiringOrder;
        // Get the initial speeds if doing sensor cross check
        if(GetParameterBool("CollectSensorSpeedData"))
        {   // Wait for forces to stabilize
            BposSleep(GetParameterInt("BrakeForceStabilizeTime"));
            // Get the roller speeds
            m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
            // Get the wheel speeds from the module
            m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
            sensorSpeedIndex++;
        }
        // Fire each wheel in the sequence
        for(XmlNodeMapItr iter = firingOrder->begin(); iter != firingOrder->end(); iter++)
        {   // Get the index for the current wheel
            RollerIndexItr rollerIter;
            if(((rollerIter = m_rollerIndex.find(iter->second->getValue())) != m_rollerIndex.end()))
            {
                INT32 wheelNameIndex;
                wheelNameIndex = rollerIter->second;

                // Provide a short delay between valves -- NOTE: use delay instead of BposSleep(), timing is critical!
                delay(GetParameterInt("InterWheelGapDelay"));
                // Determine if reduction or recovery should be performed first
                if(reductionFirst)
                {   // Reduce the wheel
                    valveFiringResults[wheelNameIndex].reductionResult = 
                    Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
                              m_reduxRecovIndex[wheelNameIndex].reductionEnd);
                    // Recover the wheel
                    valveFiringResults[wheelNameIndex].recoveryResult = 
                    Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
                             m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
                }
                else
                {
                    // Recover the wheel
                    valveFiringResults[wheelNameIndex].recoveryResult = 
                    Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
                             m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
                    // Reduce the wheel
                    valveFiringResults[wheelNameIndex].reductionResult = 
                    Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
                              m_reduxRecovIndex[wheelNameIndex].reductionEnd);
                }
                // Get the wheel speed data if performing a wheel speeds sensor cross test
                if(GetParameterBool("CollectSensorSpeedData"))
                {   // Get the roller speeds
                    Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
                        sensorSpeedIndex);
                    m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
                    // Get the wheel speeds from the module
                    Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
                        sensorSpeedIndex);
                    m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
                    sensorSpeedIndex++;
                }
                if(GetParameterBool("SendKeepAliveDurringValveFire"))
                {//send single keep alive message
                    SendKeepAliveMessage();
                }
            }
            else
            {   // No roller index specified
                Log(LOG_ERRORS, "No roller index defined for wheel: %s -- Not firing valves\n", 
                    iter->second->getValue().c_str());
            }
        }
        // Reenable force updates once the valve firing is complete
        m_baseBrakeTool->EnableForceUpdates();
        // Get the final speed data if performing a wheel speeds sensor cross test
        if(GetParameterBool("CollectSensorSpeedData"))
        {   // Get the roller speeds
            Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
                sensorSpeedIndex);
            m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
            // Get the wheel speeds from the module
            Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
                sensorSpeedIndex);
            m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
        }
        // Tag the end of the ABS sequence
        m_absEndIndex = TagArray("ABSEnd");
        // Remove the operator prompts
        RemovePrompts();
        // Assume all wheels have passed unless otherwise indicated
        overallReductionResult = testPass;
        overallRecoveryResult  = testPass;
        // Determine the results of the test
        for(UINT32 wheelIndex = 0; wheelIndex < firingOrder->size(); wheelIndex++)
        {   // Check if the reduction sequence on this wheel failed
            if(testPass != valveFiringResults[wheelIndex].reductionResult)  overallReductionResult = testFail;
            // Check if the recovery sequence on this wheel failed
            if(testPass != valveFiringResults[wheelIndex].recoveryResult)   overallRecoveryResult  = testFail;
        }
        // Send the results for this step
        testResult = ((overallReductionResult == testPass) && (overallRecoveryResult == testPass)) ? testPass : testFail;
        testResultCode = testPass == testResult ? testResultCode : GetFaultCode("ReduxRecovSequenceFail");
        testDescription = testPass == testResult ? testDescription : GetFaultDescription("ReduxRecovSequenceFail");
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "OverallReductionSequence", overallReductionResult, "",
                                 "OverallRecoverySequence", overallRecoveryResult, "");
    }
    else
    {   // Test step is being skipped
        Log(LOG_FN_ENTRY, "Skipping %s -- \n\t\tShortCircuitTestStep: %s\n\t\tBrakeTestingStatus: %s\n\t\tDeveloperTest: %s\n",
            GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
            m_baseBrakeTool->GetBrakeTestingStatus().c_str(), GetParameterBool("DeveloperTest") ? "True" : "False");
        testResult = testSkip;
    }

    status = testResult;
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    /*Here*/

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::ValveFiringTest(), status=%s\n", status.c_str());

    return( status);
}

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepAnalyzeDrag(void)
{
    std::string status( BEP_FAIL_STATUS);

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAnalyzeDrag()\n");

    // Copy our parameters into the test step info for the base class
    CopyParamToTestStepInfo( "FrontMaxDragForce", GetParameter( "FrontMaxDragForce"));
    CopyParamToTestStepInfo( "RearMaxDragForce", GetParameter( "RearMaxDragForce"));
    //MAM 12/3/08
    // Copy our min parameters into the test step info for the base class
    CopyParamToTestStepInfo( "FrontMinDragForce", GetParameter( "FrontMinDragForce"));
    CopyParamToTestStepInfo( "RearMinDragForce", GetParameter( "RearMinDragForce"));

    m_dragTestPassed = false; //MAM 10/30/08

    status = m_baseBrakeTool->TestStepAnalyzeDrag();

    //MAM 10/30/08
    if (BEP_PASS_STATUS == status)
    {
        m_dragTestPassed = true;      
    }
    Log( LOG_DEV_DATA, "FordABSTCTemplate::TestStepAnalyzeDrag(), m_dragTestPassed=%d\n", m_dragTestPassed);

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAnalyzeDrag(), status=%s\n", status.c_str());

    return( status);
}

/*template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepAnalyzeDrag(std::string axle)
{
    std::string status( BEP_FAIL_STATUS);

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAnalyzeDrag()\n");

    // Copy our parameters into the test step info for the base class
    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
    { 
       CopyParamToTestStepInfo( "FrontMinDragForce", GetParameter( "FrontMinDragForce"));
       CopyParamToTestStepInfo( "FrontMaxDragForce", GetParameter( "FrontMaxDragForce"));
    }
    else
    {
        CopyParamToTestStepInfo( "RearMinDragForce", GetParameter( "RearMinDragForce"));
        CopyParamToTestStepInfo( "RearMaxDragForce", GetParameter( "RearMaxDragForce"));
    }
    //MAM 12/3/08
    // Copy our min parameters into the test step info for the base class

    m_dragTestPassed = false; //MAM 10/30/08

    status = m_baseBrakeTool->TestStepAnalyzeDrag();

    //MAM 10/30/08
    if (BEP_PASS_STATUS == status)
    {
        m_dragTestPassed = true;      
    }
    Log( LOG_DEV_DATA, "FordABSTCTemplate::TestStepAnalyzeDrag(), m_dragTestPassed=%d\n", m_dragTestPassed);

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAnalyzeDrag(), status=%s\n", status.c_str());

    return( status);
} */

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepAnalyzeBaseBrake(void)
{
    std::string status( BEP_FAIL_STATUS);
    std::string brakeStatus( BEP_FAIL_STATUS);
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");

    float       elapsedBrakeTime, maxBrakeTime = 0;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAnalyzeBaseBrake()\n");

    // Copy our parameters into the test step info for the base class
    CopyParamToTestStepInfo( "FrontMinBrakeForce", GetParameter( "FrontMinBrakeForce"));
    CopyParamToTestStepInfo( "FrontMaxBrakeForce", GetParameter( "FrontMaxBrakeForce"));
    CopyParamToTestStepInfo( "RearMinBrakeForce", GetParameter( "RearMinBrakeForce"));
    CopyParamToTestStepInfo( "RearMaxBrakeForce", GetParameter( "RearMaxBrakeForce"));

    status = m_baseBrakeTool->TestStepAnalyzeBaseBrake();

    string axle = (ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1" ? "Front" : "Rear");
    maxBrakeTime = (ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1" ? GetParameterFloat("FrontMaxBrakeTime") : GetParameterFloat("RearMaxBrakeTime"));
    elapsedBrakeTime = ( m_brakeStopTime.tv_sec - m_brakeStartTime.tv_sec ) * 1000
                  + ( m_brakeStopTime.tv_nsec - m_brakeStartTime.tv_nsec ) / 1000000;
    elapsedBrakeTime /= 1000;
    Log(LOG_DEV_DATA,"Start Time:%d\n",m_brakeStartTime.tv_sec);
    Log(LOG_DEV_DATA,"Stop Time:%d\n",m_brakeStopTime.tv_sec);
    Log(LOG_DEV_DATA,"Brake time: %.2f", elapsedBrakeTime);
    Log(LOG_DEV_DATA,"Max Brake time: %.2f", maxBrakeTime);
    string color = "white";
    if(elapsedBrakeTime > maxBrakeTime)
    {
        brakeStatus == testFail;
        testResultCode = GetFaultCode("BrakeTimeExceedsMax");
        testDescription = GetFaultDescription("BrakeTimeExceedsMax");
        color = "Red";
    }
    else
    {
        brakeStatus = testPass;
        color = "Green";
    }
    SystemWrite(GetDataTag("BrakeTime"), elapsedBrakeTime);
    SystemWrite(GetDataTag("BrakeTimeBGColor"), color);
    char temp[256];
    SendSubtestResultWithDetail(axle + "BrakeTime",brakeStatus, testDescription, testResultCode,
                                "BrakeTime",CreateMessage(temp, 256, "%.2f", elapsedBrakeTime),"s",
                                "MaxBrakeTime",CreateMessage(temp, 256,"%.2f",maxBrakeTime),"s");

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAnalyzeBaseBrake(), status=%s\n", status.c_str());

    return( status);
}

/*template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepAnalyzeBaseBrake(std::string axle)
{
    std::string status( BEP_FAIL_STATUS);
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float       elapsedTime, maxBrakeTime = 0;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepAnalyzeBaseBrake() for axle %s\n", axle.c_str());

    // Copy our parameters into the test step info for the base class
    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
    {
        CopyParamToTestStepInfo("FrontMinBrakeForce", GetParameter("FrontMinBrakeForce"));
        CopyParamToTestStepInfo("FrontMaxBrakeForce", GetParameter("FrontMaxBrakeForce"));
        maxBrakeTime = GetParameterFloat("FrontMaxBrakeTime");
    }
    else
    {
        CopyParamToTestStepInfo("RearMinBrakeForce", GetParameter("RearMinBrakeForce"));
        CopyParamToTestStepInfo("RearMaxBrakeForce", GetParameter("RearMaxBrakeForce"));
        maxBrakeTime = GetParameterFloat("RearMaxBrakeTime");
    }

    status = m_baseBrakeTool->TestStepAnalyzeBaseBrake();

    //elapsedTime = difftime( m_brakeStopTime, m_brakeStartTime);
    Log(LOG_DEV_DATA,"Brake time: %.2f", elapsedTime);

    if(status == "Pass" && elapsedTime > maxBrakeTime)
    {

        status == testFail;
        testResultCode = GetFaultCode("BrakeTimeExceedsMax");
        testDescription = GetFaultDescription("BrakeTimeExceedsMax");
    }
    SystemWrite(GetDataTag("BrakeTime"), elapsedTime);
    char temp[256];
    SendSubtestResultWithDetail(axle + "BrakeTime",status, testDescription, testResultCode,
                                "BrakeTime",CreateMessage(temp, 256, "%.2f", elapsedTime),"s"
                                "MaxBrakeTime",CreateMessage(temp, 256,"%.2f",maxBrakeTime),"s");

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepAnalyzeBaseBrake(), status=%s\n", status.c_str());

    return( status);
} */

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepBalance(void)
{
    std::string status( BEP_FAIL_STATUS);

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepBalance()\n");

    // Copy our parameters into the test step info for the base class
    CopyParamToTestStepInfo( "FrontSideToSideBalance", GetParameter( "FrontSideToSideBalance"));
    CopyParamToTestStepInfo( "RearSideToSideBalance", GetParameter( "RearSideToSideBalance"));
    CopyParamToTestStepInfo( "BalanceFrontToRearMin", GetParameter( "BalanceFrontToRearMin"));
    CopyParamToTestStepInfo( "BalanceFrontToRearMax", GetParameter( "BalanceFrontToRearMax"));

    status = m_baseBrakeTool->TestStepBalance();
    
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepBalance(), status=%s\n", status.c_str());

    return( status);
}

/*template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepBalance(std::string axle)
{
    std::string status( BEP_FAIL_STATUS);

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepBalance()\n");

    // Copy our parameters into the test step info for the base class
    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
    {
        CopyParamToTestStepInfo( "FrontSideToSideBalance", GetParameter( "FrontSideToSideBalance"));
    }
    else
    {
        CopyParamToTestStepInfo( "RearSideToSideBalance", GetParameter( "RearSideToSideBalance"));
    }


    //status = m_baseBrakeTool->TestStepBalance();
    INT32 testStatus = BEP_STATUS_SUCCESS;      // the status of the test being performed

    if(!ShortCircuitTestStep())
    {
        try
        {
            if(m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
            {   // display driver prompts
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // perform the test
                testStatus = m_baseBrakeTool->FrontBalance();
            }
            else
            {
                if(m_baseBrakeTool->GetBrakeTestingStatus() == "Pass")   testStatus = BEP_STATUS_SUCCESS;
                else                                         testStatus = BEP_STATUS_SKIP;
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "TestStepBalance Exception: %s\n", e.what());
            testStatus = BEP_STATUS_SOFTWARE;
        }

        // update the test status
        UpdateResult(testStatus, status);
        SendTestResult(status, GetTestStepInfo("Description"));

        if(status == BEP_PASS_RESPONSE)
        {
            SystemWrite(GetDataTag("TestResultBox3"), colorGreen);
        }
        else SystemWrite(GetDataTag("TestResultBox3"), colorRed);
    }
    else
    {   // Skip the test
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        status = BEP_SKIP_RESPONSE;
    }

    
    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepBalance(), status=%s\n", status.c_str());

    return( status);
}*/

template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepParkBrakeBalance(void)
{
    std::string     testResult(testFail);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    const std::string faultTag("ParkBrakeBalanceFault");    
    int     driverDelayTime;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::TestStepParkBrakeBalance()\n");

    if (!ShortCircuitTestStep())
    {
        try
        {
            // display driver prompts if desired
            UpdatePrompts();

            // load the parameters and convert to %
            float balance = GetParameterFloat("ParkBrakeBalance") / 100.0;
            Log(LOG_DEV_DATA, "ParkBrakeBalance parameter: %f, values: %f, %f\n", balance, m_parkBrakeForce[0], m_parkBrakeForce[1]);

            float sum = m_parkBrakeForce[0] + m_parkBrakeForce[1];
            float percentBalance = 0;
            if (sum > 0)
            { // if sum > 0 calculate the percentage
                // validate it the balance
                percentBalance = m_parkBrakeForce[0] / sum;        
                if ((percentBalance <= (.5 + balance)) && (percentBalance >= (.5 - balance)))
                {
                    status = BEP_STATUS_SUCCESS;
                    //color = "Green";
                }
                // convert the balnce for display XX.XX%
                percentBalance *= 100;
                Log(LOG_DEV_DATA, "Park Brake Balance Percent = %f\n", percentBalance);
                SystemWrite(GetDataTag("PBBalanceValue"), percentBalance);  //MAM 6/3/10
            }
            else
            {
                //MAM 9/25/08 - If the sum of the 2 forces is less than zero, we're going to pass the balance check
                //to avoid misleading failures
                //percentBalance = -1;
                Log(LOG_DEV_DATA, "ParkBrakeBalance: Sum of forces not greater than zero! %f \n", sum);
                status = BEP_STATUS_SUCCESS;
            }

        }
        catch ( BepException &err)
        {
            Log( LOG_ERRORS, "Exception in TestStepParkBrakeBalance: %s\n", err.GetReason());
            status = BEP_STATUS_SOFTWARE;
        }

        // tell the driver to release the parking brake
        driverDelayTime = GetParameterInt("ParkBrakeDriverDelay");
        DisplayTimedPrompt(GetParameter("ReleaseParkBrakePrompt"), "1", 0, driverDelayTime);
        RemovePrompts();

        // Update overall test step status
        if ( BEP_STATUS_SUCCESS == status)   testResult = testPass;
        else                                testResult = testFail;

        // Report the status of the test
        if (testResult == testPass)
        {
            SendTestResult(testResult, GetTestStepInfo("Description"));
        }
        else
        {
            // Send the overall test result
            SendTestResultWithDetail(testResult, 
                                     GetFaultDescription(faultTag),
                                     GetFaultCode(faultTag),
                                     GetFaultName(faultTag));
        }

    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }


    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::TestStepParkBrakeBalance()\n");

    return( testResult);
}

//MAM 3/14/12
//=============================================================================
template <class ModuleType>
const string FordABSTCTemplate<ModuleType>::TestStepQuickStop (const std::string &value)
{
    Log(LOG_FN_ENTRY, "FordABSTCTemplate::TestStepQuickStop()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the conditions are correct to perform the task
        if (StatusCheck() == BEP_STATUS_SUCCESS)
        {
            if (UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");


            if (SystemWrite(MOTOR_MODE, string(QUICK_BRAKE_MODE)) == BEP_STATUS_SUCCESS)
            {
                bool done = false;
                while ((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if (ReadSubscribeData(GetDataTag("Zerospeed")) != "1")   BposSleep(250);
                    else                                                    done = true;
                }
                // update the status of the test
                if (StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if (!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
                else if (!done)                          status = BEP_FATALFAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            else
                status = BEP_FATALFAIL_STATUS;
        }
        // else the conditions are not correct, indicate not started
        else
            status = BEP_TEST_NOT_STARTED;
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "FordABSTCTemplate::TestStepQuickStop Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // return the system to Boost Mode
    SystemWrite(MOTOR_MODE, string(BOOST_MODE));

    // update the test result
    if (SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "FordABSTCTemplate::TestStepBrakeToStop Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "FordABSTCTemplate::TestStepQuickStop(): status = %s\n", status.c_str());

    return(status);
}

/*template <class ModuleType>
std::string FordABSTCTemplate<ModuleType>::TestStepSendModuleMessage(void)
{
    std::string         testStatus = testPass;
    std::string         faultTag("FaultComError");
    BEP_STATUS_TYPE     status;
    const bool          responseReq = GetTestStepInfoBool( "ResponseRequired");
    const std::string   messageTag = GetTestStepInfo( "MessageTag");

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::SendModuleMessage(%s)\n", messageTag.c_str());

    try
    {
        string          moduleResponse;

        Log( LOG_FN_ENTRY, "Sending message tag '%s' response required=%d\n", messageTag.c_str(), responseReq);

        if ( responseReq)    status = m_vehicleModule.GetInfo( messageTag, moduleResponse);
        else                status = m_vehicleModule.GetInfo( messageTag);

        if ( status != BEP_STATUS_SUCCESS)
        {
            testStatus = ConvertStatusToResponse( status);
        }
    }
    catch (ModuleException& caughtModuleException)
    {
        Log(LOG_ERRORS, "ModuleException error: FordABSTCTemplate::TestStepSendModuleMessage( %s): %s\n", 
            GetTestStepName().c_str(), caughtModuleException.message().c_str());
        testStatus = testSoftwareFail;
        status = BEP_STATUS_SOFTWARE;
        faultTag = "FaultException";
    }
    catch (BepException &ex)
    {
        Log(LOG_ERRORS, "Exception error: FordABSTCTemplate::TestStepSendModuleMessage( %s): %s\n", 
            GetTestStepName().c_str(), ex.Reason().c_str());
        testStatus = testSoftwareFail;
        status = BEP_STATUS_SOFTWARE;
        faultTag = "FaultException";
    }
    catch ( ...)
    {
        Log(LOG_ERRORS, "Unknown Exception: FordABSTCTemplate::TestStepSendModuleMessage( %s)\n", 
            GetTestStepName().c_str());
        testStatus = testSoftwareFail;
        status = BEP_STATUS_SOFTWARE;
        faultTag = "FaultException";
    }

    if (testStatus == testPass)
    {
        SendTestResult(testStatus, GetTestStepInfo("Description"));
    }
    else
    {
        SendTestResultWithDetail(testStatus,
                                 GetTestStepInfo("Description"),
                                 GetFaultCode(faultTag),
                                 GetFaultName(faultTag),
                                 GetFaultDescription(faultTag));
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::SendModuleMessage(%s), status=%s\n", messageTag.c_str(), testStatus.c_str());

    return( testStatus);
} */

template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::ComboTorqueTest(const std::string &direction)
{
    std::string status( BEP_FAIL_STATUS);

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ComboTorqueTest(%s)\n", direction.c_str());

    // Copy our parameters into the test step info for the base class
    CopyParamToTestStepInfo( "TorqueValue", GetParameter("ParkPawlApplyForce"));
    CopyParamToTestStepInfo( "MaxRearDistance", GetParameter("ParkBrakeMaxDistance"));
    CopyParamToTestStepInfo( "MaxFrontDistance", GetParameter("ParkPawlMaxDistance"));

    status = GenericTC::ComboTorqueTest( direction);;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::ComboTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return( status);
}

//MAM 3/24/10
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::ResetCommHw(void)
{
    std::string status( BEP_PASS_STATUS);
    std::string response;
    Log(LOG_DEV_DATA, "FordABSTCTemplate ResetCommHw: Re-Initializing the serial communication hardware\n");
    IBepCommunication commObj;
    commObj.Initialize(SER_SRVR_NAME, "Client", IsDebugOn());
    std::string tag("Reset");
    std::string value("Reset");
    if (commObj.Command(tag, value, response, true))
        status = BEP_FAIL_STATUS;
    Log(LOG_DEV_DATA, "Re-Initialization complete\n");

    return(status); 
}

//MAM 1/31/13
template <class ModuleType>
const std::string FordABSTCTemplate<ModuleType>::DisableCommHw(void)
{

    std::string status( BEP_PASS_STATUS);

    std::string response;
    Log(LOG_DEV_DATA, "FordABSTCTemplate: Disabling the serial communication hardware\n");
    IBepCommunication commObj;
    commObj.Initialize(SER_SRVR_NAME, "Client", IsDebugOn());
    std::string tag("Abort");
    std::string value("Abort");
    if (commObj.Command(tag, value, response, true))
        status = BEP_FAIL_STATUS;
    Log(LOG_DEV_DATA, "Disabling complete\n");

    return(status); 
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::SetPrompt1ForceValue(void)
{
    float   centerValue;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::SetPrompt1ForceValue()\n");

    // Calculate the meter center value to send to eCATS
    // NOTE: The value we send to eCATS is half the actual value
    //MAM 3/20/07 - Use ForceMeterCenterValue parameter if available
    centerValue = GetParameterFloat("ForceMeterCenterValue")/2.0;
    if (0 == centerValue)
    { //backup plan
        centerValue = (GetParameterFloat("FrontMinBrakeForce") + GetParameterFloat("FrontMaxBrakeForce")) / 4.0;
    }

    CopyParamToTestStepInfo( "Prompt1Value1", centerValue);

    Log( LOG_FN_ENTRY, "Exit SetPrompt1ForceValue: ForceMeterCenterValue = %.2f\n", centerValue);
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::UpdateParameterValue(const std::string& tag, const float value)
{
    XmlNode *infoNode = NULL;
    XmlNode *parent = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::UpdateParameterValue(%s, %.2f)\n", tag.c_str(), value);

    sprintf( valStr, "%.2f", value);
    try
    {
        infoNode = (XmlNode*)m_parameters.getNode( tag);
        infoNode->setValue( valStr);
    }
    catch ( XmlException &err)
    {
        try
        {
            parent = (XmlNode*)( ((XmlNode*)m_parameters.getNode(0))->getParent());
        }
        catch ( ...)
        {
            parent = NULL;
        }

        infoNode = new XmlNode( tag, valStr, parent, ELEMENT_NODE);
        m_parameters.addNode( infoNode);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::UpdateParameterValue(%s, %.2f)\n", tag.c_str(), value);
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::UpdateParameterValue(const std::string& tag, const int value)
{
    XmlNode *infoNode = NULL;
    XmlNode *parent = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::UpdateParameterValue(%s, %.2f)\n", tag.c_str(), value);

    sprintf( valStr, "%d", value);
    try
    {
        infoNode = (XmlNode*)m_parameters.getNode( tag);
        infoNode->setValue( valStr);
    }
    catch ( XmlException &err)
    {
        try
        {
            parent = (XmlNode*)( ((XmlNode*)m_parameters.getNode(0))->getParent());
        }
        catch ( ...)
        {
            parent = NULL;
        }

        infoNode = new XmlNode( tag, valStr, parent, ELEMENT_NODE);
        m_parameters.addNode( infoNode);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::UpdateParameterValue(%s, %d)\n", tag.c_str(), value);
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::UpdateParameterValue(const std::string& tag, const std::string& value)
{
    XmlNode *infoNode = NULL;
    XmlNode *parent = NULL;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::UpdateParameterValue(%s, %s)\n", tag.c_str(), value.c_str());

    try
    {
        infoNode = (XmlNode*)m_parameters.getNode( tag);
        infoNode->setValue( value);
    }
    catch ( XmlException &err)
    {
        try
        {
            parent = (XmlNode*)( ((XmlNode*)m_parameters.getNode(0))->getParent());
        }
        catch ( ...)
        {
            parent = NULL;
        }

        infoNode = new XmlNode( tag, value, parent, ELEMENT_NODE);
        m_parameters.addNode( infoNode);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::UpdateParameterValue(%s, %s)\n", tag.c_str(), value.c_str());
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::CopyParamToTestStepInfo(const std::string& tag, const float value)
{
    XmlNode* infoNode = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::CopyParamToTestStepInfo(%s, %.2f)\n", tag.c_str(), value);

    sprintf( valStr, "%.2f", value);
    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( tag);
        infoNode->setValue( valStr);
    }
    catch ( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( tag, valStr, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::CopyParamToTestStepInfo(%s, %.2f)\n", tag.c_str(), value);
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::CopyParamToTestStepInfo(const std::string& tag, const int value)
{
    XmlNode* infoNode = NULL;
    char    valStr[ 128];

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::CopyParamToTestStepInfo(%s, %d)\n", tag.c_str(), value);

    sprintf( valStr, "%d", value);
    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( tag);
        infoNode->setValue( valStr);
    }
    catch ( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( tag, valStr, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::CopyParamToTestStepInfo(%s, %d)\n", tag.c_str(), value);
}

template <class ModuleType>
void FordABSTCTemplate<ModuleType>::CopyParamToTestStepInfo(const std::string& tag, const std::string& value)
{
    XmlNode* infoNode = NULL;

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::CopyParamToTestStepInfo(%s, %s)\n", tag.c_str(), value.c_str());

    try
    {
        infoNode = (XmlNode*)m_currentTestStep->getAttribute( tag);
        infoNode->setValue( value);
    }
    catch ( XmlException &err)
    {
        infoNode = ((XmlNode*)m_currentTestStep)->addChild( tag, value, ATTRIBUTE_NODE);
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::CopyParamToTestStepInfo(%s, %s)\n", tag.c_str(), value.c_str());
}



//MAM 12/23/14
template <class ModuleType>
string FordABSTCTemplate<ModuleType>::CheckPartNumber()
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string ecuPartNumber = "";    
    string expectedPartNum = GetParameter("ModulePartNumber");
    //MAM 2/17/15
    INT32 partNumBytes = GetParameterInt("PartNumberBytes");
    SerialString_t response;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::CheckPartNumber()\n");
    if (!ShortCircuitTestStep())
    {                       // Do not need to skip
        try
        {                    // Read the part number from the module            
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModulePartNumber"), response);

            // Check the status of the data
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {
                for (int ii=0; ii<partNumBytes; ii++)
                {
                    ecuPartNumber[ii] = response[ii+7];
                }

                for (int ii=0; ii<partNumBytes; ii++)
                {
                    if (ecuPartNumber[ii] != expectedPartNum[ii])
                    {
                        testResult = testFail;
                    }
                }

                if (testFail != testResult)
                {
                    testResult = testPass;
                }

                Log(LOG_DEV_DATA, "Part Number Verification: %s - Parameter: %s, Module: %s\n",
                    testResult.c_str(), expectedPartNum.c_str(), ecuPartNumber.c_str());
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("PartNumberMismatch"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("PartNumberMismatch"));
            }
            else
            {                 // Error getting data from the module
                SetCommunicationFailure(true);
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error reading module part number - status: %s\n", 
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result

        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "ECUPartNumber", ecuPartNumber, "",
                                 "PartNumberParameter", GetParameter("ModulePartNumber"), "");

    }
    else
    {                       // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::RequestAbsSecurity(void)
{   // Set up some variables
    //INT32 status = BEP_STATUS_SUCCESS;        // the test status
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    SerialString_t response;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::RequestAbsSecurity()\n");    
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Get the security seed bytes                
                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestSecuritySeeds"),response);

                //MAM 1/25/13
                int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
                if (securitySeedIndex <= 0)
                {
                    securitySeedIndex = 2;
                }
                Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);

                //MAM 10/29/08
                Log(LOG_DEV_DATA, "FordABSTCTemplate::RequestAbsSecurity: seed1=%2X seed2=%2X seed3=%2X \n",response[securitySeedIndex],response[securitySeedIndex+1],response[securitySeedIndex+2]);


                if (BEP_STATUS_SUCCESS == moduleStatus)
                {
                    //Now that we have the seeds, run the Ford security algorithm to calculate the keys
                    Log(LOG_DEV_DATA, "FordABSTCTemplate::RequestAbsSecurity - Calling the Ford security algorithm\n");
                    //MAM 6/25/10
                    int securityLevel = GetParameterInt("SecurityAccessLevel");
                    if (securityLevel <= 0)
                    {
                        securityLevel = 1;
                    }
                    Log(LOG_DEV_DATA, "securityLevel = %d \n", securityLevel);
                    //CalculateSecurityKey(1,response);
                    CalculateSecurityKey(securityLevel,response);

                    //MAM 10/29/08
                    Log(LOG_DEV_DATA, "Calculated Security Keys: key1=%2X key2=%2X key3=%2X \n",response[2],response[3],response[4]);
                    //Here we need to write the 3 key bytes to the security accesss message and send it to the ABS module.
                    moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestSecurityAccess"),response);
                    if (BEP_STATUS_SUCCESS != moduleStatus) SetCommunicationFailure(true);
                }
                else
                    SetCommunicationFailure(true);

                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "RequestAbsSecurity test result: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::RequestAbsSecurity() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::RequestAbsSecurity()\n");
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::RequestBendixSecurity1(void)
{       
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    SerialString_t response;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::RequestBendixSecurity1()\n");    
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Get the security seed bytes                
                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixSeeds1"),response);

                int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
                if (securitySeedIndex <= 0)
                {
                    securitySeedIndex = 6;
                }
                Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);
                //seed is 2 bytes
                Log(LOG_DEV_DATA, "seed1=%2X seed2=%2X \n",response[securitySeedIndex],response[securitySeedIndex+1]);


                if (BEP_STATUS_SUCCESS == moduleStatus)
                {
                    //Now that we have the seeds calculate the key
                    int securityLevel = GetParameterInt("BendixSecurityLevel1");
                    if (securityLevel <= 0)
                    {
                        securityLevel = 1;
                    }
                    Log(LOG_DEV_DATA, "securityLevel = %d \n", securityLevel);                    
                    CalculateBendixKey(securityLevel,response);

                    Log(LOG_DEV_DATA, "Calculated Security Keys: key1=%2X key2=%2X \n",response[2],response[3]);
                    //Here we need to write the 2 key bytes to the security accesss message and send it to the Bendix module.
                    moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixAccess1"),response);
                    if (BEP_STATUS_SUCCESS != moduleStatus) SetCommunicationFailure(true);
                }
                else
                    SetCommunicationFailure(true);

                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "RequestBendixSecurity1 test result: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::RequestBendixSecurity1() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    //MAM 12/3/14 - commented out
    //Write fingerprint data to the module
    /*
    if (testPass == testResult)
    {
        Log(LOG_DEV_DATA, "Writing fingerprint data to the module \n");
        moduleStatus = m_vehicleModule.GetInfo( GetDataTag("WriteFingerprint"), response );        
        if (BEP_STATUS_SUCCESS != moduleStatus)
        {
            Log(LOG_ERRORS, "EvaluateSBA - Error setting StatusDID to PASS\n");
            //can't pass if we can't write the fingerprint?           
            testResult = testFail; //set comm fault here?
        }
    }
      */

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::RequestBendixSecurity1()\n");
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::RequestBendixSecurity3(void)
{       
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    SerialString_t response;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::RequestBendixSecurity3()\n");    
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Get the security seed bytes                
                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixSeeds3"),response);

                int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
                if (securitySeedIndex <= 0)
                {
                    securitySeedIndex = 6;
                }
                Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);
                //seed is 2 bytes
                Log(LOG_DEV_DATA, "seed1=%2X seed2=%2X \n",response[securitySeedIndex],response[securitySeedIndex+1]);


                if (BEP_STATUS_SUCCESS == moduleStatus)
                {
                    //Now that we have the seeds calculate the key
                    int securityLevel = GetParameterInt("BendixSecurityLevel3");
                    if (securityLevel <= 0)
                    {
                        securityLevel = 3;
                    }
                    Log(LOG_DEV_DATA, "securityLevel = %d \n", securityLevel);                    
                    CalculateBendixKey(securityLevel,response);

                    Log(LOG_DEV_DATA, "Calculated Security Keys: key1=%2X key2=%2X \n",response[2],response[3]);
                    //Here we need to write the 2 key bytes to the security accesss message and send it to the Bendix module.
                    moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixAccess3"),response);
                    if (BEP_STATUS_SUCCESS != moduleStatus) SetCommunicationFailure(true);
                }
                else
                    SetCommunicationFailure(true);

                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "RequestBendixSecurity3 test result: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::RequestBendixSecurity3() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    //MAM 12/3/14 - commented out
    //Write fingerprint data to the module
    /*
    if (testPass == testResult)
    {
        Log(LOG_DEV_DATA, "Writing fingerprint data to the module \n");
        moduleStatus = m_vehicleModule.GetInfo( GetDataTag("WriteFingerprint"), response );        
        if (BEP_STATUS_SUCCESS != moduleStatus)
        {
            Log(LOG_ERRORS, "EvaluateSBA - Error setting StatusDID to PASS\n");
            //can't pass if we can't write the fingerprint?           
            testResult = testFail; //set comm fault here?
        }
    }
      */

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::RequestBendixSecurity3()\n");
    return(testResult);
}

template <class ModuleType>
string FordABSTCTemplate<ModuleType>::RequestBendixSecurity7(void)
{       
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    SerialString_t response;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::RequestBendixSecurity7()\n");    
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Get the security seed bytes                
                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixSeeds7"),response);

                int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
                if (securitySeedIndex <= 0)
                {
                    securitySeedIndex = 6;
                }
                Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);
                //seed is 2 bytes
                Log(LOG_DEV_DATA, "seed1=%2X seed2=%2X \n",response[securitySeedIndex],response[securitySeedIndex+1]);


                if (BEP_STATUS_SUCCESS == moduleStatus)
                {
                    //Now that we have the seeds calculate the key
                    int securityLevel = GetParameterInt("BendixSecurityLevel7");
                    if (securityLevel <= 0)
                    {
                        securityLevel = 3;
                    }
                    Log(LOG_DEV_DATA, "securityLevel = %d \n", securityLevel);                    
                    CalculateBendixKey(securityLevel,response);

                    Log(LOG_DEV_DATA, "Calculated Security Keys: key1=%2X key2=%2X \n",response[2],response[3]);
                    //Here we need to write the 2 key bytes to the security accesss message and send it to the Bendix module.
                    moduleStatus = m_vehicleModule.GetInfo(GetDataTag("RequestBendixAccess7"),response);
                    if (BEP_STATUS_SUCCESS != moduleStatus) SetCommunicationFailure(true);
                }
                else
                    SetCommunicationFailure(true);

                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "RequestBendixSecurity7 test result: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "FordABSTCTemplate::RequestBendixSecurity7() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::RequestBendixSecurity7()\n");
    return(testResult);
}

//  MAM 8/1/14
//
//  FUNCTION: CalculateBendixKey
//
//  Take two seed bytes and calculate two key bytes.
//
template <class ModuleType>
void FordABSTCTemplate<ModuleType>::CalculateBendixKey(short level, SerialString_t &seed )
{   
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::CalculateBendixKey()\n");
    unsigned long key = 0;
    unsigned short seedVal = 0;

    //MAM 1/25/13
    int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
    if (securitySeedIndex <= 0)
    {
        securitySeedIndex = 6;
    }
    Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);
    Log(LOG_DEV_DATA, "seed1=%2X seed2=%2X \n",seed[securitySeedIndex],seed[securitySeedIndex+1]);

    seedVal = seed[securitySeedIndex];
    seedVal = seedVal << 8;
    seedVal = seedVal + seed[securitySeedIndex+1];
    Log(LOG_DEV_DATA, "CalculateBendixKey: seedVal=%4X \n",seedVal);

    switch (level)
    {
    case 1: //Vehicle Manufacturer EC-60/EC-80
        key = seedVal*5157+3993;
        break;
    case 2: //Special Functions EC-80
        key = seedVal*25823+8063;
        break;
    case 3: //Special Functions EC-60        
        key = seedVal*4131+9778;
        break;
    case 4: //KB Parameters EC-80
        key = seedVal*136+221;
        break;
    default:
        break;
    }

    Log(LOG_DEV_DATA, "CalculateBendixKey: key=%8X \n",key);

    seed[3] = key&0x000000FF;
    seed[2] = (key >> 8)&0x000000FF;

    Log(LOG_DEV_DATA, "CalculateBendixKey: key1=%2X key2=%2X \n",seed[2],seed[3]);
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::CalculateSecurityKey()\n");

    return;
}

//  MAM 3/12/08
//
//  FUNCTION: CalculateSecurityKey
//
//  Take three seed bytes and five fixed bytes and calculate three key bytes.
//  Using security level 1.
//
//void CalculateSecurityKey(short level, unsigned char seed1, unsigned char seed2, unsigned char seed3)
//MAM 3/11/08
template <class ModuleType>
//void FordABSTCTemplate<ModuleType>::CalculateSecurityKey(short level, unsigned char seed1, unsigned char seed2, unsigned char seed3)
void FordABSTCTemplate<ModuleType>::CalculateSecurityKey(short level, SerialString_t &seed )
{
    unsigned short usSecType;
    unsigned short usCalc;

    unsigned char bChallengeBits[8];
    unsigned char bBitPosition;
    unsigned char bNextChallengeBit;
    unsigned char bNextXORBit;
    unsigned char bNewBit24;
    unsigned char bNewBit;
    unsigned char bGetBit;
    unsigned char bKeyLow;
    unsigned char bKeyHigh;

    unsigned long ulShiftBits;
    unsigned long ulChallengeBits;
    unsigned long ulChallengeBitsLow;
    unsigned long ulChallengeBitsHigh;
    unsigned long ulTempValue;

    // get security type
    //usSecType = pSecParm->usSecurityType;
    usSecType = level;

    //bprintf("\nEnter New CalculateSecurityKey() ");
    Log(LOG_FN_ENTRY, "Enter FordABSTCTemplate::CalculateSecurityKey()\n");

    //MAM 1/25/13
    int securitySeedIndex = GetParameterInt("SecuritySeedIndex");
    if (securitySeedIndex <= 0)
    {
        securitySeedIndex = 2;
    }
    Log(LOG_DEV_DATA, "securitySeedIndex = %d \n", securitySeedIndex);
    Log(LOG_DEV_DATA, "FordABSTCTemplate::CalculateSecurityKey: seed1=%2X seed2=%2X seed3=%2X \n",seed[securitySeedIndex],seed[securitySeedIndex+1],seed[securitySeedIndex+2]);

    switch (usSecType)
    {
    //case SECURITY_TYPE_USER:
    case 1:

        // challenge bytes        
        //MAM 10/21/09
        if (GetParameterBool("SBABrakeSystem"))
        {
            bChallengeBits[3] = 0x83;
            bChallengeBits[4] = 0x55;
            bChallengeBits[5] = 0x07;
            bChallengeBits[6] = 0x51;
            bChallengeBits[7] = 0x9A;
        }
        else if (GetParameterBool("EUBrakeSystem"))
        { //MAM 12/6/12
            bChallengeBits[3] = 0x22;
            bChallengeBits[4] = 0x5F;
            bChallengeBits[5] = 0xB4;
            bChallengeBits[6] = 0xAC;
            bChallengeBits[7] = 0x54;
        }
        else
        {
            bChallengeBits[3] = 0x3F;
            bChallengeBits[4] = 0x43;
            bChallengeBits[5] = 0xEF;
            bChallengeBits[6] = 0x74;
            bChallengeBits[7] = 0xBE;
        }

        break;
        //MAM 6/25/10
    case 3:

        //MAM 02/16/12
        if (GetParameterBool("SBABrakeSystem"))
        {
            bChallengeBits[3] = 0x3F;
            bChallengeBits[4] = 0x43;
            bChallengeBits[5] = 0xEF;
            bChallengeBits[6] = 0x74;
            bChallengeBits[7] = 0xBE;
        }
        else if (GetParameterBool("MKCBrakeSystem"))
        { //MAM 8/5/13
            bChallengeBits[3] = 0x3F;
            bChallengeBits[4] = 0x43;
            bChallengeBits[5] = 0xEF;
            bChallengeBits[6] = 0x74;
            bChallengeBits[7] = 0xBE;
        }
        else if (GetParameterBool("EUBrakeSystem"))
        { //MAM 12/6/12
            //bChallengeBits[3] = 0x08;
            bChallengeBits[3] = 0x80;   //MAM 4/16/15
            bChallengeBits[4] = 0x0F;
            bChallengeBits[5] = 0x1B;
            bChallengeBits[6] = 0x40;
            bChallengeBits[7] = 0x5E;
        }
        else if (GetParameterBool("V363BrakeSystem"))
        { //MAM 12/6/12
            bChallengeBits[3] = 0x08;
            bChallengeBits[4] = 0x0F;
            bChallengeBits[5] = 0x1B;
            bChallengeBits[6] = 0x40;
            bChallengeBits[7] = 0x5E;
        }
        else if (GetParameterBool("H567BrakeSystem"))
        { //MAM 10/28/14
            bChallengeBits[3] = 0x14;
            bChallengeBits[4] = 0x4E;
            bChallengeBits[5] = 0x23;
            bChallengeBits[6] = 0x33;
            bChallengeBits[7] = 0x32;
        }
        else
        {

            bChallengeBits[3] = 0xB8;
            bChallengeBits[4] = 0x3F;
            bChallengeBits[5] = 0x24;
            bChallengeBits[6] = 0x7F;
            bChallengeBits[7] = 0xFF;
        }

        break;

    default:
        //return(lpINVALID_SECURITY_LEVEL);
        break;
    }

    //MAM 6/25/10 - debug
    Log(LOG_DEV_DATA, "security challenge bytes = %2X %2X %2X %2X %2X \n",
        bChallengeBits[3], bChallengeBits[4], bChallengeBits[5], bChallengeBits[6], bChallengeBits[7]);

    //MAM 1/25/13    
    bChallengeBits[0] = seed[securitySeedIndex];
    bChallengeBits[1] = seed[securitySeedIndex+1];
    bChallengeBits[2] = seed[securitySeedIndex+2];

    // init variables
    ulChallengeBitsLow  = 0x00000000;
    ulChallengeBitsHigh = 0x00000000;

    // load challenge bits into lower area
    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[3];

    ulTempValue = ulTempValue<<24;
    ulChallengeBitsLow += ulTempValue;

    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[2];
    ulTempValue = ulTempValue<<16;
    ulChallengeBitsLow += ulTempValue;

    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[1];
    ulTempValue = ulTempValue<<8;
    ulChallengeBitsLow += ulTempValue;

    ulChallengeBitsLow += bChallengeBits[0];

    // load challenge bits into upper area
    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[7];
    ulTempValue = ulTempValue<<24;
    ulChallengeBitsHigh += ulTempValue;

    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[6];
    ulTempValue = ulTempValue<<16;
    ulChallengeBitsHigh += ulTempValue;

    ulTempValue = 0x00000000;
    ulTempValue = bChallengeBits[5];
    ulTempValue = ulTempValue<<8;
    ulChallengeBitsHigh += ulTempValue;

    ulChallengeBitsHigh += bChallengeBits[4];

    // load shift bits
    ulShiftBits = 0x00C541A9;


    //
    // loop
    //
    for (usCalc = 0; usCalc < 64; ++usCalc)
    {
        if (usCalc < 32)
        {
            ulChallengeBits = ulChallengeBitsLow;
            bBitPosition = (unsigned char)usCalc;
        }
        else
        {
            ulChallengeBits = ulChallengeBitsHigh;
            bBitPosition = (unsigned char)(usCalc - 32);
        }

        // get the next challenge bit
        bNextChallengeBit = ReadBitLong(ulChallengeBits,bBitPosition);

        // get the next XOR bit
        bNextXORBit = ReadBitLong(ulShiftBits,0);

        // shift me big boy
        ulShiftBits = ulShiftBits>>1;

        // calculate new "B24"
        bNewBit24 = bNextChallengeBit ^ bNextXORBit;

        // insert new "B24"
        ulShiftBits = AdjustBitLong(ulShiftBits,23,bNewBit24);

        // calculate new "C21"
        bGetBit = ReadBitLong(ulShiftBits,20);
        bNewBit = bNewBit24 ^ bGetBit;

        // insert new "C21"
        ulShiftBits = AdjustBitLong(ulShiftBits,20,bNewBit);

        // calculate new "C16"
        bGetBit = ReadBitLong(ulShiftBits,15);
        bNewBit = bNewBit24 ^ bGetBit;

        // insert new "C16"
        ulShiftBits = AdjustBitLong(ulShiftBits,15,bNewBit);

        // calculate new "C13"
        bGetBit = ReadBitLong(ulShiftBits,12);
        bNewBit = bNewBit24 ^ bGetBit;

        // insert new "C13"
        ulShiftBits = AdjustBitLong(ulShiftBits,12,bNewBit);

        // calculate new "C6"
        bGetBit = ReadBitLong(ulShiftBits,5);
        bNewBit = bNewBit24 ^ bGetBit;

        // insert new "C6"
        ulShiftBits = AdjustBitLong(ulShiftBits,5,bNewBit);

        // calculate new "C4"
        bGetBit = ReadBitLong(ulShiftBits,3);
        bNewBit = bNewBit24 ^ bGetBit;

        // insert new "C4"
        ulShiftBits = AdjustBitLong(ulShiftBits,3,bNewBit);
    }

    //
    // calculate result byte #1
    //
    ulTempValue = ulShiftBits;
    ulTempValue = ulTempValue>>4;
    //pSecParm->bKey1 = (unsigned char)ulTempValue;
    //key1 = (unsigned char)ulTempValue;
    //MAM 10/29/08
    //seed[1] = (unsigned char)ulTempValue;
    seed[2] = (unsigned char)ulTempValue;

    //
    // calculate result byte #2
    //
    ulTempValue = ulShiftBits;
    ulTempValue = ulTempValue>>20;
    bKeyLow = (unsigned char)ulTempValue;
    bKeyLow = (bKeyLow & 0x0F);

    ulTempValue = ulShiftBits;
    ulTempValue = ulTempValue>>8;
    bKeyHigh = (unsigned char)ulTempValue;
    bKeyHigh = (bKeyHigh & 0xF0);

    //pSecParm->bKey2 = bKeyHigh + bKeyLow;
    //key2 = bKeyHigh + bKeyLow;
    seed[3] = bKeyHigh + bKeyLow;

    //
    // calculate result byte #3
    //
    ulTempValue = ulShiftBits;
    ulTempValue = ulTempValue>>16;
    bKeyLow = (unsigned char)ulTempValue;
    bKeyLow = (bKeyLow & 0x0F);

    ulTempValue = ulShiftBits;
    ulTempValue = ulTempValue<<4;
    bKeyHigh = (unsigned char)ulTempValue;
    bKeyHigh = (bKeyHigh & 0xF0);

    //pSecParm->bKey3 = bKeyHigh + bKeyLow;
    //key3 = bKeyHigh + bKeyLow;
    //MAM 10/29/08
    //seed[3] = bKeyHigh + bKeyLow;
    seed[4] = bKeyHigh + bKeyLow;

    //bprintf("\nkey1 = %2X ", key1);
    //bprintf("\nkey2 = %2X ", key2);
    //bprintf("\nkey3 = %2X ", key3);
    //Log(LOG_DEV_DATA, "FordABSTCTemplate::CalculateSecurityKey: key1=%2X key2=%2X key3=%2X \n",seed[1],seed[2],seed[3]);
    Log(LOG_DEV_DATA, "FordABSTCTemplate::CalculateSecurityKey: key1=%2X key2=%2X key3=%2X \n",seed[2],seed[3],seed[4]);
    Log(LOG_FN_ENTRY, "Exit FordABSTCTemplate::CalculateSecurityKey()\n");

    //return(lpSUCCESSFUL);
    return;
}

//
//  FUNCTION: ReadBitLong
//  Input Parameters:
//  unsigned long ulDataValue,       // byte value
//  unsigned char bBitPosition       // bit position  ! must be 0 through 31
//
//unsigned char ReadBitLong( unsigned long ulDataValue,unsigned char bBitPosition)
template <class ModuleType>
unsigned char FordABSTCTemplate<ModuleType>::ReadBitLong( unsigned long ulDataValue,unsigned char bBitPosition)
{
    unsigned char bBitValue;         // bit value
    unsigned long ulBitMask;         // bit mask

    static unsigned long ulBitMaskValues[] =
    {
        0x00000001,     // bit 0  mask (clear all bits except 0)
        0x00000002,     // bit 1  mask
        0x00000004,     // bit 2  mask
        0x00000008,     // bit 3  mask
        0x00000010,     // bit 4  mask
        0x00000020,     // bit 5  mask
        0x00000040,     // bit 6  mask
        0x00000080,     // bit 7  mask
        0x00000100,     // bit 8  mask
        0x00000200,     // bit 9  mask
        0x00000400,     // bit 10 mask
        0x00000800,     // bit 11 mask
        0x00001000,     // bit 12 mask
        0x00002000,     // bit 13 mask
        0x00004000,     // bit 14 mask
        0x00008000,     // bit 15 mask
        0x00010000,     // bit 16 mask
        0x00020000,     // bit 17 mask
        0x00040000,     // bit 18 mask
        0x00080000,     // bit 19 mask
        0x00100000,     // bit 20 mask
        0x00200000,     // bit 21 mask
        0x00400000,     // bit 22 mask
        0x00800000,     // bit 23 mask
        0x01000000,     // bit 24 mask
        0x02000000,     // bit 25 mask
        0x04000000,     // bit 26 mask
        0x08000000,     // bit 27 mask
        0x10000000,     // bit 28 mask
        0x20000000,     // bit 29 mask
        0x40000000,     // bit 30 mask
        0x80000000      // bit 31 mask
    };

    if (bBitPosition > 31)
    {
        return(0);
    }
    else
    {
        ulBitMask = ulBitMaskValues[bBitPosition];

        if ((ulDataValue & ulBitMask) == ulBitMask)
        {
            bBitValue = 0x01;
        }
        else
        {
            bBitValue = 0x00;
        }
    }

    return(bBitValue);
}


//
//  FUNCTION: AdjustBitLong
//
//  Input Parameters:
//  unsigned long ulDataValue,       // data value
//  unsigned char bBitPosition,      // bit position  ! must be 0 through 31
//  unsigned char bBitValue          // bit value     ! must be 0 or 1
//unsigned long AdjustBitLong(unsigned long ulDataValue,unsigned char bBitPosition,unsigned char bBitValue)
template <class ModuleType>
unsigned long FordABSTCTemplate<ModuleType>::AdjustBitLong(unsigned long ulDataValue,unsigned char bBitPosition,unsigned char bBitValue)
{
    unsigned long ulNewDataValue;    // new data value

    static unsigned long ulBitMaskClear[] =
    {
        0xFFFFFFFE,       // bit 0  mask
        0xFFFFFFFD,       // bit 1  mask
        0xFFFFFFFB,       // bit 2  mask
        0xFFFFFFF7,       // bit 3  mask
        0xFFFFFFEF,       // bit 4  mask
        0xFFFFFFDF,       // bit 5  mask
        0xFFFFFFBF,       // bit 6  mask
        0xFFFFFF7F,       // bit 7  mask
        0xFFFFFEFF,       // bit 8  mask
        0xFFFFFDFF,       // bit 9  mask
        0xFFFFFBFF,       // bit 10 mask
        0xFFFFF7FF,       // bit 11 mask
        0xFFFFEFFF,       // bit 12 mask
        0xFFFFDFFF,       // bit 13 mask
        0xFFFFBFFF,       // bit 14 mask
        0xFFFF7FFF,       // bit 15 mask
        0xFFFEFFFF,       // bit 16 mask
        0xFFFDFFFF,       // bit 17 mask
        0xFFFBFFFF,       // bit 18 mask
        0xFFF7FFFF,       // bit 19 mask
        0xFFEFFFFF,       // bit 20 mask
        0xFFDFFFFF,       // bit 21 mask
        0xFFBFFFFF,       // bit 22 mask
        0xFF7FFFFF,       // bit 23 mask
        0xFEFFFFFF,       // bit 24 mask
        0xFDFFFFFF,       // bit 25 mask
        0xFBFFFFFF,       // bit 26 mask
        0xF7FFFFFF,       // bit 27 mask
        0xEFFFFFFF,       // bit 28 mask
        0xDFFFFFFF,       // bit 29 mask
        0xBFFFFFFF,       // bit 30 mask
        0x7FFFFFFF        // bit 31 mask
    };

    static unsigned long ulBitMaskSet[] =
    {
        0x00000001,     // bit 0  mask
        0x00000002,     // bit 1  mask
        0x00000004,     // bit 2  mask
        0x00000008,     // bit 3  mask
        0x00000010,     // bit 4  mask
        0x00000020,     // bit 5  mask
        0x00000040,     // bit 6  mask
        0x00000080,     // bit 7  mask
        0x00000100,     // bit 8  mask
        0x00000200,     // bit 9  mask
        0x00000400,     // bit 10 mask
        0x00000800,     // bit 11 mask
        0x00001000,     // bit 12 mask
        0x00002000,     // bit 13 mask
        0x00004000,     // bit 14 mask
        0x00008000,     // bit 15 mask
        0x00010000,     // bit 16 mask
        0x00020000,     // bit 17 mask
        0x00040000,     // bit 18 mask
        0x00080000,     // bit 19 mask
        0x00100000,     // bit 20 mask
        0x00200000,     // bit 21 mask
        0x00400000,     // bit 22 mask
        0x00800000,     // bit 23 mask
        0x01000000,     // bit 24 mask
        0x02000000,     // bit 25 mask
        0x04000000,     // bit 26 mask
        0x08000000,     // bit 27 mask
        0x10000000,     // bit 28 mask
        0x20000000,     // bit 29 mask
        0x40000000,     // bit 30 mask
        0x80000000      // bit 31 mask
    };

    if (bBitPosition > 31)
    {
        return(0);
    }
    else
    {
        if (bBitValue == 0)
        {
            // clear the specified bit
            ulNewDataValue = (ulDataValue & ulBitMaskClear[bBitPosition]);
        }
        else
        {
            // set the specified bit
            ulNewDataValue = (ulDataValue | ulBitMaskSet[bBitPosition]);
        }
    }

    return(ulNewDataValue);
}

//----------------------------------------------------------------------------
template<class ModuleType>
string FordABSTCTemplate<ModuleType>::IndividualSensorTest(void)
{   //Copied from Isuzu720AbsTc.cpp

	// Log the entry and determine if this test should be performed
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "FordABSTCTemplate::IndividualSensorTest() - Enter");
	if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
	{	// Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
		m_MotorController.Write(COMMAND_SPEED, "0", false);
		m_MotorController.Write(COMMAND_TORQUE, "0", false);
		m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
		BposSleep( 100);
		string sensorResult[4];
		DisplayPrompt(1, GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
		DisplayPrompt(2, GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
		BposSleep(GetParameterInt("IndividualSensorSpeedDriverDelay"));
		// Wait for the operator to remove foot from brake pedal
		bool brakeOn = true;
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		do
		{	// Read the brake switch styatus from the module
			status = m_vehicleModule.ReadModuleData("ReadBrakeSwitch", brakeOn);
			if(brakeOn)	BposSleep(500);
			// Keep checking until time expires or brake off or bad status
		} while(TimeRemaining() && brakeOn && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status));
		// Make sure it is still OK to continue with the test
		if(!brakeOn && (BEP_STATUS_SUCCESS == StatusCheck()))
		{	// Get individual wheel speed from config to send to motor controller instead of above (one speed for all axles)
			for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= GetRollerCount()) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
			{
				if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                {
                    Log(LOG_DEV_DATA,"Loading Rear parameters for a Single Axle Machine\n");
                    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", GetParameter("Individual" + rollerName[rollerIndex+2] + "SensorTestSpeed"), true);
                }
                else
                    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", GetParameter("Individual" + rollerName[rollerIndex] + "SensorTestSpeed"), true);
			}
			// Wait a bit for the rollers to start accelerating
			BposSleep(GetParameterInt("IndividualSensorTestStartDelay"));
			// Stop the keep alive
			StopModuleKeepAlive();
			// Read the sensor speeds from the module
			WheelSpeeds_t moduleSpeeds;
			//status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
            status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds);
            int i = 0;
            for(;i<4;i++)
                Log(LOG_DEV_DATA, "WheelIndex:%d, TestComponentWheelSpeed:%g", i, moduleSpeeds[i]);
			// Start the Keep Alive thread
			StartModuleKeepAlive();
			if(BEP_STATUS_SUCCESS == status)
			{	// Get the roller speeds
				float rollerSpeeds[4];
				GetWheelSpeeds(rollerSpeeds);
                if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                {
                    rollerSpeeds[2] = rollerSpeeds[0];
                    rollerSpeeds[3] = rollerSpeeds[1];
                }
				// Check all wheels
				float maxSpeeds[4];
				float minSpeeds[4];  //ERIC: NOT SURE IF I SHOULD HARD CODE A 4 OR LOOP TILL GetRollerCount()
				for(UINT8 rollerIndex = LFWHEEL; (rollerIndex < GetRollerCount()) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
				{
					maxSpeeds[rollerIndex] = rollerSpeeds[rollerIndex] * (1.0 + (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));
					minSpeeds[rollerIndex] = rollerSpeeds[rollerIndex] * (1.0 - (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));

                    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                    {
                        Log(LOG_DEV_DATA,"Checking rear speed parameters against the roller speeds for Single Axle machine\n");
                        maxSpeeds[rollerIndex+2] = rollerSpeeds[rollerIndex] * (1.0 + (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));
					    minSpeeds[rollerIndex+2] = rollerSpeeds[rollerIndex] * (1.0 - (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));
					    sensorResult[rollerIndex+2] = (rollerSpeeds[rollerIndex] >= (GetParameterFloat("Individual" + rollerName[rollerIndex+2] + "SensorTestSpeed") - 1.0)) ? testPass : testFail;
                    }
                    else
					    sensorResult[rollerIndex] = (rollerSpeeds[rollerIndex] >= (GetParameterFloat("Individual" + rollerName[rollerIndex] + "SensorTestSpeed") - 1.0)) ? testPass : testFail;
				}
                UINT8 wheel;
                if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                    wheel = LRWHEEL;
                else
                    wheel = LFWHEEL;
                Log(LOG_DEV_DATA, "Checking Wheel Sensor Speeds against parameters\n");
                UINT32 wheelCount = GetWheelCount();
                if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                    {
                        wheelCount = 4; //setting to 4 for RearAxle test on SingleAxleMachine
                    }
                for (; (wheel <= wheelCount) && (BEP_STATUS_SUCCESS == StatusCheck()); wheel++)
				{
                    //Check if running a front axle single axle test, rear results should not be recorded in this case 
                    if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1" && wheel == 2)
                    {
                        Log(LOG_DEV_DATA, "Breaking out of Test Result check loop because Single Axle machine running a Front Axle Test");
                        break;
                    }
                    Log(LOG_DEV_DATA,"Checking Test Result for wheel %d\n", wheel);
					if(!sensorResult[wheel].compare(testPass))
					{
						sensorResult[wheel] = ((minSpeeds[wheel] <= moduleSpeeds[wheel]) && (moduleSpeeds[wheel] <= maxSpeeds[wheel])) ? testPass : testFail;
					}

					// Log and send sub wheel test result
					Log(LOG_DEV_DATA, "%s: %s - Sensor Speed: %.2f, Roller Speed: %.2f, [min: %.2f, max: %.2f]", 
						rollerName[wheel].c_str(), sensorResult[wheel].c_str(), moduleSpeeds[wheel], 
						rollerSpeeds[wheel], minSpeeds[wheel], maxSpeeds[wheel]);
					// Report this roller
					char buff[32];
					SendSubtestResultWithDetail(rollerName[wheel]+"SensorSpeedTest", sensorResult[wheel], 
												GetTestStepInfo("Description"), "0000",
												"SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[wheel]), unitsMPH,
												"MinLimit", CreateMessage(buff, sizeof(buff), "%.2f", minSpeeds[wheel]), unitsMPH,
												"MaxLimit", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeeds[wheel]), unitsMPH,
												"RollerSpeed", GetParameter("Individual" + rollerName[wheel] + "SensorTestSpeed"), unitsMPH);
				}                
			}
			else
			{	// Could not read sensor speeds
				Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
                UINT8 rollerIndex;
                if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
                    rollerIndex = LRWHEEL;
                else
                    rollerIndex = LFWHEEL;
				for(; (rollerIndex < GetRollerCount()); rollerIndex++)
				{
					sensorResult[rollerIndex] = testFail;
				}
			}
            Log(LOG_DEV_DATA, "Resetting Machine to default state");
			// Remove the prompts
			RemovePrompt(1, GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
			RemovePrompt(2, GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
			// Set the motor back to boost and zero speed
			for(UINT8 rollerIndex = LFWHEEL; (rollerIndex < GetWheelCount()); rollerIndex++)
			{
				m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", "0", true);
			}
			// Wait for zero speed
			CheckZeroSpeed();
			m_MotorController.Write(COMMAND_SPEED, "0", false);
			m_MotorController.Write(COMMAND_TORQUE, "0", false);
			m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
			BposSleep( 100);
		}
		else
		{	// Could not perform the test
			Log(LOG_ERRORS, "Could not perform sensor test - brake applied: %s,  System Status: %s",
				brakeOn ? "True" : "False", ConvertStatusToResponse(StatusCheck()).c_str());
		}
		// Remove the prompts
		RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
		RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
		// Report the overall result

        if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "1")
        {
            testResult = (!sensorResult[LFWHEEL].compare(testPass) && !sensorResult[RFWHEEL].compare(testPass)) ? testPass : testFail;
            SendSubtestResult("Front" + GetTestStepName(),testResult, GetTestStepInfo("Description"), "0000");
        }
        else if(ReadSubscribeData( GetDataTag("SingleAxleMachine")) == "1" && ReadSubscribeData( GetDataTag("FrontAxleTestSelected")) == "0")
        {
            testResult = (!sensorResult[LRWHEEL].compare(testPass) && !sensorResult[RRWHEEL].compare(testPass)) ? testPass : testFail;
            SendSubtestResult("Rear" + GetTestStepName(),testResult, GetTestStepInfo("Description"), "0000");
        }
        else
        {
		    testResult = (!sensorResult[LFWHEEL].compare(testPass) && !sensorResult[RFWHEEL].compare(testPass) &&
					      !sensorResult[LRWHEEL].compare(testPass) && !sensorResult[RRWHEEL].compare(testPass)) ? testPass : testFail;
            SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
        }
		
	}
	else if(!GetTestStepResult().compare(testPass))
	{	// This test already passed, skip it but leave it marked as pass
		Log(LOG_DEV_DATA, "Already passed sensor test, not testing again");
		testResult = testPass;
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping sensor test");
		testResult = testSkip;
	}
	// Log the exit and return the result.
	Log(LOG_FN_ENTRY, "FordABSTCTemplate::IndividualSensorTest() - Exit");
	return testResult;
}

template<class ModuleType>
string FordABSTCTemplate<ModuleType>::AccelerateRolls()
{
    std::string     testStatus( testPass);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    std::string lfArmSpeed( GetTestStepInfo( "LfArmSpeed"));
    std::string lfSpeedTag( "LeftFrontSpeedValue");
    std::string rfArmSpeed( GetTestStepInfo( "RfArmSpeed"));
    std::string rfSpeedTag( "RightFrontSpeedValue");

    Log( LOG_FN_ENTRY, "Enter FordABSTCTemplate::AccelerateRolls()\n");

    // Update the speed target
    SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedTarget"));

    Log( LOG_DEV_DATA, "\tLF: Commanding %s=%s\n", lfSpeedTag.c_str(), lfArmSpeed.c_str());
    Log( LOG_DEV_DATA, "\tRF: Commanding %s=%s\n", rfSpeedTag.c_str(), rfArmSpeed.c_str());

    // Tell the motor controller what to do
    if( atof(lfArmSpeed.c_str()) == 0)  m_MotorController.Write("LeftFrontMotorMode", TORQUE_MODE, false);
    else                                m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write(lfSpeedTag, lfArmSpeed, true);

    if( atof(rfArmSpeed.c_str()) == 0)  m_MotorController.Write("RightFrontMotorMode", TORQUE_MODE, false);
    else                                m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write(rfSpeedTag, rfArmSpeed, true);

    if( BEP_STATUS_SUCCESS == status)
    {
        Log( LOG_DEV_DATA, "Successfully commanded motors to speed\n");

        const int   rollerCount = GetRollerCount();
        float       targets[ rollerCount];
        float       wheelSpeeds[ rollerCount];
        uint16_t    ii, atSpeedMask = 0x0000, atTargetMask=0x0000;

        // Build the mask value for when all wheels reach their target
        for( ii=0; ii<rollerCount; ii++)
        {
            atTargetMask |= (1 << ii);
            targets[ ii] = 0.0;
        }

        targets[ 0] = atof( lfArmSpeed.c_str()) - 1.0;
        targets[ 1] = atof( rfArmSpeed.c_str()) - 1.0;

        status = (BEP_STATUS_TYPE)GetWheelSpeeds( wheelSpeeds);

        // Wait for the rollers to accelerate
        while( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) &&
               ((atSpeedMask & atTargetMask) != atTargetMask) )
        {
            atSpeedMask = 0x0000;
            for( ii=0; ii<rollerCount; ii++)
            {
                // If this wheel is above the target or this wheel has no target
                if( ((targets[ ii] > 0) && ( wheelSpeeds[ ii] > targets[ ii])) ||
                    ( targets[ ii] <= 0))
                {
                    // Set bit to sya this wheel is good
                    atSpeedMask|= (1 << ii);
                }
            }
            Log( LOG_DEV_DATA, "At Speed Mask: $%04hX ?= $%04hX\n", atSpeedMask, atTargetMask);

            status = StatusSleep( 200);
            if( BEP_STATUS_SUCCESS == status)
            {
                status = (BEP_STATUS_TYPE)GetWheelSpeeds( wheelSpeeds);
            }
        }
        if( BEP_STATUS_SUCCESS == status)
        {
            m_baseBrakeTool->SetBrakeTestingStatus(BEP_TESTING_RESPONSE);   // flag that the brakes can be tested
            Log( LOG_DEV_DATA, "Rolls successfully reached speed\n");
            testStatus = testPass;
        }
        else
        {
            Log( LOG_ERRORS, "Error waiting for target speed\n");
            testStatus = testFail;
        }
    }
    else
    {
        Log( LOG_ERRORS, "Error commanding motors to speed\n");
        testStatus = testFail;
    }

    Log( LOG_FN_ENTRY, "Exit FordABSTCTemplate::AccelerateRolls()\n");

    return( testStatus);
}

template<class ModuleType>
string FordABSTCTemplate<ModuleType>::AccelerateVehicleToSpeed()
{
    float armSpeed = GetParameterFloat("ArmSpeed");
    std::string     testStatus( testPass);
    UpdatePrompts();
    SetTestStepInfoValue( "LfArmSpeed", armSpeed);
    SetTestStepInfoValue( "RfArmSpeed", armSpeed);

    testStatus = AccelerateRolls();

    // if we successfully reached the arm speed, disengage so we can
    // continue on to the drag test
    if(testStatus == testPass)
    {
        Log(LOG_DEV_DATA, "Armspeed reached, disengaging for drag test");
        m_baseBrakeTool->Disengage();
    }

    return(testStatus);
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string FordABSTCTemplate<ModuleType>::Publish(const XmlNode *node)
{	
    Log(LOG_DEV_DATA,"Enter FordABSTCTemplate::Publish()\n");
    // Check if zerospeed is reached
	if(node != NULL)
	{
		try
		{
			if((node->getName() == "Zerospeed") && (node->getValue() == "1") && m_zerospeedBrakeTimeFlag)
			{	//stop timer here
                Log(LOG_DEV_DATA, "Stopping brake timer at %d seconds\n", m_brakeStopTime.tv_sec);
                clock_gettime( CLOCK_REALTIME, &m_brakeStopTime);
                m_zerospeedBrakeTimeFlag = false;
                m_zerospeedParkBrakeTimeFlag = false;
			}
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "XmlException during FordABSTCTemplate::Publish()1 - %s", excpt.GetReason());
		}
        try
		{
			if((node->getName() == "Zerospeed") && (node->getValue() == "1"))
			{	//stop timer here
                Log(LOG_DEV_DATA, "Stopping park brake timer at %d seconds\n", m_parkBrakeStopTime.tv_sec);
                clock_gettime( CLOCK_REALTIME, &m_parkBrakeStopTime);
                m_zerospeedBrakeTimeFlag = false;
                m_zerospeedParkBrakeTimeFlag = false;
			}
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "XmlException during FordABSTCTemplate::Publish()2 - %s", excpt.GetReason());
		}
	}
    Log(LOG_DEV_DATA, "Exit FordABSTCTemplate::Publish()\n");
	// Pass the publish to the base class for further handling                                                                          
	return GenericTCTemplate<ModuleType>::Publish(node);
}

