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
#ifndef RemoteVehicleBuildServer_h 
#define RemoteVehicleBuildServer_h
//-------------------------------------------------------------------------------------------------
#include <time.h>
#include "BepServer.h"
//-------------------------------------------------------------------------------------------------
class RemoteVehicleBuildServer : public BepServer
{
	#define BUILD_SERVER_PUSLE_CODE			SYSTEM_PULSE+16
	#define BUILD_FILE_CHECK_PULSE			13
	
public:

	RemoteVehicleBuildServer();
	virtual ~RemoteVehicleBuildServer();

	/**
	 * Process the reload configuration pulse.  If any other pulses
	 * are received, send them on to BepComponent.
	 *
	 * @param code   Pulse code
	 * @param value  Pulse value
	 * @return If pulse is handled.
	 *         Handled     == BEP_STATUS_SUCCESS
	 *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
	 */
	virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * <b>Description:</b>
     * Method for initialization passing in the file name.
     *
     * @param fileName Name of the XML configuration file to be used to 
     *                 initialize.
     */
    virtual void Initialize(const string &fileName);

	/**
	 * <b>Description:</b>
	 * Method for initialization passing an XML node.
	 *
	 * <b>Configuration Items:</b>
	 *     <ul>
	 *     <li> Setup/Debug </li>
	 *     <li> Setup/DataTags </li>
	 *     </ul>
	 *
	 * @param document Pointer to a parsed configuration document.
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






protected:

	/**
	 * Load the next vehicle build data to prepare for testing.
	 */
	void LoadNextVehicleBuildRecord(void);

	/**
	 * Load the existing vehicle build records.
	 */
	void LoadVehicleBuildRecords(void);

	/**
	 * <b>Description:</b>
	 * Write the requested data item from the server.
	 *
	 * @param node  The data tag and value to write.
	 *
	 * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
	 *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
	 *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
	 */
	virtual const string Publish(const XmlNode *node);

	/**
	 * Remove the build file for the vehicle that just compelted tested.
	 */
	void RemoveVehicleBuildFile();

	/**
	 * Store the provided build data to file.
	 * 
	 * @param buildData Build data to be stored.
	 */
	void StoreVehicleBuildRecord(const XmlNode *buildData);





private:

	/**
	 * Get/Set the flag indicating if the server is being run in producer mode.
	 * 
	 * @param inProducerMode
	 *               Flag indicating if the server is being run in producer mode.
	 * 
	 * @return Flag indicating if the server is being run in producer mode.
	 */
	const bool& IsInProducerMode(const bool *inProducerMode = NULL);

	/**
	 * Setup the specified timer.
	 * 
	 * @param timer      Timer to be setup.
	 * @param timerDelay Time between timer pulses.
	 * @param timerPulseCode
	 *                   Pulse code to be sent when the timer expires.
	 */
	void SetupTimer(BepTimer &timer, UINT64 &timerDelay, UINT8 &timerPulseCode);

	/**
	 * Get/Set the path to the vehicle build files.
	 * 
	 * @param path   Path to the vehicle build files.
	 * 
	 * @return Path to the vehicle build files.
	 */
	const string& VehicleBuildFilePath(const string *path = NULL);

	// Directory for the vehicle build files
	string m_buildFilePath;

	// Flag to indicate if the server is running in producer mode.
	bool m_isProducerMode;

	// Timer to use for checking for new build files
	BepTimer m_buildFileTimer;

	BepMutex m_buildFileListMutex;

	// Structure to contain the file name and creation time for each file being tracked.
	typedef struct _buildFileData
	{
		string fileName;	   // Name of the file being tracked.
		time_t creationTime;   // Time the file was created.
	} BuildFileData;

	typedef map<string, *FileData>    FileDataList;
	typedef FileDataList::iterator    FileDataListItr;
	typedef FileDataList::value_type  FileDataListVal;

	// Sorting criteria
	struct Is_File_Older
	{
		inline bool operator() (const BuildFileData *file1, const BuildFileData *file2)
		{
			return (difftime(file1->creationTime,file2->creationTime) < 0);
		}
	};

	// List of the existing vehicle build files.
	FileDataList m_vehicleBuildFiles;
};

//-------------------------------------------------------------------------------------------------
#endif //RemoteVehicleBuildServer_h
