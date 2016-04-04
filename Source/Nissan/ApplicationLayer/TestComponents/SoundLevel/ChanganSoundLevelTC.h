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
//    $Log: /GM/Source/Changan/ApplicationLayer/TestComponents/SoundLevel/ChanganSoundLevelTC.h $
// 
// 5     6/27/11  2:17p Rsmith
// Took ChanganSoundLevelTC from TataSoundLevelTC
//
// 4     3/07/06 12:37a Bmeinke
// Added idle and running sound level tests
// 
// 3     3/06/06 3:39p Bmeinke
// Added new test steps for the Horn test at idel and while the vehicle is
// at speed
// 
// 2     3/01/06 1:01a Bmeinke
// Added TestStepFinishUp()
// 
// 1     2/28/06 2:03p Bmeinke
// Seperated from TataMachineTC
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _CHANGAN_SOUND_LEVEL_TC_H_INCLUDED_
#define _CHANGAN_SOUND_LEVEL_TC_H_INCLUDED_

#include "GenericTC.h"
#include "ILogicalPort.h"

//==============================================================================
                
/**
 * ChanganSoundLevelTC class that is responsible for performing
 * machine based responsibilities during a vehicle test.
 */
class ChanganSoundLevelTC : public GenericTC
{
public:
	/**
	 * Changan Machine Component Constructor.
	 */
    ChanganSoundLevelTC();
	/**
	 * Changan Machine Component Destructor.
	 */
    virtual ~ChanganSoundLevelTC();

	/**
	 * Initialization method to set up the MachineTC
	 * component.
	 *
	 * @param configFile Configuration file to use for initialization.
     * @since Version 1.0
	 */
    void Initialize(const XmlNode *config);

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 2.1
	 */
	virtual const std::string Publish(const XmlNode *node);
	
protected:
	/**
     * This is the method that will handle the execution of each
     * test step that has been specified by the system for each
     * test objective that is passed to it.
     * 
     * @param value    This is the value that was commanded by the test sequencer.
     * @return The status of the overall objective.
     */
    virtual const std::string CommandTestStep(const std::string &value);
	
/* ************************************************ */
/* Everything Below Here Is Component Specific. 	*/
/* ************************************************ */

	/**
	 * This test step will check the sound pressure level from the sound level meter
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepSoundPressureLevel(const std::string &value);

	/**
	 * This test step will check the sound pressure level for the horn with the
	 * vehicle at idle
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepIdleHornTest(const std::string &value);
	
	/**
	 * This test step will check the sound pressure level for the horn with the
	 * vehicle running
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepRunningHornTest(const std::string &value);
	
	/**
	 * Test step used to start the sound level monitoring thread
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepStartSoundTest(const std::string &value);

	/**
	 * Test step used to stop the sound level monitoring thread
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepStopSoundTest(const std::string &value);
	
	/**
	 * This test step will reset the sound pressure meter
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepResetSoundMeter(const std::string &value);
	
	/**
	 * Sets the filter frequency for the sound pressure level readings
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepSoundMeterFilterFreq(const std::string &value);
	
	/**
	 * Set the frequency weighting for sound pressure level readings (A,B,C)
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepSoundMeterFreqWeighting(const std::string &value);
	
	/**
	 * Set the time weighting for sound pressure level readings (fast, slow, integral)
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepSoundMeterTimeWeighting(const std::string &value);
	
	/**
	 * Read the battery voltage of the sound pressure meter
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepReadSoundMeterBatteryVoltage(const std::string &value);
	
	/**
	 * Read the internal temperature of the sound pressure meter
	 * 
	 * @param value  This is the value that was commanded by the test sequencer.
	 * 
	 * @return The status of the test step.
	 */
	virtual const std::string TestStepReadSoundMeterInternalTemp(const std::string &value);

	/**
	 * Finishup test step that always returns overall as pass.
	 *
	 * @param value    This is the value that was commanded by the test sequencer.
	 * @return The result of the test.
	 */
	virtual const std::string TestStepFinishUp(const std::string &value);
	
private:
	/**
	 * Method used to shutdown the sound level monitoring thread
	 * 
	 * @return EOK if the thread was stopped (or was not running)
	 */
	int StopSoundThread();

	/**
	 * Thread used to monitor the sound pressure level during the test
	 * 
	 * @param arg    Pointer to the parent ChanganSoundLevelTC object
	 * 
	 * @return NULL
	 */
	static void* SoundLevelThread( void* arg);
	
	/**
	 * Logical Port interface used to read data from the sound level meter
	 */
	ILogicalPort	m_soundLevelPort;

	/**
	 * Thread ID of the sound pressure level monitoring thread
	 */
	pthread_t		m_soundThread;

	/**
	 * Flag used to tell the sound level thread to stop
	 */
	volatile bool	m_soundThreadStop;

	/**
	 * Maximum recorded sound level
	 */
	volatile float	m_maxSoundLevel;

	/**
	 * Maximum recorded sound level
	 */
	volatile float	m_minSoundLevel;

	/**
	 * Current sound level
	 */
	volatile float	m_soundLevel;

	/**
	 * Fault tag recorded by the sound level thread
	 */
	std::string		m_soundLevelFaultTag;
};

#endif	// _CHANGAN_SOUND_LEVEL_TC_H_INCLUDED_
