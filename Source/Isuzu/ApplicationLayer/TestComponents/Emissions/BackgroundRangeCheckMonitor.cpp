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
#include "BackgroundRangeCheckMonitor.h"
#include <algorithm>

//-------------------------------------------------------------------------------------------------
BackgroundRangeCheckMonitor::BackgroundRangeCheckMonitor(const string &pidName, const string &messageTag, 
                                                         GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *module, 
                                                         BepServer *server) : 
m_minSensorValue(0.0), m_maxSensorValue(0.0)
{
    SensorName(&pidName);
    MessageTag(&messageTag);
    m_module = module;
    m_server = server;
}

//-------------------------------------------------------------------------------------------------
BackgroundRangeCheckMonitor::~BackgroundRangeCheckMonitor()
{   //clear the pidValue vector
    m_sensorValues.clear();
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE BackgroundRangeCheckMonitor::ReadCurrentSensorValue(void)
{   // Read the current sensor value from the module
    float sensorValue = 0.0;
    BEP_STATUS_TYPE status = m_module->ReadModuleData(MessageTag(), sensorValue);
    if(BEP_STATUS_SUCCESS == status)
    {   // If the reading was good, store the value
        m_sensorValues.push_back(sensorValue);
        Server()->Log(LOG_DEV_DATA, "Sensor: %s = %.2f", SensorName().c_str(), sensorValue);
    }
    else
    {
        Server()->Log(LOG_ERRORS, "Could not read %s from the module for %s: %s", 
                      MessageTag().c_str(), SensorName().c_str(), ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return status;
}

//-------------------------------------------------------------------------------------------------
string BackgroundRangeCheckMonitor::VerifySensorValues(float &minDeltaToCompare)
{
    string testResult(BEP_TESTING_RESPONSE);
    // Make sure sensor data was collected
    if(m_sensorValues.size() > 0)
    {   // Find the minimum and maximum values observed
        float maxVal = *(max_element(m_sensorValues.begin(), m_sensorValues.end()));
        float minVal = *(min_element(m_sensorValues.begin(), m_sensorValues.end()));
        MaxSensorValue(&maxVal);
        MinSensorValue(&minVal);
        // Find the delta between the minimum and maximum values observed
        testResult = (fabs(MaxSensorValue() - MinSensorValue()) > minDeltaToCompare) ? testPass : testFail;
    }
    else
    {   //Failure - No values to compare or recorded
        testResult = testFail;
    }
    return testResult;
}

//-------------------------------------------------------------------------------------------------
inline const string& BackgroundRangeCheckMonitor::SensorName(const string *sensorName /*= NULL*/)
{
    if(sensorName != NULL)  m_sensorName = *sensorName;
    return m_sensorName;
}

//-------------------------------------------------------------------------------------------------
inline const float BackgroundRangeCheckMonitor::MinSensorValue(const float *value /*= NULL */)
{
    if(value != NULL) m_minSensorValue = *value;
    return m_minSensorValue;
}

//-------------------------------------------------------------------------------------------------
inline const float BackgroundRangeCheckMonitor::MaxSensorValue(const float *value /*= NULL */)
{
    if(value != NULL) m_maxSensorValue = *value;
    return m_maxSensorValue;
}

//-------------------------------------------------------------------------------------------------
inline const string& BackgroundRangeCheckMonitor::MessageTag(const string *messageTag /*= NULL*/)
{
    if(messageTag != NULL)  m_messageTag = *messageTag;
    return m_messageTag;
}

//-------------------------------------------------------------------------------------------------
inline BepServer* BackgroundRangeCheckMonitor::Server()
{
    return m_server;
}
