//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		CoreLogFile
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/CoreLogFile.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.7  2003/10/03 15:44:19  crosby
//    Changed includes to be correct for QNX
//
//    Revision 1.6  2003/10/03 15:06:43  crosby
//    Added some includes for use by LimitCoreLog()
//
//    Revision 1.5  2003/10/03 14:04:05  crosby
//    Added CoreFileEntry struct, MarkOldestFile() method.
//
//    Revision 1.4  2003/10/02 20:13:23  anderson
//    Fixed tests so they now run and run successfully
//
//    Revision 1.3  2003/10/01 22:44:04  anderson
//    got tests to build
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 9:17a Khykin
// Upded with changes from Flatrock.
//    Revision 1.10  2003/07/18 17:06:55  crosby
//    Added maxTestLogFiles field, tweaked constructor
//
//    Revision 1.9  2003/07/18 16:41:00  crosby
//    RotateOrCap() --> RotateIfNeeded()
//
//    Revision 1.8  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.7  2003/07/17 20:42:52  crosby
//    Beefed up constructor
//
//    Revision 1.6  2003/07/17 16:21:24  crosby
//    Fixed constructor params
//
//    Revision 1.5  2003/07/17 15:55:42  karlin
//    added IncrementFileName
//
//    Revision 1.4  2003/07/15 21:31:53  crosby
//    Overriding RotateIfNeeded() (unimplemented).
//    Updating constructor to accept rotate params.
//
//    Revision 1.3  2003/07/14 19:26:40  crosby
//    Accepting log dir base in constructor
//
//    Revision 1.2  2003/07/11 21:14:37  crosby
//    Added destructor and RedirectToFile() skel.
//
//    Revision 1.1  2003/07/10 22:54:11  crosby
//    added
//
//    Revision 1.1  2003/07/09 21:44:25  crosby
//    Skel of header for LogWriter
//
//******************************************************************************
#ifndef _CORE_LOG_FILE_H_INCLUDED_
#define _CORE_LOG_FILE_H_INCLUDED_

#include "LogFile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/dir.h>


/**
 * CoreFileEntry struct is for tracking core log file entries,
 * comparing their modification times, and marking for them deletion.
 */
typedef struct _CoreFileEntry {
	/** Full file name */
	std::string name; 

	/** Modification time */
	time_t modtime;

	/** Marked for removal */
	bool removed;
} CoreFileEntry;	


/**
 * CoreLogFile is a specialization of LogFile for the extra
 * features needed by the primary log file in the CCRT server.
 */
class CoreLogFile : public LogFile {
	friend class CoreLogFileTest;
	public:
		CoreLogFile::CoreLogFile(
				const std::string &pLogDir,
				const std::string &pFileName,
				const std::string &pBaseName,
				const UINT32 &pMaxStartLogSize,
				const UINT32 &pMaxStartLogFiles,
				const UINT32 &pTestLogCapSize,
				const UINT32 &pMaxTestLogFiles);

		virtual ~CoreLogFile();
		void RedirectToFile(std::string);
		virtual int RotateIfNeeded();
		virtual void IncrementFileName(const std::string& suffix);
		virtual int LimitCoreLog();
	private:
		std::string baseName;  // Eg, StartLog, TestLog
		bool usingStartLog;     // currently this CoreLogFile is using StartLog
		UINT32 capSize;        // How big can TestLog* get before capping
		UINT32 maxTestLogFiles; // maximum number of TestLog* files to keep around
		
		void MarkOldestFile(CoreFileEntry *files[], int size);
};


#endif 
