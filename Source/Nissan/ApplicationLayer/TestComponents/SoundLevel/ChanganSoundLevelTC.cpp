//******************************************************************************
// Description:
// 	Sound level monitoring test component
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /GM/Source/Changan/ApplicationLayer/TestComponents/SoundLevel/ChanganSoundLevelTC.cpp $
//
// 7     6/27/11  2:17p Rsmith
// Took ChanganSoundLevelTC from TataSoundLevelTC
//
// 6     3/09/06 12:50a Bmeinke
// The SoundPressureLevelBGColor value was not being set in a few
// failure/abort cases inside TestStepSoundPressureLevel()
// Changed unit strings passedto the test result server to the new unit
// string constants from GenericTC
// 
// 5     3/07/06 7:42p Bmeinke
// Changed IdleHornTestSoundPressureLevelMax to TestSoundPressureLevelMax
// inside TestStepSoundPressureLevel()
// 
// 4     3/07/06 12:37a Bmeinke
// Added idle and running sound level tests
// 
// 3     3/06/06 3:39p Bmeinke
// Added new test steps for the Horn test at idel and while the vehicle is
// at speed
// SystemWrite() the sound level tags so they are displayed on the front
// panel
// 
// 2     3/01/06 1:01a Bmeinke
// Stop the sound level monitoring thread if we get Abort,
// TestInProgress!=1 or StopCurrentTest!=0
// Added TestStepFinishUp()
// 
// 1     2/28/06 2:03p Bmeinke
// Seperated from TataMachineTC
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "ChanganSoundLevelTC.h"
#include "SerialDefs.h"
#include "WaitableTimer.h"

ChanganSoundLevelTC::ChanganSoundLevelTC() : GenericTC(),
	m_soundThread( -1), m_soundThreadStop( true), m_maxSoundLevel( 0.0),
	m_minSoundLevel(1000000.0), m_soundLevel( 0.0), m_soundLevelFaultTag("")
{
}

ChanganSoundLevelTC::~ChanganSoundLevelTC()
{
}

void ChanganSoundLevelTC::Initialize(const XmlNode *config)
{
	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::Initialize()\n");

	try
	{
		GenericTC::Initialize(config);

		XmlParser	protocolParser;
		std::string protocolFile = GetParameter( "ProtocolFile");

		FixUpPathString( protocolFile);
		const XmlNode *portConfig = protocolParser.ReturnXMLDocument( protocolFile);
		const std::string portName = portConfig->getChild("Setup/Communication/Name")->getValue();

		m_soundLevelPort.Initialize( portConfig);
		if( true != m_soundLevelPort.OpenPort( portName))
		{
			throw( BepException( "Unable to open port %s", portName.c_str()) );
		}
		// Stop the meter (to save the battery)
		(void)m_soundLevelPort.WritePort( "Stop");
	}
	catch( BepException &err)
	{
		Log(LOG_ERRORS, "Exception during initialization: %s\n", err.GetReason());
		throw;
	}

	// signal that initialization is complete
	InitializationComplete();

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::Initialize()\n");
}

const std::string ChanganSoundLevelTC::Publish(const XmlNode *node)
{
	const std::string &tag = node->getName();
	const std::string &value = node->getValue();
	std::string result;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::Publish(%s, %s)\n", tag.c_str(), value.c_str());

	// If we are aborting the current test
	if( ((BEP_ABORT_RESPONSE == tag) && ("0" != value)) ||
		// -OR- no test is in progress
		((TEST_IN_PROG_DATA_TAG == tag) && ("1" != value)) ||
		// -OR- we need to stop the current test
		((STOP_CURRENT_TEST_TAG == tag) && ("0" != value)))
	{
		// Shutdown the sound level monitoring thread
		(void)StopSoundThread();
	}
	
	result = GenericTC::Publish( node);
	
	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::Publish(%s, %s)\n", tag.c_str(), value.c_str());

	return( result);
}

const std::string ChanganSoundLevelTC::CommandTestStep(const std::string &value)
{
	std::string testStatus(testSoftwareFail);
	const std::string step(GetTestStepName());	// get the name of the sequenced test step

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::CommandTestStep(%s): %s\n", value.c_str(), step.c_str());

	try
	{
		if("ResetSoundMeter" == step)					testStatus = TestStepResetSoundMeter(value);
		else if("SoundMeterOctaveFilter" == step)		testStatus = TestStepSoundMeterFilterFreq(value);
		else if("SoundMeterFreqWeight" == step)			testStatus = TestStepSoundMeterFreqWeighting(value);
		else if("SoundMeterTimeWeight" == step)			testStatus = TestStepSoundMeterTimeWeighting(value);
		else if("ReadSoundMeterBattery" == step)		testStatus = TestStepReadSoundMeterBatteryVoltage(value);
		else if("ReadSoundMeterTemp" == step)			testStatus = TestStepReadSoundMeterInternalTemp(value);
		else if("StartSoundTest" == step)				testStatus = TestStepStartSoundTest(value);
		else if("StopSoundTest" == step)				testStatus = TestStepStopSoundTest(value);
		else if("SoundPressureLevelTest" == step)		testStatus = TestStepSoundPressureLevel(value);
		else if("IdleHornTest" == step)					testStatus = TestStepIdleHornTest(value);
		else if("RunningHornTest" == step)				testStatus = TestStepRunningHornTest(value);
		else if("FinishUp" == step)						testStatus = TestStepFinishUp(value);
		else											testStatus = GenericTC::CommandTestStep(value);
			
		Log(LOG_FN_ENTRY, "ChanganSoundLevelTC::CommandTestStep(%s): %s Returned: %s\n", 
			value.c_str(), step.c_str(), testStatus.c_str());
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.GetReason());
		testStatus = testError;
	}

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::CommandTestStep(%s): %s, status=%s\n", 
		value.c_str(), step.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepSoundPressureLevel(const std::string &value)
{
	std::string 	testStatus(testPass);
	std::string 	color("Green");
	float 			maxSoundLevel;
    float           minSoundLevel;
	int				testDuration, samplePeriod;
	float			soundLevel=-1.0, maxSample=-1.0, minSample=999.9;
	SerialString_t	response;
	std::string		faultTag("NoFault"), faultCode("000");
	char			buff[256];
	int				loopCount = 0;
	INT32			status = BEP_STATUS_FAILURE;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepSoundPressureLevel(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    { 
    	try
    	{
    		// Read the test parameters
    		(void)GetTestStepParameter( "SoundPressureLevelMin", minSoundLevel);
            (void)GetTestStepParameter( "SoundPressureLevelMax", maxSoundLevel);
    		(void)GetTestStepParameter( "SoundPressureLevelDuration", testDuration);
    		(void)GetTestStepParameter( "SoundPressureLevelSampleInterval", samplePeriod);
    
    		SystemWrite( GetTestStepName()+"SoundPressureLevelMax", maxSoundLevel);
            SystemWrite( GetTestStepName()+"SoundPressureLevelMin", minSoundLevel);
    
    		// Make sure parameters are sane
    		if( samplePeriod == 0)				samplePeriod = 1000;
    		if( testDuration < samplePeriod)	testDuration = samplePeriod;
    
    		// If the sound thread is not already running
    		if( -1 != m_soundThread)
    		{
    			SetTestStepInfoValue( "Timeout", testDuration);
    			UpdatePrompts();
    
    			do
    			{
    				// The sound level value will be updated by the sound level thread
    				soundLevel = m_soundLevel;
    
    				// Update the GUI in real time
    				SystemWrite( GetTestStepName()+"SoundPressureLevelValue", soundLevel);
    
    				// Record min and max sound levels
    				if(soundLevel > maxSample)	maxSample = soundLevel;
                    if(soundLevel < minSample)  minSample = soundLevel;
    
    				Log( LOG_DEV_DATA, "Sound meter result: Value=%.2f\n", soundLevel);
    
    				// Fail if sound level is too high
    				if((soundLevel > maxSoundLevel) || (soundLevel < minSoundLevel))
    				{
    					Log( LOG_ERRORS, "Sound meter test failed\n");
    					faultTag = "SoundPressureLevelFault";
    					color = "Red";
    					testStatus = testFail;
    				}
    				else
    				{
    					status = StatusSleep( samplePeriod);
    				}
    
    				loopCount++;
    			} while( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) && (testPass == testStatus));
    		}
    		else
    		{
    			Log( LOG_ERRORS, "Sound level monitoring thread not running\n");
    			color = "Red";
    			testStatus = testSoftwareFail;
    		}
    	}
    	catch(BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during sound meter test: %s\n", err.GetReason());
    		color = "Red";
    		testStatus = testFail;
    	}
    
    	RemovePrompts();
    	
    	// check to make sure an abort did not happen, if so flag it
    	if(CheckAbort())
    	{
    		testStatus = testAbort;
    		color = "Yellow";
    	}
    
    	// Publish final values to the GUI
    	SystemWrite( GetTestStepName()+"SoundPressureLevelValue", soundLevel);
    	SystemWrite( GetTestStepName()+"SoundPressureLevelMax", maxSoundLevel);
        SystemWrite( GetTestStepName()+"SoundPressureLevelMin", minSoundLevel);
    	SystemWrite( GetTestStepName()+"SoundPressureLevelBGColor", color);
    	
    	// update the test results												  
    	status = SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag),
    									  "SoundPressureLevel", CreateMessage(buff, sizeof(buff), "%.02f", soundLevel), "dB",
    									  "MaxSoundPressureLevel", CreateMessage(buff, sizeof(buff), "%.02f", maxSoundLevel), "dB",
                                          "MinSoundPressureLevel", CreateMessage(buff, sizeof(buff), "%.02f", minSoundLevel), "dB",
    									  "SoundPressureLevelMax", CreateMessage(buff, sizeof(buff), "%.02f", maxSample), "dB",
                                          "SoundPressureLevelMin", CreateMessage(buff, sizeof(buff), "%.02f", minSample), "dB");
    	if(BEP_STATUS_SUCCESS != status)
    	{
    		Log(LOG_ERRORS, "ChanganSoundLevelTC Could Not Send Test Result: %s, %s\n", 
    			GetTestStepName().c_str(), testStatus.c_str());
    	}
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }


	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepSoundPressureLevel(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);				// return the status
}

const std::string ChanganSoundLevelTC::TestStepIdleHornTest(const std::string &value)
{
	std::string 	testStatus(testPass);

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepHornTest(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {
        // place tag on drive curve graph
        TagArray("IdleHornTest");

    	SetTestStepInfoValue( "SoundPressureLevelMax", GetParameter( "IdleHornLevelMax"));
        SetTestStepInfoValue( "SoundPressureLevelMin", GetParameter( "IdleHornLevelMin"));
    	SetTestStepInfoValue( "SoundPressureLevelDuration", GetParameter( "IdleHornLevelDuration"));
    	SetTestStepInfoValue( "SoundPressureLevelSampleInterval", GetParameter( "IdleHornLevelSampleInterval"));
    
    	testStatus = TestStepSoundPressureLevel( value);
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }
	
	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepHornTest(%s), status=%s\n", value.c_str(), testStatus.c_str());

	return(testStatus);				// return the status
}

const std::string ChanganSoundLevelTC::TestStepRunningHornTest(const std::string &value)
{
	std::string 	testStatus(testPass);

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepHornTest(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {
        // place tag on drive curve graph
        TagArray("RunningHornTest");

    	SetTestStepInfoValue( "SoundPressureLevelMax", GetParameter( "RunningHornLevelMax"));
        SetTestStepInfoValue( "SoundPressureLevelMin", GetParameter( "RunningHornLevelMin"));
    	SetTestStepInfoValue( "SoundPressureLevelDuration", GetParameter( "RunningHornLevelDuration"));
    	SetTestStepInfoValue( "SoundPressureLevelSampleInterval", GetParameter( "RunningHornLevelSampleInterval"));
    
    	testStatus = TestStepSoundPressureLevel( value);
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }
	
	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepHornTest(%s), status=%s\n", value.c_str(), testStatus.c_str());

	return(testStatus);				// return the status
}

const std::string ChanganSoundLevelTC::TestStepStartSoundTest(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultTag("NoFault");
	char			buff[256];
	int				status;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepStartSoundTest(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    { 
        UpdatePrompts();
    	try
    	{
    		// If the sound thread is not already running
    		if( -1 == m_soundThread)
    		{
    			// Clear the flag used to terminate the sound level monitoring thread
    			m_soundThreadStop = false;
    
    			// Start the sound level monitoring thread
    			status = pthread_create( &m_soundThread, NULL, ChanganSoundLevelTC::SoundLevelThread, (void*)this);
    			if( EOK == status)
    			{
    				Log( LOG_DEV_DATA, "Sound level monitoring thread started, thread id = %d\n", m_soundThread);
    				testStatus = testPass;
    			}
    			else
    			{
    				Log( LOG_ERRORS, "Error starting the sound level monitoring thread: %s\n", strerror( status));
    				testStatus = testFail;
    				faultTag = "SoftwareFailure";
    			}
    		}
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during ChanganSoundLevelTC::TestStepStartSoundTest: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    	RemovePrompts();
    
    	// update the test results												  
    	status = SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag),
    									  "ThreadID", CreateMessage(buff, sizeof(buff), "%d", m_soundThread), "");
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepStartSoundTest(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepStopSoundTest(const std::string &value)
{
	std::string 	testStatus(testPass);
	std::string		faultTag("NoFault");
	float 			maxSoundLevel;
	int				status;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepStopSoundTest(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {
    	// Read the test parameters
    	(void)GetTestStepParameter( "SoundPressureLevelMax", maxSoundLevel);
    	
    	UpdatePrompts();
    	try
    	{
    		// If the sound thread is not already running
    		if( -1 != m_soundThread)
    		{
    			// Tell the sound monitoring thread to stop
    			status = StopSoundThread();
    
    			// If we successfully stopped the sound level monitoring thread
    			if( EOK == status)
    			{
    				// If the thread did not encounter any problems
    				if( true == m_soundLevelFaultTag.empty())
    				{
    					testStatus = testPass;
    				}
    				else
    				{
    					Log( LOG_ERRORS, "Sound monitoring thread detected error: %s\n", 
    						 m_soundLevelFaultTag.c_str());
    					testStatus = testFail;
    					faultTag = m_soundLevelFaultTag;
    				}
    			}
    			else
    			{
    				testStatus = testFail;
    				faultTag = "SoftwareFailure";
    			}
    		}
    		else
    		{
    			Log( LOG_DEV_DATA, "Sound level thread not running\n");
    			testStatus = testFail;
    		}
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during ChanganSoundLevelTC::TestStepStartSoundTest: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    	RemovePrompts();
    
    	Log( LOG_DEV_DATA, "Sound Test Results: Avg: %.02f, Min: %.02f, Max, %.02f\n", 
    		 m_soundLevel, m_minSoundLevel, m_maxSoundLevel);
    	
    	// update the test results												  
    	status = SendTestResult( testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag));
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepStopSoundTest(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepResetSoundMeter(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	int				bytesSent=-2;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepResetSoundMeter(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    { 
    	try
    	{
    		// Setup the meter filter frequency
    		bytesSent = m_soundLevelPort.WritePort( "Reset");
    		// Disregard any response to this command
    		m_soundLevelPort.ResetConnection();
    
    		if( bytesSent <= 0)	faultCode = GetFaultCode("CommunicationError");
    		else				testStatus = testPass;
    
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepResetSoundMeter: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    
    	SendTestResult(testStatus, GetTestStepInfo("Description"), faultCode);
    
    	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepResetSoundMeter(%s), status=%s\n", 
    		value.c_str(), testStatus.c_str());  
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepSoundMeterFilterFreq(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	std::string		filterFreq;
	int				bytesSent=-2;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepSoundMeterFilterFreq(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {  

    	UpdatePrompts();
    	try
    	{
    		// Read the test parameters
    		(void)GetTestStepParameter( "SoundPressureLevelFilterFrequency", filterFreq);
    		
    		// Setup the meter filter frequency
    		switch( atoi(filterFreq.c_str()))
    		{
    		case 31:			bytesSent = m_soundLevelPort.WritePort( "Filter31Hz");		break;
    		case 63:			bytesSent = m_soundLevelPort.WritePort( "Filter63Hz");		break;
    		case 125:			bytesSent = m_soundLevelPort.WritePort( "Filter125Hz");		break;
    		case 250:			bytesSent = m_soundLevelPort.WritePort( "Filter250Hz");		break;
    		case 500:			bytesSent = m_soundLevelPort.WritePort( "Filter500Hz");		break;
    		case 1:	case 1000:	bytesSent = m_soundLevelPort.WritePort( "Filter1000Hz");	break;
    		case 2:	case 2000:	bytesSent = m_soundLevelPort.WritePort( "Filter2000Hz");	break;
    		case 4:	case 4000:	bytesSent = m_soundLevelPort.WritePort( "Filter4000Hz");	break;
    		case 8:	case 8000:	bytesSent = m_soundLevelPort.WritePort( "Filter8000Hz");	break;
    		default:			break;
    		}
    
    		if( bytesSent == -2)		faultCode = GetFaultCode("InvalidFilterFrequency");
    		else if( bytesSent <= 0)	faultCode = GetFaultCode("CommunicationError");
    		else						testStatus = testPass;
    
    		// Disregard any response to this command
    		m_soundLevelPort.ResetConnection();
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepSoundMeterFilterFreq: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    	RemovePrompts();
    
    	SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), faultCode,
    							 "SoundMeterFilterFrequency", filterFreq, "Hz");
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }


	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepSoundMeterFilterFreq(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepSoundMeterFreqWeighting(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	std::string		filterFreqWeight;
	int				bytesSent=-2;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepSoundMeterFreqWeighting(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {  
    	UpdatePrompts();
    	try
    	{
    		// Read the test parameters
    		(void)GetTestStepParameter( "SoundPressureLevelFreqWeighting", filterFreqWeight);
    
    		Log( LOG_ERRORS, "Frequency weighting option: %s\n", filterFreqWeight.c_str());
    		// Setup the meter frequency weighting
    		if( 0 == stricmp( filterFreqWeight.c_str(), "A"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "A-WeightReset");
    		}
    		else if( 0 == stricmp( filterFreqWeight.c_str(), "B"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "B-WeightReset");
    		}
    		else if( 0 == stricmp( filterFreqWeight.c_str(), "C"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "C-WeightReset");
    		}
    		else
    		{
    			Log( LOG_ERRORS, "Invalid frequency weighting option: %s\n", filterFreqWeight.c_str());
    		}
    
    		if( bytesSent == -2)		faultCode = GetFaultCode("InvalidFrequencyWeighting");
    		else if( bytesSent <= 0)	faultCode = GetFaultCode("CommunicationError");
    		else						testStatus = testPass;
    
    		// Disregard any response to this command
    		m_soundLevelPort.ResetConnection();
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepSoundMeterFreqWeighting: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    	RemovePrompts();
    
    	SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), faultCode,
    							 "SoundMeterFrequencyWeighting", filterFreqWeight, "");
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }


	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepSoundMeterFreqWeighting(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepSoundMeterTimeWeighting(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	std::string		filterTimeWeight;
	int				bytesSent=-2;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepSoundMeterTimeWeighting(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {  
    	try
    	{
    		// Read the test parameters
    		(void)GetTestStepParameter( "SoundPressureLevelTimeWeighting", filterTimeWeight);
    		
    		Log( LOG_DEV_DATA, "Time weighting option: %s\n", filterTimeWeight.c_str());
    		// Setup the meter time weighting
    		if( 0 == stricmp( filterTimeWeight.c_str(), "Fast"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "FastMode");
    		}
    		else if( 0 == stricmp( filterTimeWeight.c_str(), "Slow"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "SlowMode");
    		}
    		else if( 0 == stricmp( filterTimeWeight.c_str(), "Integral"))
    		{
    			bytesSent = m_soundLevelPort.WritePort( "Leq");
    		}
    		else
    		{
    			Log( LOG_ERRORS, "Invalid time weighting option: %s\n", filterTimeWeight.c_str());
    		}
    
    		if( bytesSent == -2)		faultCode = GetFaultCode("InvalidTimeWeighting");
    		else if( bytesSent <= 0)	faultCode = GetFaultCode("CommunicationError");
    		else						testStatus = testPass;
    
    		// Disregard any response to this command
    		m_soundLevelPort.ResetConnection();
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepSoundMeterTimeWeighting: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    
    	SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), faultCode,
    							 "SoundMeterTimeWeighting", filterTimeWeight, "");
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepSoundMeterTimeWeighting(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepReadSoundMeterBatteryVoltage(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	SerialString_t	response;
	int				bytesRead;
	float			voltage=0.0;
	char			buff[256];

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepReadSoundMeterBatteryVoltage(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    {  
        try
    	{
    		// Start with a clean buffer
    		m_soundLevelPort.ResetConnection();
    		// Read the Meter battery voltage
    		m_soundLevelPort.WritePort( "ReadBatteryVoltage");
    		bytesRead = m_soundLevelPort.ReadPort( response, 100, 10);
    		if( bytesRead > 5)
    		{
    			response.push_back( 0);
    			voltage = (float)(atoh( (const char*)response.c_str())) / 10.0;
    			Log( LOG_DEV_DATA, "Sound meter battery voltage: %.2f\n", voltage);
    
    			testStatus = testPass;
    		}
    		else
    		{
    			faultCode = GetFaultCode("CommunicationError");
    			Log( LOG_ERRORS, "Error reading sound meter battery voltage\n");
    		}
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepReadSoundMeterBatteryVoltage: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    
    	SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), faultCode,
    							 "Voltage", CreateMessage(buff, sizeof(buff), "%.2f", voltage), "Volts");
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepReadSoundMeterBatteryVoltage(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepReadSoundMeterInternalTemp(const std::string &value)
{
	std::string 	testStatus(testFail);
	std::string		faultCode("0000");
	SerialString_t	response;
	int				bytesRead;
	float			temp=0.0;
	char			buff[256];

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::TestStepReadSoundMeterInternalTemp(%s)\n", value.c_str());

    if (!ShortCircuitTestStep())
    { 
        try
    	{
    		// Start with a clean buffer
    		m_soundLevelPort.ResetConnection();
    		// Read the Meter internal temperature
    		m_soundLevelPort.WritePort( "ReadInternalTemp");
    		bytesRead = m_soundLevelPort.ReadPort( response, 100, 10);
    		if( bytesRead > 5)
    		{
    			response.push_back( 0);
    			temp = (float)(atoh( (const char*)response.c_str())) / 10.0;
    			Log( LOG_DEV_DATA, "Sound meter battery temperature: %.2f\n", temp);
    
    			testStatus = testPass;
    		}
    		else
    		{
    			faultCode = GetFaultCode("CommunicationError");
    			Log( LOG_ERRORS, "Error reading sound meter internal temperature\n");
    		}
    	}
    	catch( BepException &err)
    	{
    		Log(LOG_ERRORS, "Error during TestStepReadSoundMeterInternalTemp: %s\n", err.GetReason());
    		testStatus = testFail;
    	}
    
    	SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), faultCode,
    							 "SoundMeterTemperature", CreateMessage(buff, sizeof(buff), "%.2f", temp), unitsDEGF);
    }
    else
    {
        // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testStatus = testSkip;
    }

	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::TestStepReadSoundMeterInternalTemp(%s), status=%s\n", 
		value.c_str(), testStatus.c_str());

	return(testStatus);
}

const std::string ChanganSoundLevelTC::TestStepFinishUp(const std::string &value)
{
	// Close our comm port
	m_soundLevelPort.ClosePort();
	// send the overall test result
	SendOverallResult();
	// always return success
	return(GetOverallResult());
}

int ChanganSoundLevelTC::StopSoundThread()
{
	int			status = EOK;

	Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::StopSoundThread()\n");

	// If the sound thread is not already running
	if( -1 != m_soundThread)
	{
		uint32_t	samplePeriod = GetParameterInt( "SoundPressureLevelSampleInterval");
		uint64_t	joinTime = mSEC_nSEC( 4*samplePeriod);
		
		// Tell the sound monitoring thread to stop
		m_soundThreadStop = true;

		// Wait (up to 5 secs) for the thread to stop
		TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_JOIN, NULL, &joinTime, &joinTime);
		status = pthread_join( m_soundThread, NULL);
		if( EOK != status)
		{
			Log( LOG_ERRORS, "Error stopping the sound level monitoring thread: %s\n", strerror( status));
			pthread_cancel( m_soundThread);
			pthread_kill( m_soundThread, SIGKILL);
		}
		m_soundThread = -1;
	}
	Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::StopSoundThread(), status=%d\n", status);

	return( status);
}

void* ChanganSoundLevelTC::SoundLevelThread( void* arg)
{
	ChanganSoundLevelTC	*machTC = (ChanganSoundLevelTC*)arg;
	ILogicalPort	&port = machTC->m_soundLevelPort;
	uint32_t		sampleCount = 0;
	float			soundLevel=-1;
	SerialString_t	response;
	int				bytesRead, samplePeriod;
	WaitableTimer	timer;
    int             commFaultThreshold = 0;
    int             commFaults = 0;

	machTC->Log(LOG_FN_ENTRY, "Enter ChanganSoundLevelTC::SoundLevelThread()\n");

	samplePeriod = machTC->GetParameterInt( "SoundPressureLevelSampleInterval");
    commFaultThreshold = machTC->GetParameterInt("CommFaultThreshold");
	if( samplePeriod == 0)	samplePeriod = 1000;
	
	// Read the sound level from the meter
	//////////////////////////////////////
	// Make sure the sound meter is running
	(void)port.WritePort( "Stop");	BposSleep( 500);
	(void)port.WritePort( "Run");	BposSleep( 500);
	
	// Disregard any response to this command
	(void)port.ResetConnection();

	// Start our timer
	timer.Start( samplePeriod);

	// Loop until told to stop
	while( false == machTC->m_soundThreadStop)
	{
		try
		{
			// Wait for our timer to fire
			if( EOK == timer.Wait())
			{
				// Send the "read" command
				if( port.WritePort( "ScreenDump") > 0)
				{
					// Get the sound pressure reasing value
					response.clear();
					bytesRead = port.ReadPort( response, 100, 10);
					if( bytesRead > 14)
					{
						// Increment sample count in order to calculate average
						sampleCount++;

						/**
						 * Extract the sound pressure level from the response string.
						 * The sound pressure level value starts in column 10 and ends 
						 * on column 14 of line 1 of the meter's screen.
						 */
						response.push_back( 0);
						soundLevel = atof((const char*)response.c_str()+9);

						// Record min and max sound levels
						if( soundLevel > machTC->m_maxSoundLevel)
							machTC->m_maxSoundLevel = soundLevel;
						if( soundLevel < machTC->m_minSoundLevel)
							machTC->m_minSoundLevel = soundLevel;

						machTC->m_soundLevel = soundLevel;

						machTC->Log( LOG_DETAILED_DATA, "Sound meter result: Value=%.2f\n", soundLevel);
					}
					else
					{
						machTC->Log( LOG_ERRORS, "Error reading sound meter level\n");
                        commFaults++;
                        if(commFaults > commFaultThreshold)
                        {
						machTC->m_soundLevelFaultTag = "CommunicationFailure";
					}
				}
				}
				else
				{
					machTC->Log( LOG_ERRORS, "Error commanding sound meter to sample\n");
                    commFaults++;
                    if(commFaults > commFaultThreshold)
                    {
					machTC->m_soundLevelFaultTag = "CommunicationFailure";
				}
			}
			}
			else
			{
				BposSleep( samplePeriod);
			}

			port.ResetConnection();
		}
		catch(BepException &err)
		{
			machTC->Log(LOG_ERRORS, "Error during sound meter test: %s\n", err.GetReason());
			machTC->m_soundLevelFaultTag = "SoftwareFailure";
		}
	}
	(void)port.WritePort( "Stop");
	machTC->Log(LOG_FN_ENTRY, "Exit ChanganSoundLevelTC::SoundLevelThread()\n");
	
	return( NULL);
}

