//*************************************************************************
// FILE DESCRIPTION:
//  Tire Pressure Monitor Test Component.
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
#include "GenericTCTemplate.cpp"
#include "BepTimer.h"
#include <time.h>
//-------------------------------------------------------------------------------------------------
#ifndef TpmsTc_h
#define TpmsTc_h
//-------------------------------------------------------------------------------------------------

template <class ModuleType> 
class TpmsTc : public GenericTCTemplate<ModuleType>
{
public:

	TpmsTc();
	virtual ~TpmsTc();

	/**
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * This method will handle the execution of each test step that has been specified by the system for each test
	 * objective that is passed to it.  If the test step is not supported by this class, it will be
	 * passed to GenericTC.
	 * <p>
	 * @return The result of the overall test step.  BEP_UNSUPPORTED_RESPONSE will be returned if the test step is not supported.
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

	/**
	 * Register the component with the system.
	 * <p><b>Category:</b> Utility
	 * <p><b>Description:</b><br>
	 * Command the component to perform any system registration functions necessary.  The component testing status will
	 * be set to TEST_STATUS_IN_PROGRESS.  The base class will then be called to complete system registration.
	 * <p>
	 * <b>Functions Called:</b>
	 *      <ul>
	 *      <li> UpdateTestStatus() </li>
	 *      <li> BepServer::Register() </li>
	 *      </ul>
	 *
	 * @return BEP_SUCCESS_RESPONSE if the registration was successful.
	 *         BEP_FAILURE_RESPONSE if the registration was not successful.
	 */
	virtual const string Register(void);


	#define TPMS_TC_PULSE_CODE		SYSTEM_PULSE+19
	#define TPMS_SPEED_TIME_PULSE	10





protected:

	/**
	 * Check the TPMS module data to determine if the system is OK.
	 * 
	 * @return  Result of validating the TPMS counters and factory mode.
	 */
	string CheckTpmsResults(void);

	/**
	 * Start the timer for monitoring vehicle speed above threshold.
	 * 
	 * @return  Result of stating the vehicle above speed threshold timer.
	 */
	string StartSpeedTimer(void);

	/**
	 * Wait for the vehicle speed above required threshold time to be complete.
	 * The operator will be prompted to maintain speed until the time is complete.
	 * 
	 * @return  Result of waiting for required time above speed to be complete
	 */
	string WaitForTimeAtSpeedComplete(void);




private:

	/**
	 * Get/Set the flag indicating if the vehicle is equipped with TPMS.
	 * 
	 * @param equipped  Flag indicating if the vehicle is equipped with TPMS
	 * 
	 * @return  Flag indicating if the vehicle is equipped with TPMS
	 */
	const bool& TpmEquipped(const bool *equipped = NULL);

	/** 
	 * Check if the vehicle has been above the required speed for the required amount of time.
	 * 
	 * @return Status of updating the speed above time status.
	 */
	INT32 TpmUpdateSpeedTime(void);


	// Flag indicating if the vehicle is equipped with TPMS
	bool m_tpmEquipped;
	// Flag indicating vehicle speed has been above threshold for required time.
	bool m_timeAtSpeedDone;
	double m_remainingTimeAboveSpeed;
	// Vehicle above speed threshold start time
	time_t m_tpmSpeedStartTime;
	// Timer to use for check vehicle speed above threshold
	BepTimer m_speedCheckTimer;
	// Frame counts read from the vehicle
	vector<UINT16> m_initialFrameCounts;
	vector<UINT16> m_finalFrameCounts;
};

//-------------------------------------------------------------------------------------------------
#endif //TpmsTc_h
