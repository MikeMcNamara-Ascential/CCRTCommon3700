//******************************************************************************
// Description:
// Test steps specific to Isuzu Emissions testing.
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
#ifndef IsuzuClearFaultsTc_h
#define IsuzuClearFaultsTc_h
//-------------------------------------------------------------------------------------------------
#include "BepTimer.h"
#include "GenericEmissionsTCTemplate.cpp"
#include "BackgroundSwitchMonitor.cpp"
#include "BackgroundRangeCheckMonitor.cpp"
#include <vector>

//class XmlNodeMap;

//-------------------------------------------------------------------------------------------------
#define ISUZU_EMISSIONS_PULSE_CODE  SYSTEM_PULSE+7
#define SWITCH_MON_PULSE            10
#define SENSOR_MON_PULSE            11
//-------------------------------------------------------------------------------------------------
template<class ModuleType>
class IsuzuClearFaultsTc : public GenericEmissionsTCTemplate<ModuleType>
{
public:
    IsuzuClearFaultsTc();
    virtual ~IsuzuClearFaultsTc();

    /**
     * Execute the specified test step.
     * <p><b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the
     * system for each test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * @param value  The value that was commanded by the test sequencer.
     *
     * @return The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

    /**
     * <b>Description:</b>
     * Handle the specified pulse.  This method simply calls 
     * BepComponent::HandlePulse().
     *
     * @param code  The pulse code to process.
     * @param value The pulse value to process.
     * 
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);


protected:

    /**
     * Perform GenericEmissionsTCTemplate intitialization.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method overloads the base class to allow the GenericEmissionsTCTemplate test component to initialize any
     * data sepcific to its needs.
     * <p>
     * 
     * @param config The configuration data to use for initializing the test component.
     */
    virtual void InitializeHook(const XmlNode *config);


private:

    /**
     * Read the current fault count from the module.
     * If any faults are present, the test may be aborted depending on parameter settings.
     * 
     * @return Result of check the module for faults.
     */
    string CheckFaultCount(void);
    virtual string ReadFaults(void);
    /**
     * Simple delay before reading the DTCs of the emission.
     * 
     * @return Result of the function (skipped or passed)
     */
    virtual string DelayBeforeDtcRead(void);


    /**
     * Checks for a locked module before trying to clear faults
     * 
     * @return Result of clearing the faults from the module
     */
    virtual string ClearFaults(void);

    virtual string ConditionalFaultClear(void);
    string EnterNormalMode(void);
    string DisableNormalComms(void);
    string ReadMemoryLocation(void);    
};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuClearFaultsTc_h
