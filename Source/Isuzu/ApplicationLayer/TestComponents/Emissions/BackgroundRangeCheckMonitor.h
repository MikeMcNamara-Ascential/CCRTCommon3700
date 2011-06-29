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
#ifndef BackgroundRangeCheckMonitor_h
#define BackgroundRangeCheckMonitor_h
//-------------------------------------------------------------------------------------------------
#include "GenericEmissionsModuleTemplate.h"
#include "KwpCanProtocolFilter.h"
#include "math.h"
#include <vector>
//-------------------------------------------------------------------------------------------------
class BackgroundRangeCheckMonitor
{
public:
    /**
     * Class constructor.
     * 
     * @param pidName	 Name of the sensor being monitored (ie IntakeAirTemperature).
     * @param messageTag Message tag to use to read the sensor value from the module.
     * @param config     Module configuration data.
     * @param server     Server that created this object.
     */
    BackgroundRangeCheckMonitor(const string &pidName, const string &messageTag,
                                GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *module, BepServer *server);
    virtual ~BackgroundRangeCheckMonitor();

    /**
     * Read the current sensor value from the module.
     * 
     * @return Status of reading the sensor value from the module.
     */
    BEP_STATUS_TYPE ReadCurrentSensorValue(void);

    /**
     * Get/Set the name the sensor being monitored.
     * 
     * @param sensorName Name of the sensor being monitored.
     * 
     * @return Name of the sensor being monitored.
     */
    const string& SensorName(const string *sensorName = NULL);

    /**
     * Verify the stored readings with a mininium delta value
     * 
     * @param minDeltaToCompare value of the min allowed delta between the min and max read values
     * 
     * @return string with pass or fail of the verify
     */
    string VerifySensorValues(float &minDeltaToCompare);

    /**
     * Get/Set the minimum sensor value observed.
     * 
     * @param value  Minimum sensor value observed.
     * 
     * @return Minimum sensor value observed.
     */
    const float MinSensorValue(const float *value = NULL);

    /**
     * Get/Set the maximum sensor value observed.
     * 
     * @param value  Maximum sensor value observed.
     * 
     * @return Maximum sensor value observed.
     */
    const float MaxSensorValue(const float *value = NULL);

private:

    /**
     * Get/Set the message tag to use for reading the sensor value from the module.
     * 
     * @param messageTag Message tag to use for reading the sensor value from the module.
     * 
     * @return Message tag to use for reading the sensor value from the module.
     */
    const string& MessageTag(const string *messageTag = NULL);

    /**
     * Get the pointer to the server that created this object.
     * 
     * @return Pointer to the server that created this object.
     */
    BepServer* Server();

    //** Float vector to hold values when reads are performed
    vector<float> m_sensorValues;
    /** Name of the sensor being monitored */
    string m_sensorName;
    /** Message tag for reading the sensor value */
    string m_messageTag;
    /** Pointer to the server that created this object */
    BepServer *m_server;
    /** Module interface for reading sensor data from the module */
    GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *m_module;
    /** Minimum sensor value observed while monitoring */
    float m_minSensorValue;
    /** Maximum sensor value observed while monitoring */
    float m_maxSensorValue;
};
//-------------------------------------------------------------------------------------------------
#endif //BackgroundRangeCheckMonitor_h
