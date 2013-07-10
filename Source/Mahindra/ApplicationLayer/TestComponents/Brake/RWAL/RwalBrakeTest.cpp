//******************************************************************************
// Description:
//  RWAL Brake test component class
//
//==============================================================================
// Copyright (c) 2012 - Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "RwalBrakeTest.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
RwalBrakeTest<ModuleType>::RwalBrakeTest() : GenericABSTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
RwalBrakeTest<ModuleType>::~RwalBrakeTest()
{
}

template<class ModuleType>
void RwalBrakeTest<ModuleType>::InitializeHook(const XmlNode *config)
{
	Log(LOG_FN_ENTRY, "Enter RwalBrakeTest::InitializeHook()\n");
	try
	{

		XmlParser	protocolParser;
		std::string protocolFile = GetParameter( "ProtocolFile");

		FixUpPathString( protocolFile);
		const XmlNode *portConfig = protocolParser.ReturnXMLDocument( protocolFile);
		const std::string portName = portConfig->getChild("Setup/Communication/Name")->getValue();

		m_rwalCommPort.Initialize( portConfig);
		if( true != m_rwalCommPort.OpenPort( portName))
		{
		  //  throw( BepException( "Unable to open port %s", portName.c_str()) );
		}
	}
	catch( BepException &err)
	{
		Log(LOG_ERRORS, "Exception during initialization: %s\n", err.GetReason());
		//throw;
	}
      catch(XmlException &ex)
      {
          Log(LOG_ERRORS, "xml Exception during initialization: %s\n", ex.GetReason());
      
      }
      catch(...)
      {

          Log(LOG_ERRORS, "unknown Exception during initialization\n");
      }

    GenericABSTCTemplate<ModuleType>::InitializeHook(config);

	Log(LOG_FN_ENTRY, "Exit RwalBrakeTest::InitializeHook()\n");
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string RwalBrakeTest<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "RwalBrakeTest::CommandTestStep(value: %s) - Enter", value.c_str());
	try
	{   // Make sure the system status is ok before trying to run this test step
		if(BEP_STATUS_SUCCESS != StatusCheck())
		{   // System status is not ok, do not perform the test step
			result = testSkip;
			SendTestResult(result, GetTestStepInfo("Description"));
			Log(LOG_ERRORS, "RwalBrakeTest::CommandTestStep() - Status Check failed (%s), not performing %s",
				ConvertStatusToResponse(StatusCheck()).c_str(), GetTestStepName().c_str());
		}
		else if(!GetTestStepName().compare("RwalSensorTest"))   result = ValidateRwalSensor();
		else                                                    result = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BepException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "RwalBrakeTest::CommandTestStep(value: %s) - Exit", value.c_str());
	return result;
}


//-------------------------------------------------------------------------------------------------
template<class ModuleType>
float RwalBrakeTest<ModuleType>::GetAverageRWALSpeed(void)
{
    
	SerialString_t	response;
	int				bytesRead;
	float			temp=0.0;
    float           total=0.0;
    float           average;
    UINT8 MESSAGE_LENGTH = 8;
    UINT8 DATA_LENGTH = 6;
    bytesRead = m_rwalCommPort.ReadPort( response, 100, 0);
    int fullSamples = (int)(bytesRead / MESSAGE_LENGTH);
    if (fullSamples > 0)
    {
        for(int x = 0; x <fullSamples; x++)
        {

            const SerialString_t::size_type dataStartIdx = x * MESSAGE_LENGTH+2;    
            const SerialString_t::size_type dataRespSz = DATA_LENGTH;
            SerialString_t currentSample = response.substr(dataStartIdx,dataRespSz);
            temp = atof( (const char*)currentSample.c_str()) / KPH_MPH;
            total += temp;
            Log( LOG_DEV_DATA, "RWAL sample: %.2f\n", temp);
        }
         average = total / fullSamples;
        Log( LOG_DEV_DATA, "Number of samples: %d, Total: %.2f Average: %.2f\n", fullSamples, total, average);
        return average;

    }
    else
    {
            return -1;
    }
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string RwalBrakeTest<ModuleType>::ValidateRwalSensor(void)
{   // Log the entry and check if this step should be performed
	Log(LOG_FN_ENTRY, "RwalBrakeTest::ValidateRwalSensor() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep() && IsRwalEquipped())
	{
	    INT32 sampleCount = 0;
		float currentRollerSpeed = 0.0;
		float runningRollerSpeed = 0.0;
		TestResultDetails details;
        uint8_t tempBuff[ 4096];
        int				bytesRead;
		result = AccelerateToTestSpeed(GetParameterFloat("RwalTestSpeed"), GetParameter("RwalTestSpeedRange"),
									   GetTestStepInfoInt("ScanDelay"), false,"RWALAccelerateToTargetSpeed");
		if(!result.compare(testPass))
		{   // Keep the green band displayed during the test
			DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
						  GetPromptPriority("MaintainSpeed"));
			SystemWrite(GetDataTag("SpeedTarget"), GetParameter("RwalTestSpeedRange"));

    		m_rwalCommPort.WritePort( "StartRWALSpeedBroadcast");
            //read only 3 byte response to start command
            bytesRead=m_rwalCommPort.ReadPort( tempBuff, 1, 100, 10);
            if (bytesRead == 1)
            {
			do
			{	// Read the current sample from the module
				currentRollerSpeed = GetRollSpeed();
					runningRollerSpeed += currentRollerSpeed;
					Log(LOG_DEV_DATA, "RWAL Sensor Sample %03d:  total: %.2f,  roller Speed: %.2f", 
						sampleCount, runningRollerSpeed, currentRollerSpeed);
					sampleCount++;
					BposSleep(GetParameterInt("RwalSensorCheckInterval"));
			} while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) &&
					(sampleCount < GetParameterInt("TotalRwalSamples")));
			// Determine the result of the sensor test
			if(sampleCount)
			{	// Get the average RWAL speed

				float averageSensorSpeed = GetAverageRWALSpeed();

                if(averageSensorSpeed > 0)
                {
    				float averageRollerSpeed = runningRollerSpeed / (float)sampleCount;
    				float lowLimit = averageRollerSpeed * ((100 - GetParameterFloat("RwalSensorTolerance")) / 100.0);
    				float hiLimit = averageRollerSpeed * ((100 + GetParameterFloat("RwalSensorTolerance")) / 100.0);
    				result = ((lowLimit <= averageSensorSpeed) && (averageSensorSpeed <= hiLimit)) ? testPass : testFail;
    				Log(LOG_DEV_DATA, "RWAL Sensor:  %.2f  [%.2f  %.2f] - %s,  Average Roller Speed: %.2f",
    					averageSensorSpeed, lowLimit, hiLimit, result.c_str(), averageRollerSpeed);
    				char buff[64];
    				details.AddDetail("RwalSensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", averageSensorSpeed), unitsMPH);
    				details.AddDetail("RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", averageRollerSpeed), unitsMPH);
    				details.AddDetail("LowLimit", CreateMessage(buff, sizeof(buff), "%.2f", lowLimit), unitsMPH);
    				details.AddDetail("HighLimit", CreateMessage(buff, sizeof(buff), "%.2f", hiLimit), unitsMPH);
    				details.AddDetail("SampleCount", CreateMessage(buff, sizeof(buff), "%d", sampleCount), "");
                }
                else 
                {
                        // No samples collected, cannot analyze anything
				result = testFail;
				Log(LOG_DEV_DATA, "No rwal speed samples collected");                
                }
			}
			else
			{	// No samples collected, cannot analyze anything
				result = testFail;
				Log(LOG_DEV_DATA, "No roll speed samples collected");
			}
            }
            else
			{	// error starting rwal comms
				result = testFail;
				Log(LOG_DEV_DATA, "Error starting comms with rwal serial device");

            }
			RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
						 GetPromptPriority("MaintainSpeed"));
			SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
		}
		else
		{   // Could not achieve test speed
			Log(LOG_ERRORS, "Failed to reach RWAL test speed");
			result = testFail;
		}
		// Report the results
		SendTestResultWithDetail(result, details, GetTestStepInfo("Description"), "0000");
		// Make sure to return to zero speed
		CheckZeroSpeed();
	}
	else
	{   // Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping ValidateRwalSensor()");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "RwalBrakeTest::ValidateRwalSensor() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
bool RwalBrakeTest<ModuleType>::IsRwalEquipped(void)
{
	string rwalValue(SystemRead(GetDataTag("RwalEquippedBuildItemTag")));
	string rwalCompare(GetDataTag("RwalEQuippedValue"));
	bool rwalEquipped = !rwalValue.compare(rwalCompare);
	Log(LOG_DEV_DATA, "Vehicle equipped with RWAL: %s - ABS Type: %s,  RWAL Type: %s",
		rwalEquipped ? "True" : "False", rwalValue.c_str(), rwalCompare.c_str());
	return rwalEquipped;
}
