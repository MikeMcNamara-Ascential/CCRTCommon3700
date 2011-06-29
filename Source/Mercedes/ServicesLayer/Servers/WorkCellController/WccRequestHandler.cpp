//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WccRequestHandler.cpp 2     4/20/05 7:30a Derickso $
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
//    $Log: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WccRequestHandler.cpp $
// 
// 2     4/20/05 7:30a Derickso
// Updated to allow the software to read system data to append to the
// response to send back to the WCC.
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
// 1     6/12/03 3:56p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:53a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:43  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:20a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:14a Khykin
// Initial Checkin From DCTC
// 
// 3     8/14/02 12:56p Skypig13
// Removed compiler warnings
// 
// 2     6/13/02 7:23a Dan.erickson
// The external workcell controller is looking for the original
// request to be sent followed by the response.  Modified the
// software to accomplish this.
// 
// 1     6/13/02 7:17a Dan.erickson
// Created, tested and debugged.
// 
//*************************************************************************

#include "WccRequestHandler.h"

WccRequestHandler::WccRequestHandler(WorkCellController *wccServer) : 
AtomicThread(), m_ndb(NULL), m_ignoreIncomingData(false)
{
	m_workCellController = wccServer;
}

WccRequestHandler::~WccRequestHandler()
{   // Get rid of the INDB comm object
	if (m_ndb != NULL)  delete m_ndb;
	m_ndb = NULL;
}

void WccRequestHandler::Initialize(const XmlNode *document)
{
	m_workCellController->Log("WccRequestHandler::Initialize()");
	m_wccComm.Initialize(document);
	// Store the requests that will be handled and the responses that will be sent
	try
	{
		SetValidRequests(document->getChild("Setup/WccRequests"));
		SetRequestResponses(document->getChild("Setup/RequestResponses"));
	}
	catch(...)
	{
		SetValidRequests(NULL);
		SetRequestResponses(NULL);
	}
	// Store the delay and max attempts
	try
	{
		SetMaxAttempts(document->getChild("Setup/MaxAttempts")->getValue());
		SetMessageDelay(document->getChild("Setup/MessageDelay")->getValue());
		SetMaxRequestLength(document->getChild("Setup/MaxRequestLength")->getValue());
	}
	catch(...)
	{
		SetMaxAttempts(3);
		SetMessageDelay(100);
		SetMaxRequestLength(50);
	}
	m_workCellController->Log("WccRequestHandler::Initialize() - done");
}

void* WccRequestHandler::Execute(void)
{   
	unsigned char wccRequest[GetMaxRequestLength()];
	INT32 byteCount = 0;
	// Loop until the server terminates
	while (m_workCellController->GetStatus() != BEP_TERMINATE)
	{   // Wait for a request to arrive
		m_workCellController->Log(LOG_DEV_DATA, "Waiting for a request\n");
		// Only get data from the port if we are allowed
		if (!ShouldIgnoreIncomingData()) 
		{   // Get the data from the port
			byteCount = m_wccComm.Get(wccRequest, GetMaxRequestLength());
			m_workCellController->Log(LOG_DEV_DATA, "Received %d bytes\n", byteCount);
			// If we got data and its not an ACK from the workcell for a build request
			if ((byteCount > 0) && !ShouldIgnoreIncomingData())
			{   // Get the remaining data from the port
				m_wccComm.GetRemainingSerialData(wccRequest, GetMaxRequestLength(), byteCount, GetMaxAttempts(), GetMessageDelay()); 	
				// Get the response to send to the workcell controller
				std::string response = GetWccResponse(GetRequestName(wccRequest, byteCount));
				m_workCellController->Log(LOG_DEV_DATA, "Sending response to WCC: {%s}\n", response.c_str());
				unsigned char wccResponse[byteCount+response.length()+2];
				INT32 wccResponseIndex = 0;
				// Add the original request to the wcc string
				for (INT32 ii = 0; ii < byteCount; ii++) 
					wccResponse[wccResponseIndex++] = wccRequest[ii];
				// Add the response to the wcc string
				for (UINT32 responseIndex = 0; responseIndex < response.length(); responseIndex++)
					wccResponse[wccResponseIndex++] = response[responseIndex];
				// Terminate string with CR LF
				wccResponse[wccResponseIndex++] = CR;
				wccResponse[wccResponseIndex] = LF;
				// Send the response to the workcell controller
				BEP_STATUS_TYPE status;
				INT32 attempts = 0;
				// Check once more that it is ok to reply to the unsolicited request
				if (!ShouldIgnoreIncomingData())
				{   // Still ok to send the reply
					do
					{
						status = m_wccComm.Send(wccResponse, wccResponseIndex+1);
					} while ((status != BEP_STATUS_SUCCESS) && (attempts++ < GetMaxAttempts()));
				}
			}
		}
		else
		{   // Wait here until the WCC can make another request
			while(ShouldIgnoreIncomingData())   BposSleep(GetMessageDelay());
		}
	}
	return(NULL);
}

void WccRequestHandler::ClearIncomingDataBuffer(void)
{
	unsigned char junkData[1024];   // Temporary queue
	INT32 junkBytes = 0;
	m_workCellController->Log("Clearing incoming data buffer\n");
	m_wccComm.GetRemainingSerialData(junkData, 1024, junkBytes, GetMaxAttempts(), GetMessageDelay());
}

const std::string WccRequestHandler::GetRequestName(const unsigned char *request, const INT32 &byteCount)
{
	if (m_workCellController->IsDebugOn()) 
	{
		printf("Received Request - byteCount: %d\n", byteCount);
		for (INT32 ii = 0; ii < byteCount; ii++)
			printf("\tii:%02d -- %c <$%02X>\n", ii, (isprint(request[ii]) ? request[ii] : '?'), request[ii]);
	}
	char asciiRequest[byteCount];
	// Convert the request to ASCII
	for (INT32 ii = 0; ii < byteCount; ii++)
		itoa(request[ii], &asciiRequest[ii], 10);
	// Find the request in the list
	std::string requestName = BEP_NO_IMPLEMENTED_RESPONSE;
	for (XmlNodeMapItr iter = m_validWccRequests.begin(); iter != m_validWccRequests.end(); iter++)
	{   // Get the name of the request
		if (asciiRequest == iter->second->getValue()) requestName = iter->second->getName();
	}
	// Return the request name
	if (m_workCellController->IsDebugOn()) printf("WccRequestHandler::GetRequestName() returning: %s\n", requestName.c_str());
	return requestName;
}

const std::string WccRequestHandler::GetWccResponse(const std::string &requestName)
{
	std::string response("");
	XmlNodeMapItr responseIter;
	if ((responseIter = m_wccRequestResponse.find(requestName)) != m_wccRequestResponse.end())
	{   // Get the response to send to the WCC
		response = responseIter->second->getValue();
		// Ensure we have a NDM object to use
		if (m_ndb == NULL)
		{
			m_workCellController->Log(LOG_DEV_DATA, "Creating new INamedDataBroker object...");
			m_ndb = new INamedDataBroker();
			m_workCellController->Log(LOG_DEV_DATA, "Done creating new INamedDataBroker object.");
		}
		// Check if any data should be added to the response
		try
		{   // Get the data item to add to the response
			string addDataItem = responseIter->second->getAttribute("AddData")->getValue();
			if (m_ndb != NULL)
			{	// Read the data from the system
			XmlNode dataNode("","");
			string ndbResponse;
				if (BEP_STATUS_SUCCESS == m_ndb->Read(addDataItem, ndbResponse, true))
				{
					m_ndb->GetNext(dataNode, ndbResponse);
				response += dataNode.getValue();
				m_workCellController->Log(LOG_DEV_DATA, "Returning %s for request %s", response.c_str(), requestName.c_str());
			}
			else
			{
				m_workCellController->Log(LOG_ERRORS, "Could not read %s for request %s", addDataItem.c_str(), requestName.c_str());
			}
		}
			else
			{
				m_workCellController->Log(LOG_ERRORS, "WccRequestHandler::GetWccResponse() INamedDataBroker comm object does not exist.");
				response = BEP_UNAVAILABLE_RESPONSE;
			}
		}
		catch (XmlException &excpt)
		{
			m_workCellController->Log(LOG_ERRORS, "No additional data required for request %s", requestName.c_str());
		}
	}
	else
	{   // Log an error since we could not find a response
		m_workCellController->Log("Could not find a response for request: %s\n", requestName.c_str());
	}
	// Return the response to be sent
	return response;
}

