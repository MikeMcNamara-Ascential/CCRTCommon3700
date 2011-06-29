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
#ifndef BackgroundSwitchMonitor_h
#define BackgroundSwitchMonitor_h
//-------------------------------------------------------------------------------------------------
#include "GenericEmissionsModuleTemplate.h"
#include "KwpCanProtocolFilter.cpp"
//-------------------------------------------------------------------------------------------------
class BackgroundSwitchMonitor
{
public:
    /**
     * Class constructor.
     * 
     * @param switchName Name of the switch being monitored (ie BrakeSwitch).
     * @param messageTag Message tag to use to read the switch status from the module.
     * @param module     Module interface to use for reading data from the vehicle module.
     * @param server     Server that created this object.
     */
    BackgroundSwitchMonitor(const string &switchName, const string &messageTag, 
                            GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *module, BepServer *server);
    virtual ~BackgroundSwitchMonitor();

    /**
     * Read the current switch state from the module.
     * 
     * @return Status of reading the switch state from the module.
     */
    BEP_STATUS_TYPE ReadCurrentSwitchState(void);

    /**
     * Get the flag indicating if the switch was detected in the ON position.
     * 
     * @param onDetected
     *               Flag indicating if the switch was detected in the ON position.
     * 
     * @return Flag indicating if the switch was detected in the ON position.
     */
    bool& SwitchOnDetected(const bool *onDetected = NULL);

    /**
     * Get the flag indicating if the switch was detected in the OFF position.
     * 
     * @param offDetected
     *               Flag indicating if the switch was detected in the OFF position.
     * 
     * @return Flag indicating if the switch was detected in the OFF position.
     */
    bool& SwitchOffDetected(const bool *offDetected = NULL);

    /**
     * Check if switch monitoring is complete.
     * Switch monitoring is complete when the switch has been detected in the ON and OFF states.
     * 
     * @return Flag indicating if switch monitoring has completed.
     */
    bool SwitchMonitoringComplete(void);

    /**
     * Get/Set the name the switch being monitored.
     * 
     * @param switchName Name of the switch being monitored.
     * 
     * @return Name of the switch being monitored.
     */
    const string& SwitchName(const string *switchName = NULL);

private:

    /**
     * Get/Set the message tag to use for reading the switch status from the module.
     * 
     * @param messageTag Message tag to use for reading the switch status from the module.
     * 
     * @return Message tag to use for reading the switch status from the module.
     */
    const string& MessageTag(const string *messageTag = NULL);

    /**
     * Get the pointer to the server that created this object.
     * 
     * @return Pointer to the server that created this object.
     */
    BepServer* Server();

    /** Flag to indicate the ON position has been detected */
    bool m_switchOnDetected;
    /** Flag to indicate the OFF position has been detected */
    bool m_switchOffDetected;
    /** Name of the switch being monitored */
    string m_switchName;
    /** Message tag for reading the switch status */
    string m_messageTag;
    /** Module interface to use for reading data from the vehicle module */
    GenericEmissionsModuleTemplate<KwpCanProtocolFilter> *m_module;
    /** Pointer to the server that created this object */
    BepServer *m_server;
};
//-------------------------------------------------------------------------------------------------
#endif //BackgroundSwitchMonitor_h
