//****************************************************************************
// Description:
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogWriter.cpp $
// 
// 5     10/30/07 4:13p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     10/26/06 11:21a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the CCRT software version string to the log file.
// 
// 3     5/08/06 7:42p Cward
// added slash between base name and filename
//
// 2     6/24/04 1:14a Cward
// Removed pound if 0 block. Updated the header block comments to the new
// standard.
//
// 1     12/21/03 6:41p Cward
//    Revision 1.10  2003/10/07 21:02:48  swieton
//    Added needed .c_str()
//
//    Revision 1.9  2003/10/03 17:13:47  crosby
//    Added debug msg when an additional log file is requested that is not opened.
//
//    Revision 1.8  2003/10/03 16:04:10  karlin
//    privLogFilename --> auxFileName
//
//    Revision 1.7  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.6  2003/10/02 22:03:47  swieton
//    Added safety check to prevent deallocation of internal log
//
//    Revision 1.5  2003/10/02 19:35:13  crosby
//    checking mutex lock/unlock return status
//
//    Revision 1.4  2003/10/02 17:55:14  crosby
//    Edited a comment
//
//    Revision 1.3  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.34  2003/08/01 01:00:38  crosby
//    temp dev debugging code
//
//    Revision 1.33  2003/07/25 18:13:29  swieton
//    added tests for LogWriter
//
//    Revision 1.32  2003/07/25 17:52:18  crosby
//    When getting message destined for additional log X, do not bitch.
//
//    Revision 1.31  2003/07/23 20:22:23  swieton
//    logwriter tests
//
//    Revision 1.30  2003/07/23 14:09:49  crosby
//    Setting thread priority in Run()
//
//    Revision 1.29  2003/07/23 14:02:52  crosby
//    Accepting thread priority in Run()
//
//    Revision 1.28  2003/07/22 23:38:40  crosby
//    Delimiter was not being correctly
//
//    Revision 1.27  2003/07/22 17:39:23  karlin
//    dealt with TODOs and FIXMEs
//
//    Revision 1.26  2003/07/21 22:36:57  swieton
//    All ActivateLog errors now set the message
//
//    Revision 1.25  2003/07/21 17:37:04  crosby
//    Tweaked a debug msg.
//
//    Revision 1.24  2003/07/21 15:39:48  crosby
//    Cleaning up the error message / debug message output
//
//    Revision 1.23  2003/07/21 14:57:31  crosby
//    Fixed up FormatMessage, using fixed width 20 chars for who filed
//
//    Revision 1.22  2003/07/18 21:11:55  crosby
//    Removed obsolete call from constructor
//
//    Revision 1.21  2003/07/18 20:58:54  crosby
//    *** empty log message ***
//
//    Revision 1.20  2003/07/18 20:22:06  crosby
//    Set debug on/off for new LogFile instances
//
//    Revision 1.19  2003/07/18 19:58:45  karlin
//    added PrivilegedLogger
//
//    Revision 1.18  2003/07/18 19:16:18  crosby
//    *** empty log message ***
//
//    Revision 1.17  2003/07/18 17:49:42  karlin
//    removed spurious couts
//
//    Revision 1.16  2003/07/18 16:35:11  crosby
//    Blanking File field in message after deactivating the additional log file
//
//    Revision 1.15  2003/07/18 15:59:06  karlin
//    fixed removal and fork() bug
//
//    Revision 1.14  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.13  2003/07/17 20:41:13  crosby
//    Calling RotateIfNeeded() on CoreLogFile
//
//    Revision 1.12  2003/07/17 16:19:04  crosby
//    Name space issue IncrementCoreLogFile()
//
//    Revision 1.11  2003/07/17 16:07:32  karlin
//    added rotateIfNeeded to IncrementCoreLogFile
//
//    Revision 1.10  2003/07/17 16:07:08  crosby
//    Tabs in switch block
//
//    Revision 1.9  2003/07/17 15:57:55  karlin
//    added IncrementCoreLogFile
//
//    Revision 1.8  2003/07/17 15:34:04  crosby
//    Changed some [] to ()
//
//    Revision 1.7  2003/07/16 23:26:04  crosby
//    Preventing normal log messages from being treated as unrecognized
//
//    Revision 1.6  2003/07/16 21:41:40  crosby
//    Added ProcessUnknownCommand()
//
//    Revision 1.5  2003/07/16 21:27:19  crosby
//    Added activate/deactivate handlers for additional log files.
//
//    Revision 1.4  2003/07/14 19:25:08  crosby
//    Changed *other* Write() to WriteLine()
//
//    Revision 1.3  2003/07/14 15:58:17  crosby
//    Changed Write() to WriteLine()
//
//    Revision 1.2  2003/07/11 19:46:34  crosby
//    First stab at using LogFileMap and CoreLogFile objects.
//    Added FormatMessage.
//
//    Revision 1.1  2003/07/10 22:53:47  crosby
//    Threadable; just writing messages to stdout
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "LogDaemon.h"
#include "LogWriter.h"

/**
 * Create a new LogWriter
 */
LogWriter::LogWriter(LogMsgQueue *q, LogFileMap *fm)
{
    queue = q;
    fileMap = fm;
}

/**
 * Static launcher function.  This is the entry point for the
 * worker thread spawned by LogDaemon.
 * @param myLogWriter The LogWriter instance to call Run() on.
 */
void* LogWriter::Launch(void* myLogWriter)
{
    ((LogWriter*)myLogWriter)->Run();
    return NULL;
}


/**
 * Start logging.  Pull messages from the queue and send them to the 
 * appropriate log.  Consumed messages MUST BE DESTROYED when done being used.
 */
void LogWriter::Run()
{
    while(true)
    {
        if(queue == NULL)
        {
            LogDaemon::out("LogWriter: MESSAGE QUEUE WAS NULL; CANNOT RUN!!!\n"
                    "LogWriter exiting.");
            break;
        }
        // Get a message object
        LogMessage* mesg = queue->Dequeue();
        if(mesg == NULL)
        {
            // How odd. But possible. (Eg, mutex lock failure in the queue)
            // Let's not attempt to process.
            if(IsDebugOn())
            {
                LogDaemon::out("LogWriter: got a NULL off the message queue. Ignoring.");
            }
            continue;
        }

        RouteMessage(mesg);
        ProcessNormalMessage(mesg);

        // Destroy message object
        delete(mesg);
    }
}

/** Find out what kind of command mesg is and make sure the correct function is
 * called */
void LogWriter::RouteMessage (LogMessage *mesg)
{
    // Handle commands from client.
    // Some handlers may modify the message contents for
    // the benefit of outputting them in the logs, after the command
    // have been processed.
    switch (mesg->GetCommand()) {
        case ROLL_LOG_STRING:
            // do nothing special, we'll log this in a moment
            break;
        case ROLL_ACTIVATE_SPECIAL_LOG:
            ActivateLogFile(mesg);
            break;
        case ROLL_DEACTIVATE_SPECIAL_LOG:
            DeActivateLogFile(mesg);
            break;
        case ROLL_NEW_TEST_NUMBER:
            IncrementCoreLogFile(mesg);
            break;
        default:
            ProcessUnknownCommand(mesg);
            break;
    }
}

/**
 * Process a normal log message.
 */
void LogWriter::ProcessNormalMessage(LogMessage *mesg) {

    // NOTE: ?? does this method need to be wrapped in a mutex lock ??

    // Format the output line
    std::string outString;
    FormatMessage(*mesg, outString);

    if(IsDebugOn())
    {
        LogDaemon::out("LogWriter::ProcesNormalMessage: outString=[%s]",
                outString.c_str());
    }

    // Log to the core log
    CoreLogFile* coreLog = fileMap->GetCoreLog();
    if(coreLog != NULL)
    {
        coreLog->WriteLine(outString);
    }
    else
    {
        LogDaemon::out("LogWriter::ProcessNormalMessage: core log was null!"
            "Cannot log to it!");
    }

    // Get the name of the additional log file
    std::string file = mesg->GetFile();
    if(file != "")
    {
        // We have an additional log
        LogFile* additional = fileMap->Get(file);
        if(additional != NULL)
        {
            additional->WriteLine( outString );
        }
        else
        {
            // Requested additional log that has not yet been opened.
            // Let's not be whiney about this.
            // TODO: implicit opening of additional log w/ default rotation params
            // TODO: ... and log to core (and additional), mention this anomally

            LogDaemon::out(
                    "LogWriter::ProcessNormalMessage: Additional file [%s] requested but was never open",
                    file.c_str());
        }
    }
}


/**
 * Handle ROLL_ACTIVATE_SPECIAL_LOG command.
 * The message body is expected to contain a space-separated pair
 * of int values that represent file rotation params: "<maxsize> <maxfiles>".
 * Message body will be modified to reflect what happens here.
 * If a LogFile already exists, it will not be recreated.
 *
 * LogFile is created, opened, and added to the LogFileMap.
 * @return 0 on success, 1 if couldn't open file.
 */
int LogWriter::ActivateLogFile(LogMessage* mesg)
{
    std::string fileName = mesg->GetFile();
    if(fileName.empty())
    {
        mesg->SetMessage("Could not activate addditional log file; file name was blank");
        return -1;
    }
    LogFile* logFile = fileMap->Get( fileName );
    if(logFile != NULL)
    {
        // Increment this LogFile object's reference count
        logFile->AddRef();

        // The log file is already active
        std::string newMsg("Additional log [");
        newMsg.append(fileName);
        newMsg.append("] already activated.");
        mesg->SetMessage(newMsg);
    }
    else
    {
        // Extract rotation params.  Format: "<maxsize> <maxfiles>"
        std::string pString = mesg->GetMessage();
        int space = pString.find(" ");
        string sizeStr = pString.substr(0, space);
        string filesStr = pString.substr(space+1);
        int size = atoi(sizeStr.c_str());
        int files = atoi(filesStr.c_str());

        // Create new log file, open it, add it to the file map
        std::string base = fileMap->GetLogDir();
        logFile = new LogFile(base, fileName, size, files);
        logFile->SetDebugging( IsDebugOn() );
        // Open it...
        if(logFile->Open() != 0)
        {
            // Error
            std::string newMsg("Failed to open additional log [");
            newMsg.append(base);
            newMsg.append("/");
            newMsg.append(fileName);
            newMsg.append("]");
            mesg->SetMessage (newMsg);
            delete(logFile);
            return -1;
        }
        // Add the file to the map
        fileMap->Put(fileName, logFile);

        // Set the log message
        std::string newMsg("Activated additional log [");
        newMsg.append(base);
        newMsg.append("/");
        newMsg.append(fileName);
        newMsg.append("] with rotation params: maxSize=");
        char numBuf[12];
        sprintf(numBuf, "%d", size);
        newMsg.append(numBuf);
        newMsg.append(", maxFiles=");
        sprintf(numBuf, "%d", files);
        newMsg.append(numBuf);
        mesg->SetMessage(newMsg);
    }
    return 0;
}

/**
 * Deactivate additional log file (ROLL_DEACTIVATE_SPECIAL_LOG command).
 * LogFile will be removed from LogFileMap, closed, and deleted.
 */
int LogWriter::DeActivateLogFile(LogMessage* mesg)
{
    std::string fileName = mesg->GetFile();
    std::string newMsg;

    if(fileName.empty())
    {
        mesg->SetMessage("Could not de-activate log file; file name was blank");
        return -1;
    } else if (fileName == LogDaemon::auxFileName)
    {
        /* privLogFilename is a magic name that refers to the logger's internal
         * log file. This file is allocated statically, so we must make sure
         * that we never deactivate it, because that would call delete on it
         * and crash the logger */
        return 0;
    }

    LogFile* logFile = fileMap->Get( fileName );
    if( logFile != NULL)
    {
        // Decrement this log file object's reference count
        //  Only remove the log file object from the file map
        //  when no more references to it exist
        if( logFile->DelRef() == 0)
        {
            fileMap->Remove(fileName);
            delete(logFile);
            // modify the the outgoing message to reflect our action
            newMsg = std::string("De-activated additional log file [");
            newMsg.append(fileName);
            newMsg.append("]...no more refrences exist");
        }
        else
        {
            // modify the the outgoing message to reflect our action
            newMsg = std::string("De-activated additional log file [");
            newMsg.append(fileName);
            newMsg.append("]...more references exist");
        }
    }
    else
    {
        // modify the the outgoing message to reflect our action
        newMsg = std::string("Attempt to de-activate NULL additional log file [");
        newMsg.append(fileName);
        newMsg.append("]");
    }

    mesg->SetMessage(newMsg);
    // Since this log file has been removed, blank the File field in the
    // message to avoid attempting to log this message to additional log file.
    mesg->SetFile("");

    return 0;
}

/**
 * We started a new test, so tell our CoreLogFile to increment
 * it's number and start a new file.  Rotate it if needed.
 */
int LogWriter::IncrementCoreLogFile(LogMessage* mesg)
{
    std::string suffix = mesg->GetMessage();
    if(suffix.empty())
    {
        mesg->SetMessage("Could not increment Core Log: new number was empty string");
        return -1;
    }

    fileMap->GetCoreLog()->IncrementFileName(suffix);

    // modify the the outgoing message to reflect our action
    std::string newMsg("Incremented core log file [");
    newMsg.append(suffix);
    newMsg.append("]");
    newMsg.append( std::string(", software version: ") + std::string( CCRT_VERSION_STR));

    mesg->SetMessage(newMsg);

    return 0;
}


/**
 * This is what happens when we get an unrecognized command.
 * Try to construct a meaningful message to send to the log.
 */
void LogWriter::ProcessUnknownCommand(LogMessage* mesg)
{
    std::string oldMsg = mesg->GetMessage();
    std::string obsoleteCmd;
    std::string newMsg;

    switch(mesg->GetCommand())
    {
    case ROLL_LOG_STRING:
        // nothing special
        break;
    case ROLL_LOAD_CONFIGURATION:
        obsoleteCmd = "ROLL_LOAD_CONFIGURATION";
        break;
    case ROLL_DEACTIVATE_ALL_SPECIAL_LOG:
        obsoleteCmd = "ROLL_DEACTIVATE_ALL_SPECIAL_LOG";
        break;
    case ROLL_LOG_READ_NEXT:
        obsoleteCmd = "ROLL_LOG_READ_NEXT";
        break;
    case ROLL_LOG_READ_CLOSE:
        obsoleteCmd = "ROLL_LOG_READ_CLOSE";
        break;
    case ROLL_DITTO_PROCESS:
        obsoleteCmd = "ROLL_DITTO_PROCESS";
        break;
    }

    if(obsoleteCmd != "")
    {
        // Old, obsoleted command was sent
        //newMsg = "The [] command is obsolete. (Orig mesg: )";
        newMsg = "The [";
        newMsg.append(obsoleteCmd);
        newMsg.append("] command is obsolete. (Orig mesg: ");
        newMsg.append(oldMsg);
        newMsg.append(")");
    }
    else
    {
        newMsg = "Unrecognized command [";
        char numBuf[12];
        sprintf(numBuf, "%d", mesg->GetCommand());
        newMsg.append(numBuf);
        newMsg.append("] with message (");
        newMsg.append(oldMsg);
        newMsg.append(")");
    }
    // Set the log message body for output
    mesg->SetMessage(newMsg);
}

/**
 * Convert a LogMessage into a string ready to be written to
 * a log file. No newline or linefeed character is appended.
 */
std::string LogWriter::FormatMessage(const LogMessage& mesg, std::string &ret)
{
//  std::string ret;
    // compose a delimiter character as a 1-length string
    char delim[2];
    delim[0] = (char)0x80;
    delim[1] = (char)0;
    //sprintf(delim, "%c", mesg.GetDelimiter());
    // Start with delimiter
    ret.append(delim);
    // Add timestamp (8 + 1 + 12 = 20)
    ret.append(mesg.GetTimestamp());
    ret.append(" ");
    // Get 'who'
    std::string who = mesg.GetWho();
    // Ensure precise length of 30, either by truncating or appending spaces.
    who.resize(30, ' ');// = who.substr(0,30);
    who.append( " ");   // Make sure therre is at least 1 space after 'who'
    ret.append(who);
    // Append message
    ret.append(mesg.GetMessage());
    // closing delimiter
    ret.append(delim);
    // done
    return ret;
}


/** Activate debugging */
void LogWriter::SetDebugging(bool t)
{
    SetDebug(t);
}
