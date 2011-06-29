//******************************************************************************
//
// FILE:	IInfoCommunication
//     $Archive::  $
//     $Author: Cward $
//     $Date: 2/26/04 6:33p $
//
// Description:
//	Extend IBepCommunication to support reporting of information
//
//==============================================================================
//
//     Copyright © 2002 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Core/ServicesLayer/Library/SystemCommunication/IInfoCommunication.cpp $
// 
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004. 
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 1:01p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 3     10/15/03 8:47a Khykin
// Removing debug print statement.
//
// 2     10/10/03 9:36a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:52p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:03:43  tbochene
//
//     PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 19    5/20/02 4:37p Dan.erickson
// Added try catch block around delChild calls in SpecialProcessing
// method so exceptions are caught.
//
// 18    5/06/02 3:42p Dan.erickson
// Corrected memory leak by clearing all node maps before they
// go out of scope.
//
// 17    4/05/02 2:50p Dan.erickson
// Updated ValidateMessage() due to changes in
// IBepCommunication::ValidateMessage().
//
// 16    2/26/02 2:41p Dan.erickson
// Updated SetDescription and SetProcess methods to return
// the node that was built.
//
// 15    2/13/02 8:48a Skypig13
// Removed compiler warnings
//
// 14    11/28/01 12:33p Dan.erickson
// Added PromptServer.
//
// 13    11/26/01 3:40p Dan.erickson
// Moved AddDetail and AddDetailUnits methods to IInfoCommunication.
//
// 12    11/14/01 3:51p Dan.erickson
// Modified logic in CheckAttributes.
//
// 11    11/13/01 1:28p Dan.erickson
// Added GetDescription and GetProcess methods.
//
// 10    11/07/01 10:24a Dan.erickson
// Removed processing for Publish messages in SpecialProcessing method.
//
// 9     11/06/01 10:23a Dan.erickson
// Moved to IInfoCommunication directory.
// Removed methods which were geared more towards providing information
// for TestRsultServer
//
// 7     11/05/01 4:24p Dan.erickson
// Testing.
//
// 6     11/01/01 6:02p Dan.erickson
// Debugged and tested.
//
// 5     11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 4     11/01/01 2:56p Dan.erickson
// Compiles, not tested.
//
// 3     10/26/01 3:58p Dan.erickson
// Revised to only extend IBepCommunication.
//
// 2     10/22/01 3:39p Dan.erickson
// Debugged and tested.
//
// 1     10/19/01 2:37p Dan.erickson
// Revised from ITestResult.
//
//
//******************************************************************************

#include "IInfoCommunication.h"

IInfoCommunication::IInfoCommunication(void) : IBepCommunication()
{
}

IInfoCommunication::~IInfoCommunication(void)
{
	if (IsDebugOn()) printf("IInfoCommunication::~IInfoCommunication() done\n");
}

XmlNode * IInfoCommunication::SetDescription(const std::string &description, XmlNode *existNode)
{
	// Add the attribute to the node
	AddAttribute(BEP_DESCRIPTION, description, existNode);
	if (IsDebugOn()) printf("Description attribute added: node = %s\n", existNode->ToString().c_str());
	return(existNode);
}

XmlNode * IInfoCommunication::SetProcess(const std::string &process, XmlNode *existNode)
{
	// Add the attribute to the node
	AddAttribute(BEP_PROCESS, process, existNode);
	if (IsDebugOn()) printf("Process attribute added: node = %s\n", existNode->ToString().c_str());
	return(existNode);
}

std::string IInfoCommunication::GetDescription(const XmlNode *node)
{
	std::string description = BEP_UNAVAILABLE_RESPONSE;
	try
	{	// Get the description attribute from the node
		description = node->getAttributes().getNode(BEP_DESCRIPTION)->getValue();
	}
	catch (...)
	{	// The node has no description attribute
		description = BEP_UNAVAILABLE_RESPONSE;
	}
	return(description);
}

std::string IInfoCommunication::GetProcess(const XmlNode *node)
{
	std::string process = BEP_UNAVAILABLE_RESPONSE;
	try
	{	// Get the description attribute from the node
		process = node->getAttributes().getNode(BEP_PROCESS)->getValue();
	}
	catch (...)
	{	// The node has no description attribute
		process = BEP_UNAVAILABLE_RESPONSE;
	}
	return(process);
}

void IInfoCommunication::AddDetail(const std::string &detail, const std::string &value, XmlNode *existNode)
{	
	// Add the tag and value as a child node
	existNode->addChild(detail, value, ELEMENT_NODE);
	if (IsDebugOn()) printf("Detail node added: detail = %s\n", existNode->getChild(detail)->ToString().c_str());
}

void IInfoCommunication::AddDetailUnits(const std::string &detail, const std::string &units, XmlNode *existNode)
{	
	// Get the detail node to add the attribute to
	XmlNode *detailNode;
	try
	{
		detailNode = const_cast<XmlNode *>(existNode->getChild(detail));
	}
	catch (...)
	{
		detailNode = new XmlNode(detail, "");
		existNode->addChild(detailNode);
	}
	// Add the units to the detail node
	AddAttribute(BEP_UNITS, units, detailNode);
	if (IsDebugOn()) printf("Units attribute added to detail node: %s\n", existNode->ToString().c_str());
}

void IInfoCommunication::SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server)
{
	if (IsDebugOn()) printf("IInfoCommunication::SpecialProcessing()\n");
	if (type == BEP_READ)
	{	// Call the base class method so it can do its thing
		IBepCommunication::SpecialProcessing(node, type, server);
		// Remove the original attributes so we can set the correct values
		try
		{
			const XmlAttribute* attrib = (const XmlAttribute*)node->getAttributes().getNode(BEP_PROCESS);
//			node->delChild(node->getAttributes().getNode(BEP_PROCESS));
			node->delChild( attrib);
			attrib = (const XmlAttribute*)node->getAttributes().getNode(BEP_DESCRIPTION);
//			node->delChild(node->getAttributes().getNode(BEP_DESCRIPTION));
			node->delChild( attrib);
		}
		catch(...)
		{   // Do nothing, attributes may not exist
		}
		// Set the process and description attributes for this node
		std::string description = server->GetSpecialInfo(node->getName(), BEP_DESCRIPTION);
		AddAttribute(BEP_PROCESS, GetName(), node);
		AddAttribute(BEP_DESCRIPTION, description, node);
		if (IsDebugOn()) printf("Attributes added to node: %s\n", node->ToString().c_str());
	}
}

bool IInfoCommunication::ValidateMessage(const XmlNode* doc)
{
	bool status = false;
	bool goodNode = true;

	// Check additional attributes if base message is valid
	if (IBepCommunication::ValidateMessage(doc))
	{
		try
		{	// Verify process and description attributes on all nodes in the message
			if (IsDebugOn()) printf("IInfoCommunication::ValidateMessage(%s)\n", doc->ToString().c_str());
			if (doc->getName() == BEP_XML_ROOT)
			{
				XmlNodeMap nodes;
				nodes.DeepCopy(doc->getChildren());	// Get children nodes
				//Check each node for the proper attributes
				XmlNodeMap attributes;
				for (XmlNodeMapItr iter = nodes.begin(); (iter != nodes.end()) && goodNode; iter++)
				{   // Get the attributes associated with this node
					attributes.DeepCopy((*iter).second->getAttributes());
					if (goodNode) goodNode = CheckAttribute(attributes.getNode(BEP_PROCESS));
					if (goodNode) goodNode = CheckAttribute(attributes.getNode(BEP_DESCRIPTION));
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
	return(status);
}

bool IInfoCommunication::CheckAttribute(const XmlNode *attribute)
{
	bool status = false;		// Operation status
	const XmlString& name = attribute->getName();
	if ((name == BEP_DESCRIPTION) || (name == BEP_PROCESS) || IBepCommunication::CheckAttribute(attribute))
	{	// If attribute is Ok
		status = true;
	}
	return(status);
}

