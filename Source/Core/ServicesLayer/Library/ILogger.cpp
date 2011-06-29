//******************************************************************************
// Description:
//
//    Logging interface class.
//
//============================================================================
//
//    Copyright (c) 1999-2007 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/ILogger.cpp $
// 
// 5     2/22/07 11:03a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a separator between the methods.
// 
// 4     2/15/06 7:50p Cward
// Updated to to not echo 0 length messages, also beautified, and updated
// module header comments
//
// 3     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 3     10/24/03 12:12p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 2     10/10/03 9:33a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/07/03 2:13p Khykin
// Updated with changes from Flatrock.
//    Revision 1.19  2003/08/01 00:28:27  crosby
//    fix for memory leak
//
//    Revision 1.18  2003/07/25 17:42:25  crosby
//    Merging in Bill's flatrock changes.
//
//    Revision 1.17  2003/07/24 01:40:24  crosby
//    Attempting to make ILogger robust in the face of an absent LogDaemon
//
//    Revision 1.15  2003/07/22 21:28:39  karlin
//    fixed define DEFAULT_TIMEOUT
//
//    Revision 1.14  2003/07/22 17:39:23  karlin
//    dealt with TODOs and FIXMEs
//
//    Revision 1.13  2003/07/21 22:43:36  crosby
//    accepting new params via XML:
//      MessageTimeout
//      AdditionalLogMaxFiles
//      AdditionalLogMaxSize
//
//    Revision 1.12  2003/07/17 23:41:20  crosby
//    Using LOGGER_BUFFER_SIZE for IInterProcessComm buffer
//
//    Revision 1.11  2003/07/17 18:34:29  crosby
//    Fixed "empty" to "empty()"
//
//    Revision 1.10  2003/07/17 15:58:27  karlin
//    swapped in a empty() call
//
//    Revision 1.9  2003/07/16 23:27:31  crosby
//    hard coded default rotation params
//
//    Revision 1.8  2003/07/16 21:50:13  crosby
//    renamed ivars yet again
//
//    Revision 1.7  2003/07/16 21:47:59  crosby
//    Changed case of maxNumFiles and maxFileSize ivars
//
//    Revision 1.6  2003/07/16 21:28:55  karlin
//    added rotation parms to additional log messages
//
//    Revision 1.5  2003/07/15 16:57:21  crosby
//    Now using IInterProcessCommunication.
//    Now using new string-based message format. (Unbounded length.)
//    Timestamping done on client side.
//
//    Revision 1.4  2003/07/14 22:59:29  crosby
//    Starting the addition of methods/ivars to support
//    IInterProcessCommunnication-based messaging, using new string format
//    for msgs.
//    INCOMPLETE
//
//    Revision 1.3  2003/07/14 22:18:22  crosby
//    pseudo code for SendToDaemon
//
//    Revision 1.2  2003/06/18 13:11:48  tenharmsel
//    Syncing directory trees with the FordChicago system
//
//
// 1     6/12/03 4:03p Khykin
// Initial checkin.
//
// 2     5/19/03 12:24p Khykin
// Update for local echo timestamps to look like logging timestamps.
//
// 1     4/22/03 9:48a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:56  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:13a Khykin
// Initial Checkin
//
// 1     1/18/03 8:09a Khykin
// Initial Checkin From DCTC
//
// 35    10/10/02 10:24a Khykin
// Added debugging information.
//
// 34    8/23/02 8:36a Kh
//
// 33    4/08/02 1:56p Skypig13
// Removed compiler warning on line 1101 by changing format string
//
// 32    4/04/02 3:35a Bmeinke
// Added thread ID to the local echo printout of Logger messages
//
// 31    4/02/02 4:38p Bmeinke
// Added timestamp to local printout of logger messages and moved local
// echo printout after the IPC message is sent to the Logger process
//
// 30    3/14/02 4:23p Bmeinke
// Changed the processing of NULL XML documents in Initialize() to not
// throw an XmlException and to not perform any further processing of the
// document
//
// 29    3/11/02 4:43p Kh
// Updating CommanReloadConfiguration to CommandReloadConfiguratio
//
// 28    1/29/02 10:08a Kh
// Speeding up the Log method for speed.
//
// 27    12/04/01 10:04a Kh
// Updating the method ReLoadConfig to CommandReloadConfig.
//
// 26    11/16/01 9:51a Brian.meinke
// Changed Initialize():
//    Try to read process name first
//    If a particular node does not exist (throwing an XML exception), do
// not set a new value in the catch block: defaults are set in the
// constructor
//
// 25    10/10/01 9:46a Kh
// Fixing a print statement.
//
// 24    10/10/01 9:42a Kh
// Updating the initialization to perform each entry individually.
//
// 23    10/05/01 2:39p Brian.meinke
// Added check for IsDebugOn() before printing error in Initialize if no
// Logging setup node
//
// 22    10/03/01 1:21p Kh
// Updating debug capabilities and adding an exception if a NULL document
// is passed the the Initialize method.
//
// 21    8/28/01 3:52p Brian.meinke
// Added call to name_close in ~ILogger() to release file descriptor used
// for sending data to the logger process
//
// 20    8/15/01 2:56p Kh
// Updating configuration file output exception with fault description.
//
// 19    8/01/01 3:59p Brian.meinke
// Made GetLocalEcho(), GetVerboseMask(), and GetLogStatus() const methods
//
// 18    6/27/01 8:36a Kh
// Fixing logic problem with the "Disable" check.
//
// 17    6/20/01 4:06p Brian.meinke
// Added method GetLocalEcho() to retrieve the local echo status
// Added new Log() method which takes a verbose mask argument
// Added methods SetVerboseMask/GetVerboseMask() to control verbosity for
// log messages
// Added m_verbose data member
//
// 16    6/12/00 8:37a Kh
// Update debug methods for common name usage.
//
// 15    6/11/00 2:21p Kh
// Making the initialize method more robust to take bad config files and
// just disable.
//
// 14    6/07/01 11:51a Brian.meinke
// Added LocalEcho node to Logging Config XML section
// Added processing in Initialize() to handle LocalEcho config
// Changed Initialize() to read Config Nodes as child nodes, not
// attributes
//
// 13    6/07/00 10:34a Kh
// Updating tags.
//
// 12    5/25/00 3:09p Kh
// Updating methods to take object references.
//
// 11    5/16/01 10:12p Brian.meinke
// Removed un-needed pthread_mutex_lock/unlock pairs
// Changed cout's to printfs because printf is thread safe
//
// 10    5/15/00 12:24p Kh
// Making Thread Safe.
//
// 9     4/26/01 10:39a Kh
// Update the destructor to only close the file specified by the
// configuration file.  If any other file was opened, you must close it
// prior to the destruction of the object.
//
// 8     4/25/01 3:15p Kh
// Fixes made after running against the LoggerTest TestCase.
//
// 7     4/20/00 2:52p Kh
// Changing INT8 to char.
//
// 6     4/19/01 3:50p Brian.meinke
// Changed 'caWho[ WHO_SIZE-1] = 0' to 'caTempWho[ WHO_SIZE-1] = 0' in
// Log(caWho, ...)
//
// 5     4/19/01 3:44p Brian.meinke
// Explicitly set localEcho=false in constructor
//
// 4     4/19/01 3:42p Brian.meinke
// Fixed in Log( caWho, caMessage, ...) overwriting caTempWho and
// caTempMessage arrays
//
// 3     4/19/01 1:53p Brian.meinke
// Changed function comments to javadoc style
//
// 2     4/19/00 10:03a Kh
// Added more logging methods that take different types.
//
// 6     3/20/00 1:54p Kevin.hykin
// Adding methods to be able to enable and disable logging
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 1     2/22/01 5:33p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 5     1/11/00 6:38p Hil.sybesma
//
// 4     10/19/99 6:39p Hil.sybesma
//
//  10/12/99 14:15 hilbrand.sybesma
//      - Released
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ROLL_LOGGER_METHOD_CPP
#define ROLL_LOGGER_METHOD_CPP

#include    <sys/time.h>
#include    <netdb.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <errno.h>
#include    <iostream>

#include    <sys/mman.h>
#include    <sys/stat.h>
#include    <sys/sched.h>

#include    <fcntl.h>
#include    <string.h>
#include    <signal.h>
#include    <process.h>
#include    <malloc.h>
#include    <mem.h>
#include    <ctype.h>
#include    <math.h>
#include    <time.h>
#include    <stdarg.h>
#include    <sys/dispatch.h>

#include    "BepDefs.h"         // Burke E. Porter Defines
#include    "XmlNode.h"         // XML node definition

#include    "BepComponent.h"

#include "ILogger.h"
#include "IInterProcessCommunication.h"

#ifdef _NTO_VERSION
    #if (_NTO_VERSION >= 630)
        #define vsnprintf std::vsnprintf
    #endif
#endif

//=============================================================================
ILogger::ILogger()
{
    // Get the PID of the log server
    m_processName = "Blank";        // default the process name to the program name
    m_fileName = "";        // the additional file name to record log to
    m_usingAdditionalLog = false;
    m_logStatus = false;            // turn on logging (for component initialization)
    m_delimiter = DELIMITER_ID;     // delimiter to use for logging messages
    m_localEcho = false;            // echo to the screen
    m_needSendOpen = false;
    SetDebug(false);                // interface debug status
    m_verboseMask = 0;              // No verbosity at first

    // Initialize the mutex
    memset( &m_logAttrMutex, 0, sizeof( m_logAttrMutex));
    memset( &m_logMutex, 0, sizeof( m_logMutex));
    pthread_mutexattr_init(&m_logAttrMutex);
    pthread_mutexattr_settype( &m_logAttrMutex, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setrecursive(&m_logAttrMutex, PTHREAD_RECURSIVE_DISABLE);
    pthread_mutex_init( &m_logMutex, &m_logAttrMutex);

    m_daemonIpc = NULL; // See Initialize()

    m_maxNumFiles = DEFAULT_MAXNUMFILES;  // 10 files
    m_maxFileSize = DEFAULT_MAXFILESIZE; // 1MB max file size

    m_timeout = DEFAULT_IFCTIMEOUT; // default 100 millis timeout
}

//=============================================================================
ILogger::~ILogger()
{   // shut down any special log files
    DeActivateSpecialLogFile();
    // destroy the mutex
    pthread_mutex_destroy( &m_logMutex);

    if(m_daemonIpc != NULL)
    {
        // Close IPC
        m_daemonIpc->Close();
        delete m_daemonIpc;
        m_daemonIpc = NULL;
    }
}

//=============================================================================
void ILogger::Initialize(const XmlNode *document)
{
    // get the LOGGING tag and parse out the setup information
    try
    {
        if(document != NULL)
        {
            const XmlNodeMap& logging = document->getChild("Setup")->getChild("Logging")->getChildren();

            // set up the debug status
            try
            {
                SetDebug(logging.getNode("Debug")->getValue());
            }
            catch(XmlException &e)
            {
            }

            if(IsDebugOn())
                printf("ILogger::Initialize()\n");

            try
            {
                const std::string optTimeout = logging.getNode("MessageTimeout")->getValue();
                if(!optTimeout.empty())
                {
                    m_timeout = atoi(optTimeout.c_str());
                }
            }
            catch(XmlException &e)
            {
                if(IsDebugOn())
                    printf("Couldn't parse MessageTimeout option, defaulting to %ld\n", m_timeout);
            }

            // Parse rotation parameter: max size of log file before rotation
            try
            {
                const std::string optMaxSize =
                    logging.getNode("AdditionalLogMaxSize")->getValue();
                if(!optMaxSize.empty())
                {
                    m_maxFileSize = atoi(optMaxSize.c_str());
                }
            }
            catch(XmlException &e)
            {
                if(IsDebugOn())
                    printf("Couldn't parse AdditionalLogMaxSize option, defaulting to %ld\n",
                           m_maxFileSize);
            }

            // Parse rotation parameter: max number of files to keep
            try
            {
                const std::string optMaxFiles =
                    logging.getNode("AdditionalLogMaxFiles")->getValue();
                if(!optMaxFiles.empty())
                {
                    m_maxNumFiles = atoi(optMaxFiles.c_str());
                }
            }
            catch(XmlException &e)
            {
                if(IsDebugOn())
                    printf("Couldn't parse AdditionalLogMaxFiles option, defaulting to %d\n",
                           m_maxNumFiles);
            }

            if(IsDebugOn())
                printf("ILogger: About to try to connect IPC\n");

            // Fire up IPC
            ConnectIpc();

            if(IsDebugOn())
                printf("ILogger: After call to ConnectIpc\n");

            try
            {
                SetProcessName(logging.getNode("ProcessName")->getValue());     // set the process name
            }
            catch(XmlException &e)
            {
                if(GetProcessName().empty())
                {
                    size_t maximum_path;
                    char *buff;

                    maximum_path = (size_t)pathconf( "/", _PC_PATH_MAX );
                    buff = new char[ maximum_path+1];
                    if(_cmdname( buff ) == (char*)0)
                    {
                        sprintf( buff, "PID %d\n", getpid());
                    }
                    SetProcessName( buff);
                    delete[] buff;
                    if(IsDebugOn()) printf("ProcessName Default: %s\n", GetProcessName().c_str());
                }
            }

            // set up the logger status
            try
            {
                const XmlString &status = logging.getNode("Status")->getValue();
                // if logger has been enabled
                if(( status == "Enabled") || ( status == "Enable") ||
                   ( status == "enabled") || ( status == "enable"))
                {
                    EnableLogger();
                    // check if there is an additional log file to send the logging information to
                    if(logging.getNode("AdditionalLogFile"))
                    {   // retrieve the filename
                        SetAdditionalLogFileName(logging.getNode("AdditionalLogFile")->getValue());
                    }
                    else
                    {
                        SetAdditionalLogFileName(""); // Disable
                    }
                }
            }
            catch(XmlException &e)
            {
            }

            try
            {   // set the local echo status
                const XmlNode *echoNode = logging.getNode( XML_T("LocalEcho"));
                if(echoNode->getValue() == XML_T("Enabled"))
                {
                    SetLocalEcho( true);
                }
                else
                {
                    SetLocalEcho( false);
                }
            }
            catch(XmlException &xmlErr)
            {
            }

            try
            {   // set the verbose status
                const XmlNode *verbosityNode = logging.getNode( XML_T("Verbose"));
                SetVerboseMask(BposReadInt( verbosityNode->getValue().c_str()));
            }
            catch(XmlException &xmlErr)
            {   // if it does not exist, set to full verbosity
                SetVerboseMask(0xFFFF);
            }
        }
    }
    catch(XmlException &xmlErr)
    {
        if(IsDebugOn()) printf("No Logger Configuration: %s\n", xmlErr.Reason().c_str());
        DisableLogger();
        if( GetProcessName().empty())           SetProcessName(std::string("Blank"));
        if( GetAdditionalLogFileName().empty()) SetAdditionalLogFileName("");
        SetDebug(false);
        SetLocalEcho( false);
    }

    if(IsDebugOn())
        printf("Status: %d, Name: %s, Additional File: %s\n", IsDebugOn(),
               GetProcessName().c_str(), GetAdditionalLogFileName().c_str());

    if(IsDebugOn())
        printf("ILogger: Initialize: EXIT\n");
}

//=============================================================================
void ILogger::LogMessage( const char *caMessage )
{
    if(IsDebugOn())
    {
        printf( "Log( %s );\n",  caMessage);
    }

    if(GetLogStatus())
    {
        // send to logging server
        SendToDaemon(ROLL_LOG_STRING, caMessage);
    }
}

//=============================================================================
void ILogger::Log( const std::string &message)
{
    SendToDaemon(ROLL_LOG_STRING, message);
}

//=============================================================================
void ILogger::Log( const char *cpFmt, ...)
{
    if(GetLogStatus() || IsDebugOn())
    {
        char caMessage[MSG_SIZE];
        va_list         vaArgs;

        // parse out the varaible argument message
        // clear the print buffer
        memset(caMessage, 0 , sizeof(caMessage));
        // format the input into a String
        va_start( vaArgs, cpFmt);
        // Limit the size of the message so that it fits into the buffer (else SIGSEGV)
        vsnprintf(caMessage, sizeof(caMessage)-1, (char *)cpFmt, vaArgs);
        // remove left overs
        va_end( vaArgs);

        LogMessage(caMessage);
    }
}

//=============================================================================
void  ILogger::Log( UINT32 mask, const char *cpFmt, ...)
{
    if(((mask & m_verboseMask) && GetLogStatus()) || IsDebugOn())
    {
        char caMessage[MSG_SIZE];
        va_list         vaArgs;

        // parse out the varaible argument message
        // clear the print buffer
        memset(caMessage, 0 , sizeof(caMessage));
        // format the input into a String
        va_start( vaArgs, cpFmt);
        // Limit the size of the message so that it fits into the buffer (else SIGSEGV)
        vsnprintf(caMessage, sizeof(caMessage)-1, (char *)cpFmt, vaArgs);
        // remove left overs
        va_end( vaArgs);

        if((mask & m_verboseMask) && ( GetLogStatus()))
        {
            LogMessage(caMessage);
        }
        else
        {
            printf("%s\n", caMessage);
        }
    }
}

//=============================================================================
void ILogger::CommandReloadConfiguration( void )
{
    SendToDaemon(ROLL_LOAD_CONFIGURATION, "");
}

//=============================================================================
void ILogger::NewTestNumberLog( INT32 iTestNumber )
{
    char caTestNum[12]; // 11 digits for up to -2.1 billion plus null
    sprintf(caTestNum, "%d", iTestNumber);
    // Send it
    SendToDaemon(ROLL_NEW_TEST_NUMBER, caTestNum);
}

//=============================================================================
void ILogger::ActivateSpecialLogFile(void)
{
    // if not using additional log then do nothing
    if(!m_usingAdditionalLog)
    {
        return;
    }

    if(ConnectIpc())
    {
        char buf[25];
        sprintf(buf, "%ld %d", m_maxFileSize, m_maxNumFiles);
        std::string body(buf);
        m_needSendOpen = false;
        SendToDaemon(ROLL_ACTIVATE_SPECIAL_LOG, buf);
        if(m_daemonIpc == NULL)
        {
            // we lost our connection to the server.  Probably, LogDaemon went down
            m_needSendOpen = true;
        }
    }
    else
    {
        // Don't log anything here, because this would show up EVERY time
        // the client tries to log and fails
        m_needSendOpen = true;
    }
}

//=============================================================================
void ILogger::DeActivateSpecialLogFile(void)
{
    SendToDaemon(ROLL_DEACTIVATE_SPECIAL_LOG, "");
}

//=============================================================================
inline void ILogger::SetLocalEcho( bool iEnable)
{
    m_localEcho = iEnable;
}

//=============================================================================
bool ILogger::GetLocalEcho(void) const
{
    return( m_localEcho);
}

//=============================================================================
inline void ILogger::SetProcessName( const std::string &name )
{
    if(name.empty()) m_processName = "Blank";
    else             m_processName = name;
}

//=============================================================================
inline std::string ILogger::GetProcessName( void ) const
{
    return(m_processName);
}

//=============================================================================
inline void ILogger::SetDelimiter( const char data)
{
    m_delimiter = data;
}

//=============================================================================
inline char ILogger::GetDelimiter( void ) const
{
    return(m_delimiter);
}

//=============================================================================
inline void ILogger::SetAdditionalLogFileName(const std::string &file)
{   // if the filename is not disabled
    if(file.compare("") || file.compare("Disabled") || file.compare("disabled")
       || file.compare("Disable") || file.compare("disable"))
    {
        // The filename sent in is a valid filename.
        m_fileName = file;
        if(m_fileName[0] == '/')    m_fileName.erase(0,1);  // remove leading / for logging file name
        if(IsDebugOn()) printf("Logging to file: %s\n", m_fileName.c_str());

        m_usingAdditionalLog = true;
        // Send the "activate" command to the log server
        m_needSendOpen = true;
        ActivateSpecialLogFile(); // activate it
    }
    else
    {
        m_fileName = "";    // set the file name to nothing
        m_usingAdditionalLog = false;
    }
}

//=============================================================================
inline std::string ILogger::GetAdditionalLogFileName( void ) const
{
    return(m_fileName);
}

//=============================================================================
inline void ILogger::EnableLogger(void)
{
    m_logStatus = true;
}

//=============================================================================
inline void ILogger::DisableLogger(void)
{
    m_logStatus = false;
}

//=============================================================================
inline const bool ILogger::GetLogStatus(void) const
{
    return(m_logStatus);
}

//=============================================================================
void ILogger::SetVerboseMask( UINT32 mask)
{
    m_verboseMask = mask;
}

//=============================================================================
UINT32  ILogger::GetVerboseMask(void) const
{
    return( m_verboseMask);
}

//=============================================================================
INT8 *ILogger::GetTimeString( INT8 *cpRealTime )
{
    struct timespec     stRealTime;
    INT8                caTime[32],
        caMonths[]="JanFebMarAprMayJunJulAugSepOctNovDec";
    INT32               iYear, iMonth, iDate, iMilliSec;

    if(0 == clock_gettime( CLOCK_REALTIME, &stRealTime))
    {
        //"Sat Mar 21 15:58:27 1987\n\0"
        // 012345678901234567890123
        strcpy( (char *)caTime, (char *)ctime( &stRealTime.tv_sec));
        caTime[ 7] = 0;             // null after month
        caTime[10] = 0;             // null after date
        caTime[19] = 0;             // null after time
        caTime[24] = 0;             // null after year
        iYear = atoi( (const char *) &caTime[20]);
        iMonth = ((((long)strstr( (char *)caMonths, (char *)&caTime[4])) - ((long)caMonths)) / 3) + 1;
        iDate = atoi( (const char *) &caTime[8]);
        iMilliSec = (INT32)(stRealTime.tv_nsec / 1000000L);
        if(iYear < 0)
        {
            strcpy( (char *)cpRealTime, "REALTIME YEAR ERROR  ");
        }
        else if((iMonth < 1) || (12 < iMonth))
        {
            strcpy( (char *)cpRealTime, "REALTIME MONTH ERROR ");
        }
        else if((iDate < 1) || (31 < iDate))
        {
            strcpy( (char *)cpRealTime, "REALTIME DATE ERROR  ");
        }
        else if((iMilliSec < 0) || (999 < iMilliSec))
        {
            strcpy( (char *)cpRealTime, "REALTIME [ms] ERROR  ");
        }
        else
        {
            // build time string
            // 21 = strlen( "01/02/03 04:05:06.789")
            sprintf( (char *)cpRealTime, "%02d/%02d/%02d %s.%03d", iMonth, iDate, iYear % 100, &caTime[11], iMilliSec);
        }
    }
    else
    {
        strcpy( (char *)cpRealTime, "REALTIME CLOCK ERROR ");
    }

    return(cpRealTime);
}

//=============================================================================
void ILogger::SendToDaemon(const UINT32      &command,
                           const std::string &message)
{
    // Make sure the "Open" message was sent to server
    if(m_needSendOpen)
    {
        ActivateSpecialLogFile();
    }
    // This string gets built up and sent over ipc
    std::string stOutMesg;

    // First 8 bytes: hex representation of command
    char caBuff[9];
    snprintf(caBuff, 9, "%08x", command);
    stOutMesg.append( caBuff );

    // Next 21 bytes: human-readable time stamp
    char caTimeBuff[22];
    GetTimeString( (INT8*)caTimeBuff );
    stOutMesg.append( caTimeBuff );
    stOutMesg.append( 1, (char)m_delimiter );

    // Add the process name as the "who" field
///////////////////////////////////////////////
// BRM HERE
// Add thread ID to the logger data
    sprintf( caBuff, "(%d) ", pthread_self());
    stOutMesg.append( caBuff );
// BRM HERE
///////////////////////////////////////////////
    stOutMesg.append( m_processName ); // (remaining fields are delim'd)
    stOutMesg.append( 1, (char)m_delimiter );

    // Add logger message
    stOutMesg.append( message );
    stOutMesg.append( 1, (char)m_delimiter );

    // Add the additional log file name
    if(m_usingAdditionalLog)
    {
        stOutMesg.append( GetAdditionalLogFileName() );
    }

    // Lock the IPC object
    if(pthread_mutex_lock( &m_logMutex ) == EOK)
    {
        // Send the message to the LogDameon process
        if(ConnectIpc())
        {
            try
            {
                m_daemonIpc->Write(stOutMesg);
            }
            catch(BepException &bepex)
            {
                if(bepex.Reason().find("timed out") != string::npos)
                {
                    // Timed out, do nothing
                }
                else
                {
                    // Connection went bad; let's clear it and  try again
                    delete(m_daemonIpc);
                    m_daemonIpc = NULL;
                }
            }
        }

        // Unlock the IPC object
        pthread_mutex_unlock( &m_logMutex );
    }
    else
        printf("Error locking m_logMutex ILogger::SendToDaemon: [%s]\n", stOutMesg.c_str());

    if(IsDebugOn())
    {
        printf("Debug ILogger::SendToDaemon: [%s]\n", stOutMesg.c_str());
    }
    // Echo to console if enabled:
    if(m_localEcho == true || m_daemonIpc == NULL)
    {
        if(message.length() > 0)
        {
            printf( "%s %s (%d)> %s", caTimeBuff, m_processName.c_str(),
                    pthread_self(), message.c_str());
        }
        fflush( stdout);
    }
}

//=============================================================================
bool ILogger::ConnectIpc(void)
{
    if(m_daemonIpc != NULL)
    {
        return(true);
    }
    if(BposIsTaskAlive(DATA_LOGGER_NAME) != BEP_STATUS_SUCCESS)
    {
        // no logger so just return false
        return(false);
    }

    try
    {
        m_daemonIpc = new IInterProcessCommunication(DATA_LOGGER_NAME, "Client",
                                                     IsDebugOn(), LOGGER_BUFFER_SIZE, m_timeout);

        if(IsDebugOn())
        {
            printf("Initialized IInterProcessCommunication\n");
        }

        // After any reconnection, activate the log file in case this
        // log daemon is new
        if(m_usingAdditionalLog)
        {
            m_needSendOpen = true;
        }

        return(true);
    }
    catch(BepException &bepex)
    {
        m_daemonIpc = NULL;
        cout << "ILogger::ConnectIpc: Couldn't connect to LogDaemon. Reason: " << bepex.Reason() << endl;
        return(false);
    }
}
#endif // END
