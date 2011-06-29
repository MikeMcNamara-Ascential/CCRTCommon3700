//******************************************************************************
// Description:
//  Class used to gather process data for "registered" proecsses. Processes
//	register via a devvctl() registration message. When the registration 
//	messages are received, we get the PID for each process. Using the PID for
//	a process we can retrieve process data via the /proc filesystem using 
//	devctl() messages to the Neutino kernel (procnto).
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMon/TaskMon.h $
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

#ifndef _TASKMON_H_INCLUDED_
#define _TASKMON_H_INCLUDED_

#include "ResManagerBase.h"
#include "TaskMonTask.h"		
#include "TaskMonProcTask.h"		
#include "TaskMonTaskList.h"
#include "TaskMonDefs.h"
#include "TaskMonTimer.h"

#include <set>
#include <map>

/**
 * Resource Manager Open Control Block type
 * Used to hold data for a client connection
 */
typedef struct _taskMonMgrIoOcb_t
{
	/**
	 * OCB used (and expected) by the base class I/O routines.
	 * !!!!! MUST BE FIRST !!!!!
	 */
	resMgrIoOcb_t	baseOcb;

	/**
	 * Task name registered by the client
	 */
	std::string		taskName;

	/**
	 * Class to which this task belongs
	 * 
	 * @see TaskMonClass_t
	 */
	TaskMonClass_t	taskClass;
	
} taskMonMgrIoOcb_t;

/**
 * Object used to hold a list of directory entry names
 */
typedef std::set<std::string>		DirList_t;
typedef DirList_t::iterator			DirListItr_t;
typedef DirList_t::const_iterator	DirListCItr_t;

/**
 * Structure used to maintain information about a device path
 * for a registered device
 */
typedef struct
{
	/**
	 * Path adopted for this device (relative to /dev/TaskMon)
	 */
	std::string		devPath;

	/**
	 * Path ID (as returned from resmgr_attach())
	 */
	int				devNo;

	/**
	 * Task object to which this device maps
	 */
	TaskMonTask		*task;
	
} TaskDevice_t;

/**
 * Object used to maintain the mapping of registered 
 * client tasks to device entries under /dev/TaskMon/Tasks
 */
typedef std::map< int, TaskDevice_t>		TaskDeviceMap_t;
typedef TaskDeviceMap_t::iterator			TaskDeviceMapItr_t;
typedef TaskDeviceMap_t::const_iterator		TaskDeviceMapCItr_t;
typedef TaskDeviceMap_t::value_type			TaskDeviceMapVal_t;

class TaskMon : public ResManagerBase
{
public:
	/**
	 * Default constructor
	 */
	TaskMon();
	
	/**
	 * Constructor from command line args
	 */
	TaskMon( int argc, char *argv[]);

	/**
	 * Class destructor
	 */
	~TaskMon();

	/**
	 * Reads configuration data from the given XML node
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	
	/**
	 * Perform any server initialization
	 *
	 * @return true if successful
	 *         false if error
	 */
	virtual bool InitManager();
	
protected:

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
	virtual bool HandleCommandLineArg(int option, const char *value);
	
	/**
	 * Handles pulses delivered to our resource manager I/O channel
	 *
	 * @param pulse  Pulse received on the I/O channel
	 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
	 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
	 *         RES_MGR_CMD_NONE for all other pulses
	 */
	virtual ResManagerCmd_t HandleResMgrPulse(io_pulse_t &pulse);

	/**
	 * The purpose of this is to give us a place to free our ocb. It is called
	 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
	 * being done (e.g. iofunc_close_ocb_default causes it to be called).
	 * ResManagerBase::ocbFree was registered through our mount structure.
	 *
	 * @param ocb    Client's Open Context Block for this connection
	 * @see ResManagerBase::ocbFree
	 */
	virtual void OcbFree(resMgrIoOcb_t *ocb);

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
	virtual IOFUNC_OCB_T * OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device);

	/**
	 * Method used to initialize an allocated OCB structure. Called
	 * immediately after the OCB is allocated
	 *
	 * @param ioOcb  OCB structure to be initialized
	 */
	virtual void OcbInit(resMgrIoOcb_t *ioOcb);

	/**
	 * Method used to finalize an allocated OCB structure. Called immediately
	 * before the OCB is destroyed
	 *
	 * @param ioOcb  OCB structure to be finalized
	 */
	virtual void OcbFinalize(resMgrIoOcb_t *ioOcb);

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
	virtual int IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);
	
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
	virtual int IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb);

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
	virtual int IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);
    
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
	int HandleClientRegister( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb);
	
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
	int HandleClientUp( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb);
	
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
	int HandleClientDown( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb);
	
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
	int HandleClientUnregister( resmgr_context_t *ctp, io_devctl_t *msg, taskMonMgrIoOcb_t *taskOcb);

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
	ResManagerCmd_t HandleTimerPulse(const io_pulse_t &pulse);
	
	/**
	 * Sends a "TaskMon register" pulse to all the processes with
	 * names registered under the "discover" directories (m_dirList)
	 */
	void RegisterCurrentTasks();

	/**
	 * Sends a "TaskMon register" pulse to the given process
	 * 
	 * @param taskName Name of the process to send the pulse to (in the form
	 *                 /net/NodeName/dev/name/local/core/Launcher)
	 */
	void SendRegisterPulse( const std::string &taskName);

	/**
	 * Adopt a new device path for the given task under the /dev/TaskMon
	 * directory
	 * 
	 * @param taskClass Class to which the process belongs. This defines which subdir
	 *                  the task's path will be adopted under
	 * @param newTask   Pointer to the task we are adopting the device path for
	 * 
	 * @return EOK if successful
	 */
	int AdoptTaskPath( TaskMonClass_t taskClass, const TaskMonTask *newTask);

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
	int AdoptTaskPath( const std::string &taskName, pid_t taskPid,
					   TaskState_t taskState, TaskMonClass_t taskClass);
	
	/**
	 * Removes a task's device entry under /dev/Taskmon and free the
	 * memory allocated for the corresponding task
	 * 
	 * @param device Task device to unmap
	 * 
	 * @return EOK if successful
	 */
	int RemoveTaskPath( TaskDevice_t &device);
	
private:
	/**
	 * Mapping of registered task names to device path
	 */
	TaskDeviceMap_t	m_taskDevs;

	/**
	 * Timer used to signal us to update task stats
	 */
	TaskMonTimer	m_updateTimer;

	/**
	 * Flag used to indicate whether or not we want to maintain 
	 * device entries for 'dead' tasks or not
	 */
	bool			m_keepDead;

	/**
	 * Flag used to indicate whether or not we want to calculate
	 * CPU usage for monitored tasks
	 */
	bool			m_showCpu;

	/**
	 * Flag used to indicate whether or not we want to gather
	 * memory usage for monitored tasks
	 */
	bool			m_showMem;

	/**
	 * Name of the node being monitored
	 */
	std::string		m_nodeName;

	/**
	 * Interval (ms) at which we will update task stats
	 */
	int32_t			m_loopTime;

	/**
	 * Priority to run at
	 */
	int				m_priority;

	/**
	 * Task entry used to get statistics about the kernel. The kernel
	 * stats are used to determine the overall system CPU load
	 */
	TaskMonProcTask	*m_procEntry;

	/**
	 * List of directories to "discover" currently running tasks from
	 */
	DirList_t		m_dirList;
};

#endif // _TASKMON_H_INCLUDED_

