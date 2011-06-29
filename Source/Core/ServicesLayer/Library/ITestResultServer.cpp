//******************************************************************************
//
// FILE:	ITestResultServer
//     $Archive::  $
//     $Author: Derickso $
//     $Date: 11/02/07 12:31p $
//
// Description:
//	Extend IInfoCommunication to support reporting of test results
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Library/ITestResultServer.cpp $
// 
// 1     11/02/07 12:31p Derickso
// Added ReportDTC that takes a XmlNode as an argument.
// 
// 3     4/13/06 3:42a Cward
// TestResultDetails::AddDetail() was not using the units passed in
// Removed the default parameter value specifications from the cpp file
//
// 2     3/20/06 10:51a Bmeinke
// TestResultDetails::AddDetail() was not using the units passed in
//
// 2     2/15/06 3:55p Bmeinke
// Removed the default parameter value specifications from the cpp file
// (should only be in the h file)
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 12:50p Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:06:37  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:18a Khykin
// Initial Checkin
//
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
//
// 22    8/23/02 10:42a Dan.erickson
// Added DTC reporting capabilities.
//
// 21    6/18/02 12:51p Dan.erickson
// Added messagePopulated field and support to HandleRequest().
//
// 20    6/07/02 11:58a Dan.erickson
// Added HandleRequest() method to add Result attribute to
// Subscribe nodes before calling the base class method.
//
// 19    5/30/02 4:30p Skypig13
// Added Unsibscribe to VaidateMessage() if case
//
// 18    5/20/02 4:39p Dan.erickson
// Added try catch block around the delChild call in SpecialProcessing().
// Modified ValidateMessage() to allow an improperly formed
// Read message to retrieve test result data.
//
// 17    4/15/02 2:48p Dan.erickson
// Updated ValidateMessage() to take an XmlNode instead of std::string.
//
// 16    3/20/02 10:57a Dan.erickson
// Modified Read() method to call the base IBepCommunication Read()
// method instead of the IInfoCommunication Read() method.
//
// 15    3/12/02 11:53a Dan.erickson
// Updated SpecialProcessing method.
//
// 14    3/05/02 5:06p Dan.erickson
// Removed SpecialProcessing method.
//
// 13    2/26/02 2:49p Dan.erickson
// Updated to take advantage of XmlNode type data.
//
// 12    2/18/02 3:36p Skypig13
// Updated for new BepServer
//
// 11    1/08/02 8:08a Dan.erickson
// Modified GetNextTestResult method to handle a single test
// result instead of just the published test results.
//
// 10    1/04/02 11:22a Dan.erickson
// Added Register message type to list to exclude from
// extended validation in ValidateMessage method.
//
// 9     12/12/01 8:56a Dan.erickson
// Modified SpecialProcessing method to send publish messages
// to server.
//
// 8     12/03/01 12:23p Dan.erickson
// Updated to use macro for TestResultServer name.
//
// 7     11/26/01 3:40p Dan.erickson
// Moved AddDetail and AddDetailUnits methods to IInfoCommunication.
//
// 6     11/14/01 3:47p Dan.erickson
// Added methods to get test result information.
// Debugged and tested.
//
// 5     11/13/01 7:36a Dan.erickson
// Modified ValidateMessage method to handle Subscribe messages.
//
// 4     11/09/01 2:14p Dan.erickson
// Added Initialize, Read, WriteTestResult, AddDetail, and
// AddDetailUnits methods.
//
// 3     11/07/01 10:15a Dan.erickson
// Modifed SpecialProcessing method to not do anything for
// Publish methods.  Also added processing for Write messages.
// Modified ValidateMessage method to handle a Publish message.
//
// 2     11/06/01 10:28a Dan.erickson
// Removed all IInfoCommunication methods.
//
// 1     11/06/01 9:37a Dan.erickson
// Created.
//
//
//******************************************************************************

#include "ITestResultServer.h"

ITestResultServer::ITestResultServer( void) : IInfoCommunication()
{
}

ITestResultServer::~ITestResultServer( void)
{
}

void ITestResultServer::Initialize( IBepCommunication &creator, INT32 id	/*= -1*/)
{	// Call the base class initializer
	IBepCommunication::Initialize( creator, id);
}

void ITestResultServer::Initialize( const std::string &server,
									const std::string &type,
									const bool debug /* = false */,
									const INT32 size /* = DEFAULT_BUFFER_SIZE */,
									const INT32 timeout	/* = DEFAULT_TIMEOUT */)
{	// Make sure we are using the well known name for the test result server
	std::string srvr( TEST_RESULT_SERVER_NAME);	// Well-known name of the TestResultServer to connect to.
	if( server != srvr)
		printf("Attempt to use non-standard TestResultServer name: %s, using: %s\n", server.c_str(), srvr.c_str());
	IBepCommunication::Initialize( srvr, type, debug, size, timeout);
}

void ITestResultServer::Initialize( const XmlNode *config, const INT32 id /* = -1 */, const INT32 rid /* = -1 */)
{
	std::string srvr( TEST_RESULT_SERVER_NAME);	// Well-known name of the TestResultServer to connect to.
	XmlNode *name = const_cast<XmlNode *>( config->getChildren().getNode("Name"));
	if( name->getValue() != srvr)
	{
		printf("Attempt to use non-standard TestResultServer name: %s, using: %s\n", name->getValue().c_str(), srvr.c_str());
		name->setValue( srvr);
	}
	IBepCommunication::Initialize( config, id, rid);
}


XmlNode * ITestResultServer::SetResult( const std::string &result, XmlNode* existNode)
{
	// Add the attribute to the node
	AddAttribute( BEP_RESULT, result, existNode);
	if( IsDebugOn())	   printf("Result attribute added: node = %s\n", existNode->ToString().c_str());
	return( existNode);
}

XmlNode * ITestResultServer::SetCode( const std::string &code, XmlNode* existNode)
{
	// Add the attribute to the node
	AddAttribute( BEP_TEST_RESULT_CODE, code, existNode);
	if( IsDebugOn())	   printf("Code attribute added: node = %s\n", existNode->ToString().c_str());
	return( existNode);
}

INT32 ITestResultServer::WriteTestResult( const std::string test, const std::string result, const std::string process,
										  const std::string description, std::string &message, const bool isFinal /*= false*/,
										  const std::string code /*= "0000"*/)
{	// Get the node be written
	XmlNode node( test, "");
	INT32 status = BEP_STATUS_ERROR;

	// Add the attributes to the node
	SetCode( code, &node);
	SetResult( result, &node);
	SetProcess( process, &node);
	SetDescription( description, &node);
	// Send the message
	status = DoOperation( BEP_WRITE, &node, message, isFinal);

	return( status);
}

INT32 ITestResultServer::WriteTestResult( const std::string test, const std::string result, const std::string process,
										  const std::string description, std::string &message, const TestResultDetails &details,
										  const bool isFinal /*= false*/, const std::string code /*= "0000"*/)
{	// Get the node be written
	XmlNode node( test, "");
	INT32 status = BEP_STATUS_ERROR;
	TestResultDetail_t      detail;
	TestResultDetailsCItr_t itr;

	// Add details for this test result
	itr=details.GetFirstDetail( detail);
	while( itr != details.end())
	{
		if( IsDebugOn())	   printf( "\tITestResultServer::WriteTestResult adding detail %s=%s\n", detail.detailTag.c_str(), detail.detailValue.c_str());
		AddDetail( detail.detailTag, detail.detailValue, &node);
		if( IsDebugOn())	   printf( "\tITestResultServer::WriteTestResult Detail added %s=%s\n", detail.detailTag.c_str(), detail.detailValue.c_str());
		if( detail.detailUnits.empty() == false)
		{
			if( IsDebugOn())	   printf( "\tITestResultServer::WriteTestResult detail unit %s=%s\n", detail.detailTag.c_str(), detail.detailUnits.c_str());
			AddDetailUnits( detail.detailTag, detail.detailUnits, &node);
		}
		itr=details.GetNextDetail( detail, itr);
		if( IsDebugOn())	   printf( "\tITestResultServer::WriteTestResult next detail %s=%s\n", detail.detailTag.c_str(), detail.detailValue.c_str());
	}

	// Add the attributes to the node
	SetCode( code, &node);
	SetResult( result, &node);
	SetProcess( process, &node);
	SetDescription( description, &node);
	// Send the message
	status = DoOperation( BEP_WRITE, &node, message, isFinal);

	return( status);
}

INT32 ITestResultServer::ReportDTC( const std::string &vehicleModule, const std::string &dtcName,
									const std::string &dtcCode, const std::string &dtcDescription)
{	// Get the node to be written
	XmlNode dtcNode( SET_DTC_TAG, dtcCode);
	INT32 status = BEP_STATUS_ERROR;
	std::string response;

	// Add the attributes to the node
	SetCode( dtcCode, &dtcNode);
	SetResult( dtcName, &dtcNode);
	SetProcess( vehicleModule, &dtcNode);
	SetDescription( dtcDescription, &dtcNode);
	status = DoOperation( BEP_WRITE, &dtcNode, response, true);

	return( status);
}

INT32 ITestResultServer::ReportDTC(const std::string &vehicleModule, XmlNode *dtcNode)
{   // Get the DTC info from the supplied node
    XmlNodeMapCItr iter = dtcNode->getAttributes().find(DTC_CODE);
    string dtcCode((iter != dtcNode->getAttributes().end()) ? iter->second->getValue() : "0000");
    XmlNode faultNode(SET_DTC_TAG, dtcCode);
    SetCode(dtcCode, &faultNode);
    SetResult(dtcNode->getName(), &faultNode);
    SetProcess(vehicleModule, &faultNode);
    SetDescription(dtcNode->getValue(), &faultNode);
    // Add the real fault node
    faultNode.addChild(dtcNode->Copy());
    string response;
    INT32 status = DoOperation(BEP_WRITE, &faultNode, response, true);
    return status;
}

INT32 ITestResultServer::ClearDTC( const std::string &vehicleModule, const std::string &dtcCode	/*= CLEAR_ALL_DTC*/)
{	// Get the node to be written
	XmlNode dtcNode( CLEAR_DTC_TAG, dtcCode);
	INT32 status = BEP_STATUS_ERROR;
	std::string response;

	// Add the Attributes to the node
	SetCode( "0000", &dtcNode);
	SetResult( BEP_PASS_STATUS, &dtcNode);
	SetProcess( vehicleModule, &dtcNode);
	SetDescription( CLEAR_DTC_TAG, &dtcNode);
	status = DoOperation( BEP_WRITE, &dtcNode, response, true);

	return( status);
}

INT32 ITestResultServer::Read( const XmlNode *node, std::string &response, const bool isFinal	/*= false*/)
{
	XmlNode *readNode = const_cast<XmlNode*>( node);
	readNode = SetCode("0000", readNode);
	readNode = SetResult( "", readNode);
	readNode = SetProcess( "", readNode);
	readNode = SetDescription( "", readNode);
	return( IBepCommunication::Read( readNode, response, isFinal));
}

std::string ITestResultServer::GetResult( const XmlNode *node)
{
	std::string result = BEP_UNAVAILABLE_RESPONSE;
	try
	{
		// Get the result attribute from the node
		result = node->getAttribute( BEP_RESULT)->getValue();
	}
	catch(...)
	{
		// The node has no result attribute
		result = BEP_UNAVAILABLE_RESPONSE;
	}
	return( result);
}

std::string ITestResultServer::GetCode( const XmlNode *node)
{
	std::string result = BEP_UNAVAILABLE_RESPONSE;
	try
	{
		// Get the test result code attribute from the node
		result = node->getAttribute( BEP_TEST_RESULT_CODE)->getValue();
	}
	catch(...)
	{
		// The node has no code attribute
		result = BEP_UNAVAILABLE_RESPONSE;
	}
	return( result);
}

INT32 ITestResultServer::GetNextTestResult( std::string &testName, std::string &result, XmlNode **testResult)
{	// Get the test results from the parser
	if( IsDebugOn())	   printf("ITestResultServer::GetNextTestResult() - begin:\n");
	XmlNode *allResults;
	INT32 num = 0;
	try
	{
		allResults = const_cast<XmlNode *>( m_parser.getDocElement()->getChildren().getNode( BEP_TEST_RESULT));
		num = allResults->getChildren().size()-1;
		if( num >= 0)
		{
			// Get the first node in the list
			*testResult = allResults->getChildren().getNode(0)->Copy();
			// Get the name of the test and the result
			testName = (*testResult)->getName();
			result = (*testResult)->getAttributes().getNode( BEP_RESULT)->getValue();
			// Remove the test result node from the parser
			allResults->delChild( allResults->getChildren().getNode( testName));
		}
		else
		{
			// Test results are not available
			testName = result = BEP_UNAVAILABLE_RESPONSE;
			testResult = NULL;
		}
	}
	catch( XmlException &XmlErr)
	{
		allResults = const_cast<XmlNode *>( m_parser.getDocElement()->getChildren().getNode(0));
		*testResult = allResults->Copy();
		testName = allResults->getName();
		result = GetResult( allResults);
		num = 0;
	}
	// Return the number of test remaining
	if( IsDebugOn())	   printf("ITestResultServer::GetNextTestResult(%s, %s, %s)\n", testName.c_str(), result.c_str(), (*testResult)->ToString().c_str());
	return( num);	// How many left
}

std::string ITestResultServer::GetTestDescription( const std::string &testName, XmlNode **testResult	/* = NULL*/)
{	// Find the test result node
	FindResultNode( testName, testResult);
	// Get the description for the test result
	std::string description = GetDescription(*testResult);
	if( description == BEP_UNAVAILABLE_RESPONSE)	   description.erase();
	// Return the description for this test result
	return( description);
}

std::string ITestResultServer::GetTestProcess( const std::string &testName, XmlNode **testResult	/*= NULL*/)
{	// Find the test result node
	FindResultNode( testName, testResult);
	// Get the process for this result
	std::string process = GetProcess(*testResult);
	if( process == BEP_UNAVAILABLE_RESPONSE)	   process.erase();
	// Return the process for this test result
	return( process);
}

std::string ITestResultServer::GetTestResult( const std::string &testName, XmlNode **testResult	/*= NULL*/)
{	// Find the test result node
	FindResultNode( testName, testResult);
	std::string result = GetResult(*testResult);
	if( result == BEP_UNAVAILABLE_RESPONSE)	result.erase();
	// Return the result for this test result
	return( result);
}

std::string ITestResultServer::GetTestResultCode( const std::string &testName, XmlNode **testResult	/*= NULL*/)
{	// Find the test result node
	FindResultNode( testName, testResult);
	std::string code = GetCode(*testResult);
	if( code == BEP_UNAVAILABLE_RESPONSE) code.erase();
	// Return the code for this test result
	return( code);
}

INT32 ITestResultServer::GetNextTestResultDetail( const std::string &testName, std::string &detailName,
												  std::string &value, XmlNode **resultDetail, XmlNode **testResult	/*= NULL*/)
{	// Clear out the node
	INT32 num = -1;
	if( IsDebugOn())	   printf("ITestResultServer::GetNextTestResultDetail( test: %s, detail: %s)\n", testName.c_str(), detailName.c_str());
	// Try to get the test result node
	if( FindResultNode( testName, testResult))
	{
		// Get the test results document so we can modify it later
		XmlNode *doc = const_cast<XmlNode *>( m_parser.getDocElement()->getChild( BEP_TEST_RESULT));
		// Get the number of children this node has
		num = (*testResult)->getChildren().size()-1;
		if( num >= 0)
		{
			// If there are children
			*resultDetail = (*testResult)->getChildren().getNode(0)->Copy();
			detailName = (*resultDetail)->getName();
			value = (*resultDetail)->getValue();
			try
			{
				// Delete tag just processed
				doc->delChild( doc->getChild( testName)->getChild( detailName));
			}
			catch(...)
			{
				// Tag may have already been removed in GetNextTestResult()
			}
			(*testResult)->delChild((*testResult)->getChild( detailName));
		}
	}
	else detailName	= value = BEP_UNAVAILABLE_RESPONSE;
	if( IsDebugOn())	   printf("ITestResultServer::GetNextTestResultDetail(%s, %s, %s)\n", testName.c_str(), detailName.c_str(), value.c_str());
	return( num);	// How many left
}

std::string ITestResultServer::GetTestResultDetailUnits( const std::string &testName, const std::string detailName, const XmlNode *resultDetail	/*= NULL*/, XmlNode **testResult	/*= NULL*/)
{
	std::string units = BEP_UNAVAILABLE_RESPONSE;
	if( FindResultNode( testName, testResult))
	{
		// Test Result node exists
		try
		{
			// Try to get the units from the detail node
			units = resultDetail->getAttributes().getNode( BEP_UNITS)->getValue();
		}
		catch(...)
		{
			// Detail node may not have been passed in
			try
			{
				// Get units through the test result node
				units = (*testResult)->getChild( detailName)->getAttributes().getNode( BEP_UNITS)->getValue();
			}
			catch(...)
			{
				// The units just do not exist
				units = BEP_UNAVAILABLE_RESPONSE;
			}
		}
	}
	return( units);
}

const INT32 ITestResultServer::HandleRequest( std::string &message, BepComponent *server	/*= NULL*/, const bool &messagePopulated /*= false*/)
{
	INT32 status = BEP_STATUS_SUCCESS;
	if( IsDebugOn())	   printf("ITestResultServer::HandleRequest() begin\n");
	IInterProcessCommunication *ipc = dynamic_cast<IInterProcessCommunication *>( m_ISystemCommunication);
	// Get the request from the client
	GetRequest( message);
	// Process the request
	if(( ipc != 0) && ( ipc->GetReplyId() == 0))
	{
		// This is a pulse, go directly to base class, do not pass go
		status = IBepCommunication::HandleRequest( message, server, true);
	}
	else
	{
		// Parse the message into an XmlNode
		m_parser.Parse( message.c_str(), message.length(), true, BEP_XML_ROOT);
		XmlNode *doc = const_cast<XmlNode *>( m_parser.getDocElement());
		// Get the message type
		const std::string messageType = GetRequestType( doc);
		// If it is a subscribe message, add the result attribute with a value of TestNotStarted
		if( messageType == BEP_SUBSCRIBE)
		{
			// Get all of the nodes that will need the Result attribute added
			XmlNodeMap nodes = doc->getChildren();
			for( XmlNodeMapItr iter = nodes.begin(); iter != nodes.end(); iter++)
			{
				// Remove the Result attribute if it already exists
				try
				{
					iter->second->delChild( iter->second->getAttribute( BEP_RESULT));
					iter->second->delChild( iter->second->getAttribute( BEP_TEST_RESULT_CODE));
				}
				catch(...)
				{
					// Do nothing.  Attribute may not exist
				}
				// Add the correct attribute to the node
				AddAttribute( BEP_RESULT, BEP_TEST_NOT_STARTED, iter->second);
				AddAttribute( BEP_TEST_RESULT_CODE, "0000", iter->second);
			}
			// Update the message now that the nodes have been updated
			message.erase();
			doc->writeNode( message);
		}
		// Call the base class method to actually do the work
		if( IsDebugOn())	   printf("ITestResultServer::HandleRequest() calling IBepCommunication::HandleRequest() with message: %s\n", message.c_str());
		status = IBepCommunication::HandleRequest( message, server, true);
	}
	return( status);
}

void ITestResultServer::SpecialProcessing( XmlNode *node, const std::string type, BepComponent *server)
{
	if( IsDebugOn())	   printf("ITestResultServer::SpecialProcessing()\n");
	if( type == BEP_READ)
	{
		// Call the base class method so it can do its thing
		IInfoCommunication::SpecialProcessing( node, type, server);
		try
		{
			// Remove the original attributes so we can set the correct values
			node->delChild( node->getAttribute( BEP_RESULT));
			node->delChild( node->getAttribute( BEP_TEST_RESULT));
		}
		catch(...)
		{
			// Do nothing, attribute may not exist.
		}
		// Set the result attribute for this node
		std::string result = server->GetSpecialInfo( node->getName(), BEP_RESULT);
		std::string code = server->GetSpecialInfo( node->getName(), BEP_TEST_RESULT_CODE);
		AddAttribute( BEP_RESULT, result, node);
		AddAttribute( BEP_TEST_RESULT_CODE, code, node);
		if( IsDebugOn())	   printf("Attributes added to node: %s\n", node->ToString().c_str());
	}
}
bool ITestResultServer::ValidateMessage( const XmlNode* doc)
{
	bool status = false;
	bool goodNode = true;
	if( IsDebugOn())	   printf("ITestResultServer::ValidateMessage(%s)\n", doc->ToString().c_str());
	std::string messageType = doc->getAttributes().getNode( BEP_XML_MSG_TYPE)->getValue();
	if( IsDebugOn())	   printf("document:\n%s\n", doc->ToString().c_str());
	// Check additional attributes if base message is valid
	if(( BEP_PUBLISH == messageType) || ( BEP_SUBSCRIBE == messageType) || ( BEP_UNSUBSCRIBE == messageType) ||
	   ( BEP_REGISTER == messageType) || ( BEP_READ == messageType))
	{
		// A Publish message will not neccessarily have all the correct attributes
		status = IBepCommunication::ValidateMessage( doc);
	}
	else if( IInfoCommunication::ValidateMessage( doc))
	{
		// Reload the document in case something got wacked
		try
		{
			// Verify process and description attributes on all nodes in the message
			if( doc->getName() == BEP_XML_ROOT)
			{
				//Check each node for the proper attributes
				XmlNodeMap nodes;
				nodes.DeepCopy( doc->getChildren());	// Get children nodes
				for( XmlNodeMapItr iter = nodes.begin(); ( iter != nodes.end()) && goodNode; iter++)
				{
					// Get the attributes associated with this node
					if( goodNode) goodNode = CheckAttribute( iter->second->getAttribute( BEP_RESULT));
					if( goodNode) goodNode = CheckAttribute( iter->second->getAttribute( BEP_TEST_RESULT_CODE));
				}
				if( goodNode) status = true;
				// Clear out the node map so we do not get a memory leak
				nodes.clear( true);
			}
		}
		catch(...)
		{
			status = false;
		}
	}
	if( IsDebugOn())	   printf("ITestResultServer::ValidateMessage() - Message is %s\n", ( status ? "VALID" : "INVALID"));
	return( status);
}

bool ITestResultServer::CheckAttribute( const XmlNode *attribute)
{
	bool status = false;		// Operation status
	if(( attribute->getName() == BEP_RESULT) || ( attribute->getName() == BEP_TEST_RESULT_CODE) ||
	   IInfoCommunication::CheckAttribute( attribute))
	{
		// If attribute is Ok
		status = true;
	}
	return( status);
}

bool ITestResultServer::FindResultNode( const std::string &testName, XmlNode **resultNode)
{
	bool status = false;
	if(*resultNode == NULL)
	{
		// Test Result node was not provided
		try
		{
			// Get the result node for the test specified
			(*resultNode) = const_cast<XmlNode *>( m_parser.getDocElement()->getChild( BEP_TEST_RESULT)->getChild( testName));
			status = true;
		}
		catch(...)
		{
			// Result node does not exist
			*resultNode = NULL;
		}
	}
	else
	{
		// The node was passed in
		status = true;
	}
	if( IsDebugOn())	   printf("ITestResultServer::FindResultNode( test: %s, node: %s) done\n", testName.c_str(), (*resultNode)->ToString().c_str());
	return( status);
}

//////////////// TestResultDetails methods  /////////////////



TestResultDetails::TestResultDetails()
{
}

TestResultDetails::~TestResultDetails()
{
}

void TestResultDetails::AddDetail( const std::string &tag, const std::string &value, const std::string &units)
{
    const TestResultDetail_t	newDetail = { tag, value, units };
    TestResultDetails_t::insert( TestResultDetails_t::end(), newDetail);
}

TestResultDetailsCItr_t TestResultDetails::GetFirstDetail( TestResultDetail_t &firstDetail) const
{
    TestResultDetailsCItr_t itr = TestResultDetails_t::begin();

    if( TestResultDetails_t::empty() == false)
    {
        // Return the first detail in the list
        firstDetail = *itr;
    }
    else
    {
        itr = TestResultDetails_t::end();
    }
    return( itr);
}

TestResultDetailsCItr_t TestResultDetails::GetNextDetail( TestResultDetail_t &nextDetail, TestResultDetailsCItr_t itr) const
{
    if( TestResultDetails_t::empty() == false)
    {
        // If not at the end
        if( itr != TestResultDetails_t::end())
        {
            // go to the next detail item in the list
            itr++;
        }
        // At the end....wrap back to beginning
        else
        {
            itr = TestResultDetails_t::begin();
        }

        // If the next detail exists in the list
        if( itr != TestResultDetails_t::end())
        {
            // Get the detail at this position
            nextDetail = *itr;
        }
    }
    else
    {
        itr = TestResultDetails_t::end();
    }
    return( itr);
}


