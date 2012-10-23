//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef GatewayTc_h
#define GatewayTc_h

//---------------------------------------------------------------------------------------------------------------------
#include "GenericTCTemplate.cpp"

//---------------------------------------------------------------------------------------------------------------------
template<class ModuleType>
class GatewayTc : public GenericTCTemplate<ModuleType>
{
public:

	GatewayTc();
	virtual ~GatewayTc();

	/**
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * This method will handle the execution of each test step that has been specified by the system for each test
	 * objective that is passed to it.  If the test step is not supported by this class, it will be
	 * passed to GenericTC.
	 * <p>
	 *
	 * @return The result of the overall test step.  
	 *         BEP_UNSUPPORTED_RESPONSE will be returned if the test step is not supported.
	 */
	virtual const string CommandTestStep(const string &value);



private:

	/**
	 * Walk the operator through cycling the ignition.
	 * The vehicle bus will also be monitored to ensure the ignition is indeed cycled.
	 * 
	 * @return Result of cycling the vehicle ignition.
	 */
	string CycleIgnition(void);
};
//---------------------------------------------------------------------------------------------------------------------
#endif //GatewayTc_h
