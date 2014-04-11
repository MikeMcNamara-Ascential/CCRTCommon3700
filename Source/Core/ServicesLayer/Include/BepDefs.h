//****************************************************************************
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/BepDefs.h 6     10/26/06 9:10a Rwiersem $
//
// DESCRIPTION:
//
//    Provide constant definitions common to all sub-systems of the BEP
//    End-of-Line Software rewrite software.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepDefs.h $
// 
// 6     10/26/06 9:10a Rwiersem
// Changes for the 06102701 core release:
// 
// Added defines for test component activation and deactivation
// 
// 5     2/15/06 7:45p Cward
// Moved definition of Log Port Pulse Codes from Log Port Defs to Bep Defs
//
// 4     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 3     7/20/05 8:44p Cward
// Removed trinary. Added TaskMon Pulse
//
// 2     5/25/04 4:11p Cward
// Moved pound if to below the haeder comments. Added definition of eeprom
// test result
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/20/03 8:48a Khykin
// Latest changes.
//
// 1     6/12/03 4:06p Khykin
// Initial checkin.
//
// 3     5/29/03 6:29a Khykin
// Added a new status of "Shutdown".  The launcher uses this status to
// indicate that it should shut down the OS.
//
// 2     4/22/03 3:11p Khykin
// Updating for MotorController changes.
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 87    11/20/02 6:58a Khykin
// Adding wheel definitions and extended test definitions.
//
// 86    10/07/02 9:44a Derickso
// Added BEP_PRIORITY define for priority based prompting.
//
// 85    8/23/02 12:57p Dan.erickson
// Added YesNo state for InputServer.
//
// 84    8/23/02 8:07a Dan.erickson
// Added macros for TestResultCode, Clear All DTC, and an enumeration for
// Abort status.
//
// 83    7/31/02 11:21a Skypig13
// Added test definitions
//
// 82    7/29/02 8:17a Dan.erickson
// Added macros for BEP_COMM_BUS and INPUT_SERVER_TEST_RESULT_STATE.
//
// 81    6/27/02 3:48p Skypig13
// Added more macros for error handling
//
// 80    6/25/02 5:03p Skypig13
// Added macros for error descriptions when message fails
//
// 79    6/19/02 2:15p Skypig13
// Added DATA_SERVER_PORT macro
//
// 78    6/13/02 2:49p Dan.erickson
// Added VEHICLE_TEST_RESULT_SERVER macro.
//
// 77    6/07/02 12:02p Dan.erickson
// Added TestNotStarted macro.
//
// 76    5/20/02 4:27p Dan.erickson
// Added PFS server name.
//
// 75    4/29/02 4:13p Kh
// Adding COMMAND_CLUTCH
//
// 74    4/25/02 4:39p Skypig13
// Updated with new changes
//
// 73    4/16/02 2:46p Dan.erickson
// Added defines for the Vin read status values.
//
// 72    4/08/02 2:20p Skypig13
// Added MIN_RATE and MAX_RATE for read verifications
//
// 71    4/04/02 3:04a Bmeinke
// Added defines for DEFAULT_TIMEOUT, DEFAULT_BUFFER_SIZE, mSEC_nSEC, and
// nSEC_mSEC( x)
//
// 70    4/03/02 7:34a Kh
// Updating the "Unsupported" definition.
//
// 69    3/28/02 1:35p Dan.erickson
// Added well-known name of InputServer.
//
// 68    3/28/02 10:19a Dan.erickson
// Added states for the InputServer.
//
// 67    3/25/02 6:27p Kh
// Updating the global tag for requesting OverallTest
//
// 66    3/20/02 10:57p Bmeinke
// Added #include <sys/neutrino.h> if compiling for QNX neutrino to ensure
// that _NTO_VERSION is defined
//
// 65    3/20/02 8:16a Dan.erickson
// Added well-known name for TestResultServer.
//
// 64    3/06/02 2:26p Kh
// Updating
//
// 63    2/19/02 7:47p Bmeinke
// Changed CALIBRATE_MODE value from Calibrate to LossCompensation
//
// 62    2/05/02 12:26a Skypig13
// Added BEP_XML_ROOT_END
//
// 61    1/30/02 2:26p Kh
// Adding a software and hardware error to the system errors.
//
// 60    1/29/02 9:05p Kh
// Adding a timeout definition to the BEP_STATUS enumeration.
//
// 59    1/18/02 4:58p Kh
// Adding the PASS status.
//
// 58    1/11/02 5:11p Brian.meinke
// Added BEP_MSSG_CLASS xml tag name to be used to distinguish different
// sources of messages in the system
//
// 57    1/10/02 11:15a Russ.ross
// Updating with a missing define...
//
// 56    1/09/02 10:54a Dan.erickson
// Added Plant Host name.
//
// 55    1/04/02 11:21a Kh
// Updating a redundant declaration.
//
// 54    12/26/01 1:46p Kh
// Adding a BEP_UNTESTED_RESPONSE definition.
//
// 53    12/20/01 4:11p Kh
// Adding definition for OverallPassFail.
//
// 52    12/20/01 2:31p Kh
// Adding a VEHICLE_TEST definition.
//
// 51    12/18/01 11:32a Kh
// Adding the Register command definition.
//
// 50    12/14/01 4:49p Kh
// Adding the REGISTER_PULSE pulse definition.
//
// 49    12/13/01 12:28a Skypig13
// Added DATA_SERVER_NAME
//
// 48    12/03/01 4:09p Dan.erickson
// Added VehicleDataBroker name and WorkCellController name.
//
// 47    11/30/01 12:58p Brian.meinke
// Added PULSE_HI_PRIO define for setting the priority of a pulse to the
// highest value possible for a process
//
// 46    11/29/01 3:52p Kh
// Adding status definitions.
//
// 45    11/26/01 12:27p Dan.erickson
// Added definitions for DataType and Prompt.
//
// 44    11/20/01 8:52a Kh
// Adding standard test status definitions.
//
// 43    11/15/01 4:48p Skypig13
// Added BEP_SEVERITY and BEP_NO_SUPPORTED_RESPONSE
//
// 42    11/14/01 4:37p Kh
// Adding test result types.
//
// 41    11/13/01 2:51p Dan.erickson
// Re-added BEP_RESULT after Kevin removed it.
// Added BEP_UNITS.
//
// 40    11/13/01 8:51a Kh
// Adding definitions for Pass and Fail.
//
// 37    10/24/01 11:19a Kh
// Updating the motor mode definitions.
//
// 36    10/23/01 6:23p Skypig13
// Added BEP_STATUS
//
// 35    10/16/01 4:28p Dan.erickson
// Added defines for BEP_DESCRIPTION, BEP_PROCESS and BEP_TEST_RESULT.
//
// 34    10/09/01 3:45p Skypig13
// Added NANOS_PER_SECOND
//
// 33    10/09/01 3:23p Kh
// Updating motor mode tags.
//
// 32    10/08/01 10:05a Kh
// Adding motor mode definitions.
//
// 31    10/04/01 7:32p Skypig13
// Added TIMER_PULSE
//
// 30    10/04/01 6:35p Skypig13
// Added BEP_DATA_RATE
//
// 29    10/04/01 5:34p Skypig13
// Added NAMED_DATA_BROKER_NAME
//
// 28    10/04/01 1:33p Skypig13
// added BEP_UNSUPPORTED_RESPONSE
//
// 27    10/04/01 1:22p Kh
// Updating the message type definition.
//
// 26    9/12/01 11:28p Brian.meinke
// Added BepTagDefs.h include file
//
// 25    9/12/01 11:27p Brian.meinke
// Changed BEP_SUBSCRIPTION_UPDATE to BEP_PUBLISH
//
// 24    9/11/01 3:55p Brian.meinke
// Added BEP_SUBSCRIPTION_UPDATE define
//
// 23    8/30/01 12:12p Skypig13
// Added THREAD_DEATH pulse value
//
// 22    8/28/01 8:24p Skypig13
// Change parameters to not take defaults and add timeout to Write()
//
// 21    8/14/01 3:57p Skypig13
// Added BEP_NONE
//
// 20    8/13/01 9:10a Brian.meinke
// CHanged typedef of INT8 from "signed char" to plain old char
//
// 19    8/02/01 4:24p Skypig13
// Left out , on line 275
//
// 18    8/02/01 4:17p Skypig13
// Add BEP_STATUS_FAILURE
//
// 17    7/24/01 3:02p Skypig13
// Change BEP_VALIDATE to BEP_VERIFY
//
// 16    7/19/01 9:43a Skypig13
// Added BEP_RELOAD and RELOAD_PULSE
//
// 15    7/17/01 7:03p Skypig13
// Change SYSTEM_PULSE to 50
//
// 14    7/17/01 1:57p Skypig13
// Added BEP_ABORT_RESPONSE, BEP_RESET_RESPONSE
//
// 13    7/16/01 5:15p Skypig13
// Added BEP_TERMINATE
//
// 12    7/12/01 5:47p Skypig13
// Added BEP_AVAILABLE_RESPONSE
//
// 11    7/12/01 9:13a Kh
// Adding Pulse Definitions.
//
// 10    7/06/00 7:47p Skypig13
// Add BEP_VALIDATE and BEP_INVALIDATE macros
//
// 9     6/27/00 7:30p Skypig13
// Changed value of BEP_XML_MSSG_TYPE
//
// 8     6/27/00 6:56p Skypig13
// Fixed defines for BEP_XML_ROOT and BEP_XML_TAG
//
// 7     6/27/00 9:02a Skypig13
// Added BEP_ERROR_RESPONSE macro
//
// 6     6/26/00 2:05p Skypig13
// Fixed spelling of BEP_UNAVAILABLE_RESPONSE
//
// 5     6/08/01 12:54p Skypig13
// Added defines for communicaiton
//
// 4     6/06/01 5:39p Skypig13
// Added BEP_XML_MSG_TYPE for defining the standard command
// attribute of 'MssgType'.
//
// 3     5/03/01 10:40a Skypig13
// Add BepXmlRoot define
//
// 2     4/10/01 12:28p Brian.meinke
// Changed BEP_XML_TAG define to BEPXmlRoot
//
// 1     4/10/01 10:46a Admin
//
// 3     3/06/01 6:31p Brian.meinke
// Added BEP_XML_TAG definition for common BEP XML Root Node Value
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 11    1/24/00 10:09a Hil.sybesma
//
// 10    10/20/99 5:12p Michael.haugen
// Put back to "original" version 8, which was "correct".
//
// 9     10/19/99 6:14p Hil.sybesma
//
// 8     10/13/99 4:50p Michael.haugen
// Changed UINT8 data type definition from "unsigned char"  to just
// "char".  Apparently WatcomC++ compiler cannot translate it's default
// "char" data type ( unsigned ) to a variable assigned explicitly as
// "unsigned char".
//
// 7     10/07/99 11:10p Michael.haugen
// Added #ifdef wrapper around "sys/kernel.h" so that header file could be
// included in GUI compiles as well.  Also, incorporated "proper" size
// typecasting of data types for 'C' and 'C++' compatibility.
//
// 6     10/05/99 2:07p Brian.vanenk
//
// 5     10/05/99 9:13a Gene.swope
// Add Enumeration for test evaluataion
//
// 4     8/20/99 3:16p Michael.haugen
// Add additional "generic" BEPCO definitions
//
// 3     8/20/99 8:42a Michael.haugen
// Fixed a couple of compile time errors.
//
// 2     8/19/99 11:44a Michael.haugen
// Added BEP_STATUS_TYPE return status definition
//
// 1     8/19/99 9:09a Michael.haugen
// Preliminary version to use as reference for setup data store accesses.
//
//****************************************************************************

#ifndef _BEPDEFS_H_
#define _BEPDEFS_H_

#define DEBUG 0

//----------------------------------------------------------------------------
// SYSTEM HEADER FILE INCLUSIONS
//----------------------------------------------------------------------------
#include <wchar.h>                  // UNICODE character stuff

#ifdef   __WATCOMC__                // QNX4 w/WATCOM
    #ifndef  __KERNEL_H_INCLUDED
        #include <sys/kernel.h>      // for pid_t
    #endif

    #include <tchar.h>                  // Generic Text Stuff

/***********************/
/***** NEUTRUNO/GCC ****/
/***********************/
#elif defined(__QNXNTO__)           //Neutrino w/GCC
    #ifndef  __KERNEL_H_INCLUDED
        #include <sys/types.h>      // for pid_t
    #endif

    #include <sys/neutrino.h>       // for _NTO_VERSION
#endif //__WATCOMC__

//----------------------------------------------------------------------------
// USER DEFINED HEADER FILE INCLUSIONS
//----------------------------------------------------------------------------
#include "BepTagDefs.h"

//----------------------------------------------------------------------------
// GLOBAL DEFINITIONS
//----------------------------------------------------------------------------
#define STR(x) #x
#define STRINGIFY(x) STR(x)
#ifdef VERSION_STRING
    #define CCRT_VERSION_STR STRINGIFY(VERSION_STRING)
#else
    #define CCRT_VERSION_STR    " "
#endif

//----------------------------------------------------------------------------
// EXTERNAL DECLARATIONS
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// CONSTANT DEFINITIONS
//----------------------------------------------------------------------------

#ifndef PASS
    #define  PASS 0
#endif

#ifndef FAIL
    #define FAIL 2
#endif

#ifndef NO_TEST
    #define NO_TEST 3
#endif

#ifndef ERROR
    #define ERROR 5
#endif

// Methods returning the above defined PASS and FAIL are declared to return
// the type 'pf_t':
typedef short pf_t;

//----------------------------------------------------------------------------
// BEPCO Boolean definition
//----------------------------------------------------------------------------
#ifdef TRUE
    #undef TRUE
    #undef FALSE
#endif

typedef  int     Boolean;
#define  TRUE    ((Boolean)1)
#define  FALSE   ((Boolean)0)



//----------------------------------------------------------------------------
// BEPCO Generic definitions
//----------------------------------------------------------------------------
#define  NO      0
#define  YES     1

#define  OFF     0
#define  ON      1

#define  READ    0
#define  WRITE   1



//----------------------------------------------------------------------------
// BEPCO wheel definitions for dealing with force arrays
//----------------------------------------------------------------------------
#define LFWHEEL 0
#define RFWHEEL 1
#define LRWHEEL 2
#define RRWHEEL 3
#define LTWHEEL 4
#define RTWHEEL 5
#define MAXWHEELS   6   /* the maximum number of wheels that a vehicle can have */




/**
 * Default timeout value used by IBepCommunication objects
 */
#define DEFAULT_TIMEOUT         2000



/**
 * Default buffer size (in bytes) used by IBepCommunication objects
 */
#define DEFAULT_BUFFER_SIZE     3000


/**
 * Minimum data rate a client can request for streaming data (messages per second)
 */
#define MIN_RATE 1


/**
 * Maximum data rate a client can request for streaming data (messages per second)
 */
#define MAX_RATE 20


/**
 * Amount of space to be reserved for responses
 * NOTE:  This should be a multiple of 16 and just larger than typical response
 */
#define STRING_RESERVE_SIZE 128


/**
 * Standard port to connect to the Data Server
 */
#define DATA_SERVER_PORT    10000



// Standard BEP XML definitions
#define BEP_XML_TAG                 "BepXmlRoot"
#define BEP_XML_ROOT                "BepXmlMssg"
#define BEP_XML_ROOT_END            "/"BEP_XML_ROOT
#define BEP_XML_MSG_TYPE            "MssgType"
#define BEP_DATA_RATE               "DataRate"
#define BEP_DATA_TYPE               "DataType"
#define BEP_PROMPT                  "Prompt"
#define BEP_PRIORITY                "Priority"
#define BEP_TEST_RESULT_CODE        "TestResultCode"
#define BEP_DESCRIPTION             "Description"
#define BEP_PROCESS                 "Process"
#define BEP_RESULT                  "Result"
#define BEP_SEVERITY                "Severity"
#define BEP_UNITS                   "Units"
#define BEP_STATUS                  "Status"
#define BEP_MSSG_CLASS              "MssgClass"
#define BEP_REASON                  "Reason"
#define BEP_COMM_BUS                "CommunicationBus"
#define BEP_CONVERT_TO_ENG_UNITS    "ConvertToEnglishUnits"



// Standard BEP responses
#define BEP_ERROR_RESPONSE          "Error"
#define BEP_FAILURE_RESPONSE        "Failure"
#define BEP_INVALID_RESPONSE        "Invalid"
#define BEP_SUCCESS_RESPONSE        "Success"
#define BEP_NO_IMPLEMENTED_RESPONSE "NotImplemented"
#define BEP_AVAILABLE_RESPONSE      "Available"
#define BEP_UNAVAILABLE_RESPONSE    "Unavailable"
#define BEP_UNTESTED_RESPONSE       "Untested"
#define BEP_UNSUPPORTED_RESPONSE    "Unsupported"
#define BEP_TIMEOUT_RESPONSE        "Timeout"
#define BEP_TESTING_RESPONSE        "Testing"
#define BEP_PASS_RESPONSE           "Pass"
#define BEP_FAIL_RESPONSE           "Fail"
#define BEP_ABORT_RESPONSE          "Abort"
#define BEP_ACTIVATE_RESPONSE       "Activate"
#define BEP_DEACTIVATE_RESPONSE     "Deactivate"
#define BEP_FATALFAIL_RESPONSE      "FatalFail"
#define BEP_HARDWAREFAIL_RESPONSE   "HardwareFail"
#define BEP_SOFTWAREFAIL_RESPONSE   "SoftwareFail"
#define BEP_RESET_RESPONSE          "Reset"
#define BEP_REGISTER_RESPONSE       "Register"
#define BEP_OVERALL_RESPONSE        "OverallResponse"
#define BEP_NONE                    "None"
#define BEP_SKIP_RESPONSE           "Skip"
#define BEP_NO_CHILDREN             "No Children"
#define BEP_ROOT_NAME_WRONG         "Root Name Wrong"
#define BEP_ATTRIBUTE_VALUE_WRONG   "Attribute Value Wrong"
#define BEP_NO_MSG_TYPE_ATTRIBUTE   "No "BEP_XML_MSG_TYPE" Attribute"
#define BEP_NO_BEP_DATA_TYPE_ATTR   "No "BEP_DATA_TYPE" Attribute"
#define BEP_NO_BEP_PROMPT_ATTRIBUTE "No "BEP_PROMPT" Attribute"
#define BEP_NO_BEP_REQ_PROMPT_ATTR  "Either No "BEP_DATA_TYPE" or "BEP_PROMPT" Attribute"
#define BEP_UNKNOWN_ERROR_RESPONSE  "Unknown Error"
#define BEP_EMPTY_MESSAGE           "Empty Message"
#define BEP_NO_DATA                 "No Data"



// Standard BEP Test Status
#define BEP_SEQUENCED_STATUS        "Sequenced"
#define BEP_TESTING_STATUS          "Testing"
#define BEP_COMPLETE_STATUS         "Complete"
#define BEP_TERMINATE_STATUS        "Terminate"
#define BEP_ACTIVATE_STATUS         "Activate"
#define BEP_DEACTIVATE_STATUS       "Deactivate"
#define BEP_SHUTDOWN_STATUS         "Shutdown"
#define BEP_UNTESTED_STATUS         "Untested"
#define BEP_PASS_STATUS             "Pass"
#define BEP_FAIL_STATUS             "Fail"
#define BEP_ABORT_STATUS            "Abort"
#define BEP_FATALFAIL_STATUS        "FatalFail"
#define BEP_HARDWAREFAIL_STATUS     "HardwareFail"
#define BEP_SOFTWAREFAIL_STATUS     "SoftwareFail"
#define BEP_INITIALIZED_STATUS      "Initialized"
#define BEP_TIMEOUT_STATUS          "Timeout"
#define BEP_RUNNING_STATUS          "Running"
#define BEP_TEST_NOT_REQUIRED_STATUS "Test Not Required"
#define BEP_TEST_NOT_STARTED        "TestNotStarted"
#define BEP_SKIP_STATUS             "Skip"
#define CLEAR_ALL_DTC               "All DTCs"



// Standard BEP Commands
#define BEP_TEST_RESULT             "TestResult"         // Extension for Test Result server.
#define BEP_INTERMEDIATE_TEST_RESULT             "IntermediateTestResult"         // Extension for Test Result server.
#define BEP_INTERMEDIATE_OVERALL_RESULT             "IntermediateOverallResult"         // Extension for Test Result server.
#define BEP_READ                    "Read"
#define BEP_WRITE                   "Write"
#define BEP_SERVER_WRITE			"ServerWrite"
#define BEP_COMMAND                 "Command"
#define BEP_SUBSCRIBE               "Subscribe"
#define BEP_UNSUBSCRIBE             "Unsubscribe"
#define BEP_VERIFY                  "Verify"
#define BEP_INVALIDATE              "Invalidate"
#define BEP_TERMINATE               "Terminate"
#define BEP_RELOAD                  "Reload"
#define BEP_PUBLISH                 "Publish"
#define BEP_REGISTER                "Register"
#define BEP_ACTIVATE                "Activate"
#define BEP_DEACTIVATE              "Deactivate"


// Standard Motor Modes
#define MOTOR_MODE                  "MotorMode"
#define BOOST_MODE                  "Boost"
#define QUICK_BRAKE_MODE            "QuickBrake"
#define REAR_BRAKE_ASSIST_MODE      "RearBrakeAssist"
#define SPEED_MODE                  "Speed"
#define INDIVIDUAL_SPEED_MODE       "IndividualSpeed"
#define TORQUE_MODE                 "Torque"
#define NO_LIMIT_TORQUE_MODE        "NoLimitTorque"
#define LOSSCOMPENSATION_MODE       "LossCompensation"
#define IDLE_MODE                   "Idle"
#define UPLOAD_PARAMETERS_MODE      "UploadDriveParameters"
#define COMMAND_SPEED               "CommandSpeed"
#define COMMAND_TORQUE              "CommandTorque"
#define COMMAND_CLUTCH              "CommandClutch"
#define COMMAND_MOTOR_DELAY         "CommandMotorDelay"



// Standard Test Status Definitions
#define COMMAND_TEST                "CommandTest"
#define TEST_START                  "Start"
#define TEST_START_COMMANDPHASE     "StartCommandPhase"
#define TEST_RETEST                 "Retest"
#define TEST_END                    "Complete"
#define TEST_ABORT                  "Abort"
#define TEST_TERMINATE              "Terminate"
#define TEST_LOSS_COMPENSATION      "LossCompensation"
#define TEST_BASE_BRAKE             "BaseBrake"



// Standard InputServer states
#define INPUT_SERVER_VIN_STATE              "Vin"
#define INPUT_SERVER_NORMAL_STATE           "Normal"
#define INPUT_SERVER_DRIVER_NUMBER_STATE    "DriverNumber"
#define INPUT_SERVER_TEST_RESULT_STATE      "TestResult"
#define INPUT_SERVER_YES_NO_STATE           "YesNo"



// Testing States
#define TEST_STATUS_NOT_STARTED         "TestNotStarted"
#define TEST_STATUS_IN_PROGRESS         "TestInProgress"
#define TEST_STATUS_PASSED              "Passed"
#define TEST_STATUS_FAILED              "Failed"
#define TEST_STATUS_ABORT               "Abort"
#define TEST_STATUS_ERROR               "Error"
#define TEST_STATUS_TIMEOUT             "Timeout"



// Standard Vin Read status values
#define NO_VIN                    "Red"
#define PROCESSING_VIN            "Yellow"
#define READY_TO_TEST             "Green"



// Component Standard Objective Definitions
#define COMPONENT_OBJECTIVE_FINISHUP    "FinishUp"
#define COMPONENT_OVERALL_RESPONSE      "Overall"


// Machine Types
#define MACHINE_TYPE_3700           "3700"
#define MACHINE_TYPE_4AXLE			"5702"


//----------------------------------------------------------------------------
// BEPCO Pulse Definitions
//----------------------------------------------------------------------------
#define SYSTEM_PULSE        50          /* system pulse definition */



// types of system pulses
#define ABORT_PULSE         10
#define TERMINATE_PULSE     20
#define RESET_PULSE         30
#define RELOAD_PULSE        40
#define TIMER_PULSE         50
#define TASK_MON_PULSE      60
#define ACTIVATE_PULSE      70
#define DEACTIVATE_PULSE    71

#define PULSE_HI_PRIO       sched_get_priority_max( sched_getscheduler(0))



/**
 * The number of nanoseconds in a millisecond.
 * This needs to be a signed 64 bit value.
 */
#define NANOSINAMILLI       ((INT64)1000000LL)


/**
 * The number of nanoseconds in a second.
 * This needs to be a signed 64 bit value.
 */
#define NANOS_PER_SECOND    ((INT64)1000000000LL)



/**
 * millisec to nano-second conversion
 */
#define mSEC_nSEC( x)   ((x) * NANOSINAMILLI)



/**
 * nano-second to millisec conversion
 */
#define nSEC_mSEC( x)   ( (x) / NANOSINAMILLI)



//----------------------------------------------------------------------------
// BEPCO Process Name Definitions
//----------------------------------------------------------------------------
#define DATA_LOGGER_NAME            "core/Log"
#define NAMED_DATA_BROKER_NAME      "core/NamedDataBroker"
#define TEST_RESULT_SERVER_NAME     "core/TestResultServer"
#define VEHICLE_DATA_BROKER_NAME    "core/VehicleDataBroker"
#define WCC_NAME                    "core/WorkCellController"
#define PFS_SERVER_NAME             "core/PFS"
#define DATA_SERVER_NAME            "core/DataServer"
#define PLANT_HOST_NAME             "core/PlantHost"
#define INPUT_SERVER_NAME           "core/InputServer"
#define VEHICLE_TEST_RESULT_SERVER  "application/Sequencer"



//----------------------------------------------------------------------------
// Consistent data representation
//----------------------------------------------------------------------------
typedef char                INT8;
#ifdef __WATCOMC__


// Watcom C++ defaults "char" type to "unsigned char", but cannot convert
// data type correctly when used as an input function parameter.
typedef char                UINT8;
#else
typedef unsigned char       UINT8;
#endif // __WATCOMC__

#ifdef __cplusplus
typedef short int           INT16;
typedef unsigned short int UINT16;
typedef signed int          INT32;
typedef unsigned int       UINT32;
typedef signed long long    INT64;
typedef unsigned long long UINT64;
#else
typedef int                 INT16;
typedef unsigned int       UINT16;
typedef signed long         INT32;
typedef unsigned long      UINT32;
typedef signed long long    INT64;
typedef unsigned long long UINT64;
#endif // __cplusplus



//------------------------------------------------------------------------------
// Test result enumerations (instead of simple pass/fail)
//------------------------------------------------------------------------------
typedef enum
{
    BEP_NOT_TESTED = 0,
    BEP_PASS,
    BEP_FAIL,
    BEP_ABORT,
    BEP_TEST_NOT_REQUIRED
} BEP_TEST_EVAL;

/**
 * Test result values to be written to EEPROM (Module).
 * @since Version 1.0
 */
typedef enum
{
    EEPROM_PASS = 0,
    EEPROM_FAIL
} EEPROM_TEST_RESULT;

//------------------------------------------------------------------------------
// Function return type (instead of simple true/false)
//------------------------------------------------------------------------------
typedef enum
{
    BEP_STATUS_ERROR=0xFFFFFFFF,
    BEP_STATUS_SUCCESS=0,
    BEP_STATUS_NA,
    BEP_STATUS_FAILURE,
    BEP_STATUS_ABORT,
    BEP_STATUS_TIMEOUT,
    BEP_STATUS_SOFTWARE,
    BEP_STATUS_HARDWARE,
    BEP_STATUS_SKIP,
    BEP_STATUS_END_OF_LIST,
} BEP_STATUS_TYPE;

/**
 * Pulse code identifiers for logical port events
 *  LOG_PORT_IRQ_PULSE_CODE
 *  LOG_PORT_NOTIFY_PULSE_CODE
 *  MSSG_GAP_TIMER
 *  BYTE_GAP_TIMER
 */
typedef enum
{
    /**
     * Pulse code to be associated with serial interrupt events
     */
    LOG_PORT_IRQ_PULSE_CODE     = 0x01,

    /**
     * Pulse code used to tell the logical port driver to send event notifications
     * to subscribed clients. The pulse value will be either PORT_SUBSCRIBE_RX to
     * deliver notifications to clients subscribed for received data events or
     * PORT_SUBSCRIBE_TX to deliver notifications to clients subscribed for transmit
     * data events.
     */
    LOG_PORT_NOTIFY_PULSE_CODE  = 0x02,

    /**
     * Pulse code identifier for inter-message timing events
     */
    MSSG_GAP_TIMER              = 0x03,

    /**
     * Pulse code identifier for inter-byte timing events
     */
    BYTE_GAP_TIMER              = 0x04,

    /**
     * Pulse code returned from the serial driver to tell the reader thread that new
     * data is available to read
     */
    SERIAL_DATA_READY           = 0x05,

    /**
     * Pulse code used by the serial server to tell us
     * to register
     */
    REGISTER_PULSE              = 0x06,

    /**
     * Pulse code identifier for inter-message timing events
     */
    REPLY_TIMEOUT_TIMER         = 0x07,

    NEXT_READ_CODE

} LogPortPulseCodes_t;




#endif //_BEPDEFS_H_
