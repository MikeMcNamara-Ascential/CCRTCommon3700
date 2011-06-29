//******************************************************************************
// Description:
//   The Log Rotator class scans the LogFileMap for files that need to be
//   rotated.
//
//==============================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogRotator.cpp $
// 
// 5     10/30/07 4:12p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added std:: to the parameter files in GetAdditionalLogFiles() and
// RotateLogFiles() to match the header file.
// 
// 4     10/26/06 11:20a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the CCRT software version string to the log file.
// 
// 3     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     9/01/04 9:37p Cward
// Added functionality to prevent the log daemon from rotating files
// during a test sequence.
//
// 1     12/21/03 6:41p Cward
//    Revision 1.5  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.4  2003/10/02 16:18:10  crosby
//    Error checking on mutex locking (LogFileMap.Lock/Unlock())
//
//    Revision 1.3  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.22  2003/07/23 14:45:36  crosby
//    Fixed default sleep time
//
//    Revision 1.21  2003/07/23 14:09:07  crosby
//    Setting thread priority in Run()
//
//    Revision 1.20  2003/07/23 13:59:45  crosby
//    Taking thread priority in constructor
//
//    Revision 1.19  2003/07/22 21:37:34  karlin
//    fixed rebug with const args
//
//    Revision 1.18  2003/07/22 17:39:23  karlin
//    dealt with TODOs and FIXMEs
//
//    Revision 1.17  2003/07/21 19:06:06  swieton
//    fixed typo
//
//    Revision 1.16  2003/07/21 17:36:44  crosby
//    Debug and log message touch up.
//    Error handling.
//
//    Revision 1.15  2003/07/21 14:28:16  swieton
//    Refactored for easier testing
//
//    Revision 1.14  2003/07/18 21:12:15  crosby
//    Added SetDebugging()
//
//    Revision 1.13  2003/07/18 19:57:00  crosby
//    Added SetPeriod()
//
//    Revision 1.12  2003/07/18 16:40:56  crosby
//    RotateOrCap() --> RotateIfNeeded()
//
//    Revision 1.11  2003/07/18 15:58:03  karlin
//    fixed NULL rotations
//
//    Revision 1.10  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.9  2003/07/17 23:40:27  crosby
//    Re-wrote iteration loops, checking for null core log
//
//    Revision 1.8  2003/07/17 22:44:51  crosby
//    Set sleepTime.
//    Added debugging output.
//
//    Revision 1.7  2003/07/17 20:16:31  crosby
//    Calling RotateIfNeeded() on CoreLogFile
//
//    Revision 1.6  2003/07/17 16:06:56  karlin
//    added cap core call
//
//    Revision 1.5  2003/07/17 14:10:28  karlin
//    added sleeping and an infinite loop
//
//    Revision 1.4  2003/07/16 23:25:41  crosby
//    *** empty log message ***
//
//    Revision 1.3  2003/07/15 22:18:55  crosby
//    Fixed iteration loop and destructor and constructor
//
//    Revision 1.2  2003/07/15 22:10:03  karlin
//    filled in methods
//
//    Revision 1.1  2003/07/10 22:53:51  crosby
//    added
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "LogRotator.h"
#include "LogDaemon.h"
#include "INamedDataBroker.h"

/**
 * Create a new LogRotator
 */
LogRotator::LogRotator(LogFileMap* fm) :
    m_ndb(NULL), sleepTime(10), m_maxDelays(0),
    m_tag(""), m_delayValue("")
{
    logFileMap = fm;
    SetProcessName("core/LogRotator");
}

/**
 * Destroy a LogRotator
 */
LogRotator::~LogRotator()
{
    if(m_ndb != NULL)           // delete the ndb interface to null
        delete m_ndb;
}

/**
 * Static launcher function.  This is the entry point for the
 * worker thread spawned by LogDaemon.
 * @param myLogRotator The LogRotator instance to call Run() on.
 */
void* LogRotator::Launch(void* myLogRotator)
{
    string verStr( CCRT_VERSION_STR );
    
    LogRotator* rotator = (LogRotator*)myLogRotator;
    rotator->PrivLog( std::string("Launching software version ") +  verStr);
    if(rotator->IsDebugOn())
    {
        LogDaemon::out("Launching LogRotator");
    }
    ((LogRotator*)myLogRotator)->Run();
    return NULL;
}

/**
 * Set the delay attributes. This is the tag to listen for, the
 * value to look for and the maximum number of times to delay.
 * @param tag tag to listen for
 * @param value value to look for
 * @param delays maximum number of times to delay.
 */
void LogRotator::SetDelayOn( const std::string &tag, const std::string &value, const int &delays)
{
    m_tag = tag;
    m_delayValue = value;
    m_maxDelays = delays;
}

/**
 * Start polling loop.  When we wake up, we grab the list of log files from
 * the LogFileMap, then check each file therein for rotation readiness,
 * rotating when approprate.
 * Then, check for corelog capping and do so when appropriate.
 */
void LogRotator::Run()
{
    std::string value, response;
    char sleepString[12];
    sprintf(sleepString, "%d", sleepTime);
    std::string dbgMsg = "Starting Run, wake-up period ";
    dbgMsg.append(sleepString);
    PrivLog(dbgMsg);
    if(IsDebugOn())
    {
        LogDaemon::out(dbgMsg.c_str());
    }

    while(true)
    {
        sleep(sleepTime);

        PrivLog("Waking");
        if(IsDebugOn())
        {
            LogDaemon::out("LogRotator waking");
        }

        /* Grab pointers to every log file */
        std::vector<LogFile*> files;

        /* Lock the filemap so no one corrupts our iterator */
        int stat = logFileMap->Lock();
        if (stat == EOK)
        {
            GetAdditionalLogFiles (files);

            /* Rotate all the files in the vector we just grabbed (all the
             * additional log files) */
            RotateLogFiles (files);

            RotateCoreLogFile ();

            stat = logFileMap->Unlock();
            if(stat != EOK)
            {
                LogDaemon::out("LogRotator::Run: FAILED to LogFileMap.Unlock().");
            }
        }
        else
        {
            LogDaemon::out("LogRotator::Run: failed to LogFileMap.Lock().");
        }
    }
}

void LogRotator::RotateCoreLogFile ()
{
    if(logFileMap->GetCoreLog() != NULL)
    {
        // Ensure core log has not exceeded it's size limit
        if(IsDebugOn())
        {
            LogDaemon::out("LogRotator::RotateCoreLogFile: start...");
        }
        // "Rotate" returns 0 for success, non-zero for failure.
        int fail = logFileMap->GetCoreLog()->RotateIfNeeded();
        if(fail)
        {
            // The rotation failed.  CoreLogFile is now capped!
            LogDaemon::out("LogRotator::RotateCoreLogFile: failed! CoreLogFile "
                    "[%s] is now capped!",
                    logFileMap->GetCoreLog()->GetFileName().c_str());
        }
        else
        {
            if(IsDebugOn())
            {
                LogDaemon::out("LogRotator::RotateCoreLogFile: done.");
            }
        }
    }
    else
    {
        LogDaemon::out("LogRotator::RotateCoreLogFile: couldn't find the CoreLogFile."
            " Couldn't rotate.");
        PrivLog("In RotateCoreLogFile: Couldn't get the CoreLogFile from the LogFileMap!");
    }
}

/** Grab a pointer to all files in the LogFileMap and store the pointer in the
 * provided parameter. Using the iterator will lock the map while we do this.
 * */
void LogRotator::GetAdditionalLogFiles (std::vector<LogFile*> &files)
{
    if(IsDebugOn())
    {
        if(!files.empty())
        {
            LogDaemon::out("LogRotator::GetAdditionalLogFiles(): WARNING! expected 'files' "
                "parameter to be empty");
        }
    }

    LogFileIterator *iter = logFileMap->Iterator();
    if(IsDebugOn())
    {
        LogDaemon::out("LogRotator::GetAdditionalLogFiles: Gathering logs");
    }
    while(iter->HasNext())
    {
        files.push_back(iter->Next());
        if(IsDebugOn())
        {
            LogDaemon::out("."); // for visual feedback
        }
    }
    delete iter;
}

/** Call RotateIfNeeded() on all files in the provided vector. */
void LogRotator::RotateLogFiles(std::vector<LogFile*> &files)
{
    // If a test is in progress, do not rotate the file.
    if(m_ndb == NULL)
    {
        m_ndb = new INamedDataBroker();
    }
    string response, value;
    string tag("TestInProgress");
    bool testInProgress = true;
    INT32 status = m_ndb->Read(tag, response, true);
    if (BEP_STATUS_SUCCESS == status)
    {
        status = m_ndb->GetByTag(tag, value, response);
        testInProgress = !value.compare("1");
    }
    else
    {   // Error reading test in progress status - assume test in progress.
        testInProgress = true;
        LogDaemon::out("LogRotator::RotateLogFiles: Error reading system tag - TestInProgress");
    }
    // Loop through all additional LogFiles and rotate if needed
    if(!files.empty() && !testInProgress)
    {
        if(IsDebugOn())
        {
            LogDaemon::out("LogRotator::RotateLogFiles...");
        }
        PrivLog("Rotating additional log files");
        std::vector<LogFile*>::iterator firstFile = files.begin();
        std::vector<LogFile*>::iterator lastFile = files.end();
        while(firstFile != lastFile)
        {
            // STL inserts a new pair into the map when we call Get(), so we need
            // to make sure we don't try to rotate a NULL LogFile pointer.
            if(*firstFile != NULL)
            {
                (*firstFile)->RotateIfNeeded();
            }
            else
            {
                if(IsDebugOn())
                {
                    PrivLog("LogFileMap had NULL entry for a key.");
                }
            }

            ++firstFile;
            if(IsDebugOn())
            {
                LogDaemon::out("."); // for visual progress
            }
        }
    }
    else
    {
        if (testInProgress)
        {
            LogDaemon::out("LogRotator::RotateLogFiles:  test in progress, not rotating log files\n");
        }
        else if (IsDebugOn())
        {   // no files in vector
            LogDaemon::out("LogRotator::RotateLogFiles: no additional log files to rotate or test in progress");
        }
    }
}

/**
 * Set the amount of time between rotation wake-ups
 */
void LogRotator::SetPeriod(int period)
{
    sleepTime = period;
}

/** Activate debugging */
void LogRotator::SetDebugging(bool t)
{
    SetDebug(t);
}
