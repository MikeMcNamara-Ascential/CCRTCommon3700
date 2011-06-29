//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Logging
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogFileMap.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.4  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.3  2003/10/02 18:26:03  crosby
//    Changed Lock/Unlock to return int
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 9:19a Khykin
// Updated with changes from Flatrock.
//    Revision 1.9  2003/07/21 13:55:22  karlin
//    cleaned up, added some PrivilegedLoggers
//
//    Revision 1.8  2003/07/18 20:49:52  anderson
//    added some more tests
//
//    Revision 1.7  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.6  2003/07/16 20:19:15  crosby
//    Added logDirBase ivar and SetLogDir() and GetLogDir()
//
//    Revision 1.5  2003/07/14 20:02:44  crosby
//    removed typedef for LogFileIterator_t
//
//    Revision 1.4  2003/07/14 19:18:17  karlin
//    added LogFileIterator
//
//    Revision 1.3  2003/07/11 21:15:05  crosby
//    Implemented.  Got typedef and ivars, added functions for CoreLogFile access
//
//    Revision 1.2  2003/07/10 22:50:14  crosby
//    Skel'd a few methods, need to resolve iterator type.
//
//    Revision 1.1  2003/07/09 21:45:49  crosby
//    Skel header for LogFileMap
//
//******************************************************************************
#ifndef _LOG_FILE_MAP_H_INCLUDED_
#define _LOG_FILE_MAP_H_INCLUDED_

#include "LogFile.h"
#include "LogFileIterator.h"
#include "CoreLogFile.h"
#include <string>
#include <map>

typedef std::map<std::string, LogFile*> LogFileMap_t;

/**
 * LogFileMap provides a home for LogFile objects via a thread-safe map.
 * Also stores the a CoreLogFile reference.
 */
class LogFileMap {
	friend class LogFileMapTest;
	public:
		LogFileMap();
		virtual ~LogFileMap();
		int Lock();
		int Unlock();
		void Put(const std::string &, LogFile*);
		LogFile* Get(const std::string &);
		CoreLogFile* GetCoreLog();

		LogFileIterator* Iterator();
		
		void SetCoreLog(CoreLogFile*);

		void SetLogDir(const std::string &);
		const std::string& GetLogDir();
		void Remove(std::string&);
		
	private:
		CoreLogFile* coreLog;
		LogFileMap_t fileMap;
		pthread_mutex_t lock;
		pthread_mutexattr_t lockAtts;
		std::string logBaseDir;
};

#endif 
