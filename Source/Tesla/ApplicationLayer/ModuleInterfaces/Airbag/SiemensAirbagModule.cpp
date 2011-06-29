//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SiemensAirbagModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
SiemensAirbagModule<ProtocolFilterType>::SiemensAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
SiemensAirbagModule<ProtocolFilterType>::~SiemensAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string SiemensAirbagModule<ProtocolFilterType>::ModuleName()
{
    return "Siemens Airbag Module";
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE SiemensAirbagModule<ProtocolFilterType>::PerformModuleLinkup(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    INT32 connectStat = -1;
    Log(LOG_FN_ENTRY, "SiemensAirbagModule::PerformModuleLinkup() - Enter");
    // Send the low baud init command
    if((connectStat = m_protocolFilter->ResetConnection()) == EOK)
    {
        status = (m_protocolFilter->LowSpeedInit() == EOK) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
        // Clear the serial port
        m_protocolFilter->ResetConnection();
        Log(LOG_FN_ENTRY, "SiemensAirbagModule::LowSpeedInit status %s", ConvertStatusToResponse(status).c_str());
    }
    else
    {   // Could not reset the connection
        Log(LOG_ERRORS, "Failed to reset the connection: %d - %s", connectStat, strerror(connectStat));
        status = BEP_STATUS_HARDWARE;
    }
    Log(LOG_FN_ENTRY, "SiemensAirbagModule::PerformModuleLinkup() - Exit: %s", ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE SiemensAirbagModule<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{   // Log the entry and read the fault codes from the module
    Log(LOG_FN_ENTRY, "SiemensAirbagModule::ReadFaults() - Enter");
    vector<string> dtc;
    BEP_STATUS_TYPE status = ReadModuleData(m_readFaults, dtc);
    Log(LOG_ERRORS, "Read faults from the module: %s", ConvertStatusToResponse(status).c_str());
    // Print all the faults found in the module
    for(UINT8 index = 0; index < dtc.size(); index++)
    {
        Log(LOG_ERRORS, "Module fault %d: %s", index+1, dtc[index].c_str());
        faultCodes.push_back(dtc[index]);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "SiemensAirbagModule::ReadFaults() - Exit");
    return status;
}
