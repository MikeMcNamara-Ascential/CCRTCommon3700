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
#ifndef SiemensAirbagTC_h
#define SiemensAirbagTC_h
//-----------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ModuleType>
class SiemensAirbagTC : public GenericTCTemplate<ModuleType>
{
public:
    /** Class constructor */
    SiemensAirbagTC();
    /** Class destructor */
    virtual ~SiemensAirbagTC();
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
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Initialze the configuration items for this class.  If the parameter <i>DelayModuleInitialization</i> is false, 
     * the module interface will be initialized.  Otherwise it will be initialized in Setup().  The parameter 
     * <i>LoadStaticTestResults</i> will also be checked to determine if static test results need to be loaded.  The
     * list of module faults to ignore will be saved as will the list of valid module part numbers.  The list of
     * module data items will also be saved.
     * <p>
     * 
     * @param config The configuration data to use to initialize the component.
     */
    virtual void InitializeHook(const XmlNode *config);
    /**
     * Linkup with the module.
     * A low baud initialization sequence will be used to establish communications with the module.
     * 
     * @return Result of linking up with the module.
     */
    string PerformModuleLinkup(void);
    /**
     * The operator will be instructed to verify the airbag lamp is on.
     * 
     * @param state  Airbag state to check for - On or Off.
     * 
     * @return Result of validating the airbag lamp status.
     */
    string CheckAirbagLamp(const string &state);
    /**
     * Airbag system resistance values will be read from the module and the limits will be checked.
     * 
     * @return Result of validating the airbag system resistance values.
     */
    string CheckResistanceValues(void);

private:
    /** Airbag system resistance data */
    XmlNodeMap m_resistanceData;
};
//-----------------------------------------------------------------------------
#endif //SiemensAirbagTC_h
