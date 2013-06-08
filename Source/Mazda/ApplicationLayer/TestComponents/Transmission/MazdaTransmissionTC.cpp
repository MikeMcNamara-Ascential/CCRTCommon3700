//******************************************************************************
// Description:
//  Manual transmission component class
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "MazdaTransmissionTC.h"

//-------------------------------------------------------------------------------------------------
MazdaTransmissionTC::MazdaTransmissionTC() : GenericTC(),
m_accelTestStartTime( 0)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaTransmissionTC::~MazdaTransmissionTC()
{
    m_driveCurveParametersNodeMap.clear(true);
}

//-------------------------------------------------------------------------------------------------
void MazdaTransmissionTC::Initialize(const string &fileName)
{
    // call the base class to perfrom the initialization
    GenericTC::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void MazdaTransmissionTC::Initialize(const XmlNode *config) 
{
    try
    {
        //Iniitalize base class
        GenericTC::Initialize(config);

        // Call the initialize complete method
        Log(LOG_DEV_DATA, (GetComponentName() + ": calling InitializationComplete()").c_str());
        InitializationComplete();

        // Reset the data tags
        ResetSystemTags();  

        Log(LOG_FN_ENTRY, (GetComponentName() + ": Initialize: EXIT").c_str());
    }
    catch (XmlException &e)
    {
        Log(LOG_ERRORS, "XML Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
    }

    //Get the list of drive curve parameters 
    try
    {
        m_driveCurveParametersNodeMap.DeepCopy(config->getChild("Setup/Parameters/DriveCurveParameters")->getChildren());
        RetrieveDriveCurveParameters();
    }
    catch(BepException &err)
    {   // Error loading Module data items
        Log("XML Exception loading drive curve parameters - %s\n", err.Reason().c_str());
        m_driveCurveParametersNodeMap.clear(true);
    }

}




//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::CommandTestStep(const string &value)
{
    string testResult = testFail;
    const string &step = GetTestStepName();

    // Determine if the next test step can be sequenced
    if (StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {   // Get the next test step to perform
            Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());

            if (step == "AccelerateInReverse")
            {
                testResult = AccelerateInReverse(GetParameterFloat("ReverseSpeedMinimum"), 
                                                 GetParameterFloat("ReverseSpeedMaximum"), 
                                                 GetTestStepInfoInt("ScanDelay"));
            }
            else if ( step == "FollowDriveCurve")        testResult = FollowDriveCurve();
            else if ( step == "BrakeToStop")             testResult = TestStepBrakeToStop(value);
            else if ( step == "FinishUp")                testResult = TestStepFinishUp( value);
            //else if ( step == "CheckShifting")
            //{
            //    testResult = OperatorPassFail(GetDataTag("CheckShiftingPrompt"), GetTestStepInfoInt("Timeout"));
            //    SendSubtestResult("ReverseGearObserved", testResult, "Reverse Gear Observed", "0000");
            //}
            // No defined test step, try the base class
            else      testResult = GenericTC::CommandTestStep(value);
        }
        catch (BepException &err)
        {   // Log the exception and return the error
            Log("CommandTestStep %s BepException: %s\n", step.c_str(), err.what());
            testResult = BEP_ERROR_RESPONSE;
        }
    }
    else
    {   // Status of the system is not right to continue testing
        Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
    }
    // Return the result of the test
    Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::Publish(const XmlNode *node)
{
    const string   &tag = node->getName();
    const string   &value = node->getValue();
    string         result;

    if ( (ZEROSPEED_TAG == tag) && ("0" == value))
    {
        m_accelTestStartTime = time( NULL);
        Log( LOG_DEV_DATA, "Accel Test start time set to %d\n", m_accelTestStartTime);
    }
    result = BepServer::Publish( node);

    return( result);
}

//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::TestStepFinishUp(const string &value)
{
    // send the overall test result
    SendTestResult(testPass, GetTestStepInfo("Description"), "0000");

    SendOverallResult(testPass);
    // always return success
    return(testPass);
}

//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::FollowDriveCurve(void)
{
    string testStatus( testPass);  // set test step status to pass
    int         scanDelay = GetTestStepInfoInt( "ScanDelay");
    int         status = StatusCheck();

    Log(LOG_FN_ENTRY, "Enter MazdaTransmissionTC::FollowDriveCurve()\n");
                             
    if ( scanDelay <= 0) scanDelay = 500;

    UpdatePrompts();

    //Wait for loss of zero speed

    while ( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) && (ReadSubscribeData(ZEROSPEED_TAG) == "0"))
    {
        status = StatusSleep( scanDelay);
        Log( LOG_DETAILED_DATA, "Waiting for acceleration start\n");
    }


    //cycle through drive curve sections
    for (UINT32 ii = 0; (ii < m_driveCurveParameters.size()) && (testStatus == testPass) && !(ReadSubscribeData(ZEROSPEED_TAG) == "0"); ii++)
    {
        testStatus = PromptDriveCurveSection(m_driveCurveParameters[ii],
                                ii == 0 ? 0 : m_driveCurveParameters[ii-1].targetSpeed,
                                ii == 0 ? 0 : m_driveCurveParameters[ii-1].endTime);

    }


    RemovePrompts();
    SystemWrite( "SpeedTarget",  string("0 0"));

    if ( BEP_STATUS_SUCCESS == status)   testStatus = testPass;
    else                                testStatus = testFail;

    SendTestResult( testStatus, GetTestStepInfo( "Description"), "0000");

    Log(LOG_FN_ENTRY, "Exit MazdaTransmissionTC::FollowDriveCurve(), status=%s\n", testStatus.c_str());

    return( testStatus);
}

//-------------------------------------------------------------------------------------------------
void MazdaTransmissionTC::RetrieveDriveCurveParameters(void)
{
   m_driveCurveParameters.clear();
   for(XmlNodeMapItr iter = m_driveCurveParametersNodeMap.begin();
   (iter != m_driveCurveParametersNodeMap.end());
   iter++)
    {
       XmlNodeMap driveCurveSection = iter->second->getChildren();
       DRIVECURVE_PARAMETERS driveCurveParams;

       driveCurveParams.prompt1 = driveCurveSection.getNode("Prompt1")->getValue();
       driveCurveParams.prompt1Value1 = driveCurveSection.getNode("Prompt1")->getAttribute("Value1")->getValue(); 
       driveCurveParams.prompt1Value2 = driveCurveSection.getNode("Prompt1")->getAttribute("Value2")->getValue(); 

       driveCurveParams.prompt2 = driveCurveSection.getNode("Prompt2")->getValue();
       driveCurveParams.prompt2Value1 = driveCurveSection.getNode("Prompt2")->getAttribute("Value1")->getValue(); 
       driveCurveParams.prompt2Value2 = driveCurveSection.getNode("Prompt2")->getAttribute("Value2")->getValue(); 

       driveCurveParams.promptSpeedHigh = driveCurveSection.getNode("PromptSpeedHigh")->getValue();
       driveCurveParams.promptSpeedHighValue1 = driveCurveSection.getNode("PromptSpeedHigh")->getAttribute("Value1")->getValue(); 
       driveCurveParams.promptSpeedHighValue2 = driveCurveSection.getNode("PromptSpeedHigh")->getAttribute("Value2")->getValue(); 

       driveCurveParams.promptSpeedLow = driveCurveSection.getNode("PromptSpeedLow")->getValue();
       driveCurveParams.promptSpeedLowValue1 = driveCurveSection.getNode("PromptSpeedLow")->getAttribute("Value1")->getValue(); 
       driveCurveParams.promptSpeedLowValue2 = driveCurveSection.getNode("PromptSpeedLow")->getAttribute("Value2")->getValue(); 

       driveCurveParams.targetSpeed = ConvertFloatUnitsIfNecessary(driveCurveSection.getNode("TargetSpeed"));
       driveCurveParams.speedTolerance = atof(driveCurveSection.getNode("SpeedTolerance")->getValue().c_str());
       driveCurveParams.endTime = atof(driveCurveSection.getNode("EndTime")->getValue().c_str());

       Log(LOG_FN_ENTRY, 
           "Retrieved Drive curve parameters Prompt1: %s Prompt2: %s PromptSpeedHigh: %s PromptSpeedLow: %s TargetSpeed: %f SpeedTolerance: %f EndTime: %f\n",
           driveCurveParams.prompt1.c_str(),
           driveCurveParams.prompt2.c_str(),
           driveCurveParams.promptSpeedHigh.c_str(),
           driveCurveParams.promptSpeedLow.c_str(),
           driveCurveParams.targetSpeed,
           driveCurveParams.speedTolerance, 
           driveCurveParams.endTime);

      m_driveCurveParameters.push_back(driveCurveParams);
    }
}

//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::PromptDriveCurveSection(DRIVECURVE_PARAMETERS driveCurveParams, float previousTargetSpeed, float previousEndTime)
{
    string testResult = testPass;
    int         status = StatusCheck();
    int scanDelay = GetTestStepInfoInt("ScanDelay");
    bool promptSpeedInRangeDisplayed = true;
    bool promptSpeedLowDisplayed = false;
    bool promptSpeedHighDisplayed = false;
    float slope = 0;
    char        buff[ 256];
    std::string response;
    Log(LOG_FN_ENTRY, "Enter MazdaTransmissionTC::PromptDriveCurveSection\n");

    if ( scanDelay <= 0) scanDelay = 500;

    //display initial prompts and target speed
           string speedRange = "";
    DisplayPrompt(GetPromptBox(driveCurveParams.prompt1), GetPrompt(driveCurveParams.prompt1), GetPromptPriority(driveCurveParams.prompt1),"white");
    DisplayPrompt(GetPromptBox(driveCurveParams.prompt2), GetPrompt(driveCurveParams.prompt2), GetPromptPriority(driveCurveParams.prompt2),"white");

    if (driveCurveParams.targetSpeed != 0)
    {
        sprintf( buff, "%.02f %.02f", driveCurveParams.targetSpeed - 
                 (driveCurveParams.targetSpeed * driveCurveParams.speedTolerance / 100), 
                                    driveCurveParams.targetSpeed - 
                 (driveCurveParams.targetSpeed * driveCurveParams.speedTolerance / 100));
        speedRange = buff;
        SystemWrite( "SpeedTarget", speedRange);
    }
    else
    {
        SystemWrite( "SpeedTarget", string("0 1"));
    }


    // get the current time
    time_t  currentTime = time(NULL);
    //y - b / x  = m
    slope = (driveCurveParams.targetSpeed - previousTargetSpeed) / (driveCurveParams.endTime - previousEndTime);
    Log(LOG_DEV_DATA, "EndTime = %f SpeedTarget = %s, speedRange = %s slope = %f\n", buff, speedRange.c_str(), slope);


    // while time remaining and good status and section time remaining and not at zero speed
    while (TimeRemaining() && (BEP_STATUS_SUCCESS == status) && (difftime( currentTime, m_accelTestStartTime) < driveCurveParams.endTime) &&
           !(ReadSubscribeData(ZEROSPEED_TAG) == "0"))
    {
        float currentSpeed = GetRollSpeed();
        //given current time, determine max and min speeds expected
        float expectedSpeed = slope * currentTime + previousTargetSpeed;
        float minSpeed = expectedSpeed * ((100 - driveCurveParams.speedTolerance) / 100);
        float maxSpeed = expectedSpeed * ((100 + driveCurveParams.speedTolerance) / 100);

        Log(LOG_DEV_DATA, "Current Speed = %f Expected Speed = %f, Min Speed = %f Max Speed = %f\n", 
            currentSpeed, expectedSpeed, minSpeed, maxSpeed);
        //determine if speed is in range
        //if not, display appropriate prompt if not already displayed
        if (currentSpeed < minSpeed)
        {
            if(!promptSpeedLowDisplayed)
            {
                //display speed low prompt
                if (driveCurveParams.promptSpeedLow != "")
                {
                    DisplayPrompt(GetPromptBox(driveCurveParams.promptSpeedLow), GetPrompt(driveCurveParams.promptSpeedLow), GetPromptPriority(driveCurveParams.promptSpeedLow),"red");
                    Log(LOG_DEV_DATA, "Updating prompt to Speed low");
                    promptSpeedLowDisplayed = true;
                    promptSpeedHighDisplayed = false;
                    promptSpeedInRangeDisplayed = false;
                }
                else
                {//in case a high speed prompt exists and no low speed, just remove current prompt
                    if(promptSpeedHighDisplayed)
                    {
                         m_prompt->ClearPromptBox(GetPromptBox(driveCurveParams.promptSpeedHigh), response);
                         Log(LOG_DEV_DATA, "Removed speed high prompt");
                         promptSpeedLowDisplayed = true;
                         promptSpeedHighDisplayed = false;
                         promptSpeedInRangeDisplayed = false;
                    }
                    //if prompt 2 is displayed, nothing needs to be done
                }
            }
        }
        else if(currentSpeed > maxSpeed)
        {
            if(!promptSpeedHighDisplayed)
            {
                //display speed high prompt

                if (driveCurveParams.promptSpeedHigh != "")
                {
                    DisplayPrompt(GetPromptBox(driveCurveParams.promptSpeedHigh), GetPrompt(driveCurveParams.promptSpeedHigh), GetPromptPriority(driveCurveParams.promptSpeedHigh),"red");
    
                    Log(LOG_DEV_DATA, "Updating prompt to Speed high");
                    promptSpeedLowDisplayed = false;
                    promptSpeedHighDisplayed = true;
                    promptSpeedInRangeDisplayed = false;
                }
                else
                {
                    if(promptSpeedLowDisplayed)
                    {
                        m_prompt->ClearPromptBox(GetPromptBox(driveCurveParams.promptSpeedLow), response);
                         Log(LOG_DEV_DATA, "Removed speed low prompt");
                         promptSpeedLowDisplayed = false;
                         promptSpeedHighDisplayed = true;
                         promptSpeedInRangeDisplayed = false;
                    }
                    //if prompt 2 is displayed, nothing needs to be done

                }
            }
        }
        else
        {
           if (!promptSpeedInRangeDisplayed)
           {
               //display in range prompt (prompt2)
               if (driveCurveParams.prompt2 != "")
               {

                   DisplayPrompt(GetPromptBox(driveCurveParams.prompt2), GetPrompt(driveCurveParams.prompt2), GetPromptPriority(driveCurveParams.prompt2),"white");
    
                   Log(LOG_DEV_DATA, "Updating prompt to Speed in range");
                   promptSpeedLowDisplayed = false;
                   promptSpeedHighDisplayed = false;
                   promptSpeedInRangeDisplayed = true;
               }
               else
               {
                    if(promptSpeedLowDisplayed)
                    {
                        m_prompt->ClearPromptBox(GetPromptBox(driveCurveParams.promptSpeedLow), response);
                         Log(LOG_DEV_DATA, "Removed speed low prompt");
                         promptSpeedLowDisplayed = false;
                         promptSpeedHighDisplayed = false;
                         promptSpeedInRangeDisplayed = true;
                    }
                    else
                    {
                        m_prompt->ClearPromptBox(GetPromptBox(driveCurveParams.promptSpeedHigh), response);
                         Log(LOG_DEV_DATA, "Removed speed high prompt");
                         promptSpeedLowDisplayed = false;
                         promptSpeedHighDisplayed = false;
                         promptSpeedInRangeDisplayed = true;
                    }
               }
           }
        }


        status = StatusSleep( scanDelay);
        currentTime = time(NULL);
    }
    
    Log(LOG_FN_ENTRY, "Exit MazdaTransmissionTC::PromptDriveCurveSection\n");
    return testResult;
}
//-------------------------------------------------------------------------------------------------
const string MazdaTransmissionTC::TestStepBrakeToStop(const string &value)
{
    bool done = false;  // status of the braking
    string status = BEP_NONE;  // set test step status to no response

    Log(LOG_FN_ENTRY, "Enter MazdaTransmissionTC::TestStepBrakeToStop()\n");

    try
    {   // if the conditions are correct to perform the task
        if (StatusCheck() == BEP_STATUS_SUCCESS)
        {
            // check if at zerospeed
            if (ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
            {
                // if not, put up the question and wait
                if (UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // Set the start time
                SetStartTime();
                while ((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if (ReadSubscribeData(GetDataTag("Zerospeed")) != "1")   BposSleep(250);
                    else                                                    done = true;
                }
            }
            // else flag done
            else
            {
                done = true;
            }
			Log(LOG_DEV_DATA, "Vehicle at zero speed: %s", done ? "True" : "False");
            // update the status of the test
            if (StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
            else if (!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
            else if (!done)                          status = BEP_FATALFAIL_STATUS;
            else                                    status = BEP_PASS_STATUS;

            //if successful, ensure that the motor mode is Boost mode
            if (status == BEP_PASS_STATUS)
            {
				Log(LOG_DEV_DATA, "Setting motor mode to %s", string(BOOST_MODE).c_str());
                SystemWrite(MOTOR_MODE, string(BOOST_MODE));
            }
        }
        // else the conditions are not correct, indicate not started
        else
        {
            status = BEP_TEST_NOT_STARTED;
        }
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "MazdaTransmissionTC::TestStepBrakeToStop Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    if (SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MazdaTransmissionTC::TestStepBrakeToStop Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "Exit MazdaTransmissionTC::TestStepBrakeToStop(), status=%s\n",
        status.c_str());

    return(status);     // return the status
}
