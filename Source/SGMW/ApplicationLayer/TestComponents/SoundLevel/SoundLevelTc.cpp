//******************************************************************************
// Description:
//  Sound level monitoring test component
//
//==============================================================================
// Copyright (c) 2008 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log$
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SoundLevelTc.h"
#include "SerialDefs.h"
#include "WaitableTimer.h"

SoundLevelTc::SoundLevelTc() : GenericTC(),                                                       
m_soundThread( -1), m_soundThreadStop( true), m_maxSoundLevel(0),                          
m_minSoundLevel(999), m_soundLevel(0), m_soundLevelFaultTag(""), m_startByte(0x00),
m_testCompleteByte(0x00), m_permitTransmitByte(0x00),m_resetByte(0x00), m_ackResponseByte(0x00), 
m_dataEndByte(0x00), m_dataStartByte(0x00)
{                                                                                                 
}                                                                                           

SoundLevelTc::~SoundLevelTc()                                                                 
{
}

void SoundLevelTc::Initialize(const XmlNode *config)
{
    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::Initialize()\n");

    try
    {
        GenericTC::Initialize(config);

        XmlParser   protocolParser;
        std::string protocolFile = GetParameter( "ProtocolFile");
        UINT8 value;

        FixUpPathString( protocolFile);
        const XmlNode *portConfig = protocolParser.ReturnXMLDocument( protocolFile);
        const std::string portName = portConfig->getChild("Setup/Communication/Name")->getValue();

        m_soundLevelPort.Initialize( portConfig);
        if ( true != m_soundLevelPort.OpenPort( portName))
        {
            throw( BepException( "Unable to open port %s", portName.c_str()) );
        }
        Log(LOG_FN_ENTRY, "Port Initialized\n");
        try
        {
            SMProtocolType(GetParameter("SoundMeterProtocol"));
        }
        catch (...)
        {
            SMProtocolType(GetParameter(""));
        }
        Log(LOG_FN_ENTRY, "Got Protocol\n");
        try
        {
            bool useChecksum = false;
            useChecksum = GetParameterBool("UseChecksum");
            UseChecksum(&useChecksum);
        }
        catch (...)
        {
        }
        try
        {
            value = atoh(GetParameter("StartTestByte").c_str());
            StartTestByte(&value);
        }
        catch (...)
        {
            value = 0x00;
            StartTestByte(&value);
            Log(LOG_ERRORS, "\tStartTestByte: <$%02X>", StartTestByte());
        }
        try
        {
            value = atoh(GetParameter("TestCompleteByte").c_str());
            TestCompleteByte(&value);
        }
        catch (...)
        {
            value = 0x00;
            TestCompleteByte(&value);
            Log(LOG_ERRORS, "\tTestCompleteByte: <$%02X>", TestCompleteByte());
        }
        try
        {
            value = atoh(GetParameter("PermitTransmitByte").c_str());
            PermitTransmitByte(&value);
        }
        catch (...)
        {
            value = 0x00;
            PermitTransmitByte(&value);
            Log(LOG_ERRORS, "\tPermitTransmitByte: <$%02X>", PermitTransmitByte());
        }
        try
        {
            value = atoh(GetParameter("ResetByte").c_str()); 
            ResetByte(&value);
        }
        catch (...)
        {
            value = 0x00;
            ResetByte(&value);
            Log(LOG_ERRORS, "\tResetByte: <$%02X>", ResetByte());
        }
        try
        {
            value = atoh(GetParameter("AckResponseByte").c_str());
            AckResponseByte(&value);
        }
        catch (...)
        {
            value = 0x00;
            AckResponseByte(&value);
            Log(LOG_ERRORS, "\tAckResponseByte: <$%02X>", AckResponseByte());
        }
        try
        {
            value = atoh(GetParameter("DataEndByte").c_str());
            DataEndByte(&value);
        }
        catch (...)
        {
            value = 0x0D;
            DataEndByte(&value);
            Log(LOG_ERRORS, "\tDataEndByte: <$%02X>", DataEndByte());
        }
        try
        {
            value = atoh(GetParameter("DataStartByte").c_str());
            DataStartByte(&value);
        }
        catch (...)
        {// Comand Bytes not specified, use defaults
            value = 0x0D;
            DataStartByte(&value);
            Log(LOG_ERRORS, "\tDataStartByte: <$%02X>", DataStartByte());
        }

        Log(LOG_FN_ENTRY, "Got Other Values\n");
    }
    catch ( BepException &err)
    {
        Log(LOG_ERRORS, "Exception during initialization: %s\n", err.GetReason());
        throw;
    }

    // signal that initialization is complete
    InitializationComplete();
	ResetSystemTags();

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::Initialize()\n");
}

void SoundLevelTc::WarmInitialize()
{
    Log( LOG_FN_ENTRY, "Enter SoundLevelTc::WarmInitialize()\n");

    // Reset data members that will get 'set' from test steps
    m_soundThread = -1;
    m_soundThreadStop = false;
    m_maxSoundLevel = 0;
    m_minSoundLevel = 999;
    m_soundLevel = 0;
    m_soundLevelFaultTag = "";

    // Call the base class to complete initialization
    GenericTC::WarmInitialize();

    Log( LOG_FN_ENTRY, "Exit SoundLevelTc::WarmInitialize()\n");
}

const std::string SoundLevelTc::Publish(const XmlNode *node)
{
    const std::string &tag = node->getName();
    const std::string &value = node->getValue();
    std::string result;

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::Publish(%s, %s)\n", tag.c_str(), value.c_str());

    // If we are aborting the current test
    if ( ((BEP_ABORT_RESPONSE == tag) && ("0" != value)) ||
         // -OR- no test is in progress
         ((TEST_IN_PROG_DATA_TAG == tag) && ("1" != value)) ||
         // -OR- we need to stop the current test
         ((STOP_CURRENT_TEST_TAG == tag) && ("0" != value)))
    {
        // Shutdown the sound level monitoring thread
        (void)StopSoundThread();
    }

    result = GenericTC::Publish( node);

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::Publish(%s, %s)\n", tag.c_str(), value.c_str());

    return( result);
}

const std::string SoundLevelTc::CommandTestStep(const std::string &value)
{
    std::string testStatus(testSoftwareFail);
    const std::string step(GetTestStepName());  // get the name of the sequenced test step

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::CommandTestStep(%s): %s\n", value.c_str(), step.c_str());

    try
    {
        // make sure it is OK to test
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {
            testStatus = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS,"SoundLevelTc::CommandTestStep: StatusCheck() failed: %s\n",
                testStatus.c_str());
            SendTestResult(testStatus,GetTestStepInfo("Description"));
        }
        else if ("StartSoundTest" == step)              testStatus = TestStepStartSoundTest(value);
        else if ("StopSoundTest" == step)          testStatus = TestStepStopSoundTest(value);
        else if ("SoundPressureLevelTest" == step) testStatus = TestStepSoundPressureLevel(value);
        else if ("FinishUp" == step)               testStatus = TestStepFinishUp(value);
        else                                      testStatus = GenericTC::CommandTestStep(value);

        Log(LOG_FN_ENTRY, "SoundLevelTc::CommandTestStep(%s): %s Returned: %s\n", 
            value.c_str(), step.c_str(), testStatus.c_str());
    }
    catch (BepException &err)
    {
        Log(LOG_ERRORS, "CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.GetReason());
        testStatus = testError;
    }

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::CommandTestStep(%s): %s, status=%s\n", 
        value.c_str(), step.c_str(), testStatus.c_str());

    return(testStatus);
}

//-----------------------------------------------------------------------------
const std::string SoundLevelTc::TestStepSoundPressureLevel(const std::string &value)
{
    std::string     testStatus(testPass);
    int             testDuration, samplePeriod;
    float           minSoundLevel, maxSoundLevel, soundLevel = 0.0;
    std::string     faultTag("NoFault"), faultCode("0000");
    INT32           status = BEP_STATUS_FAILURE;
    m_maxSoundLevel = 0;
    m_minSoundLevel = 999;
    m_soundLevel = 0;
    std::string color("Red");

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::TestStepSoundPressureLevel(%s)\n", value.c_str());

    SystemWrite(GetDataTag("TestResultBox6"),GetDataTag("Fail"));

    GetSoundLevelParameters(minSoundLevel, maxSoundLevel,testDuration,samplePeriod);

    // If the sound thread is already running
    if ( -1 != m_soundThread)
    {
        SetTestStepInfoValue( "Timeout", testDuration);

        UpdatePrompts();

        status = SampleSoundLevel(soundLevel,minSoundLevel,samplePeriod);

        // if there was an error while sampling the sound level or we didn't reach the minimum sound level
        if ((status != BEP_STATUS_SUCCESS) || (soundLevel < minSoundLevel) || ( m_maxSoundLevel > maxSoundLevel))
        {
            Log( LOG_ERRORS, "Sound meter test failed\n");
            faultTag = "SoundPressureLevelFault";
            testStatus = testFail;
        }

        RemovePrompts();
    }
    else
    {
        Log( LOG_ERRORS, "Sound level monitoring thread not running\n");
        faultTag = "SoundPressureLevelFault";
        testStatus = testSoftwareFail;
    }

    // check to make sure an abort did not happen, if so flag it
    if (CheckAbort())
    {
        faultTag = "SoundPressureLevelFault";
        testStatus = testAbort;
    }
    if (testStatus == testFail)
        SystemWrite(GetDataTag("TestResultBox6"),GetDataTag("Fail"));
    else
        SystemWrite(GetDataTag("TestResultBox6"),GetDataTag("Pass"));
    //write sound level value and status to system
    color = testStatus == testPass ? "Green" : "Red";
    SystemWrite(GetDataTag("SoundLevelBGColor"), color);
    SystemWrite(GetDataTag("SoundLevelValue"), soundLevel);
    SendSoundLevelTestResult(testStatus,faultTag,soundLevel,minSoundLevel,maxSoundLevel);

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::TestStepSoundPressureLevel(%s), status=%s\n", 
        value.c_str(), testStatus.c_str());

    return(testStatus);
}

//-----------------------------------------------------------------------------
const std::string SoundLevelTc::TestStepStartSoundTest(const std::string &value)
{
    std::string     testStatus(testFail);
    std::string     faultTag("NoFault");
    char            buff[256];
    int             status;

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::TestStepStartSoundTest(%s)\n", value.c_str());

    UpdatePrompts();
    try
    {
        // If the sound thread is not already running
        if ( -1 == m_soundThread)
        {
            // Clear the flag used to terminate the sound level monitoring thread
            m_soundThreadStop = false;

            // Start the sound level monitoring thread
            status = pthread_create( &m_soundThread, NULL, SoundLevelTc::SoundLevelThread, (void*)this);
            if ( EOK == status)
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
    catch ( BepException &err)
    {
        Log(LOG_ERRORS, "Error during SoundLevelTc::TestStepStartSoundTest: %s\n", err.GetReason());
        testStatus = testFail;
    }
    RemovePrompts();

    // update the test results                                                
    status = SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag),
                                      "ThreadID", CreateMessage(buff, sizeof(buff), "%d", m_soundThread), "");

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::TestStepStartSoundTest(%s), status=%s\n", 
        value.c_str(), testStatus.c_str());

    return(testStatus);
}

const std::string SoundLevelTc::TestStepStopSoundTest(const std::string &value)
{
    std::string     testStatus(testPass);
    std::string     faultTag("NoFault");
    int             status;

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::TestStepStopSoundTest(%s)\n", value.c_str());

    UpdatePrompts();
    try
    {
        // If the sound thread is running
        if ( -1 != m_soundThread)
        {
            // Tell the sound monitoring thread to stop
            status = StopSoundThread();

            // If we successfully stopped the sound level monitoring thread
            if ( EOK == status)
            {
                // If the thread did not encounter any problems
                if ( true == m_soundLevelFaultTag.empty())
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
    catch ( BepException &err)
    {
        Log(LOG_ERRORS, "Error during SoundLevelTc::TestStepStartSoundTest: %s\n", err.GetReason());
        testStatus = testFail;
    }
    RemovePrompts();

    // update the test results                                                
    status = SendTestResult( testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag));

    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::TestStepStopSoundTest(%s), status=%s\n", 
        value.c_str(), testStatus.c_str());

    return(testStatus);
}

const std::string SoundLevelTc::TestStepFinishUp(const std::string &value)
{
    // Close our comm port
    m_soundLevelPort.ClosePort();

    // send the overall test result
    SendOverallResult();

    return(GetOverallResult());
}

//-----------------------------------------------------------------------------
void SoundLevelTc::GetSoundLevelParameters(float &minSoundLevel,float &maxSoundLevel,int &testDuration,int &samplePeriod)
{
    // Read the test parameters
    minSoundLevel = GetParameterFloat("SoundPressureLevelMin");
    maxSoundLevel = GetParameterFloat("SoundPressureLevelMax");
    testDuration = GetParameterInt("SoundPressureLevelDuration");
    samplePeriod = GetParameterInt("SoundPressureLevelSampleInterval");

    // Make sure parameters are sane
    if ( samplePeriod == 0)              samplePeriod = 1000;
    if ( testDuration < samplePeriod)    testDuration = samplePeriod;
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::SampleSoundLevel(float &soundLevel,float minSoundLevel,int samplePeriod)
{
    INT32 status = BEP_STATUS_FAILURE;
    bool minSoundLevelReached = false;

    do
    {
        // The sound level value will be updated by the sound level thread
        soundLevel = m_soundLevel;

        Log( LOG_DEV_DATA, "Sound meter result: Value=%.2f\n", soundLevel);

        // if the sound level is above the minimum, the test passes
        if ( soundLevel > minSoundLevel)
        {
            Log(LOG_DEV_DATA, "Minimum sound level reached\n");
            minSoundLevelReached = true;
            status = BEP_STATUS_SUCCESS;
            if (GetParameterBool("SampleForEntirePeriod"))
            {
                status = StatusSleep( samplePeriod);
            }
        }
        else
        {
            status = StatusSleep( samplePeriod);
        }
        if (status == BEP_STATUS_TIMEOUT && minSoundLevelReached)
        {
            return BEP_STATUS_SUCCESS;
        }
    } while ( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) && 
              (minSoundLevelReached == false || GetParameterBool("SampleForEntirePeriod")));

    return(status);
}

//-----------------------------------------------------------------------------
void SoundLevelTc::SendSoundLevelTestResult(string testStatus,string faultTag,int soundLevel,int minSoundLevel,int maxSoundLevel)
{
    INT32 status;
    char  buff[256];

    status = SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"), GetFaultCode(faultTag),
                                      "SoundPressureLevel", CreateMessage(buff, sizeof(buff), "%d", soundLevel), "dB",
                                      "MinSoundPressureLevel", CreateMessage(buff, sizeof(buff), "%d", minSoundLevel), "dB",
                                      "MaxSoundPressureLevel", CreateMessage(buff, sizeof(buff), "%d", maxSoundLevel), "dB");
    if (BEP_STATUS_SUCCESS != status)
    {
        Log(LOG_ERRORS, "SoundLevelTc Could Not Send Test Result: %s, %s\n", 
            GetTestStepName().c_str(), testStatus.c_str());
    }
}

//-----------------------------------------------------------------------------
int SoundLevelTc::StopSoundThread(void)
{
    int         status = EOK;

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::StopSoundThread()\n");

    // If the sound thread is running
    if ( -1 != m_soundThread)
    {
        uint32_t    samplePeriod = GetParameterInt( "SoundPressureLevelSampleInterval");
        uint64_t    joinTime = mSEC_nSEC( 4*samplePeriod);

        // Tell the sound monitoring thread to stop
        m_soundThreadStop = true;

        // Wait (up to 5 secs) for the thread to stop
        TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_JOIN, NULL, &joinTime, &joinTime);
        status = pthread_join( m_soundThread, NULL);
        if ( EOK != status)
        {
            Log( LOG_ERRORS, "Error stopping the sound level monitoring thread: %s\n", strerror( status));
            pthread_cancel( m_soundThread);
            status = pthread_kill( m_soundThread, SIGKILL);
        }
        m_soundThread = -1;
    }
    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::StopSoundThread(), status=%d\n", status);

    return( status);
}

//-----------------------------------------------------------------------------
void* SoundLevelTc::SoundLevelThread( void* arg)
{
    SoundLevelTc    *soundTc = (SoundLevelTc*)arg;
    int             samplePeriod;
    WaitableTimer   timer;

    soundTc->Log(LOG_FN_ENTRY, "Enter SoundLevelTc::SoundLevelThread()\n");
    if (soundTc->SMProtocolType() == soundTc->GetDataTag("TES1352H"))
    {
        soundTc->SetupTES1352HCommunication();
    }
	else if(soundTc->SMProtocolType() == soundTc->GetDataTag("BK2236"))
	{
		soundTc->SetupBK2236Communication();
	}
    samplePeriod = soundTc->GetParameterInt( "SoundPressureLevelSampleInterval");
    if ( samplePeriod == 0)  samplePeriod = 1000;
    // Start our timer
    timer.Start( samplePeriod);
    // Loop until told to stop
    while ( false == soundTc->m_soundThreadStop)
    {

        try
        {
            // Wait for our timer to fire
            if ( EOK == timer.Wait())
            {

                // Get the sound pressure reading
                if (soundTc->ProcessCommunication() != BEP_STATUS_FAILURE)
                {// check for min and max values
                    if (soundTc->m_soundLevel > soundTc->m_maxSoundLevel) soundTc->m_maxSoundLevel = soundTc->m_soundLevel;
                    if (soundTc->m_soundLevel < soundTc->m_minSoundLevel) soundTc->m_minSoundLevel = soundTc->m_soundLevel;

                    soundTc->Log(LOG_DETAILED_DATA, "Sound meter result: Value=%.2f\n", soundTc->m_soundLevel);
                }
                else
                {//Sound Meter Communications failed
                    soundTc->Log(LOG_ERRORS, "ERROR Communications failed\n");

                }

            }
            else
            {
                BposSleep( samplePeriod);
            }
            soundTc->m_soundLevelPort.ResetConnection();
        }
        catch (BepException &err)
        {
            soundTc->Log(LOG_ERRORS, "Error during sound meter test: %s\n", err.GetReason());
            soundTc->m_soundLevelFaultTag = "SoftwareFailure";
        }
    }
    soundTc->Log(LOG_FN_ENTRY, "Exit SoundLevelTc::SoundLevelThread()\n");

    return( NULL);
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::ProcessCommunication(void)
{
    SerialString_t  response;
    INT32 status = BEP_STATUS_SUCCESS;

    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::ProcessCommunication()");
    if (SMProtocolType() == GetDataTag("HY114"))
    {//use HY114 protocol
        //Clear any buffered data
        m_soundLevelPort.ResetConnection();
        status = HY114Protocol();
    }
    else if (SMProtocolType() == GetDataTag("TES1352H"))
    {//use TES1352 protocol
        //Clear any buffered data
        m_soundLevelPort.ResetConnection();
        status = TES1352HProtocol();
    }
	else if(SMProtocolType() == GetDataTag("BK2236"))
	{
		m_soundLevelPort.ResetConnection();
		status = BK2236Protocol();
	}
    else
    {
        status = ProcessSoundLevelData(response);
        if (status == BEP_STATUS_SUCCESS)
        {// The sound reading will be (actual reading * 10)
            m_soundLevel = m_soundLevel / 10;
        }
    }
    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::ProcessCommunication(), status=%d\n", status);
    return status;
}

//-----------------------------------------------------------------------------
void SoundLevelTc::SetupTES1352HCommunication(void)
{
    SerialString_t response;
    //Send Start Test data byte twice to initiate rapid data transfer
    //note: if left on device always transfers data after first init - always 
    //perform this initialization in case device was reset for any reason
    //Clear any buffered data
    m_soundLevelPort.ResetConnection();
    m_soundLevelPort.WritePort(&StartTestByte(), 1, 5);
    if (CheckForResponse(DataStartByte(), response) == BEP_STATUS_SUCCESS)
    {
        delay(100);
        Log( LOG_DEV_DATA, "Sending Second (any key) byte to receive instantaneous data <$%02X>\n", StartTestByte());
        m_soundLevelPort.WritePort(&StartTestByte(), 1, 5);
        if (CheckForResponse(DataStartByte(), response) == BEP_STATUS_SUCCESS)
        {
            Log( LOG_DEV_DATA, "DataStartByte Received\n");
        }
        else
        {
            Log( LOG_DEV_DATA, "CheckForResponse 2 failed\n");
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "CheckForResponse 1 failed\n");
    }
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::TES1352HProtocol( void )
{
    SerialString_t  response;
    INT32 status = BEP_STATUS_SUCCESS;
    Log( LOG_DEV_DATA, "Processing received data\n");
    if (CheckForResponse(DataStartByte(), response) == BEP_STATUS_SUCCESS)
    {
        //Collect incoming data
        status = ProcessSoundLevelDataTES1352H(response, response.length());
    }  
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
void SoundLevelTc::SetupBK2236Communication(void)
{
	SerialString_t response, msg;
	string message = GetParameter("StartTestCommand");
	Log(LOG_DEV_DATA, "Sending start command to the sound meter: %s", message.c_str());
	for(int index = 0; index < message.length(); index++)
	{
		msg += message[index];
	}
	msg += GetParameterInt("StartTestCommandComplete"); 
	m_soundLevelPort.ResetConnection();
	m_soundLevelPort.WritePort(msg);
	Log(LOG_DEV_DATA, "Sent start command to the sound meter");
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::BK2236Protocol(void)
{
	SerialString_t  response;
	INT32 status = BEP_STATUS_SUCCESS;
	INT32 bytesRead = ReadSerialData(response);
	if(bytesRead > 0)
	{
		status = ProcessSoundLevelData(response, bytesRead);
	}
	else
	{
		status = BEP_STATUS_FAILURE;
	}
	return status;
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::HY114Protocol( void )
{
    SerialString_t  response;
    INT32 status = BEP_STATUS_SUCCESS;
	// Start by resetting the sound meter
	Log( LOG_DEV_DATA, "Sending ResetByte <$%02X>\n", ResetByte());
	m_soundLevelPort.WritePort(&ResetByte(), 1, 5);
	//wait for acknowledgement
	CheckForResponse(AckResponseByte(), response);
    Log( LOG_DEV_DATA, "Sending StartTestByte <$%02X>\n", StartTestByte());
    //should we allow retries?
    m_soundLevelPort.WritePort(&StartTestByte(), 1, 5);
    if (CheckForResponse(AckResponseByte(), response) == BEP_STATUS_SUCCESS)
    {
        if (WaitForTestComplete(response) == BEP_STATUS_SUCCESS)
        {
			if(!GetParameterBool("SkipWaitForTestCompleteByte"))
			{
				Log( LOG_DEV_DATA, "TestCompleteByte Received\n");
			}
			else
			{
				Log(LOG_DEV_DATA, "Not waiting for test complete byte from the sound meter");
			}
            m_soundLevelPort.WritePort(&PermitTransmitByte(), 1, 5);
            if (CheckForResponse(DataStartByte(), response) == BEP_STATUS_SUCCESS)
            {
                Log( LOG_DEV_DATA, "DataStartByte Received\n");
                //Collect incoming data
                status = ProcessSoundLevelData(response, response.length());
                //Send Reset Byte to Sound Meter
                m_soundLevelPort.WritePort(&ResetByte(), 1, 5);
                //wait for acknowledgement
                CheckForResponse(AckResponseByte(), response);
            }
            else
            {
                status = BEP_STATUS_FAILURE;
            }
        }
        else
        {
            status = BEP_STATUS_FAILURE;
        }
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::WaitForTestComplete(SerialString_t &response)
{
	if(!GetParameterBool("SkipWaitForTestCompleteByte"))
	{
		WaitableTimer   timer;
		int samplePeriod = 0;
		int totalTestTime = 0;
		totalTestTime = GetParameterInt("SoundMeterTestTime");
		samplePeriod = GetParameterInt( "SoundTestCompleteSampleInterval");
		if( samplePeriod == 0)	samplePeriod = 500;
		if( totalTestTime == 0)	 totalTestTime = 10000;
		int responseAttempts = totalTestTime / samplePeriod;
		// Start our timer
		timer.Start(samplePeriod);
		// Loop until told to stop
		while(!m_soundThreadStop && responseAttempts > 0)
		{
			if(EOK == timer.Wait())
			{//check to see if response has been sent
				if(CheckForResponse(TestCompleteByte(), response) != BEP_STATUS_FAILURE)
				{
					return BEP_STATUS_SUCCESS;
				}
			}
			else
			{//check if testcomplete was received

				BposSleep(samplePeriod);
			}
			responseAttempts--;
		}
		return BEP_STATUS_FAILURE;
	}
	else
	{
		return BEP_STATUS_SUCCESS;
	}
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::CheckForResponse( UINT8 responseByte, SerialString_t &response)
{
    int             bytesRead;
    if (SMProtocolType() == GetDataTag("TES1352H"))
    {
        bytesRead = ReadSerialDataTES1352H(response);
    }
    else bytesRead = ReadSerialData(response);
    INT32 status = BEP_STATUS_FAILURE;
    if (bytesRead > 0)
    {
        //check if we saw the byte we expected
        if (response[0] == responseByte)
        {
            status = BEP_STATUS_SUCCESS;
        }
    }
    return status;
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::ProcessSoundLevelData(SerialString_t &response, int bytesRead)/*=0*/
{
    bool soundLevelRead = false;
    bool dataStartByteReceived = false;
    char soundLevelBuffer[256];
    UINT16 checksum = 0;
    UINT16 calcChecksum = 0;
    INT32 status = BEP_STATUS_SUCCESS;
    int soundLevelIndex = 0;
    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::ProcessSoundLevelData()");
    do
    {
        if (bytesRead > 0)
        {
            for (int byteIndex = 0; byteIndex < bytesRead; byteIndex++)
            {
                // if the character is a digit or decimal, store the value
                if (((response[byteIndex] >= 0x30) && (response[byteIndex] <= 0x39)) || (response[byteIndex] == 0x2E))
                {
                    Log( LOG_DEV_DATA, "Data Value Recieved\n");
                    soundLevelBuffer[soundLevelIndex] = response[byteIndex];
                    calcChecksum += response[byteIndex];
                    if (soundLevelIndex < 256) soundLevelIndex++;
                }
                // if the character is a return, store the reading
                else if (response[byteIndex] == DataEndByte())
                {
                    Log( LOG_DEV_DATA, "DataEndByte Recieved\n");
                    soundLevelBuffer[soundLevelIndex] = 0;
                    if (UseChecksum())
                    {
                        soundLevelRead = (checksum == calcChecksum);
                        Log( LOG_DEV_DATA, "CheckSum Value: %d Calculated Checksum: %d match: %s \n", checksum, calcChecksum,
                             soundLevelRead ? "true" : "false");
                        status = (soundLevelRead) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
                        Log(LOG_DEV_DATA, "Checksum Received:%d Checksum Calculated: %d \n", checksum, calcChecksum);
                    }
                    else
                    {
                        soundLevelRead = true;
                    }
                    if (soundLevelRead)
                    {
                        m_soundLevel = atof(soundLevelBuffer);
                    }
                }
                else if (response[byteIndex] == DataStartByte() && !dataStartByteReceived)
                {//Start Byte received ignore
                    dataStartByteReceived = true;
                    Log( LOG_DEV_DATA, "Data Start Byte Recieved\n");
                }
                else
                {//Unknown Data
                    Log(LOG_DEV_DATA, "Data Byte is not an Ascii Digit or decimal Value: %<$%02X>", response[byteIndex]);
                    if (UseChecksum())
                    {//This could possibly be a checksum byte
                        if (byteIndex == GetParameterInt("CheckSumHighMSBIndex"))
                        {
                            checksum += (response[byteIndex] << 8);
                            status = BEP_STATUS_SUCCESS;
                        }
                        else if (byteIndex == GetParameterInt("CheckSumLSBIndex"))
                        {
                            checksum += response[byteIndex];
                            status = BEP_STATUS_SUCCESS;
                        }
                        else
                            status = BEP_STATUS_FAILURE;
                    }
                    else status = BEP_STATUS_FAILURE;
                }
            }
            if (!soundLevelRead && status == BEP_STATUS_SUCCESS)
            {//attempt to read data
                bytesRead = ReadSerialData(response);
            }
        }
        else
        {//No data read, attempt to read
            bytesRead = ReadSerialData(response);
        }
    } while ((status == BEP_STATUS_SUCCESS) && (bytesRead > 0) && !soundLevelRead);
    if (!soundLevelRead)
    {
        status = BEP_STATUS_FAILURE;
    }
    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::ProcessSoundLevelData(), status=%d\n", status);
    return status;
}

//-----------------------------------------------------------------------------
INT32 SoundLevelTc::ProcessSoundLevelDataTES1352H(SerialString_t &response, int bytesRead)/*=0*/
{
    Log(LOG_FN_ENTRY, "Enter SoundLevelTc::ProcessSoundLevelDataTES1352H()");
    INT32 status = BEP_STATUS_SUCCESS;
    if (response.length() >= (unsigned int)GetParameterInt("ValidResponseLength"))
    {//check for end byte
        if (response[GetParameterInt("ValidResponseLength") - 1] == DataEndByte())
        {//end byte valid get data
            UINT8 highByte =  response[GetParameterInt("DataHighByteIndex")];
            UINT8 lowByte =  response[GetParameterInt("DataLowByteIndex")];
            m_soundLevel = (((float)(highByte << 8) + lowByte)) / 10;
        }
        else
        {
            Log(LOG_ERRORS, "Data End Byte incorrect\n");
            status = BEP_STATUS_FAILURE;
        }
    }
    else
    {
        Log(LOG_ERRORS, "Response Length Incorrect Length: %d Expected: %d\n", response.length(), GetParameterInt("ValidResponseLength"));
        status = BEP_STATUS_FAILURE;
    }
    Log(LOG_FN_ENTRY, "Exit SoundLevelTc::ProcessSoundLevelData(), status=%d\n", status);
    return status;
}

//-----------------------------------------------------------------------------
int SoundLevelTc::ReadSerialData(SerialString_t &response)
{
    int bytesRead = -1;
    INT32 timeoutTime = GetParameterInt("SerialTimeoutTime");
    // Set delay in do-while loop using BposSleep() function call
    int delay = GetParameterInt("SerialDelayMSec");
    // Start the timer
    SetSecondaryStartTime();
    do
    {
        response.clear();
        bytesRead = m_soundLevelPort.ReadPort(response,1,100);
        if (bytesRead > 0)
        {
            for (INT16 ii = 0; ii < bytesRead; ii++)
            {
                Log(LOG_DETAILED_DATA,"Byte %02d:  <$%02X>\n", ii, response[ii]);
            }
            return bytesRead;
        }
        Log(LOG_DETAILED_DATA,"Bytes Read:  %d\n", bytesRead);
        BposSleep(delay);
    } while (SecondaryTimeRemaining(&timeoutTime) && !m_soundThreadStop);
    return bytesRead;
}

//-----------------------------------------------------------------------------
int SoundLevelTc::ReadSerialDataTES1352H(SerialString_t &response)
{
    int bytesRead = -1;
    INT32 timeoutTime = GetParameterInt("SerialTimeoutTime");
    // Set delay in do-while loop using BposSleep() function call
    int delay = GetParameterInt("SerialDelayMSec");
    // Start the timer
    SetSecondaryStartTime();
    do
    {
        response.clear();
        bytesRead = m_soundLevelPort.ReadPort(response,1,100);
        if (bytesRead > 0)
        {
            for (INT16 ii = 0; ii < bytesRead; ii++)
            {
                Log(LOG_DETAILED_DATA,"Byte %02d:  <$%02X>\n", ii, response[ii]);
            }
        }
        Log(LOG_DETAILED_DATA,"Bytes Read:  %d\n", bytesRead);
        if (bytesRead < GetParameterInt("ValidResponseLength"))
        {
            BposSleep(delay);
        }
    } while (SecondaryTimeRemaining(&timeoutTime) && !m_soundThreadStop && 
             (bytesRead < GetParameterInt("ValidResponseLength")));
    return bytesRead;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::StartTestByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_startByte = *byte;
    return m_startByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::TestCompleteByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_testCompleteByte = *byte;
    return m_testCompleteByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::PermitTransmitByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_permitTransmitByte = *byte;
    return m_permitTransmitByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::ResetByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_resetByte = *byte;
    return m_resetByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::AckResponseByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_ackResponseByte = *byte;
    return m_ackResponseByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::DataEndByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_dataEndByte = *byte;
    return m_dataEndByte;
}

//-----------------------------------------------------------------------------
inline const UINT8& SoundLevelTc::DataStartByte(UINT8 *byte) /* = NULL*/
{
    if (byte != NULL)  m_dataStartByte = *byte;
    return m_dataStartByte;
}

//-----------------------------------------------------------------------------
inline const bool& SoundLevelTc::UseChecksum(bool *useChecksum) /* = NULL*/
{
    if (useChecksum != NULL)  m_useChecksum = *useChecksum;
    return m_useChecksum;
}

//-----------------------------------------------------------------------------
inline const string SoundLevelTc::SMProtocolType(string protocolName) /* = NULL*/
{
    if (protocolName != "")  m_protocol = protocolName;
    return m_protocol;
}






