//*************************************************************************
// FILE:
//    $Header: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/FaultServer/FaultServer.cpp 1     11/02/07 1:43p Derickso $
//
// FILE DESCRIPTION:
//   This defines the FaultServer object.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/FaultServer/FaultServer.cpp $
// 
// 1     11/02/07 1:43p Derickso
// Added delete of allFaults XmlNode at the end of UpdateFaultList to
// prevent memory leaks.
// 
// 3     3/30/06 1:43a Cward
// Updated to add Fault History Log PE#79
//
// 2     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 1     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 3     6/26/03 1:22p Khykin
// Updated with latest changes during the Flatrock debug.
//
// 25    6/24/03 10:16a Tbochene
// Updated malformed message checking
//
// 24    6/24/03 9:10a Tbochene
// Updated for thread safety and updated logging
//
// 23    6/24/03 8:56a Derickso
// Check for malformed messages prior to processing.
//
// 22    10/17/02 11:06a Derickso
// Added functionality to clear all faults.
//
// 21    7/31/02 11:02a Skypig13
// Changes due to notification list prototype changes
//
// 20    7/30/02 11:33a Dan.erickson
// Need to invoke the Start() method for the NotificationThreadPool.
//
// 19    6/19/02 2:10p Skypig13
// Updated NotificationList configuration setup
//
// 18    6/07/02 11:44a Dan.erickson
// Moved setup of Notification list to Initialize(XmlNode *) method.
// Updated Read() method to keep an iterator when a find is performed.
// Replaced occurrances of ToString() with writeNode().
// Removed mutex logic since it was pushed down into the SetData() method.
// Cleaned up potential memory leaks.
// Replaced try catch blocks with if else, which is faster.
//
// 17    4/15/02 3:33p Dan.erickson
// Add constructor and destructor methods.
// Add creation of notification list in the Initialize method since
// we need a special comm object type of notification list.
// Remove the comm object that was being passed around to use for
// notification since the notification list is set up as an
// IFaultServer comm object.
//
// 16    3/21/02 1:57p Dan.erickson
// Need to pass IFaultServer object to Notify() method so when
// faults are published, the MssgClass attribute is placed in
// the root node of the message.
//
// 15    3/20/02 9:08a Dan.erickson
// Overloaded Initialize(std::string &fileName) method.
//
// 14    3/11/02 1:13p Dan.erickson
// Updated FindFaultNode() method to remove Copy operations.
// Added diagnostic statements.
// Removed obsolete methods and code.
//
// 13    3/05/02 5:01p Dan.erickson
// Updated to make better use of Xml Nodes.
// Removed SpecialProcessing method.
//
// 12    3/04/02 4:54p Dan.erickson
// Updated Initialize method to set m_comm prior to calling
// BepServer::Initialize().
//
// 11    2/12/02 5:12p Dan.erickson
// Updated because of BepServer restructuring.
//
// 10    1/16/02 4:27p Dan.erickson
// Modified Notify method to send a pointer to an IFaultServer
// object to the base Notify method.  This allows the IFaultServer
// BuildMessage method to be called during a Publish.
//
// 9     1/15/02 3:54p Dan.erickson
// Read method: Modified to not include AllFaults node
// in response.
// UpdateFaultList: Modified to remove AllFaults node before
// updating lists.  Added diagnostics.  Call BepServer::write to
// notify subscribers of AllFaults.
//
// 8     1/11/02 4:50p Dan.erickson
// Added functionality to allow subscribers to subscribe for all faults.
//
// 7     1/04/02 5:31p Dan.erickson
// Modified Initialize method to reflect new location of configuration
// data.
//
// 6     12/03/01 11:35a Dan.erickson
// Added notify method.
// Modified UpdateFaultList method to call the new notify method.
//
// 5     11/20/01 4:52p Dan.erickson
// Testing.
//
// 4     11/20/01 11:05a Dan.erickson
// Corrected compile errors.
//
// 3     11/20/01 10:57a Dan.erickson
// Modified publish scheme when a fault is set or cleared.
//
// 2     11/20/01 10:13a Dan.erickson
// Finished writing.  Compiles, not debugged or tested.
//
// 1     11/20/01 8:31a Dan.erickson
// New server files.
//
//
//*************************************************************************

#include "FaultServer.h"
#include "NotificationThreadPool.h"
#include "FaultServerCommThreadPool.h"

//-----------------------------------------------------------------------------
FaultServer::FaultServer() : BepServer()
{	// Make sure all data is cleared when we are born
	ClearData();
}

//-----------------------------------------------------------------------------
FaultServer::~FaultServer()
{	// Clear all data we have received
	ClearData();
	if(NULL != m_faultLogger) delete m_faultLogger;
	// Disconnect from the QNX data server
	if(ConnectToQnxDataServer())  m_qnxDataServer.DisconnectFromQnxDataServer();
}

//-----------------------------------------------------------------------------
void FaultServer::Initialize(const std::string &fileName)
{
	BepServer::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void FaultServer::Initialize(const XmlNode *document)
{
	// Set up the notification thread pool
	m_nListConfig = document->getChild("Setup/NotificationList")->Copy();
	NotificationThreadPool<IFaultServer> *notifyPool = new NotificationThreadPool<IFaultServer>();
	notifyPool->Initialize(m_nListConfig);
	notifyPool->Start();
	m_nList = notifyPool;
	// Set up the communication thread pool
	m_comm = new FaultServerCommThreadPool(this);

	// Initialize the base server
	BepServer::Initialize(document);

	SetUpFaultLogger(document);

	Log(LOG_FN_ENTRY, "FaultServer::Initialize(%s)", document->ToString().c_str());

	try
	{	// Get the tag being used for All Faults
		SetAllFaultsTag(document->getChild("Setup/DataTags/AllFaultsTag/")->getValue());
	}
	catch(XmlException &XmlErr)
	{	// Default since the tag was not specified.
		SetAllFaultsTag("AllFaults");
	}
	// Connect to the QNX data server if config says to
	bool connect = false;
	try
	{
		connect = atob(document->getChild("Setup/ConnectToQnxDataServer")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Connect to qnx data server not specified, not connecting: %s", excpt.GetReason());
		connect = false;
	}
	ConnectToQnxDataServer(&connect);
}

//-----------------------------------------------------------------------------
const string FaultServer::Register(void)
{	// Call the base class to begin the registration
	string result = BepServer::Register();
	// Determine if the data server should be connected.
	if(ConnectToQnxDataServer())
	{
		Log(LOG_DEV_DATA, "Connected to QNX Data Server: %s", 
			ConvertStatusToResponse(m_qnxDataServer.ConnectToQnxDataServer()).c_str());
	}
	return result;
}

//-----------------------------------------------------------------------------
const std::string FaultServer::Read(const XmlNode *dataNode, const INT32 rate) /*= 0*/
{
	INT32 status = EOK;

	std::string result;
	XmlNodeMap faultMap;
	XmlNodeMapItr faultMapIter;
	result.erase();

	Log(LOG_FN_ENTRY, "FaultServer::Read(dataNode:%s)\n", dataNode->ToString().c_str());

	if((status = m_faultDataLock.Acquire()) == EOK)
	{
		try
		{
			if((faultMapIter = m_processFaults.find(dataNode->getName())) != m_processFaults.end())
			{	// Get process related faults
				Log(LOG_DEV_DATA, "Getting Process related faults\n");
				faultMap.DeepCopy(faultMapIter->second->getChildren());
			}
			else if((faultMapIter = m_severityFaults.find(dataNode->getName())) != m_severityFaults.end())
			{	// Get severity level related faults
				Log(LOG_DEV_DATA, "Getting Severity Level related faults\n");
				faultMap.DeepCopy(faultMapIter->second->getChildren());
			}
			else if(GetAllFaultsTag() == dataNode->getName())
			{	// Return all faults
				Log(LOG_DEV_DATA, "Getting All related faults\n");
				faultMap.DeepCopy(m_data);
			}
			// Make sure the fault map is not empty
			if(!faultMap.empty())
			{	// Get all faults that match the request
				for(XmlNodeMapItr iter = faultMap.begin(); iter != faultMap.end(); iter++)
				{	// Append the fault to the string
					if(iter->second->getName() != GetAllFaultsTag())
						iter->second->writeNode(result);
				}
			}
			else
			{	// Just read the tag normally
				result = BepServer::Read(dataNode, rate);
			}
			// Clear out the fault map we created
			faultMap.clear(true);
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "Exceptional Condition in FaultServer::Read(): %s\n", e.what());
			result = BEP_ERROR_RESPONSE;
		}

		if((status = m_faultDataLock.Release()) != EOK)
		{
			Log(LOG_ERRORS, "Failed to release data acces synchronization variable because: %s", strerror(status));
		}
	}
	else Log(LOG_ERRORS, "Failed to acquire data acces synchronization variable because: %s", strerror(status));

	Log(LOG_FN_ENTRY, "FaultServer::Read() returning: -->%s<--\n", result.c_str());

	return(result);
}

//-----------------------------------------------------------------------------
const std::string FaultServer::Write(const XmlNode *dataNode)
{
	INT32 status = EOK;
	Log(LOG_FN_ENTRY, "FaultServer::Write(%s)\n", dataNode->ToString().c_str());

	std::string result = BEP_ERROR_RESPONSE;
	std::string process = dataNode->getAttribute(BEP_PROCESS)->getValue();
	std::string severity = dataNode->getAttribute(BEP_SEVERITY)->getValue();
	std::string name = dataNode->getName();

	if((status = m_faultDataLock.Acquire()) == EOK)
	{	// Only process this fault if the critical data is available
		try
		{
			if(!name.empty())
			{	// Name is not empty
				if(dataNode->getValue() == SET)
				{
					if(!process.empty())
					{	// Process is not empty
						if(!severity.empty())
						{	// Severity is not empty
							// Store the fault and then update all the lists
							result = SetData(dataNode);
							m_nList->Notify(dataNode);
							AddProcessNameFault(dataNode, SET);
							AddSeverityLevelFault(dataNode, SET);
							UpdateFaultList(name, SET);
						}
						else Log(LOG_ERRORS, "Improperly formed fault message --> severity is empty\n");
					}
					else Log(LOG_ERRORS, "Improperly formed fault message --> process is empty\n");
				}
				else if(dataNode->getValue() == CLEAR)
				{	// Check if this is a clear all faults request
					if(name == GetAllFaultsTag())
					{	// Clear all faults from the server
						m_data.clear(true);
						// Update all fault lists
						UpdateFaultList(name, CLEAR);
						// Return success
						result = BEP_SUCCESS_RESPONSE;
					}
					else
					{	// Clear the fault and then update all the lists
						result = SetData(dataNode);
						m_nList->Notify(dataNode);
						UpdateFaultList(name, CLEAR);
						// Return success
						result = BEP_SUCCESS_RESPONSE;
					}
				}
				else Log(LOG_ERRORS, "Improperly formed fault message --> data is not %d or %d\n",SET, CLEAR);
			}
			else Log(LOG_ERRORS, "Improperly formed fault message --> name is empty\n");
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "Exceptional Condition in FaultServer::Write(): %s\n", e.what());
			result = BEP_ERROR_RESPONSE;
		}

		Log(LOG_DEV_DATA, "FaultServer received fault:%s, value:%s from process:%s with description:%s and severity:%s\n",
			dataNode->getName().c_str(), dataNode->getValue().c_str(), dataNode->getAttributes().getNode(BEP_PROCESS)->getValue().c_str(),
			dataNode->getAttributes().getNode(BEP_DESCRIPTION)->getValue().c_str(),
			dataNode->getAttributes().getNode(BEP_SEVERITY)->getValue().c_str());

		if((status = m_faultDataLock.Release()) != EOK)
		{
			Log(LOG_ERRORS, "Failed to release data acces synchronization variable because: %s", strerror(status));
		}
	}
	else Log(LOG_ERRORS, "Failed to acquire data acces synchronization variable because: %s", strerror(status));

	return(result);
}

//-----------------------------------------------------------------------------
void FaultServer::UpdateFaultList(const std::string &fault, const std::string &value)
{
	Log(LOG_FN_ENTRY, "FaultServer::UpdateFaultList(fault:%s, value:%s)\n", fault.c_str(), value.c_str());


	if(NULL != m_faultLogger)
	{
		std::string tempStat;

		// send to unique fault log
		if(value == "1") tempStat = "+ ";
		else if(value == "0") tempStat = "- ";
		else tempStat = "?";
		m_faultLogger->Log(LOG_DEV_DATA,"%s %s [%s]",
						   tempStat.c_str(), fault.c_str(), value.c_str());
	}
	XmlNodeMapItr faultMapIter;
	if((faultMapIter = m_data.find(GetAllFaultsTag())) != m_data.end())
	{	// Delete the AllFaults node if it exists
		delete faultMapIter->second;
		m_data.delNode(GetAllFaultsTag());
	}
	// Update the other lists
	if(value == CLEAR)
	{	// Remove the fault from all fault lists
		if((faultMapIter = m_data.find(fault)) != m_data.end())
		{
			// delete the object that is referenced by the pointer
			if(faultMapIter->second != NULL)
			{
				delete faultMapIter->second;
				faultMapIter->second = NULL;
			}
			// delete the node map entry
			m_data.delNode(fault);
		}

		// Reload the process fault list and the severity fault list
		m_processFaults.clear(true);
		m_severityFaults.clear(true);

		for(XmlNodeMapItr iter = m_data.begin(); iter != m_data.end(); iter++)
		{
			Log(LOG_DEV_DATA, "Processing fault: %s\n", iter->second->ToString().c_str());
			AddProcessNameFault(iter->second);
			AddSeverityLevelFault(iter->second);
		}
		// Notify everyone of the new fault data
		for(XmlNodeMapItr iter = m_processFaults.begin(); iter != m_processFaults.end(); iter++)
		{	// Publish all faults for each process that has generated faults
			m_nList->Notify(iter->second);
		}
		for(XmlNodeMapItr iter = m_severityFaults.begin(); iter != m_severityFaults.end(); iter++)
		{	// Publish all faults for each severity level
			m_nList->Notify(iter->second);
		}
	}
	// Publish the AllFaults tag
	XmlNode *allFaults = new XmlNode(GetAllFaultsTag(),"");
	string dataServerFaultList(",");
	for(XmlNodeMapItr iter = m_data.begin(); iter != m_data.end(); iter++)
	{
		allFaults->addChild(iter->second->Copy());
		dataServerFaultList += (iter->second->getName() + ",");
	}
	if(ConnectToQnxDataServer())
	{	// Update the data in the QNX data server
		Log(LOG_DEV_DATA, "Set fault list in QNX Data Server to [%s], status: %s", dataServerFaultList.c_str(),
			ConvertStatusToResponse(m_qnxDataServer.UpdateQnxDataServerVariable(GetAllFaultsTag(), dataServerFaultList)).c_str());
	}
	m_nList->Notify(allFaults);
	Log(LOG_DEV_DATA, "Notify %s\n", allFaults->ToString().c_str());
	SetData(allFaults);
	// Clear out the allFaults node since we are done with it
	allFaults->clear();
	delete allFaults;
	Log(LOG_FN_ENTRY, "FaultServer::UpdateFaultList() done\n");
}
void FaultServer::AddProcessNameFault(const XmlNode *node, const bool publishFault)	/*= false*/
{
	// Get the node for the process fault
	IFaultServer faultComm;
	Log(LOG_FN_ENTRY, "FaultServer::AddProcessNameFault(fault:%s, publishFault:%d)\n", node->ToString().c_str(), publishFault);
	std::string process = faultComm.ProcessNameToTag(node->getAttributes().getNode(BEP_PROCESS)->getValue());
	XmlNode *processNode = FindFaultNode(PROCESS_MAP, process);
	// Add the fault to the process name node
	processNode->addChild(node->Copy());
	// Notify anyone who has subscribed for faults from the process
	if(publishFault) m_nList->Notify(processNode);
	Log(LOG_FN_ENTRY, "FaultServer::AddProcessNameFault() done\n");
}
void FaultServer::AddSeverityLevelFault(const XmlNode *node, const bool publishFault)/*= false*/
{
	// Get the node for the severity level
	Log(LOG_FN_ENTRY, "FaultServer::AddSeverityLevelFault(fault:%s, publishFault:%d)\n", node->ToString().c_str(), publishFault);
	std::string severity = std::string("_") + node->getAttributes().getNode(BEP_SEVERITY)->getValue();
	XmlNode *severityNode = FindFaultNode(SEVERITY_MAP, severity);
	// Add the fault to the severity level node
	severityNode->addChild(node->Copy());
	// Notify anyone who has subscribed for faults at this severity level
	if(publishFault) m_nList->Notify(severityNode);
	Log(LOG_FN_ENTRY, "FaultServer::AddSeverityLevelFault() done\n");
}

XmlNode * FaultServer::FindFaultNode(const std::string &mapType, const std::string &tag)
{
	XmlNodeMapItr faultMapIter;
	XmlNodeMap *faultMap = NULL;
	XmlNode *faultNode = NULL;
	// Get the correct map type
	if(SEVERITY_MAP == mapType)		 faultMap = &m_severityFaults;
	else if(PROCESS_MAP == mapType)	 faultMap = &m_processFaults;
	// Find the fault in the map
	if((faultMapIter = faultMap->find(tag)) != faultMap->end())
	{	// Retrieve the fault node from the map
		faultNode = faultMapIter->second;
	}
	else
	{	// Create the node and add it to the fault list
		faultNode = new XmlNode(tag, "");
		faultMap->addNode(faultNode);
	}
	return(faultNode);
}

/**
 * crafts a string that is unique and represents the current time
 * used to modify the fault logger filename to prevent overwriting on reload
 *
 * @return the specially crafted string
 */
std::string FaultServer::GetFileMod(void)
{
	std::string locString = "";
#if 0
	struct timespec     stRealTime;
	INT8                caTime[32];
	if(0 == clock_gettime( CLOCK_REALTIME, &stRealTime))
	{
		strcpy( (char *)caTime, (char *)ctime( &stRealTime.tv_sec));
		//           1         2
		// 012345678901234567890123
		//"Sat Mar 21 15:58:27 1987\n\0"
		//".87Mar21.1558"
		locString += "."       ;   // .
		locString += caTime[22];   // Y
		locString += caTime[23];   // Y
		locString += caTime[ 4];   // M
		locString += caTime[ 5];   // M
		locString += caTime[ 6];   // M
		locString += caTime[ 8];   // D
		locString += caTime[ 9];   // D
		locString += "."       ;   // .
		locString += caTime[11];   // H
		locString += caTime[12];   // H
		locString += caTime[14];   // M
		locString += caTime[15];   // M
	}
#else
	// Get the current time
	time_t currentTime = time(NULL);
	// Format the time YYMMMDD.HHMM
	char formattedTime[32];
	memset(formattedTime, 0, sizeof(formattedTime));
	strftime(formattedTime, sizeof(formattedTime), ".%Y%b%d.%H%M", localtime(&currentTime));
	locString = formattedTime;
#endif 
	Log(LOG_DEV_DATA,"FileNameModifier: %s\n",locString.c_str());
	return(locString);
}

void FaultServer::SetUpFaultLogger(const XmlNode *document)
{
	m_faultLogger = new ILogger();
	if(NULL != m_faultLogger)
	{
		try
		{
			XmlNode *locNode = document->getChild("Setup/FaultLogger")->Copy();
			// modify the filename
			XmlNode *fileNameNode = const_cast <XmlNode *>(locNode->getChild("Setup/Logging/AdditionalLogFile"));
			std::string filename = fileNameNode->getValue();
			int offset = filename.find_last_of('.');

			filename.insert(offset,GetFileMod());
			fileNameNode->setValue(filename);
			m_faultLogger->Initialize(locNode);
		}
		catch(...)
		{
			Log(LOG_ERRORS, "Unable to start the Fault Log\n");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Unable to create the Fault Logger\n");
	}
}

//-----------------------------------------------------------------------------
inline const bool& FaultServer::ConnectToQnxDataServer(const bool *connect)	/*= NULL*/
{
	if(connect != NULL)	 m_connectToQnxDataServer = *connect;
	return m_connectToQnxDataServer;
}
