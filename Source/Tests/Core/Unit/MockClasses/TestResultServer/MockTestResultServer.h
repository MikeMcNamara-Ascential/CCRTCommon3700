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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/TestResultServer/MockTestResultServer.h $
// 
// 1     10/26/06 2:02p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockTestResultServer_h
#define MockTestResultServer_h

#include "ITestResultServer.h"

class MockTestResultServer : public ITestResultServer
{
public:
    MockTestResultServer();
    ~MockTestResultServer();

    INT32 WriteTestResult(const std::string test,  const std::string result, 
                          const std::string process,
                          const std::string description, std::string &message, 
                          const bool isFinal = false,
                          const std::string code = "0000");

    INT32 WriteTestResult(const std::string test, const std::string result, 
                          const std::string process,
                          const std::string description, std::string &message, 
                          const TestResultDetails &details,
                          const bool isFinal = false,
                          const std::string code = "0000");

    /** member variable to tell how many times WriteTestResult() was called */
    int m_writeTestResultCalled;

    /** vectors to store the parameters passed to WriteTestResult() */
    vector<std::string> m_writeTestResultParamTest;
    vector<std::string> m_writeTestResultParamResult;
    vector<std::string> m_writeTestResultParamProcess;
    vector<std::string> m_writeTestResultParamDescription;
    vector<TestResultDetails> m_writeTestResultParamDetails;
    vector<bool> m_writeTestResultParamIsFinal;
    vector<std::string> m_writeTestResultParamCode;

    /** 
     * Vector of return messages for WriteTestResult().  These will need to be 
     * set in the unit tests.
     */ 
    vector<std::string> m_writeTestResultReturnMessage;

    /**
     * Vector of return values for WriteTestResult().  These will need to be 
     * set in the unit tests.
     */ 
    vector<INT32> m_writeTestResultReturnValue;
};

#endif
