//*************************************************************************
// Description:
//  Pure Virtual Proxy for a test component.  It is designed to be extended by
//  specialized forms of component proxys.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/ComponentProxy.h $
// 
// 8     1/17/07 4:01p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a component name agrgument to CommandActivate().
// 
// 7     10/25/06 4:18p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a "reuseComponents" variable to Initialize().  Added
// CommandActivate() and CommandDeactivate().  These are used to keep the
// test components running when a new drive curve starts.
// 
// 6     5/08/06 7:17p Cward
// switched printf statements to log statements. Added Log function to
// allow component manager to use as well.
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     6/09/04 11:25p Cward
// Refactored App layer includes to remove implimentation. Moved header
// comment block to front of routine for better differences. Updated the
// header comment block to the new standard
//
// 3     3/04/04 5:12p Cward
// added logger
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 2     12/17/03 9:28a Derickso
// Latest updates from testing at JTE and merging with Ford and Korea.
//
// 1     8/21/03 9:02a Derickso
// 05WK CCRT Project
//
// 2     8/06/03 12:56p Khykin
// Updating with latest changes from Flatrock.
//
// 1     6/20/03 9:43a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 1     4/22/03 10:25a Khykin
// Initial Working Checkin.
//
// 1     1/27/03 11:09a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 11    11/11/02 2:56p Khykin
// Updated proxy fault handling.
//
// 10    10/25/02 6:11a Khykin
// Removing depracated code.
//
// 9     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 8     3/06/02 1:53p Kh
// Adding debug statements.
//
// 7     8/28/01 6:53p Kh
// Adding a BepSemaphoreTryPost method.
//
// 6     8/24/01 3:24p Kh
//
// 5     8/17/01 2:35p Kh
// Checking in for weekly backup.
//
// 4     7/26/01 7:48p Kh
// Adding support for the defined system pulses.
//
// 3     7/19/01 7:44a Kh
// Changing CommandObjective function declaration.
//
// 2     7/12/01 3:20p Kh
// Tested and ready to go.
//
// 1     7/06/01 5:35p Kh
// Just checking in for the weekend.  Will debug next week.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef COMPONENTPROXY_H
#define COMPONENTPROXY_H
//==============================================================================
#define USE_CONDVAR 0

using namespace std;

#include "IBepCommunication.h"
#include "Debuggable.h"
#include "Status.h"
#include "Bepos.h"
#include "BepSemaphore.h"
#include "BepSync.h"
#include "ILogger.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef BepCondVar< bool>       CpCondVar;

/**
 * Component Proxy Class that provides the methods to
 * command objectives and send update results to the
 * sequencer.
 *
 * @author Kevin Hykin
 * @version 1.4
 */
class ComponentProxy : public Debuggable, public Status
{
public:
    /**
     * Vehicle Component Manager constructor.
     *
     * @param  logger  Pointer to a logging interface
     *                 so logging to file can occur.
     * @since Version 1.0
     */
    ComponentProxy(ILogger *logger);

    /**
     * Vehicle Component Manager destructor.
     * @since Version 1.0
     */
    virtual ~ComponentProxy(void);

    /**
     * Initialization method that launches the component
     * that will be managed, connects to it for communication,
     * and connects to the results server.
     *
     * @param component The component information necessary to launch a component
     *                  and connect to it as a client.
     * @param reuseComponents
     *                 Flag indicating if the components should be killed/relaunched (false) or
     *                 if they should be reused from one test to another (true)
     */
    virtual void Initialize(const XmlNode *component, string driveCurveRootDir, 
                            bool reuseComponents=false);

    /**
     * Method that the thread waits on until an objective
     * is commanded.  It will send the request to the
     * component, wait for the response, clear the objective
     * and then send the response back to the sequencer.
     * @since Version 1.0
     */
    virtual void ProcessObjective(void)=0;

    /**
     * Method that will be used to command a test objective.
     * The Component Proxy will set the internal objective
     * information and release the component proxy thread
     * to process the objective.
     *
     * @param objective The objective that needs to be commanded.
     * @since Version 1.0
     */
    virtual void CommandObjective(const XmlNode *objective);

    /**
     * This is a special command in the fact that it will
     * send a terminate signal to the component.
     * @since Version 1.0
     */
    void CommandTerminate(void);

    /**
     * This will command the component to Abort the current
     * processing and return to an idle state.
     * @since Version 1.1
     */
    void CommandAbort(void);

    /**
     * This will command the component to Reload its configuration
     * file.
     * @since Version 1.1
     */
    void CommandReloadConfiguration(void);

    /**
     * This will command the component to Reset its current status
     * and prepare to execute commands.
     * @since Version 1.1
     */
    void CommandReset(void);

    /**
     * This will command the component to activate itself
     * 
     * @param componentName
     *               The name of the component from the drive curve
     */
    void CommandActivate( const std::string &componentName);

    /**
     * This will command the component to deactivate itself
     * and return to an idle state.
     */
    void CommandDeactivate(void);
    
    /**
     * Get the name of the current process that the proxy
     * is serving.
     *
     * @return The name of the process that the proxy is serving.
     * @since Version 1.0
     */
    const std::string GetName(void);

    /**
     * Get the name of the type of component that is being
     * managed. (Ex. If a Teves component is being managed
     * "ABS" would be returned)
     *
     * @return The name of the type of component that is being
     *         managed.
     */
    const std::string GetMyName(void);

    /**
     * Get the thread ID of the thread that is using
     * the component proxy.
     *
     * @param id     The id of the thread the proxy is running in.
     * @since Version 1.0
     */
    void SetThreadID(const pthread_t id);

    /**
     * Get the thread I.D. of the thead that is managing
     * the component proxy.
     *
     * @return The managing thread I.D.
     * @since Version 1.0
     */
    const pthread_t GetThreadID(void);

    /**
     * This will return the executable name of the component
     * that is being managed.
     *
     * @return The name of the component being managed.
     */
    const std::string GetExecutableName(void);

    /**
     * Get the ComponentProxys type.
     *
     * @return The type of component proxy that is running.
     *         Foreground or Background
     * @since Version 1.2
     */
    const std::string GetType(void);
    /**
     * Debug function to return Memory arena
     *
     */
    int ReturnMemArena(void);
    /**
     * Provide an interface to the logging utility.
     *
     * @param mask  Logging mask to apply.
     * @param message  Message to be written to the log file.
     */
    void Log( UINT32 mask, const char *message);

protected:

    /**
     * Send the received command to the component, wait for
     * the results and then update them and send notification
     * to the results server.
     *
     * @return The status of the operation.
     *         0 - successful
     * @since Version 1.0
     */
    INT32 SendCommand(void);

    /**
     * Set the name of the process that the proxy is serving.
     *
     * @param name   The name of the process that the proxy is serving.
     * @since Version 1.0
     */
    void SetName(const std::string name);

    /**
     * Get the name of the name of the type of component
     * that is being handled by the component proxy. (Ex
     * it would be ABS if a Teves component is being managed)
     *
     * @param name   The name of the component being managed.
     */
    void SetMyName(const std::string name);

    /**
     * Launch the specified component and verify that it
     * is running.
     *
     * @param component Component information from the configuration file.
     * @since Version 1.0
     */
    void LaunchComponent(const XmlNode *component);

    /**
     * Load the components configuration file and configure
     * the component proxy component channel using the
     * configuration of the component.
     * Note: this changes the configuration type from server
     * to client
     *
     * @param component The component information that is used to load the
     *                  configuration file.
     * @since Version 1.0
     */
    void ComponentCommInit(const XmlNode *component);

    /**
     * Update the results of a command.  This will set
     * the Results attribute of the objective and send
     * the results to the results server.
     *
     * @param status The status of the previously commanded operation.
     * @return The status of the update.
     */
    INT32 UpdateResults(INT32 status);

    /**
     * If component will not terminate, kill it because
     * it must DIE, DIE, DIE!
     * @since Version 1.3
     */
    void KillComponent(void);

    /**
     * Slays a test component process.
     * 
     * @param compToSlay Name of the test component executable to slay (BaseBrakeTC.qxe)
     */
    void SlayComponent( const std::string &compToSlay);
    
    /**
     * Launches a new test component process
     * 
     * @param exeName Path to the component executable file
     * @param cfgFile Path to the config file for the test component to load
     * 
     * @return PID of the launched test component process or -1 if an error occurrs
     */
    INT32 SpawnComponent( const std::string &exeName, const std::string &cfgFile);
    
    /**
     * This will set the executable name of the component
     * that is being managed.
     *
     * @param name   The name of the executable component that is being
     *               managed.
     */
    void SetExecutableName(const std::string name);

    /**
     * Set the ComponentProxy type.
     *
     * @param type   The type of component proxy that is running.
     *               Foreground or Background
     *
     * @since Version 1.2
     */
    void SetType(const std::string type);

    /**
     * Create the object to use for signaling an objective is ready.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE CreateNewObjectiveSignal(void);

    /**
     * Delete the object used to signal that a new objective is
     * ready.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE DeleteNewObjectiveSignal(void);

    /**
     * Wait until a new objective is ready.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE WaitForNewObjective(void);

    /**
     * Signal that a new objective has been received.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE SignalNewObjective();

    /**
     * Create the object to use for signaling an objective has
     * been processed.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE CreateNewProcessSignal(void);

    /**
     * Delete the object used to signal that an objective has
     * been processed.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE DeleteNewProcessSignal(void);

    /**
     * Wait for the current objective processing to be completed.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE WaitForObjectiveDone(void);

    /**
     * Signal that the objective is complete.
     *
     * @return The status of the operation.
     */
    BEP_STATUS_TYPE SignalObjectiveDone(void);

    /**
     * The objective that was commanded by the component
     * manager.
     * @since Version 1.0
     */
    XmlNode *m_objective;

    /**
     * BepSemaphore to wait on when waiting for a command.
     * @since Version 1.0
     */
    BepSemaphore *m_semCommand;

    /**
     * BepSemaphore to wait on when waiting for a component to
     * complete a process.
     * @since Version 1.0
     */
    BepSemaphore *m_semProcess;

    /**
     * BepSemaphore to wait on when sending a pulse to the component.
     * @since Version 1.0
     */
    BepSemaphore *m_semPulse;

    /**
     * Communication connection to the component.  This is
     * used to command test objectives and receive the results.
     * @since Version 1.0
     */
    IBepCommunication m_componentComm;

    /**
     * Communication connection to the component for sending pulses.
     * This is used so that the component can abort during a test.
     * @since Version 1.0
     */
    IBepCommunication m_componentCommPulse;

    /**
     * The generic name of the component that is being
     * commanded.  (Ex. Teves would be refered to as ABS)
     * @since Version 1.0
     */
    std::string m_myName;

    /**
     * Communication connection to the results server (in most
     * cases this is the sequencer).  This is used to
     * update the commanded objective results.
     * @since Version 1.0
     */
    IBepCommunication m_resultsComm;

    /**
     * Logging interface from object that instantiated this thread.  This will allow
     * logging to occur.  Previously could only printf, not very
     * useful if you are not present to observe the failure.
     *
     * @since Version 1.4
     */
     ILogger *m_logger;
    /**
     * The thread I.D. of the thread that the component
     * proxy is running in.
     * @since Version 1.0
     */
    pthread_t m_thread;

    /**
     * The name of the component that the proxy is managing.
     * @since Version 1.1
     */
    std::string m_componentName;

    /**
     * The name of the executable file so the destructor
     * can slay it if it was not properly terminated.
     * @since Version 1.2
     */
    std::string m_executableName;

    /**
     * The type of component that is running.
     * @since Version 1.2
     */
    std::string m_type;

    /**
     * The PID of the component that is being managed.
     * @since Version 1.3
     */
    int m_componentPID;
    
    /**
     * Flag indicating if we want to re-use existing test components instead of
     * always killing/relaunching them
     */
    bool m_reuseComponents;

    /**
     * The type of component that is running.
     * @since Version 1.2
     */
    std::string m_driveCurveRootDir;
};

#endif //COMPONENTPROXY_H

