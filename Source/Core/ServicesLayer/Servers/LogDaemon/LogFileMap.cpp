//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.cpp $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		LogFileMap
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.cpp 1     12/21/03 6:41p Cward $
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.cpp $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.7  2003/10/09 21:35:56  swieton
//    initializing memory for mutexes, fixed possible file-leak
//
//    Revision 1.6  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.5  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.4  2003/10/02 16:16:32  crosby
//    cerr --> cout
//
//    Revision 1.3  2003/10/02 16:07:47  crosby
//    mutex is non-recursive, error-checking type now.
//    All lock/unlock events are checked.
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.12  2003/07/23 20:22:23  swieton
//    logwriter tests
//
//    Revision 1.11  2003/07/21 21:16:00  crosby
//    Removed recursive mutex lock semantics
//
//    Revision 1.10  2003/07/21 20:34:27  crosby
//    Recursive pthread mutex lock
//
//    Revision 1.9  2003/07/21 19:20:30  swieton
//    Fixed uninitialized pointers
//
//    Revision 1.8  2003/07/21 13:55:22  karlin
//    cleaned up, added some PrivilegedLoggers
//
//    Revision 1.7  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.6  2003/07/16 20:19:26  crosby
//    Added GetLogDir() and SetLogDir()
//
//    Revision 1.5  2003/07/14 20:03:18  crosby
//    Constructing iterator with reference to map, not to self
//
//    Revision 1.4  2003/07/14 19:18:17  karlin
//    added LogFileIterator
//
//    Revision 1.3  2003/07/11 21:09:32  crosby
//    Implemented Put() and Get(), fixed const string refs
//
//    Revision 1.2  2003/07/11 19:48:28  crosby
//    Added accessors for CoreLogFile.
//    Get() and Put() accept LogFile ptrs.
//
//    Revision 1.1  2003/07/10 22:53:51  crosby
//    added
//
//
//******************************************************************************
#include "LogFileMap.h"
#include "LogDaemon.h"

/**
 * Create a new LogFileMap
 */
LogFileMap::LogFileMap()
: coreLog (NULL)
{
	memset(&lockAtts, 0, sizeof(lockAtts));
	memset(&lock, 0, sizeof(lock));

	// Make a non-recursive, error-checking mutex:
	pthread_mutexattr_init(&lockAtts);
	pthread_mutexattr_settype(&lockAtts, PTHREAD_MUTEX_ERRORCHECK); 
	pthread_mutex_init(&lock, &lockAtts);
}

/**
 * Destroy a LogFileMap.
 */
LogFileMap::~LogFileMap()
{
	pthread_mutex_destroy(&lock);
}

/**
	* Lock our mutex lock.
	*/
int LogFileMap::Lock() {
	return pthread_mutex_lock(&lock);
}

/**
	* Unlock our mutex lock.
	*/
int LogFileMap::Unlock() {
	return pthread_mutex_unlock(&lock);
}
/**
 * Map a file name to a LogFile pointer 
 */
void LogFileMap::Put(const std::string &fname, LogFile* logFile)
{
	int stat = Lock();
	if (stat != EOK) 
	{
		// Complain and abort.
		LogDaemon::out("LogFileMap::Put: FAILED to get lock.");
		return;
	}

	fileMap[fname] = logFile;

	stat = Unlock();
	if (stat != EOK) {
		LogDaemon::out("LogFileMap::Put: FAILED to unlock mutex.");
	}
}

/**
 * Retrieve a LogFile by name
 */
LogFile* LogFileMap::Get(const std::string &fname)
{
	LogFileMap_t::iterator	itr;

	int stat = Lock();
	if (stat != EOK) 
	{
		// Complain and abort.
		LogDaemon::out("LogFileMap::Get FAILED to get lock.");
		return( NULL );
	}


	if((itr=fileMap.find( fname)) == fileMap.end())
	{
		stat = Unlock();
		if (stat != EOK) {
			LogDaemon::out("LogFileMap::Get: FAILED to unlock mutex.");
		}
		return( NULL);
	}
	LogFile *logFile = itr->second;

	// unlock
	stat = Unlock();
	if (stat != EOK) {
		LogDaemon::out("LogFileMap::Get: FAILED to unlock mutex.");
	}
	return logFile;
}

/**
 * Get an iterator for the map.
 */
LogFileIterator *LogFileMap::Iterator() {
	LogFileIterator *iter = new LogFileIterator(&fileMap);
	return iter;
}

/**
 * Return a pointer to the core log file
 */
CoreLogFile* LogFileMap::GetCoreLog()
{
	return coreLog;
}


/**
 * Set the core log reference.
 * @param pCoreLog Pointer to a CoreLogFile instance
 */
void LogFileMap::SetCoreLog(CoreLogFile* pCoreLog)
{
	coreLog = pCoreLog;
}

/**
 * Set the log dir value
 */
void LogFileMap::SetLogDir(const std::string& fname)
{
	logBaseDir = fname;
}

/**
 * Return the log dir value 
 */
const std::string& LogFileMap::GetLogDir()
{
	return logBaseDir;
}

/**
 * Remove the item with the given key from the map.
 */
void LogFileMap::Remove(std::string& filename)
{
	int stat = Lock();
	if (stat != EOK) 
	{
		// Complain and abort.
		LogDaemon::out("LogFileMap::Put FAILED to get lock.");
		return;
	}

	fileMap.erase(filename);

	stat = Unlock();
	if (stat != EOK) {
		LogDaemon::out("LogFileMap::Remove: FAILED to unlock mutex.");
	}
}


