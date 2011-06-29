//******************************************************************************
// Description:
//	Object to manage data log files for ICM data
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/Include/IcmLogFile.h $
// 
// 3     4/13/06 4:03a Cward
// move implementation to cpp
//
// 2     2/15/06 4:08p Bmeinke
// Moved all code to a cpp file
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 2     8/06/03 4:47p Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/26/03 1:32p Bmeinke
// Added some commented out code to detect if log file samples were
// successfully to disk (NOT TESTED)
//
// 1     6/12/03 4:11p Khykin
// Initial checkin.
//
// 2     5/19/03 11:19a Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:43p Bmeinke
// Migration to new personalized configuration management structure
//
// 8     3/18/03 6:53p Bmeinke
// Changed the raw mutex used to lock the log file object to a BepMutex
//
// 7     3/10/03 8:41p Bmeinke
// Synchornozed with Toluca source
//
// 2     2/08/03 9:49a Khykin
// Updated for logging issues.
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 5     11/14/02 10:28a Bmeinke
// Links were being created as hard links...not symlinks
// Return value for OpenLogFile() not set to true if file was opened
// successfully
//
// 4     8/27/02 12:21a Bmeinke
// Fixed it
//
// 3     8/23/02 11:24a Kh
// Updating for logging changes.
//
// 1     1/07/02 10:33a Brian.meinke
// ASCII Data Log File object for ICM data
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _ICM_LOG_FILE_H_INCLUDED
#define _ICM_LOG_FILE_H_INCLUDED

/*******************
 include files
 ******************/
#include <stdio.h>			// perror, printf
#include <stdarg.h>			// Variable argument stuff
#include <stdlib.h>			// rand
#include <string.h>			// memmove, strlen, etc
#include <time.h>			// POSIX Timers
#include <signal.h>			// POSIX signals
#include <semaphore.h>		// POSIX semaphores
#include <errno.h>
#include <pthread.h>
#include <inttypes.h>		// uint32_t, uint64_t, etc

//#include <sys/neutrino.h>
//#include <sys/dispatch.h>
//#include <sys/wait.h>
//#include <sys/resource.h>
#include <string>
#include <map>

#include "tchar.h"
#include "Bepos.h"			// Standard BEP wrappers
#include "XmlParser.h"		// XML Parser definition
#include "XmlDefs.h"		// XML definitions
#include "XmlNode.h"		// XML Node definitions
#include "BepXmlUtils.h"	// XML Utilities
#include "BepException.h"
#include "IcmDefs.h"
#include "IcmTagList.h"
#include "BepSync.h"		// BepMutex
/**
 * Mapping of data sample number to tag
 */
typedef std::multimap< uint32_t, string>	IcmTagMap_t;
typedef IcmTagMap_t::iterator				IcmTagMapItr_t;
typedef IcmTagMap_t::const_iterator			IcmTagMapCItr_t;
typedef IcmTagMap_t::value_type				IcmTagMapVal_t;

/**
 * Object used to manage an ascii data log file for ICM data
 *
 * @author Brian Meinke
 * @version 1
 * @since Jan 03, 2002
 */
class IcmLogFile
{
public:
	IcmLogFile();

	~IcmLogFile();

	/**
	 * Object initializer from XML node
	 *
	 * @param tagList    Pointer to the object holding the list of sample indices and
	 *                   associated tags to be used when logging tags to the data log
	 * @param configNode Node to read log file configuration from
	 */
	void Initialize( IcmTagList *tagList, const XmlNode* configNode);

	/**
	 * Object initializer from XML node
	 *
	 * @param tagList  Pointer to the object holding the list of sample indices and
	 *                 associated tags to be used when logging tags to the data log
	 * @param fileName Name of the file to log data to
	 * @param linkName Path to the file link we need to create at the end of
	 *                 the test to notify the GUI that the data file is valid
	 */
	void Initialize( IcmTagList *tagList, const string &fileName, const string &linkName);

	/**
	 * Prepares the log file object to start logging new data.
	 * Resets the tag list and removes the associated file link.
	 */
	void StartLog();

	/**
	 * Finalizes the log file data. Updates the link to the log file
	 * used by the GUI to know when the log file is valid
	 */
	void EndLog();

	/**
	 * Adds an entry to the log field with the given data values
	 *
	 * @param dataIdx ICM array index of this sample
	 * @param lf      LF data point
	 * @param rf      RF data point
	 * @param lr      LR data point
	 * @param rr      RR data point
	 *
	 * @return EOK if successful, other values indicate error
	 */
	int LogSample( uint32_t dataIdx, float lf, float rf, float lr, float rr);

	/**
	 * Adds an entry to the log field with the given data values
	 *
	 * @param dataIdx ICM array index of this sample
	 * @param lf      LF data point
	 * @param rf      RF data point
	 * @param lr      LR data point
	 * @param rr      RR data point
	 * @param lt      LT data point
	 * @param rt      RT data point
	 *
	 * @return EOK if successful, other values indicate error
	 */
	int LogSample( uint32_t dataIdx, float lf, float rf, float lr, float rr, float lt, float rt);

	/**
	 * Attempts to lock the object for extended use.
	 *
	 * @param block  Flag indicating if caller wants to block until the lock can
	 *               be accomplished (block=true), or retrurn immediately if the
	 *               lock cannot be obtained (block=false).
	 * @return EOK if successful, other value if an error occurrs
	 */
	int Lock( bool block=true);

	/**
	 * Releases a lock on the object (after a successful call to Lock())
	 *
	 * @return EOK if successful, other value if an error occurs
	 */
	int Unlock();

	/**
	 * Return the status of the log file.
	 *
	 * @return The status of logging to file.
	 *         true - logging, false - not logging
	 */
	bool IsEnabled(void);

	/**
	 * Set the enabled status of the log file.
	 *
	 * @param enableStatus The status to update the logger with.
	 */
	void SetEnabled(bool enableStatus);

	/**
	 * Returns the name of the log file we are saving data to
	 *
	 * @return The name of the log file we are saving data to
	 */
	const std::string& GetName() const;

private:

	/**
	 * Opens a new log file
	 *
	 * @return true if file opened successfully, false if an error occurred
	 */
	bool OpenLogFile();

	/**
	 * Closes an existing log file
	 */
	void CloseLogFile();

	/**
	 * Updates (creates) the file link to the log file at the end of a test.
	 * The link is used by the GUI to know when the log files are valid
	 *
	 * @return -1 if error occurs, any other value indicates success
	 */
	int UpdateFileLink();

	/**
	 * Removes the file link to the log file at the beginning of a test.
	 * The link is used by the GUI to know when the log files are valid
	 */
	void RemoveFileLink();

	/**
	 * Appends a sample tag to the log file for the given sample index
	 *
	 * @param dataIdx Index of the sample to append tags to in the log file
	 */
	void AddTagToSample( uint32_t dataIdx);

	/**
	 * List of tags to be applied to data samples
	 */
	IcmTagList			*m_tagList;

	/**
	 * Mutex used to lock the log file object
	 */
	BepMutex			m_logFileLock;;

	/**
	 * Name of the file to log data to
	 */
	string				m_logFileName;

	/**
	 * Path to the link to update after data logging is finalized
	 */
	string				m_logFileLink;

	/**
	 * Handle to the file used to log data
	 */
	FILE				*m_file;

	/**
	 * Flag to indicate if loggin to this log file is enabled
	 */
	bool				m_enabled;
};


#endif // _ICM_LOG_FILE_H_INCLUDED
