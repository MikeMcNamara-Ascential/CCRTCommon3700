//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/IFaultServer.cpp 3     4/13/06 3:40a Cward $
//
// FILE DESCRIPTION:
//   This defines the interface for FaultServer object.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/IFaultServer.cpp $
// 
// 3     4/13/06 3:40a Cward
// Removed the default parameter value specifications from the cpp file
//
// 2     2/15/06 3:54p Bmeinke
// Removed the default parameter value specifications from the cpp file
// (should only be in the h file)
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:12p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:32a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:29p Khykin
// Initial checkin.
//
// 2     5/19/03 11:16a Khykin
// Updated to make the methods thread safe.
//
// 21    10/17/02 11:07a Derickso
// Added functionality to clear all faults.
//
// 20    10/09/02 9:15a Derickso
// Ignore attributes for Read messages.
//
// 19    9/23/02 12:31p Derickso
// Added diagnostics to aid debugging.
// Modified Read() method to only add attributes to the node if
// the attributes were not provided.  Removed the cleanup of the node
// to prevent SigSegs.  Cleanup should be handled by the calling
// object.
//
// 18    9/16/02 2:45p Derickso
// Corrected memory leak.  Need to clean up allocated memory.
//
// 17    6/07/02 11:46a Dan.erickson
// Removed potential memory leaks.
// Replaced occurrances of ToString() with writeNode().
// Improved efficiency of ProcessNameToTag() method.
//
// 16    4/25/02 4:38p Skypig13
// Updated with new changes
//
// 14    4/15/02 3:38p Dan.erickson
// Updated based on changes to IBepCommunication.
//
// 13    3/21/02 1:58p Dan.erickson
// Do not need to re-parse the message after ValidateMessage is
// called.  Only need to get the document from the parser.
//
// 12    3/20/02 1:59p Dan.erickson
// Updated ValidateMessage(0 to not check for attributes on
// Register messages.
//
// 11    3/13/02 10:17a Dan.erickson
// Corrected function header of ValidateMessage.  Commented out
// the setting of parse to true.
//
// 10    3/11/02 1:13p Dan.erickson
// Removed SpecialProcessing method.
//
// 9     2/26/02 2:48p Dan.erickson
// Updated to take advantage of XmlNode type data.
//
// 8     2/12/02 5:12p Dan.erickson
// Updated because of BepServer restructuring.
//
// 7     1/15/02 3:55p Dan.erickson
// Added SubscribeAllFaults method.
// Overloaded BuildMessage to add MssgClass attribute to all
// messages.
//
// 6     11/21/01 3:02p Dan.erickson
// Modified SubscribeSeverityLevelFaults method to subscribe for
// all faults that have a higher severity level that the level
// specified.
//
// 5     11/21/01 9:40a Dan.erickson
// Overloaded IBepCommunication::Read() to set attributes prior
// to calling IBepCommunication::Read() method.
//
// 4     11/20/01 5:37p Dan.erickson
// Expanded GetNextFault to return attributes as well.
//
// 3     11/19/01 3:55p Dan.erickson
// Revised Get and Subscribe methods.
// Added GetAllFaults, GetNextFault,
// ProcessNameToTag and SeverityLevelToTag methods.
//
// 2     11/16/01 4:34p Dan.erickson
// Testing.
//
// 1     11/15/01 4:54p Dan.erickson
// New File.
//
//
//*************************************************************************
#include "IFaultServer.h"

IFaultServer::IFaultServer() : IInfoCommunication(), m_severityLevel(0)
{   // Nothing special to do
}

IFaultServer::~IFaultServer()
{
	SetDefaultSeverityLevel(0);
}

void IFaultServer::Initialize(IBepCommunication &creator, INT32 id	/*= -1*/)
{	// Call the base class initializer
	IBepCommunication::Initialize(creator, id);
}

void IFaultServer::Initialize(const std::string &server,
							  const INT32 size	  /* = DEFAULT_BUFFER_SIZE */,
							  const INT32 timeout /* = DEFAULT_TIMEOUT */,
							  const bool debug	  /* = false */)
{	// Make sure we are using the well known name for the test result server
	if (server != FAULT_SERVER_NAME) printf("Attempt to use non-standard FaultServer name: %s, using: %s\n", server.c_str(), FAULT_SERVER_NAME);
	IBepCommunication::Initialize(FAULT_SERVER_NAME, "Client", debug);
}

void IFaultServer::Initialize(const XmlNode *config, const INT32 id	/* = -1 */, const INT32 rid	/* = -1 */)
{
	XmlNode *name = const_cast<XmlNode *>(config->getChildren().getNode("Name"));
	if (name->getValue() != FAULT_SERVER_NAME)
	{
		printf("Attempt to use non-standard FaultServer name: %s, using: %s\n", name->getValue().c_str(), FAULT_SERVER_NAME);
		name->setValue(FAULT_SERVER_NAME);
	}
	try
	{	// Set the default severity level
		SetDefaultSeverityLevel(atoi(config->getChildren().getNode("DefaultSeverityLevel")->getValue().c_str()));
	}
	catch (...)
	{	// Set to lowest level
		SetDefaultSeverityLevel(0);
	}
	IBepCommunication::Initialize(config, id, rid);
}

XmlNode * IFaultServer::SetSeverity(const std::string &severity, XmlNode *existNode)
{
	// Add the attribute to the node
	AddAttribute(BEP_SEVERITY, severity, existNode);
	if (IsDebugOn()) printf("Severity attribute added: node = %s\n", existNode->ToString().c_str());
	return(existNode);
}

INT32 IFaultServer::SetFault(const std::string &fault, const std::string &description,
							 const std::string &process, std::string &response,
							 const bool isFinal	/*= false*/, INT32 severity	/*= -1*/)
{
	XmlNode node( fault, SET);
	INT32 result = BEP_STATUS_ERROR;

	// Set the description, process, and severity attributes
	SetSeverity(SeverityToString(severity), &node);
	SetDescription(description, &node);
	SetProcess(process, &node);
	// Write the fault to the FaultServer
	result = Write(&node, response, isFinal);

	// Return the result of the operation
	return(result);
}

INT32 IFaultServer::ClearFault(const std::string &fault, std::string &response, const bool isFinal /*= false*/)
{	// Set the attributes so the node will parse properly
	XmlNode node( fault, CLEAR);
	INT32 result = BEP_STATUS_ERROR;

	SetDescription( "", &node);
	SetSeverity( "", &node);
	SetProcess("", &node);
	// Clear the fault from the FaultServer
	result = Write(&node, response, isFinal);

	// Return the result of the operation
	return(result);
}

INT32 IFaultServer::ClearAllFaults(std::string &response)
{	// Clear the ALL_FAULTS tag
	return ClearFault(ALL_FAULTS, response, true);
}

INT32 IFaultServer::GetDefaultSeverityLevel(void)
{
    return(m_severityLevel);
}

void IFaultServer::SetDefaultSeverityLevel(INT32 severityLevel)
{
    if (severityLevel > MAX_SEVERITY) severityLevel = MAX_SEVERITY;
    else if (severityLevel < 0)	severityLevel = 0;
    m_severityLevel = severityLevel;
}



INT32 IFaultServer::GetAllFaults(std::string &response, const bool isFinal /*= false*/)
{	// Set the attributes
	XmlNode node( ALL_FAULTS,"");;
	INT32 result = BEP_STATUS_ERROR;

	SetDescription( "", &node);
	SetSeverity( "", &node);
	SetProcess( "", &node);
	// Read all the faults from the server
	result = Read(&node, response, isFinal);

	// Return the result of the operation
	return(result);
}

INT32 IFaultServer::GetSeverityLevelFaults(INT32 severityLevel, std::string &response, const bool isFinal /*= false*/)
{	// Set the attributes
	std::string severity = SeverityLevelToTag(severityLevel);
	XmlNode node( severity,"");;
	INT32 result = BEP_STATUS_ERROR;

	SetSeverity(SeverityToString(severityLevel), &node);
	SetDescription( "", &node);
	SetProcess( "", &node);
	// Read all the faults from the server
	result = Read(&node, response, isFinal);

	// Return the result of the operation
	return(result);
}

INT32 IFaultServer::GetProcessFaults(const std::string &processName, std::string &response, const bool isFinal /*= false*/)
{
	if (IsDebugOn()) printf("IFaultServer::GetProcessFaults(processName:%s, isFinal:%d)\n", processName.c_str(), isFinal);
	std::string process = ProcessNameToTag(processName);
	// Set the attributes
	XmlNode node( process, "");
	INT32 result = BEP_STATUS_ERROR;

	SetSeverity( "", &node);
	SetDescription( "", &node);
	SetProcess( processName, &node);
	if (IsDebugOn()) printf("Node to send to server: %s\n", node.ToString().c_str());
	// Read the faults from the server
	result = Read(&node, response, isFinal);
	if (IsDebugOn()) printf("Read faults -- result:%d, response:%s\n", result, response.c_str());

	// Return the result of the operation
	if (IsDebugOn()) printf("IFaultServer::GetProcessFaults(processName:%s, response:%s, isFinal:%d) complete\n",
							processName.c_str(), response.c_str(), isFinal);
	return(result);
}

INT32 IFaultServer::Read(const XmlNode *node, std::string &response, const bool IsFinal)
{	// First set the attributes
	XmlNode *readNode = const_cast<XmlNode *>(node);
	XmlNodeMap attributes = node->getAttributes();
	XmlNodeMapItr iter;
	// Ensure all the proper attributes have been provided
	if ((iter = attributes.find(BEP_DESCRIPTION)) == attributes.end())	 readNode = SetDescription( "", readNode);
	if ((iter = attributes.find(BEP_PROCESS)) == attributes.end())		 readNode = SetProcess( "", readNode);
	if ((iter = attributes.find(BEP_SEVERITY)) == attributes.end())		 readNode = SetSeverity( "", readNode);
	// Read the data
	INT32 result = IBepCommunication::Read(readNode, response, IsFinal);
	// Return the result of the operation
	return(result);
}

INT32 IFaultServer::SubscribeSeverityLevelFaults(INT32 severityLevel, const std::string &server,
												 std::string &response, const bool isFinal /*= false*/)
{
	std::string severity;
	INT32 status = BEP_STATUS_SUCCESS;
	for (INT32 ii = severityLevel; (ii <= MAX_SEVERITY) && (status == BEP_STATUS_SUCCESS); ii++)
	{
		// Set the attributes
		std::string severity = SeverityLevelToTag(ii);
		XmlNode node( severity, server);
		SetSeverity(SeverityToString(ii), &node);
		SetDescription( "", &node);
		SetProcess( "", &node);
		// Subscribe for the faults
		status = Subscribe(&node, response, isFinal);
	}
	return(status);
}

INT32 IFaultServer::SubscribeProcessFaults(const std::string &processName, const std::string &server,
										   std::string &response, const bool isFinal /*= false*/)
{
	// Set the attributes
	std::string process = ProcessNameToTag(processName);
	XmlNode node( process, server);
	INT32 status = BEP_STATUS_ERROR;

	SetSeverity( "", &node);
	SetDescription( "", &node);
	SetProcess(processName, &node);
	// Subscribe for the faults
	status = Subscribe(&node, response, isFinal);

	// Return the status of the operation
	return(status);
}

INT32 IFaultServer::SubscribeAllFaults(const std::string &server, std::string &response, const bool isFinal)
{	// Set the attributes for the message
	std::string allFaults = ALL_FAULTS;
	XmlNode node( allFaults, server);
	INT32 status = BEP_STATUS_ERROR;

	// Set the attributes
	SetSeverity( "", &node);
	SetDescription( "", &node);
	SetProcess( "", &node);
	// Subscribe for all faults
	status = Subscribe(&node, response, isFinal);

	// Return the status of the operation
	return(status);
}

void IFaultServer::BuildMessage(const std::string function, std::string &message)
{
	// Use the base class method to build the message
	IBepCommunication::BuildMessage(function, message);
	// Parse the message into an XmlNode
	XmlNode *doc = const_cast<XmlNode *>(m_parser.getDocElement());
	// Add the MssgClass attribute to the message
	doc->addChild(BEP_MSSG_CLASS, "Fault", ATTRIBUTE_NODE);
	message.erase();
	doc->writeNode(message);
}

bool IFaultServer::ValidateMessage(const XmlNode *doc)
{
	bool status = false;
	bool goodNode = true;
	if (IsDebugOn()) printf("IFaultServer::ValidateMessage(%s)\n", doc->ToString().c_str());
	std::string messageType = doc->getAttributes().getNode(BEP_XML_MSG_TYPE)->getValue();
	// Verify the message -- Do not check attributes for Subscribe, resgister, publish or read messages
	if ((BEP_PUBLISH == messageType) || (BEP_SUBSCRIBE == messageType) || (BEP_REGISTER == messageType) || (BEP_READ == messageType))
	{	// A Publish message will not neccessarily have all the correct attributes
		status = IBepCommunication::ValidateMessage(doc);
	}
	else if (IInfoCommunication::ValidateMessage(doc))
	{	// Check additional attributes if base message is valid
		try
		{	// Verify process and description attributes on all nodes in the message
			if (doc->getName() == BEP_XML_ROOT)
			{	//Check each node for the proper attributes
				XmlNodeMap nodes;
				nodes.DeepCopy(doc->getChildren());;	// Get children nodes
				for (XmlNodeMapItr iter = nodes.begin(); (iter != nodes.end()) && goodNode; iter++)
				{	// Get the attributes associated with this node
					goodNode = CheckAttribute((*iter).second->getAttributes().getNode(BEP_SEVERITY));
				}
				if (goodNode) status = true;
				// Cleanup the node map we created
				nodes.clear(true);
			}
		}
		catch (...)
		{
			status = false;
		}
	}
	if (IsDebugOn()) printf("IFaultServer::ValidateMessage() - Message is %s\n\tDoc: %s\n",
							(status ? "VALID" : "INVALID"), doc->ToString().c_str());
	return status;
}

const INT32 IFaultServer::GetNextFault(std::string &tag, std::string &value, std::string &description,
									   std::string &process, std::string &severity, std::string &message)
{
	message.erase();
	if (IsDebugOn()) printf("IFaultServer::GetNextFault() - buffer: %s\n\n", m_parser.getDocElement()->ToString().c_str());
	XmlNode *doc = const_cast<XmlNode *>(m_parser.getDocElement()->getChildren().getNode(0));
	doc->writeNode(message);
	if (IsDebugOn()) printf("IFaultServer::GetNextFault() - doc: %s\n\n", message.c_str());
	INT32 num = doc->getChildren().size() - 1;
	if (num >= 0)
	{	// If there are children
		const XmlNode *node =doc->getChildren().getNode(0);
		if (IsDebugOn()) printf("Getting info from node: %s\n", node->ToString().c_str());
		tag = node->getName();				// Get tag name
		value = node->getValue();			// Get data
		description = node->getAttributes().getNode(BEP_DESCRIPTION)->getValue();
		process     = node->getAttributes().getNode(BEP_PROCESS)->getValue();
		severity    = node->getAttributes().getNode(BEP_SEVERITY)->getValue();
		doc->delChild(node);				// Delete tag just processed
	}
	else tag = value = BEP_UNAVAILABLE_RESPONSE;
	if (IsDebugOn()) printf("IFaultServer::GetNextFault(%s, %s)\n", tag.c_str(), value.c_str());
	return(num);	// How many left
}

bool IFaultServer::CheckAttribute(const XmlNode *attribute)
{
	bool status = false;		// Operation status
	if ((attribute->getName() == BEP_SEVERITY) || IInfoCommunication::CheckAttribute(attribute))
	{	// If attribute is Ok
		status = true;
	}
	return(status);
}

std::string IFaultServer::SeverityToString(INT32 severity)
{
	char buff[10];
	if (severity > MAX_SEVERITY) severity = MAX_SEVERITY;
	else if (severity < 0) severity = GetDefaultSeverityLevel();
	return(itoa(severity, buff, 10));
}

std::string IFaultServer::ProcessNameToTag(const std::string &name)
{	// Parse out any "/" and replace with "_"
	std::string process = name;
	INT32 index = process.find_first_of('/',0);
	if (index >= 0)	process[index] = '_';
	return(process);
}

inline std::string IFaultServer::SeverityLevelToTag(INT32 severityLevel)
{
	return(std::string("_") + SeverityToString(severityLevel));
}

