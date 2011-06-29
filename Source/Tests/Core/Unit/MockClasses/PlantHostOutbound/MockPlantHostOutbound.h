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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/PlantHostOutbound/MockPlantHostOutbound.h $
// 
// 1     1/18/07 4:48p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockPlantHostOutbound_h
#define MockPlantHostOutbound_h

#include "PlantHostOutbound.h"

class MockPlantHostOutbound : public PlantHostOutbound
{
public:
    MockPlantHostOutbound();
    virtual ~MockPlantHostOutbound();

    virtual const std::string BuildTestResultString(
        const XmlNode *testResult,const XmlNodeMap &resultStringMap,
        const XmlNodeMap &resultConversions,INT32 &totalSerialNumbers,
        const bool &useVehicleBuild = true,
        const bool &allowSpaceInResult = true);

    /** 
     * member variable to tell how many times BuildTestResultString() is called 
     */
    int m_buildTestResultStringCalled;

    /** vectors to store the variables passed to BuildTestResultString() */
    vector<const XmlNode*> m_buildTestResultStringParamTestResult;
    vector<XmlNodeMap> m_buildTestResultStringParamResultStringMap;
    vector<XmlNodeMap> m_buildTestResultStringParamResultConversions;
    vector<INT32> m_buildTestResultStringParamTotalSerialNumbers;
    vector<bool> m_buildTestResultStringParamUseVehicleBuild;
    vector<bool> m_buildTestResultStringParamAllowSpaceInResult;

    /**
     * Vector of return values for BuildTestResultString().  These need to be 
     * set in the unit tests.
     */ 
    vector<std::string> m_buildTestResultStringReturnValue;

    virtual const BEP_STATUS_TYPE SendTestResultString(
        std::string &resultString,SerialChannel &portComm,
        const unsigned char ackChar,const INT32 &maxAttempts = 3,
        const INT32 &hostResponseTime = 1000);

    /** 
     * member variable to tell how many times SendTestResultString() is called 
     */
    int m_sendTestResultStringCalled;

    /** vectors to store the variables passed to SendTestResultString() */
    vector<std::string> m_sendTestResultStringParamResultString;
    vector<SerialChannel> m_sendTestResultStringParamPortComm;
    vector<unsigned char> m_sendTestResultStringParamAckChar;
    vector<INT32> m_sendTestResultStringParamMaxAttempts;
    vector<INT32> m_sendTestResultStringParamHostResponseTime;

    /**
     * Vector of return values for SendTestResultString().  These need to be 
     * set in the unit tests.
     */ 
    vector<BEP_STATUS_TYPE> m_sendTestResultStringReturnValue;
};

#endif
