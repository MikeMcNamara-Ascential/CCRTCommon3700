//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommServer.h 1     10/26/06 1:21p Bbarrett $
//
// FILE DESCRIPTION:
//   The InterCcrtCommServer manages the data for the endpoint.  Build
//   data will be retrieved for the endpoint when requested.  Test
//   result data will also be saved and stored from the endpoint.
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
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommServer.h $
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     10/16/06 3:08p Derickso
// Updated reconnect logic and added keep alive/heartbeat messages.
// 
// 1     8/30/06 4:42p Derickso
// New endpoint server.
// 
//===========================================================================
#ifndef InterCcrtCommServer_h
#define InterCcrtCommServer_h
//===========================================================================
#include <time.h>
#include "BepSync.h"
#include "BepServer.h"
#include "BepTimer.h"
#include "InterCcrtCommInterface.h"
#include "IFaultServer.h"

class InterCcrtCommInterface;


/**
 * InterCcrtCommServer
 * The InterCcrtCommServer manages data for the endpoint.
 * Build data will be retrieved for the endpoint and test results from the
 * endpoint will be stored for the dynamic test sequence.  The InterCcrtCommServer
 * will manage the build data and the test results by removing each of these
 * files after a specific time period.
 */
class InterCcrtCommServer : public BepServer
{
private:
    /**
     * Structure to contain the file name and creation time for each file being tracked.
     */
    typedef struct _fileData
    {
        string fileName;	   // Name of the file being tracked.
        time_t creationTime;   // Time the file was created.
    } FileData;
    /**
     * List of valid FileData items.
     */
    typedef map<string, FileData>     FileDataList;
    typedef FileDataList::iterator    FileDataListItr;
    typedef FileDataList::value_type  FileDataListVal;

    struct Is_File_Older
    {
        inline bool operator() (const FileData& file1, const FileData& file2)
        {
            return (difftime(file1.creationTime,file2.creationTime) < 0);
        }
    };
    /**
     * Pulse code and value to use for checking data file status.
     */
#define INTER_CCRT_COMM_SERVER_PULSE_CODE     SYSTEM_PULSE+19
#define DATA_FILE_PULSE	        10
//#define INTER_CCRT_COMM_SERVER_PULSE_CODE     SYSTEM_PULSE+19
#define KEEPALIVE_PULSE     11

public:
	/**
	 * Class Constructor.
	 */
	InterCcrtCommServer();
	/**
	 * Class Destructor.
	 */
	virtual ~InterCcrtCommServer();
	/**
	 * Method for initialization passing in the file name
	 *
	 * @param fileName Name of the XML configuration file to be used to initialize
	 */
	virtual void Initialize(const std::string &fileName);
	/**
	 * Method for initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 */
	virtual void Initialize(const XmlNode *document);
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 */
	virtual const string Register(void);
    /**
     * Read the build data for the requested vin.
     * The data will be retrieved for the specified VIN and stored for reuse.
     * The local data will be checked first.  If the build data has not already been retrieved,
     * it will be read from the WCC.
     * 
     * @param vin       VIN to get build data for.
     * @param buildData Build data for the requested vin.
     * 
     * @return Result of getting the build data from the system.
     */
    const string ReadBuildData(const string &vin, XmlNode &buildData);
	/**
	 * Request to read a data tag
	 *
	 * @param node    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 */
	virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
	/**
	 * Request to write a data tag
	 *
	 * @param node    Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 */
	virtual const string Write(const XmlNode *node);
    /**
     * Will handle pulse for thread deathdata file management.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * Check if the build data for the requested VIN has already been retrieved.
     * If the data has been retrieved, it will be loaded; otherwise, nothing will be done.
     *
     * @param vin  VIN to load build data for.
     * @param buildData  Build data for the requested VIN.
     *
     * @return  Flag indicating if the build data was already stored locally.
     *				True  - Build data is locally available for the requested VIN.
     *				False - Build data must be retrieved from the server.
     */
    bool CheckForBuildData(const string &vin, XmlNode* &buildData);
    /**
     * Condition variable to signal all threads to check for comms from the endpoint.
     */
    BepCondVar<std::string, false> m_checkComms;
	/**
	 * Get a system data tag by reference from the data tag
	 * list in the configuration file.
	 *
	 * @param dataTag The data tag to look up.
	 * @return The name for the data tag that was looked up.
	 * @since Version 4.3
	 */
	const string GetDataTag(const string dataTag);
    void StoreBuildData(string partIdentifier, const XmlNode *vehicleBuild);
    const BEP_STATUS_TYPE BroadcastBuildRecord(const XmlNode *vehicleBuild, bool storeRecordOnFailure, bool deleteOnSuccess);
    void BroadcastStoredBuildRecords();
    void SetFault(string faultName);
    void ClearFault(string faultName);
    /**
     * Interface to the fault server
     */
    IFaultServer        *m_faultServer;

protected:
	/**
	 * Store and retrieve the path for the endpoint test results.
	 * If the path is NULL, then the stored path will be returned.
	 *
	 * @param path	Directory path to the endpoint test results.
	 * @return Directory path for test results.
	 */
	const string& TestResultPath(string *path = NULL);
	/**
	 * Store and retrieve the path for the build files.
	 * If the path is NULL, then the stored path will be returned.
	 *
	 * @param path	Directory path to the build files.
	 * @return Directory path for build files.
	 */
	const string& BuildFilePath(string *path = NULL);
    /**
     * Check if the specified data file is currently being managed.
     * 
     * @param fileName Data file to check for.
     * @param fileList List to check for the specified file.
     * 
     * @return Flag indicating the requested data file is being managed.
     *         True - the file is being managed.
     *         False - the file does not exist.
     */
    const bool CheckForDataFile(const string &fileName, FileDataList &fileList);
    /**
     * Load the existing temporary files from the specified location into the FileDataList provided.
     * The files will be checked to ensure they are still valid.  If the files are too old, they will
     * be removed and not stored.
     * 
     * @param path     Path to use for loading the endpoint files.
     * @param fileList Data list to populate with the information from the specified location.
     */
    void LoadDataFiles(const string &path, FileDataList &fileList);
    /**
     * Store the data the tester is using.
     * The data will be stored in a file using vin as the filename in the directory specified
     * by path.  If the appendToFile parameter is supplied, the data will be appended to the 
     * file if the file exists.  If the file does not exist, it will be created.  The entry
     * will then be added to the managed data list.
     *
     * @param path  		Directory path for storing the data.
     * @param vin   		VIN to store data for.
     * @param data  		Data to be stored for the provided VIN.
     * @param fileList  	Managed data file list to add the file entry to.
     * @param appendToFile	Flag to indicate if data should be appended to the file.
     */
    void StoreTesterData(const string &path, const string &vin, const XmlNode *data, FileDataList &fileList, const bool &appendToFile);
    /**
     * Add the specified file to the list.
     * The file creation date will be compared against the current time.
     * If the file is too old, it will be removed from the system; otherwise,
     * the file will be added to the list.
     *
     * @param path      Path to the file name.
     * @param fileList  List to add the file data to.
     * @param fileEntry File to be age checked and added to the provided list.
     */
    void AddDataFileToList(const string &path, FileDataList &fileList, FileData &fileEntry);
    /**
     * Append the data to the existing file data.
     * The existing data will be retrieved.  The new data will then be checked to determine if any of it 
     * should be added to an existing node or just added to the main node.
     *
     * @param path			Directory path of the file to be appended.
     * @param vin			VIN for the data to be stored.
     * @param dataToAppend	New data to be added to the file.
     * @param fileData		Data to be written back to the file.
     */
	void AppendDataToFile(const string &path, const string &vin, const XmlNode *dataToAppend, XmlNode* &fileData);
    /**
     * The new data will be compared to the existing data and updated appropriately.
     * Each itme in the nodes will be compared and resolved.  Any item that is in the new data node and not in the 
     * existing daat node, will be copied into the existing data node.  If any of the items have different values,
     * it will be assumed that the new data is correct.  The only exception is for setting a pass or fail value.  
     * A fail value will always overwrite a pass value, but a pass value will never overwrite a fail value.
     * 
     * @param newData New data to be included in the existing data node.
     * @param existingData
     *                Existing data node to update.
     */
    void UpdateNodeData(XmlNode *newData, XmlNode *existingData);
    /**
     * This function resolves any differences between the nodes supplied.
     * If the node is a test result node, the data will only be updated if the value was pass.  A pass value will not
     * overwrite a fail value.  Otherwise, the new data value will be stored into the existing node.
     * 
     * @param newNode New data to be included in the existing node.
     * @param existingNode
     *                Existing data node to update.
     */
    void ResolveNodeDiffs(XmlNode *newNode, XmlNode *existingNode);
    /**
     * Load the endpoint interface threads.
     * One thread will be loaded for each node in the configuration data.  The configuration data 
     * will be checked to ensure the tester thread is enabled.  If the config data indicates the
     * thread is not enabled, an interface thread will not be started for that endpoint.
     *
     * @param config  Configuration data to use for starting all the required interface threads.
     */
    void LoadInterfaceThreads(const XmlNode *config);


    void CreateDirectory(string path);
    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle build records published.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const std::string Publish(const XmlNode *node);

private:
	/**
	 * Directory path where vehicle build files are kept.
	 */
	string m_buildFilePath;
	/**
	 * Directory path where vehicle build files are kept.
	 */
	string m_testResultPath;
	/**
	 * Current test results from endpoints.
	 */
	FileDataList  m_interCcrtCommResults;
	/**
	 * Current build files requested by endpoints.
	 */
	FileDataList  m_buildFiles;

    /**
     * Communication object to interface with the plant host server to retrieve build data.
     * 
     * @see IBepCommunication
     */
    IBepCommunication *m_plantHostComm;
    /**
     * Vector to hold the pointers to the endpoint interface objects that
     * have been started.
     *
     * @see InterCcrtCommServer
     */
    vector<InterCcrtCommInterface *>  m_interCcrtComms;
    /**
     * Configuration node containing the setup data for the endpoint interface threads.
     */
    XmlNode *m_interfaceThreadConfig;

    /**
     * Reference to the fault information (Name, Description, ...).
     */
    XmlNode *m_faultInformation;
};

//===========================================================================
#endif //InterCcrtCommServer_h
