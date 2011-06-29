//******************************************************************************
//
// FILE:
//     $Header: /Ccrt/Source/Core/ServicesLayer/Include/ILogger.h 5     2/22/07 11:00a Rwiersem $
//
// Description:
//     Logging interface class
//
//==============================================================================
//
//     Copyright © 1999-2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/ILogger.h $
// 
// 5     2/22/07 11:00a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// 
// 4     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 3     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 2     4/29/05 8:00a Gpattison
// Changed the Log() methods to virtual to allow them to be
// overridden in support of unit testing
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 2     8/06/03 3:47p Khykin
// Updated with changes from Flatrock.
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
//     Revision 1.13  2003/07/22 17:39:23  karlin
//     dealt with TODOs and FIXMEs
//
//     Revision 1.12  2003/07/21 22:42:55  crosby
//     Added m_timeout
//
//     Revision 1.11  2003/07/18 19:58:45  karlin
//     added PrivilegedLogger
//
//     Revision 1.10  2003/07/17 16:00:03  crosby
//     Made SendToDaemon() public for experiments
//
//     Revision 1.9  2003/07/16 21:50:19  crosby
//     renamed ivars yet again
//
//     Revision 1.8  2003/07/16 21:47:33  crosby
//     Changed case of ivar names
//
//     Revision 1.7  2003/07/16 21:45:48  karlin
//     Added ivars to store log file rotation parameters.
//
//     Revision 1.6  2003/07/15 16:56:42  crosby
//     Now using IInterProcessCommunication and new string-based message format.
//     Removed some (apparently) obsolete methods.
//     COMPILES, not tested yet.
//
//     Revision 1.5  2003/07/14 22:54:20  crosby
//     Started adding methods/ivars to support new IInterProcessCommunication-based
//     messaging.
//
//     Revision 1.4  2003/06/18 13:11:48  tenharmsel
//     Syncing directory trees with the FordChicago system
//
//
// 1     6/12/03 4:16p Khykin
// Initial checkin.
//
// 2     5/19/03 11:19a Khykin
// Updated with Brians latest changes.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:42  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:07a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 23    10/31/02 11:13a Bmeinke
// Made LogMessage() virtual so new LogMgr would not send logger messages
// to itself
//
// 22    10/10/02 10:25a Khykin
// Updated comments for the Log() method.
//
// 21    8/23/02 8:34a Kh
// Synchronizing the pit machine with VSS.
//
// 20    3/06/02 2:32p Kh
// Fixing fat finger.
//
// 19    1/29/02 10:08a Kh
// Updating the Log method for speed.
//
// 18    12/04/01 10:04a Kh
// Updating the declaration of ReLoadConfig to CommandReloadConfig.
//
// 17    8/01/01 3:59p Brian.meinke
// Made GetLocalEcho(), GetVerboseMask(), and GetLogStatus() const methods
//
// 16    7/25/01 2:54p Brian.meinke
// Changed Set/GetVerbosMask to Set/GetVerboseMask
//
// 15    7/02/01 9:57a Kh
// Updating to add debug messages when logging.
//
// 14    6/27/01 8:36a Kh
// Integrating the Debuggable class.
//
// 13    6/20/01 4:05p Brian.meinke
// Added method GetLocalEcho() to retrieve the local echo status
// Added new Log() method which takes a verbose mask argument
// Added methods SetVerboseMask/GetVerboseMask() to control verbosity for
// log messages
// Added m_verbose data member
//
// 12    6/12/00 8:35a Kh
// Update debug methods for common name usage.
//
// 11    6/07/00 2:59p Kh
// Adding debug configurability.
//
// 10    6/07/01 11:29a Brian.meinke
// Changed Boolean declarations to intrinsic bool data type
//
// 9     5/25/00 3:09p Kh
// Updating methods to take object references.
//
// 8     5/15/00 1:37p Kh
// Adding include to headder file for mutex.
//
// 6     4/25/01 3:15p Kh
// Fixes made after running against the LoggerTest TestCase.
//
// 5     4/20/00 5:00p Kh
// Checking in for weekend.
//
// 4     4/20/00 2:49p Kh
// Changing INT8 to char.
//
// 3     4/19/01 1:53p Brian.meinke
// Changed function comments to javadoc style
//
// 2     4/19/00 1:25p Kh
// Modifications for porting to Neutrino.
//
// 4     3/06/01 7:13p Brian.meinke
// Added SetLocalEcho method to have client side interface echo messages
// to the console.
//
// 3     3/05/01 3:05p Brian.meinke
// Added SetLocalEcho method and localEcho data member to enable printing
// logger strings to the console
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 4     2/01/00 9:27a Hil.sybesma
// registered name chanes
//
// 3     1/11/00 6:37p Hil.sybesma
//
// 2     10/19/99 6:24p Hil.sybesma
//
// 9/02/99 3:49p Hil.sybesma
//
//******************************************************************************
#ifndef LOGGER_IFC_HPP
#define LOGGER_IFC_HPP

#include    <iostream>
#include    <stdarg.h>
#include    <string>
#include    <pthread.h>
#include    "Debuggable.h"
#include    "Bepos.h"           // Burke E. Proter O.S. abstraction
#include    "BepDefs.h"         // Burke E. Porter Defines
#include    "Message.h"         // communication
#include    "XmlParser.h"       // XML parser definition
#include    "XmlNode.h"         // XML node definition
#include    "LogDefs.h"
#include    "BepException.h"

class IInterProcessCommunication;

#define DEFAULT_MAXNUMFILES 5
#define DEFAULT_MAXFILESIZE 10
#define DEFAULT_IFCTIMEOUT 100

/**
 * Logging interface class.  This provides the methods to log information to 
 * the Logger task.  The user is responsible for calling the Initialize method
 * with an opened XML document that has a child node that is called "Logging".  
 * Under this child node there should be eight attributes:
 * 
 * Status (Enabled/Disabled)
 * ProcessName (The name to log information with)
 * AdditionalLogFile (An additional log file to log to)
 * MessageTimeout (The amount of time to wait for a response from the Log task)
 * AdditionalLogMaxSize (The maximum size of the additional log file)
 * AdditionalLogMaxFiles (The maximum number of additional log files)
 * LocalEcho (The local echo enable/disable flag)
 * Verbose (The logging verbosity level)
 *
 * Once this is performed, the only thing that needs to be done to log is use 
 * the Log and LogMessage methods.
 *
 * Note:  If you are using an alternate file and you decide to use the 
 * LogMessage method you will also need to include the file name (explicitly or 
 * using GetAdditionalFileName()) to have the message go to both files.
 */
class ILogger : public Debuggable
{
public:
    /**
     * <b>Description:</b>
     * Default Constructor for ILogger objects.
     */
    ILogger();

    /**
     * <b>Description:</b>
     * Default Destructor for ILogger objects.
     */
    virtual ~ILogger();

    /**
     * <b>Description:</b>
     * Forces a reload of the configuration file.
     */
    void CommandReloadConfiguration( void );

    /**
     * <b>Description:</b>
     * Changes the default log to a file with the test number added so the file 
     * can be uniquely identified.
     *
     * @param iTestNumber The test number for the default log name.
     */
    void NewTestNumberLog( INT32 iTestNumber );

    /**
     * <b>Description:</b>
     * Opens a file to hold debug messages.  Messages sent with the 'Log()' 
     * method that have the same file name passed will be sent to this file in 
     * addition to the default debug log file.
     */
    void ActivateSpecialLogFile(void);

    /**
     * <b>Description:</b>
     * Closes the special log file.
     */
    void DeActivateSpecialLogFile(void);

    /**
     * <b>Description:</b>
     * Enables or disables local console echo of logger strings.
     *
     * @param iEnable TRUE to enable, FALSE to disable (Default=TRUE).
     */
    void SetLocalEcho( bool iEnable=true);

    /**
     * <b>Description:</b>
     * Returns state of log interface local echo.
     *
     * @return State of log interface local echo.
     */
    bool GetLocalEcho(void) const;

    /**
     * <b>Description:</b>
     * Set the process name to use for logging.
     *
     * @param name The process name to use for log messages.
     */
    void SetProcessName( const std::string &name );

    /**
     * <b>Description:</b>
     * Get the current process name used for logging.
     *
     * @return The process name being  used for log messages.
     */
    std::string GetProcessName( void ) const;

    /**
     * <b>Description:</b>
     * Set the file delimiter.
     *
     * @param data The delimiter value.
     */
    void SetDelimiter( const char data );

    /**
     * <b>Description:</b>
     * Get the file delimiter.
     *
     * @return The current delimiter value.
     */
    char GetDelimiter( void ) const;

    /**
     * <b>Description:</b>
     * Set the name of the special log file.
     *
     * @param file The filename string.
     */
    void SetAdditionalLogFileName( const std::string &file);

    /**
     * <b>Description:</b>
     * Get the name of the additional log file.
     *
     * @return The addition log filename string.
     */
    std::string GetAdditionalLogFileName( void ) const;

    /**
     * <b>Description:</b>
     * Sends message to debug logger.
     *
     * @param caMessage Message to be logged
     *
     *     Note:  If you are using an alternate file and you decide to use the 
     *            LogMessage method you will also need to include the file name 
     *            (explicitly or using GetAdditionalFileName()) to have the 
     *            message go to both files.
     */
    virtual void LogMessage( const char *caMessage );

    /**
     * <b>Description:</b>
     * Converts a string into a message to be sent to the logger and uses 
     * LogMessage to send the message to debug logger.
     *
     * @param message The message to be logged.
     */
    virtual void Log( const std::string &message);

    /**
     * <b>Description:</b>
     * Converts a variable argument list into a message to be sent to the 
     * logger and uses LogMessage to send the message to debug logger.
     *
     * @param cpFmt The printf style format string to fill in and send to 
     *              logger.
     */
    virtual void Log( const char *cpFmt, ...);

    /**
     * <b>Description:</b>
     * Converts a variable argument list into a message to be sent to the 
     * logger and uses LogMessage to send the message to debug logger.
     *
     * @param mask Verbosity mask: if the current configuration verbosity 
     *                             setting & mask != 0, the message is printed.
     *                             Mask Values: LOG_NONE,LOG_ERRORS,
     *                             LOG_FN_ENTRY,LOG_IO_MSSG,LOG_DEV_DATA,
     *                             LOG_SER_COM,LOG_ALL,LOG_CLASSCOMPONENT_DATA
     * @param cpFmt The printf style format string to fill in and send to 
     *              logger.
     */
    virtual void Log( UINT32 mask, const char *cpFmt, ...);

    /**
     * <b>Description:</b>
     * Method to initialize the logging interface.
     * 
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/Logging/Debug </li>
     *     <li> Setup/Logging/MessageTimeout </li>
     *     <li> Setup/Logging/AdditionalLogMaxSize </li>
     *     <li> Setup/Logging/AdditionalLogMaxFiles </li>
     *     <li> Setup/Logging/ProcessName </li>
     *     <li> Setup/Logging/Status </li>
     *     <li> Setup/Logging/AdditionalLogFile </li>
     *     <li> Setup/Logging/LocalEcho </li>
     *     <li> Setup/Logging/Verbose </li>
     *     </ul>
     *
     * @param document The XML document reference that specifies the logger
     *                 setup information.
     */
    void Initialize(const XmlNode *document);

    /**
     * <b>Description:</b>
     * Method used to enable logging.
     */
    void EnableLogger(void);

    /**
     * <b>Description:</b>
     * Method used to disable logging.
     */
    void DisableLogger(void);

    /**
     * <b>Description:</b>
     * Method used to get the logging status.
     *
     * @return The logging status.
     */
    const bool GetLogStatus(void) const;

    /**
     * <b>Description:</b>
     * Applies a new verbosity mask to logger output.
     *
     * @param mask The new logging verbosity mask.
     */
    void SetVerboseMask( UINT32 mask);

    /**
     * <b>Description:</b>
     * Returns the current verbose mask setting.
     *
     * @return The current verbose mask setting.
     */
    UINT32  GetVerboseMask(void) const;

    /**
     * <b>Description:</b>
     * Generates a formated string representing the current time and date.
     *
     * @param cpRealTime A pointer to a 22 byte minimum character buffer for 
     *                   the time and date string.
     * 
     * @return A pointer to the updated time and date buffer passed in.
     */
    static INT8 *GetTimeString( INT8 *cpRealTime );

    /**
     * <b>Description:</b>
     * Send message over IPC to the logging process.
     * Message format will be:
     *   bytes   contents
     *   0-7     hex string encoding the command
     *   8-28    human-readable timestamp, 21 characters
     *   29      delimiter
     *   30-n    who
     *   .       delimiter
     *   .       message
     *   .       delimiter
     *   .       additional log file name
     * 
     * @param command The command code for the message (see ROLL_* in LogDefs.h)
     * @param message The message body.
     */
    void SendToDaemon(const UINT32 &command, const std::string &message);

    /** 
     * <b>Description:</b>
     * If needed, establish an Ipc connection to log daemon.
     * 
     * @return If successfully connected, return true.
     *         Otherwise, return false.
     */
    bool ConnectIpc(void);

private:

    /** Number of log files allowed before they start getting deleted. */
    int m_maxNumFiles;

    /** Maximum size of each log file in megabytes. */
    long m_maxFileSize;

    /**
     * How long (millis) to wait on a message send to log daemon before giving 
     * up.
     */
    long m_timeout;

    /** 
     * Flag indicates that the Open message for additional log files still needs
     * to be sent 
     */
    bool m_needSendOpen;

    /** Name of the process for logging. */
    std::string  m_processName;

    /** Communication channel to the logging process. */
    IInterProcessCommunication* m_daemonIpc;

    /** The debug level identifier. */
    char    m_delimiter;

    /** The special logging file name. */
    std::string  m_fileName;

    /** Indicates that we're using an additional log. */
    bool m_usingAdditionalLog;

    /** Status of logging. */
    bool    m_logStatus;

    /** Flag to enable local console echo. */
    bool    m_localEcho;

    /** The current verbosity mask. */
    UINT32  m_verboseMask;

    /** Mutex used to lock methods for exclusive access. */
    pthread_mutex_t     m_logMutex;

    /** Mutex lock attrubute structure. */
    pthread_mutexattr_t m_logAttrMutex;
};

#endif  // LOGGER_IFC_HPP

