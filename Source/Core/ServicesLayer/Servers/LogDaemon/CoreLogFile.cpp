//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.cpp $
// $Date: 2/26/04 6:33p $
//
// CLASS:
//		CoreLogFile
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.cpp 2     2/26/04 6:33p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.cpp $
// 
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004. 
//
// 1     12/21/03 6:41p Cward
//    Revision 1.13  2003/10/07 21:55:19  swieton
//    Removed perrors
//
//    Revision 1.12  2003/10/07 21:02:17  swieton
//    Minor sanity checks
//
//    Revision 1.11  2003/10/07 18:04:51  karlin
//    tester stuff, and small bug fix in corelogfile
//
//    Revision 1.10  2003/10/03 16:03:28  karlin
//    Added MarkOldestFile()
//
//    Revision 1.9  2003/10/03 15:44:06  crosby
//    Fixed scandir for QNX
//
//    Revision 1.8  2003/10/03 14:03:47  crosby
//    Replaced LimitCoreLog code
//
//    Revision 1.7  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.6  2003/10/02 19:35:13  crosby
//    checking mutex lock/unlock return status
//
//    Revision 1.5  2003/10/01 18:55:53  crosby
//    Fixed mutex leak in RotateIfNeeded()
//
//    Revision 1.4  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.16  2003/07/21 19:25:23  crosby
//    Fixed LimitCoreLog() bugs
//
//    Revision 1.15  2003/07/21 17:34:03  crosby
//    Error handling in rotate code; now calling Cap() on rotation failure.
//
//    Revision 1.14  2003/07/21 17:16:25  crosby
//    Need to finish error handling in rotation code
//
//    Revision 1.13  2003/07/18 21:10:41  crosby
//    Deleted obsolete debug msg
//
//    Revision 1.12  2003/07/18 17:49:42  karlin
//    removed spurious couts
//
//    Revision 1.11  2003/07/18 17:06:39  crosby
//    Implemented LimitCoreLog
//    Shuffled/added params in/to constructor
//
//    Revision 1.10  2003/07/18 16:41:10  crosby
//    RotateOrCap() --> RotateIfNeeded()
//
//    Revision 1.9  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.8  2003/07/17 23:32:54  crosby
//    Coppied RotateIfNeeded() code from superclass trying to debug hang-up problem in rotator.
//
//    Revision 1.7  2003/07/17 20:40:14  crosby
//    Implemented RotateIfNeeded(), beefed up constructor
//
//    Revision 1.6  2003/07/17 16:21:15  crosby
//    Fixed constructor params
//
//    Revision 1.5  2003/07/17 15:56:25  karlin
//    added IncrementFileName
//
//    Revision 1.4  2003/07/15 21:32:49  crosby
//    Updated constructor.
//    Skel'd override of RotateIfNeeded()
//
//    Revision 1.3  2003/07/14 19:24:34  crosby
//    Updated constructor to accept log base dir
//
//    Revision 1.2  2003/07/11 19:47:52  crosby
//    Added skel of RedirectToFile().
//    Added destructor.
//
//    Revision 1.1  2003/07/10 22:53:51  crosby
//    added
//
//
//******************************************************************************
#include <limits.h>
#include "LogDaemon.h"
#include "CoreLogFile.h"


/**
 * Create a new CoreLogFile
 */
CoreLogFile::CoreLogFile(
		const std::string &pLogDir,
		const std::string &pFileName,
		const std::string &pBaseName,
		const UINT32 &pMaxStartLogSize,
		const UINT32 &pMaxStartLogFiles,
		const UINT32 &pTestLogCapSize,
		const UINT32 &pMaxTestLogFiles)
		: LogFile(pLogDir,pFileName,pMaxStartLogSize,pMaxStartLogFiles),
			baseName(pBaseName),
			usingStartLog(true),
			capSize(pTestLogCapSize),
			maxTestLogFiles(pMaxTestLogFiles)
{
}

CoreLogFile::~CoreLogFile() {}

/**
 * Redirect output to a new file
 */
void CoreLogFile::IncrementFileName(const std::string& suffix) {
	pthread_mutex_lock(&writeLock);
	
	Close();
	LimitCoreLog(); // delete really old core logs, if we are over the limit

	// then opens a new log file with the requested name
	fileName = logDir + baseName + suffix;

	Open();

	usingStartLog = false;

	pthread_mutex_unlock(&writeLock);
}

/**
 * If we're in "StartLog" mode, rotate the StartLog file as needed,
 * according to maxFiles and maxSize.  Otherwise, we check for
 * filesize larger than cap size; if exceeded, the current log
 * is "capped" (closed; Writes result in silent no-op).
 *
 * (Override default behavior of LogFile::RotateIfNeeded, as
 * core log is special.)
 */
int CoreLogFile::RotateIfNeeded() {
	if (isCapped) {
		// We've been capped
		return 0;
	}

	if (usingStartLog) {
		// Start log is treated/rotated like an additional log, via savelog
		pthread_mutex_lock(&writeLock);
		if (MaxSizeExceeded()) {
			if (IsDebugOn()) {
				LogDaemon::out("CoreLogFile::RotateIfNeeded: StartLog maxsize exceeded.");
			}
			Close();
			//XXX int fail = RunSaveLog();
			int fail = Rotate();

			if (fail) {
				// This just goes to stderr:
				LogDaemon::out("CoreLogFile::RotateIfNeeded: Rotation failed with exit code: %d", fail);
				// Let's cap it
				Cap();
				// Unlock
				pthread_mutex_unlock(&writeLock);
				// Bail out
				return fail;
			}
			// Reopen the file for logging
			Open();

			if (IsDebugOn()) {
				LogDaemon::out("CoreLogFile::RotateIfNeeded: StartLog rotation completed.");
			}
		}
		// savelog is done, so release the lock
		pthread_mutex_unlock(&writeLock);
		
	} else {
		// We're in "TestLog" mode.
		// Test logs aren't rotated; we merely check for cap limit
		if (Size() > ((long)capSize*(long)1048576))
		{
			// we're too big; cap it
			Cap();
		}
	}	
	return 0;
}

/**
 * Execute the limit-core-log.sh script.
 * This makes sure that core log files do not exceed a certain count.
 * If needed, old files are removed to make room for new.
 */
int CoreLogFile::LimitCoreLog() {
	char buf[PATH_MAX]; // temp buf, holds full filenames.

	// Make sure that logPath ends in '/'.
	// The constructor for LogFile ensures this, but we're
	// being especially careful.
	std::string logPath = logDir;
	if (logPath.size() == 0)
	{
		// zero-length log dir.
		logPath = "/";
	}
	else if (logPath.c_str()[logPath.size()-1] != '/')
	{
		// append a slash
		logPath += "/";
	}


	// Scan the directory, listing all files
	struct direct **namelist;
	char fnameCopy[logPath.size()+1];
	strcpy(fnameCopy, logPath.c_str()); // scandir on QNX won't take const
	int n;
	n = scandir(fnameCopy, &namelist, 0, alphasort);
	if (n < 0)
	{
		// dir listing failed
		char tbuf[300];
		snprintf(tbuf, 300, "scandir [%s]", logPath.c_str());
		LogDaemon::out("%s: %s", tbuf, strerror(errno));
		return -1;
	}

	int slen = baseName.size();
	char* fname;

	// Get a count of how many files are actually core logs
	int count = 0;
	for (int i = 0; i < n; i++) {
		fname = namelist[i]->d_name;
		if (strlen(fname) >= (unsigned int)slen && strncmp(baseName.c_str(), fname, slen)==0) {
			count++;
		}
	}

	// allocate space for core log entries (array of CoreFileEntry ptrs)
	CoreFileEntry** testLogs = (CoreFileEntry**)calloc(count, sizeof(CoreFileEntry*));
	struct stat myStat;
	int idx = 0;
	// Create CoreFileEntry objects for the testLogs
	for (int i = 0; i < n; i++) {
		fname = namelist[i]->d_name;

		if (strlen(fname) >= (unsigned int)slen && strncmp(baseName.c_str(), fname, slen)==0) {
			strcpy(buf, logPath.c_str());
			strcat(buf, fname);

			testLogs[idx] = new CoreFileEntry();
			testLogs[idx]->name = buf;
			testLogs[idx]->removed = false;

			int status = stat(buf, &myStat);
			if (status == 0)
			{
				testLogs[idx]->modtime = myStat.st_mtime;
			}
			else
			{
				// couldn't stat
				char tbuf[300];
				snprintf(tbuf, 300, "stat [%s]", buf);
				LogDaemon::out("%s: %s", tbuf, strerror(errno));
				// set modtime to "ancient"
				testLogs[idx]->modtime = 0;
			}
			// increment array index
			idx++;

		}
		// deallocate dir entry
		free(namelist[i]);
	}
	// deallocate list structure
	free(namelist);


	// Compare count to maxfiles
	int killCount = count - maxTestLogFiles;
	if (killCount > 0) {
		// Loop on the difference n
		for (int i = 0; i < killCount; i++) {
			// Mark oldest remaining file
			MarkOldestFile(testLogs,count);
		}
		// Find and delete any marked files
		for (int i = 0; i < count; i++) {
			if (testLogs[i]->removed) {
				// Delete the file
				if (unlink(testLogs[i]->name.c_str()) == -1)
				{
					// deletion failed;
					char tbuf[300];
					snprintf(tbuf, 300, "CoreLogFile::LimitCoreLog: Deleting core log [%s]",
							testLogs[i]->name.c_str());
					LogDaemon::out("%s: %s", tbuf, strerror(errno));
				}
			}
		}
	}

	// Deallocate the CoreFileEntry objects
	for (int i = 0; i < count; i++) {
		delete(testLogs[i]);
	}
	// dellallocate the array space
	free(testLogs);

	return 0;




	//XXX 	// usage: limit-core-logs.sh <prefix> <maxfiles>
	//XXX 	std::string cmdString("limit-core-logs.sh");
	//XXX 	cmdString.append(" ");
	//XXX 	cmdString.append(logDir);
	//XXX 	cmdString.append(baseName);
	//XXX 	char totalFiles[12];
	//XXX 	sprintf(totalFiles, "%d", maxTestLogFiles);
	//XXX 	cmdString.append(" ");
	//XXX 	cmdString.append(totalFiles);

	//XXX 	if (IsDebugOn()) {
	//XXX 		LogDaemon::out("CoreLogFile::LimitCoreLog(): %s", cmdString.c_str());
	//XXX 	}
	//XXX 	// Execute the savelog script
	//XXX 	int exitCode = system(cmdString.c_str());
	//XXX 	if (exitCode != 0) {
	//XXX 		// non-zero return indicates execution failure
	//XXX 		if (IsDebugOn()) {
	//XXX 			LogDaemon::out("CoreLogFile::LimitCoreLog(): %s", strerror(errno));
	//XXX 		}
	//XXX 		return -1;
	//XXX 	}

	//XXX 	return 0;
}

/**
 * Given an array of CoreFileEntry objects,
 * find the oldest CoreFileEntry not marked "removed"
 * and mark it thus.
 * @param files The CoreFileEntry array
 * @param size The length of the array
 */
void CoreLogFile::MarkOldestFile(CoreFileEntry *files[], int size) {
		// To start with, the "oldest" file is the first one
		// we find in the array that isn't aready marked:
		int start = 0;
		CoreFileEntry *oldest = files[start++];
		while (oldest->removed) {
				oldest = files[start++];
		}

		// Loop through remaining files in the list:
		for (int i=start; i < size; i++) {
				if (files[i]->removed) {
						// Skip files that are already marked
						continue;
				}
				if (difftime(files[i]->modtime, oldest->modtime) < 0) {
						// files[i] is older than current oldest, track it
						oldest = files[i];
				}
		}
		// Mark oldest file as "removed"
		oldest->removed = true;
}

