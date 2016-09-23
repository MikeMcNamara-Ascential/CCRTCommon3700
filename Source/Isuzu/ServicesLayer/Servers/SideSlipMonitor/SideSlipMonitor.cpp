//******************************************************************************
// Description:
//  Server used to monitor the side slip reading.
//
//==============================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "SideSlipMonitor.h"

//-------------------------------------------------------------------------------------------------
SideSlipMonitor::SideSlipMonitor() : BepServer()
{   
	m_axleCount = 0.0;
	m_currentMaxSideSlipValue = 0.0;
	m_ndb = NULL;
}

//-------------------------------------------------------------------------------------------------
SideSlipMonitor::~SideSlipMonitor()
{   
	m_sideSlipTimer.Stop();
}

//-------------------------------------------------------------------------------------------------
const INT32 SideSlipMonitor::HandlePulse(const INT32 code, const INT32 value)
{
	INT32 status = BEP_STATUS_ERROR;
    Log(LOG_DEV_DATA, "Recieved code: %d, Recieved value: %d, axle count: %.1f", code, value, m_axleCount);
	switch(code)
	{
	case SIDE_SLIP_MONITOR_PULSE_CODE:
		switch(value)
		{
		case SIDE_SLIP_PULSE:
			status = UpdateSideSlipValue();
			break;

		case INTERAXLE_GAP_PULSE:
            m_axleGapTimer.Stop();
			if(m_axleCount == 2.0)
			{
				char buff[8];
				string response;
				status = m_ndb->Write("SideSlipValue", 
									  CreateMessage(buff, sizeof(buff), "%.2f", m_currentMaxSideSlipValue), 
									  response, true);
                Log(LOG_DEV_DATA, "SideSlip value set to: %2f", m_currentMaxSideSlipValue);
			}
			else
			{
				status = BEP_STATUS_SUCCESS;
				Log(LOG_ERRORS, "Detected axle count is unexpected for a truck - axle count: %.1f, expected 2.0", 
					m_axleCount);
			}
			Log(LOG_DEV_DATA, "Resetting axle count to 0");
			m_axleCount = 0.0;
			break;

		case AXLE_TIME_LIMIT_PULSE:
			Log(LOG_DEV_DATA, "Axle time limit expired, stopping side slip timer (axle count: %.1f)", m_axleCount);
			m_axleTimeLimitTimer.Stop();
			m_sideSlipTimer.Stop();
			break;

		default:
			Log(LOG_ERRORS, "Unhandled side slip monitor pulse value: %d", value);
			break;
		}
		break;

	default:
		status = BepServer::HandlePulse(code, value);
		break;
	}
	return status;
}

//-------------------------------------------------------------------------------------------------
void SideSlipMonitor::Initialize(const XmlNode *document)
{
	// Call the base class to begin the initialization
	BepServer::Initialize(document);

	// Get the conversion factor for the side slip data value
	try
	{
		m_conversionFactor = atof(document->getChild("Setup/Parameters/SideSlipConversionFactor")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find conversion factor, setting to 0.01 - %s", excpt.GetReason());
		m_conversionFactor = 0.01;
	}

	// Setup the side slip timer
	UINT64 interval = 100;
	try
	{
		interval = BposReadInt(document->getChild("Setup/Parameters/SideSlipMonitorInterval")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find monitor interval, setting to 100ms - %s", excpt.GetReason());
		interval = 100;
	}
	m_sideSlipTimer.SetPulseCode(SIDE_SLIP_MONITOR_PULSE_CODE);
	m_sideSlipTimer.SetPulseValue(SIDE_SLIP_PULSE);
	m_sideSlipTimer.Initialize(GetProcessName(), mSEC_nSEC(interval), mSEC_nSEC(interval));
	m_sideSlipTimer.Stop();

	// Setup the interaxle timer
	try
	{
		interval = BposReadInt(document->getChild("Setup/Parameters/AxleInterval")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find axle interval, setting to 10s - %s", excpt.GetReason());
		interval = 10000;
	}
	m_axleGapTimer.SetPulseCode(SIDE_SLIP_MONITOR_PULSE_CODE);
	m_axleGapTimer.SetPulseValue(INTERAXLE_GAP_PULSE);
	m_axleGapTimer.Initialize(GetProcessName(), mSEC_nSEC(interval), mSEC_nSEC(interval));
	m_axleGapTimer.Stop();

	// Setup the axle time limit timer
	try
	{
		interval = BposReadInt(document->getChild("Setup/Parameters/AxleTimeLimit")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find axle time limit, setting to 5s - %s", excpt.GetReason());
		interval = 5000;
	}
	m_axleTimeLimitTimer.SetPulseCode(SIDE_SLIP_MONITOR_PULSE_CODE);
	m_axleTimeLimitTimer.SetPulseValue(AXLE_TIME_LIMIT_PULSE);
	m_axleTimeLimitTimer.Initialize(GetProcessName(), mSEC_nSEC(interval), mSEC_nSEC(interval));
	m_axleTimeLimitTimer.Stop();
}

//-------------------------------------------------------------------------------------------------
const string SideSlipMonitor::Register(void)
{
	string result = BepServer::Register();
	if(m_ndb == NULL)  m_ndb = new INamedDataBroker();
	return result;
}

//-------------------------------------------------------------------------------------------------
const string SideSlipMonitor::Publish(const XmlNode *node)
{
	string result = BEP_ERROR_RESPONSE;
	if(!node->getName().compare(GetDataTag("SideSlipIn")) && atob(node->getValue().c_str()))
	{
        Log(LOG_DEV_DATA, "axleCount: %.1f", m_axleCount);
        if (!(m_axleCount == 0.0 || m_axleCount == 1.0))
        { //should not have axle enter before previous axle leaves, so must be whole number
            m_axleCount = 0.0;                             //makes sure does not trigger if someone walks in front of vehicle sensor
        }
        if(m_axleCount == 0.0)
		{
			Log(LOG_DEV_DATA, "Vehicle front axle arrived at side slip tester, starting timer, reset currentMax");
			m_sideSlipTimer.Start();
			m_axleTimeLimitTimer.Start();
            m_currentMaxSideSlipValue = 0.0;
		}
        m_axleCount += 0.5;
	}
	else if(!node->getName().compare(GetDataTag("SideSlipOut")) && atob(node->getValue().c_str()))
	{
		m_sideSlipTimer.Stop();
		m_axleTimeLimitTimer.Stop();
		if (m_axleCount != 0.0 && m_axleCount != 1.0) //vehicleOut should not start the axleCount, must be halfway b/t axles 
        {                                             //makese sure does not trigger if someone walks in front of vehicle sensor
            m_axleCount += 0.5;
        }
        else
        {
            m_axleCount = 0.0; //reset back to 0.0
        }
		if(m_axleCount == 1.0)
		{
			m_axleGapTimer.Start();
		}
		Log(LOG_DEV_DATA, "Vehicle departed side slip tester, stopped timer; axleCount: %.1f", m_axleCount);
	}
	result = BepServer::Publish(node);
	return result;
}

//-------------------------------------------------------------------------------------------------
INT32 SideSlipMonitor::UpdateSideSlipValue()
{
    Log(LOG_DEV_DATA, "entered updateSideSlipValueFunc");
    INT32 status = BEP_STATUS_ERROR;
	float currentSideSlipValue = -99.9;
	string response;
	string tag("RawSideSlipValue");
	string value;
	status = m_ndb->Read(tag, response, true);
	if(BEP_STATUS_SUCCESS == status)
	{
        //Log(LOG_DEV_DATA, "entered BEP_STATUS_SUCCESS 1");
        status = m_ndb->GetByTag(tag, value, response);
		if(BEP_STATUS_SUCCESS == status)
		{
            Log(LOG_DEV_DATA, "entered BEP_STATUS_SUCCESS 2, currentSideSlip: %f", currentSideSlipValue);
            short temp = BposReadInt(value.c_str());
            float fTemp = temp;
            currentSideSlipValue = fTemp * m_conversionFactor;

            Log(LOG_DEV_DATA, "currentSideSlip: %f, currentMax: %f, conversion: %f, value: %s, temp: %d, fTemp: %f", currentSideSlipValue, m_currentMaxSideSlipValue, m_conversionFactor, value.c_str(), temp, fTemp);
			if(fabs(currentSideSlipValue) > fabs(m_currentMaxSideSlipValue)) //add in absValue (fabs for float) to this so the max slip is reported whether pos or neg
			{
                Log(LOG_DEV_DATA, "CurrentMax: %f, CurrentSideSlipVal: %f", m_currentMaxSideSlipValue, currentSideSlipValue);
				m_currentMaxSideSlipValue = currentSideSlipValue;
				Log(LOG_DEV_DATA, "New max side slip value: %.2f", m_currentMaxSideSlipValue);
			}
		}
	}
	else
	{
		Log(LOG_ERRORS, "Error reading side slip value from PLC - %s", ConvertStatusToResponse(status).c_str());
	}
	return status;
}
