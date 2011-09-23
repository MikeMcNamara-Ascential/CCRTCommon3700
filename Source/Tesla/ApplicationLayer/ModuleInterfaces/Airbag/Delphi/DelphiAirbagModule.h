//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DelphiAirbagModule_h
#define DelphiAirbagModule_h
//-----------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
class DelphiAirbagModule : public GenericModuleTemplate<ProtocolFilterType> {
public:
    DelphiAirbagModule();
    virtual ~DelphiAirbagModule();
    virtual string ModuleName();
    BEP_STATUS_TYPE GetSecurityAccess();
};
//-----------------------------------------------------------------------------
#endif //DelphiAirbagModule_h
