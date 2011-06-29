//****************************************************************************
// $Archive: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogFile.cpp $
// $Date: 10/30/07 4:12p $
//
// CLASS:
//      Logging
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogFile.cpp 3     10/30/07 4:12p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogFile.cpp $
// 
// 3     10/30/07 4:12p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004. 
//
// 1     12/21/03 6:41p Cward
//    Revision 1.20  2003/10/24 20:21:55  tenharmsel
//    updated LogDaemon tests to correctly access files during a test
//
//    Revision 1.19  2003/10/09 21:35:56  swieton
//    initializing memory for mutexes, fixed possible file-leak
//
//    Revision 1.18  2003/10/07 23:19:47  swieton
//    Added stuff for mutext tests
//
//    Revision 1.17  2003/10/07 18:04:51  karlin
//    tester stuff, and small bug fix in corelogfile
//
//    Revision 1.16  2003/10/06 21:21:24  crosby
//    Fixed some debug messages, calling Flush() from Close()
//
//    Revision 1.15  2003/10/06 20:22:51  crosby
//    Fixed a debug msg
//
//    Revision 1.14  2003/10/03 17:10:13  crosby
//    Erased a FIXME near call to Cap(); return type is not really very important at this point
//
//    Revision 1.13  2003/10/02 23:20:53  swieton
//    Protect from multiple opens
//
//    Revision 1.12  2003/10/02 23:07:50  swieton
//    Output now using internal logger
//
//    Revision 1.11  2003/10/02 19:54:06  crosby
//    missing semi
//
//    Revision 1.10  2003/10/02 19:49:39  crosby
//    Fixed collision between stat and stat()
//
//    Revision 1.9  2003/10/02 19:35:13  crosby
//    checking mutex lock/unlock return status
//
//    Revision 1.8  2003/10/01 18:51:41  crosby
//    Removed RunSaveLog() method
//
//    Revision 1.7  2003/10/01 18:50:16  crosby
//    Fixed the mutex leak in RotateIfNeeded().
//    Now using Rotate() instead of RunSaveLog().
//
//    Revision 1.6  2003/10/01 18:46:40  crosby
//    roll back to korea-like code
//
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//    Revision 1.32  2003/07/21 21:46:56  swieton
//    fileStream made dynamically allocated
//
//    Revision 1.31  2003/07/21 20:34:16  crosby
//    Recursive pthread mutex lock
//
//    Revision 1.30  2003/07/21 20:16:03  swieton
//    Added locks to LogFile::WriteLine()
//
//    Revision 1.29  2003/07/21 17:36:10  crosby
//    Capping in case of rotation
//
//    Revision 1.28  2003/07/21 17:17:19  crosby
//    Added some error handling/return code.  Needs more.
//    INCOMPLETE.
//
//    Revision 1.27  2003/07/18 21:11:32  crosby
//    Added SetDebugging()
//
//    Revision 1.26  2003/07/18 20:26:44  crosby
//    Checking stat() return val in Size()
//
//    Revision 1.25  2003/07/18 16:35:41  crosby
//    Deleted obsolete fork/exec code.
//    Fixed inverted logic for error checking on system()
//
//    Revision 1.24  2003/07/18 15:59:06  karlin
//    fixed removal and fork() bug
//
//    Revision 1.23  2003/07/18 15:00:13  karlin
//    debugging a fork
//
//    Revision 1.22  2003/07/18 00:00:29  crosby
//    Added debug mssgs
//
//    Revision 1.21  2003/07/17 23:51:57  crosby
//    Added debug mssgs
//
//    Revision 1.20  2003/07/17 23:50:13  crosby
//    using 'savelog' hardcode instead of #define
//
//    Revision 1.18  2003/07/17 23:33:15  crosby
//    Found mutex abuse in Open()
//
//    Revision 1.17  2003/07/17 20:42:18  crosby
//    Broke out some filesize checking and rotation code into separate methods
//
//    Revision 1.16  2003/07/17 16:06:41  crosby
//    RotateIfNeeded(): correctly converting maxFiles into a char for exec.
//
//    Revision 1.15  2003/07/17 15:35:24  crosby
//    Using execlpe to run savelog
//
//    Revision 1.14  2003/07/17 15:03:46  crosby
//    Cleaned up constructor and constructor params
//
//    Revision 1.13  2003/07/16 20:47:57  karlin
//    better error checks on wait()
//
//    Revision 1.12  2003/07/16 20:39:33  crosby
//    Made const ref type for GetFileName()
//
//    Revision 1.11  2003/07/16 20:11:12  crosby
//    Fiddling with exec, fix type-o
//
//    Revision 1.10  2003/07/16 15:25:29  karlin
//    fork attempt
//
//    Revision 1.9  2003/07/16 13:35:02  karlin
//    added open check to WriteLine
//
//    Revision 1.8  2003/07/15 22:11:13  crosby
//    fileName is now set to store the FULL PATH, by prepending the logDir
//
//    Revision 1.7  2003/07/15 21:32:25  crosby
//    Skelling RotateIfNeeded().
//    Changed outFile to fileStream.
//
//    Revision 1.6  2003/07/15 18:31:23  crosby
//    Using fileStream to write messages.
//
//    Revision 1.5  2003/07/14 19:26:02  crosby
//    Using int return types for rudimentary error reporting;
//    implemented thread protection via mutex locks;
//    implemented open/close/flush,
//    INCOMPLETE: need to implement WriteLine()
//
//    Revision 1.4  2003/07/14 15:58:05  crosby
//    Changed Write() to WriteLine()
//
//    Revision 1.3  2003/07/11 19:47:32  crosby
//    Added destructor.
//    Added GetFileName().
//
//    Revision 1.2  2003/07/10 22:52:29  crosby
//    Added default constructor for CoreLogFile subclass
//
//    Revision 1.1  2003/07/10 19:08:24  crosby
//    Moved here from Managers/LoggingUtility
//
//    Revision 1.1  2003/07/09 22:08:49  karlin
//    added for LogDaemon system
//
//******************************************************************************
#include "LogFile.h"
#include "LogDaemon.h"



/**
 * Creates a LogFile with specified name.
 * 
 * @param pLogDir   The current log file directory.
 * @param pFileName The current log file name.
 * @param pMaxSize  The maximum allowable log file size.
 * @param pMaxFiles The maximum allowable number of log files.
 */
LogFile::LogFile( const std::string &pLogDir,  const std::string &pFileName,
                  const UINT32 &pMaxSize, const UINT32 &pMaxFiles)  :
logDir(pLogDir),
fileName(pFileName),
maxSize(pMaxSize),
maxFiles(pMaxFiles),
fileStream (NULL),
isCapped(false),
refCount( 1)
{
    // Store relative log name (remove leading slashes)
    while(*fileName.begin() == '/')
    {
        fileName = fileName.substr(1);
    }

    // Store log dir, make sure it ends in slash
    int len = logDir.size();
    if( len == 0 )
    {
        logDir = "/";
    }
    else if(logDir[len-1] != '/')
    {
        logDir.append("/");
    }

    memset(&writeLockAtts, 0, sizeof(writeLockAtts));
    memset(&writeLock, 0, sizeof(writeLock));

    // Initialize the write lock: recursive
    pthread_mutexattr_init(&writeLockAtts);
    pthread_mutexattr_settype(&writeLockAtts, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&writeLock, &writeLockAtts);

    // Compose absolute file path
    fileName = logDir + fileName;

}

/** destructor */
LogFile::~LogFile()
{
    Close();
    pthread_mutex_destroy(&writeLock);
}
/**
 * Opens a log file.
 * @return -1 if unable to open file for writing.
 */
int LogFile::Open()
{
    //lock
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Open FAILED to get lock.");
        return -1;
    }

    isCapped = false;

    // Don't try to open a file twice
    if((fileStream != NULL) && (*fileStream != 0))
    {
        Close();
    }

    // Open logfile : DEFAULT is output and truncate
    fileStream = new ofstream( fileName.c_str());

    //, std::ios::trunc );
    int ret = 0;
    if(!*fileStream)
    {
        if(IsDebugOn()) LogDaemon::out("LogFile::Open: [%s] failed to open!.", fileName.c_str());
        ret = -1;
    }
    else if( fileStream->fail() == true)
    {
        LogDaemon::out("LogFile::Open: [%s] failed to open due to ios::failbit.", fileName.c_str());
        ret = -1;
    }
    else
    {
        if(IsDebugOn()) LogDaemon::out("LogFile::Open: [%s] opened.", fileName.c_str());
        ret = 0;
    }

    // unlock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK) {
        ret = -1; // fail to open
        // Close the file if it's really open
        if(fileStream != NULL)
        {
            fileStream->close();
            delete fileStream;
            fileStream = NULL;
        }
        LogDaemon::out("LogFile::Open: FAILED to unlock mutex, not opening file");
    }

    return ret;
}

/**
 * Closes a log file.
 */
int LogFile::Close()
{
    int ret = 0;
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Close FAILED to get lock.");
        return -1;
    }
    // flush
    Flush();
    // close
    if(fileStream != NULL)
    {
        fileStream->close();
        delete fileStream;
        fileStream = NULL;
    }
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK)
    {
        ret = -1;
        LogDaemon::out("LogFile::Close: FAILED to unlock mutex.");
    }
    if(IsDebugOn()) LogDaemon::out("LogFile::Close: [%s] closed.", fileName.c_str());
    return ret;
}

/**
 * Flush the output stream
 */
int LogFile::Flush()
{
    int ret = 0;
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Flush FAILED to get lock.");
        return -1;
    }
    if(fileStream)
    {
        fileStream->flush();
    }
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK) {
        ret = -1;
        LogDaemon::out("LogFile::Flush: FAILED to unlock mutex.");
    }
    return ret;
}

/**
 * Decides if the log file needs to be rotated, and does
 * rotation if needed.
 * Return is zero for success, non-zero for failure.
 */
int LogFile::RotateIfNeeded()
{
    if (isCapped)
    {
        // we've been capped.  do not consider.
        return 0;
    }

    int ret = 0;
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::RotateIfNeeded FAILED to get lock.");
        return -1;
    }

    if(MaxSizeExceeded())
    {
        if(IsDebugOn())
        {
            LogDaemon::out("LogFile::RotateIfNeeded: [%s] max size exceeded (size=%d), rotating.",
                    fileName.c_str(), Size());
        }
        Close();

        // Do rotation
        stat = Rotate();
        if(stat != 0)
        {
            // Rotation failed.
            // Cap the file.
            Cap();
            // Unlock
            stat = pthread_mutex_unlock(&writeLock);
            if (stat != EOK) {
                LogDaemon::out("LogFile::RotateIfNeeded: FAILED to unlock mutex.");
            }
            // return failure
            return -1;
        }
        
        // Reopen the file
        if (Open() != 0)
        {
            stat = pthread_mutex_unlock(&writeLock);
            if (stat != EOK) {
                LogDaemon::out("LogFile::RotateIfNeeded: FAILED to unlock mutex.");
            }
            // return failure
            return -1;
        }
    }
    else
    {
        // Rotation was not neccessary
        if(IsDebugOn())
        {
            LogDaemon::out("LogFile::RotateIfNeeded: [%s] not rotating.", fileName.c_str());
        }
    }

    
    // rotation is complete, release the lock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK) {
        ret = -1;
        LogDaemon::out("LogFile::RotateIfNeeded: FAILED to unlock mutex.");
    }
    return ret;
}

/**
 * Return true if current file size exceeds maxSize
 */
bool LogFile::MaxSizeExceeded()
{
    if(maxSize <= 0)
    {
        // If maxSize is 0, rotation is disabled
        return false;
    }
    return Size() > ((long)maxSize*(long)1048576);
}

/**
 * Rotate our log files:
 *      ii      old         new
 *      --------------------------
 *      5       file.4      file.5
 *      4       file.3      file.4
 *      3       file.2      file.3
 *      2       file.1      file.2
 *      1       file.0      file.1
 *
 * @return 0 if files were successfully  rotated, != 0 on error
 */
int LogFile::Rotate()
{
    char    oldFileName[ _POSIX_PATH_MAX+1];
    char    newFileName[ _POSIX_PATH_MAX+1];
    int     i;

    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Rotate FAILED to get lock.");
        return -1;
    }
    stat = 0;

    // Rotate the old log files
    for( i=maxFiles-1; i>0; i--)
    {
        snprintf( oldFileName, _POSIX_PATH_MAX, "%s.%d", fileName.c_str(), i-1);
        snprintf( newFileName, _POSIX_PATH_MAX, "%s.%d", fileName.c_str(), i);
        
        // Ignore the return value from rename. If the oldFileName does not exist,
        //  we dont really care.
        (void)rename( oldFileName, newFileName);
    }
    
    // Rotate the <fileName> into <fileName>.0
    snprintf( newFileName, _POSIX_PATH_MAX, "%s.0", fileName.c_str());

    // We care about this rename() status because <fileName> must exist (it
    //  is the file we are currently writing data to)
    int renameStat = rename( fileName.c_str(), newFileName);
    if (renameStat != 0)
    {
        stat = -1; // for returning failure
        LogDaemon::out("LogFileMap::Rotate: FAILED to rename [%s] to [%s]",
                fileName.c_str(), newFileName);
    }
    
    int unlockStat = pthread_mutex_unlock(&writeLock);
    if (unlockStat != EOK) {
        stat = -1; // for returning failure
        LogDaemon::out("LogFileMap::Rotate: FAILED to unlock mutex.");
    }

    return(stat);
}

/**
 * Writes a line to a log file.
 * @param line the line to write to the log.
 */
int LogFile::WriteLine(std::string line)
{
    if(isCapped)
    {
        // quietly do nothing
        return 0;
    }
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::WriteLine FAILED to get lock.");
        return -1;
    }

    if( !fileStream  || !*fileStream)
    {
        // file not properly opened
        stat = pthread_mutex_unlock(&writeLock);
        if (stat != EOK) {
            LogDaemon::out("LogFileMap::WriteLine: FAILED to unlock mutex.");
        }
        return -1;
    }
    *fileStream << line << "\n"; // careful not to flush stream

    // unlock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK)
    {
        LogDaemon::out("LogFileMap::WriteLine: FAILED to unlock mutex.");
        return -1;
    }
    else
    {
        return 0;
    }
}

/**
 * Closes the file with allowed writes later.
 */
int LogFile::Cap()
{
    // lock
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Cap FAILED to get lock.");
        return -1;
    }

    isCapped = true;

    int closeStat = Close();

    // unlock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK)
    {
        LogDaemon::out("LogFile::Cap FAILED to unlock.");
        return -1;
    }

    if (closeStat != 0)
    {
        // Closing failed
        return -1;
    }
    else
    {
        return 0;
    }
}

/**
 * Return the file name
 */
const std::string& LogFile::GetFileName() const
{
    return fileName;
}

/**
 * Set the file name
 */
void LogFile::SetFileName(std::string &newFileName)
{
    fileName = newFileName;
}

long LogFile::Size()
{
    int lockStat = pthread_mutex_lock(&writeLock);
    if (lockStat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::Size FAILED to get lock.");
        return -1;
    }

    // Check file size
    struct stat myStat;
    int status = stat(fileName.c_str(), &myStat);
    if(status != 0)
    {
        if(IsDebugOn())
        {
            LogDaemon::out("LogFile::Size(): stat() failed: %s", strerror(errno));
        }

        // unlock
        lockStat = pthread_mutex_unlock(&writeLock);
        if (lockStat != EOK)
        {
            LogDaemon::out("LogFile::Size FAILED to unlock.");
        }

        // if someone deleted our log file out from under us, we need to
        // re-open it to get a real file back on disk
        if (errno == ENOENT)
        {
            LogDaemon::out("LogFile::Size(): %s appears to have leaked, "
                    "attempting reopen", fileName.c_str());
            Close();
            Open();
            return 0;
        }
        
        return -1;
    }
    //unlock
    lockStat = pthread_mutex_unlock(&writeLock);
    if (lockStat != EOK)
    {
        LogDaemon::out("LogFile::Size FAILED to unlock.");
        return -1;
    }
    return myStat.st_size;
}


/** Activate debugging */
void LogFile::SetDebugging(bool t)
{
    SetDebug(t);
}

/**
 * Increments the reference count to this LogFile object. This is needed when
 * multiple clients are logging data to the same special log file. Without a
 * reference count, a special logfile will be "closed" for all clients the first
 * time any client de-activates the special log file.
 *
 * @return The new reference count of this LogFile object (after being incremented)
 */
int LogFile::AddRef()
{
    int newRefCount;

    // lock
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::AddRef FAILED to get lock.");
        return -1;
    }

    // increment lock count
    newRefCount = ++refCount;

    // unlock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK)
    {
        LogDaemon::out("LogFile::AddRef FAILED to unlock.");
    }
    return( newRefCount);
}

/**
 * Retuens the new reference count of this LogFile object (after being incremented)
 *
 * @return The new reference count of this LogFile object (after being incremented)
 */
uint32_t LogFile::GetRefCount() const
{
    return(refCount);
}

/**
 * Decrements the reference count to this LogFile object. This is needed when
 * multiple clients are logging data to the same special log file. Without a
 * reference count, a special logfile will be "closed" for all clients the first
 * time any client de-activates the special log file.
 *
 * @return The new reference count of this LogFile object (after being decremented)
 */
int LogFile::DelRef()
{
    int newRefCount=0;

    // lock
    int stat = pthread_mutex_lock(&writeLock);
    if (stat != EOK)
    {
        // Complain and abort.
        LogDaemon::out("LogFile::DelRef FAILED to get lock.");
        return -1;
    }
    // Decrement refcount
    if( refCount > 0)
    {
        newRefCount = --refCount;
    }
    //unlock
    stat = pthread_mutex_unlock(&writeLock);
    if (stat != EOK)
    {
        LogDaemon::out("LogFile::DelRef FAILED to unlock.");
    }
    return( newRefCount);
}

