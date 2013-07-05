//*************************************************************************
// FILE DESCRIPTION:
//  Remote vehicle build server for Mazda.
//
//===========================================================================
// Copyright (c) 2013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd>
#include <algorithm>
#include "INamedDataBroker.h"
#include "RemoteVehicleBuildServer.h"

//-------------------------------------------------------------------------------------------------
RemoteVehicleBuildServer::RemoteVehicleBuildServer()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
RemoteVehicleBuildServer::~RemoteVehicleBuildServer()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const INT32 RemoteVehicleBuildServer::HandlePulse(const INT32 code, const INT32 value)
{
	INT32 status = BEP_STATUS_ERROR;
	switch(code)
	{
	case BUILD_SERVER_PUSLE_CODE:
		switch(value)
		{
		case BUILD_FILE_CHECK_PULSE:
			LoadVehicleBuildRecords();
			status = BEP_STATUS_SUCCESS;
			break; 

		default:
			Log(LOG_ERRORS, "Unknown pulse value %d", value);
			break;
		}
		break;

	default:
		status = BepServer::HandlePulse(code, value);
		break;
	}
	return status;
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::Initialize(const string &fileName)
{
	BepServer::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
	BepServer::Initialize(document);
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::Initialize() - Enter");

	// Setup for the correct mode
	bool producerMode = false;
	try
	{
		producerMode = !document->getChild("Setup/Parameters/ServerType")->getValue().compare("Producer");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Server type not specified, defaulting to Consumer mode - %s", excpt.GetReason());
		producerMode = false;
	}
	IsInProducerMode(&producerMode);
	Log(LOG_DEV_DATA, "Set server to %s mode", IsInProducerMode() ? "Producer" : "Consumer");

	// Store the vehicle build file path
	string path(getenv("USR_ROOT") + string("/Configuration/VehicleTest/Buildrecords/"));;
	try
	{
		path = document->getChild("Setup/Parameters/BuildFilePath")->getValue();
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Build file path not defined, defaulting to %s - %s", path.c_str(), excpt.GetReason());
	}
	VehicleBuildFilePath(&path);
	Log(LOG_DEV_DATA, "Vehicle build file path: %s", VehicleBuildFilePath().c_str());

	if(!IsInProducerMode())
	{   // Setup the file check timer
		UINT64 timerDelay = 2000;
		try
		{
			timerDelay = BposReadInt(document->getChild("Setup/Parameters/BuildFileCheckDelay")->getValue().c_str());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Build file check delay time not set, using %d - %s", timerDelay, excpt.GetReason());
		}
		SetupTimer(m_buildFileTimer, timerDelay, BUILD_FILE_CHECK_PULSE);
		// Load the existing build records
		LoadVehicleBuildRecords();
	}
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string RemoteVehicleBuildServer::Register(void)
{
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::Register() - Enter");
	string result = BepServer::Register();
	// Start the file check timer if we are not a producer
	if(!IsInProducerMode())
	{
		m_buildFileTimer.Start();
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::LoadNextVehicleBuildRecord(void)
{
	if(m_vehicleBuildFiles.size() > 0)
	{
		if(m_buildFileListMutex.Acquire())
		{
			FileDataListItr iter = m_vehicleBuildFiles.begin();
			string aonFile = iter->first;
			m_buildFileListMutex.Release();
			int startIndex = GetDataTag("FileNamePrefix").length();
			int numChars = aonFile.find_first_of(GetDataTag("FileNameSuffix")) - startIndex;
			string aon = aonFile.substr(startIndex, numChars);
			INamedDataBroker broker;
			string response;
			broker.Write(GetDataTag("VehicleIdTag"), aon, response, true);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::LoadVehicleBuildRecords(void)
{
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::LoadVehicleBuildrecords() - Enter");
	DIR *buildDir = NULL;
	struct dirent *entry = NULL;
	// Open the build file directory
	if((buildDir = opendir(VehicleBuildFilePath().c_str())) != NULL)
	{
		bool sortRequired = false;
		while((entry = readdir(buildDir)) != NULL)
		{   // Get the file name and creation date
			BuildFileData *buildFile = new BuildFileData;
			buildFile->fileName = entry->d_name;
			struct stat fileStat;
			if(stat(string(VehicleBuildFilePath() + buildFile->fileName).c_str(), &fileStat) != -1)
			{
				buildFile->creationTime = fileStat.st_atime;
			}
			else
			{   // Could not get the file creation time, use the current time
				Log(LOG_ERRORS, "Could not get file creation time, setting time to now");
				buildFile->creationTime = time(NULL);
			}
			if(!S_ISDIR(fileStat.st_mode))
			{   // Make sure the file is not already stored
				if(m_buildFileListMutex.Acquire())
				{
					FileDataListItr iter = m_vehicleBuildFiles.find(buildFile.fileName);
					if(iter != m_vehicleBuildFiles.end())
					{
						Log(LOG_DEV_DATA, "Storing %s created at %s", 
							buildFile->fileName.c_str(), ctime(&buildFile->creationTime));
						m_vehicleBuildFiles.insert(FileDataListVal(buildFile->fileName, buildFile));
						sortRequired = true;
					}
					else
					{
						delete buildFile;
					}
					m_buildFileListMutex.Release();
				}
				else
				{
					delete buildFile;
				}
			}
			else
			{
				delete buildFile;
			}
		}
		closedir(buildDir);
		// Sort the list if anything was added
		if(sortRequired)
		{
			if(m_buildFileListMutex.Acquire())
			{
				sort(m_vehicleBuildFiles.begin(),m_vehicleBuildFiles.end(),Is_File_Older());
				m_buildFileListMutex.Release();
			}
		}
	}
	else
	{
		Log(LOG_ERRORS, "No vehicle build files to load: %s", strerror(errno));
	}
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::LoadVehicleBuildrecords() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string RemoteVehicleBuildServer::Publish(const XmlNode *node)
{
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::Publish(tag: %s) - Enter", node->getName().c_str());
	if(!node->getName().compare(VEHICLE_BUILD_TAG) && IsInProducerMode())
	{   // Save the build data to file
		StoreVehicleBuildRecord(node);
	}
	else if(!node->getName().compare("VehiclePresent") && !atob(node->getValue().c_str() && !IsInProducerMode())
	{   // Remove build file
		RemoveVehicleBuildFile();
		// Command the system to load the next build data
		LoadNextVehicleBuildRecord();
	}
	Log(LOG_FN_ENTRY, "RemoteVehicleBuildServer::Publish(tag: %s) - Exit", node->getName().c_str());
	return BepServer::Publish(node);
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::RemoveVehicleBuildFile()
{   // Get the current vehicle identifier from the system
	string vid = SystemRead(GetDataTag("VehicleIdTag"));
	if(vid.length() > 1)
	{   // Make sure the vehicle ID is still in the list of files
		string idName = GetDataTag("FileNamePrefix") + vid + GetDataTag("FileNameSuffix");
		if(m_buildFileListMutex.Acquire())
		{
			FileDataListItr iter = m_vehicleBuildFiles.find(idName);
			if(idName != m_vehicleBuildFiles.end())
			{   // Need to remove the file from the system and then from the list
				unlink(string(VehicleBuildFilePath() + idName).c_str());
				delete iter->second;   // Delete the file entry so we do not get a memory leak
				m_vehicleBuildFiles.erase(iter);   // Remove the entry from the list
			}
			m_buildFileListMutex.Release();
		}
	}
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::StoreVehicleBuildRecord(const XmlNode *buildData)
{   // Create a file name based on the specified build item
	string fileNameRoot = buildData->getChild(GetDataTag("FileNameRootItem"))->getValue();
	string fileName = VehicleBuildFilePath() + GetDataTag("FileNamePrefix") + fileNameRoot + GetDataTag("FileNameSuffix");
	Log(LOG_DEV_DATA, "Storing vehicle build in %s", fileName.c_str());
	// Open the file for writing
	FILE *buildFile = NULL;
	if((buildFile = fopen(fileName.c_str(), "w")) != NULL)
	{
		fprintf(buildFile, "%s", buildData->ToString().c_str());
		fclose(buildFile);
		Log(LOG_DEV_DATA, "Wrote build data to file");
	}
	else
	{
		Log(LOG_ERRORS, "Could not open %s - %s", fileName.c_str(), strerror(errno));
	}
}

//-------------------------------------------------------------------------------------------------
void RemoteVehicleBuildServer::SetupTimer(BepTimer &timer, UINT64 &timerDelay, UINT8 &timerPulseCode)
{
	timer.SetPulseCode(BUILD_SERVER_PUSLE_CODE);
	timer.SetPulseValue(timerPulseCode);
	timer.Initialize(GetProcessName(), mSEC_nSEC(timerDelay), mSEC_nSEC(timerDelay));
	timer.Stop();
}

//-------------------------------------------------------------------------------------------------
const bool& RemoteVehicleBuildServer::IsInProducerMode(const bool *inProducerMode /*= NULL*/)
{
	if(inProducerMode != NULL)  m_isProducerMode = *inProducerMode;
	return m_isProducerMode;
}

//-------------------------------------------------------------------------------------------------
const string& RemoteVehicleBuildServer::VehicleBuildFilePath(const string *path /*= NULL*/)
{
	if(path != NULL)  m_buildFilePath = *path;
	return m_buildFilePath;
}
