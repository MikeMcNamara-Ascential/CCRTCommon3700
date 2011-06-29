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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/GenericTc/MockGenericTc.cpp $
// 
// 1     10/31/07 10:21a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "MockGenericTc.h"

//-----------------------------------------------------------------------------
MockGenericTc::MockGenericTc() : GenericTC(),
    m_readDataArraysCalled(0),m_systemWriteCalled(0),
    m_sendSubtestResultWithDetailCalled(0)
{
}

//-----------------------------------------------------------------------------
MockGenericTc::~MockGenericTc()
{
}

//-----------------------------------------------------------------------------
INT32 MockGenericTc::ReadDataArrays(const string &array, int startIdx, 
                                    int endIdx, WHEELDATAARRAY &dataArray)
{
    // store the passed in parameters
    m_readDataArraysParamArray.push_back(array);
    m_readDataArraysParamStartIdx.push_back(startIdx);
    m_readDataArraysParamEndIdx.push_back(endIdx);

    // set the return data array
    dataArray = m_readDataArraysReturnDataArray;

    // return the specified return value and increment the method called counter
    return(m_readDataArraysReturnValue[m_readDataArraysCalled++]);
}

//-----------------------------------------------------------------------------
INT32 MockGenericTc::SystemWrite(const std::string &tag,
                                 const std::string &value)
{
    // store the passed in parameters
    m_systemWriteParamTag.push_back(tag);
    m_systemWriteParamValue.push_back(value);

    // return the specified return value and increment the method called counter
    return(m_systemWriteReturnValue[m_systemWriteCalled++]);
}

//-----------------------------------------------------------------------------
INT32 MockGenericTc::SendSubtestResultWithDetail(
        const std::string &testName,INT32 result,
        const std::string description,const std::string code,
        const std::string firstDetail,
        const std::string firstDetailValue,
        const std::string firstDetailUnits,
        const std::string secondDetail, 
        const std::string secondDetailValue,
        const std::string secondDetailUnits,
        const std::string thirdDetail, 
        const std::string thirdDetailValue,
        const std::string thirdDetailUnits,
        const std::string fourthDetail, 
        const std::string fourthDetailValue,
        const std::string fourthDetailUnits,
        const std::string fifthDetail, 
        const std::string fifthDetailValue,
        const std::string fifthDetailUnits)
{
    // store the passed in parameters
    m_sendSubtestResultWithDetailParamTestName.push_back(testName);
    m_sendSubtestResultWithDetailParamResult.push_back(result);
    m_sendSubtestResultWithDetailParamDescription.push_back(description);
    m_sendSubtestResultWithDetailParamCode.push_back(code);
    m_sendSubtestResultWithDetailParamFirstDetail.push_back(firstDetail);
    m_sendSubtestResultWithDetailParamFirstDetailValue.push_back(firstDetailValue);
    m_sendSubtestResultWithDetailParamFirstDetailUnits.push_back(firstDetailUnits);
    m_sendSubtestResultWithDetailParamSecondDetail.push_back(secondDetail);
    m_sendSubtestResultWithDetailParamSecondDetailValue.push_back(secondDetailValue);
    m_sendSubtestResultWithDetailParamSecondDetailUnits.push_back(secondDetailUnits);
    m_sendSubtestResultWithDetailParamThirdDetail.push_back(thirdDetail);
    m_sendSubtestResultWithDetailParamThirdDetailValue.push_back(thirdDetailValue);
    m_sendSubtestResultWithDetailParamThirdDetailUnits.push_back(thirdDetailUnits);
    m_sendSubtestResultWithDetailParamFourthDetail.push_back(fourthDetail);
    m_sendSubtestResultWithDetailParamFourthDetailValue.push_back(fourthDetailValue);
    m_sendSubtestResultWithDetailParamFourthDetailUnits.push_back(fourthDetailUnits);
    m_sendSubtestResultWithDetailParamFifthDetail.push_back(fifthDetail);
    m_sendSubtestResultWithDetailParamFifthDetailValue.push_back(fifthDetailValue);
    m_sendSubtestResultWithDetailParamFifthDetailUnits.push_back(fifthDetailUnits);

    // return the specified return value and increment the method called counter
    return(m_sendSubtestResultWithDetailReturnValue[m_sendSubtestResultWithDetailCalled++]);
}

