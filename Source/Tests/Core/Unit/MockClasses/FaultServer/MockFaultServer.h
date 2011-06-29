//******************************************************************************
// Description:
//  The mock fault server class.  This is used in unit testing to verify the 
//  correct information was sent to the fault server.
//
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/FaultServer/MockFaultServer.h $
// 
// 1     1/18/07 4:46p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockFaultServer_h
#define MockFaultServer_h

#include "IFaultServer.h"

class MockFaultServer : public IFaultServer
{
public:
    MockFaultServer();
    ~MockFaultServer();

    INT32 ClearFault(const std::string &fault, std::string &response,
                     const bool isFinal = false);

    INT32 SetFault(const std::string &fault,const std::string &description,
                   const std::string &process,std::string &response,
                   const bool isFinal = false,INT32 severity = -1);

    /** member variable to tell how many times ClearFault() was called */
    int m_clearFaultCalled;

    /** vectors to store the parameters passed to ClearFault() */
    vector<std::string> m_clearFaultParamFault;
    vector<bool> m_clearFaultParamIsFinal;

    /** 
     * Vector of responses for ClearFault().  These will need to be set in the 
     * unit tests.
     */ 
    vector<std::string> m_clearFaultResponse;

    /**
     * Vector of return values for ClearFault().  These will need to be set in 
     * the unit tests.
     */
    vector<INT32> m_clearFaultReturnValue;

    /** member variable to tell how many times SetFault() was called */
    int m_setFaultCalled;

    /** vectors to store the parameters passed to SetFault() */
    vector<std::string> m_setFaultParamFault;
    vector<std::string> m_setFaultParamDescription;
    vector<std::string> m_setFaultParamProcess;
    vector<bool> m_setFaultParamIsFinal;
    vector<INT32> m_setFaultParamSeverity;

    /** 
     * Vector of responses for SetFault().  These will need to be set in the 
     * unit tests.
     */ 
    vector<std::string> m_setFaultResponse;

    /**
     * Vector of return values for SetFault().  These will need to be set in 
     * the unit tests.
     */
    vector<INT32> m_setFaultReturnValue;
};

#endif
