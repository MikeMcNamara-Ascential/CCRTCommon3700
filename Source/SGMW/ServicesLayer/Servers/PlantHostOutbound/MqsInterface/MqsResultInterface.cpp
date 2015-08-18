//******************************************************************************
// Description:
//   Host Interface to send results to MQS.  If results cannot be sent, they
//   will be stored and tried again at a later time.
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
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <iostream>
#include <fstream>
#include "MqsResultInterface.h"

//-----------------------------------------------------------------------------
MqsResultInterface::MqsResultInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
MqsResultInterface::~MqsResultInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
void MqsResultInterface::LoadAdditionalConfigurationItems(const XmlNode *config)
{
	// Call the base class to complete the setup
	HostInterface::LoadAdditionalConfigurationItems(config);
	string failedFileName = "FailedTransmits.txt";
	try
	{
		failedFileName = config->getChild("Setup/FailedResultTransmitFile")->getValue();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No failed transmit file defined, using %s", failedFileName.c_str());
	}
	failedFileName = getenv("USR_ROOT") + failedFileName;
	FailedTxFileName(&failedFileName);
}

//-----------------------------------------------------------------------------
void MqsResultInterface::SendResultToHost(const string &result)
{
	Log(LOG_FN_ENTRY, "MqsResultInterface::SendResultToHost() - Enter");
	list<string> resultStrings;
	list<string> failedTx;
	resultStrings.push_back(result);
	// Get the list of failed transmits
	if(!access(FailedTxFileName().c_str(), F_OK))
	{
		ifstream failedFile(FailedTxFileName().c_str());
		if(failedFile.is_open())
		{
			while(failedFile.good())
			{
				string failedRslt;
				getline(failedFile, failedRslt);
				if(failedRslt.length() > 0)
				{
					resultStrings.push_back(failedRslt);
				}
			}
			failedFile.close();
		}
		else
		{
			Log(LOG_ERRORS, "Error opening %s to get failed transmit results", FailedTxFileName().c_str());
		}
	}
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	do
	{
		string txResult = resultStrings.front();
		resultStrings.pop_front();
		// Transmit the result string to the host system
		Log(LOG_DEV_DATA, "MqsResultInterface: Message to send to host system - %s", txResult.c_str());
		status = SendTestResultString(txResult, *(HostComm()), MaxMsgSendAttempts(), 0);
		Log(LOG_ERRORS, "Sent result to host system: %", ConvertStatusToResponse(status).c_str());
		if(BEP_STATUS_SUCCESS != status)
		{
			failedTx.push_back(txResult);
		}
	} while(!resultStrings.empty() && (BEP_STATUS_SUCCESS == status));
	// Store any failed transmit strings
	if(!failedTx.empty())
	{
		FILE *rsltFile = NULL;
		if((rsltFile = fopen(FailedTxFileName().c_str(), "w")) != NULL)
		{
			for(list<string>::iterator iter = failedTx.begin(); iter != failedTx.end(); iter++)
			{
				fprintf(rsltFile, "%s\n", (*iter).c_str());
			}
			fclose(rsltFile);
		}
	}
	Log(LOG_FN_ENTRY, "MqsResultInterface::SendResultToHost() - Exit");
}

//-----------------------------------------------------------------------------
const BEP_STATUS_TYPE MqsResultInterface::SendTestResultString(string &resultString, 
															   SerialChannel &portComm, 
															   const INT32 &maxAttempts, 
															   const INT32 &hostResponseTime)
{
	INT32 currentAttempt = 1;                   // Keep track of attempts
	bool portLocked = false;                    // Port lock status
	string message(resultString);
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;  // Transmit status
	// Attempt to send the message
	do
	{   // Lock the port so only we have access to it
		portLocked = portComm.LockPort();
		// If the port is locked, transmit the message
		if(portLocked)
		{   // Transmit the message
			portComm.ResetConnection();
			status = portComm.Send(message);
			// Log the status
			Log(LOG_ERRORS, "Sent message to host: %s", ConvertStatusToResponse(status).c_str());
			// Unlock the port for this attempt
			portLocked = portComm.UnlockPort();
		}
		else
		{   // Log the error
			Log(LOG_ERRORS, "WARNING: Could not lock %s port.  Not transmitting results\n", 
				portComm.GetProcessName().c_str());
			BposSleep(100);       //keep from locking up if port is not released to use.
		}
	// Keep trying while attempts remaining and failed transmit
	} while((BEP_STATUS_SUCCESS != status) && (currentAttempt++ <= maxAttempts));
	// Return the status
	return status;
}

//-----------------------------------------------------------------------------
const string& MqsResultInterface::FailedTxFileName(const string *fileName /*= NULL*/)
{
	if(fileName != NULL)   m_failedTxFileName = *fileName;
	return m_failedTxFileName;
}
