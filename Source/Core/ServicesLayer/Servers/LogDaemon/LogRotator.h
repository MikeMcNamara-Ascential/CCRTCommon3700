//******************************************************************************
// Description:
//   The Log Rotator class scans the LogFileMap for files that need to be
//   rotated.
//
//==============================================================================
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
//    $Log: /Core/ServicesLayer/Servers/LogDaemon/LogRotator.h $
// 
// 2     9/01/04 9:37p Cward
// Added functionality to prevent the log daemon from rotating files
// during a test sequence.
//
// 1     12/21/03 6:41p Cward
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 9:19a Khykin
// Updated with changes from Flatrock.
//    Revision 1.14  2003/07/22 21:37:34  karlin
//    fixed rebug with const args
//
//    Revision 1.13  2003/07/22 17:39:23  karlin
//    dealt with TODOs and FIXMEs
//
//    Revision 1.12  2003/07/21 17:15:58  crosby
//    Including "PrivilegedLogger.h"
//
//    Revision 1.11  2003/07/21 14:28:27  swieton
//    Refactored for easier testing
//
//    Revision 1.10  2003/07/18 21:14:21  crosby
//    Added SetDebugging()
//
//    Revision 1.9  2003/07/18 19:56:08  crosby
//    Added SetPeriod()
//
//    Revision 1.8  2003/07/17 22:44:36  crosby
//    Inherit from Debuggable
//
//    Revision 1.7  2003/07/17 20:20:55  crosby
//    Made Launch() static
//
//    Revision 1.6  2003/07/17 14:09:59  karlin
//    added sleepTime
//
//    Revision 1.5  2003/07/16 23:26:25  crosby
//    include unistd.h
//
//    Revision 1.4  2003/07/15 22:19:05  crosby
//    Destructor was not declared.
//
//    Revision 1.3  2003/07/15 22:10:03  karlin
//    filled in methods
//
//    Revision 1.2  2003/07/10 22:50:39  crosby
//    Skel'd constructor
//
//    Revision 1.1  2003/07/10 19:06:38  crosby
//    skel'd the header
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _LOG_ROTATOR_H_INCLUDED_
#define _LOG_ROTATOR_H_INCLUDED_
//==============================================================================

#include "LogFileMap.h"
#include "LogFile.h"
#include "Debuggable.h"
#include "PrivilegedLogger.h"
#include <string>
#include <vector>
#include <iostream.h>
#include <unistd.h>

class INamedDataBroker;

/**
 * Wakes up periodically and scans the LogFileMap for files that need to be
 * rotated.
 */
class LogRotator : public Debuggable, public PrivilegedLogger {
	friend class LogRotatorTest;

	public:
		LogRotator(LogFileMap*);
		virtual ~LogRotator();
		void Run();
		static void* Launch(void*);
		void SetPeriod(int);
		void SetDebugging(bool);

		/**
		 * Set the delay attributes. This is the tag to listen for, the
		 * value to look for and the maximum number of times to delay.
		 * @param tag tag to listen for
		 * @param value value to look for
		 * @param delays maximum number of times to delay.
		 */
		void SetDelayOn( const std::string &tag, const std::string &value, const int &delays);
		
	private:
		LogFileMap *logFileMap;

		/**
		 * Named data broker interface which provides the ability
		 * for the component to interact with the real time
		 * system. (Read, Write, Command, Subscribe, Unsubscribe)
		 * @since Version 1.0
		 */
		INamedDataBroker *m_ndb;

		/**
		 * Time between rotation wakes, in seconds.
		 */
		int sleepTime;

		/**
		 * Maximum number of times to delay when waking up.
		 */
		INT32 m_maxDelays;

		/**
		 * Tag to look at before waking up.
		 */
		std::string m_tag;

		/**
		 * Value to look for from tag which causes us to go back to sleep.
		 */
		std::string m_delayValue;

		void GetAdditionalLogFiles (std::vector<LogFile*> &files);
		void RotateLogFiles (std::vector<LogFile*> &files);
		void RotateCoreLogFile ();
};

#endif
