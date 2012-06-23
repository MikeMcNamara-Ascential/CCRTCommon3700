//*************************************************************************
// Description:
//   Interface to the Windows based CCRT system.
//   This class allows reading and writting data from the Windows based 
//   CCRT system so all data available to the QNX CCRT system can be available
//   to the Windows based CCRT system.  This is mainly intended to assist
//   the migration of CCRT from QNX to Windows.
//
//===========================================================================
// Copyright (c) 2011 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "WinCcrtInterface.h"

//-------------------------------------------------------------------------------------------------
WinCcrtInterface::WinCcrtInterface() : BepServer(), 
m_updateTestType(false), m_ndb(NULL), m_winCcrtCommConfig(NULL)
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
WinCcrtInterface::~WinCcrtInterface()
{	// Get rid of the communication config
	if(m_winCcrtCommConfig != NULL)
	{
		delete m_winCcrtCommConfig;
		m_winCcrtCommConfig = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
void WinCcrtInterface::Initialize(const XmlNode *document)
{	// Call the base class to begin the initialization
	BepServer::Initialize(document);
	// Save the scanner communication configuration
	try
	{
		m_winCcrtCommConfig = const_cast<XmlNode *>(document->getChild("Setup/WinCcrtCommunication"));
	}
	catch(XmlException &excpt)
	{
		m_winCcrtCommConfig = NULL;
		Log(LOG_ERRORS, "Could not load Windows CCRT communication configuration - %s", excpt.GetReason());
	}
	// Set the inter-message wait time
	INT32 waitTime = 0;
    try
	{
		waitTime = BposReadInt(document->getChild("Setup/MessageCheckDelay")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		waitTime = 1000;
		Log(LOG_ERRORS, "No inter-message gap time defined, defaulting to 1000ms between messages - %s", excpt.GetReason());
	}

	// Set the inter-message wait time
    INT32 waitTimeReadPortInit = 0;
	try
	{
		waitTimeReadPortInit = BposReadInt(document->getChild("Setup/MessageReadPortInitDelay")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		waitTimeReadPortInit = 100;
		Log(LOG_ERRORS, "No inter-message gap time defined, defaulting MessageReadPortInitDelay to 100ms between messages - %s", excpt.GetReason());
	}

	// Set the inter-message wait time
	INT32 waitTimeReadPortEnd = 0;
	try
	{
		waitTimeReadPortEnd = BposReadInt(document->getChild("Setup/MessageReadPortEndDelay")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		waitTimeReadPortEnd = 100;
		Log(LOG_ERRORS, "No inter-message gap time defined, defaulting MessageReadPortEndDelay to 100ms between messages - %s", excpt.GetReason());
	}

	 MessageWaitTime(&waitTime);
    MessageWaitTimeInit(&waitTimeReadPortInit);
    MessageWaitTimeEnd(&waitTimeReadPortEnd);

    Log(LOG_DEV_DATA, "waitTime: %d   waitTimeReadPortInit: %d   waitTimeReadPortEnd:%d\n", waitTime, waitTimeReadPortInit, waitTimeReadPortEnd);
}

//-------------------------------------------------------------------------------------------------
const string WinCcrtInterface::Read(const XmlNode *node, const INT32 rate /*= 0*/)
{
	Log(LOG_DEV_DATA, "WinCcrtInterface::Read(XmlNode - tag: %s, rate: %d) - Enter", 
		node->getName().c_str(), rate);
	string result;
	if(!node->getName().compare("BeltTensions"))
	{
		for(XmlNodeMapItr iter = m_beltTensions.begin(); iter != m_beltTensions.end(); iter++)
		{   // Save the belt tension to the return string
			result += iter->second->ToString();
			// Delete the belt tensions
		}
		m_beltTensions.clear(true);
	}
	else if(!node->getName().compare("Notes"))
	{   // Save the notes to a file
		FILE *noteFile = NULL;
		string fileName(getenv("USR_ROOT") + string("/TestResults/Machine/Notes.txt"));
		if((noteFile = fopen(fileName.c_str(), "w")) != NULL)
		{
			fprintf(noteFile, "%s", m_notes.c_str());
			fclose(noteFile);
			result = fileName;
		}
		else
		{
			Log(LOG_ERRORS, "Could not write notes out to file - %s", strerror(errno));
			result = BEP_ERROR_RESPONSE;
		}
	}
	else
	{   // Call the base class
		result = BepServer::Read(node, rate);
	}
	Log(LOG_DEV_DATA, "WinCcrtInterface::Read(XmlNode - tag: %s, rate: %d) - Exit", 
		node->getName().c_str(), rate);
	return result;
}

//-------------------------------------------------------------------------------------------------
const string WinCcrtInterface::Register(void)
{	// Call the base class to begin the registration
	string result = BepServer::Register();
	Log(LOG_FN_ENTRY, "WinCcrtInterface::Register() - Enter");
	// Create an INamedDataBroker object
	if(m_ndb != NULL)
	{
		delete m_ndb;
		m_ndb = NULL;
	}
	m_ndb = new INamedDataBroker();
	// Make sure we have a valid comm config
	if(m_winCcrtCommConfig != NULL)
	{
		m_winCcrtComm.Initialize(m_winCcrtCommConfig);
	}
	else
	{
		Log(LOG_ERRORS, "No communication setup defined, not starting interface with Windows CCRT system");
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "WinCcrtInterface::Register() - Exit: %s", result.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
void WinCcrtInterface::Run(volatile bool *terminateFlag/*=NULL*/)
{   
	while(GetStatus() != BEP_TERMINATE)
#if 1
	{	// Check for a new message
		CheckForWinCcrtMessage();
		BposSleep(MessageWaitTime());
	}
	m_winCcrtComm.ClosePort();
#else 
	{	// Determine if we need to wait for a connection
		if(m_winCcrtComm->GetSock() == -1)
		{	// Initialize the comm object
			Log(LOG_DEV_DATA, "Initializing the Windows CCRT comm object");
			bool commInit = false;
			while(!commInit && (GetStatus() != BEP_TERMINATE))
			{
				try
				{
					m_winCcrtComm->Initialize(m_winCcrtCommConfig);
					commInit = true;
				}
				catch(BepException &excpt)
				{
					Log(LOG_ERRORS, "BepException attempting to initialize the Windows CCRT comm object: %s", excpt.GetReason());
					m_winCcrtComm->Close();
					for(int index = 0; (index < (SocketReleaseTime() / 100)) && (GetStatus() != BEP_TERMINATE); index++)
					{
						BposSleep(100);
					}
				}
			}
			if(commInit)
			{	// Wait for a connection
				Log(LOG_DEV_DATA, "Waiting for a client to connect");
				INT32 sock = m_winCcrtComm->ProcessRequest();
				WinCcrtSocket(&sock);
				if(WinCcrtSocket() != -1)
				{	// Start the command timer
					Log(LOG_DEV_DATA, "Client connected, begin polling for messages");
					// NOTE: the read call does a blocking read, so no need for a delay
					while((BEP_STATUS_SUCCESS == CheckForWinCcrtMessage()) && (GetStatus() != BEP_TERMINATE));
					m_winCcrtComm->Close();
					Log(LOG_DEV_DATA, "Windows CCRT system disconnected, closed socket");
				}
			}
		}
		else
		{
			BposSleep(1000);
		}
	}
#endif 
}

//-------------------------------------------------------------------------------------------------
const string WinCcrtInterface::Write(const XmlNode *node)
{
	if(!node->getName().compare("Notes"))
	{
		m_notes = node->getValue();
	}
	else if(!node->getName().compare("WinMessage"))
	{
		string temp(node->getValue());
		unsigned char *msg = (unsigned char *)&temp[0];
		m_winCcrtComm.WritePort(msg, temp.length());
	}
	return BepServer::Write(node);
}

//-------------------------------------------------------------------------------------------------
const string WinCcrtInterface::Publish(const XmlNode *node)
{	// Let the base class do what it needs to do
	string result = BepServer::Publish(node);
	// Check if this has changed the tes ttype
	if(!node->getName().compare("TestType"))
	{	// Store the test type
		string type = node->getValue();
		TestType(&type);
		UpdateTestType(true);
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE WinCcrtInterface::CheckForWinCcrtMessage()
{
	SerialString_t message;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Check if the test type has been changed from normal
	INT32 byteCount = m_winCcrtComm.ReadPort(message, MessageWaitTimeInit(), MessageWaitTimeEnd());
	if(byteCount > 0)
	{
		Log(LOG_DEV_DATA, "Received %d bytes from Windows CCRT system", byteCount);
		string tempMsg = string((char *)&message[0], message.length());
		status = ProcessWinCcrtMessage(tempMsg);
	}
	else
	{
		status = BEP_STATUS_SUCCESS;
	}

	return status;
}

//-------------------------------------------------------------------------------------------------
INT32& WinCcrtInterface::MessageWaitTime(const INT32 *waitTime /*= NULL*/)
{
	if(waitTime != NULL)  m_messageWaitTime = *waitTime;
	return m_messageWaitTime;
}

//-------------------------------------------------------------------------------------------------
INT32& WinCcrtInterface::MessageWaitTimeInit(const INT32 *waitTime /*= NULL*/)
{
	if(waitTime != NULL)  m_messageWaitTimeInit = *waitTime;
	return m_messageWaitTimeInit;
}

//-------------------------------------------------------------------------------------------------
INT32& WinCcrtInterface::MessageWaitTimeEnd(const INT32 *waitTime /*= NULL*/)
{
	if(waitTime != NULL)  m_messageWaitTimeEnd = *waitTime;
	return m_messageWaitTimeEnd;
}


//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE WinCcrtInterface::ProcessWinCcrtMessage(const string &message)
{	// Determine what type of message we are dealing with
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "Processing Win CCRT Message: %s", message.c_str());
	string msgType = message.substr(0, message.find_first_of(":"));
	int  tagStart = message.find_first_of(":") + 1;
	unsigned int  tagValSepIndex = message.find_first_of(",");
	int  tagLength = tagValSepIndex != string::npos ? (tagValSepIndex - tagStart) : string::npos;
	string tag = message.substr(tagStart, tagLength);
	if(msgType == BEP_READ)
	{	// Read the tag from the system
		string tempVal = tag.compare("Notes") ? SystemRead(tag) : m_notes;
		// Send the value back to the Windows CCRT system
		if(tempVal.length() == 0)
		{
			tempVal = BEP_UNAVAILABLE_RESPONSE;
		}
		unsigned char *value = (unsigned char *)&tempVal[0];
		status = (m_winCcrtComm.WritePort(value, tempVal.length(), 3) > 0) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
	}
	else if(msgType == BEP_WRITE)
	{
		string value(message.substr(message.find_first_of(",")+1));
		if(tag.find("BeltTension") != tag.npos)
		{
			XmlNode *beltNode = new XmlNode(tag, value);
			m_beltTensions.addNode(beltNode);
			Log(LOG_DEV_DATA, "Added new belt tension: %s, %s", tag.c_str(), value.c_str());
			status = BEP_STATUS_SUCCESS;
		}
		else if(tag.find("Notes") != tag.npos)
		{
			m_notes += value;
			Log(LOG_DEV_DATA, "Added new note: %s", value.c_str());
			Log(LOG_DEV_DATA, "Current Notes: %s", m_notes.c_str());
			status = BEP_STATUS_SUCCESS;
		}
		else
		{
			status = SystemWrite(tag, value);
		}
	}
	else
	{
		Log(LOG_ERRORS, "Unknown message type: %s", msgType.c_str());
	}
	Log(LOG_FN_ENTRY, "Processed Win CCRT message: %s", ConvertStatusToResponse(status).c_str());
	return status;
}

//-------------------------------------------------------------------------------------------------
string WinCcrtInterface::SystemRead(const string &tag)
{
	string value, response;
	if(m_ndb != NULL)
	{
		INT32 status = m_ndb->Read(tag, response, true);
		if(status == BEP_STATUS_SUCCESS)
			status = m_ndb->GetByTag(tag, value, response);
		// check for errors
		if(status != BEP_STATUS_SUCCESS)
		{
			Log(LOG_ERRORS, "Error Reading: %s, %d\n", tag.c_str(), status);
			value = BEP_ERROR_RESPONSE;
		}
	}
	else
	{
		Log(LOG_ERRORS, "SystemRead() - INamedDataBroker object is NULL!");
		value = BEP_SOFTWAREFAIL_RESPONSE;
	}
	return(value);
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE WinCcrtInterface::SystemWrite(const string &tag, const string &value)
{
	BEP_STATUS_TYPE result = BEP_STATUS_SOFTWARE;	// the result of the operation
	string response;
	if(m_ndb != NULL)
	{
		try
		{
			result = ConvertIntToBepStatus(m_ndb->Write(tag, value, response, true));
			Log(LOG_DEV_DATA, "Wrote %s,%s - result: %s", tag.c_str(), value.c_str(), ConvertStatusToResponse(result).c_str());
		}
		catch(BepException &excpt)
		{
			Log(LOG_ERRORS, "SystemWrite %s Error: %s\n", tag.c_str(), excpt.GetReason());
		}
	}
	return(result);
}

//-------------------------------------------------------------------------------------------------
string& WinCcrtInterface::TestType(const string *type /*= NULL*/)
{
	if(type != NULL)  m_testType = *type;
	return m_testType;
}

//-------------------------------------------------------------------------------------------------
const bool& WinCcrtInterface::UpdateTestType()
{
	return m_updateTestType;
}

//-------------------------------------------------------------------------------------------------
void WinCcrtInterface::UpdateTestType(const bool &updateType)
{
	m_updateTestType = updateType;
}

