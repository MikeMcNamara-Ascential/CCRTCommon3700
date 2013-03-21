//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommServer.cpp 2     1/16/07 5:27a Derickso $
//
// FILE DESCRIPTION:
//   The InterCcrtCommServer manages the data for the ccrt comm endpoint.  
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
//===========================================================================
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>
#include "InterCcrtCommServer.h"
//===========================================================================


InterCcrtCommServer::InterCcrtCommServer() : BepServer(), 
m_plantHostComm(NULL), m_interfaceThreadConfig(NULL),m_faultServer( NULL),
m_faultInformation(NULL)
{   // Nothing special to be done
}

InterCcrtCommServer::~InterCcrtCommServer()
{   // Clear all structures
    m_interCcrtCommResults.clear();
    m_buildFiles.clear();
    // Remove all the interface threads
    while (m_interCcrtComms.size() > 0)
    {
        delete m_interCcrtComms.back();
    }
    m_interCcrtComms.clear();
    // if the IFaultServer exists, delete it
    if (m_faultServer != NULL)   delete m_faultServer;
    if (m_plantHostComm != NULL) delete m_plantHostComm;
    // if the fault information list exists, delete it
    if (m_faultInformation)      delete m_faultInformation;
}

void InterCcrtCommServer::Initialize(const std::string &fileName)
{   // Call the base class to handle this
    BepServer::Initialize(fileName);
}

void InterCcrtCommServer::CreateDirectory(string path)
{
    string command("mkdir -m 777 " + path);

    //check if path exists

    if (system(command.c_str()) != -1)
    {
        Log(LOG_DEV_DATA, "Successfully created path: %s", path.c_str());
    }
    else
    {
        Log(LOG_ERRORS, "Path already exists %s", path.c_str());
    }
}

void InterCcrtCommServer::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization process
    BepServer::Initialize(document);


    // get the fault information
    try
    {
        m_faultInformation = const_cast< XmlNode *> (document->getChild( XML_T("Setup"))->getChild( XML_T("SystemFaults"))->Copy());
    }
    catch (XmlException &err)
    {   // If no hysterisis specified, default to 0 seconds
        throw BepException("No SystemFaults node under Setup in the configuration file!\n");
    }

    // Get the path to the build file
    string buildFilePath(getenv("USR_ROOT") + string("/"));
    try
    {
        buildFilePath += document->getChild("Setup/Parameters/BuildFilePath")->getValue() + string("/");
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: BuildFilePath -- %s", excpt.GetReason());
        buildFilePath += string("/Configuration/VehicleTest/BuildRecords/");
    }
    // Store the file path
    BuildFilePath(&buildFilePath);
    //If path does not exist, create
    CreateDirectory(buildFilePath);

    // Get the path to the result files
    string resultPath(getenv("USR_ROOT") + string("/"));
    try
    {
        resultPath += document->getChild("Setup/Parameters/TestResultPath")->getValue() + string("/");
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not find parameter: TestResultPath using /Configuration/VehicleTest/TestResults/ -- %s", 
            excpt.GetReason());
        resultPath += string("/TestResults/Ccts");
    }
    // Store the file path
    TestResultPath(&resultPath);
    //If path does not exist, create
    CreateDirectory(resultPath);



    // Store the interface thread config
    try
    {
        m_interfaceThreadConfig = document->getChild("Setup/InterCcrtCommList")->Copy();
    }
    catch (XmlException &excpt)
    {
        if (m_interfaceThreadConfig != NULL)  delete m_interfaceThreadConfig;
        m_interfaceThreadConfig = NULL;
        Log(LOG_ERRORS, "No interface thread config, interface threads will not be started: %s", excpt.GetReason());
    }
    // Load the existing data files
    LoadDataFiles(BuildFilePath(), m_buildFiles);
    LoadDataFiles(TestResultPath(), m_interCcrtCommResults);
    Log(LOG_FN_ENTRY, "Completed InterCcrtCommServer::Initialize()");
}

const string InterCcrtCommServer::Register(void)
{   // Call the base class to start registering
    string result = BepServer::Register();   
    string retVal( BEP_SUCCESS_RESPONSE);
    // Create a comm object to the plant host server
    if (m_plantHostComm == NULL)
    {
        Log(LOG_DEV_DATA, "Creating comm object to interface with %s", GetDataTag("PlantHostServer").c_str());
        m_plantHostComm = new IBepCommunication();
        m_plantHostComm->Initialize(GetDataTag("PlantHostServer"), "Client", IsDebugOn(), 5000);
        Log(LOG_DEV_DATA, "Initialized comms with %s", GetDataTag("PlantHostServer").c_str());
    }
    // Launch ccrt comm interface threads
    if (m_interfaceThreadConfig != NULL)
    {
        Log(LOG_DEV_DATA, "Starting interface threads for inter Ccrt Comms");
        LoadInterfaceThreads(m_interfaceThreadConfig);
        Log(LOG_DEV_DATA, "Done starting interface threads for inter Ccrt Comms");
    }
    else
    {
        Log(LOG_ERRORS, "No interface thread config provided, threads not started");
    }

    // create a fault server interface
    if (m_faultServer != NULL)
    {
        delete m_faultServer;
        m_faultServer = NULL;
    }
    m_faultServer = new IFaultServer;
    Log( LOG_DEV_DATA, "m_faultServer::Initialize()\n");
    m_faultServer->Initialize( FAULT_SERVER_NAME);
    // subscribe for all faults
    m_faultServer->SubscribeSeverityLevelFaults( 0, GetProcessName(), retVal, true);

    // Return the registration status
    Log(LOG_FN_ENTRY, "InterCcrtCommServer::Register() - done: %s", result.c_str());
    return result;
}

const string InterCcrtCommServer::ReadBuildData(const string &vin, XmlNode &buildData)
{   // Create the request node
    XmlParser parser;
    XmlNode buildRequest(GetDataTag("VinRequest"), vin);
    XmlNode *vehicleBuildData = NULL;
    bool addBuildData = false;
    bool storeBuildData = false;
    string response;
    string status = BEP_ERROR_RESPONSE;
    // Check for local build data first
    if (!CheckForBuildData(vin, vehicleBuildData))
    {   // Request the build data from the plant host server
        Log(LOG_DEV_DATA, "Read build record from plant host NOW: %s", response.c_str());
        if (m_plantHostComm->Read(&buildRequest, response, true) == BEP_STATUS_SUCCESS)
        {   // Ensure the response is valid
            Log(LOG_DEV_DATA, "Read build record from plant host: %s", response.c_str());
            if (response.compare("Invalid"))
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
                catch (XmlException &excpt)
                {
                    Log(LOG_ERRORS, "Could not find node: %s - %s", GetDataTag("VinRequest").c_str(), excpt.GetReason());
                    if (vehicleBuildData != NULL)  delete vehicleBuildData;
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
        Log(LOG_DEV_DATA, "Retrieved local build data for inter Ccrt Comm");
        addBuildData = true;
        status = BEP_SUCCESS_RESPONSE;
    }
    // Add the build data if it was retrieved or found locally
    if (addBuildData && (vehicleBuildData != NULL))
    {   // Add the data to the node
        //buildData.setName("Vin"+vin);
        buildData.setName(vin);
        buildData.addChild(vehicleBuildData->Copy());
        Log(LOG_DEV_DATA, "Created build data node: %s", buildData.ToString().c_str());
        // Store the build data
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
    if (vehicleBuildData != NULL)  delete vehicleBuildData;
    vehicleBuildData = NULL;
    return status;
}

const string InterCcrtCommServer::Read(const XmlNode *node, const INT32 rate) /*= 0*/
{
    string result = BEP_ERROR_RESPONSE;
    Log(LOG_FN_ENTRY, "InterCcrtCommServer::Read(%s) - Enter", node->getName().c_str());
    // Check the type of data being requested
    if (!node->getName().compare(GetDataTag("BuildData")))
    {   // Retrieve build data
        result = CheckForDataFile(node->getValue(), m_buildFiles) ? (BuildFilePath() + node->getValue()) : BEP_UNAVAILABLE_RESPONSE;
    }
    else if (!node->getName().compare(GetDataTag("TestResult")))
    {   // retrieve test result data
        Log(LOG_DETAILED_DATA, "Checking for test result %s",node->getValue().c_str());
        result = CheckForDataFile(node->getValue(), m_interCcrtCommResults) ? (TestResultPath() + node->getValue()) : BEP_UNAVAILABLE_RESPONSE;
        if (result == BEP_UNAVAILABLE_RESPONSE)
        {//test result not found, request from endpoint
            Log(LOG_DETAILED_DATA, "Result not present requesting from endpoint");
            for (int i = 0; i < m_interCcrtComms.size(); i++)
            {  
                if (m_interCcrtComms[i]->IsResultConnection())
                {
                    if (m_interCcrtComms[i]->SendTestResultRequest(node->getValue()) == BEP_STATUS_SUCCESS)
                    {
                        int sleepTime = 200;
                        int timeout = 5000;
                        int timeCount = timeout / sleepTime;
                        //wait for file
                        while ((timeCount--) && !CheckForDataFile(node->getValue(), m_interCcrtCommResults) && GetStatus() != BEP_TERMINATE)
                        {
                            BposSleep(sleepTime);
                        }
                        result = (timeCount > 0) ? (TestResultPath() + node->getValue()) : BEP_UNAVAILABLE_RESPONSE;
                    }
                }
            }
        }
        //if test result obtained remove 
        if (result != BEP_UNAVAILABLE_RESPONSE)
        {

            Log(LOG_DEV_DATA, "File %s retrieved by system, removing.", 
                node->getValue().c_str());
            if (0 != unlink((TestResultPath() + node->getValue()).c_str()))
            {   // Error removing the file
                Log(LOG_ERRORS, "Could not remove file %s - %s", node->getValue().c_str(), strerror(errno));
            }
            else
            {   // File removed from file system, remove from managed list
                FileDataListItr iter = m_interCcrtCommResults.find(node->getValue());
                Log(LOG_DEV_DATA, "Removing file %s created on %s", iter->second.fileName.c_str(), ctime(&iter->second.creationTime));
                m_interCcrtCommResults.erase(iter);
            }
        }
    }
    else
    {   // Let the base class handle the request
        result = BepServer::Read(node, rate);
    }
    Log(LOG_FN_ENTRY, "InterCcrtCommServer::Read(%s:%s) returning %s", 
        node->getName().c_str(), node->getValue().c_str(), result.c_str());
    return result;
}

const string InterCcrtCommServer::Write(const XmlNode *node)
{
    string status = BEP_ERROR_RESPONSE;
    // Determine if this node should be handled as a test result
    if (!node->getName().compare(GetDataTag("TestResult")))
    {   // Process the test result node and add to managed data file
        Log(LOG_DEV_DATA, "DEBUG: Store test result\n");
        StoreTesterData(TestResultPath(), node->getValue(), node, m_interCcrtCommResults, true);
        status = BEP_SUCCESS_RESPONSE;
    }
    else
    {   // Process the data normally
        status = BepServer::Write(node);
    }
    return status;
}

const INT32 InterCcrtCommServer::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 result = BEP_STATUS_FAILURE;
    Log(LOG_DETAILED_DATA, "Enter InterCcrtCommServer::HandlePulse(pulse code: %d, value: %d)", code, value);
    // Handle the pulse code
    switch (code)
    {
    case INTER_CCRT_COMM_SERVER_PULSE_CODE:
        switch (value)
        {
        case KEEPALIVE_PULSE:
            for (int i = 0; i < m_interCcrtComms.size(); i++)
            {
                m_interCcrtComms[i]->SendKeepAlive();
            }
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
    Log(LOG_DETAILED_DATA, "Exit InterCcrtCommServer::HandlePulse(pulse code: %d, value: %d)", code, value);
    return result;
}

bool InterCcrtCommServer::CheckForBuildData(const string &vin, XmlNode* &buildData)
{
    FileDataListItr iter = m_buildFiles.find(vin);
    bool buildDataFound = (iter != m_buildFiles.end());
    if (buildDataFound)
    {   // Found local build data for vin
        XmlParser parser;
        if (buildData != NULL)  delete buildData;
        buildData = NULL;
        try
        {
            Log(LOG_DEV_DATA, "Loading local build data for %s from %s%s", 
                vin.c_str(), BuildFilePath().c_str(), iter->second.fileName.c_str());
            const XmlNode *loadedBuild = parser.ReturnXMLDocument(BuildFilePath() + iter->second.fileName);
            buildData = loadedBuild->getChild(GetDataTag("VehicleBuild"))->Copy();
            if (buildData != NULL)
            {
                Log(LOG_DEV_DATA, "Loaded local build data: %s", buildData->ToString().c_str());
            }
            else
            {
                Log(LOG_ERRORS, "Local build data not loaded, document is NULL");
            }
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not retrieve build record for %s: %s", vin.c_str(), excpt.GetReason());
            if (buildData != NULL)  delete buildData;
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

inline const string InterCcrtCommServer::GetDataTag(const string dataTag)
{
    return BepServer::GetDataTag(dataTag);
}

const string& InterCcrtCommServer::TestResultPath(string *path) /*= NULL*/
{
    if (path != NULL)  m_testResultPath = *path;
    return m_testResultPath;
}

const string& InterCcrtCommServer::BuildFilePath(string *path) /*= NULL*/
{
    if (path != NULL)  m_buildFilePath = *path;
    return m_buildFilePath;
}

inline const bool InterCcrtCommServer::CheckForDataFile(const string &fileName, FileDataList &fileList)
{
    return(fileList.find(fileName) != fileList.end());
}

void InterCcrtCommServer::LoadDataFiles(const string &path, FileDataList &fileList)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    // Open the directory requested
    Log(LOG_DEV_DATA, "Processing directory: %s", path.c_str());
    if ((dir = opendir(path.c_str())) != NULL)
    {   // Directory has been opened, read entries
        FileData fileData;
        while ((entry = readdir(dir)) != NULL)
        {   // Get the file name and creation date
            fileData.fileName = entry->d_name;
            int fileDes = open((path + fileData.fileName).c_str(), O_RDONLY);
            struct stat data;
            if (fstat(fileDes, &data) != -1)
            {
                fileData.creationTime = data.st_atime;
            }
            else
            {   // Could not get file creation time, set creation time to now
                Log(LOG_ERRORS, "Could not get file creation time for %s, using current time!", fileData.fileName.c_str());
                fileData.creationTime = time(NULL);
            }
            // Only attempt to add if this is not a directory
            if (!S_ISDIR(data.st_mode))
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


void InterCcrtCommServer::StoreTesterData(const string &path, const string &vin, const XmlNode *data, 
                                          FileDataList &fileList, const bool &appendToFile)
{
    FILE *dataFile;
    string fileName(path + vin);
    XmlNode *dataToStore = NULL;
    bool createFileDataEntry = true;
    // Check if the data must be appended
    if (appendToFile)
    {        // Append the new data to the existing data
        AppendDataToFile(path, vin, data, dataToStore);
        // Update the managed data file info
        FileDataListItr iter = fileList.find(vin);
        if (iter != fileList.end())
        {   // Update file time for existing file
            iter->second.creationTime = time(NULL);
            createFileDataEntry = false;
        }
    }
    else
    {   // New data list
        dataToStore = data->Copy();
    }
    // Ensure we actually have good data to store
    if (dataToStore != NULL)
    {
        // Attempt to open the file
        if ((dataFile = fopen(fileName.c_str(), "w+")) != NULL)
        {   // Write the data to a file
            rewind(dataFile);
            fprintf(dataFile, "%s", dataToStore->ToString().c_str());
            fclose(dataFile);
            Log(LOG_DEV_DATA, "Wrote test result data to file: %s", dataToStore->ToString().c_str());
            // Create the file data entry if it does not exist
            if (createFileDataEntry)
            {   // Add the data file to the managed list
                FileData file;
                file.fileName = vin;
                file.creationTime = time(NULL);
                AddDataFileToList(path, fileList, file);
            }
        }
        else
        {   // Could not open the file
            Log(LOG_ERRORS, "Could not open %s for storing data: %s, creating new file", fileName.c_str(), strerror(errno));
        }
        // Get rid of the data we created
        try
        {
            Log(LOG_DEV_DATA, "Deleting test result data %s", dataToStore->ToString().c_str());
            //if(dataToStore != NULL)	delete dataToStore;
            if (dataToStore != NULL)
            {
                dataToStore->clear();
                delete dataToStore;
            }
            Log(LOG_DEV_DATA, "Set dataToStore to NULL\n");
            dataToStore = NULL;
            Log(LOG_DEV_DATA, "Deleted temp data.  Already written to file");
        }
        catch (XmlException &excpt)
        {
            Log(LOG_DEV_DATA, "XmlException while deleting data already written to file: %s", excpt.GetReason());
        }
        catch (BepException &excpt)
        {
            Log(LOG_DEV_DATA, "BepException while deleting data already written to file: %s", excpt.GetReason());
        }
        catch (...)
        {
            Log(LOG_DEV_DATA, "Unknown Exception while deleting data already written to file");
        }
    }
    else
    {
        Log(LOG_ERRORS, "No data to store - data is NULL!");
    }
}

void InterCcrtCommServer::AddDataFileToList(const string &path, FileDataList &fileList, FileData &fileEntry)
{
    Log(LOG_DEV_DATA, "Storing %s created at %s", fileEntry.fileName.c_str(), ctime(&fileEntry.creationTime));
    fileList.insert(FileDataListVal(fileEntry.fileName, fileEntry));
}

void InterCcrtCommServer::AppendDataToFile(const string &path, 
                                           const string &vin, 
                                           const XmlNode *dataToAppend, 
                                           XmlNode* &fileData)
{
    XmlParser parser;
    XmlNodeMap newResults;
    bool processData = (dataToAppend != NULL);

    if (processData)
    {
        try
        {
            newResults.DeepCopy(dataToAppend->getChildren());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "No data to append to file!");
            processData = false;
        }
        // Ensure we still have good data
        if (processData)
        {
            // Read in the existing data if available
            if (fileData != NULL)
            {
                fileData->clear();
                delete fileData;
            }
            try
            {
                fileData = parser.ReturnXMLDocument(path + vin)->Copy();
                Log(LOG_DEV_DATA, "Loaded existing test results: %s", fileData->ToString().c_str());
            }
            catch (XmlException &excpt)
            {
                Log(LOG_ERRORS, "XmlExcpetion - Could not retrieve result file for %s: %s", vin.c_str(), excpt.GetReason()); 
                fileData = NULL;
            }
            catch (BepException &excpt)
            {
                Log(LOG_ERRORS, "BepException - Could not retrieve result file for %s: %s", vin.c_str(), excpt.GetReason()); 
                fileData = NULL;
            }
            // Process the data according to what we were able to retrieve from the system
            if (fileData != NULL)
            {
                const XmlNodeMap *currentResults = &fileData->getChildren();
                // Walk through the new results to see if they are already in the existing results
                for (XmlNodeMapItr iter = newResults.begin(); iter != newResults.end(); iter++)
                {
                    XmlNodeMapCItr currentResultIter = currentResults->find(iter->second->getName());
                    if (currentResultIter != currentResults->end())
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

void InterCcrtCommServer::UpdateNodeData(XmlNode *newData, XmlNode *existingData)
{   // Check attributes first
    const XmlNodeMap *newAttributes = &newData->getAttributes();
    const XmlNodeMap *existingAttributes = &existingData->getAttributes();
    for (XmlNodeMapCItr newAttributeIter = newAttributes->begin(); 
        newAttributeIter != newAttributes->end(); 
        newAttributeIter++)
    {
        XmlNodeMapCItr existingIter = existingAttributes->find(newAttributeIter->second->getName());
        if (existingIter != existingAttributes->end())
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
    for (XmlNodeMapCItr newChildNodeIter = newChildNodes->begin();
        newChildNodeIter != newChildNodes->end();
        newChildNodeIter++)
    {
        XmlNodeMapCItr existingChildNodeIter = existingChildNodes->find(newChildNodeIter->second->getName());
        if (existingChildNodeIter != existingChildNodes->end())
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

void InterCcrtCommServer::ResolveNodeDiffs(XmlNode *newNode, XmlNode *existingNode)
{
    string attributeValue(newNode->getValue());
    bool updateAttributeValue = false;
/* Initial design logic would allow for inter Ccrt Comm to use the same test result name to run 
 * multiple tests.  If the there were passes followed by fails followed by more passes, the
 * test result value should not be updated from fail to pass.  
 * That is why the logic below looks wierd.  
 * Upon review and the possibility of re-running a test, we would want to update fails with
 * passes, or the vehicle would never be able to get through.  It has been decided to always
 * update the result attribute for each test step and the inter Ccrt Comm will send unique
 * test result names for each test.
 */
#if 0  
    if (!newNode->getName().compare(BEP_RESULT))
    {   // This is a result, handle it properly
        if (!existingNode->getValue().compare(BEP_PASS_RESPONSE))
        {   // Currently set to pass, update with current value
            updateAttributeValue = true;
        }
    }
    else
    {   // Just update the value if it is different
        if (existingNode->getValue().compare(attributeValue))
        {   // New value is different, update it
            updateAttributeValue = true;
        }
    }
#else
    updateAttributeValue = true;
#endif
    // Update the value if required
    if (updateAttributeValue)
    {
        Log(LOG_DEV_DATA, "Updating attribute %s value %s to %s", 
            existingNode->getName().c_str(), existingNode->getValue().c_str(), attributeValue.c_str());
        existingNode->setValue(attributeValue);
    }
}


void InterCcrtCommServer::LoadInterfaceThreads(const XmlNode *config)
{
    const XmlNodeMap *threadConfigs = &config->getChildren();
    for (XmlNodeMapCItr iter = threadConfigs->begin(); iter != threadConfigs->end(); iter++)
    {
        if (iter->second->getAttribute("enabled")->getValue() == "1")
        {   // Start a new interface thread
            Log(LOG_DEV_DATA, "Creating inter Ccrt Comm interface %s", iter->second->getName().c_str());
            m_interCcrtComms.push_back(new InterCcrtCommInterface(this));
            // Initialize and start the interface
            Log(LOG_DEV_DATA, "Initialing inter Ccrt Comm interface %s", iter->second->getName().c_str());
            m_interCcrtComms.back()->Initialize(iter->second);
            Log(LOG_DEV_DATA, "Running inter Ccrt Comm interface %s", iter->second->getName().c_str());
            m_interCcrtComms.back()->Resume();
        }
    }
}


const std::string InterCcrtCommServer::Publish(const XmlNode *node)
{
    string result = BepServer::Publish(node);
    Log( LOG_FN_ENTRY, "Enter InterCcrtCommServer::Publish(%s)\n", node->getName().c_str(),node->ToString().c_str());
    if (node->getName() == GetDataTag("VehicleBuild"))
    {
        try
        {
            BroadcastBuildRecord(node, true, false);
        }

        catch (...)
        {
            Log(LOG_DEV_DATA, "Unknown Exception while Broadcasting build data");
        }
    }

    Log( LOG_FN_ENTRY, "Exit InterCcrtCommServer::Publish()\n");

    return(result);
}


const BEP_STATUS_TYPE InterCcrtCommServer::BroadcastBuildRecord(const XmlNode *vehicleBuild, bool storeRecordOnFailure, bool deleteOnSuccess)
{
    Log( LOG_FN_ENTRY, "Enter InterCcrtCommServer::BroadcastBuildRecord()\n");
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    string bom;
    //get part identifier
    string partIdentifier="";
    try
    {
        partIdentifier = vehicleBuild->getChildren().getNode(GetDataTag("BuildPartId").c_str())->getValue();
    }
    catch (XmlException &excpt)
    {
        Log(LOG_DEV_DATA, "XmlException while obtaining part identifier: %s", excpt.GetReason());
        return BEP_STATUS_ERROR;
    }
    Log(LOG_DEV_DATA, "Obtained %s : %s", GetDataTag("BuildPartId").c_str(),partIdentifier.c_str());
    //transmit to connections
    for (int i = 0; i < m_interCcrtComms.size(); i++)
    {
        if(m_interCcrtComms[i]->IsBomConnection())
        {
        //convert node to string of build items
        if (m_interCcrtComms[i]->BuildDataToString(*vehicleBuild,bom) == BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "Transmitting Bom message : %s", bom.c_str());
            if (m_interCcrtComms[i]->SendBOM(partIdentifier,bom) != BEP_STATUS_SUCCESS)
            {
                if (storeRecordOnFailure)
                {
                    StoreBuildData(partIdentifier, vehicleBuild);
                }
                status = BEP_STATUS_ERROR;
            }
            else if (deleteOnSuccess)
            {

                Log(LOG_DEV_DATA, "File %s transmitted, removing.", 
                    partIdentifier.c_str());
                if (0 != unlink((BuildFilePath() + partIdentifier).c_str()))
                {   // Error removing the file
                    Log(LOG_ERRORS, "Could not remove file %s - %s", partIdentifier.c_str(), strerror(errno));
                }
                else
                {   // File removed from file system, remove from managed list
                    FileDataListItr iter = m_buildFiles.find(partIdentifier);
                    Log(LOG_DEV_DATA, "Removing file %s created on %s", iter->second.fileName.c_str(), ctime(&iter->second.creationTime));
                    m_buildFiles.erase(iter);
                }
            }
        }
        }
    }
    return status;
}



void InterCcrtCommServer::BroadcastStoredBuildRecords()
{
    Log( LOG_FN_ENTRY, "Enter InterCcrtCommServer::BroadcastStoredBuildRecords()\n");
    FileDataList  buildFilesLocal = m_buildFiles;
    //create a vector to place files in order created
    vector<FileData> buildFilesToTransmit;
    for (FileDataListItr iter = buildFilesLocal.begin(); iter != buildFilesLocal.end(); iter++)
    {//copy list
        buildFilesToTransmit.push_back(iter->second);       
    }
    //order from oldest first to newest
    sort(buildFilesToTransmit.begin(),buildFilesToTransmit.end(),Is_File_Older());
    for (vector<FileData>::iterator iter = buildFilesToTransmit.begin(); iter != buildFilesToTransmit.end(); iter++)
    {

        XmlParser parser;
        XmlNode *buildData = NULL;
        try
        {
            Log(LOG_DEV_DATA, "Loading local build data from %s%s", 
                BuildFilePath().c_str(), iter->fileName.c_str());
            const XmlNode *loadedBuild = parser.ReturnXMLDocument(BuildFilePath() + iter->fileName);
            buildData = loadedBuild->getChild(GetDataTag("VehicleBuild"))->Copy();
            if (buildData != NULL)
            {
                Log(LOG_DEV_DATA, "Loaded local build data: %s", buildData->ToString().c_str());

                //transmit data to end points delete if transmit is successful
                BroadcastBuildRecord(buildData,false,true);
            }
            else
            {
                Log(LOG_ERRORS, "Local build data not loaded, document is NULL");
            }
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not retrieve build record for %s: %s", iter->fileName.c_str(), excpt.GetReason());
            if (buildData != NULL)  delete buildData;
            buildData = NULL;
        }
    }

}
void InterCcrtCommServer::StoreBuildData(string partIdentifier, const XmlNode *vehicleBuild)
{

    XmlNode buildData(GetDataTag("BuildData"), "");
    // Add the build data if it was retrieved or found locally
    if (vehicleBuild != NULL)
    {   // Add the data to the node
        //buildData.setName("Vin"+vin);
        buildData.setName("PartID" + partIdentifier);
        buildData.addChild(vehicleBuild->Copy());
        Log(LOG_DEV_DATA, "Created build data node: %s", buildData.ToString().c_str());
        Log(LOG_DEV_DATA, "DEBUG: Add build data\n");
        StoreTesterData(BuildFilePath(), partIdentifier, &buildData, m_buildFiles, false);
        Log(LOG_DEV_DATA, "Stored the tester data information");
    }
    else
    {
        Log(LOG_ERRORS, "Build Data not added - addBuildData: %s,  vehicleBuildData NULL-%s %s", 
            vehicleBuild == NULL ? "True" : "False", 
            vehicleBuild == NULL ? "NULL" : vehicleBuild->ToString().c_str());
    }
}

void InterCcrtCommServer::SetFault(string faultName)
{
    Log( LOG_FN_ENTRY, "Setting fault: %s\n", faultName.c_str());
    if (m_faultServer != NULL)
    {
        try
        {
            const XmlNode *faultNode = m_faultInformation->getChild(faultName);
            std::string response;
            m_faultServer->SetFault(
                                   faultNode->getAttribute("Name")->getValue(),
                                   faultNode->getAttribute("Description")->getValue(),
                                   GetProcessName(),response,true,
                                   atoi(faultNode->getAttribute("Severity")->getValue().c_str()));
            Log(LOG_ERRORS, "Fault: %s, %s\n", 
                faultNode->getName().c_str(),
                faultNode->getAttribute("Description")->getValue().c_str());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not retrieve Fault info for %s", faultName.c_str());
        }
        catch (...)
        {
            Log(LOG_ERRORS, "Exception reporting fault: %s",faultName.c_str());
        }
    }
}
void InterCcrtCommServer::ClearFault(string faultName)
{
    Log( LOG_FN_ENTRY, "Clearing fault: %s\n", faultName.c_str());
    if (m_faultServer != NULL)
    {
        try
        {
            const XmlNode *faultNode = m_faultInformation->getChild(faultName);
            std::string response;
            m_faultServer->ClearFault(
                                     faultNode->getAttribute("Name")->getValue(),response,true);
            Log(LOG_ERRORS, "Fault Cleared: %s, %s\n", 
                faultNode->getName().c_str(),
                faultNode->getAttribute("Description")->getValue().c_str());
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not retrieve Fault info for %s", faultName.c_str());
        }
        catch (...)
        {
            Log(LOG_ERRORS, "Exception reporting fault: %s",faultName.c_str());
        }
    }
}
