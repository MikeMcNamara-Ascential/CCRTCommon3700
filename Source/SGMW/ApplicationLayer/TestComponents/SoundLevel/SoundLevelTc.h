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
#ifndef SoundLevelTc_h
#define SoundLevelTc_h

#include "GenericTC.h"
#include "ILogicalPort.h"

//==============================================================================
                
/**
 * SoundLevelTc class that is responsible for performing any tests that require
 * a sound reading.
 */
class SoundLevelTc : public GenericTC
{
    friend class SoundLevelTcTest;

public:
	/**
	 * Class constructor.
	 */
    SoundLevelTc();

	/**
	 * Class Destructor.
	 */
    virtual ~SoundLevelTc();

	/**
	 * Initialization method to set up the SoundLevelTc component.
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Perform any test component specific initialization.
     * Set up communication parameters 
	 * <br><p>
	 *
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GenericTC::Initialize() </li>
	 * 		<li> GetParameter() </li>
	 * 		<li> FixUpPathString </li>
	 * 		<li> SMProtocolType() </li>
	 * 		<li> GetParameterBool() </li>
	 * 		<li> UseChecksum() </li>
	 * 		<li> StartTestByte() </li>
	 * 		<li> TestCompleteByte() </li>
	 * 		<li> PermitTransmitByte() </li>
	 * 		<li> ResetByte() </li>
	 * 		<li> AckResponseByte() </li>
	 * 		<li> DataEndByte() </li>
	 * 		<li> DataStartByte() </li>
	 * 		<li> InitializationComplete() </li>
	 * 		</ul>
	 * 
	 * @param config Configuration file to use for initialization.
	 */
    void Initialize(const XmlNode *config);

    /**
     * Reset data members that will get 'set' from test steps
     *
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GenericTC::WarmInitialize() </li>
	 * 		</ul>
     * 
     */
    void WarmInitialize( void );

    /**
     * Stop Sound Meter thread when current test ends before calling the 
     * base class
     *
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> StopSoundThread </li>
	 * 		<li> GenericTC::Publish() </li>
	 * 		</ul>
     * @param node    Tag and data that was subscribed for
     * 
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const std::string Publish(const XmlNode *node);
    
protected:

	/**
	 * Sequence a test step.
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * This is the method that will handle the execution of each test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded to execute the component tests.
	 * <br><p>
	 * 
	 * <b>Test Step Functions Called:</b>
	 * 		<ul>
	 * 		<li> TestStepStartSoundTest() </li>
	 * 		<li> TestStepStopSoundTest() </li>
	 * 		<li> TestStepSoundPressureLevel() </li>
	 * 		<li> TestStepFinishUp() </li>
	 * 		</ul>
	 * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetTestStepName() </li>
	 * 		<li> GenericTC::CommandTestStep() </li>
	 *		</ul>
	 * 
	 * @param value    This is the value that was commanded by the test sequencer.
	 * @return  The result of the test step that was executed.
	 */
    virtual const std::string CommandTestStep(const std::string &value);
    
    /**
     * This test step will check the sound pressure level from the sound level 
     * meter.  If the sound pressure read is less than the minimum allowed value,
     * the test fails.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetSoundLevelParameters() </li>
	 * 		<li> SetTestStepInfoValue() </li>
	 * 		<li> UpdatePrompts() </li>
	 * 		<li> SampleSoundLevel() </li>
	 * 		<li> RemovePrompts() </li>
	 * 		<li> CheckAbort() </li>
	 * 		<li> SendSoundLevelTestResult() </li>
	 *		</ul>
     * @param value  This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    virtual const std::string TestStepSoundPressureLevel(const std::string &value);

    /**
     * Test step is used to start the sound level monitoring thread.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> UpdatePrompts() </li>
	 * 		<li> pthread_create() </li>
	 * 		<li> RemovePrompts() </li>
	 * 		<li> SendTestResultWithDetail() </li>
	 *		</ul>
     * 
     * @param value  This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    virtual const std::string TestStepStartSoundTest(const std::string &value);

    /**
     * Test step is used to stop the sound level monitoring thread.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> UpdatePrompts() </li>
	 * 		<li> StopSoundThread() </li>
	 * 		<li> RemovePrompts() </li>
	 * 		<li> SendTestResult() </li>
	 *		</ul>
     * 
     * @param value  This is the value that was commanded by the test sequencer.
     * 
     * @return The status of the test step.
     */
    virtual const std::string TestStepStopSoundTest(const std::string &value);
    
    /**
     * Finishup test step that is called as the last step for the component.
     * Close serial port and send overall result
     *
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> ClosePort() </li>
	 * 		<li> SendOverallResult() </li>
	 * 		<li> GetOverallResult() </li>
	 *		</ul>
     * 
     * @param value This is the value that was commanded by the test sequencer.
     * 
     * @return The result of the test.
     */
    virtual const std::string TestStepFinishUp(const std::string &value);
    
private:

    /**
     * Get the parameters needed for the sound leve test.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetParameterFloat() </li>
	 * 		<li> GetParameterInt() </li>
	 *		</ul>
     * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> SoundPressureLevelMin</li> 
	 * 		<li> SoundPressureLevelDuration</li> 
	 * 		<li> SoundPressureLevelSampleInterval</li> 
	 * 		</ul>
     * 
     * @param minSoundLevel The minimum allowable sound level.
     * @param testDuration  The amount of time to sample the sound level.
     * @param samplePeriod  The amount of time between sound level samples.
     */ 
    void GetSoundLevelParameters(float &minSoundLevel,float &maxSoundLevel,int &testDuration,int &samplePeriod);

    /**
     * Sample the sound level.  For the test to pass, the sound level needs to
     * reach the minimum level during the test duration.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> StatusSleep() </li>
	 * 		<li> TimeRemaining() </li>
	 *		</ul>
     * 
     * @param soundLevel    The sampled sound level.
     * @param minSoundLevel The minimum allowable sound level.
     * @param samplePeriod  The amount of time between sound level samples.
     * 
     */ 
    INT32 SampleSoundLevel(float &soundLevel,float minSoundLevel,int samplePeriod);

    /**
     * Send the sound level test result to the test result server.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> SendTestResultWithDetail() </li>
	 *		</ul>
     * 
     * @param testStatus    The status of the test.
     * @param faultTag      The fault tag for the test result.
     * @param soundLevel    The sound level test value.
     * @param minSoundLevel The minimum allowable sound level.
     * 
     * @return null
     */ 
    void SendSoundLevelTestResult(string testStatus,string faultTag,int soundLevel,int minSoundLevel,int maxSoundLevel);

    /**
     * Method used to shutdown the sound level monitoring thread.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> TimerTimeout() </li>
	 * 		<li> pthread_join() </li>
	 * 		<li> pthread_cancel() </li>
	 * 		<li> pthread_kill() </li>
	 *		</ul>
     * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> SoundPressureLevelSampleInterval - The amount of time between sound level samples</li> 
	 * 		</ul>
     * 
     * @return EOK if the thread was stopped (or was not running)
     */
    int StopSoundThread();

    /**
     * Thread used to monitor the sound pressure level during the test.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> ProcessCommunication() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> ResetConnection() </li>
	 *		</ul>
     * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> SoundPressureLevelSampleInterval - The amount of time between sound level samples</li> 
	 * 		</ul>
     * 
     * @param arg Pointer to the parent SoundLevelTc object
     * 
     * @return NULL
     */
    static void* SoundLevelThread(void* arg);
                    
    /**
     * Select appropriate sound level protocol and process 
     * communications accordingly.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> SMProtocolType() </li>
	 * 		<li> GetDataTag() </li>
	 * 		<li> HY114Protocol() </li>
	 * 		<li> ProcessSoundLevelData() </li>
	 *		</ul>
     * 
	 * <b>Data Tags:</b>
	 * 		<ul>
	 * 		<li> HY114 - HY114 protocol string</li> 
	 * 		</ul>
     * 
     * @return The status of the operation.
     */
    INT32 ProcessCommunication(void);

    /**
     * Handle communications with TES1352 sound meter.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> CheckForResponse() </li>
	 * 		<li> ProcessSoundLevelDataTES1352() </li>
	 *		</ul>
     * 
	 * <b>Data Tags:</b>
	 * 		<ul>
	 * 		<li> TES1352 - TES1352 protocol string</li> 
	 * 		</ul>
     * 
     * @return The status of the operation.
     */
    INT32 TES1352HProtocol(void);
    void SetupTES1352HCommunication(void);

	void SetupBK2236Communication(void);
	INT32 BK2236Protocol(void);

    /**
     * Handle communications with HY114 sound meter.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> SMProtocolType() </li>
	 * 		<li> GetDataTag() </li>
	 * 		<li> HY114Protocol() </li>
	 * 		<li> ProcessSoundLevelData() </li>
	 *		</ul>
     * 
	 * <b>Data Tags:</b>
	 * 		<ul>
	 * 		<li> HY114 - HY114 protocol string</li> 
	 * 		</ul>
     * 
     * @return The status of the operation.
     */
    INT32 HY114Protocol(void);

    /**
     * Compares serial data with expected respons.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> ReadSerialData() </li>
	 *		</ul>
     * 
     * @param responseByte Expected response from serial read.
     * @param response serial data read.
     * 
     * @return The status of the operation.
     */
    INT32 CheckForResponse(UINT8 responseByte, SerialString_t &response);

    /**
     * Compares serial data with expected respons.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> CheckForResponse() </li>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> TestCompleteByte() </li>
	 * 		<li> BposSleep() </li>
	 *		</ul>
     * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> SoundMeterTestTime</li> 
	 * 		<li> SoundPressureLevelDuration</li> 
	 * 		<li> SoundTestCompleteSampleInterval</li> 
	 * 		</ul>
     * 
     * @param response serial data read.
     * 
     * @return The status of the operation.
     */
    INT32 WaitForTestComplete(SerialString_t &response);

    /**
     * Compares serial data with expected respons.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> DataEndByte() </li>
	 * 		<li> UseChecksum() </li>
	 * 		<li> DataStartByte() </li>
	 *		</ul>
     * 
     * @param bytes read number of bytes read from the serial read.
     * @param response serial data read.
     * 
     * @return The status of the operation.
     */
    INT32 ProcessSoundLevelData(SerialString_t &response, int bytesRead = 0);

    /**
     * Compares serial data with expected respons.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> DataEndByte() </li>
	 *		</ul>
     * 
     * @param bytes read number of bytes read from the serial read.
     * @param response serial data read.
     * 
     * @return The status of the operation.
     */
    INT32 ProcessSoundLevelDataTES1352H(SerialString_t &response, int bytesRead = 0);

    /**
     * Compares serial data with expected respons.
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
	 * 		<li> GetParameterInt() </li>
	 * 		<li> SetSecondaryStartTime() </li>
	 * 		<li> clear() </li>
	 * 		<li> ReadPort() </li>
	 * 		<li> BposSleep() </li>
	 * 		<li> SecondaryTimeRemaining() </li>
	 *		</ul>
     * 
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
	 * 		<li> SerialTimeoutTime</li> 
	 * 		<li> SerialDelayMSec</li> 
	 * 		</ul>
     * 
     * @param response location to store serial data read.
     * 
     * @return The status of the operation.
     */
    int ReadSerialData(SerialString_t &response);
    int ReadSerialDataTES1352H(SerialString_t &response);

    /**
     * Logical Port interface used to read data from the sound level meter.
     */
    ILogicalPort    m_soundLevelPort;

    /**
     * Thread ID of the sound pressure level monitoring thread.
     */
    pthread_t       m_soundThread;

    /**
     * Flag used to tell the sound level thread to stop.
     */
    volatile bool   m_soundThreadStop;

    /**
     * Maximum recorded sound level.
     */
    volatile float  m_maxSoundLevel;

    /**
     * Minimum recorded sound level.
     */
    volatile float  m_minSoundLevel;

    /**
     * Current sound level.
     */
    volatile float  m_soundLevel;

    /**
     * String indicating the sound meter protocol being used.
     */
    string m_protocol;
    /**
     * Fault tag recorded by the sound level thread.
     */
    std::string     m_soundLevelFaultTag;
    /**
     * Start byte command.
     */
    UINT8 m_startByte;
    /**
     * Test complete byte command.
     */
    UINT8 m_testCompleteByte;
    /**
     * Permit Transmit byte command.
     */
    UINT8 m_permitTransmitByte;
    /**
     * Reset byte command.
     */
    UINT8 m_resetByte;
    /**
     * Ack Response byte command.
     */
    UINT8 m_ackResponseByte;
    /**
     * Data End byte command.
     */
    UINT8 m_dataEndByte;
    /**
     * Data Start byte command.
     */
    UINT8 m_dataStartByte;
    /**
     * Flag indicating to verify checksum.
     */
    bool m_useChecksum;
    /**
     * Get / Set Command Value.
     */
    const UINT8& StartTestByte(UINT8 *byte = NULL);
    /**
     * Get / Set Command Value.
     */
    const UINT8& TestCompleteByte(UINT8 *byte = NULL);
    /**
     * Get / Set Command Value.
     */
    const UINT8& PermitTransmitByte(UINT8 *byte = NULL);
    /**
     * Get / Set Command Value.
     */
    const UINT8& ResetByte(UINT8 *byte = NULL); 
    /**
     * Get / Set Command Value.
     */
    const UINT8& AckResponseByte(UINT8 *byte = NULL); 
    /**
     * Get / Set Command Value.
     */
    const UINT8& DataEndByte(UINT8 *byte = NULL); 
    /**
     * Get / Set Command Value.
     */
    const UINT8& DataStartByte(UINT8 *byte = NULL); 
    /**
     * Get / Set m_useChecksum flag.
     */
    const bool& UseChecksum(bool *useChecksum = NULL);
    /**
     * Get / Set Sound Meter Protocol type.
     */
    const string SMProtocolType(string protocolName = "");
};

#endif  // SoundLevelTc_h
