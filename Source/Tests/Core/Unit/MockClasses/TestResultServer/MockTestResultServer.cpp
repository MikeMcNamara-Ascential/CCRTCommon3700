//******************************************************************************
// Description:
//  The mock test result server class.  This is used in unit testing to verify
//  the correct information was sent to the test result server.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/TestResultServer/MockTestResultServer.cpp $
// 
// 1     10/26/06 2:02p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <iostream>
#include "MockTestResultServer.h"

using namespace std;

//=============================================================================
MockTestResultServer::MockTestResultServer() :
    m_writeTestResultCalled(0)
{
}

//=============================================================================
MockTestResultServer::~MockTestResultServer()
{

}

//=============================================================================
INT32 MockTestResultServer::WriteTestResult(const std::string test, 
                                            const std::string result,
                                            const std::string process,
                                            const std::string description, 
                                            std::string &message,
                                            const TestResultDetails &details,
                                            const bool isFinal/*= false*/,
                                            const std::string code/*="0000"*/)
{
    // store the passed in parameters
    m_writeTestResultParamTest.push_back(test);
    m_writeTestResultParamResult.push_back(result);
    m_writeTestResultParamProcess.push_back(process);
    m_writeTestResultParamDescription.push_back(description);
    m_writeTestResultParamDetails.push_back(details);
    m_writeTestResultParamIsFinal.push_back(isFinal);
    m_writeTestResultParamCode.push_back(code);

    // return the specified message
    message = m_writeTestResultReturnMessage[m_writeTestResultCalled];

    // increment the method called counter and return the specified value
    return(m_writeTestResultReturnValue[m_writeTestResultCalled++]);
}

//=============================================================================
INT32 MockTestResultServer::WriteTestResult(const std::string test,
                                            const std::string result,
                                            const std::string process,
                                            const std::string description,
                                            std::string &message,
                                            const bool isFinal/*= false*/,
                                            const std::string code/*= "0000"*/)
{
    // store the passed in parameters
    m_writeTestResultParamTest.push_back(test);
    m_writeTestResultParamResult.push_back(result);
    m_writeTestResultParamProcess.push_back(process);
    m_writeTestResultParamDescription.push_back(description);
    m_writeTestResultParamIsFinal.push_back(isFinal);
    m_writeTestResultParamCode.push_back(code);

    // return the specified message
    message = m_writeTestResultReturnMessage[m_writeTestResultCalled];

    // increment the method called counter and return the specified value
    return(m_writeTestResultReturnValue[m_writeTestResultCalled++]);
}
