//******************************************************************************
// Description:
//   MES Host Interface base lass to handle copying the test result file to the
//   special directory so it can be processed and placed on the MES network.
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MesInterface.h"

//-------------------------------------------------------------------------------------------------
MesInterface::MesInterface() : HostInterface()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MesInterface::~MesInterface()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void MesInterface::Initialize(int argc, char *argv[])
{
	HostInterface::Initialize(argc, argv);
}

//-------------------------------------------------------------------------------------------------
void MesInterface::Initialize(const string &fileName)
{
	HostInterface::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void MesInterface::Initialize(const XmlNode *document)
{   // Initialize the base class
	BepServer::Initialize(document);
	Log(LOG_FN_ENTRY, "ResultFileCopyHostInterface::Initialize() - Enter");
	// Create a new INamedDataBroker comm object
	// if an instance of namedDataBroker interface does not already exist, make one
	if(m_ndb == NULL)
	{
		m_ndb = new INamedDataBroker();
		Log(LOG_DEV_DATA, "Created new NamedDatBroker comm object");
	}
	// Load additional config data
	LoadAdditionalConfigurationItems(document);
	Log(LOG_FN_ENTRY, "ResultFileCopyHostInterface::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
void MesInterface::Run(volatile bool *terminateFlag)
{
	bool notReady = false;
	bool ready = true;
	while(GetStatus() != BEP_TERMINATE)
	{   // Wait for a result to process
		if(ResultReadyToProcess())
		{   // Set a flag indicating we cannot process any results
			ResultReadyToProcess(&notReady);
			// Copy the result file name
			string fileName(PopTestResult());
			if(!fileName.empty())
			{
				string command(string("cp ") + fileName + string(" ") + GetDataTag("MesResultDirectory"));
				if(system(command.c_str()) == -1)
				{
					Log(LOG_ERRORS, "Error copying %s to %s", 
						fileName.c_str(), GetDataTag("MesResultDirectory").c_str());
				}
				else
				{
					Log(LOG_DEV_DATA, "Copied %s to %s for processing", 
						fileName.c_str(), GetDataTag("MesResultDirectory").c_str());
				}
			}
			ResultReadyToProcess(&ready);
			BposSleep(5000);
		}
		else
		{   // Wait for a bit
			BposSleep(5000);
		}
	}
}

//-------------------------------------------------------------------------------------------------
const string MesInterface::Write(const XmlNode *node)
{
	string result(BEP_ERROR_RESPONSE);
	Log(LOG_FN_ENTRY, "MesInterface::Write(%s) - Enter", node->getName().c_str());
	if(!node->getName().compare(TEST_RESULT_TAG))
	{   // Place the result file name onto the list
		PushTestResult(node->getValue());
		result = BEP_SUCCESS_RESPONSE;
	}
	else
	{   // Let the base class handle it
		result = BepServer::Write(node);
	}
	Log(LOG_FN_ENTRY, "MesInterface::Write(%s) - Exit: %s", node->getName().c_str(), result.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
string MesInterface::PopTestResult(void)
{
	string fileName = "";
	if(m_resultFilesToProcess.size() > 0)
	{
		fileName = m_resultFilesToProcess.back();
		m_resultFilesToProcess.pop_back();
	}
	return fileName;
}

//-------------------------------------------------------------------------------------------------
void MesInterface::PushTestResult(const string &fileName)
{   // Lock the list for safety
	if((errno = m_testResultMutex.Acquire()) == EOK)
	{   // Add the filename to the list
		m_resultFilesToProcess.push_back(getenv("USR_ROOT") + fileName);
		Log(LOG_DEV_DATA, "Added result file: %s", fileName.c_str());
		m_testResultMutex.Release();
	}
	else
	{
		Log(LOG_ERRORS, "Could not lock mutex for adding result file name: %s", strerror(errno));
	}
}

