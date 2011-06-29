//******************************************************************************
//
//     $Header: /Ccrt/Source/Core/ServicesLayer/Library/INamedDataBroker.cpp 3     4/13/06 3:41a Cward $
//
// Description:
//	Client interface to Named Data Broker.
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/INamedDataBroker.cpp $
// 
// 3     4/13/06 3:41a Cward
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
// 3     10/24/03 12:13p Khykin
// Updated with latest mutex changes.
//
// 2     10/10/03 9:33a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/25/03 1:57p Khykin
// Added mutexes to protect during access.
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:01p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:06:07  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:15a Khykin
// Initial Checkin
//
// 1     1/18/03 8:10a Khykin
// Initial Checkin From DCTC
//
// 63    11/20/02 7:01a Khykin
// Updated the constructor to allow a receive buffer size and timeout
// value.
//
// 62    8/28/02 1:52p Kh
// Updating for changes made at the pit machine.
//
// 61    8/14/02 5:15p Skypig13
// Removed Copy of nodes in PrimaryFunctionMethod()
//
// 60    8/14/02 12:11p Skypig13
// Updated to better handle memory
//
// 59    7/31/02 12:04p Skypig13
// Latest files from pit machine
//
// 58    6/27/02 3:52p Skypig13
// Updated SendToServer()
//
// 57    6/24/02 11:58a Dan.erickson
// Tim's latest updates.
//
// 55    6/19/02 2:57p Skypig13
// Updated to use new GetNext() method
//
// 54    4/08/02 1:57p Skypig13
// Added XmlNode versions of Read(), Write(), etc.
//
// 53    4/02/02 10:55p Skypig13
// Updates for efficiency
//
// 52    3/22/02 11:29a Dan.erickson
// In VerifyMethod(), alias name was replacing the value of the
// node.  Modified the software to create a new node with the
// tag and alias name and add that to the Table, leaving the
// original node intact.
//
// 51    3/20/02 3:10p Dan.erickson
// Moved clearing of a response varialble.  It was inside a conditional
// block and would not always be executed.
//
// 50    3/13/02 3:09p Skypig13
// Updated printout on line
//
// 49    2/26/02 2:08p Skypig13
// Update StateMachine() so XmlNode gets updated as out parameter
//
// 48    2/21/02 10:58a Skypig13
// Updated VerifyMethod() to fix bug if match was found
//
// 47    2/13/02 12:56p Skypig13
// Update VerifyMethod()
//
// 46    2/12/02 6:13p Skypig13
// Oops
//
// 45    2/12/02 5:17p Dan.erickson
// Updated to make better use of XmlNode Data.
//
// 44    2/12/02 12:21p Skypig13
// Fixed VerifyMethod()
//
// 43    2/05/02 1:43p Skypig13
// Removed warnings
//
// 42    2/05/02 12:53a Skypig13
// Changes in BepServer and IBepCommunication as well as removing compiler
// warnings
//
// 41    12/19/01 11:13p Skypig13
// Updated Unsubscibe() so if a value is passed in it will set
// it as subscriber name else it will use stored value
//
// 40    12/11/01 4:30p Skypig13
// Updated VerifyMethod()
//
// 39    12/10/01 4:12p Skypig13
// Several changes to correct bugs and increase flexibility in the future
//
// 38    12/05/01 11:34a Skypig13
// Changed so if Verify does not return with success it goes
// directly to Invalidate state
//
// 37    11/29/01 2:57p Skypig13
// Update to make Subscribe() and UnsubScribe() work
//
// 36    11/20/01 4:05p Skypig13
// Fixed memory leak
//
// 35    11/15/01 8:29p Skypig13
// Fixing leak
//
// 34    11/15/01 7:13p Skypig13
// Updated to use new structure
//
// 33    11/13/01 4:54p Skypig13
// Fixed verify method
//
// 32    11/13/01 9:39a Skypig13
// Update verify method to properly handle new servers
//
// 31    11/12/01 2:47p Skypig13
// Added header
//
//
//******************************************************************************
#include <time.h>

#include "Bepos.h"
#include "XmlParser.h"
#include "INamedDataBroker.h"

INamedDataBroker::INamedDataBroker(void) : INamedDataBrokerBase(), m_tempComm(NULL)
{
	memset( &m_lockAttr, 0, sizeof( m_lockAttr));
	memset( &m_stateLock, 0, sizeof( m_stateLock));

	if( (errno=pthread_mutexattr_init( &m_lockAttr)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutexattr_init error");
	}
    else if( (errno=pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE)) != EOK)
    {
		perror( "INamedDataBroker: pthread_mutexattr_settype error");
    }
	else if ( (errno=pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutexattr_setrecursive error");
	}
	else if ((errno=pthread_mutex_init( &m_stateLock, &m_lockAttr)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutex_init error");
	}
	Initialize(std::string(NAMED_DATA_BROKER_NAME), std::string("Client"), false);
}
INamedDataBroker::INamedDataBroker(const bool debug, const INT32 size /* = DEFAULT_BUFFER_SIZE */,
								   const INT32 timeout /* = DEFAULT_TIMEOUT */) :
								   INamedDataBrokerBase(), m_tempComm(NULL)
{
	memset( &m_lockAttr, 0, sizeof( m_lockAttr));
	memset( &m_stateLock, 0, sizeof( m_stateLock));

	if( (errno=pthread_mutexattr_init( &m_lockAttr)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutexattr_init error");
	}
    else if( (errno=pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE)) != EOK)
    {
		perror( "INamedDataBroker: pthread_mutexattr_settype error");
    }
	else if ( (errno=pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutexattr_setrecursive error");
	}
	else if ((errno=pthread_mutex_init( &m_stateLock, &m_lockAttr)) != EOK)
	{
		perror( "INamedDataBroker: pthread_mutex_init error");
	}

	Initialize(std::string(NAMED_DATA_BROKER_NAME), std::string("Client"), debug, size, timeout);
}

INamedDataBroker::~INamedDataBroker(void)
{	// Get rid of memory
	for(CommObjMapIter iter = m_CommObjs.begin(); iter != m_CommObjs.end(); ++iter)
	{
		if(iter->second)
		{
			delete iter->second;
		}
	}
	m_CommObjs.clear();


	pthread_mutexattr_destroy( &m_lockAttr);
	pthread_mutex_destroy( &m_stateLock);

}
INT32 INamedDataBroker::Read(const std::string &tag, std::string &response, const bool IsFinal /* = false */)
{
	XmlNode node(tag, "");
	return(StateMachine(BEP_READ, &node, response, IsFinal));
}
INT32 INamedDataBroker::Read(const XmlNode *node, std::string &response, const bool IsFinal)
{
	return(StateMachine(BEP_READ, node, response, IsFinal));
}
INT32 INamedDataBroker::Write(const std::string &tag, const std::string &value,
							  std::string &response, const bool IsFinal	/* = false */)
{
	XmlNode node(tag, value);
	return(StateMachine(BEP_WRITE, &node, response, IsFinal));
}
INT32 INamedDataBroker::Write(const XmlNode *node, std::string &response, const bool IsFinal)
{
	return(StateMachine(BEP_WRITE, node, response, IsFinal));
}
INT32 INamedDataBroker::Command(const std::string &tag, const std::string &value,
								std::string &response, const bool IsFinal /* = false */)
{
	XmlNode node(tag, value);
	return(StateMachine(BEP_COMMAND, &node, response, IsFinal));
}
INT32 INamedDataBroker::Command(const XmlNode *node, std::string &response, const bool IsFinal)
{
	return(StateMachine(BEP_COMMAND, node, response, IsFinal));
}
INT32 INamedDataBroker::Subscribe(const std::string &tag, const std::string &value,
								  std::string &response, const bool IsFinal	/* = false */)
{
	SetSubscriberName(value);
	XmlNode node(tag, value);
	return(StateMachine(BEP_SUBSCRIBE, &node, response, IsFinal));
}
INT32 INamedDataBroker::Subscribe(const XmlNode *node, std::string &response, const bool IsFinal)
{
	SetSubscriberName(node->getValue());
	return(StateMachine(BEP_SUBSCRIBE, node, response, IsFinal));
}
INT32 INamedDataBroker::Unsubscribe(const std::string &tag, const std::string &value,
									std::string &response, const bool IsFinal /* = false */)
{
	if(!value.empty())	SetSubscriberName(value);
	XmlNode node(tag, GetSubscriberName());
	return(StateMachine(BEP_UNSUBSCRIBE, &node, response, IsFinal));
}


const INT32 INamedDataBroker::GetNext(std::string &tag, std::string &value, std::string &message)
{
    XmlNode node(XML_T(""), XML_T(""));
    const INT32 stat = INamedDataBroker::GetNext(node, message);
    tag = node.getName();
    value = node.getValue();
    return(stat);
}

const INT32 INamedDataBroker::GetNext(XmlNode &node, std::string &message)
{
    if(IsDebugOn()) printf("INamedDataBroker::GetNext(node)\n");
    INT32 stat = IBepCommunication::GetNext(node, message);
    if(node.getValue() == BEP_ERROR_RESPONSE)
    {
        try
        {
            SetMessageStatus(node.getAttribute(BEP_REASON)->getValue());
        }
        catch(BepException& BepErr)
        {
            SetMessageStatus(BEP_UNKNOWN_ERROR_RESPONSE);
        }
    }
    if(IsDebugOn()) printf("INamedDataBroker::GetNext(%s) done\n", node.ToString().c_str());
    return(stat);
}

const INT32 INamedDataBroker::GetByTag(const std::string tag, std::string &value, std::string &message)
{
    return(IBepCommunication::GetByTag(tag, value, message));
}

const std::string INamedDataBroker::GetMessageStatus(void)
{
    return(IBepCommunication::GetMessageStatus());
}

INT32 INamedDataBroker::Publish(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    return(BEP_STATUS_SUCCESS);
}

void INamedDataBroker::ResetCommunications(void)
{
    IBepCommunication::ResetCommunications();
}

void INamedDataBroker::Close(void) const
{
}

bool INamedDataBroker::IsDataOk(const XmlString &data)
{
    return(IBepCommunication::IsDataOk(data));
}



const ServerIdState INamedDataBroker::StartMethod(const std::string &function,
												  XmlNodeMap &TestData,
												  XmlNodeMap &BadData,
												  XmlNodeMap &FinalData)
{
	XmlNode *node;
	ServerIdState ReturnState = Primary;	// Set to verify state
	if(IsDebugOn())	printf("INamedDataBroker::StartMethod()\n");

	BadData.clear(true);		// Clear out basket
	FinalData.clear(true);		// Clear out basket
	m_sendData.clear(true);		// Clear out basket

	for(UINT32 ii = 0; ii < TestData.size();)
	{	// Get server name and availability
		node = const_cast<XmlNode *>(TestData.getNode(ii));
		if(node != NULL)
		{
			if(IsDebugOn())	printf("StartMethod: TestData.size(): %d, ii: %d, node: %s\n", TestData.size(), ii, node->ToString().c_str());
			if(!IsDataOk(GetServerName(node->getName())))
			{	// If server is not available put it in bad data
				BadData.addNode(node);
				TestData.delNode(node->getName());	// Go bye bye
			}
			else ii++;	// Otherwise it is ok, go to next
		}
		else ii++;	// Otherwise it is ok, go to next
	}
	if(!TestData.size() && BadData.size()) ReturnState = Verify;	// A Tag and Server was not found must get

	if(IsDebugOn())	printf("INamedDataBroker::StartMethod() done\n");
	return(ReturnState);
}
const ServerIdState INamedDataBroker::PrimaryFunctionMethod(const std::string function,
															std::string& message,
															XmlNodeMap &TestData,
															XmlNodeMap &BadData,
															XmlNodeMap &FinalData)
{
	ServerIdState ReturnState = Final;	// Set to end state
	XmlNode *node;
	if(IsDebugOn())	printf("INamedDataBroker::PrimaryFunctionMethod(%s)\n", function.c_str());
	while(TestData.size())
	{
		node    = const_cast<XmlNode *>(TestData.getNode(0));
		if(SendToServer(function, message, node, FinalData) == BEP_STATUS_SUCCESS)
		{	// Success!
			if(IsDebugOn())	printf("Adding %s to FinalData\n", node->ToString().c_str());
		}
		else
		{	// Failed to perform operation
			BadData.addNode(node);
			if(IsDebugOn())	printf("Adding %s to BadData\n", node->ToString().c_str());
		}
		TestData.delNode(node->getName());	// Go jump off a cliff.
	}
	if(BadData.size()) ReturnState = Verify;	// Go get data that we haven't got before
	if(IsDebugOn())	printf("INamedDataBroker::PrimaryFunctionMethod(%s) done\n", function.c_str());
	return(ReturnState);
}
const ServerIdState INamedDataBroker::VerifyMethod(const std::string &function,
												   XmlNodeMap &TestData,
												   XmlNodeMap &BadData,
												   XmlNodeMap &FinalData)
{
	std::string tag, value, message, alias;
	ServerIdState ReturnState = Secondary;		// Set to error
	if(IsDebugOn())	printf("INamedDataBroker::VerifyMethod()\n");
	m_sendData.clear(true);
	for(UINT32 ii = 0; ii < BadData.size(); ii++)
	{	// Send each request to the Named Data Broker
		SendToNdb(BEP_VERIFY, BadData.getNode(ii)->getName(), std::string(BEP_VERIFY), message, ii == (BadData.size()-1));
	}

	UINT32 jj = 0;
	XmlNode dataNode = XmlNode("", "");
	XmlNode *node;
	while(jj < BadData.size())
	{
		GetNext(dataNode, message);	// Get next response (has tag and server name)
		if(IsDataOk(dataNode.getValue()))
		{	// If server name is good
			alias = FindValueInTable(dataNode.getValue(), m_servers);	// Find alias name
			if(alias.empty())
			{	// Alias not found in servers table
				char temp[32];
				memset(temp, 0, sizeof(temp));
				sprintf(temp, "Alias%d", GetNextAlias());
				alias = temp;			  // Get next availabe alias
				SetServerStatus(alias, dataNode.getValue(), BEP_AVAILABLE_RESPONSE, true);	// Add alias and status to table
			}
			// Set alias name of verified server
			dataNode.setValue(alias);
			AddTableEntry(&dataNode, m_data);	// Set alias name of verified server
			// Remove entry from the BadData list and place it in the TestData list
			node = const_cast<XmlNode *>(BadData.getNode(dataNode.getName()));
			TestData.addNode(node);					// add the node to the TestData
			BadData.delNode(dataNode.getName());	// remove it from the BadData
		}
		else jj++;
	}
	if(TestData.size() == 0) ReturnState = Invalidate;
	if(IsDebugOn())	printf("INamedDataBroker::VerifyMethod() done\n");
	return(ReturnState);
}
const ServerIdState INamedDataBroker::SecondaryFunctionMethod(const std::string function,
															  std::string& message,
															  XmlNodeMap &TestData,
															  XmlNodeMap &BadData,
															  XmlNodeMap &FinalData)
{
	XmlNode *node = NULL;
	ServerIdState ReturnState = Invalidate;	// Set to error
	if(IsDebugOn())	printf("INamedDataBroker::SecondaryFunctionMethod()\n");
	while(TestData.size())
	{	// Go through all itms to be processed
		node = const_cast<XmlNode *>(TestData.getNode(0));
		if(IsDebugOn())
		{
			if(node != NULL) printf("node: %p\n", node);
			else	printf("node is NULL\n");
		}
		if((function == BEP_SUBSCRIBE) || (function == BEP_UNSUBSCRIBE)) node->setValue(GetSubscriberName());
		if(IsDebugOn())	printf("node: %s\n", node->ToString().c_str());

		if(SendToServer(function, message, node, FinalData) == BEP_STATUS_SUCCESS)
		{	// Success!
			if(IsDebugOn())	printf("Adding %s to FinalData\n", node->ToString().c_str());
		}
		else
		{	// Failed to perform operation
			ReturnState = Invalidate;
			BadData.addNode(node);
			if(IsDebugOn())	printf("Adding %s to BadData\n", node->ToString().c_str());
		}
		TestData.delNode(node->getName());
	}
	if(!BadData.size())	ReturnState = Final;
	if(IsDebugOn())	printf("INamedDataBroker::SecondaryFunctionMethod() done\n");
	return(ReturnState);
}
const ServerIdState INamedDataBroker::InValidateMethod(const std::string &function,
													   XmlNodeMap &TestData,
													   XmlNodeMap &BadData,
													   XmlNodeMap &FinalData)
{
	XmlNode *node;
	std::string tag, value, message, reason;
	ServerIdState ReturnState = Final;
	if(IsDebugOn())	printf("INamedDataBroker::InValidateMethod(%d)\n", BadData.size());
	m_sendData.clear(true);
	while(BadData.size())
	{
		node = const_cast<XmlNode *>(BadData.getNode(0));
		if(node != NULL)
		{
			tag     = node->getName();
			reason  = node->getValue();
			value   = BEP_INVALIDATE;
			BadData.delNode(tag);
			SendToNdb(BEP_INVALIDATE, tag, value, message, !BadData.size());	// Send each request to the Named Data Broker
			node->setValue(reason);
			FinalData.addNode(node);
		}
	}
	if(IsDebugOn())	printf("INamedDataBroker::InValidateMethod() done\n");
	return(ReturnState);
}
const ServerIdState INamedDataBroker::FinalMethod(const std::string &function, XmlNodeMap &FinalData, XmlNodeMap &SendData, std::string& message)
{
	ServerIdState ReturnState = Final;
	if(IsDebugOn())	printf("INamedDataBroker::FinalMethod(%s)\n", function.c_str());
	SendData.clear(true);
	SendData = FinalData;
	BuildMessage(function, message);
	if(SendData.size() > 0)	ReturnState = Success;
	m_testData.clear(true);
	m_badData.clear(true);
	SendData.clear(true);
	FinalData.clear(true);
	if(IsDebugOn())	printf("INamedDataBroker::FinalMethod(%s) done\n", function.c_str());
	return(ReturnState);
}
INT32 INamedDataBroker::StateMachine(const std::string function,
									 const XmlNode *dataNode,
									 std::string &response,
									 const bool IsFinal	/* = false*/)
{
	ServerIdState IdState = Start, lastState = IdState;	// Set state
	INT32 stat = BEP_STATUS_ERROR, attempts = 0;
	if(IsDebugOn())	printf("INamedDataBroker::%s(%s, %s, %d)\n", function.c_str(),
						   dataNode->ToString().c_str(), response.c_str(), IsFinal);

	if( (errno=pthread_mutex_lock( &m_stateLock)) == EOK)
	{
		m_testData.addNode(dataNode->Copy());
		if(IsFinal)
		{
			try
			{
				// Lock our data maps while modifying them
				m_testData.Lock();
				m_badData.Lock();
				m_finalData.Lock();
				m_sendData.Lock();

				// Start processing
				while((IdState != Success) && (attempts++ < 3))
				{
					if(lastState != IdState)
					{
						attempts = 0;
						lastState = IdState;
					}
					if(IsDebugOn())	printf("IdState = %d at start of loop\n", IdState);
					switch(IdState)
					{
					case Start:			// Start check source of data
						IdState = StartMethod(function, m_testData, m_badData, m_finalData);
						break;
					case Primary:		// Tag source not found, so come and get it
						IdState = PrimaryFunctionMethod(function, response, m_testData, m_badData, m_finalData);
						break;
					case Verify:		// Tag source found
						IdState = VerifyMethod(function, m_testData, m_badData, m_finalData);
						break;
					case Secondary:		// Tag source was crap, go get it
						IdState = SecondaryFunctionMethod(function, response, m_testData, m_badData, m_finalData);
						break;
					case Invalidate:	// Tag source was re-found
						IdState = InValidateMethod(function, m_testData, m_badData, m_finalData);
						break;
					case Final:			// Do the last thing and prepare to give data to client
						IdState = FinalMethod(function, m_finalData, m_sendData, response);
						break;
					default:
						throw BepException("Undefined state (ServerIdState)%d", IdState);
					}
					if(IsDebugOn())	printf("IdState = %d at end of loop\n", IdState);
				}
			}
			catch(BepException &BepErr)
			{
				if(IsDebugOn())	printf("INdb BepErr: %s\nState: %d\n", BepErr.what(), IdState);
			}

			// Unlock our data maps when done
			m_testData.Unlock();
			m_badData.Unlock();
			m_finalData.Unlock();
			m_sendData.Unlock();
		}

		pthread_mutex_unlock( &m_stateLock);
	}
	else
	{
		if( IsDebugOn())	perror( "Error locking mutex in StateMachine");
	}
	if(IsDebugOn())	printf("INamedDataBroker::%s(%s) done\n", function.c_str(), response.c_str());
	if((IdState == Success) || (!IsFinal))
		stat = BEP_STATUS_SUCCESS;	// If operation was successful
	return(stat);
}
const INT32 INamedDataBroker::SendToServer(const std::string &function,
										   std::string& message,
										   XmlNode* &dataNode,
										   XmlNodeMap &table,
										   const bool IsFinal /* = true */)
{
	if(IsDebugOn())	printf("SendToServer(%s, %s)\n", function.c_str(), dataNode->ToString().c_str());
	INT32 stat = BEP_STATUS_ERROR;
	std::string name;
	name = GetServerName(dataNode->getName());
	CommObjMapIter iter;
	if((!m_tempComm) || (name != m_tempComm->GetName()))
	{
		iter = m_CommObjs.find(name);
		if(iter == m_CommObjs.end())
		{
			if(IsDebugOn())	printf("Did not find server '%s' for tag %s in map\n", name.c_str(), dataNode->getName().c_str());
			m_tempComm = new IBepCommunication();
			m_tempComm->Initialize(name, std::string("Client"), IsDebugOn(), DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT);
			m_CommObjs.insert(CommObjMapValue(name, m_tempComm));
		}
		else
		{
			if(IsDebugOn())	printf("Found server '%s' for tag %s in map\n", name.c_str(), dataNode->getName().c_str());
			m_tempComm = iter->second;
		}
	}
	try
	{
		stat = m_tempComm->DoOperation(function, dataNode, message, IsFinal);
		if(IsFinal)
		{
			if(IsDebugOn())	printf("Getting data item...\n");
			INT32 num = 0;
			do
			{
				num = m_tempComm->GetNext(*dataNode, message);
				if(IsDebugOn())	printf("Got %s\n", dataNode->ToString().c_str());
				if(dataNode->getValue() == BEP_ERROR_RESPONSE)
				{
					if(IsDebugOn())	printf("Adding %s to %s\n", BEP_REASON, dataNode->ToString().c_str());
					dataNode->addChild(BEP_REASON, m_tempComm->GetMessageStatus(), ATTRIBUTE_NODE);
				}
				table.addNode(dataNode);
			}
			while(num > 0);
		}
	}
	catch(...)
	{	// Comm object is broke, remove it
		delete m_tempComm;
		iter = m_CommObjs.find(name);
		m_CommObjs.erase(iter);
	}
	if(IsDebugOn())	printf("SendToServer(%s, %s) done\n", function.c_str(), dataNode->ToString().c_str());
	return(stat);
}
const INT32 INamedDataBroker::SendToNdb(std::string function,
										const std::string &tag,
										const std::string &value,
										std::string &message,
										const bool IsFinal)
{
	INT32 stat = BEP_STATUS_ERROR;
	if(IsDebugOn())	printf("SendToNdb(%s, %s, %s, %d)\n", function.c_str(), tag.c_str(), message.c_str(), IsFinal);
	stat = IBepCommunication::Command(tag, value, message, IsFinal);
	if(IsDebugOn())	printf("SendToNdb(%s, %s, %s, %d) done\n", function.c_str(), tag.c_str(), message.c_str(), IsFinal);
	return(stat);
}



const std::string INamedDataBroker::GetSubscriberName(void)
{
    if(IsDebugOn())	printf("GetSubscriberName(%s)\n", m_name.c_str());
    return(m_name);
}


void INamedDataBroker::SetSubscriberName(const std::string &name)
{
    if(IsDebugOn())	printf("SetSubscriberName(%s)\n", name.c_str());
    m_name = name;
    if(IsDebugOn())	printf("SetSubscriberName(%s) done\n", m_name.c_str());
}
