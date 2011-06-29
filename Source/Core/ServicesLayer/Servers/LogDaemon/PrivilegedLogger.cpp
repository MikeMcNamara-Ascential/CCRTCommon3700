//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.cpp $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Logging
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.cpp 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/PrivilegedLogger.cpp $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 2     8/14/03 11:14a Khykin
// Updated with make install changes.
// 
// 1     8/07/03 1:53p Khykin
// Initial checkin.
//    Revision 1.4  2003/07/31 16:16:16  crosby
//    Fixed mem leak -- was using "new" in the creation of timestamp for LogMessage
//
//    Revision 1.3  2003/07/21 19:20:30  swieton
//    Fixed uninitialized pointers
//
//    Revision 1.2  2003/07/21 15:11:01  karlin
//    rearranged LogWriter dep
//
//    Revision 1.1  2003/07/18 19:58:45  karlin
//    added PrivilegedLogger
//
//
//============================================================================

#include "PrivilegedLogger.h"
#include "LogWriter.h"

PrivilegedLogger::PrivilegedLogger()
	: logWriter (NULL)
{
}

PrivilegedLogger::~PrivilegedLogger() {}

void PrivilegedLogger::PrivLog(const std::string& msg) {
	char tempStamp[22];
	if (logWriter != NULL) {
		LogMessage logMsg(
				ROLL_LOG_STRING,
				ILogger::GetTimeString(tempStamp),
				m_processName,
				msg,
				"");
		
		logWriter->ProcessNormalMessage(&logMsg);
	}
}

void PrivilegedLogger::SetLogWriter(LogWriter *pLogWriter) {
	logWriter = pLogWriter;
}

void PrivilegedLogger::SetProcessName(const std::string& name) {
	m_processName = name;
}

	
				
