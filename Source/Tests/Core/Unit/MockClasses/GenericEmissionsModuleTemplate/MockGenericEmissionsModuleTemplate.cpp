//******************************************************************************
// Description:
//  A mock generic emissions module template class that is used in unit testing.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/GenericEmissionsModuleTemplate/MockGenericEmissionsModuleTemplate.cpp $
// 
// 2     5/01/07 1:27p Rwiersem
// Changes for the 07050201 core release:
// 
// Added StartModuleKeepAlive() and StopModuleKeepAlive().  Added multiple
// GetInfo() methods.
// 
// 1     1/18/07 4:47p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockGenericEmissionsModuleTemplate.h"

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    MockGenericEmissionsModuleTemplate() : 
        GenericEmissionsModuleTemplate<ProtocolFilterType>(),
        m_initializeHookCalled(0),m_initializeHookReturnValue(true), 
        m_startModuleKeepAliveCalled(0),
        m_startModuleKeepAliveReturnValue(BEP_STATUS_SUCCESS),
        m_stopModuleKeepAliveCalled(0),
        m_stopModuleKeepAliveReturnValue(BEP_STATUS_SUCCESS), 
        m_getInfoCalled(0)
{
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    ~MockGenericEmissionsModuleTemplate()
{
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
bool MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    InitializeHook(const XmlNode *configNode)
{

    m_initializeHookCalled++;

    m_initializeHookParamConfigNode = configNode;

    return(m_initializeHookReturnValue);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    StartModuleKeepAlive(void) throw(ModuleException)
{
    m_startModuleKeepAliveCalled++;

    return(m_startModuleKeepAliveReturnValue);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    StopModuleKeepAlive(void) throw(ModuleException)
{
    m_stopModuleKeepAliveCalled++;

    return(m_stopModuleKeepAliveReturnValue);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,string &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueString[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,UINT16 &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueUint16[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,float &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueFloat[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,UINT8 &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueUint8[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,FaultVector_t &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueFaultVector;

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,INT32 &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueInt32[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,bool &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueBool[m_getInfoCalled];

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType> 
BEP_STATUS_TYPE MockGenericEmissionsModuleTemplate<ProtocolFilterType>::
    GetInfo(string methodName,vector<UINT16> &value) throw(ModuleException)
{
    // store the passed in parameter
    m_getInfoParamMethodName.push_back(methodName);

    // set the return data value
    value = m_getInfoReturnDataValueVectorUint16;

    return(m_getInfoReturnValue[m_getInfoCalled++]);
}
