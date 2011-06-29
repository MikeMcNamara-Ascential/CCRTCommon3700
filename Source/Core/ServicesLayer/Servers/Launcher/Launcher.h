//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/Launcher/Launcher.h 5     10/30/07 4:10p Rwiersem $
//
// FILE DESCRIPTION:
//   This class will launch the system as specified by the configuration file.
//   It will also wait for a terminate, abort, or reset message to be send and
//   then it will command all of its children to terminate, abort, or reset.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/Launcher/Launcher.h $
// 
// 5     10/30/07 4:10p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     5/08/06 7:41p Cward
// Changes that cause the launcher to report executable files that could
// not be found to the Log files
// Added m_missingTasks for storage of executable names that are called out
// in the config file, but can't be found on the drive.
//
// 3     3/03/05 11:46p Cward
// Updated for 05030302. Needed quick fix for new licensing
//
// 2     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 1     7/21/04 10:18a Cward
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 1:36p Khykin
// Updated the launcher with the security key code.
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:16p Khykin
// Initial checkin.
//
// 2     5/29/03 6:30a Khykin
// Updated the Shutdown method for the new shutdown strategy.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:33  tbochene
//
//    PooBag
//
//
// 2     2/08/03 9:51a Khykin
// Updated so it has the functionality to shutdown the system.
//
// 1     1/27/03 7:07a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 18    10/23/02 1:26p Khykin
// Updated the launcher to use the pid for killing processes
// that do not respond the the terminate pulse.
//
// 17    8/23/02 8:33a Kh
// Synchronizing the pit machine with VSS.
//
// 15    5/24/02 2:53p Kh
// Adding the bility to delay between tasks and adding more diag.
//
// 14    4/25/02 4:41p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//    started
//
//
// 12    3/19/02 12:02p Kh
// Adding features to the launcher.
//
// 11    3/07/02 2:45p Kh
// Adding features to the launcher.
//
// 10    12/18/01 3:52p Kh
// Updating the Register method.
//
// 9     12/14/01 5:44p Kh
// Adding a Register method.
//
// 8     12/07/01 3:02p Kh
// Updating the launcher.
//
// 7     11/15/01 3:51p Kh
// Updating the system commands.
//
// 6     11/09/01 7:58a Kh
// Adding a check for the files existence.
//
// 5     10/25/01 10:02a Kh
// Adding more debug information.
//
// 4     10/10/01 10:14a Kh
// Update the launcher to shutdown tasks in revers order of starting
//
// 3     10/09/01 11:17a Kh
// Updating the launcher to launch components prior to setting up
//
// 2     9/26/01 2:07p Kh
// Updating so the launcher object can be declared and used.
//
//*************************************************************************

#ifndef MYLAUNCHER_H_
#define MYLAUNCHER_H_

#include <string>

#include "XmlNode.h"
#include "BepComponent.h"
#include "IBepCommunication.h"
#include "IFaultServer.h"
#include "CCRTLicense.h"
#include "LicenseTimer.h"
#include "INamedDataBroker.h"

class LicenseTimer;

/**
 * The taskinfo structure that is used to handle the information
 * that is needed to manage a task.
 * @since Version 1.0
 */
typedef struct taskinfo
{
    /**
     * The name of the process to launch
     * @since Version 1.0
     */
    std::string process;
    /**
     * The name of the process in case it has to be killed.
     * @since Version 1.0
     */
    std::string processName;
    /**
     * The name the task will register as
     * @since Version 1.0
     */
    std::string registeredName;
    /**
     * Flag to indicate if the task should be sent the Register
     * command after launching.
     * @since Version 2.3
     */
    bool registerTask;
    /**
     * the status of the task, Running, NotRunning, ...
     * @since Version 1.0
     */
    std::string status;
    /**
     * string to hold the arguments for a task to start
     * the configuration file to use (-f)
     * Demo Mode Flag (-d)
     * Console Mode Flag (-c)
     * Address Value (-a)
     * Port Value (-p)
     * Verbose Mode Value (-v)
     * @since Version 2.2
     */
    std::string arguments;
    /**
     * The Process ID of the launched component.
     * @since Version 2.4
     */
    int processID;
} TASKINFO;

typedef map< string, TASKINFO > TASKTABLE;              // table of all of the tasks to launch
typedef TASKTABLE::iterator     TASKTABLEITR;           // iterator of the task table
typedef TASKTABLE::value_type   TABLEVAL;

typedef map< string, string >       TASKNAMELIST;       // list of the task names in case I have to kill them
typedef TASKNAMELIST::iterator      TASKNAMELISTITR;    // iterator of the task name table
typedef TASKNAMELIST::value_type    TASKNAMELISTVAL;

class LauncherTest;
/**
 * Launcher class that is used to launch and manage a group
 * of processes.
 *
 * @author Kevin Hykin
 * @version Version 2.5
 */
class Launcher : public BepComponent
{
public:
    friend class LauncherTest;
    /**
     * Default constructor.
     * @since Version 1.0
     */
    Launcher();

    /**
     * Default destructor.
     * @since Version 1.0
     */
    virtual ~Launcher();

    /**
     * Initialization method that loads all of the information
     * for the processes that will be managed.
     *
     * @param document A parsed XML document to use for the launcher configuration.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Main run method that subclasses can overload if different
     * behavior is desired.
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

    /**
     * Launch the system based on the component initialization.
     * @since Version 1.0
     */
    void LaunchSystem(void);

    /**
     * Wait for the requests of others.
     * @since Version 1.0
     */
    void ProcessRequest(void);

    /**
     * Reloads the configuration file and resets the
     * base components objects.
     * @since Version 1.0
     */
    virtual void ReloadConfiguration(void);

    /**
     * Method used to abort the current processing based
     * on the needs of the component.
     * @since Version 1.0
     */
    virtual void Abort(void);

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void);

    /**
     * Method used to command all of the components to register
     * all of their subscriptions and anything else that should be
     * done once the system is up.
     * @since Version 2.2
     */
    virtual const std::string Register(void);

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void);

    /**
     * Method used to shutdown the system.  It will first
     * terminate the servers on the system and then perform
     * a system shutdown.
     *
     * @since Version 2.5
     */
    virtual void Shutdown(void);

    /**
     * Request to read a data tag
     *
     * @param node   The data to read.
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node);

    /**
     * Request to write a data tag
     *
     * @param node   The write to perform.
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const XmlNode *node);

    /**
     * Request to Perform an action
     *
     * @param node   The command to perform.
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Command(const XmlNode *node);

    /**
     * Request to be inserted into the list to be notified
     * of a change in a data tag
     *
     * @param tag      Tag for who is requesting the subscription informaiton.
     * @param Process  The process name of who wants the data
     *               requested.
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
     *         BEP_FAILURE_RESPONSE == Error adding subscriber
     * @since Version 2.0
     */
    virtual const std::string Subscribe(const std::string &tag, const std::string &Process);

    /**
     * Request to be removed from the list to be notified
     * of a change in a data tag
     *
     * @param tag     Tag for who is requesting to be unsubscribed.
     * @param Process The process name of who wanted the data
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully removed
     *         BEP_FAILURE_RESPONSE == Error removing subscriber
     * @since Version 2.0
     */
    virtual const std::string Unsubscribe(const std::string &tag, const std::string &Process);

    /**
     * Handle the subscribed for data by the process.
     *
     * @param tag    Tag that was subscribed for
     * @param value  New data value of the tag
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const std::string &tag, const std::string &value);

    /**
     * Get a list of tasks to launch.
     *
     * @param document The configuration file to get the task information from.
     * @return The status of finding all of the tasks specified for launch
     *         on the system. true - all tasks found, false - all tasks not found
     * @since Version 2.2
     */
    bool GetTasksToLaunch(const XmlNode *document);

    /**
     * Method used to determine if the license is  valid.
     *
     * @return The status of the license validation.
     */
    void ValidateLicense(void);

    /**
     * Method used to disable the system if the license is invalid.
     *
     */
    void DisableSystem(void);

    /**
     * Background thread for terminaing License violation
     *
     * @param   threadPointer
     */
    static void * LicenseViolationTerminate(void *threadPointer);

private:

    /**
     * Task table that contains all of the managed tasks.
     */
    TASKTABLE m_taskTable;

    /**
     * List of all of the tasks launched and their pids.
     * @since Version 2.2
     */
    TASKNAMELIST m_taskNameList;

    /**
     * list of all task names that are in the configuration
     * file that could not be located on the hard drive
     */

    TASKNAMELIST m_missingTasks;
    /**
     * The communication channel that is used for processing
     * requests.
     * @since Version 1.0
     */
    IBepCommunication m_comm;

    /**
     * The amount of time to wait until a task is considered
     * unlaunchable or dead.
     * @since Version 2.1
     */
    INT32 m_interTaskDelayTime;

    /**
     * The amount of time to wait until a task is considered
     * unlaunchable or dead.
     * @since Version 2.1
     */
    INT32 m_taskStartupTime;

    /**
     * The amount of time to wait for tasks to terminate.
     * @since Version 2.2
     */
    INT32 m_taskTerminateTime;

    /**
     * Flag to set if the launcher should terminate the launched
     * tasks if an error is detected during launching.
     * @since Version 2.2
     */
    bool m_terminateOnFail;

    /**
     * Flag that is set to enable the system shutdown.  This
     * is set by the configuration parameter EnableSystemShutdown.
     * @since Version 2.5
     */
    bool m_enableSystemShutdown;

    /**
     * Flag that indicates if all of the specified tasks were
     * found.
     * @since Version 1.0
     */
    bool m_foundAllTasks;
    /**
     * Fault server interface used to publich and clear faults in the
     * system
     */
    IFaultServer *m_faultServer;

    /**
     * License timer object pointer
     */
    LicenseTimer *m_licenseTimer;

    /**
     * CCRT License object pointer for validating the Machines license
     */
    CCRTLicense m_lic;

    /**
     * Single shot flag for disableing the system
     */
    bool m_systemDisabled;
    /**
     * Communication object to interface with the NamedDataBroker.
     */
    INamedDataBroker *m_ndb;
};
#endif // LAUNCHER_H_
