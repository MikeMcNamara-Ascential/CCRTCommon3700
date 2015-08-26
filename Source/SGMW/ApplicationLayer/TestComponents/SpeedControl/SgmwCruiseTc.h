//******************************************************************************
// Description:
//  SGMW Cruise testing for all types of vehicles.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef SgmwCruiseTc_h
#define SgmwCruiseTc_h
//-------------------------------------------------------------------------------------------------
#include "GenericSpeedControlTC.cpp"
//-------------------------------------------------------------------------------------------------
template <class ModuleType>
class SgmwCruiseTc : public GenericSpeedControlTC<ModuleType>
{
public: 
	SgmwCruiseTc();
	virtual ~SgmwCruiseTc();

	virtual const string CommandTestStep(const string &value);




protected:

	/**
	 * Verify that the cruise SET feature is functioning properly.
	 * <p><b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * Wait for driver to set speed control and check for stable speed.  If the speed control does not set, the operator
	 * can press the fail button instead of waiting for the test to time out.  Once the vehicle speed is stable after
	 * the operator presses the Set button, the vehicle speed will be stored as the target speed.
	 * <br><p>
	 *
	 * @return Result of checking the speed control Set functionality.
	 */
	virtual const string CheckSetEngagement(void);

	/**
	 * Verify vehicle speed changes when the Accel/Decel button is tapped.
	 * <br><p>
	 * <b>Category:</b> Test Step
	 * <br><p>
	 * <b>Description:</b><br>
	 * This test step assumes the speed control is currently engaged.
	 * The operator will be instructed to press the accel/decel button.  The vehicle speed will be monitored to
	 * ensure it has changed.
	 * <br><p>
	 *
	 * @param direction Direction for the operator to change speed.
	 *
	 * @return Result of verifying the vehicle speed changes when the Accel/Decel vuttons are pressed.
	 */
	virtual const string CheckTapSpeedChange(const string &direction);

	virtual string ReadFaultCount(void);

	string WaitForStableSpeed(float minLimit, float maxLimit);

};
//-------------------------------------------------------------------------------------------------
#endif //SgmwCruiseTc_h
