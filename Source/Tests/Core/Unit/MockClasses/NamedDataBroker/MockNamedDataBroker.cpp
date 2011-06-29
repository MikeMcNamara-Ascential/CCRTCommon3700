//******************************************************************************
// Description:
//  The mock named data broker class.  This is used in unit testing to verify
//  the correct information was sent to the named data broker.  The data to
//  return from the named data broker can also be set in the unit tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/NamedDataBroker/MockNamedDataBroker.cpp $
// 
// 3     5/01/07 1:28p Rwiersem
// Changes for the 07050201 core release:
// 
// Added GetNext().
// 
// 2     2/22/07 11:31a Rwiersem
// 
// 1     10/26/06 2:01p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <iostream>
#include "MockNamedDataBroker.h"

using namespace std;

//=============================================================================
MockNamedDataBroker::MockNamedDataBroker() :
    m_writeCalled(0),m_readCalled(0),m_getByTagCalled(0),m_getByTagDelay(0),
    m_getNextCalled(0)
{
}

//=============================================================================
MockNamedDataBroker::~MockNamedDataBroker()
{
}

//=============================================================================
INT32 MockNamedDataBroker::Write(const std::string &tag,
                                 const std::string &value,std::string &response, 
                                 const bool IsFinal/*=false*/)
{
    // store the passed in parameters
    m_writeParamTag.push_back(tag);
    m_writeParamValue.push_back(value);
    m_writeParamIsFinal.push_back(IsFinal);

    // return the specified response
    response = m_writeReturnResponse[m_writeCalled];

    // increment the method called counter and return the specified value
    return(m_writeReturnValue[m_writeCalled++]);
}

//=============================================================================
INT32 MockNamedDataBroker::Write(const XmlNode *node, std::string &response, 
                                 const bool IsFinal)
{
    // store the passed in parameters
    m_writeParamNodeName.push_back(node->getName());
    m_writeParamNodeValue.push_back(node->getValue());
    m_writeParamIsFinal.push_back(IsFinal);

    // return the specified response
    response = m_writeReturnResponse[m_writeCalled];

    // increment the method called counter and return the specified value
    return(m_writeReturnValue[m_writeCalled++]);
}

//=============================================================================
INT32 MockNamedDataBroker::Read(const std::string &tag, std::string &response,
                                const bool IsFinal/*= false*/)
{
    // store the passed in parameters
    m_readParamTag.push_back(tag);
    m_readParamIsFinal.push_back(IsFinal);

    // return the specified response
    response = m_readReturnResponse[m_readCalled];

    // increment the method called counter and return the specified value
    return(m_readReturnValue[m_readCalled++]);
}

//=============================================================================
const INT32 MockNamedDataBroker::GetByTag(const std::string tag,
                                          std::string &value,
                                          std::string &message)
{
    // store the passed in parameters
    m_getByTagParamTag.push_back(tag);

    // return the specified data and message
    value = m_getByTagReturnDataValue[m_getByTagCalled];
    message = m_getByTagReturnMessage[m_getByTagCalled];

    // delay if required
    if(m_getByTagDelay != 0) BposSleep(m_getByTagDelay);

    // increment the method called counter and return the specified value
    return(m_getByTagReturnValue[m_getByTagCalled++]);
}

//=============================================================================
const INT32 MockNamedDataBroker::GetNext(std::string &tag, std::string &value,
                                         std::string &message)
{
    // return the last tag passed to Read()
    tag = m_readParamTag[m_readCalled - 1];

    // return the specified data and message
    value = m_getNextReturnDataValue[m_getNextCalled];
    message = m_getNextReturnMessage[m_getNextCalled];

    // increment the method called counter and return the specified value
    return(m_getNextReturnValue[m_getNextCalled++]);
}
