//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "ToyotaMachineTC.h"

//-------------------------------------------------------------------------------------------------
ToyotaMachineTC::ToyotaMachineTC() : MachineTC()   // Nothing special to do here
{
}

//-------------------------------------------------------------------------------------------------
ToyotaMachineTC::~ToyotaMachineTC()   // Nothing special to do here
{
}

//-------------------------------------------------------------------------------------------------
const string ToyotaMachineTC::CommandTestStep(const string& value)
{
   string testResult(BEP_TESTING_RESPONSE);
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::CommandTestStep(value: %s) - Enter", value.c_str());
   if (BEP_STATUS_SUCCESS == StatusCheck())
   {
      try
      {
         if (!GetTestStepName().compare("PacesetterTest")) testResult = TestStepPacesetter();
         else testResult = MachineTC::CommandTestStep(value);
      }
      catch (BepException& excpt)
      {
         Log(LOG_ERRORS, "BepException sequencing test step %s - %s", GetTestStepName().c_str(), excpt.GetReason());
         testResult = testFail;
      }
   }
   else
   {
      Log(LOG_ERRORS, "Bad system status, not starting test step %s", GetTestStepName().c_str());
      testResult = testSkip;
   }
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::CommandTestStep(value: %s) - Exit", value.c_str());
   return testResult;
}

//-------------------------------------------------------------------------------------------------
const string ToyotaMachineTC::TestStepSpeedometer(const string& value)
{
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepSpeedometer() - Enter");
   string testDescription(GetTestStepInfo("Description"));
   // Set the result box color to let operator know testing has started
   SystemWrite(GetDataTag("SpeedometerTestSpeedBGColor"), colorYellow);
   // Get the vehicle build data
   bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
   float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
   string speedoType(convertSpeed ? "KPH" : "MPH");
   float minSpeed = GetVehicleParameter("SpeedTargets/" + speedoType + "/SpeedometerSpeedTargetLow", (float)0.0) * speedScaling;
   float maxSpeed = GetVehicleParameter("SpeedTargets/" + speedoType + "/SpeedometerSpeedTargetHi", (float)0.0) * speedScaling;
   char buff[32];
   string dispMaxSpeed(CreateMessage(buff, sizeof(buff), "%.0f", maxSpeed));
   string dispMinSpeed(CreateMessage(buff, sizeof(buff), "%.0f", minSpeed));
   string speedRange(dispMinSpeed + " " + dispMaxSpeed);
   // Wait for the operator to get to the correct speed range - divide by speed scale since the base class functions in mph.
   DisplayPrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
   DisplayPrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"),
                 colorWhite, string(dispMinSpeed + " - " + dispMaxSpeed));
   string testResult = (WaitForTargetSpeed(maxSpeed + 10.0, DOWN, GetTestStepInfoInt("ScanDelay")) ==
                           BEP_STATUS_SUCCESS) ? testPass : testFail;
   SystemWrite(GetDataTag("SpeedTarget"), speedRange);
   // Get the wheel speeds
   float currentSpeed = GetRollSpeed();
   // Make sure operator is in the correct speed range
   if (!testResult.compare(testPass))   // Vehicle is in speed range, prompt operator to flash headlamps
   {
      SetStartTime();
      bool done = false;
      // Wait for the operator to flash high beams
      while (!done && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS))   // Get the current wheel speeds
      {
         currentSpeed = GetRollSpeed();
         // Check if the highbeams have been flashed yet
         if (!ReadSubscribeData(GetDataTag("StartSpeedoTest")).compare("1"))   // Highbeams flashed, update the value on the screen
         {
            SystemWrite(GetDataTag("SpeedometerTestResultSpeed"), currentSpeed);
            // Determine if the test passed or failed
            string resultColor;
            if ((minSpeed <= currentSpeed) && (currentSpeed <= maxSpeed))   // Test has passed
            {
               resultColor = colorGreen;
               testResult = testPass;
               done = true;
            }
            else   // Test failed
            {
               resultColor = colorRed;
               testResult = testFail;
               testDescription = "Speedometer out of range";
               BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Update screen colors
            SystemWrite(GetDataTag("SpeedometerTestResultBox"), resultColor);
            SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), resultColor);
         }
         else   // Need to wait a bit
         {
            BposSleep(GetTestStepInfoInt("ScanDelay"));
         }
      }
      // Check if there was an abnormal exit condition
      if (!done)   // Set test to fail
      {
         SystemWrite(GetDataTag("SpeedometerTestResultBox"), colorRed);
         SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), colorRed);
         testResult = testFail;
         testDescription = "Speedometer test timeout";
         Log(LOG_ERRORS, "Speedometer test incomplete - TimeRemaining: %s, StatusCheck: %s",
             TimeRemaining() ? "True" : "False", ConvertStatusToResponse(StatusCheck()).c_str());
      }
      // Remove prompts
      RemovePrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
      RemovePrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"));
   }
   else   // Error accelerating to speed
   {
      Log(LOG_ERRORS, "Could not achieve speed range, not performing speedometer test");
   }
   // Remove the speed target
   SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
   // Report the result, log the exit and return
   SendTestResultWithDetail(testResult, testDescription, "0000",
                            "RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", GetRollSpeed()),
                            (speedScaling != 1.0) ? unitsKPH : unitsMPH);
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepSpeedometer() - Exit");
   return testResult;
}

//-------------------------------------------------------------------------------------------------
const string ToyotaMachineTC::TestStepReverse(const string& value)   // Log the entry and prompt the operator to accelerate in reverse
{
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepReverse(value: %s) - Enter", value.c_str());
   string testResult(BEP_TESTING_RESPONSE);
   bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
   float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
   float reverseSpeed = GetVehicleParameter("ReverseTestSpeed", (float)0.0) * speedScaling;
   // Wait for the operator to reach zeroish speed
   while ((GetRollSpeed() > (GetParameterFloat("PromptReverseTestSpeed") * speedScaling)) &&
          (BEP_STATUS_SUCCESS == StatusCheck()))   // Wait just a bit so we do not hog the CPU - also reset the start time since there is no way to tell
   {
      // when the operator will stop
      BposSleep(1000);
      SetStartTime();
   }
   // Display the target for the operator
   char buff[32];
   string reverseTarget(CreateMessage(buff, sizeof(buff), "%.2f", reverseSpeed));
   // Prompt the operator to accelerate to the target
   DisplayPrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
   DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"),
                 string("white"), reverseTarget);
   // Wait for the target speed to be reached
   while (TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (GetRollSpeed() > reverseSpeed)) BposSleep(250);
   testResult = (GetRollSpeed() < reverseSpeed) ? testPass : testFail;
   SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
   // Remove the prompts
   RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
   RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
   // Log the exit and return the result
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepReverse(value: %s) - Exit", value.c_str());
   return testResult;
}

//-------------------------------------------------------------------------------------------------
inline float ToyotaMachineTC::GetRollSpeed(void)
{
   return GetParameterBool("SingleEncoder") ? SystemReadFloat(GetDataTag("MaximumRollerSpeedTag")) : GenericTC::GetRollSpeed();
}

//-------------------------------------------------------------------------------------------------
const string ToyotaMachineTC::TestStepAccelerateToSpeed(const string& value)   // Log the entry and prompt the operator
{
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Enter", value.c_str());
   bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
   float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
   char buff[32];
   string dispSpeed(CreateMessage(buff, sizeof(buff), "%.0f", atof(value.c_str()) * speedScaling));
   DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"),
                 string("white"), dispSpeed);
   // Wait for the operator to achieve speed
   float speedTarget = atof(value.c_str()) * speedScaling;
   while ((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && (GetRollSpeed() < speedTarget)) BposSleep(250);
   // Make sure speed was schieved and return the result
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Exit", value.c_str());
   RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
   return (GetRollSpeed() >= speedTarget) ? testPass : testFail;
}

const string ToyotaMachineTC::TestStepPacesetter(void)
{
   Log(LOG_FN_ENTRY, "ToyotaMachineTC::TestStepPacesetter() - Enter");
   string testDescription(GetTestStepInfo("Description"));
   string testResult(testPass);
   XYSeries upperSeries; // vector of coordinate pairs create upper limit slopes
   XYSeries lowerSeries; // same as above but lower limit

   //testResult = WaitToStart();

   SystemWrite(GetDataTag("MotorMode"), BOOST_MODE);

   // Need two series' to represent the lines to determine when vehicles speed falls outside of limits given current time and vehicle speed
   
   float currentSpeed; // speed of vehicle

   // convert GraphUpperSeries and GraphLowerSeries strings to vectors of pairs
   if (BEP_STATUS_SUCCESS == ConvertSeriesToVector(upperSeries, lowerSeries))
   {
      Log(LOG_DEV_DATA, "ok but it actually did it it got a bep status succ, sizes: upper:%d,lower:%d", upperSeries.size(), lowerSeries.size());
      int y1, y2, x1, x2, m, b; // classic formula stuff
      vector<PacesetterEntry> testEntries;
      int timeEntry = 0;
      // now that we have a vector of pairs, we need an additional vector that will represent each
      // speed value per 250ms increment.  this will allow us to know how fast the vehicle should be moving at any point of the test
      // y = mx + b
      Log(LOG_DEV_DATA, "made it here.. with upperSeries size: %d", upperSeries.size());
      for (int i = 1;i < upperSeries.size();i++)
      {
         // so you can't make the struct like this, you need to create the new entries after creating
         // two separate vectors (that since your one caveat is that the last two x values have to be the same, will be the same size)
         // that you will combine into the struct which will give you your final testEntries vector
         // wow you really underestimated this project and shame on you for being so lazy and complacent
         // seriously you could have had this done ages ago and yet you wait until the ACTUAL final moment to complete it
         // I hope this doesn't cause you and everyone else problems
         while (timeEntry != x2)
         {
            PacesetterEntry newEntry;
            y1 = upperSeries[i].second;
            y2 = upperSeries[i-1].second;
            x1 = upperSeries[i].first * 1000;
            x2 = upperSeries[i-1].first * 1000;

            m = (y2-y1)/(x2-x1);
            b = y1;

            // filling high speed value for entry
            newEntry.highSpeed = (m * timeEntry) + b;

            y1 = lowerSeries[i].second;
            y2 = lowerSeries[i-1].second;
            x1 = lowerSeries[i].first * 1000;
            x2 = lowerSeries[i-1].first * 1000;

            m = (y2-y1)/(x2-x1);
            b = y1;

            // filling low speed value for entry 
            newEntry.lowSpeed = (m * timeEntry) + b;
            
            // enter time stamp 
            newEntry.timeStamp = timeEntry;
             
            // push into vector
            testEntries.push_back(newEntry);

            timeEntry += 250;
         }
      }

      // pray
      for (int j = 0; j < testEntries.size();j++)
      {
         Log(LOG_DEV_DATA, "pacesetter entry test: timestamp:%d, high:%f, low:%f",testEntries[j].timeStamp, testEntries[j].highSpeed, testEntries[j].lowSpeed);
      }


   }

   else
   {
      Log(LOG_DEV_DATA, "Error converting series string to vector, must be rectified to use pacesetter.");
      testResult = BEP_STATUS_ERROR;
   }

   return testResult;
}

BEP_STATUS_TYPE ToyotaMachineTC::ConvertSeriesToVector(XYSeries &upper, XYSeries &lower)
{
   // take system graph series values and place them into vector
   string upperSeries = SystemRead(GetDataTag("GraphUpperSeries"));
   string lowerSeries = SystemRead(GetDataTag("GraphLowerSeries"));
   BEP_STATUS_TYPE result = BEP_STATUS_SUCCESS;
   try
   {
      // get any junk out of the coordinate vectors
      upper.clear();
      lower.clear();

      // string magic to fill upper and lower coords
      string x, y;
      string tempCoord; // ?
      size_t pos;

      // this string magic will fill upper vector with values up until the very last set of coordinates
      while ((pos = upperSeries.find(',')) != std::string::npos)
      {
         tempCoord = upperSeries.substr(0, pos);
         x = tempCoord.substr(0, tempCoord.find(' '));
         tempCoord = tempCoord.erase(0, tempCoord.find(' ')+1);
         y = tempCoord;
         upper.push_back(std::make_pair(atoi(x.c_str()), atoi(y.c_str())));
         //Log(LOG_DEV_DATA, "Upper:x=%s,y=%s", x.c_str(), y.c_str());
         upperSeries = upperSeries.erase(0, pos+1);
      }

      // final value in string that hasn't been erased because there are no more ',' values
      tempCoord = upperSeries;
      x = tempCoord.substr(0, tempCoord.find(' '));
      tempCoord = tempCoord.erase(0, tempCoord.find(' ')+1);
      y = tempCoord;
      upper.push_back(std::make_pair(atoi(x.c_str()), atoi(y.c_str())));
      //Log(LOG_DEV_DATA, "Upper final: x=%s,y=%s", x.c_str(), y.c_str());

      // this string magic will fill lower vector with values up until the very last set of coordinates
      while ((pos = lowerSeries.find(',')) != std::string::npos)
      {

         tempCoord = lowerSeries.substr(0, pos);
         x = tempCoord.substr(0, tempCoord.find(' '));
         tempCoord = tempCoord.erase(0, tempCoord.find(' ')+1);
         y = tempCoord;
         lower.push_back(std::make_pair(atoi(x.c_str()), atoi(y.c_str())));
         //Log(LOG_DEV_DATA, "Lower:x=%s,y=%s", x.c_str(), y.c_str());
         lowerSeries = lowerSeries.erase(0, pos+1);
      }

      // final value in string that hasn't been erased because there are no more ',' values
      tempCoord = lowerSeries;
      x = tempCoord.substr(0, tempCoord.find(' '));
      tempCoord = tempCoord.erase(0, tempCoord.find(' ')+1);
      y = tempCoord;
      lower.push_back(std::make_pair(atoi(x.c_str()), atoi(y.c_str())));
      //Log(LOG_DEV_DATA, "Lower final: x=%s,y=%s", x.c_str(), y.c_str());

      for (int i = 0;i<upper.size();i++)
      {
         Log(LOG_DEV_DATA, "Entry upper: %d,%d", upper[i].first, upper[i].second);
         Log(LOG_DEV_DATA, "Entry lower: %d,%d", lower[i].first, lower[i].second);

      }

      Log(LOG_DEV_DATA, "upper size:%d lower size:%d",upper.size(),lower.size());
   }
   catch (...)
   {
      Log(LOG_ERRORS, "Error during coordinate conversion to vector", GetTestStepName().c_str());
      result = BEP_STATUS_FAILURE;
   }
   return result;
}
