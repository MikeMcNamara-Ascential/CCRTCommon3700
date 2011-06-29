//******************************************************************************
// Description:
//  A mock protocol filter class that is used in unit testing.
//
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/ProtocolFilter/MockProtocolFilter.cpp $
// 
// 1     1/18/07 4:50p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockProtocolFilter.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
MockProtocolFilter::MockProtocolFilter(KeepAliveTimer_t &lastTxTime, 
									   StopCommsBepCondVar *stopCommsBepCondVar /*= NULL*/, 
                                       BepMutex *commsInUse/*=NULL*/) :
    ProtocolFilter(lastTxTime,stopCommsBepCondVar,commsInUse),m_getModuleDataCalled(0),
    m_getModuleDataShouldThrow(false),m_extractModuleDataCalled(0)
{
}

//-----------------------------------------------------------------------------
MockProtocolFilter::~MockProtocolFilter()
{
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE MockProtocolFilter::GetModuleData(
    std::string messageTag,SerialString_t &reply,SerialArgs_t *args/*=NULL*/)
{
    // store the passed in parameters
    m_getModuleDataParamMessageTag.push_back(messageTag);
    m_getModuleDataParamArgs.push_back(args);

    // check if an exception should be thrown
    if(m_getModuleDataShouldThrow == true)
    {
        throw ModuleException("GetModuleData() exception");
    }

    // return the specified reply
    reply = m_getModuleDataReply[m_getModuleDataCalled];

    // increment the method called counter and return the specified value
    return(m_getModuleDataReturnValue[m_getModuleDataCalled++]);
}

//-----------------------------------------------------------------------------
const SerialString_t MockProtocolFilter::ExtractModuleData(
    SerialString_t &moduleResponse)
{
    // store the passed in parameters
    m_extractModuleDataParamModuleResponse.push_back(moduleResponse);

    // increment the method called counter and return the specified value
    return(m_extractModuleDataReturnValue[m_extractModuleDataCalled++]);
}

