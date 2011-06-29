//******************************************************************************
// Description:
//  A mock protocol filter class that is used in unit testing.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/ProtocolFilter/MockProtocolFilter.h $
// 
// 1     1/18/07 4:50p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockProtocolFilter_h
#define MockProtocolFilter_h

#include "ProtocolFilter.h"

class MockProtocolFilter : public ProtocolFilter
{
public:
    MockProtocolFilter(KeepAliveTimer_t &lastTxTime, 
					   StopCommsBepCondVar *stopCommsBepCondVar = NULL, 
					   BepMutex *commsInUse=NULL);
    virtual ~MockProtocolFilter();

    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, 
                                                SerialString_t &reply, 
                                                SerialArgs_t *args = NULL);

    /** member variable to tell how many times GetModuleData() is called */
    int m_getModuleDataCalled;

    /** member variable to tell if GetModuleData should throw an exception */
    bool m_getModuleDataShouldThrow;

    /** vectors to store the variables passed to GetModuleData() */
    vector<std::string> m_getModuleDataParamMessageTag;
    vector<SerialArgs_t *> m_getModuleDataParamArgs;

    /**
     * Vector of replies for GetModuleData().  These need to be set in the unit 
     * tests.
     */ 
    vector<SerialString_t> m_getModuleDataReply;
    
    /**
     * Vector of return values for GetModuleData().  These need to be set in the 
     * unit tests.
     */ 
    vector<BEP_STATUS_TYPE> m_getModuleDataReturnValue;

    virtual const SerialString_t ExtractModuleData(
        SerialString_t &moduleResponse);

    /** member variable to tell how many times ExtractModuleData() is called */
    int m_extractModuleDataCalled;

    /** vector to store the variable passed to ExtractModuleData() */
    vector<SerialString_t> m_extractModuleDataParamModuleResponse;

    /**
     * Vector of return values for ExtractModuleData().  These need to be set in 
     * the unit tests.
     */ 
    vector<SerialString_t> m_extractModuleDataReturnValue;
};

#endif
