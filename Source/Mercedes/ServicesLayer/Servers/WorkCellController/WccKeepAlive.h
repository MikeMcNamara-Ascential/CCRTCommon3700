//*************************************************************************
// FILE:
//    $Header:  $
//
// FILE DESCRIPTION:
// 	The WccKeepAlive handles sending keep alive messages to the external Workcell Controller.
//	This class will also proces the response from the WCC to the keep alive message.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log:  $
// 
//*************************************************************************
#ifndef WccKeepAlive_h
#define WccKeepAlive_h
//=========================================================================
#include "WorkCellController.h"
#include "AtomicThread.h"
#include "SerialChannel.h"
#include "IFaultServer.h"

// Forward declaration of needed classes
class WorkCellController;

class WccKeepAlive : public AtomicThread
{
public:
	/**
	 * Class constructor.
	 * The pointer to the WorkCellController class will be saved so we can perform our logging functions.
	 * the machine number will also be saved so it can be used in the keep alive message that gets sent to the WCC.
	 * 
	 * @param wcc    Pointer to the WorkCellController that created this thread so logging can be performed.
	 * @param machineNumber
	 *               Machine number to be used in the keep alive message.
	 */
	WccKeepAlive(WorkCellController *wcc, const UINT32 &machineNumber);
	/**
	 * Class destructor.
	 */
	virtual ~WccKeepAlive();
	/**
	 * Initialize the keep alive thread.
	 * The keep alive data will be stored so that the keep alive mesage is sent at the required intervals.
	 * The keep alive message will also be built during the initialization and stored so it can just be sent 
	 * over and over.
	 * 
	 * @param document Configuration data for the Keep Alive thread.
	 */
	virtual void Initialize(const XmlNode *document);

protected:
	/**
	 * Funciton to be executed when thread is run
	 *
	 * @return Status
	 */
	virtual void* Execute(void);
	/**
	 * Process the error code from the keep alive response.
	 * This may involve displaying error messages on the screen for the operator.
	 * 
	 * @param errorCode Error code extracted from the keep alive response.
	 */
	void ProcessErrorCode(const UINT16 &errorCode);

private:
	/**
	 * Build the keep alive message.
	 * The keep alive string will be built and saved.  The message does not change, so it is stored for retransmission at
	 * the appointed interval.
	 * 
	 * @param document Configuration data to build the keep alive message.
	 */
	void BuildKepAliveMessage(const XmlNode *document);
	/**
	 * Send the keep alive message to the WCC.
	 * The message will be sent to the WCC.  The response will then be retrieved and processed.
	 * 
	 * @return Status of sending the keep alive message to the WCC.
	 */
	BEP_STATUS_TYPE SendKeepAliveMessage(void);
	/**
	 * Get the keep alive response from the WCC.
	 * The keep alive response will be read from the WCC.  It will then be processed to determine if any error codes
	 * are present.  The error codes will be handled.
	 * 
	 * @return Status of getting the keep alive response from the WCC.
	 */
	BEP_STATUS_TYPE GetKeepAliveResponse(void);
	/**
	 * Store the maximum response time for the WCC to respond to the keep alive message.
	 * 
	 * @param maxTime Maximum time WCC has to respond to the keep alive message.
	 */
	void SetMaxResponseTime(const INT32 &maxTime);
	/**
	 * Get the maximum amount of time the WCC can take to respond to a message.
	 * 
	 * @return Maximum response time for the keep alive message.
	 */
	const INT32& GetMaxResponseTime(void);
	/**
	 * Store the minimum keep alive message response length.
	 * 
	 * @param minResponseLength
	 *               Minimum number of bytes required to indicate a good response.
	 */
	void SetMinResponseLength(const INT32 &minResponseLength);
	/**
	 * Get the minimum message response length that denotes a good message has been received.
	 * 
	 * @return Minimum number of bytes that signifies a good message.
	 */
	const INT32& GetMinResponseLength(void);
	/**
	 * Store the starting index of the error code field in the keep alive response.
	 * 
	 * @param startIndex Starting index of the error code field.
	 */
	void SetErrorCodeStartIndex(const INT32 &startIndex);
	/**
	 * Get the starting index of the error code field in the kep alive field.
	 * 
	 * @return Starting index of the error code field.
	 */
	const INT32& GetErrorCodeStartIndex(void);
	/**
	 * Store the length of the error code field of the keep alive response.
	 * 
	 * @param length Length of the error code field.
	 */
	void SetErrorCodeLength(const INT32 &length);
	/**
	 * Get the length of the error code field of the keep alive response.
	 * 
	 * @return Length of the error code field.
	 */
	const INT32& GetErrorCodeLength(void);
	void SetMaxErrorBit(const INT32 &bit);
	const INT32& GetMaxErrorBit(void);
	/**
	 * Convert the severity level to an integer for reporting to the fault server.
	 *
	 * @param level  Severity level for the fault reported by the WCC.
	 * @return  Severity level code for the requested severity level.
	 */
	const INT32 GetSeverityLevel(const string &level);
	/**
	 * Store the time to wait between keep alive messages.
	 * 
	 * @param keepAliveDelay  Amount of time to wait between keep alive messages.
	 */
	void SetKeepAliveTime(const INT32 &keepAliveDelay);
	/**
	 * Get the time to wait between keep alive messages.
	 *
	 * @return Time to wait between keep alive messages.
	 */
	const INT32& GetKeepAliveTime(void);
	/**
	 * Store the status of the port connection.
	 * 
	 * @param portConnected
	 *               Flag indicating the connection status.
	 *               <ul>
	 *               <li> True - Port has been connected. </li>
	 *               <li> False - Port has not been connected. </li>
	 *               </ul>
	 */
	void SetPortConnected(const bool portConnected);
	/**
	 * Check the flag indicating if the port has been connected.
	 * 
	 * @return Port connection status.
	 *         <ul>
	 *         <li> True - port has been connected. </li>
	 *         <li> False - port has not been connected. </li>
	 *         </ul>
	 */
	const bool& IsPortConnected(void);
	/**
	 * Header to place on the keep alive message.
	 */
#define KEEP_ALIVE_MESSAGE_REQUEST_ID 0xFD
	/**
	 * Header on the keep alive response.
	 */
#define KEEP_ALIVE_MESSAGE_RESPONSE_ID 0xFE
	/**
	 * Keep alive message to be sent to the WCC.
	 */
	string m_keepAliveMessage;
	/**
	 * This machine's ID number.
	 */
	UINT32 m_machineNumber;
	/**
	 * Maximum response time for the WCC to respond to the keep alive message.
	 */
	INT32  m_maxResponseTime;
	/**
	 * Minimum message length for the keep alive response.
	 */
	INT32  m_minResponseLength;
	/**
	 * Starting index for the error code field in the keep alive response.
	 */
	INT32  m_errorCodeStartIndex;
	/**
	 * Number of bytes in the error code field in the keep alive response.
	 */
	INT32  m_errorCodeFieldLength;
	/**
	 * Maximum error bit field in use.
	 */
	INT32  m_maxErrorBit;
	/**
	 * Amount of time to wait between keep alive messages.
	 */
	INT32  m_keepAliveTime;
	/**
	 * Flag to indicate if the port has been connected.
	 */
	bool   m_portConnected;
	/**
	 * Error code definitions from the config file.
	 */
	XmlNodeMap m_errorCodeDefs;
	/**
	 * Severity level translation table for reporting faults to the fault server.
	 */
	XmlNodeMap m_severityLevelConversion;
	/**
	 * Pointer to the WCC object that created this thread.
	 * 
	 * @see WorkCellController
	 * @see AtomicThread
	 */
	WorkCellController *m_wcc;
	/**
	 * Communication object to talk with the external workcell controller.
	 */
	SerialChannel m_wccComm;
	/**
	 * comm object to interface with the fault server for setting and clearing faults.
	 * 
	 * @see IFaultServer
	 */
	IFaultServer  m_faultServer;
};
//=========================================================================
#endif //WccKeepAlive_h
