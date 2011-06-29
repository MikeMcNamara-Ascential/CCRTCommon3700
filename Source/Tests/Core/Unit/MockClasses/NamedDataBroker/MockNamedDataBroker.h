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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/NamedDataBroker/MockNamedDataBroker.h $
// 
// 3     5/01/07 1:29p Rwiersem
// Changes for the 07050201 core release:
// 
// Added GetNext().
// 
// 2     2/22/07 11:32a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a Write() method that takes an XmlNode.
// 
// 1     10/26/06 2:01p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockNamedDataBroker_h
#define MockNamedDataBroker_h

#include "INamedDataBroker.h"

class MockNamedDataBroker : public INamedDataBroker
{
public:
    MockNamedDataBroker();
    ~MockNamedDataBroker();

    //-------------------------------------------------------------------------
    INT32 Write(const std::string &tag, const std::string &value,
                std::string &response, const bool IsFinal = false);

    INT32 Write(const XmlNode *node, std::string &response, const bool IsFinal);

    /** member variable to tell how many times Write() is called */
    int m_writeCalled;

    /** vectors to store the variables passed to Write() */
    vector<std::string> m_writeParamTag;
    vector<std::string> m_writeParamNodeName;
    vector<std::string> m_writeParamNodeValue;
    vector<std::string> m_writeParamValue;
    vector<bool> m_writeParamIsFinal;

    /**
     * Vector of return values for Write().  These need to be set in the unit 
     * tests.
     */ 
    vector<INT32> m_writeReturnValue;

    /**
     * Vector of return responses for Write().  These need to be set in the 
     * unit tests.
     */ 
    vector<std::string> m_writeReturnResponse;

    //-------------------------------------------------------------------------
    INT32 Read(const std::string &tag, std::string &response,
               const bool IsFinal = false);

    /** member variables to tell how many times Read() is called */
    int m_readCalled;

    /** vectors to store the variables passed to Read() */
    vector<std::string> m_readParamTag;
    vector<bool> m_readParamIsFinal;

    /**
     * Vector of return values for Read().  These need to be set in the unit 
     * tests.
     */ 
    vector<INT32> m_readReturnValue;

    /**
     * Vector of return responses for Read().  These need to be set in the unit 
     * tests.
     */ 
    vector<std::string> m_readReturnResponse;

    //-------------------------------------------------------------------------
    const INT32 GetByTag(const std::string tag, std::string &value, 
                         std::string &message);

    /** member variable to tell how many times GetByTag() is called */
    int m_getByTagCalled;

    /** 
     * Amount of time (in mS) to delay before returning the value.  This allows
     * us to simulate changing data over time.
     */ 
    int m_getByTagDelay;

    /** vector to store the variables passed to GetByTag() */
    vector<std::string> m_getByTagParamTag;

    /**
     * Vector of return values for GetByTag().  These need to be set in the 
     * unit tests.
     */
    vector<INT32> m_getByTagReturnValue;

    /**
     * Vectors for the value and message returned from GetByTag().  These need
     * to be set in the unit tests.
     */
    vector<std::string> m_getByTagReturnDataValue;
    vector<std::string> m_getByTagReturnMessage;

    //-------------------------------------------------------------------------
    const INT32 GetNext(std::string &tag, std::string &value,
                        std::string &message);

    /** member variable to tell how many times GetNext() is called */
    int m_getNextCalled;

    /**
     * Vector of return values for GetNext().  These need to be set in the unit 
     * tests.
     */
    vector<INT32> m_getNextReturnValue;

    /**
     * Vectors for the value and message returned from GetByTag().  These need
     * to be set in the unit tests.
     */
    vector<std::string> m_getNextReturnDataValue;
    vector<std::string> m_getNextReturnMessage;

    // NOTE:  The returned tag will be set based on the last call to Read()
};

#endif
