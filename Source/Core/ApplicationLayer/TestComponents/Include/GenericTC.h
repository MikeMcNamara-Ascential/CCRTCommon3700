//******************************************************************************
// Description:
//  Vehicle TestComponent base class that defines the functionality that each
//  vehicle component needs.
//
//==============================================================================
// Copyright (c) 1999-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Include/GenericTC.h $
// 
// 4     1/21/08 11:17a Derickso
// Updated DisplayTimedPrompt to take optional values to display as part
// of the prompt.
// 
// 3     1/09/08 2:12p Derickso
// Added DisplayTimedPrompt that accepts an int prompt box parameter.
// 
// 2     12/14/07 7:49a Derickso
// Removed tandem axle support from WHEELINFO.  Roller speed reads do not
// work correctly for two axle machines.
// 
// 1     11/02/07 8:17a Derickso
// Added support for tandem wheel speeds.
// Added functions for accessing the force file similar to the speed file.
// 
// 22    10/30/07 1:54p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// - Added BaseBrakeToolTest as a friend class.
// - Made some methods virtual for testing.
// 
// 21    5/01/07 11:17a Rwiersem
// Changes for the 07050201 core release:
// 
// Removed BaseBrakeTool as a friend class.  Moved the public methods
// called by BaseBrakeTool from protected to public so they can be called
// without being a friend class.
// 
// 1     10/25/07 1:50p Derickso
// Updated comment content.
// 
// 20    2/22/07 8:58a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// - Removed #if 0 blocks.
// - Added AccelerateVehicleToSpeed().
// 
// 19    1/17/07 3:33p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added ReloadConfiguration().
// - Added TestStepReadMemStats() and UpdateTaskStats().
// - Added m_configCopy.
// 
// 18    10/25/06 3:58p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added m_regName and m_regAlias to hold the IPC names
//     - Added AddIPCLink() and RemoveIPCLink() 
//     - Added Activate(), Deactivate(), ShouldReloadConfigFile(), and 
//       WarmInitialize().  These methods are used to keep the test
// components
//       running when a new drive curve starts.
// 
// 17    9/05/06 3:51p Rwiersem
// Changes for the 06090601 core release:
// 
// - Made CommonMachineTest a friend class
// 
// 16    4/26/06 6:59p Cward
// Removed old commented out code,fixed ParkBrakeDirection logic, changed
// printfs to Logs, added Default values for many Paramters moved atob to
// GeneralUtilites
//
// 15    4/13/06 3:25a Cward
// check in changes (2 and 3) below
//
// 3     4/01/06 4:57p Bmeinke
// Added several 'init' methods that get caleed at the beginning of the
// park brake and aprk pawl tests to allow the Ford brake component to
// start monitoring the brake switch AFTER the "Shift" and "Apply park
// brake" prompts have been displayed.
//
// 2     3/20/06 10:48a Bmeinke
// Added constant unit strings that match the strings used by the GUI
// Added SendTestResultWithDetail() and SendSubtestResultWithDetail()
// method that take a TestResultDetails object
// Added GetElapsedTime() to return the elapsed time for a test step
//
// 14    2/15/06 7:13p Cward
// Make SpeedforTome a callible test step. Add setup operator responses
// Added MotorController initialization commands to some test steps. Added
// atob method to convert ascii to boolean. Added default value parameter
// to GetTestStepParameter methods.
//
// 2     12/16/05 1:44p Bmeinke
// Added a new "atob" method to convert a string to a boolean
// Modified the GetTestStepParameter() methods to take an optional default
// value to be returned if the desired parameter does not exist in the
// TestStepInfo section or the Parameters section
//
// 1     11/17/05 10:35a Bmeinke
// Updating Tata to latest core
//
// 13    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 12    7/20/05 8:39p Cward
// Added support for new TaskMon
//
// 11    5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 10    5/09/05 5:00p Gpattison
// Removed the member variable m_parse because it was no longer needed
// since the Initialize(filename) functionality was moved to the base
// class.
//
// 10    4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 9     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 8     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 2     12/22/04 8:14p Bmeinke
// Added all park brake and park pawl methods (and supporting routines)
// Added TestStepEnableMotorLoading() and TestStepDisableMotorLoading()
//
// 7     9/01/04 9:15p Cward
// Added Oveloaded functions for Get Parameter, Get Test Step Info, and
// Get Test Step Parameter, to allow transparently getting values from
// either test step info or parameter lists. Removed inlines from
// prototypes.
//
// 6     6/24/04 12:41a Cward
// Made Get Roll Speed virtual
//
// 5     6/09/04 11:10p Cward
// Refactored to remove implementation. Updated header comment block to
// new standard
//
// 4     5/25/04 3:40p Cward
// Removed definition of eeprom test result. Made Abort function virtual.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 5     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 4     10/28/03 7:12a Derickso
// Added a few convenience methods for prompting operator
// to speed and maintaining speed.
//
// 2     9/22/03 4:45p Derickso
// Latest version with updated architecture.
// TestComponent no longer exists.
//
// 2     8/26/03 10:25a Derickso
// Added secondary timeout methods to allow a test step
// to have multiple timeout timers.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 3     8/19/03 8:23a Derickso
// Updated TimeRemaining to take an optional argument to be used for
// determining if time is remaining.  If the optional argument is not
// provided, the timeout attribute for the test step will be used.
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//
// 1     6/20/03 9:43a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
// 2     5/19/03 7:53a Khykin
// Updated comments and logging.
//
// 1     4/22/03 10:25a Khykin
// Initial Working Checkin.
//
// 4     4/11/03 7:44a Khykin
// Promoted the GetParamter methods to public.
//
// 3     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:02:54  tbochene
//
//    PooBag
//
//
// 2     2/08/03 6:37a Khykin
// Promoted the methods for retrieving fault information to public.
//
// 1     1/27/03 11:11a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 19    11/22/02 3:11p Khykin
// Added color definitions.
//
// 18    11/11/02 2:57p Khykin
// Added prompt locking methods.
//
// 17    11/04/02 6:51a Khykin
// Updated the method to get the test step result to return untested if
// the result does not exist.
//
// 16    10/21/02 1:37p Khykin
// Adding support for prioritized prompts and system tag support.  Also
// added the method to create the test result name by appending the test
// component name on the result. (this is for localization)
//
// 15    8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 13    5/24/02 1:47p Kh
// Updating due to addition of system tags.
//
// 12    4/01/02 9:22a Kh
// Adding definition for the wheels for use in reading/writing wheel
// information.
//
// 11    3/28/02 9:36a Kh
// Adding exception handling if prompt tags are not in the TestStep.
//
// 10    3/26/02 8:13p Kh
// Adding a TestSkip static string.
//
// 9     3/25/02 6:17p Kh
// Updating due to changes in the base classes.
//
// 8     3/06/02 1:52p Kh
// Adding method to get TestStep results and to UpdatePrompts.
//
// 7     2/06/02 12:16a Kh
// Adding a wrapper for the named data broker interface.
//
// 6     2/01/02 4:41p Kh
// Adding a wrapper for the named data broker interface.
//
// 5     1/25/02 5:49p Kh
// Overloading the display test prompt method.
//
// 2     1/03/02 8:30a Kh
// Debugging complete.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTC_H
#define GenericTC_H

#include <string>
#include <time.h>
#include <fcntl.h>
#include "BepTimer.h"
#include "BepServer.h"
#include "IBepCommunication.h"
#include "INamedDataBroker.h"
#include "IMotorController.h"
#include "IPromptServer.h"
#include "IFaultServer.h"
#include "ITestResultServer.h"
#include "BepDefs.h"
#include "BepSemaphore.h"
#include "LogDefs.h"

extern char *__progname;

using namespace std;

// physical constants
#define     KPH_MPH                 1.609344            // KPH per MPH
#define     KPA_PSI                 6.894757291         // KPA per PSI
#define     KGF_LBS                 0.4359237           // KG FORCE per LBS
#define     N_LBS                   4.4482216152        // NEWTONS per LBS
#define     KN_LBS                  0.0044482216152     // KILONEWTONS per LBS
#define     DEG_GRAD                0.9                 // [deg] per [grad]
#define     DEG_RAD                 57.2957795131       // [deg] per [rad]
#define     CM_INCH                 2.54                // [cm/inch]
#define     INCH_FEET               12
#define     FEET_MILE               5280

// the roller names, in order of the way they are provided by the ICM
const static string rollerName[] = {"LeftFront", "RightFront",
    "LeftRear", "RightRear",
    "LeftTandem", "RightTandem"};

// static test result definitions
const static std::string testPass(BEP_PASS_RESPONSE);
const static std::string testFail(BEP_FAIL_RESPONSE);
const static std::string testAbort(BEP_ABORT_RESPONSE);
const static std::string testError(BEP_ERROR_RESPONSE);
const static std::string testInvalid(BEP_INVALID_RESPONSE);
const static std::string testUnsupported(BEP_UNSUPPORTED_RESPONSE);
const static std::string testTimeout(BEP_TIMEOUT_RESPONSE);
const static std::string testFatalFail(BEP_FATALFAIL_RESPONSE);
const static std::string testHardwareFail(BEP_HARDWAREFAIL_RESPONSE);
const static std::string testSoftwareFail(BEP_SOFTWAREFAIL_RESPONSE);
const static std::string testSkip(BEP_SKIP_RESPONSE);
const static std::string testFailure(BEP_SKIP_RESPONSE);
const static std::string testSuccess(BEP_SUCCESS_RESPONSE);
const static std::string testNotImplemented(BEP_NO_IMPLEMENTED_RESPONSE);

// static color definitions
const static std::string colorBlack("black");
const static std::string colorWhite("white");
const static std::string colorRed("red");
const static std::string colorBlue("blue");
const static std::string colorCyan("cyan");
const static std::string colorGreen("green");
const static std::string colorMagenta("magenta");
const static std::string colorOrange("orange");
const static std::string colorPink("pink");
const static std::string colorYellow("yellow");
const static std::string colorDarkGrey("darkgrey");
const static std::string colorGrey("grey");
const static std::string colorLightGrey("lightgrey");

// Unit strings
const static std::string unitsINCHTEN("inch*10");
const static std::string unitsCMTEN("cm*10");
const static std::string unitsINCH("inch");
const static std::string unitsCM("cm");
const static std::string unitsFEET("feet");
const static std::string unitsMETER("meter");
const static std::string unitsPSI("psi");
const static std::string unitsPA("pa");
const static std::string unitsMI("mi");
const static std::string unitsKM("km");
const static std::string unitsMPH("mph");
const static std::string unitsKPH("km/h");
const static std::string unitsLBF("lbf");
const static std::string unitsKGF("kgf");
const static std::string unitsKN("kn");
const static std::string unitsN("N");
const static std::string unitsLB("lb");
const static std::string unitsKG("kg");
const static std::string unitsMPHPS("mph/s");
const static std::string unitsKPHPS("km/h/s");
const static std::string unitsFLB("ftop");
const static std::string unitsDEGF("F");
const static std::string unitsDEGC("C");

// Channel name prefix
const static std::string ipcNamePrefix( "/dev/name/local");

/**
 * An array of sampled values
 */
typedef vector< float >             DATAARRAY;
/**
 * An iterator used to navagate through a sampled values array
 */
typedef DATAARRAY::iterator         DATAARRAYITR;
/**
 * The list of wheels and thier brake force array
 */
typedef vector< DATAARRAY >         WHEELDATAARRAY;
/**
 * An iterator used to navagate through the wheel array
 */
typedef WHEELDATAARRAY::iterator    WHEELARRAYITR;
/**
 * Wheel information structure used to read the wheel
 * information from the ICM.
 */
typedef struct
{
    float lfWheel;
    float rfWheel;
    float lrWheel;
    float rrWheel;
    float ltWheel;
    float rtWheel;
} WHEELINFO;
/**
 * Wheel information structure used to read the wheel
 * information from the ICM.
 */
typedef struct
{
    float lfWheel;
    float rfWheel;
    float lrWheel;
    float rrWheel;
} WHEELINFO2AXLE;

/**
 * Base TestComponent class. This provides all of the base functionality for 
 * test components in the system.
 *
 * @author Kevin Hykin
 */
class GenericTC : public BepServer
{
    friend class TestComponentTest;
    friend class TcaseTool;
    friend class CommonMachineTest;
    friend class GenericTcTest;
    friend class BaseBrakeToolTest;

public:
    /** Enum used for waiting for target speed. */
    enum Direction
    {
        UP = 1,
        DOWN = 2
    };
    /** Enum used for specificying the drive axle. */
    enum DriveAxle
    {
        FWD, RWD, TWD
    };

    /**
     * The maximum count value that the ICM will report. This is an unsigned 28-bit integer
     */
    static const uint32_t   m_maxIcmCountValue;

    /**
     * <b>Description:</b>
     * GenericTC constructor.
     */
    GenericTC();

    /**
     * <b>Description:</b>
     * GenericTC destructor.
     */
    virtual ~GenericTC();

    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Initialization method that parses the configuration file and initializes 
     * the test component.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BepServer::Initialize() </li>
     *      </ul>
     *
     * @param fileName Configuration file which contains the component setup information.
     */
    virtual void Initialize(const std::string &fileName);

    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Initialize the base test component. The IPC name will be re-created.  The objectives, test steps and parameters
     * will be saved off into member variables.  The fault list and prompt list will also be saved to member variables.
     * Communication objects will be created to the NamedDataBroker, TestResultServer, PromptServer and FaultServer.
     * <p>
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/Communication/Name - Used to create the IPC name for this test component. </li>
     *     <li> Objectives - List of objectives supported by this component. </li>
     *     <li> TestSteps - List aof tests supported by this component. </li>
     *     <li> Setup/Name - Name of the component. </li>
     *     <li> Setup/Parameters - List of parameters supported by this component. </li>
     *     <li> Setup/FaultList - Fault list to be used when reporting test failures. </li>
     *     <li> Setup/Prompts - Prmpts supported by this component. </li>
     *     <li> Setup/StatusCheckItems - List of items to check for system status. </li>
     *     <li> Setup/TestDomain - Domain of the test component.  Typically Foreground or Background. </li>
     *     <li> Setup/SystemTagsToReset - List of tags to reset to nominal values at the start of a test sequence. </li>
     *     </ul>
     * 
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> KeepTestStatusOnExit - Flag to determine if the test component status should be retained when the test
     *                                 component is shutdown. </li>
     *     <li> ICMName - Name of the ICM process to connect to for wheel speeds, forces, etc.</li>
     *     </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetConfigFile() </li>
     *      <li> RemoveIPCLink() </li>
     *      <li> CreateUniqueIpcName() </li>
     *      <li> BepServer::Initialize() </li>
     *      <li> SetComponentName() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetFileModificationTime() </li>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateTestStatus() </li>
     *      <li> SetICMManagerName() </li>
     *      <li> GetICMManagerName() </li>
     *      <li> SetRollerCount() </li>
     *      <li> SystemReadInt() </li>
     *      <li> AddIPCLink() </li>
     *      <li> SetStatus() </li>
     *      </ul>
     * 
     * @param config The configuration data to use to initialize the test component.
     */
    virtual void Initialize(const XmlNode *config);

    /**
     * Activate the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Indicate that the component should re-activate itself. This should happen at the start of a test when the 
     * component is needed.  Test status variables will be reset.  The config file will also be checked to determine
     * if it should be reloaded.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShouldReloadConfigFile() </li>
     *      <li> BepServer::Activate() </li>
     *      <li> InitializationComplete() </li>
     *      <li> WarmInitialize() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetProcessName() </li>
     *      </ul>
     */
    virtual void Activate(void);

    /**
     * Deactivate the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Indicate that the component should de-activate itself. This should happen at the end of a test when the 
     * component is no longer needed.  The ICM files will be closed and the IPC link removed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BepServer::Deactivate() </li>
     *      <li> CloseSpeedFile() </li>
     *      <li> CloseWheelDistanceFile() </li>
     *      <li> RemoveIPCLink() </li>
     *      </ul> 
     */
    virtual void Deactivate(void);

    /**
     * Reload the configuration file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reload the configuration file and reset the base component objects.  The fault list, prompt list and status check
     * items list will be cleared out.  The base class will then be called to complete the reloading of the configuration 
     * file.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BepServer::ReloadConfiguration() </li>
     *      </ul>
     */
    virtual void ReloadConfiguration(void);

    /**
     * Main execution loop for the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Method used to process incoming requests.  It will only return if a terminate message is received.  As long as
     * the component status is not set to BEP_TERMINATE_STATUS, this function will continue to loop, looking for new 
     * commanded objectives to execute.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetStatus() </li>
     *      <li> GetCommand() </li>
     *      <li> CommandObjective() </li>
     *      <li> SetObjectiveResult() </li>
     *      </ul>
     * 
     * @param terminateFlag Flag to tell if the task should  terminate. (this is unused)
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

    /**
     * Register the component with the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the component to perform any system registration functions necessary.  The component testing status will
     * be set to TEST_STATUS_IN_PROGRESS.  The base class will then be called to complete system registration.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateTestStatus() </li>
     *      <li> BepServer::Register() </li>
     *      </ul>
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful.
     *         BEP_FAILURE_RESPONSE if the registration was not successful.
     */
    virtual const std::string Register(void);

    /**
     * Process an incoming command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Process an incoming command.  Any commands other than objectives will be passed to the base class for processing.
     * Commands will only be processed if the component status is not set to BEP_DEACTIVATE_STATUS.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetStatus() </li>
     *      <li> SetCommand() </li>
     *      <li> BepServer::Command() </li>
     *      </ul>
     * 
     * @param temp   The command to be processed.
     * 
     * @return Result of processing the command.  BEP_UNAVAILABLE_RESPONSE will be returned if the current status is
     *         BEP_DEACTIVATE_STATUS.
     */
    virtual const std::string Command(const XmlNode *temp);

    /**
     * Terminate the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Terminate the component.  This performs any cleanup that the destructor would not and sets the state of the 
     * component to terminate.  If test status is not being preserved after the component terminates, the status will
     * be set to TEST_STATUS_NOT_STARTED.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> KeepTestStatusOnExit - Flag to determine if the current test status should be preserved after the 
     *                                 component terminates. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateTestStatus() </li>
     *      <li> BepServer::Terminate() </li>
     *      </ul>
     */
    virtual void Terminate(void);

    /**
     * Reset the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The overall test result for the component will be reset to a "Test in Progress" state.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> ResetOverallResult() </li>
     *      </ul>
     */
    virtual void Reset(void);

    /**
     * Abort test processing.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Abort the current test processing.  If testing is in process for this component, the overall result will be set
     * to Abort as well as the component status.  If the overall result is set to Pass, it will not be updated to Abort.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetStatus() </li>
     *      <li> GetPreviousOverallResult() </li>
     *      <li> SendOverallResult() </li>
     *      <li> SetStatus() </li>
     *      </ul>
     */
    virtual void Abort(void);

    /**
     * Get the name of the requested fault tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the fault name for a specified fault.  The fault will be looked up in the fault list.  The name will then be
     * returned to the calling function.  If the fault is not found in the list, "(fault) Name Unavailable" will be
     * returned.
     * <p>
     * @param fault The fault to look up.
     * 
     * @return The name of the specified fault.
     *         "(fault) Name Unavailable" if the fault name is not found.
     */
    std::string GetFaultName(const std::string fault);

    /**
     * Get the code for the requested fault tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the fault code for a specified fault.  The fault tag will be looked up in the fault list.  The code will then 
     * be returned to the calling functions.  If the fault is not found in the list, 
     * <p>
     * @param fault The fault to look up.
     * 
     * @return The code of the specified fault.
     *         "(fault) Code Unavailable" if the fault name is not found.
     */
    std::string GetFaultCode(const std::string fault);

    /**
     * Get the description of the requested fault tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the description for the specified fault. The fault tag will be looked up in the fault list.  The description 
     * will then be returned to the calling function.  If the fault is not found in the list, "(fault) Description 
     * Unavailable"will be returned to the calling function.
     * <p>
     * @param fault The fault tag to look up.
     * 
     * @return The description of the specified fault tag.
     *         "(fault) Description Unavailable" if the fault name is not found.
     */
    std::string GetFaultDescription(const std::string fault);

    /**
     * Get the failure status mask of the requested fault tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the mask that indicates failure for the specified fault. The fault tag will be looked up in the fault list.  The failure status mask 
     * will then be returned to the calling function.  If the fault is not found in the list, "(fault) Description 
     * Unavailable"will be returned to the calling function.
     * <p>
     * @param fault The fault tag to look up.
     * 
     * @return The failure status mask of the specified fault tag.
     *         "(fault) Description Unavailable" if the fault name is not found.
     */
    std::string GetFaultFailureStatusMask(const std::string fault);

    /**
     * Get the value of the specified parameter.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list.  The value of the
     * parameter will then be returned to the calling function.
     * <p>
     * @param dataTag The parameter to look up.
     * 
     * @return The value of the requested parameter.
     *         An empty string if the parameter was not found.
     */
    const std::string GetParameter(const std::string dataTag);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.  If the parameter is not found in the list, an exception will be thrown.  It is the responsibility of 
     * the calling function to handle the exception.
     * <p>
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter.
     */
    std::string GetParameter( const std::string &dataTag, std::string &value) throw(BepException);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.  If the parameter is not found in the list, an exception will be thrown.  It is the responsibility of 
     * the calling function to handle the exception.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter.
     */
    INT32 GetParameter ( const std::string &dataTag, int &value) throw(BepException);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.  If the parameter is not found in the list, an exception will be thrown.  It is the responsibility of 
     * the calling function to handle the exception.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter.
     */
    float GetParameter ( const std::string &dataTag, float &value) throw(BepException);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.  If the parameter is not found in the list, an exception will be thrown.  It is the responsibility of 
     * the calling function to handle the exception.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter.
     */
    bool GetParameter ( const std::string &dataTag, bool &value) throw(BepException);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.
     * <p>
     * @param dataTag The parameter to look up.
     * 
     * @return The value of the requested parameter or 0 if the parameter was not found in the parameter list.
     */
    const INT32 GetParameterInt(const std::string dataTag);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.
     * <p>
     * @param dataTag The parameter to look up.
     * 
     * @return The value of the requested parameter or 0 if the parameter was not found in the parameter list.
     */
    const float GetParameterFloat(const std::string dataTag);

    /**
     * Get the value of the requested parameter.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get a parameter value by name.  The parameter will be looked up in the parameter list and returned to the calling
     * function.
     * <p>
     * @param dataTag The parameter to look up.
     * 
     * @return The value of the requested parameter or False if the parameter was not found in the parameter list.
     */
    const bool GetParameterBool(const std::string dataTag);


    const float GetVehicleParameter(const string &parameterName, const float &defValue /*= 0.0*/);

    const int GetVehicleParameter(const string &parameterName, const int &defValue /*= 0*/);

    const string GetVehicleParameter(const string &parameterName, const string &defValue = "");

    const int ConvertIntUnitsIfNecessary(const XmlNode * parameterNode);

    const float ConvertFloatUnitsIfNecessary(const XmlNode * parameterNode);
    
    const string ConvertStringUnitsIfNecessary(const XmlNode * parameterNode);

    /**
     * Get the current test step name.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the name of the current test step.
     * <p>
     * @return The name of the current test step.
     */
    virtual const std::string GetTestStepName(void);

    /**
     * Get the value of a test step parameter.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The parameter will be looked up in the test step information first.  If the parameter is not found there, 
     * the parameter will be looked up in the parameter list.  If the parameter is not found there, the default parameter 
     * passed in will be returned.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * @param defVal  The default value to return if the requested parameter does not exist in either location.
     *
     * @return The value of the requested parameter.
     */
    std::string GetTestStepParameter( const std::string &dataTag, std::string &value, const std::string& defVal="");

    /**
     * Get the value of a test step parameter.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The parameter will be looked up in the test step information first.  If the parameter is not found there, 
     * the parameter will be looked up in the parameter list.  If the parameter is not found there, the default parameter 
     * passed in will be returned.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * @param defVal  The default value to return if the requested parameter does not exist in either location.
     *
     * @return The value of the requested parameter.
     */
    INT32 GetTestStepParameter( const std::string &dataTag, int &value, const int defVal=0);

    /**
     * Get the value of a test step parameter.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The parameter will be looked up in the test step information first.  If the parameter is not found there, 
     * the parameter will be looked up in the parameter list.  If the parameter is not found there, the default parameter 
     * passed in will be returned.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * @param defVal  The default value to return if the requested parameter does not exist in either location.
     *
     * @return The value of the requested parameter.
     */
    float GetTestStepParameter( const std::string &dataTag, float &value, const float defVal=0.0);

    /**
     * Get the value of a test step parameter.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The parameter will be looked up in the test step information first.  If the parameter is not found there, 
     * the parameter will be looked up in the parameter list.  If the parameter is not found there, the default parameter 
     * passed in will be returned.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param dataTag The parameter to look up.
     * @param value   The value of the parameter to return.
     * @param defVal  The default value to return if the requested parameter does not exist in either location.
     *
     * @return The value of the requested parameter.
     */
    bool GetTestStepParameter( const std::string &dataTag, bool &value, const bool defVal=false);

    /**
     * Convert units of parameter if required.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get Units attribute.  Determine if conversion is required.  Proceed accordingly
     * <p>
     * @param units   Units to convert from.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter - converted if required.
     */
    const std::string ConvertToEnglishUnits( std::string &units, const std::string &value) throw(BepException);

    /**
     * Convert units of parameter if required.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get Units attribute.  Determine if conversion is required.  Proceed accordingly
     * <p>
     * @param units   Units to convert from.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter - converted if required.
     */
    INT32 ConvertToEnglishUnits ( std::string &units, int &value) throw(BepException);

    /**
     * Convert units of parameter if required.    
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get Units attribute.  Determine if conversion is required.  Proceed accordingly
     * <p>
     * @param units   Units to convert from.
     * @param value   The value of the parameter to return.
     * 
     * @return The value of the requested parameter - converted if required.
     */
    float ConvertToEnglishUnits ( std::string &units, float &value) throw(BepException);

    /**
     * Get the prompt box.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the prompt box in which to display the specified prompt tag.  If the prompt box attribute is missing, the 
     * prompt will be displayed in prompt box 1 by default.
     * <p>
     * @param promptTag Prompt tag to be displayed.
     * 
     * @return Prompt box in which to display the prompt.
     */
    virtual INT32 GetPromptBox(const string &promptTag);

    /**
     * Get the specified prompt value.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the value of the prmpt tag to be displayed.  If the tag is not in the prompt list, <i>promptTag</i> will be 
     * returned.
     * <p>
     * @param promptTag Prompt tag to be displayed.
     *
     * @return Prompt string to display.
     */
    virtual string GetPrompt(const string &promptTag);

    /**
     * Get the prompt priority.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the priority of the prompt to be displayed.  If the tag is not in the prompt list, a the priority will be 
     * defaulted to 0.
     * <p>
     * @param promptTag Prompt tag to be displayed.
     * 
     * @return The priority of prompt to display.
     */
    virtual INT32 GetPromptPriority(const string &promptTag);

    /**
     * Get the prompt display time.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the duration of the timed prompt to be displayed.  If the tag is not in the prompt list, the duration will be 
     * defaulted to 0.
     * <p>
     * @param promptTag Prompt tag to be displayed.
     * 
     * @return The display of prompt to display.
     */
    virtual INT32 GetPromptDuration(const string &promptTag);

    /**
     * Tag the wheel data array.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Tag the ICM data arrays and return the index of the tag.  If there is an error a BepException will be thrown.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TagIndex - System tag to use for commanding the ICM to tag the data arrays. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      </ul>
     * 
     * @param arrayTag The tag to use in the data array for noting the data sample.
     * 
     * @return The index of the tag that was placed in the ICM arrays.
     */
    INT32 TagArray(const string arrayTag);

    /**
     * Read the ICM data arrays.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read a specified data array and store the values by wheel.  An example of this would the /dev/ForceArray.  
     * The data is stored by index and each index has LF, RF, LR, RR, LT, RT data.  This method will store the 
     * information in a local data array then in the force array.
     * <br>
     * Each sample in the array will contain an entry for:
     * lf, rf, lr, and rr for a 4 roller machine
     * lf, rf, lr, rr, lt, and rt for a 6 roller machine
     * This will be read into the force arrays for processing.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      </ul>
     * 
     * @param startIdx  The index of first relevant sample.
     * @param endIdx    The index of last relevant sample.
     * @param dataArray An array to store the returned data.
     * 
     * @return BEP_STATUS_SUCCESS if the read was successful.
     *         BEP_STATUS_SOFTWARE if the specified data array could not be opened
     *         BEP_STATUS_FAILURE if there were any other failures.
     */
    virtual INT32 ReadDataArrays(const string &array, int startIdx, int endIdx, WHEELDATAARRAY &dataArray);

    /**
     * Get the roller count.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the number of rollers on the machine.
     *
     * @return The number of rollers on the machine.
     */
    const UINT32 &GetRollerCount(void);

    /**
     * Get the number of wheels on the vehicle.
     * 
     * @return Number of wheels on the vehicle being tested.
     */
    const UINT32 GetWheelCount(void);

    /**
     * Store the number of rollers on the machine.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the number of rollers on the machine.  If this number is less than 4, the <i>rollerCount</i> will not be
     * stored and an error will be logged. </li>
     * <p>
     * @param rollerCount The number of rollers on the machine.
     */
    void SetRollerCount(UINT32 rollerCount);

    /**
     * Get the process name of the ICM manager.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the registered process name of the ICM manager.
     * <p>
     * @return The registered process name of the ICM manager.
     */
    const string &GetICMManagerName(void);

    /**
     * Disengage the machine.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This will put the motors in torque zero mode and disengage the clutches.  A BepException will be thrown if the 
     * motors could not be placed into torque 0 mode or if the clutches could not be disengaged.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Clutch - System tag for reading/setting the clutch status. </li>
     *      <li> Declutch - System tag for reading/setting the declutch status. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b> 
     *      <ul>
     *      <li> SystemCommand() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the machine was successfully disengaged.
     *         Otherwise, BEP_STATUS_FAILURE.
     */
    virtual BEP_STATUS_TYPE Disengage(void);

    /**
     * Engage the machine.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The motors will be placed into Boost mode and the clutches will be engaged.  The rollers must be at zero speed
     * in order to engage the clutches.  This function only checks for zero speed, but does not wait for zero speed to
     * be achieved.  A BepException will be thrown if the rollers could not be clutched or if the motors could not be
     * placed into Boost mode.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Clutch - System tag for reading/setting the clutch status. </li>
     *      <li> Declutch - System tag for reading/setting the declutch status. </li>
     *      <li> Zerospeed - System tag for determining if the rollers are at zero speed. </li>
     *      <li> Boost - System tag for enabling/disabling motor boost. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemCommand() </li>
     *      <li> SystemWrite() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the machine was successfully engaged.
     *         Otherwise, BEP_STATUS_FAILURE.
     */
    virtual BEP_STATUS_TYPE Engage(void);

    /**
     * Open the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the ICM force file for reading wheel forces.  A flag will also be set indicating if the file has been 
     * opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentForces - Name of the ICM force file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetForceFileOpen() </li>
     *      <li> IsForceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenForceFile(void);

    /**
     * Close the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the force file.  A flag will be set indicating the force file was closed.  If the force file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsForceFileOpen() </li>
     *      <li> SetForceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the force file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the force file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseForceFile(void);

    /**
     * Check if the force file has been opened.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the force file is open.
     * <p>
     * @return Flag indicating if the force file is open.
     */
    bool& IsForceFileOpen(void);

    /**
     * Open the speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the speed file for reading wheel speeds.  A flag will also be set indicating if the file has been opened 
     * successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentSpeeds - Name of the current wheels speed file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenSpeedFile(void);

    /**
     * Close the ICM speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the speed file.  A flag will be set indicating the speed file was closed.  If the speed file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the speed file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the speed file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseSpeedFile(void);

    /**
     * Check if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the speed file is open.
     * <p>
     * @return Flag indicating if the speed file is open.
     */
    bool& IsSpeedFileOpen(void);

    /**
     * Open the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the wheel distance file for reading wheel distances.  A flag will also be set indicating of the file has 
     * been opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentDistances - Name of the ICM file containing the current wheel distances. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenWheelDistanceFile(void);

    /**
     * Close the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the wheel distance file.  A flag will be set indicating the wheel distance file was closed.  If the wheel
     * distance file was not opened, nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the wheel distance file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the wheel distance file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseWheelDistanceFile(void);

    /**
     * Check if the wheel distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the wheel distance file is open.
     * <p>
     * @return The flag indicating if the wheel distance file is open.
     */
    bool& IsWheelDistanceFileOpen(void);

    /**
     * Update the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Update the current overall test result.  It contains all of the logic to determine if the current state should be
     * updated based on the result passed in.  For example, a Pass result will overwrite a Test In Progress result, but
     * a Pass result will not overwrite a Fail result.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param result        The new overall test result.
     * @param overallResult The current overall result of the component.
     */
    void UpdateResult (const std::string &result, std::string &overallResult);

    /**
     * Update the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Update the current overall test result.  It contains all of the logic to determine if the current state should be
     * updated based on the result passed in.  For example, a Pass result will overwrite a Test In Progress result, but
     * a Pass result will not overwrite a Fail result.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @param result        The new overall test result.
     * @param overallResult The current overall result of the component.
     */
    void UpdateResult (INT32 result, std::string &overallResult);

    /**
     * Update the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Update the current overall test result.  It contains all of the logic to determine if the current state should be
     * updated based on the result passed in.  For example, a Pass result will overwrite a Test In Progress result, but
     * a Pass result will not overwrite a Fail result.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @param result        The new overall test result.
     * @param overallResult The current overall result of the component.
     */
    void UpdateResult(INT32 result, INT32 &overallResult);

    /**
     * Reset the system tags.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The list of system tags from the configuration file will be reset to initial values.
     */
    void ResetSystemTags(void);

    /**
     * Determine if the test step should be skipped.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return true if this test step should just return without running.  Usually happens if this is a retest that 
     * previously passed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetOverallResult() </li>
     *      <li> IsRetest() </li>
     *      <li> GetPreviousOverallResult() </li>
     *      <li> GetTestStepResult() </li>
     *      </ul>
     *
     * @return Flag indicating if the current test step should be skipped.
     */
    virtual bool ShortCircuitTestStep(void);

    /**
     * Display any prompts contained in the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * If prompt tags are conatined in the test step info, they will be displayed for the operator.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Prompt1 - Prompt to display in prmpt box 1. </li>
     *      <li> Prompt1Priority - Priority of the prompt to display in box 1. </li>
     *      <li> Prompt1BGColor - Background color for box 1. </li>
     *      <li> Prompt1Value1 - Value to display with the prompt in box 1. </li>
     *      <li> Prompt1Value2 - Value to display with the prompt in box 1. </li>
     *      <li> Prompt1Value3 - Value to display with the prompt in box 1. </li>
     *      <li> Prompt2 - Prompt to display in prmpt box 2. </li>
     *      <li> Prompt2Priority - Priority of the prompt to display in box 2. </li>
     *      <li> Prompt2BGColor - Background color for box 1. </li>
     *      <li> Prompt2Value1 - Value to display with the prompt in box 2.  </li>
     *      <li> Prompt2Value2 - Value to display with the prompt in box 2.  </li>
     *      <li> Prompt2Value3 - Value to display with the prompt in box 2.  </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> DisplayPrompt() </li>
     *      </ul>
     * 
     * @return The status of updating the prompts.
     */
    const INT32 UpdatePrompts(void);

    /**
     * Check if this is a retest.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The flag indicating if this is an in-cycle retest will be returned to the calling function. 
     * <p>
     * @return Flag indicating if an in-cycle retest is being performed.
     */
    bool IsRetest(void);

    /**
     * Get the previous overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the overall result of the previous in-cycle test run.
     *
     * @return The previous overall test result for the component.
     */
    const string GetPreviousOverallResult(void);

    /**
     * Store the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current overall test result will be stored.  The test status will be set to BEP_COMPLETE_STATUS.  The retest
     * status will be set to true.  The previous overall result will also be updated to the overall test result for the
     * in-cycle test just completed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetOverallResult() </li>
     *      <li> SetStatus() </li>
     *      <li> SetRetestStatus() </li>
     *      <li> SetPreviousOverallResult() </li>
     *      <li> GetOverallResult() </li>
     *      <li> UpdateTestStatus() </li>
     *      </ul>
     * 
     * @param result The overall test result.
     */
    virtual void SendOverallResult(const std::string result = BEP_PASS_RESPONSE);

    /**
     * Get the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The overall result of the test component will be returned to the calling function.
     * <p>
     * @return The overall test result of the component.
     */
    const std::string GetOverallResult(void);

    /**
     * Set the overall test result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the overall result of the test component.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateResult() </li>
     *      </ul>
     * 
     * @param result The overall test result to store.
     */
    void SetOverallResult(const std::string result);

    /**
     * Complete component initialization.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Indicate that the component initialization is complete and that registration can be performed.
     */
    virtual void InitializationComplete(void);

    /**
     * Write data to a system tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Write the specified <i>value</i> to the provided system <i>tag</i>.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param tag   The data tag to write.
     * @param value The value to write to the system.
     * 
     * @return The status of the write.
     */
    virtual INT32 SystemWrite(const std::string &tag, const std::string &value);

    /**
     * Write data to a system tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Write the specified <i>value</i> to the provided system <i>tag</i>.  The data will be converted to a string format
     * and sent to the appropraite version of SystemWrite.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemWrite() </li>
     *      </ul>
     *
     * @param tag   The data tag to write.
     * @param value The value to write to the system.
     * 
     * @return The status of the write.
     */
    INT32 SystemWrite(const std::string &tag, const int value);

    /**
     * Write data to a system tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Write the specified <i>value</i> to the provided system <i>tag</i>.  The data will be converted to a string format
     * and sent to the appropraite version of SystemWrite.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemWrite() </li>
     *      </ul>
     *
     * @param tag   The data tag to write.
     * @param value The value to write to the system.
     * 
     * @return The status of the write.
     */
    INT32 SystemWrite(const std::string &tag, const float value);

    /**
     * Write data to a system tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Write the specified <i>value</i> to the provided system <i>tag</i>.  The data will be converted to a string format
     * and sent to the appropraite version of SystemWrite.  A value of true will be converted to "1" and a value of
     * false to "0".
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemWrite() </li>
     *      </ul>
     *
     * @param tag   The data tag to write.
     * @param value The value to write to the system.
     * 
     * @return The status of the write.
     */
    INT32 SystemWrite(const std::string &tag, const bool value);

    /**
     * Send a system command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the system to perform some work.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param tag   The command to be performed.
     * @param value The value of the command.
     * 
     * @return The status of commanding the system.
     */
    INT32 SystemCommand(const std::string &tag, const std::string &value);

    /**
     * Send a system command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the system to perform some work.  The commanded <i>value</i> will first be converted to a string format
     * and then the appropriate SystemCommand function will be called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      </ul>
     *
     * @param tag   The command to be performed.
     * @param value The value of the command.
     * 
     * @return The status of commanding the system.
     */
    INT32 SystemCommand(const std::string &tag, const int value);

    /**
     * Send a system command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the system to perform some work.  The commanded <i>value</i> will first be converted to a string format
     * and then the appropriate SystemCommand function will be called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      </ul>
     *
     * @param tag   The command to be performed.
     * @param value The value of the command.
     * 
     * @return The status of commanding the system.
     */
    INT32 SystemCommand(const std::string &tag, const float value);

    /**
     * Send a system command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the system to perform some work.  The commanded <i>value</i> will first be converted to a string format
     * and then the appropriate SystemCommand function will be called.  <i>value</i> will be converted to "1" if it is
     * true or "0" if it is false.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemCommand() </li>
     *      </ul>
     *
     * @param tag   The command to be performed.
     * @param value The value of the command.
     * 
     * @return The status of commanding the system.
     */
    INT32 SystemCommand(const std::string &tag, const bool value);

    /**
     * Get the INamedDataBroker object being used by this component.
     * <p><b>Category</b> Utility
     * <p><b>Description:</b><br>
     * Get the INamedDataBroker object that was instantiated by the class.
     * 
     * @return A pointer to the INamedDataBroker object.
     */
    INamedDataBroker* GetNamedDataBroker(void);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>tag</i>. If the requested <i>tag</i> does not exist in the test
     * step information, an empty string will be returned to the calling function.
     * <p>
     * @param tag The data that is required from the test step information.
     * 
     * @return The value of the information that was requested. If the tag does
     *         not exist, an empty string will be returned.
     */
    const std::string GetTestStepInfo(const std::string tag);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>dataTag</i>. If the requested <i>dataTag</i> does not exist in the test
     * step information, an empty string will be returned to the calling function.
     * <p>
     * @param dataTag The data that is required from the test step information.
     * @param value The value of the <i>dataTag</i> from the test step information.
     * 
     * @return The value of the information that was requested. If the tag does not exist, an empty string will be returned.
     * 
     * @throws BepException A BepException is thrown if the desired data tag does not exist in the test step info.
     */
    std::string GetTestStepInfo( const std::string &dataTag, std::string &value) throw(BepException);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>dataTag</i>. If the requested <i>dataTag</i> does not exist in the test
     * step information, an empty string will be returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param dataTag The data that is required from the test step information.
     * @param value The value of the <i>dataTag</i> from the test step information.
     * 
     * @return The value of the information that was requested. 
     * 
     * @throws BepException A BepException is thrown if the desired data tag does not exist in the test step info.
     */
    int GetTestStepInfo( const std::string &dataTag, int &value) throw(BepException);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>dataTag</i>. If the requested <i>dataTag</i> does not exist in the test
     * step information, an empty string will be returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param dataTag The data that is required from the test step information.
     * @param value The value of the <i>dataTag</i> from the test step information.
     * 
     * @return The value of the information that was requested. 
     * 
     * @throws BepException A BepException is thrown if the desired data tag does not exist in the test step info.
     */
    float GetTestStepInfo( const std::string &dataTag, float &value) throw(BepException);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>dataTag</i>. If the requested <i>dataTag</i> does not exist in the test
     * step information, an empty string will be returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param dataTag The data that is required from the test step information.
     * @param value The value of the <i>dataTag</i> from the test step information.
     * 
     * @return The value of the information that was requested. 
     * 
     * @throws BepException A BepException is thrown if the desired data tag does not exist in the test step info.
     */
    bool GetTestStepInfo( const std::string &dataTag, bool &value) throw(BepException);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>tag</i>. If the requested <i>tag</i> does not exist in the test
     * step information, 0 will be returned to the calling function.
     * <p>
     * @param tag The data that is required from the test step information.
     * 
     * @return The value of the information that was requested. 
     */
    const float GetTestStepInfoFloat(const std::string tag);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>tag</i>. If the requested <i>tag</i> does not exist in the test
     * step information, 0 will be returned to the calling function.
     * <p>
     * @param tag The data that is required from the test step information.
     * 
     * @return The value of the information that was requested. 
     */
    const INT32 GetTestStepInfoInt(const std::string tag);

    /**
     * Get information from the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the test step information indicated by <i>tag</i>. If the requested <i>tag</i> does not exist in the test
     * step information, False will be returned to the calling function.
     * <p>
     * @param tag The data that is required from the test step information.
     * 
     * @return The value of the information that was requested. 
     */
    const bool GetTestStepInfoBool(const std::string tag);

    /**
     * Get the current wheel speeds.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current wheels speeds for the LF, RF, LR, RR, LT and RT rollers.
     * <br><b><i>Note:</i></b> Only the wheel speeds from rollers actually attached to the machine will be stored in 
     * <i>wheelSpeed</i>.  If the machine is not equipped with tandem rollers, only the four main roller speeds 
     * (LF, RF, LR, RR) will be stored in <i>wheelSpeed</i>.
     * <p>
     * <b>Functions Called:</b>
     *     <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetWheelSpeeds() </li>
     *     </ul>
     * 
     * @param wheelSpeed The data structure to populate with the roller speeds.
     *
     * @return Status of reading the roller speeds from the system.
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel speeds were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if the speed file mutex could not be acquired or an exception was thrown. </li>
     *          <li> BEP_STATUS_FAILURE for any other errors. </li>
    *           </ul>
     */
    virtual BEP_STATUS_TYPE GetWheelSpeeds(WHEELINFO &wheelSpeed);

    /**
     * Get the current roller speeds.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current wheel speeds from the system.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentSpeeds - Name of the file conatining the current roller speeds. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> OpenSpeedFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     *
     * @param wheelSpeed The wheel speed array to populate with the current speeds.
     * 
     * @return  Status of reading the roller speeds.
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel speeds were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if the speed file mutex could not be acquired or an exception was thrown. </li>
     *          <li> BEP_STATUS_FAILURE for any other errors. </li>
     *          </ul>
     */
    INT32 GetWheelSpeeds(float wheelSpeed[]);

    /**
     * Report the results of a sub-test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a detailed subtest result to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param testName          The test name to use to label the result.
     * @param result            The result of the test.
     * @param description       The description for the result.
     * @param code              The associated code with the test step.
     * @param firstDetail       The name of the first detail.
     * @param firstDetailValue  The value of the first detail.
     * @param firstDetailUnits  The units for the first value.
     * @param secondDetail      The name of the second detail.
     * @param secondDetailValue The value of the second detail.
     * @param secondDetailUnits The units for the second value.
     * @param thirdDetail       The name of the third detail.
     * @param thirdDetailValue  The value of the third detail.
     * @param thirdDetailUnits  The units for the third value.
     * @param fourthDetail      The name of the fourth detail.
     * @param fourthDetailValue The value of the fourth detail.
     * @param fourthDetailUnits The units for the fourth value.
     * @param fifthDetail       The name of the fifth detail.
     * @param fifthDetailValue  The value of the fifth detail.
     * @param fifthDetailUnits  The units for the fifth value.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendSubtestResultWithDetail(const std::string &testName, 
                                      const std::string result, const std::string description = "",
                                      const std::string code = "0000",
                                      const std::string firstDetail = "",const std::string firstDetailValue = "",
                                      const std::string firstDetailUnits = "",
                                      const std::string secondDetail = "", const std::string secondDetailValue = "",
                                      const std::string secondDetailUnits = "",
                                      const std::string thirdDetail = "", const std::string thirdDetailValue = "",
                                      const std::string thirdDetailUnits = "",
                                      const std::string fourthDetail = "", const std::string fourthDetailValue = "",
                                      const std::string fourthDetailUnits = "",
                                      const std::string fifthDetail = "", const std::string fifthDetailValue = "",
                                      const std::string fifthDetailUnits = "");

    /**
     * Report the results of a sub-test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a detailed subtest result to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> UpdateResult() </li>
     *      </ul>
     *
     * @param testName          The test name to use to label the result.
     * @param result            The result of the test.
     * @param description       The description for the result.
     * @param code              The associated code with the test step.
     * @param firstDetail       The name of the first detail.
     * @param firstDetailValue  The value of the first detail.
     * @param firstDetailUnits  The units for the first value.
     * @param secondDetail      The name of the second detail.
     * @param secondDetailValue The value of the second detail.
     * @param secondDetailUnits The units for the second value.
     * @param thirdDetail       The name of the third detail.
     * @param thirdDetailValue  The value of the third detail.
     * @param thirdDetailUnits  The units for the third value.
     * @param fourthDetail      The name of the fourth detail.
     * @param fourthDetailValue The value of the fourth detail.
     * @param fourthDetailUnits The units for the fourth value.
     * @param fifthDetail       The name of the fifth detail.
     * @param fifthDetailValue  The value of the fifth detail.
     * @param fifthDetailUnits  The units for the fifth value.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendSubtestResultWithDetail(const std::string &testName, INT32 result, const std::string description = "",
                                      const std::string code = "0000",
                                      const std::string firstDetail = "",const std::string firstDetailValue = "",
                                      const std::string firstDetailUnits = "",
                                      const std::string secondDetail = "", const std::string secondDetailValue = "",
                                      const std::string secondDetailUnits = "",
                                      const std::string thirdDetail = "", const std::string thirdDetailValue = "",
                                      const std::string thirdDetailUnits = "",
                                      const std::string fourthDetail = "", const std::string fourthDetailValue = "",
                                      const std::string fourthDetailUnits = "",
                                      const std::string fifthDetail = "", const std::string fifthDetailValue = "",
                                      const std::string fifthDetailUnits = "");

    /**
     * Report the results of a sub-test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a detailed subtest result to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CreateTestResultName() </li>
     *      <li> GetComponentName() </li>
     *      <li> ITestResultServer::WriteTestResult() </li>
     *      </ul>
     * 
     * @param testName    The test name to use to label the result.
     * @param result      The result of the test.
     * @param details     List of test result detail values to be added to the test result node.
     * @param description The description for the result if necessary.
     * @param code        The associated code with the test step.
     *
     * @return The status of sending the test result.
     */
    INT32 SendSubtestResultWithDetail(const std::string &testName, const std::string result, const TestResultDetails &details,
                                      const std::string description = "", const std::string code = "0000");

    /**
     * Get the name of the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The name of the component will be returned to the calling function.
     * <p>
     * @return The name of the component.
     */
    const std::string GetComponentName(void);

    /**
     * Report a test result to the TestResultServer.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a test result to the TestResultServer.  The test result will be reported under the name of 
     * GetComponentName() + GetTestStepName().  If a description is not supplied, the description from the current
     * test step will be used.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test result. </li>
     *      <li> Code - Test result code to report. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> ITestResultServer::WriteTestResult() </li>
     *      <li> CreateTestResultName() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     * 
     * @param result      The result of the test.
     * @param description The description for the test result.
     * @param code        The associated code with the test step.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendTestResult(const std::string result, std::string description = "", std::string code = "0000");

    /**
     * Get the remaining test time.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if there is test time remaining for the current test step.  If <i>timeout</i> is NULL, the Timeout
     * time from test step information will be used.  Otherwise, the value of <i>timeout</i> will be used to determine
     * if there is test time remaining.  If the system clock cannot be read, a BepException will be thrown.  It is the
     * responsibility of the calling function to handle the BepException.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Timeout - Amount of time(ms) to allow for the test step to complete. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param timeout Optional timeout value to use.  
     * 
     * @return Flag indicating if there is test time remaining.
     */
    bool TimeRemaining(const INT32 *timeout = NULL);

    /**
     * Remove prompts from the display.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Remove all prompts for prompt tags contained in the test step information.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Prompt1 - Prompt to be removed from box 1.</li>
     *     <li> Prompt1Priority - Priority of the prompt to be removed from box 1. </li>
     *     <li> Prompt2 - Prompt to be removed from box 2. </li>
     *     <li> Prompt2Priority - Priority of the prompt to be removed from box 2. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> RemovePrompt() </li>
     *      </ul>
     */
    void RemovePrompts(void);

    /**
     * Report a test result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Report a test result with test details to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     *
     * @param result            The result of the test.
     * @param description       The description for the result.
     * @param code              The associated code with the test step.
     * @param firstDetail       The name of the first detail.
     * @param firstDetailValue  The value of the first detail.
     * @param firstDetailUnits  The units for the first value.
     * @param secondDetail      The name of the second detail.
     * @param secondDetailValue The value of the second detail.
     * @param secondDetailUnits The units for the second value.
     * @param thirdDetail       The name of the third detail.
     * @param thirdDetailValue  The value of the third detail.
     * @param thirdDetailUnits  The units for the third value.
     * @param fourthDetail      The name of the fourth detail.
     * @param fourthDetailValue The value of the fourth detail.
     * @param fourthDetailUnits The units for the fourth value.
     * @param fifthDetail       The name of the fifth detail.
     * @param fifthDetailValue  The value of the fifth detail.
     * @param fifthDetailUnits  The units for the fifth value.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendTestResultWithDetail(const std::string result, const std::string description = "",
                                   const std::string code = "0000",
                                   const std::string firstDetail = "",const std::string firstDetailValue = "",
                                   const std::string firstDetailUnits = "",
                                   const std::string secondDetail = "", const std::string secondDetailValue = "",
                                   const std::string secondDetailUnits = "",
                                   const std::string thirdDetail = "", const std::string thirdDetailValue = "",
                                   const std::string thirdDetailUnits = "",
                                   const std::string fourthDetail = "", const std::string fourthDetailValue = "",
                                   const std::string fourthDetailUnits = "",
                                   const std::string fifthDetail = "", const std::string fifthDetailValue = "",
                                   const std::string fifthDetailUnits = "");

    /**
     * Report a test result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Report a test result with test details to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CreateTestResultName() </li>
     *      <li> ITestResultServer::WriteTestResult() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param result      The result of the test.
     * @param details     List of test result detail values to be added to the test result node.
     * @param description The description for the result.
     * @param code        The associated code with the test step.
     *
     * @return The status of sending the test result.
     */
    INT32 SendTestResultWithDetail(const std::string result, const TestResultDetails &details,
                                   const std::string description = "", const std::string code = "0000");

    /**
     * Display the specified prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Display a prompt to the user.  The prompt will be displayed in <i>promptBox</i> with a priority of <i>priorityLevel</i>.
     * If the priority is not specified, it will be defaulted to 0.  The prompt box background color can also be 
     * specified if the default color is not desired.  Optional values can also be specified.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> DisplayPrompt() </li>
     *      </ul>
     *
     * @param promptBox       The prompt box where the prompt should be displayed.
     * @param prompt          The prompt to display in the prompt box.
     * @param priorityLevel   Priority level of the prompt (0=Lowest,9=Highest).
     * @param backgroundColor The color to use for the background of the prompt.
     * @param valueOne        The value of the first parameter of the prompt. 
     *                        In the prompt it is indicated by {0}.
     * @param valueTwo        The value of the second parameter of the prompt. 
     *                        In the prompt it is indicated by {1}.
     * @param valueThree      The value of the third parameter of the prompt. 
     *                        In the prompt it is indicated by {2}.
     * 
     * @return The status of displaying the prompt.
     */
    INT32 DisplayPrompt(const std::string promptBox, 
                        const std::string prompt, 
                        INT32 priorityLevel = 0, 
                        const std::string backgroundColor = "",
                        const std::string valueOne = "", 
                        const std::string valueTwo = "", 
                        const std::string valueThree = "");

    /**
     * Display the specified prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Display a prompt to the user.  The prompt will be displayed in <i>promptBox</i> with a priority of <i>priorityLevel</i>.
     * If the priority is not specified, it will be defaulted to 0.  The prompt box background color can also be 
     * specified if the default color is not desired.  Optional values can also be specified.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IPromptServer::DisplayPrompt() </li>
     *      <li> IPromptServer::SetBackgroundColor() </li>
     *      </ul>
     *
     * @param promptBox       The prompt box where the prompt should be displayed.
     * @param prompt          The prompt to display in the prompt box.
     * @param priorityLevel   Priority level of the prompt (0=Lowest,9=Highest).
     * @param backgroundColor The color to use for the background of the prompt.
     * @param valueOne        The value of the first parameter of the prompt. 
     *                        In the prompt it is indicated by {0}.
     * @param valueTwo        The value of the second parameter of the prompt. 
     *                        In the prompt it is indicated by {1}.
     * @param valueThree      The value of the third parameter of the prompt. 
     *                        In the prompt it is indicated by {2}.
     * 
     * @return The status of displaying the prompt.
     */
    INT32 DisplayPrompt(const INT32 promptBox, 
                        const std::string prompt, 
                        INT32 priorityLevel = 0, 
                        const std::string backgroundColor = "",
                        const std::string valueOne = "", 
                        const std::string valueTwo = "", 
                        const std::string valueThree = "");

    /**
     * Get the elapsed time(ms).
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The elapsed time(ms) since the start of the current test step will be calculated and returned to the calling 
     * function.  If the system clock cannot be read, a BepException will be thrown.  It is the responsibility of the
     * calling function to handle the exception.
     * <p>
     * @return The elapsed time since the start if the current test step. 
     */
    double GetElapsedTime(void) const;

    /**
     * Check the testing status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check the current status of the machine and the component to determine if the current conditions are valid for 
     * continuing a test.  If a list of status check items has been loaded from the configuration data, each item will
     * be checked to ensure system status will allow the test to continue.  The compenent status flag will also be 
     * checked to determine if testing can continue.  The component will also check for an Abort condition to determine
     * if testing will be allowed to continue.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetStatus() </li>
     *      <li> SystemRead() </li>
     *      <li> CheckAbort() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @return  Test continuation status:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS - Conditions are valid for testing. </li>
     *          <li> BEP_STATUS_ABORT - The test has been aborted. </li>
     *          <li> BEP_STATUS_FAILURE - One of the status check items is in the incorrect state. </li>
     *          <li> BEP_STATUS_ERROR - There is another problem. </li>
     *          </ul>
     */
    virtual INT32 StatusCheck(void);

    /**
     * Store the time the test step was started.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The test step start time will be stored.  The start time will be used to determine test step elapsed time as well
     * as to determine if the current test step has timed out.  If the system clock cannot be read, a BepException will
     * be thrown.  It is the responsibility of the calling function to handle the exception.
     * <p>
     * @throws BepException A BepException will be thrown if there is an error reading the real time clock.     
     */
    void SetStartTime(void);

    /**
     * Store the secondary start time.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The secondary start time will be stored.  The secondary timer is used for conditions in a test step that should
     * timeout before the overall test step timeout.  For example, waiting for the operator to perform an action.
     * The secondary start time will be used to determine if the secondary timer has expired.  If the system clock cannot 
     * be read, a BepException will be thrown.  It is the responsibility of the calling function to handle the exception.
     * <p>
     * @throws BepException A BepException will be thrown if there is an error reading the real time clock.     
     */
    void SetSecondaryStartTime(void);

    /**
     * Enable motor loading.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This test step will read motor loading parameters from the config file and send them to the motor controller.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MotorLoadingAccelConst - Acceleration constant to use during motor loading. </li>
     *      <li> MotorLoadingSpeedConst - Speed constant to use during motor loading. </li>
     *      <li> MotorLoadingTorqueConst - Torque constant to be applied during motor loading. </li>
     *      <li> MotorLoadingIncreasePerSecond - Percentage load to increase each second until 100% loading is achieved. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> EnableMotorLoading() </li>
     *      <li> GetParameterFloat() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return Result of enabling the motor loading.
     */
    virtual const std::string EnableMotorLoading(const std::string &value);

    /**
     * Enable motor loading.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The loading constants will be written to the motor controller.  The <i>increasePerSecond</i> value will be used
     * to gradually increase the loading constants to 100% of their value.
     * <p>
     * 
     * @param accelConst Acceleration constant to use for motor loading.
     * @param speedConst Speed constant to use for motor loading.
     * @param torqueConst
     *                   Torque constant to use for motor loading.
     * @param increasePerSecond
     *                   Percentage increase to the loading constants each second.
     * 
     * @return BEP_PASS_STATUS
     */
    virtual const std::string EnableMotorLoading(float accelConst, float speedConst, float torqueConst, float increasePerSecond);

    /**
     * Disable motor loading.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The motor controller will be commanded to disable motor loading.
     * <p>
     * @param value The value that was commanded by the test sequencer.
     * 
     * @return BEP_PASS_STATUS
     */
    virtual const std::string DisableMotorLoading(const std::string &value);

    /**
     * Get the drive axle roller speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This function returns the current average wheel speed of the driven axle. The drive axle and four wheel drive 
     * equipped data will be checked to determine if the vehicle is rear wheel drive or front wheel drive.
     * <p>
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> FourWheelDriveEquipped - System tag used to determine if the vehicle is four wheel drive equipped. </li>
     *     <li> DriveAxle - System tag to retrieve the drive axle data from the vehicle build data. </li>
     *     <li> Rear - Value that indicates the vehicle is rear wheel drive. </li>
     *     <li> LRSpeed - System tag for reading the left rear roller speed. </li>
     *     <li> RRSpeed - System tag for reading the right rear roller speed. </li>
     *     <li> LFSpeed - System tag for reading the left front roller speed. </li>
     *     <li> RFSpeed - System tag for reading the right front roller speed. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemReadBool() </li>
     *      <li> SystemReadFloat() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     * 
     * @return The average instantaneous wheel speed of the drive axle.
     */
    virtual float GetRollSpeed(void);

    /**
     * Get the boosted axle roller speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This function returns the current average wheel speed of the boosted axle. The drive axle and four wheel drive 
     * equipped data will be checked to determine if the vehicle is rear wheel drive or front wheel drive.
     * <p>
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> FourWheelDriveEquipped - System tag used to determine if the vehicle is four wheel drive equipped. </li>
     *     <li> DriveAxle - System tag to retrieve the drive axle data from the vehicle build data. </li>
     *     <li> Rear - Value that indicates the vehicle is rear wheel drive. </li>
     *     <li> LRSpeed - System tag for reading the left rear roller speed. </li>
     *     <li> RRSpeed - System tag for reading the right rear roller speed. </li>
     *     <li> LFSpeed - System tag for reading the left front roller speed. </li>
     *     <li> RFSpeed - System tag for reading the right front roller speed. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemReadBool() </li>
     *      <li> SystemReadFloat() </li>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     * 
     * @return The average instantaneous wheel speed of the boosted axle.
     */
    virtual float GetBoostedRollSpeed(void);

    /**
     * Store test step information.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set a test step info float value.  If the data tag does not exist as part of the test step information, a new
     * attribute will be created and added to the test step information.
     * <p>
     * @param infoTag The tag name of the test step info item.
     * @param value   The float value to be set.
     */
    virtual void SetTestStepInfoValue(const std::string& infoTag, const float value);

    /**
     * Store test step information.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set a test step info integer value.  If the data tag does not exist as part of the test step information, a new
     * attribute will be created and added to the test step information.
     * <p>
     * @param infoTag The tag name of the test step info item.
     * @param value   The integer value to be set.
     */
    virtual void SetTestStepInfoValue(const std::string& infoTag, const int value);

    /**
     * Store test step information.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set a test step info string value.  If the data tag does not exist as part of the test step information, a new
     * attribute will be created and added to the test step information.
     * <p>
     * @param infoTag The tag name of the test step info item.
     * @param value   The string value to be set.
     */
    virtual void SetTestStepInfoValue(const std::string& infoTag, const std::string& value);

    /**
     * Get the current wheel distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current distance for all wheels.  If the wheel distance file has not yet been opened, it will be opened.
     * The wheel distances will then be read from the file.  The wheel distance file will remain open to expedite
     * further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentDistances - Name of the current wheel distance file. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> OpenWheelDistanceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param wheelDistance An array for storing the distance values.
     * 
     * @return Status of reading the wheel distances:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel distances were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the wheel distance file mutex could not be acquired. </li>
     *          </ul>
     */
    INT32 GetWheelDistances(float wheelDistance[]);

    /**
     * Get the current wheel distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current distances for all wheels.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetWheelDistances() </li>
     *      </ul>
     *
     * @param wheelDistance A WHEELINFO structure for storing the distance values.
     * 
     * @return Status of reading the wheel distances:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel distances were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the wheel distance file mutex could not be acquired. </li>
     *          </ul>
     */
    INT32 GetWheelDistances(WHEELINFO &wheelDistance);

    /**
     * Calculate the total distance traveled.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Calculates the total distances traveled based on the starting and ending distance structures passed. Corrects for 
     * roll-over and roll-under conditions.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      </ul>
     * 
     * @param totalDistance      The total distance traveled.
     * @param startWheelDistance The starting distance readings.
     * @param endWheelDistance   The ending distance readings.
     */
    void GetTotalDistances( WHEELINFO &totalDistance, const WHEELINFO &startWheelDistance, const WHEELINFO &endWheelDistance);

    /**
     * Convert the provided pulse count into miles.
     * 
     * @param pulseCount Number of pulses to convert.
     * 
     * @return Miles calculated from the provided pulse count.
     */
    float ConvertPulsesToMiles(const float &pulseCount);

protected:
    /**
     * Add an IPC link for the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Add a "friendly" IPC channel name (i.e. component/Brake) as a symlink that points to our less friendly IPC channel 
     * (i.e. File.qxe/ConfigFile.xml).  The existing IPC link will be removed first.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> RemoveIPCLink() </li>
     *      </ul>
     */
    virtual void AddIPCLink(void);

    /**
     * Remove the current IPC link.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Removes our "friendly" IPC channel name symlink.
     */
    virtual void RemoveIPCLink(void);

    /**
     * Process the current objective.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current commanded objective will be processed.  Each test step in the objective will be sequenced.  If an 
     * "AltValue" attribute is included in the test step, that value will be used to override any value that was passed
     * in from the test sequencer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetStatus() </li>
     *      <li> GetFatalFailure() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> SetStartTime() </li>
     *      <li> CommandTestStep() </li>
     *      <li> SetTestStepResult() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> SetStartTime() </li>
     *      <li> SetOverallResult() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendOverallResult() </li>
     *      </ul>
     *
     * @param objective The objective that was sequenced.
     * @param value     The value that was commanded by the test sequencer.
     * 
     * @return The result of the overall objective.
     */
    virtual const std::string CommandObjective(const XmlNode *objective, const std::string &value);

    /**
     * Execute the current test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method will handle the execution of each test step that has been specified by the system for each test 
     * objective that is passed to it.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> TargetSpeedParameter - Parmaeter to use for the accelerate to speed target speed. </li>
     *      <li> SpeedRangeParameter - Parameter to use for the accelerate to speed speed range. </li>
     *      <li> ScanDelayParameter - Parameter to use for the accelerate to speed speed check delay. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> MaintainSpeedTime - Length of time to maintain constant speed. </li>
     *      <li> MaintainSpeedMinSpeed - Minimum speed to be maintained. </li>
     *      <li> MaintainSpeedMaxSpeed - Maximum speed to be maintained. </li>
     *      </ul>
     * 
     * <b>Supported Test Steps:</b>
     *      <ul>
     *      <li> AccelerateToTestSpeed() </li>
     *      <li> EnableMotorLoading() </li>
     *      <li> DisableMotorLoading() </li>
     *      <li> EnableElectricMotorBoost() </li>
     *      <li> DisableElectricMotorBoost() </li>
     *      <li> TestStepParkBrake() </li>
     *      <li> TestStepParkPawl() </li>
     *      <li> TestStepParkBrakePawl() </li>
     *      <li> TestStepHydraulicParkBrake() </li>
     *      <li> TestStepElectricParkBrake() </li>
     *      <li> AnalyzeParkBrakeTest() </li>
     *      <li> AnalyzeParkPawlTest() </li>
     *      <li> MaintainSpeedForTime() </li>
     *      <li> TestStepReadMemStats() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetComponentName() </li>
     *      <li> StatusCheck() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameter() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     *
     * @param value Value that was commanded by the test sequencer.
     * 
     * @return The result of the test step.
     */
    virtual const std::string CommandTestStep(const std::string &value);

    /**
     * Get the next command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * GetCommand will block until the test sequencer sends a new command.  
     * <p>
     *
     * @param command The current command received by the test component.
     * @param value   The commanded value received by the test component.
     * 
     * @return BEP_SUCCESS_RESPONSE if the command was retrieved successfully.<br>
     *         BEP_FAIL_RESPONSE if the command was empty
     */
    inline std::string GetCommand (std::string &command, std::string &value);

    /**
     * Store the current command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * SetCommand will store the command and value that the test sequencer commanded.  The component will then unblock 
     * if it is blocked on GetCommand().  The function will then wait for the result of the objective.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetObjectiveResult() </li>
     *      <li> GetProcessName() </li>
     *      </ul>
     *
     * @param command The command sent by the test sequencer.
     * @param value   The value of the command sent by the test sequencer.
     * 
     * @return The objective result.
     */
    inline std::string SetCommand (const std::string &command, const std::string &value);

    /**
     * Store the objective result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the overall objective result and trigger the response back to the sequencer.
     * <p>
     * @param result The overall result of the sequenced objective.
     */
    virtual void SetObjectiveResult (const std::string result);

    /**
     * Get the result of the test objective.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This function will wait for the commanded objective to complete and post the result.  The result will then be 
     * returned to the calling function.
     * <p>
     * @return The overall result of the sequenced objective.
     */
    virtual std::string GetObjectiveResult (void);

    /**
     * Create the test result name.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Create a test result name that includes the component name prepended.  If <i>name</i> is empty, the current test
     * step name will be appended to the component name.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param name The test step name to append to the component name.
     * 
     * @return The test result name.
     */
    virtual const std::string CreateTestResultName(const std::string name = "");

    /**
     * Store the result of the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the result of the test step being sequenced.
     * <p>
     * @param result The test step result.
     */
    void SetTestStepResult(const std::string result);

    /**
     * Get the result for this test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the result of the current test step.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *     <li> GetComponentName() </li>
     *      </ul>
     * 
     * @return The result of the test step.
     */
    const std::string GetTestStepResult(void);

    /**
     * Remove a displayed prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified prompt will be removed from the specified prompt box.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IPromptServer::RemovePrompt() </li>
     *      </ul>
     * 
     * @param promptBox     The prompt box to remove the prompt from.
     * @param prompt        The prompt to remove from the prompt box.
     * @param priorityLevel Priority level of the prompt (0=Lowest, 9=Highest).
     * 
     * @return The status of removing the prompt from the display.
     */
    INT32 RemovePrompt(const INT32 promptBox, const std::string prompt, INT32 priorityLevel = 0);

    /**
     * Remove a displayed prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified prompt will be removed from the specified prompt box.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IPromptServer::RemovePrompt() </li>
     *      </ul>
     * 
     * @param promptBox     The prompt box to remove the prompt from.
     * @param prompt        The prompt to remove from the prompt box.
     * @param priorityLevel Priority level of the prompt (0=Lowest, 9=Highest).
     * 
     * @return The status of removing the prompt from the display.
     */
    INT32 RemovePrompt(std::string promptBox, const std::string prompt, INT32 priorityLevel = 0);

    /**
     * Lock access to the PromptServer.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Lock the PromptServer so that no other tasks can prompt the operator.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IPromptServer::LockPromptServer() </li>
     *      </ul>
     *
     * @return The status of locking access to the PromptServer.
     */
    INT32 LockPrompts(void);

    /**
     * Unlock access to the PromptServer.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Unlock the PromptServer so other tasks can prompt the operator.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IPromptServer::UnlockPromptServer() </li>
     *      </ul>
     * 
     * @return The status of unlocking access to the PromptServer.
     */
    INT32 UnlockPrompts(void);

    /**
     * Report a test result for a portion of the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a test result for a subtest that was performed to the TestResultServer.  The test result will be reported 
     * under the name of GetComponentName() + <i>testName</i>.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ITestResultServer::WriteTestResult() </li>
     *      <li> CreateTestResultName() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param testName    The name of the subtest.
     * @param result      The result of the test.
     * @param description The description for the result.
     * @param code        The associated code with the test step.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendSubtestResult(const std::string &testName, 
                            const std::string &result, 
                            const std::string description = "", 
                            const std::string code = "0000");

    /**
     * Report a test result for a portion of the test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a test result for a subtest that was performed to the TestResultServer.  The test result will be reported 
     * under the name of GetComponentName() + <i>testName</i>.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateResult() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     *
     * @param testName    The name of the subtest.
     * @param result      The result of the test.
     * @param description The description for the result.
     * @param code        The associated code with the test step.
     * 
     * @return The status of sending the test result.
     */
    INT32 SendSubtestResult(const std::string &testName, 
                            const INT32 result, 
                            const std::string description = "", 
                            const std::string code = "0000");

    /**
     * Report a DTC to the TestResultServer.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Report DTC data to the TestResultServer.  If common module faults are in use, the fault data will be read from the
     * system and then reported to the TestResultServer.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> UseCommonModuleFaultList - Flag to indicate if the common module fault list should be used to get the
     *                                      DTC information to report. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> SystemRead() </li>
     *      <li> ITestResultServer::ReportDTC() </li>
     *      </ul>
     *
     * @param name        The name of the DTC.
     * @param code        The BEP code of the DTC.
     * @param description The description of the DTC.
     * 
     * @return The status of reporting the DTC information.
     */
    INT32 ReportDTC(const std::string name, 
                    const std::string code, 
                    const std::string description, 
                    string *moduleName = NULL);

    /**
     * Remove a reported DTC.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Send a request to the TestResult server to remove all or one of the DTCs that were reported.  If a code is not 
     * specified, all reported DTCs will be cleared from the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ITestResultServer::ClearDTC() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param code The DTC code to remove from the TestResult server list.
     * 
     * @return The status of clearing the DTC from the TestResultServer.
     */
    INT32 RemoveDTC(const std::string code = CLEAR_ALL_DTC);

    /**
     * Set a system fault.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified fault will be reported to the FaultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IFaultServer::SetFault() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param faultName   The name of the fault to set.
     * @param description The description of why the fault occurred.
     * @param severity    The severity of the fault.
     * 
     * @return The status of setting the fault.
     */
    INT32 SetFault(const std::string faultName, const std::string description, INT32 severity = -1);

    /**
     * Clear a system fault.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified system fault will be cleared from the FaultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IFaultServer::ClearFault() </li>
     *      <li> GetComponentName() </li>
     *      </ul>
     *
     * @param faultName The name of the fault to clear.
     * 
     * @return The status of clearing the fault.
     */
    INT32 ClearFault(const std::string faultName);

    /**
     * Set the name of the component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the component name in a member variable.
     * <p>
     * @param componentName The component name.
     */
    void SetComponentName(const std::string componentName);

    /**
     * Read data from the system and wait until a spcific result is achieved.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified <i>tag</i> will be read from the system.
     * <p>
     * @param tag         The system data tag to read.
     * @param result      The value that will be waited for
     * @param timeout     The maximum amount of time that will be taken

     * @return The value of the data.  If an error occurrs, the value ERROR will be returned.
     */
    const std::string SystemReadWaitForResult(const std::string &tag, const std::string &result, float timeout);
    /**
     * Read data from the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified <i>tag</i> will be read from the system.
     * <p>
     * @param tag    The system data tag to read.
     * @return The value of the data.  If an error occurrs, the value ERROR will be returned.
     */
    const std::string SystemRead(const std::string &tag);

    /**
     * Read data from the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The specified <i>data</i> will be read from the system.  A copy of the node read from the system will be returned
     * to the calling function.  It is the responsibility of the calling function to manage the memory associated with
     * the returned XmlNode.
     * <p>
     * @param data    The data tag to read.
     * @return The value of the data.  If an error occurrs, an empty XmlNode will be returned.
     */
    void SystemRead(const XmlNode *data, XmlNode &systemData);

    /**
     * Read data from the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the requested data from the system.  The data read from the system will be returned to the calling function.
     * If an error occurs while reading the requested data, a BepException will be thropwn.  It is the responsibility
     * of the calling function to handle the BepException.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      </ul>
     * 
     * @param tag The data tag to read.
     * 
     * @return The value of the data.
     */ 
    const INT32 SystemReadInt(const std::string &tag);

    /**
     * Read data from the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the requested data from the system.  The data read from the system will be returned to the calling function.
     * If an error occurs while reading the requested data, a BepException will be thropwn.  It is the responsibility
     * of the calling function to handle the BepException.
     * <p>
     * <b>Functions Called:</b>
     *     <ul>
     *      <li> SystemRead() </li>
     *     </ul>
     * 
     * @param tag The data tag to read.
     * 
     * @return The value of the data.
     */
    const float SystemReadFloat(const std::string &tag);

    /**
     * Read data from the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the requested data from the system.  The data read from the system will be returned to the calling function.
     * If an error occurs while reading the requested data, a BepException will be thropwn.  It is the responsibility
     * of the calling function to handle the BepException.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SystemRead() </li>
     *      </ul>
     * 
     * @param tag The data tag to read.
     *  
     * @return The value of the data.
     */
    const bool SystemReadBool(const std::string &tag);

    /**
     * Determine if there is still test time remaining.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the secondary test timer has expired.  if <i>timeout</i> is NULL, the SecondaryTimeout time from the
     * test step info will be used.  Otherwise, the <i>timeout</i> value will be used to determine if there is time 
     * remaining on the secondary test timer.  If the system clock cannot be read, a BepException will be thrown.  It is the
     * responsibility of the calling function to handle the BepException.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> SecondaryTimeout - Amount of time(ms) to allow for the secondary timer. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param timeout Optional timeout value to use.
     * 
     * @return  Flag indicating if test time is still available on the secondary timer.
     */
    bool SecondaryTimeRemaining(const INT32 *timeout = NULL);

    /**
     * Get the value of the specified system tag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the value of the specified system tag.
     * <p>
     * @param tag The name of the system data that was Read/Written/Commanded.
     * 
     * @return The value that was returned for the system data. If there was an error, "ERROR" will be returned.
     */
    const std::string GetSystemValue(const std::string &tag);

    /**
     * Store the current test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The test step currently being sequenced will be stored.
     * <p>
     * @param testStep The test step that is currently being sequenced.
     */
    void SetCurrentTestStep(const XmlNode *testStep);

    /**
     * Check if an Abort has been issued.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The Abort flag will be checked to determine if testing has been aborted.  The component status will also be
     * checked to determine if testing was aborted.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Abort - System tag to use for determining if testing was aborted. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetStatus() </li>
     *      </ul>
     * 
     * @return Flag indicating if an abort has been issued.
     */
    virtual bool CheckAbort(void);

    /**
     * Check if foreground testing has completed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check to determine if all of the foreground tests in the drive curve have been completed.  This is used by 
     * background tests to determine if they should start to command the driver to complete tests.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> ForegroundTestInProgress - System tag for determining if foreground testing has completed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     * 
     * @return Flag indicating if the foreground tests have completed.
     */
    bool ForegroundComplete(void);

    /**
     * Reset the overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The overall test result will be reset to BEP_TESTING_RESPONSE and the component status will be set to 
     * BEP_TESTING_STATUS.  If the overall result was not already set to BEP_PASS_STATUS, the testing status will be
     * set to TEST_STATUS_IN_PROGRESS.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetOverallResult() </li>
     *      <li> UpdateTestStatus() </li>
     *      <li> SetStatus() </li>
     *      </ul>
     */
    virtual void ResetOverallResult(void);

    /**
     * Store the retest status.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the flag indicating if an in-cycle retest is being performed.
     * <p>
     * @param status Flag indicating if an in-cycle retest is being performed.
     */
    void SetRetestStatus(bool status);

    /**
     * Store the previous overall result.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the overall result for the in-cycle test that just completed. This method will be called by 
     * SendOverallResult() at the end of a test.
     * <p>
     * @param result The overall test result for the recently completed in-cycle test.
     */
    void SetPreviousOverallResult(const string result);

    /**
     * Clutch the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The system will be commanded to engage or disengage the clutches.
     * <p>
     * @param type The type of operation to perform.  True will enable the clutch bit, false will disable the clutch bit.
     * 
     * @return The status of engaging or disengaging the clutches.
     */
    INT32 Clutch(bool type);

    /**
     * Report the overall test result to the TestResultServer
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The current overall test result for the component will be written to the TestResultServer.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> ITestResultServer::WriteTestResult() </li>
     *      <li> CreateTestResultName() </li>
     *      </ul>
     *
     * @param status The overall test result for the component.
     */
    void UpdateTestStatus(const std::string status);

    /**
     * Disengage the machine.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Disengage the machine.  This will disable boost as well as declutch all rollers.  If this step passes, the 
     * machine will be in free-wheel mode.  If the machine cannot be declutched, Abort will be returned as the result
     * of the test step.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FailedToDisengage - Failed to disable boost and declutch the rollers. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> Disengage() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @return Result of disengaging the machine.
     */
    virtual string DisengageMachine(void);

    /**
     * Disable electric motor boost.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the electric motors to torque 0 mode.  This will not change the CLUTCH or DECLUTCH
     * settings in the PLC as there are no clutches on electric machines.  If this step is called on 
     * a hydraulic machine, the DisengageMachine test step will be called to handle disabling the boost.
     * <p>
     * <b>Data Tags:</b>
     *         <ul>
     *         <li> MachineType - Tag used to read machine type from MachineDataBroker </li>
     *         <li> ElectricMachineType - Value used to indicate if the machine is electric </li>
     *         </ul>
     * 
     * <b>Functions Called:</b>
     *         <ul>
     *         <li> SystemRead() </li>
     *         <li> GetDataTag() </li>
     *         <li> SystemCommand() </li>
     *         <li> DisengageMachine() </li>
     *         </ul>
     * 
     * @return Result of disabling electric motor boosting.
     */
    virtual string DisableElectricMotorBoost(void);

    /**
     * Engage the machine.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Engage the machine.  This will enable boost and clutch all rollers.  If the rollers are not at zero speed, the
     * operator will be prompted to brake to stop.  The software will then wait until zero speed has been achieved
     * or test time has expired.  If the machine is not at zero speed, the clutches will be destroyed when they are
     * pulled in.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FailedToEngage - Could not Clutch the rollers and enable boost. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> Engage() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @return Result of engaging the machine.
     */
    virtual string EngageMachine(void);

    /**
     * Engage the machine.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Engage the machine.  This will enable boost and clutch all rollers.  If the rollers are not at zero speed, the
     * operator will be prompted to brake to stop.  The software will then wait until zero speed has been achieved
     * or test time has expired.  If the machine is not at zero speed, the clutches will be destroyed when they are
     * pulled in.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FailedToEngage - Could not Clutch the rollers and enable boost. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> Engage() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @return Result of engaging the machine.
     */
    virtual string ResetDriveAxle(const std::string &value);

    virtual string EnableCenteringArrows(void);
    virtual string DisableCenteringArrows(void);

    /**
     * Enable electric motor boost.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Command the electric motors to boost mode.  This will not change the CLUTCH or DECLUTCH
     * settings in the PLC as there are no clutches on electric machines.  If this step is called on 
     * a hydraulic machine, the EngageMachine test step will be called to handle enabling the boost.
     * As there are no clutches, there is also no the requirement that the rollers be at zero speed to 
     * enable boosting.
     * <br><p>
     * 
     * <b>Data Tags:</b>
     *         <ul>
     *         <li> MachineType - Tag used to read machine type from MachineDataBroker </li>
     *         <li> ElectricMachineType - Value used to indicate if the machine is electric </li>
     *         <li> Boost - Tag used to enable motor boosting </li>
     *         </ul>
     * 
     * <b>Functions Called:</b>
     *         <ul>
     *         <li> SystemRead() </li>
     *         <li> GetDataTag() </li>
     *         <li> SystemWrite() </li>
     *         <li> SystemCommand() </li>
     *         <li> EngageMachine() </li>
     *         </ul>
     * 
     * @return Result of enabling electric motor boosting.
     */
    virtual string EnableElectricMotorBoost(void);

    /**
     * Prompt the operator to slow down.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Prompt the driver to slow down to the test speed and validate that it is acheived.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag used to display the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> RemovePrompt() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param targetSpeed   The required speed to be achieved.
     * @param speedRange    The speed range to display for the operator.
     * @param scanDelay     The delay time(ms) between speed checks.
     * @param reportResults The flag that tells if the test result should be sent to the Test Results Server.
     * @param promptName    The name of the prompt to use if slowing down is needed.
     *
     * @return The result of waiting for the operator to slow down to the target speed.
     */
    const string SlowDownToTestSpeed(const float &targetSpeed, const string &speedRange, const INT32 &scanDelay,
                                     const bool &reportResults = true, string promptName = "SlowDownToTargetSpeed");

    /**
     * Accelerate to the target speed.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Prompt the driver to accelerate to the target speed and validate that it is acheived.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      <li> Prompt1Value1 - A value to display with the prompt. </li>
     *      <li> Prompt2Value1 - A value to display with the prompt.  </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> SpeedTarget - System tag to use for displaying the target speed range for the operator. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetRollSpeed() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> RemovePrompt() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param targetSpeed   The required speed to be achieved.
     * @param speedRange    The speed range to display for the operator.
     * @param scanDelay     The delay time(ms) between speed checks.
     * @param reportResults The flag that tells if the test result should be sent to the Test Results Server.
     * @param promptName    The name of the prompt to use for instructing the operator to accelerate to the target speed.
     *
     * @return Result of verifying the target speed was achieved.
     */
    const string AccelerateToTestSpeed(const float &targetSpeed, const string &speedRange, const INT32 &scanDelay,
                                       const bool &reportResults = true, string promptName = "AccelerateToTargetSpeed", 
                                       const bool &waitForBoostedAxle = false);

    /**
     * Wait for the target speed to be achieved.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This function waits until the vehicle achieves the given speed or test step time expires.  The system status will
     * also be checked periodically to ensure testing can continue.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     *
     * @param targetSpeed The required speed to be achieved.
     * @param direction   The direction of vehicle speed (UP or DOWN) to achieve the target speed.
     * @param wait        The delay time(ms) between speed checks.
     *
     * @return Status of waiting for the target speed to be achieved:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the vehicle reached the target speed. </li>
     *          <li> BEP_STATUS_SOFTWARE if the direction parameter is invalid. </li>
     *          <li> BEP_STATUS_ABORT if the test step was aborted. </li>
     *          <li> BEP_STATUS_TIMEOUT if the test step timed out. </li>
     *          </ul>
     */
    BEP_STATUS_TYPE WaitForTargetSpeed(float targetSpeed, Direction direction, UINT32 wait = 100, const bool &waitForBoostedAxle = false);

    /**
     * Wait for the operator to achieve the specified speed range.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The vehicle speed will be monitored.  If the vehicle speed is above the target speed range, the operator will
     * be instructed to slow down to the target range.  If the vehicle speed is below the target speed range, the
     * operator will be instructed to accelerate to the target speed range.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollSpeed() </li>
     *      <li> AccelerateToTestSpeed() </li>
     *      <li> SlowDownToTestSpeed() </li>
     *      </ul>
     * 
     * @param minSpeedTarget       The minimum speed target.
     * @param maxSpeedTarget       The maximum speed target.
     * @param speedScanDelay       The delay time(ms) between speed checks.
     * @param promptAccelerateName The name of the prompt to use if acceleration is needed.
     * @param promptSlowDownName   The name of the prompt to use if slowing down is needed.
     *
     * @return Result of waiting for the operator to achieve the specified speed range.
     */
    virtual string WaitAndPromptForSpeed(float minSpeedTarget, float maxSpeedTarget, int speedScanDelay,
                                         string promptAccelerateName = "AccelerateToTargetSpeed",
                                         string promptSlowDownName = "SlowDownToTargetSpeed");

    /**
     * Boost all wheels to get the vehicle to the desired speed.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step will use the motors to boost all wheels to the specified target speed.
     * In certain applications, such as a mining application, the max speed limit of the vehicle may not allow
     * the vehicle to obtain normal brake test arm speed on its own.  This test step will ensure the wheel speed
     * reaches the necessary speed to properly perform the brake test.
     * <br><p>
     * 
     * <b>Test Component Parameters:</b>
     *       <ul>
     *       <li> PreForcedAccelerateWait - Amount of time to wait for operator to shift to neutral and remove foot from brake </li>
     *       <li> ForcedAccelerateSpeedStep - Speed step to use for accelerating the rollers to brake test speed </li>
     *       <li> ForcedAccelerateSpeedStepDelay - Amount of time to wait for the motors to reach the desired speed step </li>
     *       </ul>
     * 
     * <b>Test Step Info:</b>
     *       <ul>
     *       <li> Description - Description of the test step </li>
     *       </ul>
     * 
     * <b>Prompts:</b>
     *       <ul>
     *       <li> NeutralPrompt - Shift transmission to Neutral </li>
     *       <li> RemoveFootFromBrake - Remove foot from brake pedal </li>
     *       </ul>
     * 
     * <b>Fault Codes:</b>
     *       <ul>
     *       <li> TargetSpeedTimeout - Timeout waiting for the rollers to accelerate to brake test speed </li>
     *       <li> SystemStatus - Bad system status while accelerating the rollers to brake test speed </li>
     *       <li> SoftwareFailure - Unhandled exit condition from acceleration loop </li>
     *       <li> ZeroSpeedTimeout - Timeout waiting for the operator to brake to stop </li>
     *       </ul>
     * 
     * <b>Functions Called:</b>
     *       <ul>
     *       <li> ShortCircuitTestStep() </li>
     *       <li> GetTestStepInfo() </li>
     *       <li> CheckZeroSpeed() </li>
     *       <li> DisplayPrompt() </li>
     *       <li> GetPrompt() </li>
     *       <li> GetPromptPriority() </li>
     *       <li> BposSleep() </li>
     *       <li> GetParameterInt() </li>
     *       <li> GetParameterFloat() </li>
     *       <li> SetStartTime() </li>
     *       <li> TimeRemaining() </li>
     *       <li> SystemCommand() </li>
     *       <li> GetDataTag() </li>
     *       <li> GetRollSpeed() </li>
     *       <li> StatusCheck() </li>
     *       <li> GetFaultCode() </li>
     *       <li> GetFaultDescription() </li>
     *       <li> ConvertStatusToResponse() </li>
     *       <li> SendTestResult() </li>
     *       </ul>
     * 
     * @param targetSpeed
     *               Speed to command the rollers to achieve.
     * @param startAtZeroSpeed
     *               Flag indicating if the operator must achieve zero speed before commanding the rollers to speed.
     * @param disengageAtSpeed
     *               Flag indicating if the rollers should be disengaged once the target speed is obtained.
     * 
     * @return Result of boosting all wheels to the target speed.
     */
    virtual string AccelerateVehicleToSpeed(const float targetSpeed, 
                                            const bool startAtZeroSpeed, 
                                            const bool disengageAtSpeed);

    /**
     * Check if the vehicle is at zero speed.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the vehicle is at zero speed.  If not at zero speed, prompt the operator to stop the vehicle.<br>
     * If this function is run as a background test, it will first check to see 
     * if the vehicle is at zero speed.  If not, it then checks to see if all 
     * the foreground tests are done.  It will continue to loop until either the 
     * foreground tests are complete, StatusCheck() fails, or at zero speed.
     * If the foreground tests are complete and the vehicle is not at zero speed, 
     * the function will attempt to lock the prompt server and will continue to 
     * loop as above.  Once the prompt server has been locked, the function acts 
     * as a foreground test.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between checks for zero speed. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> ZeroSpeed - Brake to stop. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> SetStartTime() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> IsForegroundTest() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     * 
     * @return Flag indicating if the vehicle is at zero speed.
     */
    virtual const bool CheckZeroSpeed(void);

    /**
     * Verify the diagnostic cable is connected.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Determine if the cable is connected.  If the cable is not connected, the operator will be prompted to connect 
     * the diagnostic cable.<br>
     * If this function is run as a background test, it will first check to see 
     * if the cable is connected.  If not, it then checks to see if all the 
     * foreground tests are done.  It will continue to loop until either the 
     * foreground tests are complete, StatusCheck() fails, or the cable is 
     * connected.  If the foreground tests are complete and the cable is not
     * connected, the function will attempt to lock the prompt server and will 
     * continue to loop as above.  Once the prompt server has been locked, the 
     * function acts as a foreground test would.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> ScanDelay - Amount of time(ms) to wait between cable connect checks. </li>
     *      </ul>
     * 
     * <b>Prompt Tags:</b>
     *      <ul>
     *      <li> CableConnect - Connect the diagnostic cable to the vehicle. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> SetStartTime() </li>
     *      <li> IsCableConnected() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> IsForegroundTest() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> StatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      </ul>
     * 
     * @return Flag indicating if the diagnostic cable is connected to the vehicle.
     */
    virtual const bool CheckCableConnect(void);

    /**
     * Check if the diagnostic cable is connected.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check to see if the diagnostic cable is connected.  The subscribed data tag CBL_CONNECT_DATA_TAG will be checked
     * to determine if the cable has been connected.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     *
     * @return Flag indicating if the cable is connected.
     */
    virtual const bool IsCableConnected(void);

    /**
     * Check if this is a foreground test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check the foreground test flag to determine if this is a foreground or background test.
     * <p>
     * @return Flag indicating if this is a foreground test.
     */
    const bool IsForegroundTest(void);

    /**
     * Check if the prompts can be locked.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check to see if there are any foreground tests, if there are no foreground tests attempt to lock the prompt server.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ForegroundComplete() </li>
     *      <li> LockPrompts() </li>
     *      </ul>
     * 
     * @return Flag indicating if the prompts are locked. 
     */
    virtual bool CheckForegroundLockPrompts(void);

    /**
     * Set the domain of the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method is used to set the foreground/background test domain of the component.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> IsForegroundTest() </li>
     *      </ul>
     * 
     * @param testDomain The test domain for the current test.  Valid values are "Foreground" and "Background".  
     *                   If the parameter does not match either of these values, the foreground test member variable 
     *                   will default to true.
     */
    virtual void SetTestDomain(const string &testDomain);

    /**
     * Get a Yes/No response from the operator.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Use the pendant to get a "Yes/No" response from the driver.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetOperatorInput() </li>
     *      </ul>
     *
     * @param operatorPrompt The prompt tag to display. 
     * @param timeout        The amount of time(ms) to wait for driver input.
     *
     * @return  Input value from the operator:
     *          <ul>
     *          <li> "Yes" or "No" if the driver input was received correctly. </li>
     *          <li> "Timeout" if no driver input was received. </li>
     *          <li> "Fail" if the driver input was not valid. </li>
     *          </ul>
     */
    virtual string OperatorYesNo(const string &operatorPrompt = "EnterYesOrNo", const INT32 &timeout = NULL);

    /**
     * Get a Pass/Fail/Abort response from the operator.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Use the pendant to get a "Pass/Fail/Abort" response from the driver.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetOperatorInput() </li>
     *      </ul>
     * 
     * @param operatorPrompt The prompt tag to display. 
     * @param timeout        The amount of time(ms) to wait for driver input.
     *
     * @return Input value from the operator:
     *          <ul>
     *          <li> "Pass", "Fail", or "Abort" if the driver input was received correctly. </li>
     *          <li> "Timeout" if no driver input was received. </li>
     *          </ul>
     */
    virtual string OperatorPassFail(const string &operatorPrompt = "PassOrFail", const INT32 &timeout = NULL);

    /**
     * Get data from the operator.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * This method uses the pendant to get input from the operator.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SystemRead() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param inputServerState The state in which to place the InputServer.  This selects the allowable input from the 
     *                         pendant and format of the data that is returned.
     * @param operatorPrompt   The prompt tag to display. 
     * @param validResponses   The list of allowable responses from the pendant.  It is used to determine what is 
     *                         returned to the calling function.
     * @param timeout          The amount of time to wait for driver input.
     *
     * @return A string containing one of the valid responses if the driver input was received correctly:
     *          <ul>
     *          <li> "Timeout" if no driver input was received. </li>
     *          <li> "Fail" if the driver input was not valid. </li>
     */
    virtual string GetOperatorInput(const string &inputServerState, const string &operatorPrompt,
                                    const vector<string> &validResponses, const INT32 &timeout = NULL);

    /**
     * Load the provided vector with the valid responses.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Setup the vector with valid operator responses for the IntputServer state requested.  This will be used to verify
     * the operator entered a correct response.
     * <p>
     *
     * @param inputServerState The current InputServer state.
     * @param responses        The vector to fill with the valid responses for the InputServer state requested.
     */
    virtual void SetupOperatorResponses(const string &inputServerState, vector<string> &responses);

    /**
     * Verify the operator maintains current speed for the specified amount of time.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This method is used to determine if a vehicle maintained speed within a specified range for a specific period of 
     * time.  It checks the foreground test flag to see if the test is being run in the foreground or background.  It 
     * will prompt to maintain speed if it is running in the foreground, if running in the background, it simply checks 
     * if speed is maintained for the required time period.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     *
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> SpeedTarget - System tag for displaying the target speed range to the operator. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> StatusCheckFailure - System status prevents further testing. </li>
     *     <li> MaintainSpeedTimeoutFailure - Timeout waiting for the maintain speed. </li>
     *     <li> MaintainSpeedFailure - Vehicle speed went out of range. </li>
     *     <li> SoftwareFailure - An unanticipated exit condition occurred. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> SetSecondaryStartTime() </li>
     *      <li> GetRollSpeed() </li>
     *      <li> IsForegroundTest() </li>
     *      <li> SetStartTime() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> RemovePrompt() </li>
     *      <li> WaitAndPromptForSpeed() </li>
     *      <li> SecondaryTimeRemaining() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> SendSubtestResult() </li>
     *      </ul>
     * 
     * @param requiredTime            The amount of time required for the vehicle to maintain speed within the range.
     * @param speedMinimum            The lower end of the speed range.
     * @param speedMaximum            The upper end of the speed range.
     * @param maintainSpeedPrompt     The maintain speed prompt tag.
     * @param promptIfOutOfSpeedRange Flag to tell if the driver will be prompted if the vehicle speed fails to stay 
     *                                within range.  If this variable is set to false, the method will fail if the 
     *                                speed violates the range at any time before the requiredTime is met.
     * @param reportResults           Flag to tell if the test result is reported to the Test Result Server.
     * @param scanDelay               The delay time(ms) for the loop.
     *
     * @return The result of waiting for the operator to maintin speed for the specified amount of time.
     */
    virtual string MaintainSpeedForTime(const INT32 &requiredTime, 
                                        const INT32 &speedMinimum, 
                                        const INT32 &speedMaximum = 100, 
                                        const string &maintainSpeedPrompt = "MaintainSpeed", 
                                        const bool &promptIfOutOfSpeedRange = true, 
                                        const bool &reportResults = true, 
                                        INT32 scanDelay = 250);

    /**
     * Have the operator accelerate in reverse.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Verify the vehicle accelerates in reverse gear to the indicated speed.  The operator will first be prompted to
     * brake to stop if the vehicle is not already at zero speed.  The operator will then be instructed to Shift the
     * transmission to reverse and accelerate to the target speed.  The transmission gear will be read while the 
     * speed is being monitored to ensure both the gear requirement and the speed requirement are met.
     * <p>
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     *
     * <b>Prompt Tags:</b>
     *     <ul>
     *     <li> ShiftToReverse - Shift the transmission to reverse. </li>
     *     <li> AccelerateToTargetSpeed - Accelerate to the target speed. </li>
     *     </ul>
     *
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> SpeedTarget - System tag to set the target speed range for the operator. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> StatusCheckFailure - System status precludes any further testing. </li>
     *     <li> CommunicationFailure - Could not read the reverse gear. </li>
     *     <li> AccelerateInReverseTimeout - Timeout waiting for the operator to accelerate. </li>
     *     <li> UnknownFailure - An unexpected exit condition occurred. </li>
     *     <li> AccelerateInReverseZeroSpeedTimeout - Timeout waiting for zero speed. </li>
     *     <li> SoftwareFailure - An exception occurred while checking for reverse speed. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepInfo() </li>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> CheckZeroSpeed() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li> 
     *      <li> GetPromptPriority() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetStartTime() </li>
     *      <li> ReadReverseGear() </li>
     *      <li> WaitForTargetSpeed() </li>
     *      <li> GetRollSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      </ul>
     * 
     * @param speedMinimum The minimum speed required for the test step to pass.
     * @param speedMaximum The maximum speed allowed for the test step to pass.
     * @param scanDelay    The loop delay time(ms).
     *
     * @return The result of checking for the required vehicle speed in reverse.
     */
    virtual string AccelerateInReverse(const float &speedMinimum, const float &speedMaximum, INT32 scanDelay = 100);

    /**
     * Check if the transmission is in reverse gear.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Check if the vehicle is in reverse gear.  This method always returns pass.  This should be overloaded for 
     * automatic transmissions as this class is not capable of interfacing with vehicle modules.
     *
     * @param inReverseGear Flag indicating if the transmission was detected in reverse gear.
     * 
     * @return BEP_STATUS_SUCCESS
     */
    virtual BEP_STATUS_TYPE ReadReverseGear(bool &inReverseGear);

    /**
     * Test the park brake.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park brake test.  The machine type will be checked to determine if this is being run on an electric
     * machine or a hydraulic machine.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> TestStepElectricParkBrake() </li>
     *      <li> TestStepHydraulicParkBrake() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park brake.
     */
    virtual const std::string TestStepParkBrake(const std::string &value, bool isTandemAxle = false);

    /**
     * Test the park pawl.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park pawl test.  The machine type will be checked to determine if this is being run on an electric
     * machine or a hydraulic machine.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> TestStepElectricParkPawl() </li>
     *      <li> TestStepHydraulicParkPawl() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park pawl.
     */
    virtual const std::string TestStepParkPawl(const std::string &value);

    /**
     * Perform a park brake and park pawl test.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a combination park brake and park pawl test.  The machine type will be checked to determine if this is 
     * being run on an electric machine or a hydraulic machine.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SetCurrentTestStep() </li>
     *      <li> TestStepElectricParkBrakePawl() </li>
     *      <li> TestStepHydraulicParkBrakePawl() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park brake and park pawl.
     */
    virtual const std::string TestStepParkBrakePawl(const std::string &value);

    /**
     * Analyze the park brake test results.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Analyzes the park brake test distance results.  The start and end distances for each wheel will be checked to 
     * ensure the park brake is adjusted properly.  The distance on the rear wheels should be very small since the 
     * park brake is holding the wheels.  The front wheels should have a high distance since these wheels are not
     * restrained.  If the front wheel distance is fairly small, then the test will fail since the operator will likely
     * have applied the brake pedal.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      <li> ParkingBrakeTestPass - System tag for operator input indicating if the park brake test passed. </li>
     *      <li> ParkingBrakeTestFail - System tag for operator input indicating if the park brake test failed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkBrakeTestDistances() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of validating the wheel distances.
     */
    virtual const std::string AnalyzeParkBrakeTest(const std::string &value, bool isTandemAxle = false);

    /**
     * Analyze the park pawl test data.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Analyzes the park pawl test distance results.  The start and end distances will be checked to ensure the park pawl 
     * engages correctly.  The distance on the drive axle should be small since the park pawl is preventing the wheels
     * from rotating.  
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> MachineType - System tag used for getting the current machine type. </li>
     *      <li> MachineType3600 - Value indicating this is a 3600 style machine. </li>
     *      <li> ParkingPawlTestPass - System tag for operator input indicating if the park pawl test passed. </li>
     *      <li> ParkingPawlTestFail - System tag for operator input indicating if the park pawl test failed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkPawlTestDistances() </li>
     *      </ul>
     *
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of validating the wheel distances.
     */
    virtual const std::string AnalyzeParkPawlTest(const std::string &value);

    /**
     * Test the park brake on a hydraulic machine.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park brake test for a hydraulic machine.  The operator will first be instructed to shift the transmission
     * to Neutral and apply the park brake.  The boost force will be set for the front and rear rollers.  The hydraulic 
     * pump motor will then be enabled to apply the torque to the rollers.  The system will then wait for a specified 
     * amount of time before disabling the hydraulic pump motor.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to release the park brake. </li>
     *     <li> ParkBrakeApplyForce - Amount of force to apply to the rollers for the park brake test. </li>
     *     <li> UnidirectionalParkBrakeTest - Flag indicating if this is a unidirectional park brake test. </li>
     *     <li> ParkBrakePrepTime - Amount of time(ms) to wait for the operator to shift to Neutral and apply the park brake. </li>
     *     <li> ReleaseParkBrakePrompt - Prompt to display to instruct the operator to release the park brake. </li>
     *     </ul>
     *
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> UnidirectionalParkBrakeTest - System tag to use to indicate this is a unidirectional park brake test. </li>
     *     <li> RearBoostForce - System tag to use for setting the rear wheel boost force. </li>
     *     <li> FrontBoostForce - System tag to use for setting the front wheel boost force. </li>
     *     <li> ParkingBrakeTest - System tag to use to enable/disable the hydraulic pump motor to apply torque to the 
     *                             wheels. </li>
     *     </ul>
     * 
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> HydraulicParkBrakeInit() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusSleep() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> UpdateResult() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of performing the park brake test.
     */
    virtual const std::string TestStepHydraulicParkBrake(const std::string &value);

    /**
     * Initialize the park brake test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-hydraulic park brake test initialization.  System status will be monitored while waiting for the 
     * operator to place the vehicle in the correct state to test the park brake.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle in the correct
     *                                 testing state. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle in a testable state.
     */
    virtual INT32 HydraulicParkBrakeInit(void);

    /**
     * Test the park brake.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park brake test on an electric machine.  The operator will be instructed to place the vehicle in the
     * correct state for testing.  The roller direction (Forward, Reverse or Both) will then be checked to determine
     * which direction to rotate the rollers.  The direction will be validated to ensure it is correct.  If the direction
     * is not valid, the test will fail and not be performed; otherwise, the park brake will then be tested.  At the 
     * end of the test, the motors will be placed into Boost mode.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to release the park brake. </li>
     *      <li> ReleaseParkBrakePrompt - Prompt tag to use to instruct the operator to release the park brake.  </li>
     *      <li> ParkBrakeApplyForce - Force to apply to the rollers for the park brake test. </li>
     *      <li> ParkBrakeForceMultiplier - Multiplier to apply to the park brake force for reporting purposes. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> RollerDirection - Direction to rotate the rollers for the test (Forward, Reverse, Both). </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> ElectricParkBrakeInit() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsParkBrakeDirectionValid() </li>
     *      <li> PerformPBTorqueTest() </li>
     *      <li> GetTestStepName() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> SystemCommand() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UpdateResult() </li>
     *      <li> ApplyForceMultiplier() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park brake.
     */
    virtual const std::string TestStepElectricParkBrake(const std::string &value, bool isTandemAxle = false);

    /**
     * Initialize the park brake test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-electirc park brake test initialization.  System status will be monitored while waiting for the 
     * operator to place the vehicle in the correct state to test the park brake.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle in a testable
     *                                 state. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle in a testable state.
     */
    virtual INT32 ElectricParkBrakeInit(void);

    /**
     * Perform the park pawl test.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park pawl test on a hydraulic machine.  The operator will be instructed to place the vehicle in the 
     * proper state for testing the park pawl.  The drive axle will then be determined and force applied to the drive
     * axle.  The software will then monitor the system status for a period of time before disabling the drive axle 
     * boost.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkPawlFault - Park pawl test failed. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlApplyForce - Amount of force to apply to the rollers to test the park pawl. </li>
     *      <li> UnidirectionalParkPawlTest - Flag indicating if this will be a unidirectional test. </li>
     *      <li> ParkPawlPrepTime - Amount of time(ms) to wait for the system to start applying force. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *     <ul>
     *      <li> DriveAxle - System tag to get the drive axle of the vehicle from the vehicle build data. </li>
     *      <li> RearBoostForce - System tag to use to set the amount of force on the rear rollers. </li>
     *      <li> FrontBoostForce - System tagto use to set the amount of force on the front rollers. </li>
     *      <li> UnidirectionalParkBrakeTest - System tag to use to indicate this is a unidirectional test. </li>
     *      <li> ParkPawlTest - System tag to use to enable/disable the hydraulic pump motor for the park pawl test. </li>
     *      <li> ParkPawlTestPass - Operator input indicating the park pawl test passed. </li>
     *      <li> ParkPawlTestFail - Operator input indicating the park pawl test failed. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> HydraulicParkPawlInit() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterBool() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> SystemWrite() </li>
     *      <li> StatusSleep() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park pawl.
     */
    virtual const std::string TestStepHydraulicParkPawl(const std::string &value);

    /**
     * Initialize the park pawl test.
     * <b>Description:</b>
     * Perform pre-hydraulic park pawl test initialization.  System status will be monitored whle the operator is 
     * placing the vehicle in the correct state to perform the park pawl test.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle in a testable state. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle in a testable state.
     */
    virtual INT32 HydraulicParkPawlInit(void);

    /**
     * Test the park pawl.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a park pawl test on an electric machine.  The operator will be instructed to place the vehicle in a 
     * testable state.  The park pawl test direction will then be retrieved from the config data and analyzed to ensure
     * a proper roller rotation direction was specified.  Valid roller rotation directions are Forward, Reverse and Both.  
     * The park pawl will then be tested.  After the test is complete, the motors will be placed into Boost mode.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle in a testable state. </li>
     *     </ul>
     *
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> RollerDirection - Direction to rotate the rollers during the park pawl test. </li>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsParkBrakeDirectionValid() </li>
     *      <li> ParkPawlTorqueTest() </li>
     *      <li> GetTestStepName() </li>
     *      <li> UpdateResult() </li>
     *      <li> SystemCommand() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park pawl.
     */
    virtual const std::string TestStepElectricParkPawl(const std::string &value);

    /**
     * Initialize the park pawl test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-electric park pawl test initialization.  System status will be monitored whlie waiting for the
     * operator to place the vehicle into a testable state.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle into a testable
     *                                 state. </li>
     *      </ul>
     *   
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle in a testable state.
     */
    virtual INT32 ElectricParkPawlInit(void);

    /**
     * Perform the park brake and park pawl test simultaneously.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a combination park brake / park pawl test for a hydraulic machine.  The operator will be instructed to 
     * place the vehicle in a testable state.  The front and rear forces to apply during the test will then be written
     * to the motor controller.  The system will then be commanded to apply the necessary force for the park brake
     * and park pawl testing.  The PLC will then be monitored to determine if the operator pressed the Pass or Fail
     * button.  Once the test is completed, the motor controller will be commanded to stop applying force.
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> FaultStaticParkBrake - Park brake test has failed. </li>
     *      <li> ParkPawlFault - The park pawl test failed. </li>
     *      </ul>
     *
     * <b>Test Component Parameters:</b>
     *     <ul>
     *      <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to release the park brake. </li>
     *      <li> ParkBrakeApplyForce - Amount of force to apply to the rollers for the park brake test. </li>
     *      <li> ParkPawlApplyForce - Amount of force to apply to the rollers for the park pawl test. </li>
     *      <li> UnidirectionalParkBrakeTest - Flag indicating if the test should be performed unidirectionally. </li>
     *      <li> ParkPawlPrepTime - Amount of time(ms) to wait for the operator to place the vehicle in a testable
     *                              state. </li>
     *      <li> ParkBrakePrepTime - Amount of time(ms) to wait for the operator to place the vehicle in a testable
     *                               state. </li>
     *      <li> ReleaseParkBrakePrompt - Prompt tag to use to instruct the operator to release the park brake. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> RearBoostForce - System tag for setting the rear boost force for the park brake test. </li>
     *      <li> FrontBoostForce - System tag for setting the front boost force for the park pawl test. </li>
     *      <li> UnidirectionalParkBrakeTest - System tag for setting if this test is unidirectional. </li>
     *      <li> ParkingBrakeTest - System tag for starting the park brake test. </li>
     *      <li> ParkPawlTest - System tag for starting the park pawl test. </li>
     *      <li> ParkingBrakeTestPass - System tag indicating if the park brake test passed. </li>
     *      <li> ParkingBrakeTestFail - System tag indicating if the park brake test failed. </li>
     *      <li> ParkPawlTestPass - System tag indicating if the park pawl test passed. </li>
     *      <li> ParkPawlTestFail - System tag indicating if the park pawl test failed. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> HydraulicParkBrakePawlInit() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> StatusSleep() </li>
     *      <li> SetStartTime() </li>
     *      <li> TimeRemaining() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park brake and the park pawl.
     */
    virtual const std::string TestStepHydraulicParkBrakePawl(const std::string &value);

    /**
     * Initialize the park brake and park pawl test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-hydraulic park brake/pawl test initialization.  System status will be monitored while waiting for the
     * operator to place the vehicle in the correct test state.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to get the vehicle into the correct 
     *                                 state. </li>
     *      <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to get the vehicle into the correct 
     *                                  state. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle in the correct test state.
     */
    virtual INT32 HydraulicParkBrakePawlInit(void);

    /**
     * Test the park brake and park pawl.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * Perform a combination park brake / park pawl test on an electric machine.  The operator will be instructed to 
     * place the vehicle in the correct state to perform the test.  The roller direction will then be validated to
     * ensure a proper direction was specified.  The test will then be performed.  
     * <p>
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkBrakeParkPawlFault - The park brake and park pawl is not correctly. </li>
     *      </ul>
     *
     * <b>test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to release the park brake. </li>
     *      <li> ReleaseParkBrakePrompt - Prompt tag to use to instruct the operator to release the park brake. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> RollerDirection - Direction to rotate the rollers for the test. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> GetParameterInt() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> ElectricParkBrakePawlInit() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> IsParkBrakeDirectionValid() </li>
     *      <li> ComboTorqueTest() </li>
     *      <li> GetTestStepName() </li>
     *      <li> UpdateResult() </li>
     *      <li> DisplayTimedPrompt() </li>
     *      <li> SendTestResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     *
     * @return The result of testing the park brake and park pawl.
     */
    virtual const std::string TestStepElectricParkBrakePawl(const std::string &value);

    /**
     * Initialize the park brake/ park pawl test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Perform pre-electric park brake/pawl test initialization.  The system status will be monitored whlie waiting for
     * the operator to place the vehicle in the correct test state.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle into a testable
     *                                 state. </li>
     *      <li> ParkBrakeDriverDelay - Amount of time(ms) to wait for the operator to place the vehicle into a testable
     *                                  state. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterInt() </li>
     *      <li> StatusSleep() </li>
     *      </ul>
     * 
     * @return The system status after waiting for the operator to place the vehicle into a testable state.
     */
    virtual INT32 ElectricParkBrakePawlInit(void);

    /**
     * Apply torque to the drive axle.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The drive axle will be determined.  Torque will then be applied to the drive axle for a set period of time.
     * After the vehicle returns to zero speed, the wheel distances for the drive axle will be analyzed to determine
     * if the park pawl is operating correctly.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlApplyForce - Force to apply to the drive axle rollers. </li>
     *      <li> ParkPawlSpeedValue - Speed to apply to the non-drive axle rollers. </li>
     *      </ul>
     *
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TestTime - Length of time(ms) to apply torque to the drive axle rollers. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System tag to determine if the vehicle is at zero speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetWheelDistances() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> SetStartTime() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> SystemCommand() </li>
     *      <li> TimeRemaining() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkPawlTestDistances() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param direction The direction to turn the rollers.  Valid values are "Forward", "Reverse" or "Both".
     *
     * @return The result of testing the park pawl.
     */
    virtual const std::string ParkPawlTorqueTest(const std::string &direction);

    /**
     * Test the park brake
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Torque will be applied to the rear axle and the front axle will be commanded to speed.  After a set amount of time,
     * torque and speed will be removed from the applicable axles.  The wheel distances will then be analyzed to ensure
     * the park brake is set correctly and the operator was not applying the brake pedal.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> ParkBrakeApplyForce - Amount of force to aply to the rear rollers. </li>
     *     <li> ParkBrakeSpeedValue - Speed to command the front rollers to. </li>
     *     </ul>
     *
     * <b>Test Step Info:</b>
     *     <ul>
     *     <li> TestTime - Amount of time(ms) to apply torque to the drive axle rollers. </li>
     *     <li> Description - Description of the test step. </li>
     *     </ul>
     * 
     * <b>Data Tags:</b>
     *     <ul>
     *     <li> Zerospeed - System tag to use to check if the vehicle is at zero speed. </li>
     *     </ul>
     * 
     * <b>Fault Tags:</b>
     *     <ul>
     *     <li> ParkBrakeFault - Park brake is not set properly. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetWheelDistances() </li>
     *      <li> SetStartTime() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> SystemCommand() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateParkBrakeTestDistances() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetFaultCode() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param direction The direction to turn the rollers.  Valid values are "Forward", "Reverse" or "Both".
     *
     * @return The result of testing the park brake.
     */
    virtual const std::string PerformPBTorqueTest(const std::string &direction, bool isTandemAxle = false);

    /**
     * Perform park brake and park pawl test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The motors will be commanded to apply torque for a specified amount of time.  Torque will then be removed from
     * the rollers.  Once the wheels reach zero speed, the wheel distances will be analyzed to ensure the park brake
     * and park pawl are adjusted correctly.  After the test is complete, the motors will be placed in Boost mode.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> TestTime - Amount of time(ms) to apply torque to the rollers. </li>
     *      <li> TorqueValue - Amount of torque to apply to the rollers. </li>
     *      <li> MaxRearDistance - Maximum distance the rear wheels are allowed to travel without failing. </li>
     *      <li> MaxFrontDistance - Maximum distance the front wheels are allowed to travel without failing. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> Zerospeed - System tag to use to determine if the vehicle is at zero speed. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> GetTestStepInfoFloat() </li>
     *      <li> GetWheelDistances() </li>
     *      <li> SystemCommand() </li>
     *      <li> SetStartTime() </li>
     *      <li> GetWheelSpeeds() </li>
     *      <li> StatusSleep() </li>
     *      <li> TimeRemaining() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetTotalDistances() </li>
     *      <li> ValidateTorqueTestDriveAxle() </li>
     *      <li> UpdateResult() </li>
     *      <li> GetTestStepName() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @param direction The direction to turn the rollers.  Valid values are "Forward", "Reverse" or "Both".
     *
     * @return The result of performing the torque test.
     */
    virtual const std::string ComboTorqueTest(const std::string &direction);

    /**
     * Get the current forces.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current force for all wheels.  If the force file has not been opened yet, the force file will be opened.
     * The forces will then be read from the file.  The file will remain open to expedite further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentForces - Name of the file that contains the current forces. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsForceFileOpen() </li>
     *      <li> OpenForceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param forces An array for storing the force values.
     * 
     * @return Status of reading the wheel forces from the force file:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the forces were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the force file mutex could not be acquired. </li>
     *          </ul>
     */
    INT32 GetForces(float force[]);

    /**
     * Validate wheel distances for the park pawl test.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Validates the distances for the park pawl test.  The total distance traveled will be calculated.  The drive axle
     * will then be determined from the vehicle build data.  The distances for both the driven and non-driven axle will
     * be analyzed to ensure they fall within parameters.  The driven axle should cover very little distance compared
     * to the non-driven axle.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkPawlMaxDistance - Maximum distance the drive axle wheels are allowed to travel. </li>
     *      <li> ParkPawlMinDistance - Minimum distance the non-driven axle should travel if the brakes are not applied. </li>
     *      </ul>
     *
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> DriveAxle - System tag to get the drive axle from the vehicle build data. </li>
     *      </ul>
     * 
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkPawlFault </li>
     *      <li> ParkPawlDriverInfluenceFault </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTotalDistances() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> ValidateTorqueTestDriveAxle() </li>
     *      <li> ValidateTorqueTestSlaveAxle() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param startWheelDistance The starting distance readings before the park pawl test.
     * @param endWheelDistance   The ending distance readings after the park pawl test.
     *
     * @return The result of validaing the wheel distances from the park pawl testing.
     */
    const std::string ValidateParkPawlTestDistances( const WHEELINFO &startWheelDistance, 
                                                     const WHEELINFO &endWheelDistance,
                                                     bool isTandemAxle = false);

    /**
     * Verify the wheel distances are acceptable.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Validates the distances for a park brake test.  The rear distances should be very small since the park brake is
     * holding these wheels.  The front wheel distances should be fairly high as the front wheel are unrestrained.  If
     * the front wheel distances are small, it would be very probable the operator was applying the brake pedal.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ParkBrakeMaxDistance - Maximum distance the rear wheels are allowed to travel. </li>
     *      <li> ParkBrakeMinDistance - Minimum distance the front wheels must travel. </li>
     *      </ul>
     *
     * <b>Fault Tags:</b>
     *      <ul>
     *      <li> ParkBrakeFault - Park brake is not adjusted correctly. </li>
     *      <li> ParkBrakeDriverInfluenceFault - Operator applied the brake pedal during the test. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTotalDistances() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> ValidateTorqueTestDriveAxle() </li>
     *      <li> ValidateTorqueTestSlaveAxle() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResultWithDetail() <li>
     *      <li> GetTestStepInfo() </li>
     *      </ul>
     * 
     * @param startWheelDistance The starting distance readings before the park brake test.
     * @param endWheelDistance   The ending distance readings after the park brake test.
     *
     * @return The result of validating the wheel distances.
     */
    virtual const std::string 
    ValidateParkBrakeTestDistances(const WHEELINFO &startWheelDistance, 
                                   const WHEELINFO &endWheelDistance,
                                   bool isTandemAxle = false);

    /**
     * Validate the drive axle distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The left and right distances will be checked to ensure they are not above the maximum distance allowed.  If the
     * left or right distance is greater than the maximum distance allowed, a failure will be returned and a failure
     * test result will be reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @param leftDistance  The distance traveled by the left wheel.
     * @param rightDistance The distance traveled by the right wheel.
     * @param maxDistance   The maximum allowed distance for either of the restrained wheels to travel.
     * @param faultTag      Fault tag to report if the validation fails.
     *
     * @return Result of validating the wheel distances:
     *          <ul>
     *          <li> BEP_PASS_STATUS if the distances are less than the max distance. </li>
     *          <li> BEP_FAIL_STATUS if the distances are not less than the max distance. </li>
     *          </ul>
     */
    virtual const std::string 
    ValidateTorqueTestDriveAxle(float leftDistance,
                                float rightDistance,
                                float maxDistance,
                                const std::string &faultTag);

    /**
     * Validate the non-drive axle distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The left and right distances will be checked to ensure they are not below the minimum distance allowed.  If the
     * left or right distance is less than the minimum distance allowed, a failure will be returned and a failure
     * test result will be reported.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendSubtestResultWithDetail() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultName() </li>
     *      <li> GetFaultDescription() </li>
     *      </ul>
     *
     * @param leftDistance  The distance traveled by the left wheel.
     * @param rightDistance The distance traveled by the right wheel.
     * @param minDistance   The minimum allowed distance for either of the non-restrained wheels to travel.
     * @param faultTag      Fault tag to report if the validation fails.
     *
     * @return Result of validating the wheel distances:
     *          <ul>
     *          <li> BEP_PASS_STATUS if the distances are greater than the min distance. </li>
     *          <li> BEP_FAIL_STATUS if the distances are not greater than the max distance. </li>
     *          </ul>
     */
    const std::string 
    ValidateTorqueTestSlaveAxle(float leftDistance, 
                                float rightDistance, 
                                float minDistance, 
                                const std::string &faultTag);

    /**
     * Display a prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Display a prompt to the driver for a specified amount of time.  System status will be monitored during the time
     * the prompt is displayed.  Once the time limit expires, the prompt will be removed from the display.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> StatusSleep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      </ul>
     * 
     * @param promptTag  The prompt to be displayed.
     * @param promptBox  The prompt box where the prompt is displayed.
     * @param priority   The priority of the prompt.
     * @param promptTime The amount of time (ms) to display the prompt.
     * @param valueOne   Value to be displayed for the prompt.
     * @param valueTwo   Value to be displayed for the prompt.
     * @param valueThree Value to be displayed for the prompt.
     * 
     * @return The system status once the time limit expires.
     */
    BEP_STATUS_TYPE DisplayTimedPrompt(const std::string &promptTag, 
                                       const std::string &promptBox, 
                                       int priority, 
                                       INT32 promptTime,
                                       const string valueOne = "",
                                       const string valueTwo = "",
                                       const string valueThree = "");
    /**
     * Display a prompt.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Display a prompt to the driver for a specified amount of time.  System status will be monitored during the time
     * the prompt is displayed.  Once the time limit expires, the prompt will be removed from the display.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> StatusSleep() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      </ul>
     *
     * @param promptTag  The prompt to be displayed.
     * @param promptBox  The prompt box where the prompt is displayed.
     * @param priority   The priority of the prompt.
     * @param promptTime The amount of time (ms) to display the prompt.
     * @param valueOne   Value to be displayed for the prompt.
     * @param valueTwo   Value to be displayed for the prompt.
     * @param valueThree Value to be displayed for the prompt.
     *
     * @return The system status once the time limit expires.
     */
    BEP_STATUS_TYPE DisplayTimedPrompt(const std::string &promptTag, 
                                       const INT32 &promptBox, 
                                       INT32 priority, 
                                       INT32 promptTime,
                                       const string valueOne = "",
                                       const string valueTwo = "",
                                       const string valueThree = "");

    /**
     * Monitor system status while waiting for time to expire.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Sleep for the specified amount of time while checking the test status.  The function will sleep for 100ms increments
     * if <i>sleepTime</i> is greater than 100ms.  The component will then wake up and check system status.  If system
     * status is good and there is still sleep time remaining, the component will sleep for another period of time.  If
     * system status is not good, the function will exit and return the system status.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      </ul>
     * 
     * @param sleepTime The amount of time (ms) to sleep.
     *
     * @return The system status.
     */
    BEP_STATUS_TYPE StatusSleep( INT32 sleepTime);

    /**
     * Store the name of the ICM manager.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Store the name being used for the ICM manager process.
     * <p>
     * @param name The name of the ICM manager process.
     */
    virtual void SetICMManagerName(const string name);

    /**
     * Named data broker interface which provides the ability for the component 
     * to interact with the real time system.
     */
    INamedDataBroker *m_ndb;

    /** PromptServer interface for prompting the user. */
    IPromptServer *m_prompt;

    /**
     * FaultServer interface that is used by the component to report faults to 
     * the system.
     */
    IFaultServer *m_fault;

    /**
     * TestResultServer interface used by the component to report the results 
     * to each of the commanded tests.
     */
    ITestResultServer *m_testResult;

    /** This is a reference to the components test steps. */
    XmlNodeMap m_testSteps;

    /** The current test step that is being processed by the component. */
    const XmlNode *m_currentTestStep;

    /**
     * Set the flag indicating if a fatal failure has occurred or been cleared.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set or clear a fatal test component failure.  This will prevent any more test steps from being sequenced.
     * <p>
     * @param status The fatal failure status.
     */
    void SetFatalFailure(bool status);

    /**
     * Check fatal failure flag.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Get the status of the component failure flag.  If set, this indicates there should be no more testing.
     * <p>
     * @return Flag indicating if a fatal failure is set and testing should stop.
     */
    bool GetFatalFailure(void);

    /**
     * Store the flag indicating if the force file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the force file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of force file.
     */
    void SetForceFileOpen(bool fileOpen);

    /**
     * Store the flag indicating if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the speed file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of speed file.
     */
    void SetSpeedFileOpen(bool fileOpen);

    /**
     * Store the flag indicating if the distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the wheel distance file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of wheel distance file.
     */
    void SetWheelDistanceFileOpen(bool fileOpen);

    /**
     * Check if the config file should be reloaded.
     * <p><b>Category:</b> Utility.
     * <p><b>Description:</b><br>
     * Checks if the component should reload it's config file in response to an "Activate" command.  The file modification
     * time stamp will be checked against the time stamp when the config file was first loaded or last reloaded.  If the
     * time stamp is newer, the config file will be reloaded.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> FileHasChanged() </li>
     *      <li> GetConfigFile() </li>
     *      </ul>
     * 
     * @return Flag indicating if the config file should be reloaded.
     */
    virtual bool ShouldReloadConfigFile(void);

    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.  The test result
     * server interface will be recreated as will the IPC link.
     * <p>
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> TestSteps - List of the test steps supported by the component. </li>
     *     <li> Setup/TestDomain - Domain (Foreground/Background) of the component. </li>
     *     </ul>
     * 
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> KeepTestStatusOnExit - Flag to determine if the test component should not update it's test status when 
     *                                 the component exits. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SetTestDomain() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetParameterBool() </li>
     *      <li> UpdateTestStatus() </li>
     *      <li> AddIPCLink() </li>
     *      <li> SetStatus() </li>
     *      <li> InitializationComplete() </li>
     *      <li> Register() </li>
     *      </ul>
     */
    virtual void WarmInitialize(void);

    /**
     * Get the test component classification.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Returns the TaskMon classification of this process.
     *
     * @return TASKMON_CLASS_COMPONENT
     * 
     * @see TaskMonClass_t
     */
    TaskMonClass_t GetTaskClass(void) const;

    /**
     * Read memory stats for all components.
     * <p><b>Category:</b> Test Step
     * <p><b>Description</b><br>
     * Read memory stats for all components. Memory usage, number of threads, and number of file descriptors are read 
     * and logged for each component.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateTaskStats() </li>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepName() </li>
     *      </ul>
     * 
     * @param value Value that was commanded by the test sequencer.
     *
     * @return Status of reading the memory statistics for eac component:
     *          <ul>
     *          <li> BEP_PASS_STATUS if the memory statistics were read successfully. </li>
     *          <li> BEP_FAIL_STATUS if the memory statistics were not read successfully. </li>
     *          </ul>
     */
    virtual const std::string TestStepReadMemStats(const std::string &value);


    virtual string GetRootDir(void);

    /**
     * Read memory stats for the specified task.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Read memory stats.  Memory usage, number of threads, and number of file descriptors are read and logged.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> LogTaskStatInfo - Flag indicating if the stats should be logged.  If set to false, the stats will not
     *                             be read. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameterBool() </li>
     *      </ul>
     * 
     * @param taskName The name of the task to log stats for.
     * @param prefix   The prefix to add to the task stats file.
     *
     * @return The status of the task gathering information.
     */
    virtual int UpdateTaskStats( const std::string &taskName, const char *prefix=NULL);

    /** Reference to the components objectives. */
    XmlNodeMap m_objectives;

    /** Reference to the parameter tags in the configuration file */
    XmlNodeMap m_parameters;

    /** Reference to the fault lookup table. */
    XmlNodeMap m_faults;

    /** Reference to the system tags to reset. */
    XmlNodeMap m_systemTags;

    /**
     * This variable is used to indicate if the current test component is to be 
     * a foreground or background.  If set to true, the component is a 
     * foreground test, false - background test.
     */
    bool m_foregroundTest;

    /** The number of rollers that are being controlled. */
    INT32 m_rollerCount;

    /** Name of the ICM manager. */
    string m_icmManagerName;

    /** Communication to use for talking with the ICM manager. */
    IBepCommunication m_icmMgrComm;

    /** Flag which indicates the current drive axle of the vehicle. */
    DriveAxle m_driveAxle;

    /** List of prompts the test component is capable of displaying. */
    XmlNodeMap m_prompts;

    /**
     * Semaphore that regulates access to the command ivar.  This allows the 
     * process to wait until a command has been received.
     */
    BepSemaphore m_semInitialized;

    /** Starting and ending distance readings for the park brake test. */
    WHEELINFO   m_parkBrakeDistances[ 2];

    /** Starting and ending distance readings for the park pawl test. */
    WHEELINFO   m_parkPawlDistances[ 2];

    /**
     * Interface object used to communicate with the motor controller.
     * Used for commanding non-standard motor modes (i.e. torque & speed 
     * simultaneously).
     */
    IMotorController m_MotorController;

    /** XmlNode pointer to our config node as passed into Initialize(). */
    const XmlNode *m_configCopy;

private:

    /**
     * Check the provided direction to ensure it is valid.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * The supplied direction will be compared against the list of valid directions to determine if a valid direction 
     * was requested for the park brake test.  Valid directions are "Forward", "Reverse", and "Both".
     * <p>
     * @param direction The direction specified for the park brake test.
     *
     * @return Flag indicating if a valid direction was supplied.
     *         <ul>
     *         <li> True - A valid direction was supplied. </li>
     *         <li> False - An invalid direction was specified. </li>
     *         </ul>
     */
    const bool IsParkBrakeDirectionValid(const string &direction);

    /**
     * <b>Description:</b><br>
     * Check the configuration file to see if the park brake handle force should 
     * be added to the test results.  If the parameter is set to true, add the
     * current park brake handle force to the test results.
     * <br><p>
     */ 
    void ProcessParkBrakeHandleForce(void);

    /** The overall status of the component. (Pass/Fail/Abort/...). */
    std::string m_overallResult;

    /**
     * Result status of the sequenced objective.  This is used to respond back 
     * to the process that commanded the executed objective.
     */
    std::string m_result;

    /** The value of the last command received by the component. */
    std::string m_command;

    /**
     * The value that is associated with the last command received by the 
     * component.
     */
    std::string m_value;

    /**
     * Semaphore that regulates access to the command ivar. This allows the 
     * process to wait until a command has been received.
     */
    BepSemaphore m_semCommand;

    /**
     * Semaphore that indicates the system status.  This will prevent multiple 
     * commands from being executed simultaneously.
     */
    BepSemaphore m_semReady;

    /**
     * Semaphore that regulates access to the result ivar. This will block the 
     * thread that commanded the component until a result has been determined.
     */
    BepSemaphore m_semResponse;

    /** The name of the component for test result information. */
    std::string m_componentName;

    /** Start time holder used for timing a test step. */
    struct timespec m_startTime;

    /** Start time holder used for secondary timing in a test step. */
    struct timespec m_secondaryStartTime;

    /**
     * Retest flag.  This is automatically set when the sequencer calls for the 
     * overall status of the component.
     */
    bool m_retest;

    /**
     * Previous result. Set by SendOverallResult. This is the previous overall 
     * test result. The value is only valid if m_retest is true.
     */
    string m_previousResult;

    /**
     * This flag indicates that there is a fatal failure in the test component 
     * and no more tests should be sequenced.
     */
    bool m_fatalFailure;

    /** Items to check when checking system status. */
    XmlNodeMap m_statusCheckItems;

    /** File descriptor for the force file. */
    INT32 m_forceFile;

    /**
     * Flag indicating the open status of the force file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_forceFileOpen;

    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_forceFileMutex;

    /** File descriptor for the speed file. */
    INT32 m_speedFile;

    /**
     * Flag indicating the open status of the speed file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_speedFileOpen;

    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_speedFileMutex;

    /** File descriptor for the wheel distance file. */
    INT32 m_wheelDistanceFile;

    /**
     * Flag indicating the open status of the wheel distance file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_wheelDistanceFileOpen;

    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_wheelDistanceFileMutex;

    /**
     * Vector to contain valid directions in which the park brake test can 
     * run.  The direction from the config fiel will be checked against these 
     * values to ensure a proper direction is specified.
     */
    vector<string> m_validParkBrakeDirections;

    /** Name of the file where the fault list is stored. */
    std::string    m_faultFile;

    /** Time of the last modification to the fault list file. */
    time_t         m_faultFileTime;

    /** The "real" task registration name. */
    std::string    m_regName;

    /** The name of the symlink to create when the component is activated. */
    std::string    m_regAlias;
};

#endif // GenericTC_H

