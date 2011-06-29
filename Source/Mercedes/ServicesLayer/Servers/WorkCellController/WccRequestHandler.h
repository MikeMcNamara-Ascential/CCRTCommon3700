//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WccRequestHandler.h 1     12/08/04 3:46p Derickso $
//
// FILE DESCRIPTION:
// 	The WccRequestHandler handles all unsolicited requests from the external
//  WorkCellController.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WccRequestHandler.h $
// 
// 1     12/08/04 3:46p Derickso
// Created new VSS database.
// 
// 1     12/07/04 4:27p Derickso
// Created new VSS database.
// 
// 1     8/04/04 1:07p Derickso
// 
// 1     1/27/04 9:49a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
// Initial checkin
// 
// 1     6/12/03 4:26p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:49  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:11a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
// 
// 1     6/13/02 7:21a Dan.erickson
// Created, tested and debugged.
// 
//*************************************************************************
#ifndef WccRequestHandler_h
#define WccRequestHandler_h
//=========================================================================

#include "WorkCellController.h"
#include "AtomicThread.h"
#include "SerialChannel.h"
#include "INamedDataBroker.h"

// Forward declaration of needed classes
class WorkCellController;


/**
 * The WccRequestHandler class will handle all unsolicited messages sent by the external
 * workcell conroller.  It is a seperate thread spun up from the WorkCellController object
 * to specificly monitor the serial port for unsolicited messages and to respond to the
 * requests.
 * 
 * @author Dan Erickson
 * @version Version 1.1
 * @since Version 1.0
 */
class WccRequestHandler : public AtomicThread
{
public:
	/**
	 * Class constructor.
	 * 
	 * @param wccServer Reference to the WorkCellController object that created us.
	 * @since Version 1.0
	 */
	WccRequestHandler(WorkCellController *wccServer);
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~WccRequestHandler();
	/**
	 * Initializes the WccRequestHandler to talk to the external WCC as well as any data that needs to 
	 * be setup before we begin processing inputs.
	 * 
	 * @param document Configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	/**
	 * Function to be executed when thread is run
	 *
	 * @return Status
	 * @since Version 1.0
	 */
	virtual void* Execute(void);
	/**
	 * Get the name for the specified request so the response can be looked up.
	 * 
	 * @param request   Request from the external workcell controller.
	 * @param byteCount Number of bytes in the request.
	 * @return Lookup name for the response.
	 * @since Version 1.0
	 */
	const std::string GetRequestName(const unsigned char *request, const INT32 &byteCount);
	/**
	 * Get the response to send to the external workcell controller.
	 * 
	 * @param requestName
	 *               Name of the request to respond to.
	 * @return Response to send to the external workcell controller.
	 * @since Version 1.0
	 */
	const std::string GetWccResponse(const std::string &requestName);
	/**
	 * Clear all data on the incoming data buffer.  This is data we should be 
	 * processing anyway.
	 * @since Version 1.1
	 */
	void ClearIncomingDataBuffer(void);
	/**
	 * Return the maximum length of a valid request.
	 * 
	 * @return Maximum request length in bytes.
	 * @since Version 1.0
	 */
	inline const INT32 GetMaxRequestLength(void)
	{
		return m_maxRequestLength;
	};
	/**
	 * Return the maximum number of attempts to retrieve a message.
	 * 
	 * @return Maximum number of attempts.
	 * @since Version 1.0
	 */
	inline const INT32 GetMaxAttempts(void)
	{
		return m_maxAttempts;
	};
	/**
	 * Get the amount of time to wait between checking for new data at the serial port.
	 * 
	 * @return Delay time in ms.
	 * @since Version 1.0
	 */
	inline const INT32 GetMessageDelay(void)
	{
		return m_messageDelay;
	};
	/**
	 * Set flag to ignore incoming data, it is vehicle build data.
	 * @since Version 1.1
	 */
	inline virtual void IgnoreIncomingData(void)
	{
		m_ignoreIncomingData = true;
	};
	/**
	 * Set flag to process incoming data.
	 * @since Version 1.1
	 */
	inline virtual void ProcessIncomingData(void)
	{
		m_ignoreIncomingData = false;
	};
	/**
	 * Determines if incoming data should be processed.
	 * 
	 * @return True - process incoming data.
	 * @since Version 1.1
	 */
	inline virtual const bool ShouldIgnoreIncomingData(void)
	{
		return m_ignoreIncomingData;
	};


protected:
	/**
	 * Store the valid requests that can be issued from the external workcell controller.
	 * 
	 * @param requestNode
	 *               The requests that will be handled.
	 * @since Version 1.0
	 */
	void SetValidRequests(const XmlNode *requestNode)
	{
		if (requestNode != NULL) m_validWccRequests.DeepCopy(requestNode->getChildren());
		else m_validWccRequests.clear(true);
	};
	/**
	 * Store the response for each request we can receive.
	 * 
	 * @param responseNode
	 *               The responses that will be sent for each request.
	 * @since Version 1.0
	 */
	void SetRequestResponses(const XmlNode *responseNode)
	{
		if (responseNode != NULL) m_wccRequestResponse.DeepCopy(responseNode->getChildren());
		else m_wccRequestResponse.clear(true);
	};
	/**
	 * Set the maximum number attempts for sending a message to the external workcll controller.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	void SetMaxAttempts(const INT32 &attempts)
	{
		m_maxAttempts = attempts;
	};
	/**
	 * Set the maximum number attempts for sending a message to the external workcll controller.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	void SetMaxAttempts(const std::string &attempts)
	{
		SetMaxAttempts(atoi(attempts.c_str()));
	};
	/**
	 * Set the time to wait in ms between failed attempts to send a message to the
	 * external workcell controller.
	 * 
	 * @param delay  Delay time in ms.
	 * @since Version 1.0
	 */
	void SetMessageDelay(const INT32 &delay)
	{
		m_messageDelay = delay;
	};
	/**
	 * Set the time to wait in ms between failed attempts to send a message to the
	 * external workcell controller.
	 * 
	 * @param delay  Delay time in ms.
	 * @since Version 1.0
	 */
	void SetMessageDelay(const std::string &delay)
	{
		SetMessageDelay(atoi(delay.c_str()));
	};
	/**
	 * Set the maximum length in bytes of a valid request.
	 * 
	 * @param length Maximum request message length in bytes.
	 * @since Version 1.0
	 */
	void SetMaxRequestLength(const INT32 &length)
	{
		m_maxRequestLength = length;
	};
	/**
	 * Set the maximum length in bytes of a valid request.
	 * 
	 * @param length Maximum request message length in bytes.
	 * @since Version 1.0
	 */
	void SetMaxRequestLength(const std::string &length)
	{
		SetMaxRequestLength(atoi(length.c_str()));
	};

private:
	/**
	 * Pointer to the WorkCellController object that created us.
	 */
	WorkCellController *m_workCellController;
	/**
	 * Communication object to talk with the external workcell controller.
	 */
	SerialChannel m_wccComm;
	/**
	 * INamedDataBroker comm object for reading data from the syatem.
	 */
	INamedDataBroker *m_ndb;
	/**
	 * Map of the requests from the external workcell controller that this object will handle.
	 */
	XmlNodeMap m_validWccRequests;
	/**
	 * Map of the responses to send for each request.
	 */
	XmlNodeMap m_wccRequestResponse;
	/**
	 * Number of attempts to send a message to the external workcell controller.
	 */
	INT32 m_maxAttempts;
	/**
	 * Time to wait in ms between failed attempts to send a response to the external workcell controller.
	 */
	INT32 m_messageDelay;
	/**
	 * Maximum size of a request message from the workcell controller.
	 */
	INT32 m_maxRequestLength;
	/**
	 * Flag to ignore incoming data.
	 */
	bool m_ignoreIncomingData;
};

#endif  // WccRequestHandler_h
