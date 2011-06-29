//****************************************************************************
// $Archive: /Core/ServicesLayer/Servers/LogDaemon/LogWriter.h $
// $Date: 12/21/03 6:41p $
//
// CLASS:
//		Logging
//
// FILE:
//    $Header: /Core/ServicesLayer/Servers/LogDaemon/LogWriter.h 1     12/21/03 6:41p Cward $
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogWriter.h $
// 
// 1     12/21/03 6:41p Cward
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
// 
// 1     8/07/03 9:20a Khykin
// Updated with changes from Flatrock.
//    Revision 1.13  2003/07/25 18:13:29  swieton
//    added tests for LogWriter
//
//    Revision 1.12  2003/07/23 14:10:05  crosby
//    #include "Bepos.h"
//
//    Revision 1.11  2003/07/23 14:00:08  crosby
//    Accepting thread priority in constructor
//
//    Revision 1.10  2003/07/21 22:06:32  swieton
//    added friend test class
//
//    Revision 1.9  2003/07/18 21:14:12  crosby
//    Added SetDebugging()
//
//    Revision 1.8  2003/07/18 19:58:45  karlin
//    added PrivilegedLogger
//
//    Revision 1.7  2003/07/18 19:56:21  crosby
//    Inherit from Debuggable
//
//    Revision 1.6  2003/07/17 15:57:55  karlin
//    added IncrementCoreLogFile
//
//    Revision 1.5  2003/07/16 21:41:41  crosby
//    Added ProcessUnknownCommand()
//
//    Revision 1.4  2003/07/16 21:27:56  crosby
//    Added methods for handling activate/deactivate log file commands
//
//    Revision 1.3  2003/07/11 21:15:32  crosby
//    Taking LogFileMap in constructor.
//
//    Revision 1.2  2003/07/10 22:50:59  crosby
//    Now takes queue in constructor, has a static Launch method
//
//    Revision 1.1  2003/07/09 21:44:25  crosby
//    Skel of header for LogWriter
//
//******************************************************************************
#ifndef _LOG_WRITER_H_INCLUDED_
#define _LOG_WRITER_H_INCLUDED_


#include "LogMsgQueue.h"
#include "LogFileMap.h"
#include "BepDefs.h"
#include "Bepos.h"
#include "Debuggable.h"
#include <iostream.h>
#include <string>
#include <stdlib.h>

/**
 * LogWriter is the consumer of the LogMsgQueue: it dequeues log messages
 * and writes them to log files.
 *
 * LogWriter also responds to the "activate/deactivate special log" message
 * sent from particular LogIfc clients,
 * and the "increment test number" message sent from VehicleTest.
 */
class LogWriter : public Debuggable {
	friend class LogWriterTest;
	public:
		LogWriter(LogMsgQueue*, LogFileMap*);
		void Run();
		static void* Launch(void*);
		static std::string FormatMessage(const LogMessage&, std::string &);
		virtual void ProcessNormalMessage(LogMessage*);
		virtual void SetDebugging(bool);
		
	protected:
		virtual void RouteMessage(LogMessage*);

		virtual int ActivateLogFile(LogMessage*);
		virtual int DeActivateLogFile(LogMessage*);
		virtual void ProcessUnknownCommand(LogMessage*);
		virtual int IncrementCoreLogFile(LogMessage*);
	private:
		LogFileMap* fileMap; // where to lookup LogFile references
		LogMsgQueue* queue;  // incoming log message queue
};

#endif 
