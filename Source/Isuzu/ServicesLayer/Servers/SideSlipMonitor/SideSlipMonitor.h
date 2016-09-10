//******************************************************************************
// Description:
//  Server used to monitor the side slip reading.
//
//==============================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#ifndef SideSlipMonitor_h
#define SideSlipMonitor_h

//-------------------------------------------------------------------------------------------------
#include "BepServer.h"
#include "BepSync.h"
#include "BepTimer.h"
#include "INamedDataBroker.h"

//-------------------------------------------------------------------------------------------------
#define SIDE_SLIP_MONITOR_PULSE_CODE	SYSTEM_PULSE+17
#define SIDE_SLIP_PULSE					10
#define INTERAXLE_GAP_PULSE				11
#define AXLE_TIME_LIMIT_PULSE			12

//-------------------------------------------------------------------------------------------------
class SideSlipMonitor : public BepServer
{
public:
	SideSlipMonitor();
	virtual ~SideSlipMonitor();

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
	 * <b>Description:</b>
	 * Method for initialization passing an XML node.
	 *
	 * @param document Pointer to a parsed configuration document.
	 */
	virtual void Initialize(const XmlNode *document);

	/**
	 * <b>Description:</b>
	 * Method that is called to command the component to perform any system 
	 * registration functions necessary.
	 *
	 * @return BEP_SUCCESS_RESPONSE if the registration was successful
	 *         BEP_FAILURE_RESPONSE if the registration was not successful
	 */
	virtual const string Register(void);




protected:

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 */
	virtual const string Publish(const XmlNode *node);

	/**
	 * Read the current side slip value and record it.
	 * 
	 * @return Status up reading the current side slip value.
	 */
	INT32 UpdateSideSlipValue();




private:

	BepTimer m_sideSlipTimer;
	BepTimer m_axleGapTimer;
	BepTimer m_axleTimeLimitTimer;
	INamedDataBroker *m_ndb;

	float m_axleCount;
	float m_conversionFactor;
	float m_currentMaxSideSlipValue;
};
//-------------------------------------------------------------------------------------------------
#endif //SideSlipMonitor_h
