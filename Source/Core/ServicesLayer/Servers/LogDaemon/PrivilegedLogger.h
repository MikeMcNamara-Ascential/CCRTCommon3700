//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Privileged
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.3  2003/10/01 22:44:04  anderson
//    got tests to build
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 2     8/14/03 11:15a Khykin
// Updated with make install changes.
// 
// 1     8/07/03 9:17a Khykin
// Updated with changes from Flatrock.
//    Revision 1.5  2003/07/21 15:11:01  karlin
//    rearranged LogWriter dep
//
//    Revision 1.4  2003/07/21 15:09:22  crosby
//    Including LogMessage.h
//
//    Revision 1.3  2003/07/21 15:05:22  crosby
//    Fixed circular dep
//
//    Revision 1.2  2003/07/18 20:44:33  crosby
//    Added ; to end of class
//
//    Revision 1.1  2003/07/18 19:58:45  karlin
//    added PrivilegedLogger
//
//
//******************************************************************************
#ifndef _PRIVILEGED_LOGGER_H_INCLUDED_
#define _PRIVILEGED_LOGGER_H_INCLUDED_

#include "LogDefs.h"
#include "ILogger.h"
#include "LogMessage.h"
class LogWriter;
/**
 * Provides internal privileged access to the Log output facilities for use by
 * Log-system objects exclusively.  It bypasses the Log Message Queue, calling
 * LogWriter directly.
 */
class PrivilegedLogger {

	friend class PrivilegedLoggerTest;

	public:
	PrivilegedLogger();
	virtual ~PrivilegedLogger();
	void SetLogWriter(LogWriter *pLogWriter);
	
	protected:
	void PrivLog(const std::string& msg);
	void SetProcessName(const std::string& name);

	private:

	/**
	 * name of the process for logging
	 */
	std::string  m_processName;

	/**
	 * LogWriter to log to.
	 */
	LogWriter *logWriter;
};

#endif
