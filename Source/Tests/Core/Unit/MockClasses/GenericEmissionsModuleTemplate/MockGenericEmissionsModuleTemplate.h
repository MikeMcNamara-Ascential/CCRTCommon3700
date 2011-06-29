//******************************************************************************
// Description:
//  A mock generic emissions module template class that is used in unit testing.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/MockClasses/GenericEmissionsModuleTemplate/MockGenericEmissionsModuleTemplate.h $
// 
// 2     5/01/07 1:28p Rwiersem
// Changes for the 07050201 core release:
// 
// Added StartModuleKeepAlive() and StopModuleKeepAlive().  Added multiple
// GetInfo() methods.
// 
// 1     1/18/07 4:47p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MockGenericEmissionsModuleTemplate_h
#define MockGenericEmissionsModuleTemplate_h

#include "GenericEmissionsModuleTemplate.cpp"

template <class ProtocolFilterType>
class MockGenericEmissionsModuleTemplate : 
    public GenericEmissionsModuleTemplate<ProtocolFilterType>
{
public:
    MockGenericEmissionsModuleTemplate();
    virtual ~MockGenericEmissionsModuleTemplate();

    //-------------------------------------------------------------------------
    virtual bool InitializeHook(const XmlNode *configNode);

    /** member variable to tell how many times InitializeHook() is called */
    int m_initializeHookCalled;

    /** member variable to store the variable passed to InitializeHook() */
    const XmlNode *m_initializeHookParamConfigNode;

    /** 
     * Return value for InitializeHook().  This should be set in the unit 
     * tests.
     */ 
    bool m_initializeHookReturnValue;

    //-------------------------------------------------------------------------
    /** Overriden StartModuleKeepAlive() method */ 
    virtual BEP_STATUS_TYPE StartModuleKeepAlive(void) throw(ModuleException);

    /** 
     * member variable to tell how many times StartModuleKeepAlive() is 
     * called 
     */
    int m_startModuleKeepAliveCalled;

    /** 
     * Return value for StartModuleKeepAlive().  This should be set in the unit 
     * tests.
     */ 
    BEP_STATUS_TYPE m_startModuleKeepAliveReturnValue;

    //-------------------------------------------------------------------------
    /** Overriden StopModuleKeepAlive() method */ 
    virtual BEP_STATUS_TYPE StopModuleKeepAlive(void) throw(ModuleException);

    /** 
     * member variable to tell how many times StopModuleKeepAlive() is called 
     */
    int m_stopModuleKeepAliveCalled;

    /** 
     * Return value for StopModuleKeepAlive().  This should be set in the unit 
     * tests.
     */ 
    BEP_STATUS_TYPE m_stopModuleKeepAliveReturnValue;

    //-------------------------------------------------------------------------
    /** Overriden GetInfo() methods */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    string &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    UINT16 &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    float &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    UINT8 &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(
        string methodName,FaultVector_t &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    INT32 &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, 
                                    bool &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(
        string methodName, vector<UINT16>&value) throw(ModuleException);

    /** member variable to tell how many times GetInfo() is called */
    int m_getInfoCalled;

    /** member variable to store the variable passed to GetInfo */
    vector<string> m_getInfoParamMethodName;

    /**
     * Return data values for GetInfo().  These should be set in the unit tests.
     */
    vector<string> m_getInfoReturnDataValueString;
    vector<UINT16> m_getInfoReturnDataValueUint16;
    vector<float>  m_getInfoReturnDataValueFloat;
    vector<UINT8>  m_getInfoReturnDataValueUint8;
    FaultVector_t  m_getInfoReturnDataValueFaultVector;
    vector<INT32>  m_getInfoReturnDataValueInt32;
    vector<bool>   m_getInfoReturnDataValueBool;
    vector<UINT16> m_getInfoReturnDataValueVectorUint16;

    /** Return value for GetInfo().  This should be set in the unit tests. */
    vector<BEP_STATUS_TYPE> m_getInfoReturnValue;
};

#endif
