//******************************************************************************
// Description:
//  A mock generic test component class that is used in unit testing.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/GenericTc/MockGenericTc.h $
// 
// 1     10/31/07 10:22a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockGenericTc_h
#define MockGenericTc_h

#include "GenericTC.h"

class MockGenericTc : public GenericTC
{
public:
    MockGenericTc();
    virtual ~MockGenericTc();

    //-------------------------------------------------------------------------
    virtual INT32 ReadDataArrays(const string &array, int startIdx, int endIdx, 
                                 WHEELDATAARRAY &dataArray);

    /** member variable to tell how many times ReadDataArrays() is called */
    int m_readDataArraysCalled;

    /** member variables to store the variables passed to ReadDataArrays() */
    vector<string> m_readDataArraysParamArray;
    vector<int>    m_readDataArraysParamStartIdx;
    vector<int>    m_readDataArraysParamEndIdx;

    /**
     * Return data values for ReadDataArrays().  These should be set in the 
     * unit tests.
     */
    WHEELDATAARRAY m_readDataArraysReturnDataArray;

    /** 
     * Return value for ReadDataArrays().  This should be set in the unit 
     * tests. 
     */
    vector<BEP_STATUS_TYPE> m_readDataArraysReturnValue;

    //-------------------------------------------------------------------------
    virtual INT32 SystemWrite(const std::string &tag,const std::string &value);

    /** member variable to tell how many times SystemWrite() is called */
    int m_systemWriteCalled;

    /** member variables to store the variable passed to SystemWrite() */
    vector<std::string> m_systemWriteParamTag;
    vector<std::string> m_systemWriteParamValue;

    /**
     * Return value for SystemWrite().  This should be set in the unit tests.
     */
    vector<BEP_STATUS_TYPE> m_systemWriteReturnValue;

    //-------------------------------------------------------------------------
    virtual INT32 SendSubtestResultWithDetail(
        const std::string &testName,INT32 result,
        const std::string description = "",const std::string code = "0000",
        const std::string firstDetail = "",
        const std::string firstDetailValue = "",
        const std::string firstDetailUnits = "",
        const std::string secondDetail = "", 
        const std::string secondDetailValue = "",
        const std::string secondDetailUnits = "",
        const std::string thirdDetail = "", 
        const std::string thirdDetailValue = "",
        const std::string thirdDetailUnits = "",
        const std::string fourthDetail = "", 
        const std::string fourthDetailValue = "",
        const std::string fourthDetailUnits = "",
        const std::string fifthDetail = "", 
        const std::string fifthDetailValue = "",
        const std::string fifthDetailUnits = "");

    /** 
     * member variable to tell how many times SendSubtestResultWithDetail() is
     * called.
     */
    int m_sendSubtestResultWithDetailCalled;

    /** 
     * member variables to store the variables passed to 
     * SendSubtestResultWithDetail()
     */
    vector<std::string> m_sendSubtestResultWithDetailParamTestName;
    vector<INT32>  m_sendSubtestResultWithDetailParamResult;
    vector<std::string> m_sendSubtestResultWithDetailParamDescription;
    vector<std::string> m_sendSubtestResultWithDetailParamCode;
    vector<std::string> m_sendSubtestResultWithDetailParamFirstDetail;
    vector<std::string> m_sendSubtestResultWithDetailParamFirstDetailValue;
    vector<std::string> m_sendSubtestResultWithDetailParamFirstDetailUnits;
    vector<std::string> m_sendSubtestResultWithDetailParamSecondDetail;
    vector<std::string> m_sendSubtestResultWithDetailParamSecondDetailValue;
    vector<std::string> m_sendSubtestResultWithDetailParamSecondDetailUnits;
    vector<std::string> m_sendSubtestResultWithDetailParamThirdDetail;
    vector<std::string> m_sendSubtestResultWithDetailParamThirdDetailValue;
    vector<std::string> m_sendSubtestResultWithDetailParamThirdDetailUnits;
    vector<std::string> m_sendSubtestResultWithDetailParamFourthDetail;
    vector<std::string> m_sendSubtestResultWithDetailParamFourthDetailValue;
    vector<std::string> m_sendSubtestResultWithDetailParamFourthDetailUnits;
    vector<std::string> m_sendSubtestResultWithDetailParamFifthDetail;
    vector<std::string> m_sendSubtestResultWithDetailParamFifthDetailValue;
    vector<std::string> m_sendSubtestResultWithDetailParamFifthDetailUnits;

    /**
     * Return value for SendSubtestResultWithDetail().  This should be set in 
     * the unit tests.
     */
    vector<INT32> m_sendSubtestResultWithDetailReturnValue;
};

#endif
