//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the VW engine contoller module.
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
// 
//*************************************************************************
#include "VwEngineControlModule.h"

//-------------------------------------------------------------------------------------------------
template<class ProtocolFileter>
VwEngineControlModule<ProtocolFileter>::VwEngineControlModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFileter>
VwEngineControlModule<ProtocolFileter>::~VwEngineControlModule()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFileter>
inline string VwEngineControlModule<ProtocolFileter>::ModuleName()
{
    return "VW Engine Module";
}

//-------------------------------------------------------------------------------------------------
template<class ProtocolFileter>
BEP_STATUS_TYPE VwEngineControlModule<ProtocolFileter>::EnterDiagnosticMode(void)
{
    Log( LOG_FN_ENTRY, "Enter VwEngineControlModule::EnterDiagnosticMode()");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    BposSleep(1000);               // Make sure the module has completed its self test
    // Send the init message at low baud rate
    m_protocolFilter->ResetConnection();
    if(m_protocolFilter->LowSpeedInit() == EOK)
    {
        Log( LOG_DEV_DATA, "Low speed init message sent to module");
        status = BEP_STATUS_SUCCESS;
    }
    else 
    {
        status = BEP_STATUS_FAILURE;
        Log(LOG_ERRORS, "Error during low speed init");
    }

    if(BEP_STATUS_SUCCESS == status)
    {
        Log( LOG_DEV_DATA, "Started diagnostic session");
    }
    else 
    {
        Log(LOG_ERRORS, "Failed to start diagnostic session");
    }
    // Log the exit and return the status
    Log( LOG_FN_ENTRY, "Exit VwEngineControlModule::EnterDiagnosticMode(), status=%d", status);
    return status;
}
