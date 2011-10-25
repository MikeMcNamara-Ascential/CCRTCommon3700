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
#include "DelphiAirbagModule.h"

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DelphiAirbagModule<ProtocolFilterType>::DelphiAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
DelphiAirbagModule<ProtocolFilterType>::~DelphiAirbagModule()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
inline string DelphiAirbagModule<ProtocolFilterType>::ModuleName()
{
    return "Delphi Airbag Module";
}



//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
BEP_STATUS_TYPE DelphiAirbagModule<ProtocolFilterType>::GetSecurityAccess()
{
    // from something else 
    SerialArgs_t seed;           // Security seed read from the module
    SerialArgs_t securityKeyc;    // Security unlock key
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t  moduleResponse;
    string test123="";
    Log(LOG_FN_ENTRY, "DelphiAirbagModule::GetSecurityAccess() - Enter\n");
    //bool AccessGranted;
    // Read the security seed from the module
    if ((status = ReadModuleData("CommandSecurityAccess", seed)) == BEP_STATUS_SUCCESS)
    {   
        if (seed.size()!=0) 
        {
        
            //calculate key using Ones Complement
            for (UINT32 ii = 0; ii < seed.size();ii++) 
            {
                securityKeyc.push_back(~seed[ii]);
            }
            //transmist key back to module
            if ((status=CommandModule("TransmitSecurityKey",&securityKeyc)) == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA, "Security Key sent successfully");
            }
            else
            {
                Log(LOG_ERRORS, "Failed to send Security Key from the module - status: %s",ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {
             Log(LOG_DEV_DATA, "Seed Key is NULL");
        }
        
        

    }
    else
    {   
        Log(LOG_ERRORS, "Failed to read Security Seed from the module - status: %s",ConvertStatusToResponse(status).c_str());
    }
    Log(LOG_FN_ENTRY, "DelphiAirbagModule::GetSecurityAccess() - Exit\n");
    // Return the status     
    return status;
}
