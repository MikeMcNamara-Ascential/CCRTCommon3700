//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The  Tesla Host Interface Server will interface to the Tesla DVT.
//  Messages will be received from the DVT and processed.  
//  A response will be generated to transmit back to the DVT.
//
//===========================================================================
// Copyright (c) 2017- Burke E. Porter Machinery
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
//*************************************************************************
#ifndef TeslaHostInterface_h
#define TeslaHostInterface_h
//-------------------------------------------------------------------------------------------------
#include "BepServer.h"
#include "BepTimer.h"
#include "SerialChannel.h"
#include "IcmDataSupplier.h"
#include "TeslaDataMessage.h"

//-------------------------------------------------------------------------------------------------
#define TESLA_HI_PULSE_CODE		SYSTEM_PULSE+6
#define ICM_DATA_PULSE			15
#define SCHEDULED_MSG_PULSE     16

typedef struct _DvtMarker
{
    string name;    // DVT marker name
    INT32 index;   // Array index of the marker tag
} DvtMarker;
typedef vector<DvtMarker*>       DvtMarkers;

//-------------------------------------------------------------------------------------------------
// const params
const UINT8 DvtNAK = 0x15;
const UINT8 DvtACK = 0x06;

const UINT8 MSG_TYPE_BYTE    = 1;
const UINT8 MSG_SUBTYPE_BYTE = 2;


//-------------------------------------------------------------------------------------------------
// DVT Marker Types
const string ABS_START   = "AbsStart";
const string ABS_END     = "AbsEnd";
const string BRAKE_START = "BrakeStart";
const string BRAKE_END   = "BrakeEnd";
const string DRAG_START  = "DragStart";
const string DRAG_END    = "DragEnd";


//-------------------------------------------------------------------------------------------------
// Message Wrapper
const UINT8 STX = 0x02;
const UINT8 ETX = 0x03;

// Message Types
const UINT8 ABS_RESULTS        = 0x41;
const UINT8 FORCE_MEASUREMENTS = 0x4D;
const UINT8 PERIODIC_MSG       = 0x50;
const UINT8 START_ABS_TEST     = 0x56;
const UINT8 START_FORCE_TEST   = 0x46;
const UINT8 STOP_ABS_TEST      = 0x54;
const UINT8 STOP_FORCE_TEST    = 0x53;
const UINT8 TEST_CYCLE_CONTROL = 0x43;

// Message Sub-types
const UINT8 BRAKE_FORCE_TEST   = 0x42;
const UINT8 DRAG_FORCE_TEST    = 0x44;
const UINT8 START_TEST         = 0x42;
const UINT8 STOP_TEST          = 0x45;

// Forward Declarations
class TeslaDataMessage;


//-------------------------------------------------------------------------------------------------
class TeslaHostInterface : public BepServer
{
public:
	TeslaHostInterface();
	virtual ~TeslaHostInterface();

	/**
	 * <b>Description:</b>
	 * Method for initialization passing in the file name.
	 *
	 * @param fileName Name of the XML configuration file to be used to
	 *                 initialize.
	 */
	virtual void Initialize(const string &fileName);

	/**
	 * <b>Description:</b>
	 * Method for initialization passing an XML node.
	 *
	 * <b>Configuration Items:</b>
	 *     <ul>
	 *     <li> Setup/Debug </li>
	 *     <li> Setup/DataTags </li>
	 *     </ul>
	 *
	 * @param document Pointer to a parsed configuration document.
	 */
	virtual void Initialize(const XmlNode *document);

	/**
	 * <b>Description:</b>
	 * Main run method.  This method runs until the status is BEP_TERMINATE.
	 *
	 * @param terminateFlag Flag used to terminate the system.  (this is unused)
	 */
	virtual void Run();

	/**
	 * <b>Description:</b>
	 * Method that is called to command the component to perform any system
	 * registration functions necessary.
	 *
	 * @return BEP_SUCCESS_RESPONSE if the registration was successful
	 *         BEP_FAILURE_RESPONSE if the registration was not successful
	 */
	virtual const string Register(void);

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




private:

	void SetupTimer(BepTimer &timer, int interval, int pulseValue);

	void ProcessIncomingMessage(SerialString_t message);

	bool IsMessageValid(SerialString_t message);

	UINT8 CalculateCheckSum(SerialString_t message);

	void SendAckMsg(UINT8 ackType, UINT8 msgType);

	void CalculateMeasuredForces(string startTag, string endTag, TeslaDataMessage *resultMsg);

	void CalculateAbsResults(void);

	INT32 FindDvtMarker(string tag);


	XmlNode *m_config;

	SerialChannel *m_teslaComm;

	BepTimer m_scheduledMessageTimer;
	BepTimer m_icmDataTimer;

	TeslaDataMessage *m_periodicMsg;
	TeslaDataMessage *m_brakeResultMsg;
	TeslaDataMessage *m_dragResultMsg;
	TeslaDataMessage *m_absResultMsg;

	IcmDataSupplier *m_icmData;

	SerialString_t m_ackMsg;

	DvtMarkers m_dvtMarkers;
};
//-------------------------------------------------------------------------------------------------
#endif //TeslaHostInterface_h
