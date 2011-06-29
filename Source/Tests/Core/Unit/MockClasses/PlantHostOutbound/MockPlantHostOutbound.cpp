//******************************************************************************
// Description:
//  A mock plant host outbound class that is used in unit testing.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/PlantHostOutbound/MockPlantHostOutbound.cpp $
// 
// 1     1/18/07 4:48p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockPlantHostOutbound.h"

//-----------------------------------------------------------------------------
MockPlantHostOutbound::MockPlantHostOutbound() : PlantHostOutbound(), 
    m_buildTestResultStringCalled(0),m_sendTestResultStringCalled(0)
{
}

//-----------------------------------------------------------------------------
MockPlantHostOutbound::~MockPlantHostOutbound()
{
}

//-----------------------------------------------------------------------------
const std::string MockPlantHostOutbound::BuildTestResultString(
        const XmlNode *testResult,const XmlNodeMap &resultStringMap,
        const XmlNodeMap &resultConversions,INT32 &totalSerialNumbers,
        const bool &useVehicleBuild/*=true*/,
        const bool &allowSpaceInResult/*=true*/)
{
    // store the passed in parameters
    m_buildTestResultStringParamTestResult.push_back(testResult);
    m_buildTestResultStringParamResultStringMap.push_back(resultStringMap);
    m_buildTestResultStringParamResultConversions.push_back(resultConversions);
    m_buildTestResultStringParamTotalSerialNumbers.push_back(totalSerialNumbers);
    m_buildTestResultStringParamUseVehicleBuild.push_back(useVehicleBuild);
    m_buildTestResultStringParamAllowSpaceInResult.push_back(allowSpaceInResult);

    // increment the method called counter and return the specified value
    return(m_buildTestResultStringReturnValue[m_buildTestResultStringCalled++]);
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE MockPlantHostOutbound::SendTestResultString(
        std::string &resultString,SerialChannel &portComm,
        const unsigned char ackChar,const INT32 &maxAttempts/*=3*/,
        const INT32 &hostResponseTime/*=1000*/)
{
    // store the passed in parameters
    m_sendTestResultStringParamResultString.push_back(resultString);
    m_sendTestResultStringParamPortComm.push_back(portComm);
    m_sendTestResultStringParamAckChar.push_back(ackChar);
    m_sendTestResultStringParamMaxAttempts.push_back(maxAttempts);
    m_sendTestResultStringParamHostResponseTime.push_back(hostResponseTime);

    // increment the method called counter and return the specified value
    return(m_sendTestResultStringReturnValue[m_sendTestResultStringCalled++]);
}
