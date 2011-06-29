//****************************************************************************
// $Archive: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.cpp $
// $Date: 4/26/06 7:01p $
//
// CLASS:
//		LogDaemon
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.cpp 5     4/26/06 7:01p Cward $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemon.cpp $
// 
// 5     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/25/06 3:37p Rwiersem
// Added an Initialize() method that takes a file name string.  Added
// SetConfigFile(), GetConfigFile(), and LoadAdditionalConfiguration()
// items.  Added processing of the RELOAD_PULSE.  These are used to reload
// the configuration without restarting the process.
//
// 4     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 3     7/20/05 9:16p Cward
// Added new TaskMon support
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:41p Cward
//    Revision 1.20  2003/10/27 18:16:56  tenharmsel
//    Seperated out sections of Run() method into RunInit and MessageReceptorLoop() so that it is more easily tested in parts, and so the method was a little shorter and clearer.  a test has been written for RunInit()
//
//    Revision 1.19  2003/10/23 18:31:26  tenharmsel
//    added logic to deal with atoi returning 0 when it can't convert the parameter given to it.  it defaults to some #defined values in the header
//
//    Revision 1.18  2003/10/09 21:35:56  swieton
//    initializing memory for mutexes, fixed possible file-leak
//
//    Revision 1.17  2003/10/03 14:05:51  crosby
//    Updated comments
//
//    Revision 1.16  2003/10/03 13:54:14  crosby
//    Changed privLog --> auxLog
//    Changed privFileName --> auxFileName
//
//    Revision 1.15  2003/10/02 23:21:03  swieton
//    Opening internal log
//
//    Revision 1.14  2003/10/02 23:16:31  swieton
//    Using internal logger output
//
//    Revision 1.13  2003/10/02 23:08:49  swieton
//    Flushing internal logger
//
//    Revision 1.12  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.11  2003/10/02 22:16:20  swieton
//    Wrote privlidged (sp?) log function
//
//    Revision 1.10  2003/10/02 22:07:43  swieton
//    Adding auxLog to the file map, removed DANGER code
//
//    Revision 1.9  2003/10/02 22:03:28  swieton
//    fixed static initialization
//
//    Revision 1.8  2003/10/02 21:25:28  swieton
//    *** empty log message ***
//
//    Revision 1.7  2003/10/02 21:10:44  swieton
//    Added static log file object
//
//    Revision 1.6  2003/10/02 19:36:11  karlin
//    fixed some typeos
//
//    Revision 1.5  2003/10/02 19:35:13  crosby
//    checking mutex lock/unlock return status
//
//    Revision 1.4  2003/10/02 17:54:12  crosby
//    Wrangled the odd exception handler in ParseMessage
//
//    Revision 1.3  2003/10/01 21:27:59  crosby
//    Parsing DANGER_RedirectStdOutStdErr
//
//    Revision 1.2  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.24  2003/07/21 19:34:08  crosby
//    Fixed another hard-coded SetDebugging(true)
//
//    Revision 1.23  2003/07/21 19:25:50  crosby
//    Setting debug truth for writer and rotator (correctly, this time)
//
//    Revision 1.22  2003/07/21 17:42:08  crosby
//    Removed FIXME and TODO items
//
//    Revision 1.21  2003/07/21 17:16:47  crosby
//    LogRotator was not having a LogWriter pointer given to it
//
//    Revision 1.20  2003/07/21 14:55:00  crosby
//    Renamed some ivars for clarity
//
//    Revision 1.19  2003/07/21 14:07:25  crosby
//    Added more debug msgs to init
//
//    Revision 1.18  2003/07/18 21:10:00  crosby
//    Calling SetDebugging() on construycted objects
//
//    Revision 1.17  2003/07/18 20:22:39  crosby
//    Turn debug on/off for CoreLogFile
//
//    Revision 1.16  2003/07/18 20:19:27  crosby
//    Got it to compile
//
//    Revision 1.15  2003/07/18 19:56:45  crosby
//    Using vars from Initialize() to build objects
//
//    Revision 1.14  2003/07/18 19:17:47  crosby
//    Implementing Initialize()
//
//    Revision 1.13  2003/07/18 17:49:42  karlin
//    removed spurious couts
//
//    Revision 1.12  2003/07/18 17:07:22  crosby
//    Adapting to new CoreLogFile constructor
//
//    Revision 1.11  2003/07/17 22:26:14  crosby
//    Setting up IPC with LOGGER_BUFFER_SIZE
//
//    Revision 1.10  2003/07/17 20:34:04  crosby
//    Launching log rotator
//
//    Revision 1.9  2003/07/17 16:20:25  crosby
//    *** empty log message ***
//
//    Revision 1.8  2003/07/17 16:19:58  crosby
//    Correcting params to CoreLogFile constructor
//
//    Revision 1.7  2003/07/16 23:25:25  crosby
//    Building core log
//
//    Revision 1.6  2003/07/15 21:33:22  crosby
//    Updated to use correct constructor for LogFile
//
//    Revision 1.5  2003/07/15 15:07:16  crosby
//    Binding IPC to DATA_LOGGER_NAME from BepDefs.h
//
//    Revision 1.4  2003/07/14 19:24:46  crosby
//    Constructing LogFiles with log dir base
//
//    Revision 1.3  2003/07/11 21:09:50  crosby
//    Debug msgs
//
//    Revision 1.2  2003/07/11 19:44:31  crosby
//    Switched LogMsgQueue to use LogMessage objects
//
//    Revision 1.1  2003/07/10 22:53:28  crosby
//    working on the producer/consumer core of the daemon
//
//
//******************************************************************************
#include "LogDaemon.h"
#include "LogFile.h"
#include "BepDefs.h"

#include <stdlib.h>
#include <string>

#ifdef _NTO_VERSION
    #if (_NTO_VERSION >= 630)
        #define vsnprintf std::vsnprintf
    #endif
#endif

/* IMPORTANT NOTE: the below declaration initialize static members which are
 * used for logging internally. Note that according to Stroustrup's "The C++
 * Programming Language", static variables are guarenteed to have their
 * constructors called in the order in which they appear in the file.
 * Therefore, the below is valid, and DO NOT CHANGE THE ORDER.
 */
string LogDaemon::auxFileName("LogDaemon.out");
// Log file at $FTPROOT/Logs/LogDaemon.out
LogFile LogDaemon::auxLog( std::string((const char*)getenv("FTP_ROOT"))
			+ "/Logs/", LogDaemon::auxFileName, 10, 20);

void* TaskMonRegisterThread( void *arg)
{
	ITaskMon *taskMon = (ITaskMon *)arg;

	taskMon->ReportUp( "LogDaemon", TASKMON_CLASS_CORE);

	return( NULL);
}

void* TaskMonTerminateThread( void *arg)
{
	ITaskMon *taskMon = (ITaskMon *)arg;

	taskMon->ReportTerminate();

	return( NULL);
}


/**
 * Create a new LogDaemon
 */
LogDaemon::LogDaemon() :
useDebugging(false),
logBasePath("/Logs"),
startFileName("StartLog"),
startFileMaxSize(LOG_START_FILE_MAX_SIZE),
startFileMaxFiles(LOG_START_FILE_MAX_FILES),
stdFileName("TestLog"),
stdFileMaxSize(LOG_STD_FILE_MAX_SIZE),
stdFileMaxFiles(LOG_STD_FILE_MAX_FILES),
receiverPriority(LOG_RECEIVER_PRIORITY),
logWriterPriority(LOG_WRITER_PRIORITY),
logRotatorPriority(LOG_ROTATOR_PRIORITY),
logRotatorPeriod(LOG_ROTATOR_PERIOD),
logRotatorMaxDelays(LOG_ROTATOR_MAX_DELAYS),
logRotatorTag(""),
logRotatorValue("1"),
msgQueueSize(LOG_MSG_QUEUE_SIZE),
m_taskMon( "LogDaemon", TASKMON_CLASS_CORE),
    m_configFile("")
{
	queue = NULL;
	fileMap = NULL;
	coreLog = NULL;
}

void LogDaemon::Initialize(const std::string &fileName)
{
    XmlParser parser;
    const XmlNode *document = parser.ReturnXMLDocument(fileName.c_str());

    SetConfigFile(fileName);    // set the configuration file name
    Initialize(document);    // initialize the LogDaemon
}

/**
 * Read in some config options from an XML document.
 * Default values are already set by the constructor for this class,
 * so parse failures result in no action to the variables in question.
 */
void LogDaemon::Initialize(const XmlNode *document)
{
	m_taskMon.Register("LogDaemon", TASKMON_CLASS_CORE);

	if(document == NULL)
	{
		return;
	}

    LoadAdditionalConfigurationItems(document);

    if(IsDebugOn())
    {
        LogDaemon::out("LogDaemon::Initialize: msgQueueSize=%d", msgQueueSize);
    }
}

void LogDaemon::LoadAdditionalConfigurationItems(const XmlNode *document)
{
	// Get the options listed under Setup
	const XmlNodeMap& children = document->getChildren().getNode("Setup")->getChildren();

	// Set debugging on/off
	try
	{
		const std::string optDebug = children.getNode("Debug")->getValue();
		if(optDebug == "On" || optDebug == "on")
		{
			useDebugging = true;
			SetDebug(true);
			LogDaemon::out("LogDaemon::Initialize: debugging is 'On'");
		}
	}
	catch(XmlException &err)
	{
		LogDaemon::out("LogDaemon::Initialize: couldn't parse Debug option from config file.\n"
					   "LogDaemon::Initialize: defaulting Debug option to 'false'");
		SetDebug(false);
	}

	// Get log dir path
	try
	{
		const std::string optPath = children.getNode("Path")->getValue();
		if(!optPath.empty())
		{
			logBasePath = optPath;
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("LogDaemon::Initilaize: Couldn't parse log Path option, defaulting to %s",
					logBasePath.c_str());
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logBasePath=%s", logBasePath.c_str());
	}

	// Get StartFile name
	try
	{
		const std::string optStartFile = children.getNode("StartFile")->getValue();
		if(!optStartFile.empty())
		{
			startFileName = optStartFile;
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse StartFile option, defaulting to %s", startFileName.c_str());
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: startFileName=%s", startFileName.c_str());
	}

	// Get StartFileMaxSize
	try
	{
		const std::string optMaxSize = children.getNode("StartFileMaxSize")->getValue();
		if(!optMaxSize.empty())
		{
			startFileMaxSize = atoi(optMaxSize.c_str());
			if(startFileMaxSize == 0)
			{
				// This is the "Default Value".
				// atoi() returns a 0 if it can't translate the value from
				// a string to an integer.  Current documentation in config
				// files specifies that "0" can be used to indicate that the
				// start log should not be rotated, but this code here
				// will not allow a "0" value, so rotating will always be
				// enabled.  Perhaps this "turn off rotation" value should be
				// something else, like -1
				startFileMaxSize = LOG_START_FILE_MAX_SIZE;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse StartFileMaxSize option, defaulting to %d",
					startFileMaxSize);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: startFileMaxSize=%d", startFileMaxSize);
	}

	// Get StartFileMaxFiles
	try
	{
		const std::string optMaxFiles = children.getNode("StartFileMaxFiles")->getValue();
		if(!optMaxFiles.empty())
		{
			startFileMaxFiles = atoi(optMaxFiles.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(startFileMaxFiles == 0)
			{
				startFileMaxFiles = LOG_START_FILE_MAX_FILES;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse StartFileMaxFiles option, defaulting to %d",
					startFileMaxFiles);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: startFileMaxFiles=%d", startFileMaxFiles);
	}

	// Get StandardFile name
	try
	{
		const std::string optStdFile = children.getNode("StandardFile")->getValue();
		if(!optStdFile.empty())
		{
			stdFileName = optStdFile;
		}
	}
	catch(XmlException &err)
	{
		LogDaemon::out("Couldn't parse StandardFile option, defaulting to %s", stdFileName.c_str());
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: stdFileName=", stdFileName.c_str());
	}

	// Get StandardFileMaxSize
	try
	{
		const std::string optMaxSize = children.getNode("StandardFileMaxSize")->getValue();
		if(!optMaxSize.empty())
		{
			stdFileMaxSize = atoi(optMaxSize.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(stdFileMaxSize == 0)
			{
				stdFileMaxSize = LOG_STD_FILE_MAX_SIZE;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse StandardFileMaxSize option, defaulting to %d",
				stdFileMaxSize);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: stdFileMaxSize=%d", stdFileMaxSize);
	}

	// Get StandardFileMaxFiles
	try
	{
		const std::string optMaxFiles = children.getNode("StandardFileMaxFiles")->getValue();
		if(!optMaxFiles.empty())
		{
			stdFileMaxFiles = atoi(optMaxFiles.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(stdFileMaxFiles == 0)
			{
				stdFileMaxFiles = LOG_STD_FILE_MAX_FILES;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse StandardFileMaxFiles option, defaulting to %d",
				stdFileMaxFiles);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: stdFileMaxFiles=%d", stdFileMaxFiles);
	}

	// Get priority of message receiver thread
	try
	{
		const std::string optRecPriority = children.getNode("ReceiverPriority")->getValue();
		if(!optRecPriority.empty())
		{
			receiverPriority = atoi(optRecPriority.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(receiverPriority == 0)
			{
				receiverPriority = LOG_RECEIVER_PRIORITY;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse ReceiverPriority option, defaulting to %d",
				receiverPriority);;
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: receiverPriority=%d", receiverPriority);
	}

	// Get priority of LogWriter thread
	try
	{
		const std::string optWriPriority = children.getNode("LogWriterPriority")->getValue();
		if(!optWriPriority.empty())
		{
			logWriterPriority = atoi(optWriPriority.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(logWriterPriority == 0)
			{
				logWriterPriority = LOG_WRITER_PRIORITY;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogWriterPriority option, defaulting to %d",
					logWriterPriority);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logWriterPriority=%d", logWriterPriority);
	}


	// Get priority of LogRotator thread
	try
	{
		const std::string optRotPriority = children.getNode("LogRotatorPriority")->getValue();
		if(!optRotPriority.empty())
		{
			logRotatorPriority = atoi(optRotPriority.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(logRotatorPriority == 0)
			{
				logRotatorPriority = LOG_ROTATOR_PRIORITY;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogRotatorPriority option, defaulting to %d",
					logRotatorPriority);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logRotatorPriority=%d", logRotatorPriority);
	}

	// Get the wake-up period of the LogRotator
	try
	{
		const std::string optRotPeriod = children.getNode("LogRotatorPeriod")->getValue();
		if(!optRotPeriod.empty())
		{
			logRotatorPeriod = atoi(optRotPeriod.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(logRotatorPeriod == 0)
			{
				logRotatorPeriod = LOG_ROTATOR_PERIOD;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogRotatorPeriod option, defaulting to %d",
					logRotatorPeriod);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logRotatorPeriod=%d", logRotatorPeriod);
	}

	// Get the maximum number of delays for the LogRotator
	try
	{
		const std::string optRotMaxDelays = children.getNode("LogRotatorMaxDelays")->getValue();
		if(!optRotMaxDelays.empty())
		{
			logRotatorMaxDelays = atoi(optRotMaxDelays.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(logRotatorMaxDelays == 0)
			{
				logRotatorMaxDelays = LOG_ROTATOR_MAX_DELAYS;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogRotatorMaxDelays option, defaulting to %d",
				logRotatorMaxDelays);
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logRotatorMaxDelays=%d", logRotatorMaxDelays);
	}

	// Get the tag to tell the LogRotator to not Rotate
	try
	{
		const std::string optRotTag = children.getNode("LogRotatorWaitTag")->getValue();
		if(!optRotTag.empty())
		{
			logRotatorTag = optRotTag;
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogRotatorWaitTag option, defaulting to %s",
				logRotatorTag.c_str());
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logRotatorTag=%s", logRotatorTag.c_str());
	}

	// Get the value to look for in the tag to tell the LogRotator to not Rotate
	try
	{
		const std::string optRotValue = children.getNode("LogRotatorWaitValue")->getValue();
		if(!optRotValue.empty())
		{
			logRotatorValue = optRotValue;
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse LogRotatorWaitValue option, defaulting to %s",
				logRotatorValue.c_str());;
		}
	}
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Initialize: logRotatorValue=%s", logRotatorValue.c_str());
	}

	// Get the message queue size
	try
	{
		const std::string optQueueSize = children.getNode("MessageQueueSize")->getValue();
		if(!optQueueSize.empty())
		{
			msgQueueSize = atoi(optQueueSize.c_str());
			// Since atoi defaults to 0 if it can't translate the data
			// we will set this value back to it's default if atoi returns
			// a zero
			if(msgQueueSize == 0)
			{
				msgQueueSize = LOG_MSG_QUEUE_SIZE;
			}
		}
	}
	catch(XmlException &err)
	{
		if(IsDebugOn())
		{
			LogDaemon::out("Couldn't parse MessageQueueSize option, defaulting to %d",
				msgQueueSize);
		}
	}
}

void LogDaemon::RunInit()
{
	// Build the message queue
	queue = new LogMsgQueue( msgQueueSize );
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Run(): built LogMsgQueue [%d]", msgQueueSize);
	}

	// Build the log file mapping
	fileMap = new LogFileMap();

	// Ensure that the auxilary log is open
	auxLog.Open();
	// Add the auxilary log to the filemap, so it will be rotated by LogRotator
	// along with all the other additional log files.
	fileMap->Put(auxFileName, &auxLog);

	std::string logDir(getenv("FTP_ROOT"));
	logDir.append( logBasePath );
	fileMap->SetLogDir(logDir);

	// Construct the core log file
	coreLog = new CoreLogFile(
		logDir,
		startFileName,
		stdFileName,
		startFileMaxSize,
		startFileMaxFiles,
		stdFileMaxSize,
		stdFileMaxFiles);

	coreLog->SetDebugging(IsDebugOn());

	// Open the core log
	int stat = coreLog->Open();
	if(stat != 0)
	{
		// failed
		LogDaemon::out("LogDaemon::Run(): couldn't open core log file! [%s]", coreLog->GetFileName().c_str());
	}
	else
	{
		// successful open
		if(IsDebugOn())
		{
			LogDaemon::out("LogDaemon::Run(): Created CoreLogFile, logDir="
				"%s.  StartFile=%s Basename=%s", logDir.c_str(),
				startFileName.c_str(), stdFileName.c_str());
		}
		fileMap->SetCoreLog(coreLog);
	}
}


/**
 * Main life-thread of the LogDaemon.
 * Here is where most major components are created, configured, and launched.
 */
void LogDaemon::Run()
{
	pthread_t writerThread;	// LogWriter worker thread
	pthread_t rotatorThread; // LogRotator worker thread

	// Call the RunInit method that builds the CoreLogFile, LogMsgQueue
	// and FileMap
	RunInit();

	// Build the LogWriter
	LogWriter logger(queue, fileMap);
	logger.SetDebugging(IsDebugOn());

	m_taskMon.ReportUp("LogDaemon", TASKMON_CLASS_CORE);

	// Launch the LogWriter in its own thread
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Run(): launching LogWriter...");
	}
	pthread_create(&writerThread, NULL, LogWriter::Launch, &logger);
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Run(): LogWriter launched.");
	}

	// Build the log rotator
	LogRotator rotator(fileMap);
	rotator.SetPeriod( logRotatorPeriod );
	rotator.SetDebugging(IsDebugOn());
	rotator.SetLogWriter( &logger );
	rotator.SetDelayOn( logRotatorTag, logRotatorValue, logRotatorMaxDelays);

	// Spin the log rotator in its own thread:
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Run(): launching LogRotator...");
	}
	pthread_create(&rotatorThread, NULL, LogRotator::Launch, &rotator);
	if(IsDebugOn())
	{
		LogDaemon::out("LogDaemon::Run(): LogRotator launched.");
	}

	// Call the main message receptor loop method
 	MessageReceptorLoop();

	m_taskMon.ReportDone();
}

void LogDaemon::MessageReceptorLoop()
{
	pthread_attr_t	attrs;

	if(queue == NULL)
	{
		// Make sure that the log msg queue isn't null
		return;
	}

	//
	// MESSAGE RECEPTOR LOOP:
	//

	// Start listening for messages
	IInterProcessCommunication ipc(DATA_LOGGER_NAME, "Server",false,LOGGER_BUFFER_SIZE);
	std::string incoming;
	bool alive = true;
    XmlParser parser;
    XmlNode *document;

	while(alive)
	{
		int replyId=0;
		// read message from client (and make sure big enough for command ID and date)
		if( (replyId=ipc.Read(incoming)) >= 0)
		{
			// If we received a pulse
			if( replyId == 0)
			{
				// If it was a terminate pulse
				if( ipc.GetPulseCode() == SYSTEM_PULSE)
				{
					switch(ipc.GetPulseValue())
					{
					case TERMINATE_PULSE:
    					/**
						 * Create a thread here to avoid a deadlock with TaskMon
						 */
						pthread_attr_init( &attrs);
						pthread_attr_setdetachstate( &attrs, PTHREAD_CREATE_DETACHED);
						pthread_create( NULL, &attrs, TaskMonTerminateThread, &m_taskMon);
					// See ya
					out( __FILE__" received TERMINATE pulse\n");
					alive = false;
						break;
					case TASK_MON_PULSE:
						/**
						 * Create a thread here to avoid a deadlock with TaskMon
						 */
						pthread_attr_init( &attrs);
						pthread_attr_setdetachstate( &attrs, PTHREAD_CREATE_DETACHED);
						pthread_create( NULL, &attrs, TaskMonRegisterThread, &m_taskMon);
						out( __FILE__" received TASK_MON_PULSE pulse\n");
						break;
                    case RELOAD_PULSE:
                        document = const_cast<XmlNode*>(parser.ReturnXMLDocument(GetConfigFile()));
                        // Load the additional configuration items needed by the
                        // log daemon.
                        LoadAdditionalConfigurationItems(document);
                        out( __FILE__" received RELOAD_PULSE\n");
                        break;
					default:
						out( __FILE__" received pulse: %d,%d\n", ipc.GetPulseCode(), ipc.GetPulseValue());
						break;
					}
				}
				else
				{
					out( __FILE__" received pulse: %d,%d\n", ipc.GetPulseCode(), ipc.GetPulseValue());
				}
			}
			else if( incoming.length() > 7)
			{
				// send reply
				ipc.Reply("Success");

				// Construct a new message
				LogMessage* newMesg = new LogMessage();
				// Parse the incoming string, populate the message object
				ParseLogMessage(newMesg, incoming);

				// Enqueue the msg.
				// NOTE: We expected the consumer end to call delete() on message objects.
				queue->Enqueue(newMesg);
			}
			else
			{
				out( __FILE__" received short message (%d bytes): \n\t", incoming.length());
				for(unsigned int ii=0; ii<incoming.length(); ii++)
				{
					out( "$%02X ", incoming[ ii]);
				}
				out( "\n");
				ipc.Reply( "Invalid");
			}
		}
	}
}

/**
 * Parse a string message into a LogMessage object
 * @param mesgObj The LogMessage object reference that will be populated
 * @param mesgString The message string to parse.
 */
void LogDaemon::ParseLogMessage(LogMessage* mesgObj, std::string &mesgString)
{
	const char* cstr = mesgString.c_str();
	int size = mesgString.size();

	try
	{
		// Parse the command code
		UINT32 cmd = 0;
		sscanf(cstr, "%08x", &cmd);
		mesgObj->SetCommand(cmd);

		// Extract the time string (it's exactly 21 chars long)
		mesgObj->SetTimestamp( mesgString.substr(8,21) );

		// First char after timestamp is 29, which should be a delimiter
		int pos = 29;
		char delim[2]; // Need a cstr to use string.find()
		delim[0] = cstr[pos];
		delim[1] = 0;

		// Extract the "Who" field
		pos++;
		int pos2 = mesgString.find(delim, pos);
		mesgObj->SetWho( mesgString.substr(pos, pos2-pos) );

		// Extract the message text
		pos = pos2 + 1;
		pos2 = mesgString.find(delim, pos);
		mesgObj->SetMessage( mesgString.substr(pos, pos2-pos) );

		// Extract the file (from current delim to end of file)
		pos = pos2 + 1;
		if(pos < size)
		{
			mesgObj->SetFile( mesgString.substr(pos, size-pos) );
		}
	}
	catch ( ... )
	{
		// Something bad happened.
		// It might be worth noting that this block can never be reached,
		// because nothing in the try{} block can possibly throw an exception
		// of any kind, ever.
		// Oh well.
		// 	-- Actually, 'new' throws on allocation errors, but by then
		// you're screwed anyway...
		if( cstr != NULL)
		{
			out( "\n\n\tBOO-YAH Caught exception parsing message <%s> (%d bytes)\n", cstr, size);
			// ...BOO-YAH?
		}
		else
		{
			out( "\n\n\tBOO-YAH Caught exception parsing message <> (0 bytes)\n");
		}

		// Make sure the log message is good enough to send along.
		// If a field is missing, set something meaningful into it.

		// Set command to ROLL_LOG_STRING
		mesgObj->SetCommand(ROLL_LOG_STRING);
		if (mesgObj->GetTimestamp().empty())
		{
			char tsBuf[23];
			std::string tsString = ILogger::GetTimeString(tsBuf);
			mesgObj->SetTimestamp(tsString);
		}
		// Check the "who" field
		if (mesgObj->GetWho().empty())
		{
			// Set something visible
			std::string myWho = "Unknown";
			mesgObj->SetWho(myWho);
		}
		// Check the "message" field
		if (mesgObj->GetMessage().empty())
		{
			// Set something visible
			std::string myMesg = "(Lost message due to parse error)";
			mesgObj->SetMessage(myMesg);
		}
		// The "file" field is unset, that's ok.
		// If it IS set, it's probably right.
	}
}

/** Log something to the special auxilary log. */
void LogDaemon::out(const char *fmt, ...)
{
	char message[4096];
	va_list vaArgs;

	// clear the text message
	memset (message, 0, sizeof(message));

	va_start(vaArgs, fmt);
	// process the format, with a max size (to prevent segfaults on long msgs)
	vsnprintf(message, sizeof(message)-1, fmt, vaArgs);
	va_end(vaArgs);

	auxLog.WriteLine(message);
	auxLog.Flush();

	//cout << message << endl;
}

void LogDaemon::SetConfigFile(std::string fileName)
{
    m_configFile = fileName;
}

std::string LogDaemon::GetConfigFile()
{
    return(m_configFile);
}
