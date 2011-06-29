//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the VW engine contoller module.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#ifndef VwEngineControlModule_h
#define VwEngineControlModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilter>
class VwEngineControlModule : public GenericModuleTemplate<ProtocolFilter>
{
public:
    VwEngineControlModule();
    virtual ~VwEngineControlModule();

    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @return VW Engine Module
     */
    virtual string ModuleName();

    /**
     * Enter diagnostic mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to enter a new diagnostic session.
     * <p>
     *
     * @return Status of commanding the module to enter diagnostic mode.
     * @throws ModuleException if object needed is null
     */
    virtual BEP_STATUS_TYPE EnterDiagnosticMode(void);
};

//-------------------------------------------------------------------------------------------------
#endif //VwEngineControlModule_h
