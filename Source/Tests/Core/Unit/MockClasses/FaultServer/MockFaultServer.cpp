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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/FaultServer/MockFaultServer.cpp $
// 
// 1     1/18/07 4:46p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <iostream>
#include "MockFaultServer.h"

using namespace std;

//=============================================================================
MockFaultServer::MockFaultServer() :
    m_clearFaultCalled(0),m_setFaultCalled(0)
{
}

//=============================================================================
MockFaultServer::~MockFaultServer()
{

}

//=============================================================================
INT32 MockFaultServer::ClearFault(const std::string &fault, 
                                  std::string &response,
                                  const bool isFinal /*=false*/)
{
    // store the passed in parameters
    m_clearFaultParamFault.push_back(fault);
    m_clearFaultParamIsFinal.push_back(isFinal);

    // return the specified message
    response = m_clearFaultResponse[m_clearFaultCalled];

    // increment the method called counter and return the specified value
    return(m_clearFaultReturnValue[m_clearFaultCalled++]);
}

//=============================================================================
INT32 MockFaultServer::SetFault(const std::string &fault,
                                const std::string &description,
                                const std::string &process,
                                std::string &response,
                                const bool isFinal /*=false*/,
                                INT32 severity /*=-1*/)
{
    // store the passed in parameters
    m_setFaultParamFault.push_back(fault);
    m_setFaultParamDescription.push_back(description);
    m_setFaultParamProcess.push_back(process);
    m_setFaultParamIsFinal.push_back(isFinal);
    m_setFaultParamSeverity.push_back(severity);

    // return the specified message
    response = m_setFaultResponse[m_setFaultCalled];

    // increment the method called counter and return the specified value
    return(m_setFaultReturnValue[m_setFaultCalled++]);
}
