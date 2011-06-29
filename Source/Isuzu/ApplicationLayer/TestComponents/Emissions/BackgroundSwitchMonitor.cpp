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
#include "BackgroundSwitchMonitor.h"

//-------------------------------------------------------------------------------------------------
BackgroundSwitchMonitor::BackgroundSwitchMonitor(const string &switchName, const string &messageTag, 
                                                 GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *module, 
                                                 BepServer *server) : 
m_switchOnDetected(false), m_switchOffDetected(false)
{
    SwitchName(&switchName);
    MessageTag(&messageTag);
    m_module = module;
    m_server = server;
}

//-------------------------------------------------------------------------------------------------
BackgroundSwitchMonitor::~BackgroundSwitchMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE BackgroundSwitchMonitor::ReadCurrentSwitchState(void)
{
    bool isSwitchOn = false;
    BEP_STATUS_TYPE status = m_module->ReadModuleData(MessageTag(), isSwitchOn);
    if(BEP_STATUS_SUCCESS == status)
    {
        if(isSwitchOn && !SwitchOnDetected())
        {
            SwitchOnDetected(&isSwitchOn);
            Server()->Log(LOG_DEV_DATA, "Detected switch %s in the ON position", SwitchName().c_str());
        }
        else if(!isSwitchOn && !SwitchOffDetected())
        {
            bool switchOff = !isSwitchOn;
            SwitchOffDetected(&switchOff);
            Server()->Log(LOG_DEV_DATA, "Detected switch %s in the OFF position", SwitchName().c_str());
        }
    }
    else
    {
        Server()->Log(LOG_ERRORS, "Could not read %s from the module for switch %s: %s", 
                      MessageTag().c_str(), SwitchName().c_str(), ConvertStatusToResponse(status).c_str());
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
inline bool& BackgroundSwitchMonitor::SwitchOnDetected(const bool *onDetected)
{
    if(onDetected != NULL)  m_switchOnDetected = *onDetected;
    return m_switchOnDetected;
}

//-------------------------------------------------------------------------------------------------
inline bool& BackgroundSwitchMonitor::SwitchOffDetected(const bool *offDetected)
{
    if(offDetected != NULL)  m_switchOffDetected = *offDetected;
    return m_switchOffDetected;
}

//-------------------------------------------------------------------------------------------------
inline bool BackgroundSwitchMonitor::SwitchMonitoringComplete(void)
{
    return SwitchOnDetected() && SwitchOffDetected();
}

//-------------------------------------------------------------------------------------------------
inline const string& BackgroundSwitchMonitor::SwitchName(const string *switchName /*= NULL*/)
{
    if(switchName != NULL)  m_switchName = *switchName;
    return m_switchName;
}

//-------------------------------------------------------------------------------------------------
inline const string& BackgroundSwitchMonitor::MessageTag(const string *messageTag /*= NULL*/)
{
    if(messageTag != NULL)  m_messageTag = *messageTag;
    return m_messageTag;
}

//-------------------------------------------------------------------------------------------------
inline BepServer* BackgroundSwitchMonitor::Server()
{
    return m_server;
}
