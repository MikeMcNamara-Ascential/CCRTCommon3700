//******************************************************************************
// Description:
//  RWAL Brake test component class
//
//==============================================================================
// Copyright (c) 2012 - Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#ifndef MahindraTransmission_h
#define MahindraTransmission_h
//-------------------------------------------------------------------------------------------------
#include "GenericTransmissionTCTemplate.cpp"

//-------------------------------------------------------------------------------------------------

template<class ModuleType>
class MahindraTransmission : public GenericTransmissionTCTemplate<ModuleType>
{
public:
	MahindraTransmission();
	virtual ~MahindraTransmission();

	/**
	 * Sequence a test step.
	 * <p><b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * This is the method that will handle the execution of each test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded to execute the component tests.
	 * <br><p>
	 *
	 * @param value    This is the value that was commanded by the test sequencer.
	 * @return  The result of the test step that was executed.
	 */
	virtual const string CommandTestStep(const string &value);

	/**
	 * Initialization method to set up the MachineTC
	 * component.
	 *
	 * @param configFile Configuration file to use for initialization.
     * @since Version 1.0
	 */
    virtual void InitializeHook(const XmlNode *config);


protected:

     /**
	 * Verify the TPMS flags are correct
	 * 
	 * @return Result of validating the RWAL sensor reading.
	 */
	string CheckTpmsStatus(void);

     /**
	 * Cruise control test
	 * 
	 * @return Result of validating cruise control
	 */
	string CruiseTest(void);

     /**
	 * AT Learn test
	 * 
	 * @return Result of validating cruise control
	 */
	string ATLearnTest(void);

     /**
	 * Misfire Learn test
	 * 
	 * @return Result of validating cruise control
	 */
	string MisfireTest(void);


    /**
    * MaintainSpeed
    * 
    * @return Result of maintaining speed
    */
	string MaintainSpeed(void);



private:


    /** TCU Module **/
    ModuleType m_tcuModule;

    /** Cruise Module *   */
    ModuleType m_cruiseModule;

    /** TPMS Module *   */
    ModuleType m_tpmsModule;
};

//-------------------------------------------------------------------------------------------------
#endif //MahindraTransmission_h
