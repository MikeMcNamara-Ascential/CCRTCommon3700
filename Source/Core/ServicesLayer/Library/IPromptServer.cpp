//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/IPromptServer.cpp 2     1/16/04 4:45p Cward $
//
// FILE DESCRIPTION:
//   This defines the interface for PromptServer object.
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
//    $Log: /Core/ServicesLayer/Library/IPromptServer.cpp $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:14p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 3     10/15/03 8:45a Khykin
// Removing print statements that were used for debugging.
//
// 2     10/10/03 9:33a Khykin
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
// 2     6/26/03 4:25p Khykin
// Updated with Flatrocks latest changes.
//
// 17    11/20/02 10:01a Derickso
// Removed parameters from function headers for LockPromptServer() and
// UnLockPromptServer().
//
// 16    10/17/02 12:24p Derickso
// Changed default priority from MAX_PRIORITY/2 to MIN_PRIORITY.
//
// 15    10/07/02 1:30p Derickso
// Commented out the value of the priority parameter in RemovePrompt().
//
// 14    10/03/02 4:16p Derickso
// Added moethods to support priority based prompting.
//
// 13    8/23/02 10:27a Dan.erickson
// Updated to return the Prompt attribute as part of the subscribe
// response.
//
// 12    8/01/02 8:54a Skypig13
// Updated with latest for prompt box locking
//
// 11    5/06/02 3:52p Dan.erickson
// Corrected memory leak.
//
// 10    4/25/02 4:44p Skypig13
// Updated with new changes
//
// 8     4/15/02 12:46p Dan.erickson
// Removed parsing of document in ValidateMessage() since the
// method now takes a XmlNode.
//
// 7     3/20/02 2:58p Dan.erickson
// Restored SpecialProcessing method but gave removed the body
// of this method.  Just do not want the IInfoCommunication
// version of this method getting called.
//
// 6     3/05/02 5:06p Dan.erickson
// Updated to make better use of Xml nodes.
// Removed SpecialProcessing method.
//
// 5     2/12/02 5:14p Dan.erickson
// Updated after restructure of BepServer.
//
// 4     1/14/02 4:13p Dan.erickson
// Added ClearPromptBox and SetBackgroundColor methods.
//
// 3     11/29/01 2:45p Dan.erickson
// Modified DisplayPrompt method to take a string for the
// prompt name instead of a number.
// Modified ValidateMessage method to not check for attributes
// on a subscribe message.
//
// 2     11/26/01 3:35p Dan.erickson
// Debugged and tested.
//
// 1     11/26/01 12:54p Dan.erickson
// New file.
//
//*************************************************************************

#include "IPromptServer.h"

IPromptServer::IPromptServer() : IInfoCommunication()
{
}

IPromptServer::~IPromptServer()
{
}

void IPromptServer::Initialize(IBepCommunication &creator, INT32 id	/*= -1*/)
{	// Call the base class initializer
	IBepCommunication::Initialize(creator, id);
}

void IPromptServer::Initialize(const std::string &server,
							   const INT32 size		   /* = DEFAULT_BUFFER_SIZE */,
							   const INT32 timeout	   /* = DEFAULT_TIMEOUT */,
							   const bool debug		   /* = false */)
{	// Make sure we are using the well known name for the test result server
	if (server != PROMPT_SERVER_NAME) printf("Attempt to use non-standard PromptServer name: %s, using: %s\n",
											 server.c_str(), PROMPT_SERVER_NAME);
	IBepCommunication::Initialize(PROMPT_SERVER_NAME, "Client", debug, size, timeout);
}

void IPromptServer::Initialize(const XmlNode *config, const INT32 id /*= -1*/, const INT32 rid /*= -1*/)
{
	XmlNode *name = const_cast<XmlNode *>(config->getChild("Name"));
	if (name->getValue() != PROMPT_SERVER_NAME)
	{
		printf("Attempt to use non-standard PromptServer name: %s, using: %s\n",
			   name->getValue().c_str(), PROMPT_SERVER_NAME);
		name->setValue(PROMPT_SERVER_NAME);
	}
	IBepCommunication::Initialize(config, id, rid);
}

std::string IPromptServer::GetPromptBoxTag(const INT32 &promptBox)
{
	char buff[10];
	return(std::string(PROMPT_BOX) + itoa(promptBox, buff, 10));
}

INT32 IPromptServer::DisplayPrompt(const INT32 &promptBox, const std::string &prompt,
								   const PromptDetails &promptDetails, std::string &response,
								   INT32 priorityLevel /*= MIN_PRIORITY*/)
{
	INT32	status;
	// Create a new node to contain the prompt attributes
	XmlNode *promptNode = new XmlNode(GetPromptBoxTag(promptBox), DISPLAY);
	PromptDetailsCItr_t	detItr;
	PromptDetail_t		detail;

	if (IsDebugOn()) printf("\tIPromptServer::DisplayPrompt(Box:%d, Prompt:%s, Priority:%d) with details\n", promptBox, prompt.c_str(), priorityLevel);
	
	// Add details for this prompt
	detItr=promptDetails.GetFirstDetail( detail);
	while( detItr != promptDetails.end())
	{
		AddDetail( detail.detailTag, detail.detailValue, promptNode);
		detItr=promptDetails.GetNextDetail( detail, detItr);
	}

	//Add the process name to the prompt node
	if (IsDebugOn()) printf( "\tIPromptServer::DisplayPrompt SetProcess=%s\n", GetProcessName().c_str());
	SetProcess(GetProcessName(), promptNode);
	
	// Add the data type, prompt attributes, and priority levels
	AddAttribute(BEP_DATA_TYPE, BEP_PROMPT, promptNode);
	AddAttribute(BEP_PROMPT, prompt, promptNode);
	AddAttribute(BEP_PRIORITY, PriorityToString(priorityLevel), promptNode);
	
	if (IsDebugOn()) printf("Writing prompt to server: %s\n", prompt.c_str());
	// Write the prompt to the server
	status = Write(promptNode, response, true);
	delete promptNode;
	if (IsDebugOn()) printf("\tExit IPromptServer::DisplayPrompt(Box:%d, Prompt:%s, Priority:%d) with details\n", promptBox, prompt.c_str(), priorityLevel);

	return( status);

}
INT32 IPromptServer::DisplayPrompt(const INT32 &promptBox, const std::string &prompt,
								   std::string &response, INT32 priorityLevel /*= MIN_PRIORITY*/)
{
	INT32	status;
	if (IsDebugOn()) printf("DisplayPrompt(Box:%d, Prompt:%s, Priority:%d)\n", promptBox, prompt.c_str(), priorityLevel);
	// Create a new node to contain the prompt attributes
	XmlNode *promptNode = new XmlNode(GetPromptBoxTag(promptBox), DISPLAY);
	//Add the process name to the prompt node
	SetProcess(GetProcessName(), promptNode);
	
	// Add the data type, prompt attributes, and priority levels
	AddAttribute(BEP_DATA_TYPE, BEP_PROMPT, promptNode);
	AddAttribute(BEP_PROMPT, prompt, promptNode);
	AddAttribute(BEP_PRIORITY, PriorityToString(priorityLevel), promptNode);
	if (IsDebugOn()) printf("Writing prompt to server: %s\n", promptNode->ToString().c_str());
	
	// Write the prompt to the server
	status = Write(promptNode, response, true);
	delete promptNode;

	return( status);
}

INT32 IPromptServer::RemovePrompt(const INT32 &promptBox, const std::string &prompt,
								  std::string &response, INT32 priorityLevel/* = MIN_PRIORITY*/)
{
	INT32	status;
	// Create a new node to contain the prompt attributes
	XmlNode *promptNode = new XmlNode(GetPromptBoxTag(promptBox), REMOVE);
	//Add the process name to the prompt node
	SetProcess(GetProcessName(), promptNode);

	// Add the data type, prompt attributes, and priority levels
	AddAttribute(BEP_DATA_TYPE, BEP_PROMPT, promptNode);
	AddAttribute(BEP_PROMPT, prompt, promptNode);
	AddAttribute(BEP_PRIORITY, PriorityToString(priorityLevel), promptNode);
	// Remove the prompt from the server
	status = Write(promptNode, response, true);
	delete promptNode;
	
	return( status);
}

INT32 IPromptServer::ClearPromptBox(const INT32 &promptBox, std::string &response)
{
	// Set the background color to the default
	SetBackgroundColor(promptBox, DEFAULT_BG_COLOR, response);
	// Display the ClearPromptBox tag
	return(DisplayPrompt(promptBox, CLEAR_PROMPT_BOX, response));
}

INT32 IPromptServer::SetBackgroundColor(const INT32 &promptBox, const std::string &color, std::string &response)
{
	INT32	status;
	// Create a new node to contain the prompt attributes
	if (IsDebugOn()) printf( "\tEnter IPromptServer::SetBackgroundColor( %d, %s)\n", promptBox, color.c_str());
	
	XmlNode *bgColorNode = new XmlNode(GetPromptBoxTag(promptBox)+BG_COLOR, color);
	//Add the process name to the prompt node
	SetProcess(GetProcessName(), bgColorNode);
	status = Write(bgColorNode, response, true);
	delete bgColorNode;

	if (IsDebugOn()) printf( "\tExit IPromptServer::SetBackgroundColor( %d, %s)\n", promptBox, color.c_str());
	return( status);
}

INT32 IPromptServer::LockPromptServer(void)
{
	std::string response;
	INT32 status = BEP_STATUS_ERROR;
	XmlNode *lockNode = new XmlNode( LOCK_PROMPT_SERVER, "");
	
	if( lockNode != NULL)
	{
		SetProcess(GetProcessName(), lockNode);
		status = Write(lockNode, response, true);
		delete lockNode;
	}

	return( status);
}

INT32 IPromptServer::UnlockPromptServer(void)
{
	std::string response;
	XmlNode *unlockNode = new XmlNode(UNLOCK_PROMPT_SERVER, "");
	INT32 status = BEP_STATUS_ERROR;

	if( unlockNode != NULL)
	{
		SetProcess(GetProcessName(), unlockNode);
		status = Write(unlockNode, response, true);
		delete unlockNode;
	}

	return( status);
}

void IPromptServer::SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server)
{   // Get attributes for subscribe nodes
	if (IsDebugOn()) printf("IPromptServer::SpecialProcessing(type: %s)\n", type.c_str());
	if ((BEP_SUBSCRIBE == type) && (node->getName() != "PromptBox1BGColor") && (node->getName() != "PromptBox2BGColor"))
	{   // Get the prompt attribute from the node from the server
		std::string promptValue = server->GetSpecialInfo(node->getName(), BEP_PROMPT);
		if (IsDebugOn()) printf("tag: %s, promptValue: %s\n", node->getName().c_str(), promptValue.c_str());
		node->addChild(BEP_PROMPT, promptValue, ATTRIBUTE_NODE);
		node->addChild(BEP_DATA_TYPE, BEP_PROMPT, ATTRIBUTE_NODE);
		if (IsDebugOn()) printf("Added Attributes, node: %s\n", node->ToString().c_str());
	}
}

bool IPromptServer::ValidateMessage(const XmlNode *doc)
{
	bool status = false;
	bool goodNode = true;

	if (IsDebugOn()) printf("IPromptServer::ValidateMessage(%s)\n", doc->ToString().c_str());
	std::string messageType = doc->getAttributes().getNode(BEP_XML_MSG_TYPE)->getValue();
	if ((messageType == BEP_SUBSCRIBE) || (messageType == BEP_REGISTER))
	{   // Subscribe and publish messages do not need DataType and Prompt attributes
		status = IBepCommunication::ValidateMessage(doc);
	}
	else if (IBepCommunication::ValidateMessage(doc))
	{	// Check additional attributes if base message is valid
		try
		{	// Verify prompt and data type attributes on all nodes in the message
			if (IsDebugOn()) printf("messageType: %s - document:\n%s\n", messageType.c_str(), doc->ToString().c_str());
			if (doc->getName() == BEP_XML_ROOT)
			{
				XmlNodeMap nodes;
				nodes.DeepCopy(doc->getChildren());	// Get children nodes
				//Check each node for the proper attributes
				XmlNodeMap attributes;
				for (XmlNodeMapItr iter = nodes.begin(); (iter != nodes.end()) && goodNode; iter++)
				{	// Do not check this node if it is for background color or locking/unlocking or read
					if (((messageType == BEP_WRITE) && (!iter->second->getValue().empty())) ||
						((messageType == BEP_WRITE) && (iter->second->getName() == LOCK_PROMPT_SERVER)) ||
						((messageType == BEP_WRITE) && (iter->second->getName() == UNLOCK_PROMPT_SERVER)) ||
						(messageType == BEP_READ))
					{   // This is a valid node
						goodNode = true;
					}
					else
					{   // Get the attributes associated with this node
						attributes.DeepCopy(iter->second->getAttributes());
						if (goodNode) goodNode = CheckAttribute(attributes.getNode(BEP_DATA_TYPE));
						if (goodNode) goodNode = CheckAttribute(attributes.getNode(BEP_PROMPT));
						if (goodNode) goodNode = CheckAttribute(attributes.getNode(BEP_PRIORITY));
					}
				}
				if (goodNode) status = true;
				// Clear out our temporary maps
				if (!nodes.empty()) nodes.clear(true);
				if (!attributes.empty()) attributes.clear(true);
			}
		}
		catch (...)
		{
			status = false;
		}
	}
	if (IsDebugOn()) printf("IPromptServer::ValidateMessage() message is %s\n", (status ? "VALID" : "INVALID"));
	return status;
}

bool IPromptServer::CheckAttribute(const XmlNode *attribute)
{
	bool status = false;		// Operation status
	const XmlString name = attribute->getName();
	const XmlString value = attribute->getValue();
	if ((name == BEP_DATA_TYPE) || (name == BEP_PROMPT) || (name == BEP_PRIORITY) ||
		(name == BEP_PROCESS) || IBepCommunication::CheckAttribute(attribute))
	{	// If attribute is Ok
		status = true;
	}
	return(status);
}


std::string IPromptServer::PriorityToString(INT32 priority)
{   // Range check the incoming priority level
    if (priority > MAX_PRIORITY)     priority = MAX_PRIORITY;
    else if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
    // Convert the priority level to a string
    char buff[10];
    std::string priorityLevel(itoa(priority, buff, 10));
    // Return the priority level
    return priorityLevel;
};


void IPromptServer::SetProcessName(const std::string &processName)
{
    m_processName = processName;
};

const std::string& IPromptServer::GetProcessName(void)
{
    return m_processName;
};




//////////////////  PromptDetails methods  /////////////////////////////



PromptDetails::PromptDetails()
{
}

PromptDetails::~PromptDetails()
{
}

void PromptDetails::AddDetail( const std::string &tag, const std::string &value)
{
    const PromptDetail_t	newDetail = { tag, value };
    PromptDetails_t::push_back( newDetail);
}

PromptDetailsCItr_t PromptDetails::GetFirstDetail( PromptDetail_t &firstDetail) const
{
    PromptDetailsCItr_t itr = PromptDetails_t::begin();

    if( PromptDetails_t::empty() == false)
    {
        // Return the first detail in the list
        firstDetail = *itr;
    }
    else
    {
        itr = PromptDetails_t::end();
    }
    return( itr);
}

PromptDetailsCItr_t PromptDetails::GetNextDetail( PromptDetail_t &nextDetail, PromptDetailsCItr_t itr) const
{
    if( PromptDetails_t::empty() == false)
    {
        // If not at the end
        if( itr != PromptDetails_t::end())
        {
            // go to the next detail item in the list
            itr++;
        }
        // At the end....wrap back to beginning
        else
        {
            itr = PromptDetails_t::begin();
        }

        // If the next detail exists in the list
        if( itr != PromptDetails_t::end())
        {
            // Get the detail at this position
            nextDetail = *itr;
        }
    }
    else
    {
        itr = PromptDetails_t::end();
    }
    return( itr);
}

