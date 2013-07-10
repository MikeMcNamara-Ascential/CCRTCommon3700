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
#ifndef RwalBrakeTest_h
#define RwalBrakeTest_h
//-------------------------------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"
#include "ILogicalPort.h"
//-------------------------------------------------------------------------------------------------

template<class ModuleType>
class RwalBrakeTest : public GenericABSTCTemplate<ModuleType>
{
public:
	RwalBrakeTest();
	virtual ~RwalBrakeTest();

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


    float GetAverageRWALSpeed(void);

protected:

	/**
	 * Verify the RWAL sensor reading is in tolerance.
	 * 
	 * @return Result of validating the RWAL sensor reading.
	 */
	string ValidateRwalSensor(void);



private:

	/**
	 * Check the build data to determine if the vehicle is equipped with RWAL.
	 * 
	 * @return Flag indicating if the vehicle is equipped with RWAL.
	 */
	bool IsRwalEquipped(void);
	/**
	 * Logical Port interface used to read data from the sound level meter
	 */
	ILogicalPort	m_rwalCommPort;
};

//-------------------------------------------------------------------------------------------------
#endif //RwalBrakeTest_h
