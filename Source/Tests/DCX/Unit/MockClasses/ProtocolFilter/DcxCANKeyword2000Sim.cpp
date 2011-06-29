//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/Tests/DCX/Unit/MockClasses/ProtocolFilter/DcxCANKeyword2000Sim.cpp 1     9/07/07 10:06a Dkrause $
//
// FILE DESCRIPTION:
//	Protocol Filter for handling Keyword 2000 Protocol as implemented by DaimlerChrysler
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/MockClasses/ProtocolFilter/DcxCANKeyword2000Sim.cpp $
// 
// 1     9/07/07 10:06a Dkrause
// 
// 3     12/06/06 1:29p Derickso
// Moved typedef to base class and added SetLastTxTime to Send function.
// 
// 2     11/14/06 3:38p Bbarrett
// Latest core components do not die.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 1     12/01/04 2:36p Derickso
// Created new VSS Database.
// 
// 1     6/09/04 8:45a Derickso
// Dcx specific protocol filter for Keyword protocol 2000 on CAN bus.
// 
// 1     1/27/04 9:31a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 8     1/20/04 6:43a Derickso
// Updated Semaphore to BepSemaphore.
// 
// 7     12/16/03 12:42p Derickso
// Added handling for module busy negative response code.
// 
// 5     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
// 
// 4     11/13/03 7:02p Derickso
// Updates to correct for response pending handling errors.
// 
// 3     10/20/03 9:40a Derickso
// Added accessor and settor methods for data byte index.
// Added loggin level masks to Log statements.
// 
// 2     9/22/03 5:30p Derickso
// Added SensMessage methods to correct comile errors.
// Updated to add module request ID to each message prior
// to sending the message.
// 
// 1     8/25/03 1:52p Derickso
// Created.  Compiles, not tested.
// 
//*************************************************************************
#include "DcxCANKeyword2000Sim.h"

DcxCANKeyword2000Sim::DcxCANKeyword2000Sim(KeepAliveTimer_t &lastTxTime, BepMutex *commPortInUse /* =NULL*/) : ProtocolFilter(lastTxTime, commPortInUse),
	m_enterDiagModeCode(0x80), m_responsePendingCode(0x78), m_responsePendingReads(3), m_dataByteCountIndex(6),
	m_moduleRequestID(0x0000), m_autoEnterDiagMode(false), m_enterDiagnosticModeMessageTag("EnterDiagnosticMode")
{   // NOthing special to do here
    currentReply = 0;
}

DcxCANKeyword2000Sim::~DcxCANKeyword2000Sim()
{	// Clear out the buffer before we leave.
	Log(LOG_FN_ENTRY, "Resetting connection in ~DcxCANKeyword2000ProtocolFilter()");
	ResetConnection();
	BposSleep(150);
	Log(LOG_FN_ENTRY, "~DcxCANKeyword2000ProtocolFilter() complete\n");
}

bool DcxCANKeyword2000Sim::Initialize(const XmlNode *config)
{	// Get the negative response code for module not in diagnostic mode
	try
	{	// Get the code being used for module not in diagnostic mode
		SetEnterDiagnosticModeCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/EnterDiagnosticMode")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetEnterDiagnosticModeCode(0x80);
	}
	try
	{	// Get the code being used for response pending from the module
		SetResponsePendingCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ResponsePending")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetResponsePendingCode(0x78);
	}
	try
	{	// Get the code being used for module busy, repeat request
		SetModuleBusyCode(BposReadInt(config->getChild("Setup/NegativeResponseCodes/ModuleBusy")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetModuleBusyCode(0x21);
	}
	// Get the message tag for entering diagnostic mode
	try
	{
		SetEnterDiagnosticModeMessageTag(config->getChild("Setup/EnterDiagnosticModeMessageTag")->getValue());
	}
	catch (XmlException &err)
	{
		SetEnterDiagnosticModeMessageTag("EnterDiagnosticMode");
	}
	// Determine if diagnostic mode should be automatically entered
	try
	{
		SetAutomaticallyEnterDiagnosticMode(config->getChild("Setup/AutomaticallyEnterDiagnosticMode")->getValue() == "Yes");
	}
	catch (XmlException &err)
	{
		SetAutomaticallyEnterDiagnosticMode(false);
	}
	// Store the max number of reads to perform for a response pending response
	try
	{
		SetResponsePendingReads(BposReadInt(config->getChild("Setup/ResponsePendingReads")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		SetResponsePendingReads(3);
	}
	// Store the module request ID
	try
	{
		SetModuleRequestID(atoh(config->getChild("Setup/ModuleRequestID")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, ": CANNOT SEND MESSAGES TO MODULE WITHOUT ID!!!!!\n");
		throw;
	}
	// Store the data byte count index
	try
	{
		SetDataByteCountIndex(BposReadInt(config->getChild("Setup/DataByteCountIndex")->getValue().c_str()));
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "Data byte count index not specified, using 6\n");
		SetDataByteCountIndex(6);
	}
	// Call the base class to complete the initialization
	return ProtocolFilter::Initialize(config);
}

const BEP_STATUS_TYPE DcxCANKeyword2000Sim::SendMessage(SerialString_t &message)
{	// Add the module ID to the message
	message = GetModuleRequestID() + message;
	// Clear the Fifos so bad data is not gathered
	Log(LOG_DETAILED_DATA, "Resetting The Connection\n");
	ResetConnection();
	Log(LOG_DETAILED_DATA, "Reset The Connection\n");
	// Send the message and return the result
	PrintSerialString( "ProtocolFilter Sending", message);
	INT32 bytesSent = ILogicalPort::WritePort(message, GetNumberOfRetries());

    SetLastTxTime();

	Log(LOG_DETAILED_DATA, "Sent the message\n");
	// Determine the result of the write
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	if (bytesSent < 0)		 status = BEP_STATUS_ERROR;
	else if (bytesSent == 0) status = BEP_STATUS_FAILURE;
	else if (bytesSent > 0)	 status = BEP_STATUS_SUCCESS;
	// Return the status
	return status;
}

const BEP_STATUS_TYPE DcxCANKeyword2000Sim::SendMessage(std::string messageTag)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag);
}

const BEP_STATUS_TYPE DcxCANKeyword2000Sim::SendMessage(std::string messageTag, SerialArgs_t &args)
{   // Call the base class
	return ProtocolFilter::SendMessage(messageTag, args);
}

const BEP_STATUS_TYPE DcxCANKeyword2000Sim::GetModuleData(string messageTag, SerialString_t &reply, SerialArgs_t *args /*= NULL*/)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string asciiMessage;
	SerialString_t xmtMessage;
	SerialString_t moduleResponse;
	UINT16 tries = GetNumberOfRetries();

    //Lookup the reply from the replyTable, so simulating failures and retries can be done, and different messages
    reply = replyTable[currentReply++];
    // Make sure the response matches a filter before processing it
    if (IsResponseValid(messageTag, reply))
    {	// Check for a valid response if we got the response
        if ((BEP_STATUS_SUCCESS == status) && (reply.length() > 0)) status = CheckForValidResponse(reply);
        // Check for a negative response from the module
        if ((BEP_STATUS_SUCCESS != status) && (reply.length() > 0)) status = CheckForNegativeResponse(reply);
        // Check if it was a negaitve response
        if ((BEP_STATUS_FAILURE == status) || (1 && (BEP_STATUS_SUCCESS != status) && (reply.length() > 0)))
        {	// Check what the negative response was
            UINT8 negativeResponseCode = reply[GetNegativeResponseCodeIndex()];
            if ((GetEnterDiagnosticModeCode() == negativeResponseCode) && AutomaticallyEnterDiagnosticMode())
            {	// Need to enter diagnostic mode first
                //SendMessage(GetEnterDiagnosticModeMessageTag());
                reply.erase();
                //GetResponse(GetEnterDiagnosticModeMessageTag(), reply);
                reply = replyTable[currentReply++];
            }
            else if (GetModuleBusyCode() != negativeResponseCode)
            {   // Negative response with no special treatment, stop all attempts
                status = BEP_STATUS_FAILURE;
                //tries = 0;
            }
        }
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }

    return status;
}

const SerialString_t DcxCANKeyword2000Sim::ExtractModuleData(SerialString_t &moduleResponse)
{
	string fullResponse;
	// Log the respnse received from the module
	char temp[256];
	for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
		fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
	Log(LOG_DETAILED_DATA, "Extracting data from response string: %s\n", fullResponse.c_str());
	// Look at the number of data bytes returned from the module
	UINT8 numberOfBytes = moduleResponse[GetDataByteCountIndex()];
	// Module response starts immediately after the module response ID
	SerialString_t dataResponse;
	const SerialString_t::size_type dataStartIdx = GetDataStartIndex();
	const SerialString_t::size_type dataRespSz = numberOfBytes-GetDataHeaderLength();
	
	dataResponse.clear();

	if( moduleResponse.size() >= (dataStartIdx + dataRespSz) )
	{
		dataResponse = moduleResponse.substr(dataStartIdx, dataRespSz);

		// Log the data extracted from the module
		fullResponse.erase();
		for (UINT16 ii = 0; ii < dataResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", dataResponse[ii]);
		Log(LOG_DEV_DATA, "Extracted module data: %s\n", fullResponse.c_str());
		// Check if multiple messages are in the module
		Log(LOG_DETAILED_DATA, "Total Response Length: %d, Current Message Length: %d, Number of Bytes: %d\n", 
			moduleResponse.length(), (GetDataByteCountIndex() + numberOfBytes + 1), numberOfBytes);
		if ((UINT16)moduleResponse.length() > (GetDataByteCountIndex() + numberOfBytes + 1))
		{   // Multiple messages in the response, update to point at next message
			moduleResponse = moduleResponse.substr(GetDataByteCountIndex() + numberOfBytes + 1);
			// Log the data remaining in the buffer
			fullResponse.erase();
			for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
				fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
			Log(LOG_DETAILED_DATA, "Module data remaining in buffer: %s\n", fullResponse.c_str());
		}
		else
		{   // No more messages to be processed, clear the buffer
			moduleResponse.erase();
		}
	}
	else
	{
		for (UINT16 ii = 0; ii < moduleResponse.length(); ii++)
			fullResponse += CreateMessage(temp, 256, "$%02X ", moduleResponse[ii]);
		Log( LOG_ERRORS, "Incomplete message in ExtractModuleData: <%s>\n", fullResponse.c_str());
		fullResponse.clear();
	}
	// Return the extracted data	
	return dataResponse;        
}

int DcxCANKeyword2000Sim::WaitForFullResponse( SerialString_t &response)
{
	int         retVal = 0;
	// set up the receive timeout
	//ComMssgTableEntry msgAttr;
	Log(LOG_DEV_DATA, "Waiting for response from module\n");
	Log(LOG_DEV_DATA, "\t -- Module Response Delay: %d\n", GetResponseDelay());
	Log(LOG_DEV_DATA, "\t -- Response End Delay: %d\n", GetResponseEndDelay());
#if 0
	int         bytesRead, retVal = 0;
	uint8_t     rcvByte;
	UINT16      responseDelay = GetResponseDelay();

	while (true)
	{	// Try to read next character
		if ( (bytesRead=ReadPort(&rcvByte, 1, responseDelay)) > 0)
		{
			response.push_back( rcvByte);
			retVal++;
			// Bytes are flowing in, can decrease delay time
			responseDelay = GetResponseEndDelay();

			Log(LOG_DEV_DATA, "\t Got %d (%d total) bytes...setting delay to %d ms\n", bytesRead, retVal, responseDelay);
		}
		// No more data
		else if ( bytesRead == 0)
		{	// No more data available..so leave
			Log(LOG_DEV_DATA, "All data received\n");
			break;
		}
		// Error reading byte
		else if ( errno != ETIMEDOUT)
		{
			Log(LOG_DEV_DATA, "ReadPort Error: %s, %d bytes read\n", strerror( errno), bytesRead);
			retVal = -1;
			break;
		}
		else
		{	// No more data available..so leave
			Log(LOG_DEV_DATA, "All data received (time out)\n");
			break;
		}
	}
#else
	retVal = ReadPort( response, GetResponseDelay(), GetResponseEndDelay());
#endif

	Log(LOG_DEV_DATA, "Done waiting for response.  %d bytes received\n", retVal);

	return( retVal);
}

inline const UINT8& DcxCANKeyword2000Sim::GetEnterDiagnosticModeCode(void)
{	// Return the negative response code for module not in diag mode
	return m_enterDiagModeCode;
}

inline const UINT8& DcxCANKeyword2000Sim::GetResponsePendingCode(void)
{	// Return the code for response pending
	return m_responsePendingCode;
}

inline const UINT8& DcxCANKeyword2000Sim::GetModuleBusyCode(void)
{	// Return the code for module busy, repeat request
	return m_moduleBusyCode;
}

inline const string& DcxCANKeyword2000Sim::GetEnterDiagnosticModeMessageTag(void)
{	// Return the tag for the enter diagnostic mode message
	return m_enterDiagnosticModeMessageTag;
}

inline const INT32& DcxCANKeyword2000Sim::GetNumberOfResponsePendingReads(void)
{
	return m_responsePendingReads;
}

inline const bool& DcxCANKeyword2000Sim::AutomaticallyEnterDiagnosticMode(void)
{
	return m_autoEnterDiagMode;
}

inline const SerialString_t DcxCANKeyword2000Sim::GetModuleRequestID(void)
{
	SerialString_t moduleRequestID;
	moduleRequestID.push_back((uint8_t)((m_moduleRequestID & 0xFF00) >> 8));
	moduleRequestID.push_back((uint8_t)(m_moduleRequestID & 0x00FF));
	return moduleRequestID;
}

inline const INT32& DcxCANKeyword2000Sim::GetDataByteCountIndex(void)
{
	return m_dataByteCountIndex;
}

inline void DcxCANKeyword2000Sim::SetEnterDiagnosticModeCode(const UINT8 &enterDiagnosticModeCode)
{	// Save the enter diagnostic mode code
	m_enterDiagModeCode = enterDiagnosticModeCode;
}

inline void DcxCANKeyword2000Sim::SetResponsePendingCode(const UINT8 &responsePendingCode)
{	// Save the response pending code
	m_responsePendingCode = responsePendingCode;
}

inline void DcxCANKeyword2000Sim::SetModuleBusyCode(const UINT8 &moduleBusyCode)
{	// Save the module busy, repeat request code
	m_moduleBusyCode = moduleBusyCode;
}

inline void DcxCANKeyword2000Sim::SetEnterDiagnosticModeMessageTag(const string& tag)
{	// Save the enter diagnostic mode message tag
	m_enterDiagnosticModeMessageTag = tag;
}

inline void DcxCANKeyword2000Sim::SetAutomaticallyEnterDiagnosticMode(const bool& autoEnterDiagMode)
{
	m_autoEnterDiagMode = autoEnterDiagMode;
}

inline void DcxCANKeyword2000Sim::SetResponsePendingReads(const INT32& responsePendingReads)
{
	m_responsePendingReads = responsePendingReads;
}

inline void DcxCANKeyword2000Sim::SetModuleRequestID(const UINT16& moduleID)
{
	m_moduleRequestID = moduleID;
}

inline void DcxCANKeyword2000Sim::SetDataByteCountIndex(const INT32& dataByteCountindex)
{
	m_dataByteCountIndex = dataByteCountindex;
}
