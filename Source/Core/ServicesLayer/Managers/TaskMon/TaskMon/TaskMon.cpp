//******************************************************************************
// Description:
//  Class used to gather process data for "registered" proecsses. Processes
//  register via a devvctl() registration message. When the registration
//  messages are received, we get the PID for each process. Using the PID for
//  a process we can retrieve process data via the /proc filesystem using
//  devctl() messages to the Neutino kernel (procnto).
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMon/TaskMon.cpp $
// 
// 3     10/26/06 10:55a Rwiersem
// Changes for the 06102701 core release:
// 
// Added the number of thread and file descriptors associated with each
// task.
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 4     10/11/05 1:54p Bmeinke
// Re-initialize ILogger inside HandleClientRegister() if the client being
// registered is the Logger
//
// 3     9/14/05 3:00p Bmeinke
// Added checks against the task class in HandleClientDown() and
// HandleClientUnregister()
//
// 2     9/14/05 10:17a Bmeinke
// Added a TIMER_PULSE+3 handler to HandleResMgrPulse(); used to remove
// dead tasks from the process list
// IoRead() now returns a task's class attribute
// HandleClientUp() now checks a task's name AND the task's class when
// looking for a matching task
// AdoptTaskPath() now passes the task class to the TaskmonTask
// constructor
//
// 1     7/20/05 8:24p Cward
// Added new TaskMon
//
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
//
// 1     4/25/05 4:11p Bmeinke
//
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <devctl.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/dir.h>        // scandir()
#include <share.h>          // SH_DENYNO

#include "TaskMon.h"
#include "TaskMonMssgDefs.h"

/**
 * Undefine these here because they get defined in the curses.h file
 **/
#undef clear
#undef erase

/**
 * Default constructor
 */
TaskMon::TaskMon() : ResManagerBase(),
    m_keepDead( false), m_showCpu(true), m_showMem(false),
    m_nodeName("localhost"), m_loopTime(-1), m_priority( getprio(0))
{
    // use local node name as the default node to read from
    struct utsname  localNode;
    uname( &localNode);
    m_nodeName = localNode.nodename;

    SetProcessName( "TaskMon");
}

/**
 * Constructor from command line args
 */
TaskMon::TaskMon( int argc, char *argv[]) : ResManagerBase() ,
    m_keepDead( false), m_showCpu(true), m_showMem(false),
    m_nodeName("localhost"), m_loopTime(-1), m_priority( getprio(0))
{
    // use local node name as the default node to read from
    struct utsname  localNode;
    uname( &localNode);
    m_nodeName = localNode.nodename;

    ParseCommandLine( argc, argv, "kl:n:p:");
    SetProcessName( "TaskMon");
}

/**
 * Class destructor
 */
TaskMon::~TaskMon()
{
    TaskDeviceMapItr_t  itr;

    m_updateTimer.Stop();

    // Remove device entries and clean up task memory
    for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
    {
        TaskDevice_t    &dev = itr->second;
        RemoveTaskPath( dev);
    }

}

/**
 * Method used to handle a command line argument. Called from
 * ParseCommandLine() for each comamnd line argument encountered.
 *
 * @param option The command line option (i.e. 'f' if -f specified on command line)
 * @param value  Value associated with the command line option being processed.
 *               May be NULL is no argument is associated with the command line
 *               option
 *
 * @return true if the argument was processed, false if not recognized
 */
bool TaskMon::HandleCommandLineArg(int option, const char *value)
{
    bool retVal = true;

    Log( LOG_FN_ENTRY, "Enter TaskMon::HandleCommandLineArg( %c, %s)\n",
         option, value == NULL ? " " : value);

    switch( option)
    {
    case 'k':
        m_keepDead = (m_keepDead == false);
        break;
    case 'n':
        if( value != NULL)
        {
            m_nodeName = std::string( value);
        }
        break;
    case 'l':
        if( value != NULL)
        {
            m_loopTime = atoi( value);
        }
        break;
    case 'p':
        if( value != NULL)
        {
            m_priority = atoi( value);
        }
        break;
    default:
        retVal = ResManagerBase::HandleCommandLineArg(option, value);
        break;
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleCommandLineArg( %c, %s)\n",
         option, value == NULL ? " " : value);

    return(retVal);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void TaskMon::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter TaskMon::Initialize\n");

    if( document != NULL)
    {
        m_dirList.clear();
        const XmlNodeMap &setupNodes = document->getChild( "Setup")->getChildren();
        try
        {
            // Initialize base class
            ResManagerBase::Initialize( document);
        }
        catch( XmlException &err)
        {
        }

        // Read the list of directories to "discover" currently
        //  tasks from
        try
        {
            const XmlNodeMap    &dirList = setupNodes.getNode( "DiscoverDirs")->getChildren();
            XmlNodeMapCItr      itr;

            for( itr=dirList.begin(); itr!=dirList.end(); itr++)
            {
                const XmlNode *xmlNode = itr->second;
                m_dirList.insert( xmlNode->getName());

                Log( LOG_DEV_DATA, "Adding discover directory <%s>\n", xmlNode->getName().c_str());
            }
        }
        catch(...)
        {
            m_dirList.insert( "application");
            m_dirList.insert( "component");
            m_dirList.insert( "core");
            m_dirList.insert( "logport");
            m_dirList.insert( "mgr");
        }

        try
        {
            m_priority = BposReadInt( setupNodes.getNode( "Priority")->getValue().c_str());
            Log( LOG_DEV_DATA, "Process Priority: %d\n", m_priority);
        }
        catch(...)
        {
        }
        if( (m_priority < sched_get_priority_min(sched_getscheduler(0))) ||
            (m_priority > sched_get_priority_max(sched_getscheduler(0))))
        {
            Log( LOG_ERRORS, "Invalid priority: %d (min=%d, max=%d)\n", m_priority,
                 sched_get_priority_min(sched_getscheduler(0)),
                 sched_get_priority_max(sched_getscheduler(0)));
            m_priority = getprio(0);
        }

        try
        {
            if( m_loopTime < 0)
            {
                m_loopTime = BposReadInt( setupNodes.getNode( "LoopTime")->getValue().c_str());
                Log( LOG_DEV_DATA, "Loop Time: %d\n", m_loopTime);
            }
        }
        catch(...)
        {
        }

        try
        {
            const std::string &keepDead = setupNodes.getNode( "ShowDeadProcesses")->getValue();
            m_keepDead = (stricmp( "Yes", keepDead.c_str()) == 0);
            Log( LOG_DEV_DATA, "Show Dead Tasks: %d\n", m_keepDead);
        }
        catch(...)
        {
        }

        try
        {
            const std::string &showCpu = setupNodes.getNode( "ShowCpuUsage")->getValue();
            m_showCpu = (stricmp( "Yes", showCpu.c_str()) == 0);
            Log( LOG_DEV_DATA, "Show CPU Usage: %d\n", m_showCpu);
        }
        catch(...)
        {
        }

        try
        {
            const std::string &showMem = setupNodes.getNode( "ShowMemUsage")->getValue();
            m_showMem = (stricmp( "Yes", showMem.c_str()) == 0);
            Log( LOG_DEV_DATA, "Show CPU Usage: %d\n", m_showMem);
        }
        catch(...)
        {
        }


    }

    if( m_loopTime < 0) m_loopTime = 250;

    Log( LOG_FN_ENTRY, "Exit TaskMon::Initialize\n");
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool TaskMon::InitManager()
{
    bool    retVal=false;
    const char *devPath = "/dev/TaskMon/TaskMon";

    Log( LOG_FN_ENTRY, "Enter TaskMon::InitManager()\n");

    if( (retVal=ResManagerBase::InitManager()) == true)
    {
        // Allocate a task for reporting overall system CPU load. This
        //  memory will get cleaned up in the destructor when we RemoveTaskPath()
        //  for the kernel task entry.
        m_procEntry = new TaskMonProcTask;
        // Adopt the base path to read system load from
        if( (m_resMgrId=AdoptTaskPath(TASKMON_CLASS_END, m_procEntry)) != -1)
        {
            if( RegisterManager() != -1)
            {
                // Tell any currently running tasks to register with us
                RegisterCurrentTasks();

                if( m_loopTime < 0) m_loopTime = 250;

                // Start the timer used to update the task list
                m_updateTimer.Initialize( m_mssgCoid, SYSTEM_PULSE, TIMER_PULSE, m_priority);
                m_updateTimer.Arm(mSEC_nSEC(m_loopTime), mSEC_nSEC(m_loopTime));

                Log( LOG_DEV_DATA, "Path %s attached as device %d\n", devPath, m_resMgrId);
                retVal = true;
            }
            else
            {
                Log( LOG_ERRORS, "\tError registering resource manager: %s\n",
                     strerror( errno));
            }
        }
        else
        {
            Log( LOG_ERRORS, "Error attaching %s: %s\n", devPath, strerror( errno));
        }
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::InitManager()\n");
    return( retVal);
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t TaskMon::HandleResMgrPulse(io_pulse_t &pulse)
{
    ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
    TaskDeviceMapItr_t  itr;

    if( pulse.code != _PULSE_CODE_DISCONNECT)
    {
        Log( LOG_DETAILED_DATA, "Enter TaskMon::HandleResMgrPulse( %d, %d)\n",
            pulse.code, pulse.value.sival_int);
    }

    if( pulse.code == SYSTEM_PULSE)
    {
        switch( pulse.value.sival_int)
        {
        case TIMER_PULSE:
            retVal = HandleTimerPulse( pulse);
            break;
        case TIMER_PULSE+1:
            // Toggle gathering of CPU information
            m_showCpu = (m_showCpu == false);
            if( false == m_showCpu)
            {
                for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
                {
                    TaskDevice_t    &taskDev = itr->second;
                    TaskMonTask     *task = taskDev.task;
                    task->SetCpuUsage(0LL);
                }
            }
            break;
        case TIMER_PULSE+2:
            // Toggle gathering of memory information
            m_showMem = (m_showMem == false);
            if( false == m_showMem)
            {
                for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
                {
                    TaskDevice_t    &taskDev = itr->second;
                    TaskMonTask     *task = taskDev.task;

                    task->SetMemoryUsage(0LL);
                }
            }
            break;
        case TIMER_PULSE+3:
            // Refresh task list
            for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
            {
                TaskDevice_t    &taskDev = itr->second;
                TaskMonTask     *task = taskDev.task;

                switch(task->GetState())
                {
                case TASKSTATE_END:
                case TASKSTATE_UNREG:
                    // Remove it from our list
                    RemoveTaskPath( taskDev);
                    // Restart enumeration
                    itr = m_taskDevs.begin();
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            retVal = ResManagerBase::HandleResMgrPulse( pulse);
            break;
        }
    }
    else
    {
        retVal = ResManagerBase::HandleResMgrPulse( pulse);
    }

    if( pulse.code != _PULSE_CODE_DISCONNECT)
    {
        Log( LOG_DETAILED_DATA, "Exit TaskMon::HandleResMgrPulse( %d, %d)\n",
            pulse.code, pulse.value.sival_int);
    }

    return( retVal);
}

/**
 * The purpose of this is to give us a place to free our ocb. It is called
 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
 * being done (e.g. iofunc_close_ocb_default causes it to be called).
 * ResManagerBase::ocbFree was registered through our mount structure.
 *
 * @param ocb    Client's Open Context Block for this connection
 * @see ResManagerBase::ocbFree
 */
void TaskMon::OcbFree(resMgrIoOcb_t *ocb)
{
    taskMonMgrIoOcb_t   *myOcb = (taskMonMgrIoOcb_t*)ocb;

    if( myOcb != NULL)
    {
        delete myOcb;
        myOcb = NULL;
    }
}

/**
 * The purpose of this is to give us a place to allocate our own ocb. It is
 * called by ocbCalloc, which in turn is called as a result of an open being done
 * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
 * called). ocbCalloc was registered through our mount structure.
 *
 * @param ctp    Resource manager's context block
 * @param device Attributes for this client connection (an OCB)
 * @return Pointer to newly allocated OCB or NULL if error
 * @see ResManagerBase::ocbCalloc
 */
IOFUNC_OCB_T* TaskMon::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
    taskMonMgrIoOcb_t   *newOcb = new taskMonMgrIoOcb_t;
    return( (IOFUNC_OCB_T*)newOcb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void TaskMon::OcbInit(resMgrIoOcb_t *ioOcb)
{
    taskMonMgrIoOcb_t   *myOcb = (taskMonMgrIoOcb_t*)ioOcb;

    ResManagerBase::OcbInit(&myOcb->baseOcb);

    myOcb->taskName = "Unknown";
    myOcb->taskClass = TASKMON_CLASS_END;
}

/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void TaskMon::OcbFinalize(resMgrIoOcb_t *ioOcb)
{
    taskMonMgrIoOcb_t   *myOcb = (taskMonMgrIoOcb_t*)ioOcb;

    myOcb->taskName = "Unknown";
    myOcb->taskClass = TASKMON_CLASS_END;

    ResManagerBase::OcbFinalize(&myOcb->baseOcb);
}

/**
 * Message handler for _IO_READ I/O messages. Called by ResManagerBase::ioRead
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioRead
 */
int TaskMon::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    TaskMonTask *task = NULL;
    int         retVal=ENOSYS;

    Log( LOG_DETAILED_DATA, "Enter TaskMon::IoRead( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    // Make sure client is reading a full task entry
    if( msg->i.nbytes == sizeof( TaskMonTask_t))
    {
        // Resolve this device back to a Task pointer
        TaskDeviceMapItr_t  itr;
        itr = m_taskDevs.find( ctp->id);
        if( m_taskDevs.end() != itr)    task = itr->second.task;

        // IF the task pointer for this entry is valid
        if( NULL != task)
        {
            TaskMonTask_t   clientTask;
            int             nbytes = sizeof( clientTask);

            // Get stats for this task
            clientTask.m_taskPid = task->GetPid();
            clientTask.m_taskState = task->GetState();
            clientTask.m_cpuUsage = task->GetCpuUsage();
            clientTask.m_memoryUsage = task->GetMemoryUsage();
            clientTask.m_taskClass = task->GetClass();
            clientTask.m_threads = task->GetThreadCount();
            clientTask.m_fds = task->GetOpenFds();

            // Send stats for this process back to the client
            MsgReply( ctp->rcvid, nbytes, &clientTask, nbytes);

            // Say we read this many bytes
            _IO_SET_READ_NBYTES( ctp, nbytes);

            // already done the reply ourselves
            retVal = _RESMGR_NOREPLY;
        }
        else
        {
            Log( LOG_ERRORS, "NULL task for device #%d\n", ctp->id);
            retVal = EFAULT;
        }
    }
    Log( LOG_DETAILED_DATA, "Exit TaskMon::IoRead( pid %d, tid %d)\n", ctp->info.pid, ctp->info.tid);

    return( retVal);
}

/**
 * Function synthesized by the base-layer library when the last
 * close has been received for a particular OCB. Called by ioCloseOcb
 *
 * @param ctp      Dispatch interface pointer
 * @param reserved Reserved for ther base-layer library
 * @param ioOcb    Control block for the client
 * @return 0 if successful
 *         < 0 if error occurs
 * @see ResManagerBase::ioCloseOcb
 */
int TaskMon::IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb)
{
    return( ResManagerBase::IoCloseOcb( ctp, reserved, ioOcb));
}

/**
 * Handler routine for custom device commands (in response to
 * a client devctl() call)
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param ioOcb  Client Open Context Block
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioDevCtl
 */
int TaskMon::IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int                 retVal = _RESMGR_STATUS( ctp, ENOSYS);
    int                 devCmd = msg->i.dcmd & 0xFFFF;
    taskMonMgrIoOcb_t   *taskOcb = (taskMonMgrIoOcb_t*)ioOcb;

    Log( LOG_FN_ENTRY, "Enter TaskMon::IoDevCtl( pid %d, tid %d) cmd=%d\n", ctp->info.pid, ctp->info.tid, devCmd);

    // Hold onto the info for this message
    ioOcb->mssgInfo = ctp->info;

    switch( devCmd)
    {
    case TASK_MON_CLIENT_REG:
        retVal = HandleClientRegister( ctp, msg, taskOcb);
        break;
    case TASK_MON_CLIENT_UP:
        retVal = HandleClientUp( ctp, msg, taskOcb);
        break;
    case TASK_MON_CLIENT_DOWN:
        retVal = HandleClientDown( ctp, msg, taskOcb);
        break;
    case TASK_MON_CLIENT_UNREG:
        retVal = HandleClientUnregister( ctp, msg, taskOcb);
        break;
    default:
        retVal = ResManagerBase::IoDevCtl(ctp, msg, ioOcb);
        break;
    }

    ////////////////////////////////////////////////////////////
    //
    Log( LOG_FN_ENTRY, "Exit TaskMon::IoDevCtl( pid %d, tid %d) cmd=%d\n", ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    return( retVal);
}

/**
 * Handler for the TASK_MON_CLIENT_REG devctl message.
 * Adds a new task entry for the calling client to the appropriate
 * task list, sets the task state to TASKSTATE_REG and sets
 * the task CPU statistics to 0
 *
 * @param ctp     Resource Manager context block
 * @param msg     Received data message data
 * @param taskOcb Client Open Context Block
 *
 * @return EOK if successful, EEXIST if the client attempts to register
 *         using a task name that already exists in the in the same task
 *         class
 */
int TaskMon::HandleClientRegister( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb)
{
    TaskMonReg_t    *regMssg = (TaskMonReg_t*)_DEVCTL_DATA( msg->i);
    std::string     taskName( &regMssg->procName[0], regMssg->nameLen);
    int             retVal;

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleClientRegister( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    taskOcb->taskName = taskName;
    taskOcb->taskClass = regMssg->taskClass;

    // If the Logger is registering
    if( taskName == "LogDaemon")
    {
        // Re-initialize our logging interface
        try
        {
            XmlParser   configParser;
            const XmlNode *configNode = configParser.ReturnXMLDocument(GetConfigFile());
            ILogger::Initialize(configNode);   // initialize the base class
        }
        catch(...)
        {
        }
    }

    // Add a device path for this client (/dev/TaskMon/<TaskClass>/<TaskName>
    int newDev = AdoptTaskPath( taskName, taskOcb->baseOcb.mssgInfo.pid,
                                TASKSTATE_REG, taskOcb->taskClass);
    if( -1 != newDev)
    {
        Log( LOG_DEV_DATA, "Task <%s> with pid %d registered and added to list %d\n",
             taskName.c_str(), taskOcb->baseOcb.mssgInfo.pid, taskOcb->taskClass);
        retVal = EOK;
    }
    else
    {
        retVal = EAGAIN;
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleClientRegister( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Handler for the TASK_MON_CLIENT_RUN devctl message.
 * Updates the task state to TASKSTATE_RUN
 *
 * @param ctp     Resource Manager context block
 * @param msg     Received data message data
 * @param taskOcb Client Open Context Block
 *
 * @return EOK if successful, ENOENT if the client's task name cannot be
 *         found in the class list for the client's class
 */
int TaskMon::HandleClientUp( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb)
{
    TaskMonReg_t    *regMssg = (TaskMonReg_t*)_DEVCTL_DATA( msg->i);
    std::string     taskName( &regMssg->procName[0], regMssg->nameLen);
    TaskDeviceMapItr_t  itr;
    int             retVal;

    Log( LOG_FN_ENTRY, "Enter TaskMon::HandleClientUp( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    taskOcb->taskName = taskName;
    taskOcb->taskClass = regMssg->taskClass;

    retVal = EINVAL;
    // Search for this task name in the device list
    for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
    {
        TaskDevice_t    &dev = itr->second;
        if( (dev.task != NULL) && ( dev.task->GetName() == taskName) &&
            (dev.task->GetClass() == regMssg->taskClass))
        {
            Log( LOG_DEV_DATA, "Task <%s> with pid %d state changed to TASKSTATE_RUN\n",
                 taskName.c_str(), taskOcb->baseOcb.mssgInfo.pid);

            dev.task->SetPid( taskOcb->baseOcb.mssgInfo.pid);
            dev.task->SetState( TASKSTATE_RUN);
            retVal = EOK;
            break;
        }
    }

    // If no matching task
    if( EOK != retVal)
    {
        // Create a new device for this task
        if( -1 != AdoptTaskPath( taskName, taskOcb->baseOcb.mssgInfo.pid,
                                 TASKSTATE_RUN, taskOcb->taskClass))
        {
            // Change the task state to running
            retVal = EOK;
        }
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleClientUp( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Handler for the TASK_MON_CLIENT_DOWN devctl message.
 * Updates the task state to TASKSTATE_STOP
 *
 * @param ctp     Resource Manager context block
 * @param msg     Received data message data
 * @param taskOcb Client Open Context Block
 *
 * @return EOK if successful, ENOENT if the client's task name cannot be
 *         found in the class list for the client's class
 */
int TaskMon::HandleClientDown( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb)
{
    TaskDeviceMapItr_t  itr;
    std::string         &taskName = taskOcb->taskName;
    int                 retVal = EINVAL;

    Log( LOG_FN_ENTRY, "Enter TaskMon::HandleClientDown( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    // Search for this task name in the device list
    for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
    {
        TaskDevice_t    &dev = itr->second;
        if( (dev.task != NULL) && ( dev.task->GetName() == taskName) &&
            (dev.task->GetClass() == taskOcb->taskClass))
        {
            Log( LOG_DEV_DATA, "Task <%s> with pid %d state changed to TASKSTATE_STOP\n",
                 taskName.c_str(), taskOcb->baseOcb.mssgInfo.pid);

            dev.task->SetPid( taskOcb->baseOcb.mssgInfo.pid);
            dev.task->SetState( TASKSTATE_STOP);
            retVal = EOK;
            break;
        }
    }

    // If no matching task
    if( EOK != retVal)
    {
        // Create a new device for this task
        if( -1 != AdoptTaskPath( taskName, taskOcb->baseOcb.mssgInfo.pid,
                                 TASKSTATE_STOP, taskOcb->taskClass))
        {
            // Change the task state to running
            retVal = EOK;
        }
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleClientDown( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Handler for the TASK_MON_CLIENT_DOWN devctl message.
 * Updates the task state to TASKSTATE_UNREG if the "Keep Dead"
 * option is enabled or removes the task from the appropriate class
 * list if the "Keep Dead" option is disabled
 *
 * @param ctp     Resource Manager context block
 * @param msg     Received data message data
 * @param taskOcb Client Open Context Block
 *
 * @return EOK if successful, ENOENT if the client's task name cannot be
 *         found in the class list for the client's class
 */
int TaskMon::HandleClientUnregister( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb)
{
    TaskDeviceMapItr_t  itr;
    std::string         &taskName = taskOcb->taskName;
    int                 retVal = EINVAL;

    Log( LOG_FN_ENTRY, "Enter TaskMon::HandleClientUnregister( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    // Search for this task name in the device list
    for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
    {
        TaskDevice_t    &dev = itr->second;
        if( (dev.task != NULL) && ( dev.task->GetName() == taskName) &&
            (dev.task->GetClass() == taskOcb->taskClass))
        {
            TaskMonTask *task = dev.task;

            // If we want to keep dead processes
            if( true == m_keepDead)
            {
                task->SetPid( taskOcb->baseOcb.mssgInfo.pid);
                task->SetState( TASKSTATE_UNREG);
                Log( LOG_DEV_DATA, "Task <%s> with pid %d state changed to TASKSTATE_UNREG\n",
                     taskName.c_str(), taskOcb->baseOcb.mssgInfo.pid);
            }
            else
            {
                Log( LOG_DEV_DATA, "Task <%s> with pid %d removed from task list\n",
                     taskName.c_str(), taskOcb->baseOcb.mssgInfo.pid);
                RemoveTaskPath( dev);
            }
            retVal = EOK;
            break;
        }
    }

    // If no matching task and we want to display dead tasks
    if( (EOK != retVal) && (true == m_keepDead))
    {
        // Create a new device for this task
        if( -1 != AdoptTaskPath( taskName, taskOcb->baseOcb.mssgInfo.pid,
                                 TASKSTATE_UNREG, taskOcb->taskClass))
        {
            // Change the task state to running
            retVal = EOK;
        }
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::HandleClientUnregister( pid %d, tid %d) cmd=%d\n",
         ctp->info.pid, ctp->info.tid, msg->i.dcmd);

    return( _RESMGR_STATUS( ctp, retVal));
}

/**
 * Handler method for timer pulses. Updates the status of all registered
 * tasks and updates the terminal screen
 *
 * @param pulse  Timer pulse we areresponding to
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t TaskMon::HandleTimerPulse(const io_pulse_t &pulse)
{
    TaskDeviceMapItr_t  itr;
    ResManagerCmd_t     retVal = RES_MGR_CMD_NONE;

    Log( LOG_DETAILED_DATA, "Enter TaskMon::HandleTimerPulse( %d, %d)\n", pulse.code, pulse.value.sival_int);

    // If we should show CPU usage for all tasks
    if( (true == m_showCpu) || ( true == m_showMem))
    {
        // Read CPU usage for individual tasks
        for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
        {
            TaskDevice_t    &currDev = itr->second;
            TaskMonTask     *currTask = currDev.task;

            if( NULL != currTask)
            {
                if( m_showCpu)  currTask->UpdateCpuUsage();
                if( m_showMem)  currTask->UpdateMemoryUsage();
            }
        }
    }
    // If not updating CPU stats for all tasks
    if( false == m_showCpu)
    {
        // Just update the kernel's CPU usage (this will get done
        //  in the above loop if m_showCpu == true)
        m_procEntry->UpdateCpuUsage();
    }

    Log( LOG_DETAILED_DATA, "Exit TaskMon::HandleTimerPulse( %d, %d)\n", pulse.code, pulse.value.sival_int);

    return( retVal);
}

/**
 * Sends a "TaskMon register" pulse to all the processes with
 * names registered under the "discover" directories (m_dirList)
 */
void TaskMon::RegisterCurrentTasks()
{
    DirListCItr_t   itr;
    struct direct   **dirEnts;
    int             dirCount;
    int             ii, jj;

    // Loop through the list of valid directories to scan
    for( itr=m_dirList.begin(), ii=0; itr!=m_dirList.end(); itr++, ii++)
    {
        std::string taskDir( *itr);
        taskDir += "/";

        char dirName[_POSIX_PATH_MAX+1];
        sprintf( dirName, "/dev/name/local/%s", taskDir.c_str());

        // Read all entries under this CCRT name directory
        dirCount = scandir( dirName, &dirEnts, NULL, alphasort);
        // if any entries were found
        if( dirCount > 0)
        {
            for( jj=0; jj<dirCount; jj++)
            {
                const std::string   taskName = taskDir + dirEnts[jj]->d_name;
                Log( LOG_DEV_DATA, "Sending pulse (%d,%d) to process %s\n", SYSTEM_PULSE, TASK_MON_PULSE,
                     taskName.c_str());
                SendRegisterPulse( taskName);
                free( dirEnts[jj]);
            }

            // Free the memory allocated by the library scandir() routine
            free( dirEnts);
        }
    }
}

/**
 * Sends a "TaskMon register" pulse to the given process
 *
 * @param taskName Name of the process to send the pulse to (in the form
 *                 /net/NodeName/dev/name/local/core/Launcher)
 */
void TaskMon::SendRegisterPulse( const std::string &taskName)
{
    int taskId;

#if 1
    taskId = name_open( taskName.c_str(), 0);
#else
    const std::string fullTaskName( "/dev/name/local/" + taskName);
    // Do this because name_open() will not accept a leading '/' in
    //  the path
    taskId = _connect(_NTO_SIDE_CHANNEL,
                      fullTaskName.c_str(),
                      0,
                      O_RDWR,
                      SH_DENYNO,
                      _IO_CONNECT_OPEN,
                      1,
                      _IO_FLAG_RD | _IO_FLAG_WR,
                      _FTYPE_NAME,
                      0, 0, 0, 0, 0, 0);
#endif
    if( -1 != taskId)
    {
        MsgSendPulse( taskId, SIGEV_PULSE_PRIO_INHERIT,
                      SYSTEM_PULSE, TASK_MON_PULSE);
        Log( LOG_DEV_DATA, "Pulse (%d,%d) sent to process %s\n", SYSTEM_PULSE, TASK_MON_PULSE,
             taskName.c_str());
        name_close( taskId);
    }
    else
    {
        Log( LOG_ERRORS, "Unable to name_open '%s'\n", taskName.c_str());
    }
}

/**
 * Adopt a new device path for the given task under the /dev/TaskMon
 * directory
 *
 * @param taskClass Class to which the process belongs. This defines which subdir
 *                  the task's path will be adopted under
 * @param newTask   Pointer to the task we are adopting the device path for
 *
 * @return -1 if an error occurrs
 */
int TaskMon::AdoptTaskPath( TaskMonClass_t taskClass, const TaskMonTask *newTask)
{
    IOFUNC_ATTR_T       *ioAttr;
    TaskDeviceMapItr_t  itr;
    TaskDevice_t        newDev;
    int                 retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter TaskMon::AdoptTaskPath()\n");

    // Make sure task pointer is valid
    if( NULL != newTask)
    {
        switch( taskClass)
        {
        case TASKMON_CLASS_CORE:
            newDev.devPath = "/dev/TaskMon/Tasks/Core/" + newTask->GetName();
            break;
        case TASKMON_CLASS_SERIAL:
            newDev.devPath = "/dev/TaskMon/Tasks/Serial/" + newTask->GetName();
            break;
        case TASKMON_CLASS_COMPONENT:
            newDev.devPath = "/dev/TaskMon/Tasks/Component/" + newTask->GetName();
            break;
        default:
            newDev.devPath = "/dev/TaskMon/" + newTask->GetName();
            break;
        }

        for( itr=m_taskDevs.begin(); itr!=m_taskDevs.end(); itr++)
        {
            TaskDevice_t    &dev = itr->second;
            if( (dev.devPath == newDev.devPath) && (taskClass == newTask->GetClass()))
                break;
        }
        if( m_taskDevs.end() == itr)
        {
            Log( LOG_FN_ENTRY, "\tAdopting path <%s>\n", newDev.devPath.c_str());

            // Initialize this func attrib struct
            ioAttr = new IOFUNC_ATTR_T;
            iofunc_attr_init( ioAttr, _S_IFNAM|0666, NULL, NULL);
            // Let the process manager assign an inode
            ioAttr->inode = 0;
            ioAttr->nbytes = 0;
            ioAttr->mount = NULL;

            newDev.devNo = PathAttach( newDev.devPath.c_str(), ioAttr);
            if( newDev.devNo == -1)
            {
                delete ioAttr;
                retVal = -1;
                Log( LOG_ERRORS, "\tError adopting '%s': %s\n", newDev.devPath.c_str(), strerror( errno));
                ThrowOsException( ENODEV, "Error adopting TaskMon task path");
            }
            else
            {
                newDev.task = const_cast<TaskMonTask*>(newTask);
                m_taskDevs.insert( TaskDeviceMapVal_t( newDev.devNo, newDev));
                Log( "%s adopted as device %d\n", newDev.devPath.c_str(), newDev.devNo);
                retVal = newDev.devNo;
            }
        }
        // We already have a device entry for the given task
        else
        {
            Log( LOG_DEV_DATA, "\tUpdating task entry for path <%s>\n", newDev.devPath.c_str());

            // Just update the task info for this device
            TaskDevice_t    &dev = itr->second;

            // Allocate a new task if we need to
            if( NULL == dev.task)
            {
                dev.task = const_cast<TaskMonTask*>(newTask);
            }
            else
            {
                delete dev.task;
                dev.task = const_cast<TaskMonTask*>(newTask);
            }
            retVal = dev.devNo;
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tERROR: NULL task\n");
        retVal = -1;
    }

    Log( LOG_FN_ENTRY, "Exit TaskMon::AdoptTaskPath()\n");

    return( retVal);
}

/**
 * Adopt a new device path for the given task under the /dev/TaskMon
 * directory
 *
 * @param taskName  Name of the task to create a device entry for
 * @param taskPid   Process ID of the new task
 * @param taskState State of the new task
 * @param taskClass Class to which the process belongs. This defines which subdir
 *                  the task's path will be adopted under
 *
 * @return -1 if an error occurrs
 */
int TaskMon::AdoptTaskPath( const std::string &taskName, pid_t taskPid,
                            TaskState_t taskState, TaskMonClass_t taskClass)
{
    TaskMonTask *newTask;
    int         retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter TaskMon::AdoptTaskPath( %s, %d, %d, %d)\n",
         taskName.c_str(), taskPid, taskState, taskClass);

    newTask = new TaskMonTask( taskName, taskPid, taskState, taskClass, 0.0, 0, 0, 0);

    retVal = AdoptTaskPath( taskClass, newTask);

    newTask->UpdateCpuUsage();

    Log( LOG_FN_ENTRY, "Exit TaskMon::AdoptTaskPath( %s, %d, %d, %d)\n",
         taskName.c_str(), taskPid, taskState, taskClass);

    return( retVal);
}

/**
 * Removes a task's device entry under /dev/Taskmon and free the
 * memory allocated for the corresponding task
 *
 * @param device Task device to unmap
 *
 * @return EOK if successful
 */
int TaskMon::RemoveTaskPath( TaskDevice_t &device)
{
    int retVal = EOK;

    Log( LOG_FN_ENTRY, "Enter TaskMon::RemoveTaskPath()\n");

    if( NULL != device.task)    delete device.task;
    PathDetach( device.devNo);
    m_taskDevs.erase( device.devNo);

    Log( LOG_FN_ENTRY, "Exit TaskMon::RemoveTaskPath()\n");

    return( retVal);
}

