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
MqsResultInterface::MqsResultInterface() : HostInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
MqsResultInterface::~MqsResultInterface()
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
string MqsResultInterface::GenerateHostResultString(const XmlNode *testResults)
{
	string resultString("");
	Log(LOG_FN_ENTRY, "MqsResultInterface::GenerateHostResultString() - Enter");
	for(XmlNodeMapCItr iter = m_resultMaps.begin(); iter != m_resultMaps.end(); iter++)
	{   // Check if this is a conditional result string
		Log(LOG_DEV_DATA, "Processing result map: %s", iter->second->getValue().c_str());
		XmlNodeMapCItr optionIter = iter->second->getAttributes().find("VehicleOption");
		bool processResult = false;
		if(optionIter != iter->second->getAttributes().end())
		{
			string vehicleOption = "VehicleBuild/" + optionIter->second->getValue();
			Log(LOG_DEV_DATA, "Looking for %s in the build data", optionIter->second->getValue().c_str());
			try
			{
				string optionValue = testResults->getChild(vehicleOption)->getValue();
				Log(LOG_DEV_DATA, "Vehicle Option: %s, Value: %s", 
					optionIter->second->getValue().c_str(), optionValue.c_str());
				XmlNodeMapCItr validValsIter = iter->second->getAttributes().find("ValidOptions");
				if(validValsIter != iter->second->getAttributes().end())
				{
					processResult = strstr(validValsIter->second->getValue().c_str(),optionValue.c_str()) != NULL;
				}
			}
			catch(XmlException &excpt)
			{
				Log(LOG_ERRORS, "Error getting %s from the build data, not processing %s - %s", 
					optionIter->second->getValue().c_str(), iter->second->getValue().c_str(), excpt.GetReason());
			}
		}
		else
		{
			Log(LOG_DEV_DATA, "No restrictions, process result string");
			processResult = true;
		}
		Log(LOG_DEV_DATA, "Processing result string: %s - %s", 
			iter->second->getName().c_str(), processResult ? "True" : "False");
		if(processResult)
		{   // Load the result map
			XmlParser parser;
			const XmlNode *resultMap = parser.ReturnXMLDocument(getenv("USR_ROOT") + iter->second->getValue());
			INT32 total = 0;
			resultString += BuildTestResultString(testResults, resultMap->getChildren(), ResultConversions(), total);
		}
		Log(LOG_DEV_DATA, "Done processing result map: %s", iter->second->getValue().c_str());
	}
	Log(LOG_FN_ENTRY, "MqsResultInterface::GenerateHostResultString() - Exit");
	return resultString;
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
	// Get the list of result maps to process
	try
	{
		m_resultMaps.DeepCopy(config->getChild("Setup/ResultTypes")->getChildren());
	}
	catch(XmlException &excpt)
	{
		m_resultMaps.clear(true);
		Log(LOG_ERRORS, "Missing result mappings, no results will be transferred to MQS - %s", excpt.GetReason());
	}
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
			// Look for the response
			SerialString_t mqsResponse, mqsResponse2;
			int bytesRead = portComm.ReadPort(mqsResponse, 1000, 0);
			int secondaryBytesRead = portComm.ReadPort(mqsResponse2, 1000, 0);
			// Unlock the port for this attempt
			portLocked = portComm.UnlockPort();
			if(bytesRead > 0)
			{
				Log(LOG_DEV_DATA, "Received from MQS: %s", string((char *)(&mqsResponse[0])).c_str());
			}
			if(secondaryBytesRead > 0)
			{
				Log(LOG_DEV_DATA, "Received from MQS: %s", string((char *)(&mqsResponse2[0])).c_str());
			}
			if(bytesRead <= 0)
			{   // No response from the host
				status = BEP_STATUS_FAILURE;
				Log(LOG_ERRORS, "No response from MQS.");
			}
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
