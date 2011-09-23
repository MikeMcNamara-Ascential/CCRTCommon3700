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
#ifndef SiemensAirbagModule_h
#define SiemensAirbagModule_h
//-----------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
class SiemensAirbagModule : public GenericModuleTemplate<ProtocolFilterType>
{
public:
    SiemensAirbagModule();
    virtual ~SiemensAirbagModule();
    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @return Siemens Airbag Module
     */
    virtual string ModuleName();
    /**
     * Perform a low baud init sequence in order to establish communications with the module.
     * 
     * @return Status of establishing communications with the module.
     */
    BEP_STATUS_TYPE PerformModuleLinkup(void);
    /**
     * Read module faults.
     * <p><b>Description:</b><br>
     * Read the fault data from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadFaults </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param faultCodes The fault data read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);
};
//-----------------------------------------------------------------------------
#endif //SiemensAirbagModule_h
