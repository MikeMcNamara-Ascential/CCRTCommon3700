//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterServer.cpp 2     1/16/07 5:27a Derickso $
//
// FILE DESCRIPTION:
//   The StaticTesterServer manages the data for the static tester.  Build
//   data will be retrieved for the static tester when requested.  Test
//   result data will also be saved and stored from the static tester.
//
//===========================================================================
// Copyright (c) 2006- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterServer.cpp $
// 
// 2     1/16/07 5:27a Derickso
// Debug updates from Warren truck install.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     10/19/06 10:37a Derickso
// Plugged memory leak.
// Updated attribute difference resolution to allow fails to be updated to
// pass.
// 
// 1     8/30/06 4:42p Derickso
// New static tester server.
// 
//===========================================================================
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <errno.h>
#include "StaticTesterServer.h"
//===========================================================================


StaticTesterServer::StaticTesterServer() : BepServer(), 
m_plantHostComm(NULL), m_interfaceThreadConfig(NULL)
{   // Nothing special to be done
}

StaticTesterServer::~StaticTesterServer()
{   // Stop the timers
    m_fileStatusTimer.Stop();
    m_commCheckTimer.Stop();
    // Clear all structures
    m_staticTestResults.clear();
    m_buildFiles.clear();
    // Remove all the interface threads
    while(m_staticTesters.size() > 0)
    {
        delete m_staticTesters.back();
    }
    m_staticTesters.clear();
}

void StaticTesterServer::Initialize(const std::string &fileName)
{   // Call the base class to handle this
    BepServer::Initialize(fileName);
}

void StaticTesterServer::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization process
    BepServer::Initialize(document);
    // Get the path to the build file
    string buildFilePath(getenv("USR_ROOT") + string("/"));
    try
    {
        buildFilePath += document->getChild("Setup/Parameters/BuildFilePath")->getValue() + string("/");
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: BuildFilePath -- %s", excpt.GetReason());
        buildFilePath += string("/Configuration/VehicleTest/BuildRecords/");
    }
    // Store the file path
    BuildFilePath(&buildFilePath);
    // Get the path to the result files
    string resultPath(getenv("USR_ROOT") + string("/"));
    try
    {
        resultPath += document->getChild("Setup/Parameters/TestResultPath")->getValue() + string("/");
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: TestResultPath using /Configuration/VehicleTest/TestResults/ -- %s", 
            excpt.GetReason());
        resultPath += string("/Configuration/VehicleTest/TestResults/");
    }
    // Store the file path
    TestResultPath(&resultPath);
    // Store the file life time
    double fileLife = 0.0;
    try
    {
        fileLife = (double)BposReadInt(document->getChild("Setup/Parameters/FileLifeTime")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: FileLifeTime, using 10 minutes -- %s", excpt.GetReason());
        fileLife = 60.0 * 10.0;   // Default to 10 minutes
    }
    FileLifeTime(&fileLife);
    // Store the thread signal times
    INT32 signalTime = 0;
    try
    {
        signalTime = BposReadInt(document->getChild("Setup/Parameters/CommCheckSignalTimeLength")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: CommCheckSignalTimeLength, using 500ms -- %s", excpt.GetReason());
        signalTime = 500;
    }
    CommCheckSignalTime(&signalTime);
    // Store the interface thread config
    try
    {
        m_interfaceThreadConfig = document->getChild("Setup/StaticTesterList")->Copy();
    }
    catch(XmlException &excpt)
    {
        if(m_interfaceThreadConfig != NULL)  delete m_interfaceThreadConfig;
        m_interfaceThreadConfig = NULL;
        Log(LOG_ERRORS, "No interface thread config, interface threads will not be started: %s", excpt.GetReason());
    }
    // Load the existing data files
    LoadDataFiles(BuildFilePath(), m_buildFiles);
    LoadDataFiles(TestResultPath(), m_staticTestResults);
    // Setup the data file manager timer and comm check timer
    SetupTimers(document);
    Log(LOG_FN_ENTRY, "Completed StaticTesterServer::Initialize()");
}

const string StaticTesterServer::Register(void)
{   // Call the base class to start registering
    string result = BepServer::Register();
    // Create a comm object to the plant host server
    if(m_plantHostComm == NULL)
    {
        Log(LOG_DEV_DATA, "Creating comm object to interface with %s", GetDataTag("PlantHostServer").c_str());
        m_plantHostComm = new IBepCommunication();
        m_plantHostComm->Initialize(GetDataTag("PlantHostServer"), "Client", IsDebugOn(), 5000);
        Log(LOG_DEV_DATA, "Initialized comms with %s", GetDataTag("PlantHostServer").c_str());
    }
    // Launch static tester interface threads
    if(m_interfaceThreadConfig != NULL)
    {
        Log(LOG_DEV_DATA, "Starting interface threads for static testers");
        LoadInterfaceThreads(m_interfaceThreadConfig);
        Log(LOG_DEV_DATA, "Done starting interface threads for static testers");
    }
    else
    {
        Log(LOG_ERRORS, "No interface thread config provided, threads not started");
    }
    // Start the timers
    Log(LOG_DEV_DATA, "Starting timers");
    m_fileStatusTimer.Start();
    m_commCheckTimer.Start();
    // Return the registration status
    Log(LOG_FN_ENTRY, "StaticTesterServer::Register() - done: %s", result.c_str());
    return result;
}

const string StaticTesterServer::ReadBuildData(const string &vin, XmlNode &buildData)
{   // Create the request node
    XmlParser parser;
    XmlNode buildRequest(GetDataTag("VinRequest"), vin);
    XmlNode *vehicleBuildData = NULL;
    bool addBuildData = false;
    bool storeBuildData = false;
    string response;
    string status = BEP_ERROR_RESPONSE;
    // Check for local build data first
    if(!CheckForBuildData(vin, vehicleBuildData))
    {   // Request the build data from the plant host server
		Log(LOG_DEV_DATA, "Read build record from plant host NOW: %s", response.c_str());
        if(m_plantHostComm->Read(&buildRequest, response, true) == BEP_STATUS_SUCCESS)
        {   // Ensure the response is valid
            Log(LOG_DEV_DATA, "Read build record from plant host: %s", response.c_str());
            if(response.compare("Invalid"))
            {
                try
                {
                    XmlNode temp("", "");
                    m_plantHostComm->GetNext(temp, response);
                    Log(LOG_DEV_DATA, "Extracted build data from response - %s", temp.ToString().c_str());
                    vehicleBuildData = temp.Copy();
                    Log(LOG_DEV_DATA, "Created copy of build data");
                    vehicleBuildData->setName(GetDataTag("VehicleBuild"));
                    vehicleBuildData->setValue(vin);
                    Log(LOG_DEV_DATA, "Setting flag to add build data");
                    addBuildData = true;
                    storeBuildData = true;
                }
                catch(XmlException &excpt)
                {
                    Log(LOG_ERRORS, "Could not find node: %s - %s", GetDataTag("VinRequest").c_str(), excpt.GetReason());
                    if(vehicleBuildData != NULL)  delete vehicleBuildData;
                    vehicleBuildData = NULL;
                    addBuildData = false;
                }
            }
            else
            {
                Log(LOG_ERRORS, "Could not retrieve build data for %s", vin.c_str());
                status = BEP_FAILURE_RESPONSE;
            }
        }
        else
        {
            Log(LOG_ERRORS, "Could not read build record for %s", vin.c_str());
            status = BEP_FAILURE_RESPONSE;
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Retrieved local build data for static tester");
        addBuildData = true;
        status = BEP_SUCCESS_RESPONSE;
    }
    // Add the build data if it was retrieved or found locally
    if(addBuildData && (vehicleBuildData != NULL))
    {   // Add the data to the node
        buildData.setName("Vin"+vin);
        buildData.addChild(vehicleBuildData->Copy());
        Log(LOG_DEV_DATA, "Created build data node: %s", buildData.ToString().c_str());
        // Store the build data
        if(storeBuildData)
        {
			Log(LOG_DEV_DATA, "DEBUG: Add build data\n");
            StoreTesterData(BuildFilePath(), vin, &buildData, m_buildFiles, false);
            Log(LOG_DEV_DATA, "Stored the tester data information");
        }
        status = BEP_SUCCESS_RESPONSE;
    }
    else
    {
        Log(LOG_ERRORS, "Build Data not added - addBuildData: %s,  vehicleBuildData NULL-%s %s", 
            addBuildData ? "True" : "False", 
            vehicleBuildData == NULL ? "True" : "False", 
            vehicleBuildData == NULL ? "NULL" : vehicleBuildData->ToString().c_str());
        status = BEP_ERROR_RESPONSE;
    }
    // Return the status
    if(vehicleBuildData != NULL)  delete vehicleBuildData;
    vehicleBuildData = NULL;
    return status;
}

const string StaticTesterServer::Read(const XmlNode *node, const INT32 rate) /*= 0*/
{
    string result = BEP_ERROR_RESPONSE;
    Log(LOG_FN_ENTRY, "StaticTesterServer::Read(%s) - Enter", node->getName().c_str());
    // Check the type of data being requested
    if(!node->getName().compare(GetDataTag("BuildData")))
    {   // Retrieve build data
        result = CheckForDataFile(node->getValue(), m_buildFiles) ? (BuildFilePath() + node->getValue()) : BEP_UNAVAILABLE_RESPONSE;
    }
    else if(!node->getName().compare(GetDataTag("TestResult")))
    {   // retrieve test result data
        result = CheckForDataFile(node->getValue(), m_staticTestResults) ? (TestResultPath() + node->getValue()) : BEP_UNAVAILABLE_RESPONSE;
    }
    else
    {   // Let the base class handle the request
        result = BepServer::Read(node, rate);
    }
    Log(LOG_FN_ENTRY, "StaticTesterServer::Read(%s:%s) returning %s", 
        node->getName().c_str(), node->getValue().c_str(), result.c_str());
    return result;
}

const string StaticTesterServer::Write(const XmlNode *node)
{
    string status = BEP_ERROR_RESPONSE;
    // Determine if this node should be handled as a test result
    if(!node->getName().compare(GetDataTag("TestResult")))
    {   // Process the test result node and add to managed data file
		Log(LOG_DEV_DATA, "DEBUG: Store test result\n");
        StoreTesterData(TestResultPath(), node->getValue(), node, m_staticTestResults, true);
        status = BEP_SUCCESS_RESPONSE;
    }
    else
    {   // Process the data normally
        status = BepServer::Write(node);
    }
    return status;
}

const INT32 StaticTesterServer::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 result = BEP_STATUS_FAILURE;
    Log(LOG_DETAILED_DATA, "Enter StaticTesterServer::HandlePulse(pulse code: %d, value: %d)", code, value);
    // Handle the pulse code
    switch(code)
    {
    case STATIC_TESTER_SERVER_PULSE_CODE:
        switch(value)
        {
        case DATA_FILE_PULSE:
            Log(LOG_DETAILED_DATA, "Managing static tester build data files...");
            ManageDataFiles(BuildFilePath(), m_buildFiles);
            Log(LOG_DETAILED_DATA, "Managing static tester test result data files...");
            ManageDataFiles(TestResultPath(), m_staticTestResults);
            break;

        case COMM_TIMER_PULSE:
            m_checkComms.Broadcast(startCommCheck);
            BposSleep(CommCheckSignalTime());
            m_checkComms.Broadcast(stopCommCheck);
            break;

        default:
            Log(LOG_ERRORS, "Unknown pulse value: %d.  Ignoring pulse!", value);
            break;
        }
        break;

    default:
        result = BepServer::HandlePulse(code, value);
        break;
    }
    Log(LOG_DETAILED_DATA, "Exit StaticTesterServer::HandlePulse(pulse code: %d, value: %d)", code, value);
    return result;
}

bool StaticTesterServer::CheckForBuildData(const string &vin, XmlNode* &buildData)
{
    FileDataListItr iter = m_buildFiles.find(vin);
    bool buildDataFound = (iter != m_buildFiles.end());
    if(buildDataFound)
    {   // Found local build data for vin
        XmlParser parser;
        if(buildData != NULL)  delete buildData;
        buildData = NULL;
        try
        {
            Log(LOG_DEV_DATA, "Loading local build data for %s from %s%s", 
                vin.c_str(), BuildFilePath().c_str(), iter->second.fileName.c_str());
            const XmlNode *loadedBuild = parser.ReturnXMLDocument(BuildFilePath() + iter->second.fileName);
            buildData = loadedBuild->getChild(GetDataTag("VehicleBuild"))->Copy();
            if(buildData != NULL)
            {
                Log(LOG_DEV_DATA, "Loaded local build data: %s", buildData->ToString().c_str());
            }
            else
            {
                Log(LOG_ERRORS, "Local build data not loaded, document is NULL");
            }
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not retrieve build record for %s: %s", vin.c_str(), excpt.GetReason());
            if(buildData != NULL)  delete buildData;
            buildData = NULL;
            buildDataFound = false;
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "Did not find local build record for %s", vin.c_str());
    }
    return buildDataFound;
}

inline const string StaticTesterServer::GetDataTag(const string dataTag)
{
    return BepServer::GetDataTag(dataTag);
}

const string& StaticTesterServer::TestResultPath(string *path) /*= NULL*/
{
    if(path != NULL)  m_testResultPath = *path;
    return m_testResultPath;
}

const string& StaticTesterServer::BuildFilePath(string *path) /*= NULL*/
{
    if(path != NULL)  m_buildFilePath = *path;
    return m_buildFilePath;
}

const double& StaticTesterServer::FileLifeTime(double *lifeTime) /*= NULL*/
{
    if(lifeTime != NULL)  m_fileLifetime = *lifeTime;
    return m_fileLifetime;
}

const INT32& StaticTesterServer::CommCheckSignalTime(INT32 *signalTime) /*= NULL*/
{
    if(signalTime != NULL)  m_commCheckSignalTime = *signalTime;
    return m_commCheckSignalTime;
}

inline const bool StaticTesterServer::CheckForDataFile(const string &fileName, FileDataList &fileList)
{
    return(fileList.find(fileName) != fileList.end());
}

void StaticTesterServer::LoadDataFiles(const string &path, FileDataList &fileList)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    // Open the directory requested
    Log(LOG_DEV_DATA, "Processing directory: %s", path.c_str());
    if((dir = opendir(path.c_str())) != NULL)
    {   // Directory has been opened, read entries
        FileData fileData;
        while((entry = readdir(dir)) != NULL)
        {   // Get the file name and creation date
            fileData.fileName = entry->d_name;
            int fileDes = open((path + fileData.fileName).c_str(), O_RDONLY);
            struct stat data;
            if(fstat(fileDes, &data) != -1)
            {
                fileData.creationTime = data.st_atime;
            }
            else
            {   // Could not get file creation time, set creation time to now
                Log(LOG_ERRORS, "Could not get file creation time for %s, using current time!", fileData.fileName.c_str());
                fileData.creationTime = time(NULL);
            }
            // Only attempt to add if this is not a directory
            if(!S_ISDIR(data.st_mode))
            {
                AddDataFileToList(path, fileList, fileData);
            }
            else
            {
                Log(LOG_DEV_DATA, "File %s is a directory, not processing", fileData.fileName.c_str());
            }
        }
        // Close the directory
        closedir(dir);
    }
    else
    {   // Could not open directory
        Log(LOG_ERRORS, "Could not open directory: %s - %s", path.c_str(), strerror(errno));
    }
}

void StaticTesterServer::ManageDataFiles(const string &path, FileDataList &fileList)
{   // Get the current time
    time_t currentTime = time(NULL);
    // Check the time for each file in the list
    if(fileList.size() > 0)
    {   // Files in the list, manage these files
        for(FileDataListItr iter = fileList.begin(); iter != fileList.end(); iter++)
        {
            if(difftime(currentTime, iter->second.creationTime) > FileLifeTime())
            {   // File is too old, remove from system
                Log(LOG_DEV_DATA, "File %s is too old (>%6.2f seconds), removing.", 
                    iter->second.fileName.c_str(), FileLifeTime());
                if(0 != unlink((path + iter->second.fileName).c_str()))
                {   // Error removing the file
                    Log(LOG_ERRORS, "Could not remove file %s - %s", iter->second.fileName.c_str(), strerror(errno));
                }
                else
                {   // File removed from file system, remove from managed list
                    Log(LOG_DEV_DATA, "Removing file %s created on %s", iter->second.fileName.c_str(), ctime(&iter->second.creationTime));
                    fileList.erase(iter);
                }
            }
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "No files currently in %s to manage", path.c_str());
    }
}

void StaticTesterServer::StoreTesterData(const string &path, const string &vin, const XmlNode *data, 
                                         FileDataList &fileList, const bool &appendToFile)
{
    FILE *dataFile;
    string fileName(path + vin);
    XmlNode *dataToStore = NULL;
    bool createFileDataEntry = true;
    // Check if the data must be appended
    if(appendToFile)
	{	
		Log(LOG_DEV_DATA, "Before AppendDataToFile(\n");
		// Append the new data to the existing data
        AppendDataToFile(path, vin, data, dataToStore);
        // Update the managed data file info
        FileDataListItr iter = fileList.find(vin);
        if(iter != fileList.end())
        {   // Update file time for existing file
            iter->second.creationTime = time(NULL);
            createFileDataEntry = false;
        }
    }
    else
    {   // New data list
		Log(LOG_DEV_DATA, "First node, copy xml node\n");
        dataToStore = data->Copy();
    }
    // Ensure we actually have good data to store
    if(dataToStore != NULL)
    {
        // Attempt to open the file
        if((dataFile = fopen(fileName.c_str(), "w+")) != NULL)
        {   // Write the data to a file
            rewind(dataFile);
            fprintf(dataFile, "%s", dataToStore->ToString().c_str());
            fclose(dataFile);
            Log(LOG_DEV_DATA, "Wrote test result data to file: %s", dataToStore->ToString().c_str());
            // Create the file data entry if it does not exist
            if(createFileDataEntry)
            {   // Add the data file to the managed list
                FileData file;
                file.fileName = vin;
                file.creationTime = time(NULL);
                AddDataFileToList(path, fileList, file);
            }
        }
        else
        {   // Could not open the file
            Log(LOG_ERRORS, "Could not open %s for storing data: %s", fileName.c_str(), strerror(errno));
        }
        // Get rid of the data we created
        try
        {
			Log(LOG_DEV_DATA, "Deleting test result data %s", dataToStore->ToString().c_str());
			//if(dataToStore != NULL)	delete dataToStore;
			if(dataToStore != NULL)
			{
				dataToStore->clear();
				delete dataToStore;
			}
			Log(LOG_DEV_DATA, "Set dataToStore to NULL\n");
	        dataToStore = NULL;
	        Log(LOG_DEV_DATA, "Deleted temp data.  Already written to file");
        }
		catch(XmlException &excpt)
		{
			Log(LOG_DEV_DATA, "XmlException while deleting data already written to file: %s", excpt.GetReason());
		}
		catch(BepException &excpt)
		{
			Log(LOG_DEV_DATA, "BepException while deleting data already written to file: %s", excpt.GetReason());
		}
		catch(...)
		{
			Log(LOG_DEV_DATA, "Unknown Exception while deleting data already written to file");
		}
	}
    else
    {
        Log(LOG_ERRORS, "No data to store - data is NULL!");
    }
}

void StaticTesterServer::AddDataFileToList(const string &path, FileDataList &fileList, FileData &fileEntry)
{   // Get the current time
    time_t currentTime = time(NULL);
    if(difftime(currentTime, fileEntry.creationTime) > FileLifeTime())
    {   // File is too old, remove from the system
        Log(LOG_DEV_DATA, "File %s is too old (>%6.2f seconds), removing.", fileEntry.fileName.c_str(), FileLifeTime());
        if(0 != unlink((path + fileEntry.fileName).c_str()))
        {
            Log(LOG_ERRORS, "Could not remove file %s - %s", fileEntry.fileName.c_str(), strerror(errno));
        }
    }
    else
    {   // Store the data
        Log(LOG_DEV_DATA, "Storing %s created at %s", fileEntry.fileName.c_str(), ctime(&fileEntry.creationTime));
        fileList.insert(FileDataListVal(fileEntry.fileName, fileEntry));
    }
}

void StaticTesterServer::AppendDataToFile(const string &path, 
                                          const string &vin, 
                                          const XmlNode *dataToAppend, 
										  XmlNode* &fileData)
{
    XmlParser parser;
    XmlNodeMap newResults;
    bool processData = (dataToAppend != NULL);

    if(processData)
    {
        try
        {
            newResults.DeepCopy(dataToAppend->getChildren());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "No data to append to file!");
            processData = false;
        }
        // Ensure we still have good data
        if(processData)
        {
            // Read in the existing data if available
            if(fileData != NULL)
            {
                fileData->clear();
                delete fileData;
            }
            try
            {
                fileData = parser.ReturnXMLDocument(path + vin)->Copy();
                Log(LOG_DEV_DATA, "Loaded existing test results: %s", fileData->ToString().c_str());
            }
            catch(XmlException &excpt)
            {
                Log(LOG_ERRORS, "XmlExcpetion - Could not retrieve result file for %s: %s", vin.c_str(), excpt.GetReason()); 
                fileData = NULL;
            }
            catch(BepException &excpt)
            {
                Log(LOG_ERRORS, "BepException - Could not retrieve result file for %s: %s", vin.c_str(), excpt.GetReason()); 
                fileData = NULL;
            }
            // Process the data according to what we were able to retrieve from the system
            if(fileData != NULL)
            {
                const XmlNodeMap *currentResults = &fileData->getChildren();
                // Walk through the new results to see if they are already in the existing results
                for(XmlNodeMapItr iter = newResults.begin(); iter != newResults.end(); iter++)
                {
                    XmlNodeMapCItr currentResultIter = currentResults->find(iter->second->getName());
                    if(currentResultIter != currentResults->end())
                    {   // Need to update existing test result
                    	Log(LOG_DEV_DATA, "Updating existing result: %s with new result: %s", 
							currentResultIter->second->ToString().c_str(), iter->second->ToString().c_str());
                        UpdateNodeData(iter->second, currentResultIter->second);
                    }
                    else
                    {   // New test result, just add
                        Log(LOG_DEV_DATA, "Adding new test result: %s", iter->second->ToString().c_str());
                        XmlNodeMap *resultUpdate = const_cast<XmlNodeMap *>(currentResults);
                        resultUpdate->addNode(iter->second->Copy());
                    }
                }
            }
            else
            {
                Log(LOG_ERRORS, "No existing test result data to append to, just writing the data to file");
				//fileData = const_cast<XmlNode *>(dataToAppend);
				fileData = dataToAppend->Copy();
			}
		}
        else
        {
            Log(LOG_ERRORS, "Test result record is not in correct format!");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Test result data is empty!");
    }
    // Clear up the node maps that have been populated
    newResults.clear(true);
}

void StaticTesterServer::UpdateNodeData(XmlNode *newData, XmlNode *existingData)
{   // Check attributes first
    const XmlNodeMap *newAttributes = &newData->getAttributes();
    const XmlNodeMap *existingAttributes = &existingData->getAttributes();
    for(XmlNodeMapCItr newAttributeIter = newAttributes->begin(); 
       newAttributeIter != newAttributes->end(); 
       newAttributeIter++)
    {
        XmlNodeMapCItr existingIter = existingAttributes->find(newAttributeIter->second->getName());
        if(existingIter != existingAttributes->end())
        {   // Found a matching attribute, update the value
            ResolveNodeDiffs(newAttributeIter->second, existingIter->second);
        }
        else
        {   // Attribute has not been entered yet, just add it to the existing data
            Log(LOG_DEV_DATA, "Adding new attribute to %s: %s", 
                existingData->getName().c_str(), newAttributeIter->second->ToString().c_str());
            existingData->addChild(newAttributeIter->second->Copy());
        }
    }
    // Process any child nodes
    const XmlNodeMap *newChildNodes = &newData->getChildren();
    const XmlNodeMap *existingChildNodes = &existingData->getChildren();
    for(XmlNodeMapCItr newChildNodeIter = newChildNodes->begin();
       newChildNodeIter != newChildNodes->end();
       newChildNodeIter++)
    {
        XmlNodeMapCItr existingChildNodeIter = existingChildNodes->find(newChildNodeIter->second->getName());
        if(existingChildNodeIter != existingChildNodes->end())
        {   // Found an existing child node, update the value
            ResolveNodeDiffs(newChildNodeIter->second, existingChildNodeIter->second);
        }
        else
        {   // Child node not found, just add to existing node
            Log(LOG_DEV_DATA, "Adding new child to %s: %s", 
                existingData->getName().c_str(), newChildNodeIter->second->ToString().c_str());
            existingData->addChild(newChildNodeIter->second->Copy());
        }
    }
}

void StaticTesterServer::ResolveNodeDiffs(XmlNode *newNode, XmlNode *existingNode)
{
    string attributeValue(newNode->getValue());
    bool updateAttributeValue = false;
/* Initial design logic would allow for static tester to use the same test result name to run 
 * multiple tests.  If the there were passes followed by fails followed by more passes, the
 * test result value should not be updated from fail to pass.  
 * That is why the logic below looks wierd.  
 * Upon review and the possibility of re-running a test, we would want to update fails with
 * passes, or the vehicle would never be able to get through.  It has been decided to always
 * update the result attribute for each test step and the static tester will send unique
 * test result names for each test.
 */
#if 0  
    if(!newNode->getName().compare(BEP_RESULT))
    {   // This is a result, handle it properly
        if(!existingNode->getValue().compare(BEP_PASS_RESPONSE))
        {   // Currently set to pass, update with current value
            updateAttributeValue = true;
        }
    }
    else
    {   // Just update the value if it is different
        if(existingNode->getValue().compare(attributeValue))
        {   // New value is different, update it
            updateAttributeValue = true;
        }
    }
#else
	updateAttributeValue = true;
#endif
    // Update the value if required
    if(updateAttributeValue)
    {
        Log(LOG_DEV_DATA, "Updating attribute %s value %s to %s", 
            existingNode->getName().c_str(), existingNode->getValue().c_str(), attributeValue.c_str());
        existingNode->setValue(attributeValue);
    }
}

void StaticTesterServer::SetupTimers(const XmlNode *config)
{   // Setup the timer for managing the data files
    UINT64 fileTimer;   // Timer pulse
    try
    {
        fileTimer = atol(config->getChild("Setup/Parameters/DataFileTimer")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DataFileTimer not specified - using 5seconds: %s", excpt.GetReason());
        fileTimer = 5000;
    }
    // Ensure a sane timer value
    fileTimer = (fileTimer > 10) ? fileTimer : 10;
    Log(LOG_DEV_DATA, "Setting file timer to %d ms", fileTimer);
    // Setup the timer
    m_fileStatusTimer.SetPulseCode(STATIC_TESTER_SERVER_PULSE_CODE);
    Log(LOG_DEV_DATA, "Set data file timer pulse code to %d", STATIC_TESTER_SERVER_PULSE_CODE);
    m_fileStatusTimer.SetPulseValue(DATA_FILE_PULSE);
    Log(LOG_DEV_DATA, "Set data file timer pulse value to %d", DATA_FILE_PULSE);
    m_fileStatusTimer.Initialize(GetProcessName().c_str(), (mSEC_nSEC(fileTimer)), (mSEC_nSEC(fileTimer)));
    m_fileStatusTimer.Stop();
    // Setup the comm check timer
    UINT64 commTimer;
    try
    {
        commTimer = atol(config->getChild("Setup/Parameters/CommCheckTimer")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        {
            Log(LOG_ERRORS, "CommCheckTimer not specified - using 500ms: %s", excpt.GetReason());
            commTimer = 500;
        }
    }
    // Ensure a sane timer setting
    commTimer = (commTimer > 5) ? commTimer : 5;
    Log(LOG_DEV_DATA, "Setting comm timer to %d ms", commTimer);
    // Setup the timer
    m_commCheckTimer.SetPulseCode(STATIC_TESTER_SERVER_PULSE_CODE);
    Log(LOG_DEV_DATA, "Set comm check timer pulse code to %d", STATIC_TESTER_SERVER_PULSE_CODE);
    m_commCheckTimer.SetPulseValue(COMM_TIMER_PULSE);
    Log(LOG_DEV_DATA, "Set comm check timer pulse value to %d", COMM_TIMER_PULSE);
    m_commCheckTimer.Initialize(GetProcessName().c_str(), (mSEC_nSEC(commTimer)), (mSEC_nSEC(commTimer)));
    m_commCheckTimer.Stop();
    // Done setting up timers
    Log(LOG_FN_ENTRY, "Timer setup complete");
}

void StaticTesterServer::LoadInterfaceThreads(const XmlNode *config)
{
    const XmlNodeMap *threadConfigs = &config->getChildren();
    for(XmlNodeMapCItr iter = threadConfigs->begin(); iter != threadConfigs->end(); iter++)
    {
        if(iter->second->getAttribute("enabled")->getValue() == "1")
        {   // Start a new interface thread
            Log(LOG_DEV_DATA, "Creating static tester interface %s", iter->second->getName().c_str());
            m_staticTesters.push_back(new StaticTesterInterface(this));
            // Initialize and start the interface
            Log(LOG_DEV_DATA, "Initialing static tester interface %s", iter->second->getName().c_str());
            m_staticTesters.back()->Initialize(iter->second);
            Log(LOG_DEV_DATA, "Running static tester interface %s", iter->second->getName().c_str());
            m_staticTesters.back()->Resume();
        }
    }
}
