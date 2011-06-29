//****************************************************************************
// $Archive: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.h $
// $Date: 10/30/07 4:11p $
//
// CLASS:
//      Logging
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.h 4     10/30/07 4:11p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.h $
// 
// 4     10/30/07 4:11p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/25/06 3:37p Rwiersem
// Added an Initialize() method that takes a file name string.  Added
// SetConfigFile(), GetConfigFile(), and LoadAdditionalConfiguration()
// items.  These are used to reload the configuration without restarting
// the process.
//
// 2     7/20/05 9:16p Cward
// Added new TaskMon support
//
// 1     12/21/03 6:41p Cward
//    Revision 1.12  2003/10/27 18:16:56  tenharmsel
//    Seperated out sections of Run() method into RunInit and MessageReceptorLoop() so that it is more easily tested in parts, and so the method was a little shorter and clearer.  a test has been written for RunInit()
//
//    Revision 1.11  2003/10/23 18:31:26  tenharmsel
//    added logic to deal with atoi returning 0 when it can't convert the parameter given to it.  it defaults to some #defined values in the header
//
//    Revision 1.10  2003/10/03 13:54:08  crosby
//    Changed privLog --> auxLog
//    Changed privFileName --> auxFileName
//
//    Revision 1.9  2003/10/02 23:16:31  swieton
//    Using internal logger output
//
//    Revision 1.8  2003/10/02 22:16:20  swieton
//    Wrote privlidged (sp?) log function
//
//    Revision 1.7  2003/10/02 22:03:28  swieton
//    fixed static initialization
//
//    Revision 1.6  2003/10/02 21:25:28  swieton
//    *** empty log message ***
//
//    Revision 1.5  2003/10/02 21:10:44  swieton
//    Added static log file object
//
//    Revision 1.4  2003/10/02 18:05:28  karlin
//    including fcntl.h
//
//    Revision 1.3  2003/10/01 21:28:59  crosby
//    Added RedirectOutputToFile()
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 9:18a Khykin
// Updated with changes from Flatrock.
//    Revision 1.11  2003/07/22 22:15:40  crosby
//    made friend class for testing
//
//    Revision 1.10  2003/07/21 15:05:49  crosby
//    Including PrivilegedLogger.h
//
//    Revision 1.9  2003/07/21 14:52:53  crosby
//    Renamed some ivars for clarity
//
//    Revision 1.8  2003/07/21 13:55:22  karlin
//    cleaned up, added some PrivilegedLoggers
//
//    Revision 1.7  2003/07/18 21:13:15  crosby
//    *** empty log message ***
//
//    Revision 1.6  2003/07/18 19:55:56  crosby
//    Inherit from Debuggable.
//    Added Initialize()
//    Added ivars for Initialize().
//
//    Revision 1.5  2003/07/16 23:25:07  crosby
//    Including LogRotator
//
//    Revision 1.4  2003/07/15 15:06:54  crosby
//    Including BepDefs.h for DATA_LOGGER_NAME
//
//    Revision 1.3  2003/07/11 21:16:35  crosby
//    Def for ParseLogMessage
//
//    Revision 1.2  2003/07/10 22:48:13  crosby
//    Changed start() to Run(), added some includes.
//
//    Revision 1.1  2003/07/10 19:06:31  crosby
//    skel'd the header
//
//    Revision 1.4  2003/07/09 22:02:31  karlin
//    converted to LogFile of new LogDaemon system
//
//******************************************************************************

#ifndef _LOG_DAEMON_H_INCLUDED_
#define _LOG_DAEMON_H_INCLUDED_

#include "LogMessage.h"
#include "LogRotator.h"
#include "BepDefs.h"
#include "LogMsgQueue.h"
#include "LogWriter.h"
#include "IInterProcessCommunication.h"
#include "XmlNode.h"
#include "Debuggable.h"
#include "PrivilegedLogger.h"
#include "ITaskMon.h"
#include <string>
#include <iostream.h>

// for dup2 and RedirectOutputToFile:
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

class LogFile;

// Defines for the default sizes of variables
#define LOG_START_FILE_MAX_SIZE 5
#define LOG_START_FILE_MAX_FILES 10
#define LOG_STD_FILE_MAX_SIZE 10
#define LOG_STD_FILE_MAX_FILES 100
#define LOG_RECEIVER_PRIORITY 10
#define LOG_WRITER_PRIORITY 20
#define LOG_ROTATOR_PRIORITY 10
#define LOG_ROTATOR_PERIOD 10
#define LOG_ROTATOR_MAX_DELAYS 30
#define LOG_MSG_QUEUE_SIZE 1000

/**
 * LogDameon is the primary class implementing the log server.
 *
 * See Managers/LoggingUtility/LogDaemonMain.cpp for the bootstrap/executable.
 */
class LogDaemon : public Debuggable, public PrivilegedLogger {
    friend class LogDaemonTest;
    public:
        LogDaemon();

        /**
         * Initialize the Log Daemon.  The configuration file name will be
         * stored to allow for configuration reloading.
         *
         * @param fileName - configuration file name
         */
        void Initialize(const std::string &fileName);

        void Initialize(const XmlNode*);

        /**
         * The main loop.
         */
        void Run();

        /** The name of the the internal log file. Used by auxLog */
        static string auxFileName;

        static void out(const char *fmt, ...);

        /**
         * Set the configuration file name.
         *
         * @param fileName - configuration file name
         */
        void SetConfigFile(std::string fileName);

        /**
         * Get the configuration file name.
         *
         * @return the configuration file name
         */
        std::string GetConfigFile();
    protected:
        /**
         * Load the additional configuration items needed by the log daemon.
         *
         * @param document A pointer to a parsed configuration file.
         */
        virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    private:
        static void ParseLogMessage(LogMessage*, std::string&);
        /**
         * This method is the actual main loop, it is called by the Run()
         * method after Run sets up the needed environment for the main
         * loop to run (creating the log rotator and the log msg queue, etc).
         * it was decoupled from the main Run() loop so that each part
         * can be unit tested more easily.
         * @since Oct 27, 2003
         */
        void MessageReceptorLoop();

        /**
         * This method initializes some variables that are used when Run is called
         * Run() calls this method.
         * @since Oct 27, 2003
         */
        void RunInit();

        /** A statically allocated LogFile used for logging messages generated
         * by the logging system itself */
        static LogFile auxLog;

        /** The main logmsg queue used by Run */
        LogMsgQueue *queue;
        /** The Log File Map used by Run */
        LogFileMap *fileMap;
        /** The core log file used by Run */
        CoreLogFile *coreLog;

        // Config options
        bool useDebugging;
        std::string logBasePath;

        std::string startFileName;
        int startFileMaxSize;
        int startFileMaxFiles;

        std::string stdFileName;
        int stdFileMaxSize;
        int stdFileMaxFiles;

        int receiverPriority;
        int logWriterPriority;
        int logRotatorPriority;
        int logRotatorPeriod;
        int logRotatorMaxDelays;
        std::string logRotatorTag;
        std::string logRotatorValue;

        int msgQueueSize;

        ITaskMon    m_taskMon;

        /** Configuration file name */
        std::string m_configFile;
};

#endif
