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
#include "WccKeepAlive.h"
//-----------------------------------------------------------------------------

WccKeepAlive::WccKeepAlive(WorkCellController *wcc, const UINT32 &machineNumber) :
AtomicThread(), m_machineNumber(machineNumber), m_portConnected(false), m_wcc(NULL)
{   // Save the pointer to the WCC object
	m_wcc = wcc;
}

WccKeepAlive::~WccKeepAlive()
{   // Nothing special to do here
}

void WccKeepAlive::Initialize(const XmlNode *document)
{   // Build the keep alive string
	BuildKepAliveMessage(document);
	// Get the time to wait between keep alive messages
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting KeepAliveTime");
	try
	{
		SetKeepAliveTime(BposReadInt(document->getChild("Setup/KeepAliveTime")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "KeepAliveTime not specified, using 30s: %s", excpt.GetReason());
		SetKeepAliveTime(30000);
	}
	// Get the keep alive max response time
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting MaxResponseTime");
	try
	{
		SetMaxResponseTime(BposReadInt(document->getChild("Setup/MaxResponseTime")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "MaxResponseTime not specified, using 5s: %s", excpt.GetReason());
		SetMaxResponseTime(5000);
	}
	// Get the minimum keep alive response length
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting MinResponseLength");
	try
	{
		SetMinResponseLength(BposReadInt(document->getChild("Setup/MinResponseLength")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "MinResponseLength not specified, using 7: %s", excpt.GetReason());
		SetMinResponseLength(7);
	}
	// Get the error code field start index
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting ErrorCodeStartIndex");
	try
	{
		SetErrorCodeStartIndex(BposReadInt(document->getChild("Setup/ErrorCodeStartIndex")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "ErrorCodeStartIndex not specified, using 1: %s", excpt.GetReason());
		SetErrorCodeStartIndex(1);
	}
	// Get the maximum error code
	try
	{
		SetMaxErrorBit(BposReadInt(document->getChild("Setup/MaxErrorBit")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "MaxErrorBit not specified, using 6: %s", excpt.GetReason());
		SetMaxErrorBit(6);
	}
	// Get the error code field length
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting ErrorCodeLength");
	try
	{
		SetErrorCodeLength(BposReadInt(document->getChild("Setup/ErrorCodeLength")->getValue().c_str()));
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "ErrorCodeLength not specified, using 4: %s", excpt.GetReason());
		SetErrorCodeLength(4);
	}
	// Save the error code definition table
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting ErrorCodeDefinitions");
	try
	{
		m_errorCodeDefs.DeepCopy(document->getChild("Setup/ErrorCodeDefinitions")->getChildren());
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "ErrorCodeDefinitions not specified: %s", excpt.GetReason());
		m_errorCodeDefs.clear(true);
	}
	// Save the severity level map
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - getting SeverityLevelConversion");
	try
	{
		m_severityLevelConversion.DeepCopy(document->getChild("Setup/SeverityLevelConversion")->getChildren());
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "SeverityLevelConversion not specified: %s", excpt.GetReason());
		m_severityLevelConversion.clear(true);
	}
	// Initialize the FaultServer interface object and WCC comm object
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - Initializing fault server comm.");
	m_faultServer.Initialize(FAULT_SERVER_NAME);
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - Initializing WCC comm.");
	try
	{   // Initialize the WCC Comms
		m_wccComm.Initialize(document);
		SetPortConnected(true);
	}
	catch (BepException &excpt)
	{   // Connection failed, keep trying until WCC is up
		m_wcc->Log(LOG_ERRORS, "WCC Port not connected, will attempt later.");
		SetPortConnected(false);
	}
	m_wcc->Log(LOG_ERRORS, "WccKeepAlive::Initialize - complete");
}

void* WccKeepAlive::Execute(void)
{   // Keep sending keep alives until the end of time, or we get terminated
	INT32 keepAliveInterval = GetKeepAliveTime() / 100;
	INT32 keepAliveSendCountDown = keepAliveInterval;
	bool timeToSendKeepAlive = true;
	while (m_wcc->GetStatus().compare(BEP_TERMINATE))
	{   // Ensure the port is connected prior to attempting to send the message
		if (IsPortConnected())
		{
			// Check if it is time to send the keep alive message
		if (timeToSendKeepAlive)
		{   // Send the keep alive message
				SetPortConnected(SendKeepAliveMessage() == BEP_STATUS_SUCCESS);
		 	timeToSendKeepAlive = false;
		 	keepAliveSendCountDown = keepAliveInterval;
		}
		else
		{   // Wait for a bit, and then decrement the count down
			BposSleep(100);
			keepAliveSendCountDown--;
			timeToSendKeepAlive = !keepAliveSendCountDown;
		}
	}
		else
		{   // Port is not connected, attempt to reconnect

			// Flag that the port was reconnected
			SetPortConnected(true);
		}
	}
	return NULL;
}

void WccKeepAlive::ProcessErrorCode(const UINT16 &errorCode)
{
	string errorTag;
	string response;
	char buff[8];
	UINT16 maxErrorBit = 0x0001 << GetMaxErrorBit();
	// Check each bit to see if it has been set
	for (UINT16 bitIndex = 0x0001; bitIndex <= maxErrorBit; bitIndex <<= 1)
	{   // Get the fault tag data
		// Create a tag to look up the error data for this bit
		errorTag = string("E") + CreateMessage(buff, sizeof(buff), "%04X", bitIndex);
		// Look up the error tag in the table
		XmlNodeMapItr iter = m_errorCodeDefs.find(errorTag);
		if (iter != m_errorCodeDefs.end())
		{   
			try
			{	// Error code was found, perform the neccessary actions.
				if (errorCode & bitIndex)
				{  // Current bit is set, process the error data
					// We will start by reporting the failure to the FaultServer
					m_wcc->Log(LOG_DEV_DATA, "Setting fault %s", iter->second->getAttribute("FaultTag")->getValue().c_str());
					INT32 severityLevel = GetSeverityLevel(iter->second->getAttribute("Severity")->getValue());
					m_faultServer.SetFault(iter->second->getAttribute("FaultTag")->getValue(), 
										   iter->second->getAttribute("FaultDescription")->getValue(),
										   "WccKeepAlive", response, true, severityLevel);
				}
				else
				{
					m_wcc->Log(LOG_DEV_DATA, "Clearing fault %s", iter->second->getAttribute("FaultTag")->getValue().c_str());
					m_faultServer.ClearFault(iter->second->getAttribute("FaultTag")->getValue(), response, true);
				}
			}
			catch (XmlException &excpt)
			{
				m_wcc->Log(LOG_ERRORS, "Error node for %s not properly configured: %s", errorTag.c_str(), excpt.GetReason());
			}
		}
		else
		{
			m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Could not find error code %s in error code table", errorTag.c_str());
		}
	}
}

void WccKeepAlive::BuildKepAliveMessage(const XmlNode *document)
{
	string equipmentType;
	string machineNumberFormat;
	string machineType;
	// Get the equipment type from the config
	try
	{
		equipmentType = document->getChild("Setup/EquipmentType")->getValue();
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Equipment type not found, using R: %s", excpt.GetReason());
		equipmentType = "R";
	}
	// Get the machine number format from the config
	try
	{
		machineNumberFormat = document->getChild("Setup/MachineNumberFormat")->getValue();
	}
	catch (XmlException &excpt)
	{
		m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Machine number format not found - %s", excpt.GetReason());
		machineNumberFormat = "%04d";
	}
	// Build the machine number portion of the string
	char buff[16];
	machineType = equipmentType + CreateMessage(buff, sizeof(buff), machineNumberFormat.c_str(), m_machineNumber);
	m_wcc->Log(LOG_DEV_DATA, "WccKeepAlive: Keep Alive Message - <$%02X>%s<$%02X><$%02X>", 
			   KEEP_ALIVE_MESSAGE_REQUEST_ID, machineType.c_str(), CR, LF);
	// Add the wrapper to the keep alive string
	m_keepAliveMessage = (char)KEEP_ALIVE_MESSAGE_REQUEST_ID + machineType + (char)CR + (char)LF;
}

BEP_STATUS_TYPE WccKeepAlive::SendKeepAliveMessage(void)
{
	BEP_STATUS_TYPE messageStatus = BEP_STATUS_ERROR;
	bool portLocked = false;
	// Attempt to lock the port
	portLocked = m_wcc->LockWccCommPort(&m_wccComm);
	if (portLocked)
	{   // Tell threads to ignore any incoming data
		m_wcc->IgnoreIncomingData(true);
		// Send the message to the WCC after clearing the buffer
		m_wccComm.ResetConnection();
		messageStatus = m_wccComm.Send(m_keepAliveMessage);
		// If we successfully sent the message, get the response from the WCC
		if (BEP_STATUS_SUCCESS == messageStatus)
		{
			messageStatus = GetKeepAliveResponse();
		}
		else
		{
			m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Could not send keep alive message to WCC - status: %s", 
					   ConvertStatusToResponse(messageStatus).c_str());
		}
		// Unlock the port for others to use
		portLocked = m_wcc->UnlockWccCommPort(&m_wccComm);
		// Tell threads to resume processing incoming data
		m_wcc->IgnoreIncomingData(false);
	}
	else
	{
		m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Could not lock the comm port.  Not sending keep alive.");
		messageStatus = BEP_STATUS_HARDWARE;
	}
	// Return the status of sending the message
	return messageStatus;
}

BEP_STATUS_TYPE WccKeepAlive::GetKeepAliveResponse(void)
{
	SerialString_t keepAliveResponse;
	INT32 numBytesRead;
	BEP_STATUS_TYPE responseStatus = BEP_STATUS_ERROR;
	// Get the response from the ECC
	numBytesRead = m_wccComm.ReadPort(keepAliveResponse, GetMaxResponseTime(), m_wccComm.GetTimeoutTime());
	// Check the number of bytes read to ensure we have a good message
	if (numBytesRead >= GetMinResponseLength())
	{   // Good response, check for correct response ID
		if (keepAliveResponse[0] == KEEP_ALIVE_MESSAGE_RESPONSE_ID)
		{   // Good response, check for error codes in message
			m_wcc->Log(LOG_DEV_DATA, "Error Response: %02X %02X %02X %02X %02X",
					   keepAliveResponse[0], keepAliveResponse[1], keepAliveResponse[2], keepAliveResponse[3], keepAliveResponse[4]);
			// Build the error code to be checked
			UINT16 errorCode = 0x0000;
			INT32 bitShift = 0;
			for (INT32 index = GetErrorCodeStartIndex(); index < (GetErrorCodeStartIndex() + GetErrorCodeLength()); index++)
			{   // Determine the bit shift
				bitShift = GetErrorCodeLength() * ((GetErrorCodeLength()) - index);
				// Convert current character to a digit
				char digit = keepAliveResponse[index];
				m_wcc->Log(LOG_DEV_DATA, "Processing erro index: %d - Bit Shift: %d, Raw Data: %02X, Digit: %c",
						  index, bitShift, keepAliveResponse[index], digit);
				// Add in the next byte
				errorCode |= (atoh(&digit) << bitShift);
			}
			m_wcc->Log(LOG_DEV_DATA, "Processing error code: %04X", errorCode);
			// Process the error code
			ProcessErrorCode(errorCode);
			responseStatus = BEP_STATUS_SUCCESS;
		}
		else
		{
			m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Incorrect response ID received %02X expected %02X",
					   keepAliveResponse[0], KEEP_ALIVE_MESSAGE_RESPONSE_ID);
			responseStatus = BEP_STATUS_FAILURE;
		}
	}
	else
	{
		m_wcc->Log(LOG_ERRORS, "WccKeepAlive: Keep alive response too short %d, expected at least %d",
				   numBytesRead, GetMinResponseLength());
		responseStatus = BEP_STATUS_TIMEOUT;
	}
	return responseStatus;
}

inline void WccKeepAlive::SetMaxResponseTime(const INT32 &maxTime)
{
	m_maxResponseTime = maxTime;
}

inline const INT32& WccKeepAlive::GetMaxResponseTime(void)
{
	return m_maxResponseTime;
}

inline void WccKeepAlive::SetMinResponseLength(const INT32 &minResponseLength)
{
	m_minResponseLength = minResponseLength;
}

inline const INT32& WccKeepAlive::GetMinResponseLength(void)
{
	return m_minResponseLength;
}

inline void WccKeepAlive::SetErrorCodeStartIndex(const INT32 &startIndex)
{
	m_errorCodeStartIndex = startIndex;
}

inline const INT32& WccKeepAlive::GetErrorCodeStartIndex(void)
{
	return m_errorCodeStartIndex;
}

inline void WccKeepAlive::SetErrorCodeLength(const INT32 &length)
{
	m_errorCodeFieldLength = length;
}

inline const INT32& WccKeepAlive::GetErrorCodeLength(void)
{
	return m_errorCodeFieldLength;
}

inline void WccKeepAlive::SetMaxErrorBit(const INT32 &bit)
{
	m_maxErrorBit = bit;
}

inline const INT32& WccKeepAlive::GetMaxErrorBit(void)
{
	return m_maxErrorBit;
}

const INT32 WccKeepAlive::GetSeverityLevel(const string &level)
{
	XmlNodeMapItr iter = m_severityLevelConversion.find(level);
	INT32 severity = MAX_SEVERITY;
	if (iter != m_severityLevelConversion.end())
	{
		severity = BposReadInt(iter->second->getValue().c_str());
	}
	else
	{
		m_wcc->Log(LOG_ERRORS, "Severity level conversion not specified for level %s, default to %d", level.c_str(), MAX_SEVERITY);
	}
	return severity;
}

inline void WccKeepAlive::SetKeepAliveTime(const INT32 &keepAliveDelay)
{
	m_keepAliveTime = keepAliveDelay;
}

inline const INT32& WccKeepAlive::GetKeepAliveTime(void)
{
	return m_keepAliveTime;
}

inline void WccKeepAlive::SetPortConnected(const bool portConnected)
{
	m_portConnected = portConnected;
}

inline const bool& WccKeepAlive::IsPortConnected(void)
{
	return m_portConnected;
}
