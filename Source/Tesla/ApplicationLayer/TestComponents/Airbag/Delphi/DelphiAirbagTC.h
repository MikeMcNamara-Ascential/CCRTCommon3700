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
#ifndef DelphiAirbagTC_h
#define DelphiAirbagTC_h
//-----------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ModuleType>
class DelphiAirbagTC : public GenericTCTemplate<ModuleType> {
public:
    /** Class constructor */
    DelphiAirbagTC();
    /** Class destructor */
    virtual ~DelphiAirbagTC();
    /**
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method will handle the execution of each test step that has been specified by the system for each test 
     * objective that is passed to it.  If the test step is not supported by this class, it will be 
     * passed to GenericTC.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description fo the test step. </li>
     *      </ul>
     * 
     * <b>Supported Test Steps:</b>
     *      <ul>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      </ul>
     * 
     * @return The result of the overall test step.  BEP_UNSUPPORTED_RESPONSE will be returned if the test step is not supported.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    string CommandSecurityAccess(void);
    string EnableAirBagModule(void);
private:
};
//-----------------------------------------------------------------------------
#endif //DelphiAirbagTC_h
