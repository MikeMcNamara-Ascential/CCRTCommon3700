//******************************************************************************
// Description:
// Test steps specific to Isuzu Emissions testing.
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
#ifndef IsuzuEmissionsTc_h
#define IsuzuEmissionsTc_h
//-------------------------------------------------------------------------------------------------
#include "BepTimer.h"
#include "GenericEmissionsTCTemplate.cpp"
#include "BackgroundSwitchMonitor.cpp"
#include "BackgroundRangeCheckMonitor.cpp"
#include <vector>

//class XmlNodeMap;

//-------------------------------------------------------------------------------------------------
#define ISUZU_EMISSIONS_PULSE_CODE  SYSTEM_PULSE+7
#define SWITCH_MON_PULSE            10
#define SENSOR_MON_PULSE            11
//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class IsuzuEmissionsTc : public GenericEmissionsTCTemplate<ModuleType>
{
public:
    IsuzuEmissionsTc();
    virtual ~IsuzuEmissionsTc();

    /**
     * Execute the specified test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the
     * system for each test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * @param value  The value that was commanded by the test sequencer.
     *
     * @return The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * <b>Description:</b>
     * Handle the specified pulse.  This method simply calls 
     * BepComponent::HandlePulse().
     *
     * @param code  The pulse code to process.
     * @param value The pulse value to process.
     * 
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);


protected:

    /**
     * Perform GenericEmissionsTCTemplate intitialization.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method overloads the base class to allow the GenericEmissionsTCTemplate test component to initialize any
     * data sepcific to its needs.
     * <p>
     * 
     * @param config The configuration data to use for initializing the test component.
     */
    virtual void InitializeHook(const XmlNode *config);


private:
    /**
     * Prompt the operator to turn on the Air condtioning.
     * The air conditioner compressor status will then be checked to make sure it has been commanded on and all self tests have 
     * completed successfully.
     * 
     * @return Result of A/C Compressor testing.
     */
    string CheckAirConditionCompressor(void);

    /**
     * Read the current fault count from the module.
     * If any faults are present, the test may be aborted depending on parameter settings.
     * 
     * @return Result of check the module for faults.
     */
    string CheckFaultCount(void);

    /**
     * Verify the sensor reading is in the proper range.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeBool(const string &sensorName);

    /**
     * Check the specified sensor to make sure it is in the proper range.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeFloat(const string &sensorName);

    /**
     * Verify the sensor reading is in the proper range.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeInt(const string &sensorName);

    /**
     * Verify the sensor reading is in the proper range.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeUint8(const string &sensorName);

    /**
     * Verify the sensor reading is in the proper range.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeUint16(const string &sensorName);

    /**
     * The vehicle speed sensor will be read from the module and compared to the drive axle roller speed.
     * The test will pass if the vehicle speed sensor is within the specified tolerance.
     * 
     * @return Result of validating the vehicle speed sensor.
     */
    string CheckVehicleSpeedSensor(void);

    /**
     * Validate accelerator pedal reading at WOT (Wide open Throttle).
     * 
     * @return Result of WOT test.
     */
    string AcceleratorPedalWOTTest();

    /**
     * Verify the engine fan functions properly.
     * 
     * @return Result of engine fan test.
     */
    string CheckEngineFan();

    /**
     * Prompt the operator to move the shift lever to the different positions and verify the module indicates the correct position.
     * 
     * @return Result of checking the PRNDL positioning.
     */
    string PrndlPositionCheck(void);

    /**
     * Read the specified sensor and verify it is in the proper range.
     * 
     * @param sensorName Sensor to read from the module.
     * @param sensorValue
     *                   Sensor value read from the module.
     * 
     * @return Result of verifying the sensor is in the proper range.
     */
    string ReadAndVerifySensor(string &sensorName, UINT8 &sensorValue);

    /**
     * Read the specified sensor and verify it is in the proper range.
     * 
     * @param sensorName Sensor to read from the module.
     * @param sensorValue
     *                   Sensor value read from the module.
     * 
     * @return Result of verifying the sensor is in the proper range.
     */
    string ReadAndVerifySensor(string &sensorName, float &sensorValue);

    /**
     * Verify CASE learn has completed successfully.
     * 
     * @return Result of CASE learn procedure.
     */
    string CheckCASELearn();

    /**
     * Read the current switch states from the module.
     * Each configured switch monitor item will be read if both switch states have not been detected.
     * If all switches have been detected in both states, the timer will be stopped.
     * 
     * @return Status of monitoring the configured switches.
     */
    BEP_STATUS_TYPE ReadCurrentSwitchStates(void);

    /**
     * Read the current engine temperature.
     * 
     * @param engineTemp Current engine temperature.  (-1 if the temperature could not be read).
     * 
     * @return Status of reading the engine temperature.
     */
    BEP_STATUS_TYPE ReadEngineTemperature(INT32 &engineTemp);

    /**
     * Read the current Switch State.
     * 
     * @param switchPid Current name of pid correlating to module interface read command. 
     * 
     * @return Status of reading the switch state.
     */
    BEP_STATUS_TYPE ReadIsSwitchOn(const string &switchPidName, bool &isSwitchOn);

    /**
     * Read the current Mode of a switched state.
     * 
     * @param modePid Current name of pid correlating to module interface read command. 
     * 
     * @return Status of reading the switch stated mode.
     */
    BEP_STATUS_TYPE ReadIsModeOn(const string &modePidName, bool &isModeOn);

    /**
     * Check if the engine is running.
     * 
     * @param isEngineRunning
     *               Flag indicating if the engine RPM is above the minimum idle RPM.
     * 
     * @return Status of reading the engine RPM from the module.
     */
    BEP_STATUS_TYPE ReadIsEngineRunning(bool &isEngineRunning);
    
    /**
     * Setup the sensor monitor timer.
     * 
     * @param config Sensor monitor timer data.
     */
    void SetupSensorMonitorTimer(const XmlNode *config);

    /**
     * Setup the switch monitor timer.
     * 
     * @param config Switch monitor timer data.
     */
    void SetupSwitchMonitorTimer(const XmlNode *config);

    /**
     * Command the module to begin the CASE learn process.
     * 
     * @return Result of commanding the module to begin the CASE learn procedure.
     */
    string StartCaseLearnSequence();

    /**
     * Start monitoring all configured switches.
     * The configuration data will specify if a specific switch should be monitored.  For example, if the 
     * vehicle is equipped with an automatic transmission, the clutch switch should not be monitored.
     * 
     * @return Result of starting the switch monitors.
     */
    string StartSwitchMonitors(void);

    /**
     * Stop the switch monitors.
     * Switch positions for each monitor will be checked to make sure both On and Off states were observed. 
     * 
     * @return Result of stopping and checking the results of each switch monitor.
     */
    string StopSwitchMonitors(void);

    string StartRangeCheckMonitors();
    string StopRangeCheckMonitors();

    /**
     * Monitor the engine temperature until it reaches proper operating temperature.
     * 
     * @return Result of waiting for engine temperature.
     */
    string WaitForOperatingTemperature(void);

    /**
     * Verify the fuel trim levels.
     * 
     * @param checkAtIdle
     *               Flag to determine if the fuel trim should be checked at an idle state.
     * 
     * @return Result of verifying the fuel trim levels.
     */
    string CheckFuelTrim(const bool &checkAtIdle);

    /**
     * Check the specified sensor to make sure it is in the proper range but first verify at idle rpm.
     * 
     * @param sensorName Name of the sensor to check.
     * 
     * @return Result of verifying the sensor is in the correct range.
     */
    string CheckSensorRangeAtIdleFloat(const string &sensorName);

    string CheckNeutralRPMOverspeed();

    virtual string ReadFaults(void);

    /**
     * Simple delay before reading the DTCs of the emission.
     * 
     * @return Result of the function (skipped or passed)
     */
    virtual string DelayBeforeDtcRead(void);

    /**
     * Checks the oxygen sensor for valid values. This test can take quite a bit of time
     * depending on the number of samples read and the time delayed in between reading
     * the sensors again.
     * 
     * @return Result of the test (pass, fail, skipped)
     */
    virtual string CheckOxygenSensors(void);

    /**
     * This method gets the limits/parameters for the O2 sensors
     * 
     * @param O2Sensor - the name of the O2 sensor that is used in the .xml file
     * @param limits[] - the array of band limits that will be populated with the correct limits
     *                  (Low Band Limit, Open Band Lower Limit, Open Band Upper Limit, and High Band Limit}
     * @return void
     */
    virtual void GetO2SensorParams(string O2Sensor, float limits[]);

    /**
     * This method reads the O2 sensor data and processes it by counting which band it is
     * in and checking if a band switch occurred.
     * 
     * @param O2Name - the name of the O2 sensor that is used in the .xml file
     * @param limits[] - the band limits of the O2 sensor
     *                  (Low Band Limit, Open Band Lower Limit, Open Band Upper Limit, and High Band Limit}
     * @param counts[] - array of the band counts and band switch count
     *                  {Low Band Count, Open Band Count, High Band Count, Band Switch Count}
     * @param &lastBand - pointer to the string that is the last band that the O2 sensor was in
     * @return the result of reading the O2 sensor
     */
    virtual BEP_STATUS_TYPE ReadAndProcessO2Sensor(string O2Name, float limits[], int counts[], string &lastBand);

    /**
     * This method reads the O2 sensor data and processes it by placing values in the array
     * 
     * @param O2Name - the name of the O2 sensor that is used in the .xml file
     * @param FLVals[] - the first and last values of the O2 sensor
     *                  (First Value, Last Value}
     * @param sampleNum - the sample number that we are on
     * @return the result of reading the O2 sensor
     */
    virtual BEP_STATUS_TYPE ReadAndProcessO2Sensor(string O2Name, float FLVals[], int sampleNum);

    /**
     * This method analyzes the O2 sensor data (checks if the sensor has passed or failed).
     * 
     * @param O2Name - the name of the O2 sensor that is used in the .xml file
     * @param counts[] - array of the band counts and band switch count
     *                  {Low Band Count, Open Band Count, High Band Count, Band Switch Count}
     * @return the result of the O2 sensor analysis (pass or fail)
     */
    virtual string AnalyzeO2SensorData(string O2Name, int counts[]);

    /**
     * This method analyzes the O2 sensor delta (checks if the sensor has passed or failed).
     * 
     * @param O2Name - the name of the O2 sensor that is used in the .xml file
     * @param FLVals[] - the first and last values of the O2 sensor
     *                  (First Value, Last Value}
     * @return the result of the O2 sensor analysis (pass or fail)
     */
    virtual string AnalyzeO2SensorDelta(string O2Name, float FLVals[]);

    /**
     * Checks for a locked module before trying to clear faults
     * 
     * @return Result of clearing the faults from the module
     */
    virtual string ClearFaults(void);

	/**
     * Sends a command to the Module to clear the MAF Learned Bit
     * 
     * @return Result of clearing the faults from the module
     */
    virtual string ClearMAFSensor(void);

	/**
     * Simple delay before clearing the MAF Learn Bit
     * 
     * @return Result of the function (skipped or passed)
     */
    virtual string DelayBeforeMAFClear(void);


    /**
     * Overloaded method to check if it is a retest first
     */
    virtual string KeyOffEngineOffKeyOn(void);

    string KeyOffEngineOffKeyOnNoStart(void);
    string GetSerialNumberFromFile(void);
    string CheckSerialNumber(void);
	string ExtractEsnSerialNumberFromFile(string &fileName);

    string EnterNormalMode(void);
    string DisableNormalComms(void);
    string ReadMemoryLocation(void);
    string LockModuleIfPass(void);
    string DPDSwitchCheck(void);
    string MAFLearn(void);
    string CheckMAFLearnComplete(void);
    string CheckMAFLearnValues(void);
    string CheckBrakeSensor(bool onPosition);
    /**
     * Check if Brake Apply Sensor Learn happened at Flash Station
     */
    string CheckBASLearnComplete(void);
    /**
     * Adjust the mass value used in cruise control calculations to a suitable value for the machine
     */ 
    string AdjustCruiseMass(void);

    typedef vector<BackgroundSwitchMonitor *>                     BackgroundSwitchMonitorVector;
    typedef BackgroundSwitchMonitorVector::iterator               BackgroundSwitchMonitorVectorItr;
    typedef vector<BackgroundRangeCheckMonitor *>                 BackgroundRangeCheckMonitorVector;
    typedef BackgroundRangeCheckMonitorVector::iterator           BackgroundRangeCheckMonitorVectorItr;

    /** Vector to hold switch monitors */
    BackgroundSwitchMonitorVector m_switchMonitors;
    /** Vector to hold range check monitors */
    BackgroundRangeCheckMonitorVector m_rangeCheckMonitors;

    /** Timer to use for checking switch status */
    BepTimer m_switchMonitorTimer;
    /** Timer to use for checking sensor values */
    BepTimer m_sensorMonitorTimer;

    /** Switch data for setting update switch monitoring */
    XmlNodeMap m_switchMonitorItems;
    /** Pid data for setting up background range check monitoring */
    XmlNodeMap m_rangeCheckMonitorItems;

    bool m_mafLearnComplete;
};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuEmissionsTc_h
