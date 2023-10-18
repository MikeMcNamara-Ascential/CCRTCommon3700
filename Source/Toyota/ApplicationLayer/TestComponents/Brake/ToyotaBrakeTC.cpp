//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "ToyotaBrakeTC.h"
#include <numeric>


//-------------------------------------------------------------------------------------------------
ToyotaBrakeTC::ToyotaBrakeTC() : GenericBaseBrakeTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
ToyotaBrakeTC::~ToyotaBrakeTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string ToyotaBrakeTC::CommandTestStep(const string& value)
{
   string testResult(BEP_TESTING_STATUS);
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::CommandTestStep(%s) - Enter", value.c_str());
   // Make sure it is OK to perform the test step
   m_baseBrakeTool->SetBrakeTestingStatus(BEP_TESTING_RESPONSE);
   if (BEP_STATUS_SUCCESS == StatusCheck())
   {
      if (!GetTestStepName().compare("AccelerateRollers"))
      {   // Set the brake ground colors to indicate testing
         SystemWrite(GetDataTag("LFBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
         SystemWrite(GetDataTag("RFBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
         SystemWrite(GetDataTag("LRBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
         SystemWrite(GetDataTag("RRBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
         testResult = AccelerateVehicleToSpeed(GetVehicleParameter("BrakeDistanceTestSpeed", (float)0.0), true, true);
      }
      else if (!GetTestStepName().compare("BrakeStoppingDistanceTest")) testResult = BrakeDistanceTest();
      else if (!GetTestStepName().compare("MaxBrakeForceMeasurement"))  testResult = MaxBrakeForceVerification();
      else if (!GetTestStepName().compare("MaxBrakeForceCalibration"))  testResult = MaxBrakeForceCalibration();
      else if (!GetTestStepName().compare("ParkBrakeForceMeasurement")) testResult = ParkBrakeForceVerification();
      else if (!GetTestStepName().compare("ParkBrakeForceCalibration")) testResult = ParkBrakeForceCalibration();
      else if (!GetTestStepName().compare("PreShiftCheckout"))          testResult = PreShiftCheck();
      else if (!GetTestStepName().compare("TractionControlTest"))       testResult = TractionControlCheck();
      else if (!GetTestStepName().compare("DualBrakeForceMeasurement"))  testResult = DualBrakeForceVerification();
      else if (!GetTestStepName().compare("Initialize"))
      {   // Reset the system tags and then call the base class to finish the Initialize
         ResetSystemTags();
         testResult = TestStepInitialize();
      }
      else testResult = GenericBaseBrakeTC::CommandTestStep(value);
   }
   else
   {   // System is not OK to perform brake test
      testResult = testSkip;
      Log(LOG_ERRORS, "System status is preventing test: %s", ConvertStatusToResponse(StatusCheck()).c_str());
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::CommandTestStep(%s) - Exit", value.c_str());
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::AccelerateVehicleToSpeed(const float targetSpeed,
                                               const bool startAtZeroSpeed,
                                               const bool disengageAtSpeed)
{   // Log the entry and make sure the test step should be performed
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::AccelerateVehicleToSpeed() - Enter");
   string testResult(BEP_TESTING_STATUS);
   if (!ShortCircuitTestStep())
   {   // Instruct the machine to engage the ABS motors
      SystemWrite(GetDataTag("EngageEbsMotors"), true);
      // Instruct the operator to shift to Neutral and prepare for acceleration
      DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"),
                    GetPromptPriority("RemoveFootFromBrake"));
      DisplayTimedPrompt(GetPrompt("NeutralPrompt"), GetPromptBox("NeutralPrompt"), GetPromptPriority("NeutralPrompt"),
                         GetPromptDuration("NeutralPrompt"));
      RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"),
                   GetPromptPriority("RemoveFootFromBrake"));
      // Wait for the wheel speeds to reach the target speed
      while ((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && (GetRollSpeed() < targetSpeed)) BposSleep(250);
      // Determine the result
      if (BEP_STATUS_SUCCESS != StatusCheck())
      {   // Bad system status
         Log(LOG_ERRORS, "Bad system status during roller acceleration - %s",
             ConvertStatusToResponse(StatusCheck()).c_str());
         testResult = testFail;
      }
      else if (!TimeRemaining())
      {   // Timeout waiting for target speed
         Log(LOG_ERRORS, "Timeout waiting for rollers to reach target speed - %.2f [current speed: %.2f]",
             targetSpeed, GetRollSpeed());
         testResult = testTimeout;
      }
      else if (GetRollSpeed() >= targetSpeed)
      {
         Log(LOG_DEV_DATA, "Target speed reached");
         testResult = testPass;
      }
      // Disengage the ABS motors once the target speed is reached
      SystemWrite(GetDataTag("EngageEbsMotors"), false);
      // Report the result
      SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
   }
   else
   {   // Need to skip this test step
      Log(LOG_FN_ENTRY, "Skipping ToyotaBrakeTC::AccelerateVehicleToSpeed()");
      testResult = testSkip;
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::AccelerateVehicleToSpeed() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::BrakeDistanceTest(void)
{   // Log the entry and determine if this should be checked
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::BrakeDistanceTest() - Enter");
   string testResult(BEP_TESTING_RESPONSE);
   if (!ShortCircuitTestStep())
   {   // Wait for vehicle speed to drop a bit
      DisplayPrompt(GetPromptBox("BrakeToStop"), GetPrompt("BrakeToStop"), GetPromptPriority("BrakeToStop"));
      INT32 sampleInterval = GetParameterInt("BrakeDistanceSpeedSampleInterval");
      float startSpeed = GetVehicleParameter("BrakeDistanceTestStartMeasureSpeed", (float)0.0);
      while ((SystemReadFloat("LFSpeed") > startSpeed) && (SystemReadFloat("RFSpeed") > startSpeed))
      {
         BposSleep(sampleInterval);
      }
      // Set the start distance
      WHEELINFO initialDistance;
      GetWheelDistances(initialDistance);
      Log(LOG_DEV_DATA, "Distance measure started:  LF Speed: %.2f,  RF Speed: %.2f",
          SystemReadFloat("LFSpeed"), SystemReadFloat("RFSpeed"));
      // Wait for zero speed
      CheckZeroSpeed();
      RemovePrompt(GetPromptBox("ZeroSpeed"), GetPrompt("ZeroSpeed"), GetPromptPriority("ZeroSpeed"));
      // Get the final distances
      WHEELINFO finalDistance;
      GetWheelDistances(finalDistance);
      // Find the delta
      WHEELINFO stoppingDistance;
      GetTotalDistances(stoppingDistance, initialDistance, finalDistance);
      // Convert the data to meters
      float circumference = 3.1415927 * GetParameterFloat("RollerDiameter");

      float revs = stoppingDistance.lfWheel / GetParameterFloat("PulsesPerRevolution");
      float lfDist = (revs * circumference) / 39.36996;  // Convert the distance to meters - 39.36996 inches in a meter
      Log(LOG_DEV_DATA, "LF Stopping Distance: %.2f", lfDist);
      bool lfPass = ((lfDist < GetVehicleParameter("FrontMaximumStoppingDistance", (float)0.0)) &&
                     (lfDist > GetVehicleParameter("FrontMinimumStoppingDistance", (float)0.0)));
      SystemWrite(GetDataTag("LFBrakeDistanceTag"), lfDist);
      SystemWrite(GetDataTag("LFBrakeDistanceColorTag"), lfPass ? string("green") : string("red"));

      revs = stoppingDistance.rfWheel / GetParameterFloat("PulsesPerRevolution");
      float rfDist = (revs * circumference) / 39.36996;  // Convert the distance to meters - 39.36996 inches in a meter
      Log(LOG_DEV_DATA, "RF Stopping Distance: %.2f", rfDist);
      bool rfPass = ((rfDist < GetVehicleParameter("FrontMaximumStoppingDistance", (float)0.0)) &&
                     (rfDist > GetVehicleParameter("FrontMinimumStoppingDistance", (float)0.0)));
      SystemWrite(GetDataTag("RFBrakeDistanceTag"), rfDist);
      SystemWrite(GetDataTag("RFBrakeDistanceColorTag"), rfPass ? string("green") : string("red"));

      revs = stoppingDistance.lrWheel / GetParameterFloat("PulsesPerRevolution");
      float lrDist = (revs * circumference) / 39.36996;  // Convert the distance to meters - 39.36996 inches in a meter
      Log(LOG_DEV_DATA, "LR Stopping Distance: %.2f", lrDist);
      bool lrPass = ((lrDist < GetVehicleParameter("RearMaximumStoppingDistance", (float)0.0)) &&
                     (lrDist > GetVehicleParameter("RearMinimumStoppingDistance", (float)0.0)));
      SystemWrite(GetDataTag("LRBrakeDistanceTag"), lrDist);
      SystemWrite(GetDataTag("LRBrakeDistanceColorTag"), lrPass ? string("green") : string("red"));

      revs = stoppingDistance.rrWheel / GetParameterFloat("PulsesPerRevolution");
      float rrDist = (revs * circumference) / 39.36996;  // Convert the distance to meters - 39.36996 inches in a meter
      Log(LOG_DEV_DATA, "RR Stopping Distance: %.2f", rrDist);
      bool rrPass = ((rrDist < GetVehicleParameter("RearMaximumStoppingDistance", (float)0.0)) &&
                     (rrDist > GetVehicleParameter("RearMinimumStoppingDistance", (float)0.0)));
      SystemWrite(GetDataTag("RRBrakeDistanceTag"), rrDist);
      SystemWrite(GetDataTag("RRBrakeDistanceColorTag"), rrPass ? string("green") : string("red"));
      // Determine the overall result
      testResult = (lfPass && rfPass && lrPass && rrPass) ? testPass : testFail;
      char buff[32];
      SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                               "LfBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", lfDist), unitsMETER,
                               "RfBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", rfDist), unitsMETER,
                               "LrBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", lrDist), unitsMETER,
                               "RrBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", rrDist), unitsMETER);
   }
   else
   {   // Need to skip this step
      testResult = testSkip;
      Log(LOG_FN_ENTRY, "Skipping Brake distance test");
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::BrakeDistanceTest() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::MaxBrakeForceVerification(void)
{   // Log the entry and see if this step should be performed
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::MaxBrakeForceVerification() - Enter");
   string testResult(BEP_TESTING_STATUS);
   if (!ShortCircuitTestStep())
   {   // Wait for minimum force on all wheels or timeout
      SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("FrontBalanceBGColor"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RearBalanceBGColor"), GetParameter("TestInProgressColor"));
      MaxBrakeData brakeData[GetRollerCount()];
      // Assign the data tags
      for (INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
      {
         brakeData[roller].currentForce = 0.0;
         brakeData[roller].maxForce     = 0.0;
         brakeData[roller].measurementComplete = false;
      }
      brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
      brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
      brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
      brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
      brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
      brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
      brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
      brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
      // Command the high torque motors to engage
      if(GetParameterBool("UseMotorModeHighTorque"))
         SystemWrite("MotorMode","Torque");
      else
         SystemWrite(GetDataTag("EngageHighTorqueMotor"), true);
      // Wait for the rollers to be rolling
      WHEELINFO rollerSpeed;
      do
      {
         GetWheelSpeeds(rollerSpeed);
         BposSleep(GetParameterInt("ApplyBrakePedalWaitTime"));
      }
      while ((rollerSpeed.lfWheel < 1.0) && (BEP_STATUS_SUCCESS == StatusCheck()));
      // Instruct operator to apply brake pedal and wait for a bit
      DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
      bool frontMeasurementComplete = false;
      float fBal = GetVehicleParameter("MaxFrontDifference", (float)0.0) + 20.0;
      float lfMaxForce = 0.0;
      float rfMaxForce = 0.0;
      do
      {   // Make sure the max values are reset
         brakeData[LFWHEEL].maxForce = 0.0;
         brakeData[LFWHEEL].measurementComplete = false;
         brakeData[RFWHEEL].maxForce = 0.0;
         brakeData[RFWHEEL].measurementComplete = false;
         BposSleep(GetParameterInt("FrontMaxBrakeStablizationTime"));
         frontMeasurementComplete = MonitorBrakeForces(LFWHEEL, RFWHEEL,
                                                       GetVehicleParameter("RequiredBrakeForceFront", (float)0.0),
                                                       GetVehicleParameter("RequiredBrakeForceRear", (float)0.0),
                                                       brakeData, GetVehicleParameter("MaxBrakeSamples", (int)0.0));
         // Set the result colors on the screen
         if (brakeData[LFWHEEL].forceSamples.size() > 0)
         {
            lfMaxForce = accumulate(brakeData[LFWHEEL].forceSamples.begin(),
                                    brakeData[LFWHEEL].forceSamples.end(), 0.0) / brakeData[LFWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "LF Max Force: %.2f, samples: %d", lfMaxForce, brakeData[LFWHEEL].forceSamples.size());
         }
         else
         {
            lfMaxForce = 0.0;
            Log(LOG_ERRORS, "LF MAX Force not measured, no samples collected");
         }
         if (brakeData[RFWHEEL].forceSamples.size() > 0)
         {
            rfMaxForce = accumulate(brakeData[RFWHEEL].forceSamples.begin(),
                                    brakeData[RFWHEEL].forceSamples.end(), 0.0) / brakeData[RFWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "RF Max Force: %.2f, samples: %d", rfMaxForce, brakeData[RFWHEEL].forceSamples.size());
         }
         else
         {
            rfMaxForce = 0.0;
            Log(LOG_ERRORS, "RF MAX Force not measured, no samples collected");
         }
         SystemWrite(brakeData[LFWHEEL].displayTag, lfMaxForce);
         SystemWrite(brakeData[RFWHEEL].displayTag, rfMaxForce);
         SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"),
                     lfMaxForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
         SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"),
                     rfMaxForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
         // Set the brake balance for each axle
         fBal = abs(lfMaxForce - rfMaxForce);
      }
      while ((!frontMeasurementComplete || (fBal > GetVehicleParameter("MaxFrontDifference", (float)0.0))) && TimeRemaining() &&
             (BEP_STATUS_SUCCESS == StatusCheck()));
      // Command the high torque motors to disengage
      SystemWrite(GetDataTag("EngageHighTorqueMotor"), false);
      RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
      DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"),
                    GetPromptPriority("RemoveFootFromBrake"));
      SystemWrite(GetDataTag("FrontBalanceValue"), fBal);
      SystemWrite(GetDataTag("FrontBalanceBGColor"),
                  fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0) ? string("green") : string("red"));
      // Log the test result data
      Log(LOG_DEV_DATA, "LF Max Force: %.2f    RF Max Force: %.2f     Balance: %.2f",
          lfMaxForce, rfMaxForce, fBal);
      // Command the high torque motors to engage
      SetStartTime(); 
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
      do
      {
         GetWheelSpeeds(rollerSpeed);
         BposSleep(GetParameterInt("ApplyBrakePedalWaitTime"));
      }
      while ((rollerSpeed.lrWheel < 1.0) && (BEP_STATUS_SUCCESS == StatusCheck()));
      RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"),
                   GetPromptPriority("RemoveFootFromBrake"));
      DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
      SetStartTime();
      bool rearMeasurementComplete = false;
      float rBal = GetVehicleParameter("MaxRearDifference", (float)0.0) + 20.0;
      float lrMaxForce = 0.0;
      float rrMaxForce = 0.0;
      do
      {   // Make sure the max values are reset
         Log(LOG_DEV_DATA, "Setting rear max brake values to incomplete state");
         brakeData[LRWHEEL].maxForce = 0.0;
         brakeData[LRWHEEL].measurementComplete = false;
         brakeData[RRWHEEL].maxForce = 0.0;
         brakeData[RRWHEEL].measurementComplete = false;
         BposSleep(GetParameterInt("RearMaxBrakeStablizationTime"));
         rearMeasurementComplete = MonitorBrakeForces(LRWHEEL, RRWHEEL,
                                                      GetVehicleParameter("RequiredBrakeForceFront", (float)0.0),
                                                      GetVehicleParameter("RequiredBrakeForceRear", (float)0.0),
                                                      brakeData, GetVehicleParameter("MaxBrakeSamples", (int)0));
         // Set the result colors on the screen
         if (brakeData[LRWHEEL].forceSamples.size() > 0)
         {
            lrMaxForce = accumulate(brakeData[LRWHEEL].forceSamples.begin(),
                                    brakeData[LRWHEEL].forceSamples.end(), 0.0) / brakeData[LRWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "LR Max Force: %.2f, samples: %d", lrMaxForce, brakeData[LRWHEEL].forceSamples.size());
         }
         else
         {
            lrMaxForce = 0.0;
            Log(LOG_ERRORS, "LR MAX Force not measured, no samples collected");
         }
         if (brakeData[RRWHEEL].forceSamples.size() > 0)
         {
            rrMaxForce = accumulate(brakeData[RRWHEEL].forceSamples.begin(),
                                    brakeData[RRWHEEL].forceSamples.end(), 0.0) / brakeData[RRWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "RR Max Force: %.2f, samples: %d", rrMaxForce, brakeData[RRWHEEL].forceSamples.size());
         }
         else
         {
            rrMaxForce = 0.0;
            Log(LOG_ERRORS, "RR MAX Force not measured, no samples collected");
         }
         SystemWrite(brakeData[LRWHEEL].displayTag, lrMaxForce);
         SystemWrite(brakeData[RRWHEEL].displayTag, rrMaxForce);
         SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"),
                     lrMaxForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
         SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"),
                     rrMaxForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
         // Set the brake balance for each axle
         rBal = abs(lrMaxForce - rrMaxForce);
         if (rBal > GetVehicleParameter("MaxRearDifference", (float)0.0))
         {
            Log(LOG_DEV_DATA, "Rear brake difference [%.2f] is out of spec!", rBal);
         }
      }
      while ((!rearMeasurementComplete || (rBal > GetVehicleParameter("MaxRearDifference", (float)0.0))) && TimeRemaining() &&
             (BEP_STATUS_SUCCESS == StatusCheck()));
      // Command the high torque motors to disengage
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
      SystemWrite(GetDataTag("RearBalanceValue"), rBal);
      SystemWrite(GetDataTag("RearBalanceBGColor"),
                  rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0) ? string("green") : string("red"));
      // Log the test result data
      Log(LOG_DEV_DATA, "LR Max Force: %.2f    RR Max Force: %.2f     Balance: %.2f",
          lrMaxForce, rrMaxForce, rBal);

      RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));

      // Determine the results
      if (BEP_STATUS_SUCCESS != StatusCheck())
      {   // Bad system status during the test
         Log(LOG_ERRORS, "Bad system status during max brake force testing: %s",
             ConvertStatusToResponse(StatusCheck()).c_str());
         testResult = testFail;
      }
      else if (!TimeRemaining())
      {   // Timeout during max brake force testing
         Log(LOG_ERRORS, "Timeout waiting for minimum required brake force");
         testResult = testTimeout;
      }
      else if (frontMeasurementComplete && rearMeasurementComplete &&
               (fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0)) &&
               (rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0)))
      {   // All brake forces are good
         Log(LOG_DEV_DATA, "All wheels met minimum brake force requirements");
         testResult = testPass;
      }
      else
      {
         testResult = testFail;
      }
      // Report the results
      char buff[32];
      TestResultDetails testData;
      testData.AddDetail("LFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", lfMaxForce), unitsKGF);
      testData.AddDetail("RFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rfMaxForce), unitsKGF);
      testData.AddDetail("LRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", lrMaxForce), unitsKGF);
      testData.AddDetail("RRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rrMaxForce), unitsKGF);
      testData.AddDetail("FrontBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", fBal), unitsKGF);
      testData.AddDetail("RearBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", rBal), unitsKGF);
      SendTestResultWithDetail(testResult, testData, GetTestStepInfo("Description"), "0000");
   }
   else
   {
      Log(LOG_FN_ENTRY, "Skipping ToyotaBrakeTC::MaxBrakeForceVerification()");
      testResult = testSkip;
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::MaxBrakeForceVerification() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::MaxBrakeForceCalibration(void)
{
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::MaxBrakeForceCalibration() - Enter");
   // Set up the max brake force structure
   MaxBrakeData brakeData[GetRollerCount()];
   // Assign the data tags
   for (INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
   {
      brakeData[roller].currentForce = 0.0;
      brakeData[roller].maxForce     = 0.0;
      brakeData[roller].measurementComplete = false;
   }
   brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
   brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
   brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
   brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
   brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
   brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
   brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
   brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
   // Clear initial background colors
   SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("FrontBalanceBGColor"), string("white"));
   SystemWrite(GetDataTag("RearBalanceBGColor"), string("white"));
   // Get the current load cell readings
   bool calibrationComplete = false;
   bool fBalGoodComplete = false;
   bool rBalGoodComplete = false;
   bool fBalBadComplete = false;
   bool rBalBadComplete = false;
   UINT16 samplesAboveLimit = 0;
   bool allForcesAboveLimit = true;
   do
   {
      ReadCurrentLoadCellValues(brakeData, LFWHEEL);
      // Handle the display
      for (UINT8 roller = LFWHEEL; roller <= RRWHEEL; roller++)
      {   // Display the current values
         SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
         if (allForcesAboveLimit)
         {
            float requiredForce = ((roller > RFWHEEL) ?
                                   GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) :
                                   GetVehicleParameter("RequiredBrakeForceFront", (float)0.0));
            allForcesAboveLimit = brakeData[roller].currentForce >= requiredForce;
         }
      }
      samplesAboveLimit = allForcesAboveLimit ? samplesAboveLimit + 1 : 0;
      Log(LOG_DEV_DATA, "Determining if analysis can start - samples above limit: %d, need %d",
          samplesAboveLimit, GetParameterInt("SamplesAboveLimitToStartAverage"));
      // Display the side to side diff
      float fBal = abs(brakeData[LFWHEEL].currentForce - brakeData[RFWHEEL].currentForce);
      float rBal = abs(brakeData[LRWHEEL].currentForce - brakeData[RRWHEEL].currentForce);
      SystemWrite(GetDataTag("FrontBalanceValue"), fBal);
      SystemWrite(GetDataTag("RearBalanceValue"), rBal);
      // Figure out which steps are complete
      if (samplesAboveLimit >= GetParameterInt("SamplesAboveLimitToStartAverage"))
      {
         for (UINT8 index = LFWHEEL; index <= RRWHEEL; index++)
         {
            brakeData[index].forceSamples.push_back(brakeData[index].currentForce);
            if (brakeData[index].forceSamples.size() > GetVehicleParameter("MaxBrakeSamples", (int)0))
            {
               brakeData[index].forceSamples.pop_front();
            }
         }
      }
      else
      {
         for (UINT8 index = LFWHEEL; index <= RRWHEEL; index++)
         {
            brakeData[index].forceSamples.clear();
         }
      }
      if (!brakeData[LFWHEEL].measurementComplete)
      {
         brakeData[LFWHEEL].measurementComplete = IsMeasurementComplete(brakeData[LFWHEEL].forceSamples,
                                                                        GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                                        GetVehicleParameter("RequiredBrakeForceFront", (float)0.0));
         Log(LOG_DEV_DATA, "LF calibration complete: %s", brakeData[LFWHEEL].measurementComplete ? "True" : "False");
      }
      if (!brakeData[RFWHEEL].measurementComplete)
      {
         brakeData[RFWHEEL].measurementComplete = IsMeasurementComplete(brakeData[RFWHEEL].forceSamples,
                                                                        GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                                        GetVehicleParameter("RequiredBrakeForceFront", (float)0.0));
         Log(LOG_DEV_DATA, "RF calibration complete: %s", brakeData[RFWHEEL].measurementComplete ? "True" : "False");
      }
      if (!brakeData[LRWHEEL].measurementComplete)
      {
         brakeData[LRWHEEL].measurementComplete = IsMeasurementComplete(brakeData[LRWHEEL].forceSamples,
                                                                        GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                                        GetVehicleParameter("RequiredBrakeForceRear", (float)0.0));
         Log(LOG_DEV_DATA, "LR calibration complete: %s", brakeData[LRWHEEL].measurementComplete ? "True" : "False");
      }
      if (!brakeData[RRWHEEL].measurementComplete)
      {
         brakeData[RRWHEEL].measurementComplete = IsMeasurementComplete(brakeData[RRWHEEL].forceSamples,
                                                                        GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                                        GetVehicleParameter("RequiredBrakeForceRear", (float)0.0));
         Log(LOG_DEV_DATA, "RR calibration complete: %s", brakeData[RRWHEEL].measurementComplete ? "True" : "False");
      }
      if (!fBalGoodComplete)
      {
         fBalGoodComplete = (brakeData[LFWHEEL].measurementComplete && brakeData[RFWHEEL].measurementComplete &&
                             fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0));
         Log(LOG_DEV_DATA, "Front diff good complete - %s", fBalGoodComplete ? "True" : "False");
      }
      if (!fBalBadComplete)
      {
         fBalBadComplete = (fBalGoodComplete && (fBal >= GetVehicleParameter("MaxFrontDifference", (float)0.0)));
         Log(LOG_DEV_DATA, "Front diff bad complete", fBalBadComplete ? "True" : "False");
      }
      if (!rBalGoodComplete)
      {
         rBalGoodComplete = (brakeData[LRWHEEL].measurementComplete && brakeData[RRWHEEL].measurementComplete &&
                             rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0));
         Log(LOG_DEV_DATA, "Rear diff good complete - %s", rBalGoodComplete ? "True" : "False");
      }
      if (!rBalBadComplete)
      {
         rBalBadComplete = (rBalGoodComplete && (rBal >= GetVehicleParameter("MaxRearDifference", (float)0.0)));
         Log(LOG_DEV_DATA, "Rear diff bad complete - %s", rBalBadComplete ? "True" : "False");
      }
      calibrationComplete = fBalBadComplete && rBalBadComplete;
      // Set the background colors
      Log(LOG_DEV_DATA, "LF: %.2f    RF: %.2f     Diff: %.2f",
          brakeData[LFWHEEL].currentForce, brakeData[RFWHEEL].currentForce, fBal);
      Log(LOG_DEV_DATA, "LR: %.2f    RR: %.2f     Diff: %.2f",
          brakeData[LRWHEEL].currentForce, brakeData[RRWHEEL].currentForce, rBal);
      SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"),
                  brakeData[LFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"),
                  brakeData[RFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"),
                  brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"),
                  brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("FrontBalanceBGColor"),
                  fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("RearBalanceBGColor"),
                  rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0) ? string("green") : string("red"));
      // Wait between updates
      BposSleep(GetTestStepInfoInt("ScanDelay"));
      // Keep looping until complete
   }
   while (/*TimeRemaining() && */!calibrationComplete && (BEP_STATUS_SUCCESS == StatusCheck()));
   // Log the exit and return Pass
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::MaxBrakeForceCalibration() - Enter");
   return testPass;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::ParkBrakeForceVerification(void)
{   // Log the entry and make sure we need to perform this step
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::ParkBrakeForceVerification() - Enter");
   string testResult(BEP_TESTING_STATUS);
   if (!ShortCircuitTestStep())
   {   // Prompt operator to engage the parking brake
      DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
      SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"), GetParameter("TestInProgressColor"));
      // Engage the high torque motors
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
      StatusSleep(GetParameterInt("ApplyParkBrakeWaitTime"));
      DisplayPrompt(GetPromptBox("ApplyParkBrake"), GetPrompt("ApplyParkBrake"), GetPromptPriority("ApplyParkBrake"));
      // Wait for minimum force on both rear wheels or timeout
      MaxBrakeData brakeData[GetRollerCount()];
      for (INT16 roller = LRWHEEL; roller <= RRWHEEL; roller++)
      {
         brakeData[roller].currentForce = 0.0;
         brakeData[roller].maxForce     = 0.0;
         brakeData[roller].measurementComplete = false;
      }
      brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
      brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
      brakeData[LRWHEEL].displayTag = GetDataTag("LRParkBrakeForceTag");
      brakeData[RRWHEEL].displayTag = GetDataTag("RRParkBrakeForceTag");
      // Monitor max brake force on rear wheels
      bool measurementComplete = MonitorBrakeForces(LRWHEEL, RRWHEEL,
                                                    GetVehicleParameter("RequiredParkBrakeForce", (float)0.0),
                                                    GetVehicleParameter("RequiredParkBrakeForce", (float)0.0),
                                                    brakeData, GetVehicleParameter("MaxBrakeSamples", (int)0));
      // Command the high torque motors to disengage and remove prompts
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
      RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
      RemovePrompt(GetPromptBox("ApplyParkBrake"), GetPrompt("ApplyParkBrake"), GetPromptPriority("ApplyParkBrake"));
      // Display the results on the screen
      float lrMaxForce = 0.0;
      float rrMaxForce = 0.0;
      if (brakeData[LRWHEEL].forceSamples.size() > 0)
      {
         lrMaxForce = accumulate(brakeData[LRWHEEL].forceSamples.begin(),
                                 brakeData[LRWHEEL].forceSamples.end(), 0.0) / brakeData[LRWHEEL].forceSamples.size();
         Log(LOG_DEV_DATA, "LR Park Brake Force: %.2f, samples: %d", lrMaxForce, brakeData[LRWHEEL].forceSamples.size());
      }
      else
      {
         lrMaxForce = 0.0;
         Log(LOG_ERRORS, "LR Park Brake Force not measured, no samples collected");
      }
      if (brakeData[RRWHEEL].forceSamples.size() > 0)
      {
         rrMaxForce = accumulate(brakeData[RRWHEEL].forceSamples.begin(),
                                 brakeData[RRWHEEL].forceSamples.end(), 0.0) / brakeData[RRWHEEL].forceSamples.size();
         Log(LOG_DEV_DATA, "RR Park Brake Force: %.2f, samples: %d", rrMaxForce, brakeData[RRWHEEL].forceSamples.size());
      }
      else
      {
         rrMaxForce = 0.0;
         Log(LOG_ERRORS, "RR Park Brake Force not measured, no samples collected");
      }
      SystemWrite(brakeData[LRWHEEL].displayTag, lrMaxForce);
      SystemWrite(brakeData[RRWHEEL].displayTag, rrMaxForce);
      SystemWrite(GetDataTag("LRParkBrakeForceColorTag"),
                  lrMaxForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("RRParkBrakeForceColorTag"),
                  rrMaxForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      float totalForce = lrMaxForce + rrMaxForce;
      SystemWrite(GetDataTag("ParkBrakeForceTag"), totalForce);
      SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"),
                  totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      // Determine the test results
      if (BEP_STATUS_SUCCESS != StatusCheck())
      {   // Bad system status during the test
         Log(LOG_ERRORS, "Bad system status during park brake force testing: %s",
             ConvertStatusToResponse(StatusCheck()).c_str());
         testResult = testFail;
      }
      else if (!TimeRemaining())
      {   // Timeout during max brake force testing
         Log(LOG_ERRORS, "Timeout waiting for minimum required park brake force");
         testResult = testTimeout;
      }
      else if (measurementComplete)
      {   // All brake forces are good
         Log(LOG_DEV_DATA, "Rear wheels met minimum park brake force requirements");
         testResult = testPass;
      }
      else
      {
         testResult = testFail;
      }
      // Report the results
      char buff[32];
      SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                               "LRParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", lrMaxForce), unitsKGF,
                               "RRParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rrMaxForce), unitsKGF,
                               "TotalParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", totalForce), unitsKGF);
   }
   else
   {   // Need to skip this test step
      Log(LOG_FN_ENTRY, "Skipping ToyotaBrakeTC::ParkBrakeForceVerification()");
      testResult = testSkip;
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::ParkBrakeForceVerification() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::ParkBrakeForceCalibration(void)
{
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::ParkBrakeForceCalibration() - Enter");
   // Set up the parameters
   MaxBrakeData brakeData[GetRollerCount()];
   for (INT16 roller = LRWHEEL; roller <= RRWHEEL; roller++)
   {
      brakeData[roller].currentForce = 0.0;
      brakeData[roller].maxForce     = 0.0;
      brakeData[roller].measurementComplete = false;
   }
   brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
   brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
   brakeData[LRWHEEL].displayTag = GetDataTag("LRParkBrakeForceTag");
   brakeData[RRWHEEL].displayTag = GetDataTag("RRParkBrakeForceTag");
   // Wait for the rear load cells to read 0
   SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), string("white"));
   SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"), string("white"));
   do
   {
      ReadCurrentLoadCellValues(brakeData, LRWHEEL);
      BposSleep(500);
   }
   while (((brakeData[LRWHEEL].currentForce > 1.0) || (brakeData[RRWHEEL].currentForce > 1.0)) &&
          (BEP_STATUS_SUCCESS == StatusCheck()));
   SetStartTime();
   // Wait for the calibration to be complete
   bool calibrationComplete = false;
   UINT16 samplesAboveLimit = 0;
   do
   {   // Get the current load cell readings
      ReadCurrentLoadCellValues(brakeData, LRWHEEL);
      // Display the values
      SystemWrite(brakeData[LRWHEEL].displayTag, brakeData[LRWHEEL].currentForce);
      SystemWrite(brakeData[RRWHEEL].displayTag, brakeData[RRWHEEL].currentForce);
      float totalForce = brakeData[LRWHEEL].currentForce + brakeData[RRWHEEL].currentForce;
      SystemWrite(GetDataTag("ParkBrakeForceTag"), totalForce);
      // Determine colors
      SystemWrite(GetDataTag("LRParkBrakeForceColorTag"),
                  brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("RRParkBrakeForceColorTag"),
                  brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"),
                  totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", (float)0.0) ? string("green") : string("red"));
      // Check if we are done
      if ((brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0)) &&
          (brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", (float)0.0)))
      {
         samplesAboveLimit++;
      }
      else
      {
         samplesAboveLimit = 0;
      }
      if (samplesAboveLimit >= GetParameterInt("SamplesAboveLimitToStartAverage"))
      {
         for (UINT8 index = LRWHEEL; index <= RRWHEEL; index++)
         {
            brakeData[index].forceSamples.push_back(brakeData[index].currentForce);
            if (brakeData[index].forceSamples.size() > GetVehicleParameter("MaxBrakeSamples", (int)0))
            {
               brakeData[index].forceSamples.pop_front();
            }
         }
      }
      else
      {
         for (UINT8 index = LRWHEEL; index <= RRWHEEL; index++)
         {
            brakeData[index].forceSamples.clear();
         }
      }
      calibrationComplete = (IsMeasurementComplete(brakeData[LRWHEEL].forceSamples,
                                                   GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                   GetVehicleParameter("RequiredParkBrakeForce", (float)0.0)) &&
                             IsMeasurementComplete(brakeData[RRWHEEL].forceSamples,
                                                   GetVehicleParameter("MaxBrakeSamples", (int)0),
                                                   GetVehicleParameter("RequiredParkBrakeForce", (float)0.0)) &&
                             (totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", (float)0.0)));
      // Wait a bit before the next run
      BposSleep(GetTestStepInfoInt("ScanDelay"));
   }
   while (/*TimeRemaining() && */!calibrationComplete && (BEP_STATUS_SUCCESS == StatusCheck()));
   // Log the exit and return pass
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::ParkBrakeForceCalibration() - Exit");
   return testPass;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::PreShiftCheck(void)
{   // Log the entry and enable the high torque motors
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::PreShiftCheck() - Enter");
   SystemWrite(GetDataTag("EngageHighTorqueMotor"), true);
   SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
   // Make sure both front and rear rollers spin
   bool checkoutComplete = false;
   WHEELINFO rollerSpeed;
   MaxBrakeData brakeData[GetRollerCount()];
   brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
   brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
   brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
   brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
   brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
   brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
   brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
   brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
   SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
   SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
   SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
   SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
   do
   {   // Get the current roller speed
      GetWheelSpeeds(rollerSpeed);
      checkoutComplete = (rollerSpeed.lfWheel > 1.0) && (rollerSpeed.lrWheel > 1.0);
      BposSleep(250);
   }
   while (!checkoutComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
   // Let the rollers run until we timeout
   Log(LOG_DEV_DATA, "Collecting %d samples for drag testing", GetParameterInt("PreShiftDragSamples"));
   while (TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
   {
      ReadCurrentLoadCellValues(brakeData, LFWHEEL);
      for (int wheel = LFWHEEL; wheel <= RRWHEEL; wheel++)
      {
         brakeData[wheel].forceSamples.push_back(brakeData[wheel].currentForce);
         if (brakeData[wheel].forceSamples.size() > GetVehicleParameter("PreShiftDragSamples", 0))
         {
            brakeData[wheel].forceSamples.pop_front();
         }
      }
      BposSleep(GetParameterInt("LoadCellReadingDelay"));
   }
   // Turn off the high torque motors
   SystemWrite(GetDataTag("EngageHighTorqueMotor"), false);
   SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
   // Display the averages
   float lfDrag = accumulate(brakeData[LFWHEEL].forceSamples.begin(), brakeData[LFWHEEL].forceSamples.end(), 0.0) / brakeData[LFWHEEL].forceSamples.size();
   float rfDrag = accumulate(brakeData[RFWHEEL].forceSamples.begin(), brakeData[RFWHEEL].forceSamples.end(), 0.0) / brakeData[RFWHEEL].forceSamples.size();
   float lrDrag = accumulate(brakeData[LRWHEEL].forceSamples.begin(), brakeData[LRWHEEL].forceSamples.end(), 0.0) / brakeData[LRWHEEL].forceSamples.size();
   float rrDrag = accumulate(brakeData[RRWHEEL].forceSamples.begin(), brakeData[RRWHEEL].forceSamples.end(), 0.0) / brakeData[RRWHEEL].forceSamples.size();
   Log(LOG_DEV_DATA, "Collected %d samples - Drag Forces - LF: %.2f, RF: %.2f, LR: %.2f, RR: %.2f",
       brakeData[LFWHEEL].forceSamples.size(), lfDrag, rfDrag, lrDrag, rrDrag);
   SystemWrite(brakeData[LFWHEEL].displayTag, lfDrag);
   SystemWrite(brakeData[RFWHEEL].displayTag, rfDrag);
   SystemWrite(brakeData[LRWHEEL].displayTag, lrDrag);
   SystemWrite(brakeData[RRWHEEL].displayTag, rrDrag);
   SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"),
               lfDrag <= GetVehicleParameter("MaxDragLimit", (float)0.0) ? string("green") : string("red"));
   SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"),
               rfDrag <= GetVehicleParameter("MaxDragLimit", (float)0.0) ? string("green") : string("red"));
   SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"),
               lrDrag <= GetVehicleParameter("MaxDragLimit", (float)0.0) ? string("green") : string("red"));
   SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"),
               rrDrag <= GetVehicleParameter("MaxDragLimit", (float)0.0) ? string("green") : string("red"));
   TestResultDetails testData;
   string testResult = checkoutComplete ? testPass : testFail;
   char buff[80];
   testData.AddDetail("LFDragForce", CreateMessage(buff, sizeof(buff), "%.2f", lfDrag), unitsKGF);
   testData.AddDetail("RFDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rfDrag), unitsKGF);
   testData.AddDetail("LRDragForce", CreateMessage(buff, sizeof(buff), "%.2f", lrDrag), unitsKGF);
   testData.AddDetail("RRDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rrDrag), unitsKGF);
   SendTestResultWithDetail(testResult, testData, GetTestStepInfo("Description"), "0000");
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::PreShiftCheck() - Enter");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::TractionControlCheck(void)
{   // Log the entry and determine if the test should be performed
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::TractionControlCheck() - Enter");
   string testResult(BEP_TESTING_RESPONSE);
   bool tractionEquipped = !SystemRead(GetDataTag("TractionControlFromPlc")).compare("1");
   if (!ShortCircuitTestStep() && tractionEquipped)
   {   // Make sure the vehicle is at zero speed
      if (CheckZeroSpeed())
      {   // Reset the start time
         SetStartTime();
         // Prompt the operator to enable the traction control and accelerate to the target speed
         DisplayPrompt(GetPromptBox("EnableTractionControl"), GetPrompt("EnableTractionControl"),
                       GetPromptPriority("EnableTractionControl"));
         float maxSpeed = GetVehicleParameter("TractionControlSpeed", (float)0.0) * 0.6213712;
         char buff[32];
         string speedRange(CreateMessage(buff, sizeof(buff), "%.2f %.2f", maxSpeed, maxSpeed + 5.0));
         testResult = AccelerateToTestSpeed(maxSpeed, speedRange, GetTestStepInfoInt("ScanDelay"), false);
         // Determine the result - Timeout means the traction control prevented us from reaching the target speed
         testResult = !testResult.compare(BEP_TIMEOUT_RESPONSE) ? testPass : testFail;
         SystemWrite(GetDataTag("TractionControlBgColor"), !testResult.compare(testPass) ? "green" : "red");
         Log(LOG_ERRORS, "Traction control result: %s", testResult.c_str());
         RemovePrompt(GetPromptBox("EnableTractionControl"), GetPrompt("EnableTractionControl"),
                      GetPromptPriority("EnableTractionControl"));
      }
      else
      {
         Log(LOG_ERRORS, "Timeout waiting for zero speed");
         testResult = testFail;
      }
      // Report the test result
      SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
   }
   else if (!tractionEquipped)
   {
      Log(LOG_DEV_DATA, "Vehicle is not equipped with traction control");
      testResult = testSkip;
   }
   else
   {
      Log(LOG_FN_ENTRY, "Skipping test step TractionControlCheck()");
      testResult = testSkip;
   }
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::TractionControlCheck() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
string ToyotaBrakeTC::DualBrakeForceVerification(void)
{   // Log the entry and see if this step should be performed
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::DualBrakeForceVerification() - Enter V2");
   string testResult(BEP_TESTING_STATUS);
   if (!ShortCircuitTestStep())
   {   // Wait for minimum force on all wheels or timeout
      SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("FrontBalanceBGColor"), GetParameter("TestInProgressColor"));
      SystemWrite(GetDataTag("RearBalanceBGColor"), GetParameter("TestInProgressColor"));
      MaxBrakeData brakeData[GetRollerCount()];
      // Assign the data tags
      for (INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
      {
         brakeData[roller].currentForce = 0.0;
         brakeData[roller].maxForce     = 0.0;
         brakeData[roller].measurementComplete = false;
      }
      brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
      brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
      brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
      brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
      brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
      brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
      brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
      brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
      
      // Command the high torque motors to engage
      SystemWrite(GetDataTag("EngageHighTorqueMotor"), true);
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
      // Wait for the rollers to be rolling
      WHEELINFO rollerSpeed;
      do
      {
         GetWheelSpeeds(rollerSpeed);
         BposSleep(GetParameterInt("ApplyBrakePedalWaitTime"));
      }
      while ((rollerSpeed.lfWheel < 1.0) && (BEP_STATUS_SUCCESS == StatusCheck()));
      // Instruct operator to apply brake pedal and wait for a bit
      DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
      bool frontMeasurementComplete = false;
      float fBal = GetVehicleParameter("MaxFrontDifference", (float)0.0) + 20.0;
      float lfMaxForce = 0.0;
      float rfMaxForce = 0.0;
      do
      {   // Make sure the max values are reset
         brakeData[LFWHEEL].maxForce = 0.0;
         brakeData[LFWHEEL].measurementComplete = false;
         brakeData[RFWHEEL].maxForce = 0.0;
         brakeData[RFWHEEL].measurementComplete = false;
         BposSleep(GetParameterInt("FrontMaxBrakeStablizationTime"));
         frontMeasurementComplete = MonitorBrakeForces(LFWHEEL, RFWHEEL,
                                                       GetVehicleParameter("RequiredBrakeForceFront", (float)0.0),
                                                       GetVehicleParameter("RequiredBrakeForceRear", (float)0.0),
                                                       brakeData, GetVehicleParameter("MaxBrakeSamples", (int)0.0));
         // Set the result colors on the screen
         if (brakeData[LFWHEEL].forceSamples.size() > 0)
         {
            lfMaxForce = accumulate(brakeData[LFWHEEL].forceSamples.begin(),
                                    brakeData[LFWHEEL].forceSamples.end(), 0.0) / brakeData[LFWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "LF Max Force: %.2f, samples: %d", lfMaxForce, brakeData[LFWHEEL].forceSamples.size());
         }
         else
         {
            lfMaxForce = 0.0;
            Log(LOG_ERRORS, "LF MAX Force not measured, no samples collected");
         }
         if (brakeData[RFWHEEL].forceSamples.size() > 0)
         {
            rfMaxForce = accumulate(brakeData[RFWHEEL].forceSamples.begin(),
                                    brakeData[RFWHEEL].forceSamples.end(), 0.0) / brakeData[RFWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "RF Max Force: %.2f, samples: %d", rfMaxForce, brakeData[RFWHEEL].forceSamples.size());
         }
         else
         {
            rfMaxForce = 0.0;
            Log(LOG_ERRORS, "RF MAX Force not measured, no samples collected");
         }
         BposSleep(GetParameterInt("LoadCellReadingDelay"));
         SystemWrite(brakeData[LFWHEEL].displayTag, lfMaxForce);
         SystemWrite(brakeData[RFWHEEL].displayTag, rfMaxForce);
         SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"),
                     lfMaxForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
         SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"),
                     rfMaxForce >= GetVehicleParameter("RequiredBrakeForceFront", (float)0.0) ? string("green") : string("red"));
         // Set the brake balance for each axle
         fBal = abs(lfMaxForce - rfMaxForce);
      }
      while ((!frontMeasurementComplete || (fBal > GetVehicleParameter("MaxFrontDifference", (float)0.0))) && TimeRemaining() &&
             (BEP_STATUS_SUCCESS == StatusCheck()));
      SystemWrite(GetDataTag("FrontBalanceValue"), fBal);
      SystemWrite(GetDataTag("FrontBalanceBGColor"),
                  fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0) ? string("green") : string("red"));
      // Log the test result data
      Log(LOG_DEV_DATA, "LF Max Force: %.2f    RF Max Force: %.2f     Balance: %.2f",
          lfMaxForce, rfMaxForce, fBal);


      bool rearMeasurementComplete = false;
      float rBal = GetVehicleParameter("MaxRearDifference", (float)0.0) + 20.0;
      float lrMaxForce = 0.0;
      float rrMaxForce = 0.0;
      do
      {   // Make sure the max values are reset
         Log(LOG_DEV_DATA, "Setting rear max brake values to incomplete state");
         brakeData[LRWHEEL].maxForce = 0.0;
         brakeData[LRWHEEL].measurementComplete = false;
         brakeData[RRWHEEL].maxForce = 0.0;
         brakeData[RRWHEEL].measurementComplete = false;
         //BposSleep(GetParameterInt("RearMaxBrakeStablizationTime"));
         rearMeasurementComplete = MonitorBrakeForces(LRWHEEL, RRWHEEL,
                                                      GetVehicleParameter("RequiredBrakeForceFront", (float)0.0),
                                                      GetVehicleParameter("RequiredBrakeForceRear", (float)0.0),
                                                      brakeData, GetVehicleParameter("MaxBrakeSamples", (int)0));
         // Set the result colors on the screen
         if (brakeData[LRWHEEL].forceSamples.size() > 0)
         {
            lrMaxForce = accumulate(brakeData[LRWHEEL].forceSamples.begin(),
                                    brakeData[LRWHEEL].forceSamples.end(), 0.0) / brakeData[LRWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "LR Max Force: %.2f, samples: %d", lrMaxForce, brakeData[LRWHEEL].forceSamples.size());
         }
         else
         {
            lrMaxForce = 0.0;
            Log(LOG_ERRORS, "LR MAX Force not measured, no samples collected");
         }
         if (brakeData[RRWHEEL].forceSamples.size() > 0)
         {
            rrMaxForce = accumulate(brakeData[RRWHEEL].forceSamples.begin(),
                                    brakeData[RRWHEEL].forceSamples.end(), 0.0) / brakeData[RRWHEEL].forceSamples.size();
            Log(LOG_DEV_DATA, "RR Max Force: %.2f, samples: %d", rrMaxForce, brakeData[RRWHEEL].forceSamples.size());
         }
         else
         {
            rrMaxForce = 0.0;
            Log(LOG_ERRORS, "RR MAX Force not measured, no samples collected");
         }
         BposSleep(GetParameterInt("LoadCellReadingDelay"));
         SystemWrite(brakeData[LRWHEEL].displayTag, lrMaxForce);
         SystemWrite(brakeData[RRWHEEL].displayTag, rrMaxForce);
         SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"),
                     lrMaxForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
         SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"),
                     rrMaxForce >= GetVehicleParameter("RequiredBrakeForceRear", (float)0.0) ? string("green") : string("red"));
         // Set the brake balance for each axle
         rBal = abs(lrMaxForce - rrMaxForce);
         if (rBal > GetVehicleParameter("MaxRearDifference", (float)0.0))
         {
            Log(LOG_DEV_DATA, "Rear brake difference [%.2f] is out of spec!", rBal);
         }
      }
      while ((!rearMeasurementComplete || (rBal > GetVehicleParameter("MaxRearDifference", (float)0.0))) && TimeRemaining() &&
             (BEP_STATUS_SUCCESS == StatusCheck()));
      // Command the high torque motors to disengage
      SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
      SystemWrite(GetDataTag("EngageHighTorqueMotor"), false);
      SystemWrite(GetDataTag("RearBalanceValue"), rBal);
      SystemWrite(GetDataTag("RearBalanceBGColor"),
                  rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0) ? string("green") : string("red"));
      // Log the test result data
      Log(LOG_DEV_DATA, "LR Max Force: %.2f    RR Max Force: %.2f     Balance: %.2f",
          lrMaxForce, rrMaxForce, rBal);

      RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));

      // Determine the results
      if (BEP_STATUS_SUCCESS != StatusCheck())
      {   // Bad system status during the test
         Log(LOG_ERRORS, "Bad system status during max brake force testing: %s",
             ConvertStatusToResponse(StatusCheck()).c_str());
         testResult = testFail;
      }
      else if (!TimeRemaining())
      {   // Timeout during max brake force testing
         Log(LOG_ERRORS, "Timeout waiting for minimum required brake force");
         testResult = testTimeout;
      }
      else if (frontMeasurementComplete && rearMeasurementComplete &&
               (fBal <= GetVehicleParameter("MaxFrontDifference", (float)0.0)) &&
               (rBal <= GetVehicleParameter("MaxRearDifference", (float)0.0)))
      {   // All brake forces are good
         Log(LOG_DEV_DATA, "All wheels met minimum brake force requirements");
         testResult = testPass;
      }
      else
      {
         testResult = testFail;
      }
      // Report the results
      char buff[32];
      TestResultDetails testData;
      testData.AddDetail("LFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", lfMaxForce), unitsKGF);
      testData.AddDetail("RFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rfMaxForce), unitsKGF);
      testData.AddDetail("LRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", lrMaxForce), unitsKGF);
      testData.AddDetail("RRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rrMaxForce), unitsKGF);
      testData.AddDetail("FrontBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", fBal), unitsKGF);
      testData.AddDetail("RearBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", rBal), unitsKGF);
      SendTestResultWithDetail(testResult, testData, GetTestStepInfo("Description"), "0000");
   }
   else
   {
      Log(LOG_FN_ENTRY, "Skipping ToyotaBrakeTC::DualBrakeForceVerification()");
      testResult = testSkip;
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaBrakeTC::DualBrakeForceVerification() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
bool ToyotaBrakeTC::MonitorBrakeForces(INT16 startingRoller, INT16 stoppingRoller,
                                       float minimumRequiredForceFront, float minimumRequiredForceRear,
                                       MaxBrakeData *brakeData, UINT16 samplesToAverage)
{
   bool measurementComplete = false;
   UINT16 samplesAboveLimit = GetParameterInt("SamplesAboveLimitToStartAverage");
   UINT16 currentSamplesAboveLimit = 0;
   float requiredForce = (stoppingRoller < LRWHEEL) ? minimumRequiredForceFront : minimumRequiredForceRear;
   do
   {   // Get the current force for each wheel
      ReadCurrentLoadCellValues(brakeData, startingRoller);
      // Check if all monitored rollers are above the limit
      bool allForcesAboveLimit = true;
      for (int roller = startingRoller; (roller <= stoppingRoller) && allForcesAboveLimit; roller++)
      {
         allForcesAboveLimit = (brakeData[roller].currentForce >= ((roller < LRWHEEL) ? minimumRequiredForceFront : minimumRequiredForceRear));
      }
      // update the number of samples above the limit
      currentSamplesAboveLimit = allForcesAboveLimit ? currentSamplesAboveLimit + 1 : 0;
      // Analyze the data
      for (int roller = startingRoller; roller <= stoppingRoller; roller++)
      {   // Display the current force
         SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
         if (allForcesAboveLimit)
         {   // Check if this is a max value
            if (brakeData[roller].currentForce > ((roller < LRWHEEL) ? minimumRequiredForceFront : minimumRequiredForceRear))
            {
               if (currentSamplesAboveLimit >= samplesAboveLimit)
               {   // We have enough samples over the threshold, start averaging
                  brakeData[roller].forceSamples.push_back(brakeData[roller].currentForce);
                  if (brakeData[roller].currentForce > brakeData[roller].maxForce)
                  {
                     brakeData[roller].maxForce = brakeData[roller].currentForce;
                     Log(LOG_DEV_DATA, "New max force for %s: %.2f", rollerName[roller].c_str(), brakeData[roller].maxForce);
                  }
                  // Check if measurement is complete on this wheel
                  if (brakeData[roller].forceSamples.size() > samplesToAverage)
                  {   // Remove old samples to keep a moving sample window
                     brakeData[roller].forceSamples.pop_front();
                     brakeData[roller].measurementComplete = true;
                     Log(LOG_DEV_DATA, "Measurement complete for %s - reached min samples: %d",
                         rollerName[roller].c_str(), samplesToAverage);
                  }
               }
            }
         }
         else
         {   // Have not reached the threshold yet, do not start collecting samples
            brakeData[roller].forceSamples.clear();
         }
      }
      // Check if all wheels have been completed
      measurementComplete = true;
      for (int roller = startingRoller; (roller <= stoppingRoller) && measurementComplete; roller++)
      {
         measurementComplete = brakeData[roller].measurementComplete;
      }
      Log(LOG_DEV_DATA, "All measurements complete: %s", measurementComplete ? "True" : "False");
      // Wait a bit before the next measurement
      if (!measurementComplete)  BposSleep(GetParameterInt("LoadCellReadingDelay"));
      // Keep checking until all min required forces are satisfied
   }
   while ((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && !measurementComplete);
   // Return the completion flag
   return measurementComplete;
}

//-------------------------------------------------------------------------------------------------
void ToyotaBrakeTC::ReadCurrentLoadCellValues(MaxBrakeData *brakeData, UINT16 startingRoller)
{
   for (UINT8 roller = startingRoller; roller <= RRWHEEL; roller++)
   {
      brakeData[roller].currentForce = (atof(SystemRead(brakeData[roller].currentForceTag).c_str()));
   }
}

//-------------------------------------------------------------------------------------------------
bool ToyotaBrakeTC::IsMeasurementComplete(list<float>& samples,
                                          const int& requiredBrakeSamples,
                                          const float& minRequiredForce)
{
   bool complete = false;
   float average = 0.0;
   if (samples.size() >= requiredBrakeSamples)
   {
      average = accumulate(samples.begin(), samples.end(), 0.0) / samples.size();
      complete = average >= minRequiredForce;
   }
   Log(LOG_DEV_DATA, "IsMeasurementComplete() - %s - average: %.4f, min force: %.4f, sample count: %d, req samples: %d",
       complete ? "true" : "false", average, minRequiredForce, samples.size(), requiredBrakeSamples);
   return complete;
}

