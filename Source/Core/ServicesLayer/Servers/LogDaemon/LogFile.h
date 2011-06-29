//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogFile.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Logging
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogFile.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogFile.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.5  2003/10/24 20:21:55  tenharmsel
//    updated LogDaemon tests to correctly access files during a test
//
//    Revision 1.4  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.3  2003/10/07 18:04:51  karlin
//    tester stuff, and small bug fix in corelogfile
//
//    Revision 1.2  2003/10/01 18:52:07  crosby
//    Removed RunSaveLog()
//
//    Revision 1.1  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 2     8/06/03 4:01p Khykin
// Updated with latest changes from Flatrock.
//    Revision 1.22  2003/07/21 21:46:56  swieton
//    fileStream made dynamically allocated
//
//    Revision 1.21  2003/07/21 15:06:11  crosby
//    Including PrivilegedLogger.h
//
//    Revision 1.20  2003/07/21 13:55:22  karlin
//    cleaned up, added some PrivilegedLoggers
//
//    Revision 1.19  2003/07/18 21:14:30  crosby
//    Added SetDebugging()
//
//    Revision 1.18  2003/07/18 18:15:41  anderson
//    Made a working test for LogFile
//
//    Revision 1.17  2003/07/17 23:41:40  crosby
//    Subclassing Debuggable
//
//    Revision 1.16  2003/07/17 20:40:44  crosby
//    *** empty log message ***
//
//    Revision 1.15  2003/07/17 20:40:31  crosby
//    Added some checks for file size, added  Size()
//
//    Revision 1.14  2003/07/17 15:35:38  crosby
//    Defined LOGFILE_ROTATE_PROG as "savelog"
//
//    Revision 1.13  2003/07/17 15:03:52  crosby
//    Cleaned up constructor and constructor params
//
//    Revision 1.12  2003/07/16 20:39:01  crosby
//    made const ref type for GetFileName()
//
//    Revision 1.11  2003/07/15 21:31:24  crosby
//    removed FILE* ivar
//
//    Revision 1.10  2003/07/15 21:30:37  crosby
//    Updated constructor to take rotation params.
//
//    Revision 1.9  2003/07/14 19:27:18  crosby
//    Changed Write() to WriteLine(),
//    removed default constructor.
//
//    Revision 1.8  2003/07/14 15:57:53  crosby
//    Changed Write() to WriteLine()
//
//    Revision 1.7  2003/07/14 13:43:59  crosby
//    Added GetFileName()
//
//    Revision 1.6  2003/07/10 22:49:29  crosby
//    Added args for Write()
//
//    Revision 1.5  2003/07/10 19:05:34  crosby
//    Changed ifdef to ifndef!
//
//    Revision 1.4  2003/07/09 22:02:31  karlin
//    converted to LogFile of new LogDaemon system
//
//******************************************************************************

#ifndef _LOG_FILE_H_INCLUDED_
#define _LOG_FILE_H_INCLUDED_

#include "BepDefs.h"
#include "Debuggable.h"
#include "PrivilegedLogger.h"
#include <string>
//#include <iostream.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define LOGFILE_ROTATE_PROG "savelog"

class LogFileTest;
class LogDaemonTest;

/**
 * Encapsulate a log file.
 * Provide thread-safe operations, because writer thread
 * may conflict with a maintenance thread (eg, log rotation).
 */
class LogFile : public Debuggable, public PrivilegedLogger {
	friend class LogFileTest;
	friend class LogDaemonTest;

	public:
	LogFile(const std::string &ldir, const std::string &filename,
			    const UINT32 &msize, const UINT32 &mfiles);
	virtual ~LogFile();

	virtual const std::string& GetFileName() const;
	virtual int Open();
	virtual int Close();
	virtual int RotateIfNeeded();
	virtual int Flush();
	virtual int WriteLine(std::string);
	virtual int Cap();
	virtual long Size();
	virtual int AddRef();
	virtual uint32_t GetRefCount() const;
	virtual int DelRef();

	void SetDebugging(bool);

	protected:
	virtual bool MaxSizeExceeded();
	virtual int Rotate();

	std::string logDir;
	std::string fileName;
	UINT32 maxSize;
	UINT32 maxFiles;
	ofstream *fileStream;
	bool isCapped;
	pthread_mutex_t writeLock;
	pthread_mutexattr_t writeLockAtts;

private:
	uint32_t	refCount;
	void SetFileName(std::string &newFileName);

};

#endif
