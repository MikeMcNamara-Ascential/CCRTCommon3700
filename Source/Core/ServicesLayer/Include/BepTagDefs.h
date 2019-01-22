//******************************************************************************
//
// Description:
//    Standard data tag definitions for system communication
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Include/BepTagDefs.h $
// 
// 1     11/02/07 10:36a Derickso
// Added system tag for common module fault.
// 
// 11    1/18/07 11:24a Rwiersem
// Changes for the 07011901 core release:
// 
// - Added PLC_SYSTEM_IS_READY_TAG, LF_MOTOR_OPERATIONAL_FAULT,
// RF_MOTOR_OPERATIONAL_FAULT, LR_MOTOR_OPERATIONAL_FAULT,
// RR_MOTOR_OPERATIONAL_FAULT, LT_MOTOR_OPERATIONAL_FAULT,
// RT_MOTOR_OPERATIONAL_FAULT, LF_MOTOR_COMM_FAULT, RF_MOTOR_COMM_FAULT,
// LR_MOTOR_COMM_FAULT,  RR_MOTOR_COMM_FAULT, LT_MOTOR_COMM_FAULT,
// RT_MOTOR_COMM_FAULT, LF_SPEED_MISMATCH_FAULT, RF_SPEED_MISMATCH_FAULT,
// LR_SPEED_MISMATCH_FAULT, RR_SPEED_MISMATCH_FAULT,
// LT_SPEED_MISMATCH_FAULT,  and RT_SPEED_MISMATCH_FAULT.
// 
// 10    2/15/06 7:43p Cward
// Added wheelbase move tag
//
// 9     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 8     4/29/05 8:00a Gpattison
// Added two tag defines used by the InputServer
//
// 8     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 7     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 6     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 5     7/07/04 6:38p Cward
// Added system shutdown and system restart tags
//
// 4     6/24/04 12:52a Cward
// Added needed system tags.
//
// 3     5/12/04 9:53p Cward
// Changes for the May 12 iteration
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:27p Cward
//
// 2     10/29/03 1:27p Khykin
// Added StartSequencePushbutton tag to the definitions.
//
// 5     10/24/03 11:15a Khykin
// Updated with new tags.
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 2     8/06/03 3:20p Khykin
// Updated with latest changes from Flatrock.
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 3     6/24/03 7:44a Khykin
// Added more data tags.
//
// 2     6/20/03 8:48a Khykin
// Latest changes.
//
// 1     6/12/03 4:28p Khykin
// Initial checkin.
//
// 2     4/30/03 10:21a Khykin
// Adding tag definitions
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 3     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:41  tbochene
//
//     PooBag
//
//
// 2     2/08/03 9:47a Khykin
// Added Shutdown tag.
//
// 1     1/27/03 7:03a Khykin
// Initial Checkin
//
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
//
// 20    11/20/02 6:58a Khykin
// Adding the brake force value tag.
//
// 19    10/22/02 3:51p Derickso
// added next vin and next vehicle build tags.
//
// 18    10/16/02 10:29a Khykin
// Removed references to Calibrate in the tag definitions (also
// synchronized with the system).
//
// 17    10/16/02 9:54a Khykin
// Updated the tag defs to include tags for LossCompensation.
//
// 16    9/24/02 7:45a Khykin
// Adding tag to enable all roll drives.
//
// 15    8/23/02 8:10a Dan.erickson
// Added macros for InputServer states, DefaultDriver number information
// and DTC reporting.
//
// 14    7/31/02 11:27a Skypig13
// Latest from pit machine
//
// 13    7/29/02 8:31a Dan.erickson
// Added tags for idling and resuming active vehicle data threads.
//
// 12    6/13/02 2:49p Dan.erickson
// Added DISPLAY* macros.
//
// 11    5/24/02 2:50p Kh
// Adding Tags.
//
// 10    4/25/02 4:39p Skypig13
// Updated with new changes
//     Revision 1.1.1.1  2002/04/08 19:49:02  erickson
//     started
//
//
// 8     2/19/02 7:51p Bmeinke
// Changed VIN_DATA_TAG from Vin to VIN
// Changed START_TEST_REQ_DATA_TAG from StartTestRequest to
// StartTestFromPlc
// Changed ROLLS_DOWN_DATA_TAG from RollsDown to RollsDownElevUp
// Changed CAL_SWITCH_DATA_TAG from LossCompensationSwitchOnStatus to
// LossCompensationSwitchStatus
// Changed CAL_CYL_DATA_TAG from LossCompensationSwitchCycleStstus to
// LossCompensationSwitchCycleStatus
// Changed SIM_SWITCH_DATA_TAG from SimSwitch to SimulatorSwitchStatus
// Changes SIM_SYL_DATA_TAG from SimCycle to SimulatorSwitchCycleStatus
// Changed ZEROSPEED_TAG from ZeroSpeed to Zerospeed
// Added ZEROSPEED_TAG_TO_PLC definition
//
// 7     2/12/02 9:43p Bmeinke
// Changed "Cal" in tag definition value to "LossCompensation"
//
// 6     1/18/02 1:58p Brian.meinke
// Added tag definitions for prompt boxes 1 and 2
//
// 5     1/08/02 3:46p Dan.erickson
// Added many tags.
//
// 4     10/29/01 5:34p Brian.meinke
// Added tags for ZeroSpeed and xxRollerDirection
//
// 3     9/13/01 10:56a Brian.meinke
// Added RETEST_DATA_TAG and CONTINUE_TEST_DATA_TAG data tags
//
// 2     9/12/01 11:48p Brian.meinke
// Added ABORT_DATA_TAG
//
// 1     9/12/01 11:08p Brian.meinke
// Standard data tag definitions for system communications
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BEP_TAG_DEFS_H_INCLUDED
#define BEP_TAG_DEFS_H_INCLUDED
//==============================================================================

/**
 * Tag to be published when ABORT status changes
 */
#define ABORT_DATA_TAG                 "Abort"

/**
 * Tag to be published when test start event occurs
 */
#define START_TEST_DATA_TAG            "StartTest"

/**
 * Tag to be published when test start event occurs
 */
#define START_VEHICLE_TEST_DATA_TAG    "StartVehicleTest"

/**
 * Tag to be published when the system is ready
 * to load test component data
 */
#define LOAD_VEHICLE_TEST_DATA_TAG     "LoadVehicleTestData"

/**
 * Tag to be published when the system is ready
 * to remove test component data
 */
#define REMOVE_VEHICLE_TEST_DATA_TAG "RemoveVehicleTestData"

/**
 * Tag to be published when test start event occurs
 */
#define READ_LATEST_BUILD_DATA_TAG     "ReadLatestBuildData"

/**
 * Tag to be commanded when the launcher should command
 * the servers to reload their configurations.
 */
#define SYSTEM_RELOAD_DATA_TAG         "SystemConfigurationReload"

/**
 * Tag to be published when retest event occurs
 */
#define RETEST_DATA_TAG                "Retest"

/**
 * Tag to be published when continue event occurs
 */
#define CONTINUE_TEST_DATA_TAG         "ContinueTest"

/**
 * Tag to be published when loss compenation start event occurs
 */
#define START_LOSSCOMPENSATION_DATA_TAG "StartLossCompensation"

/**
 * Tag to be published when sim start event occurs
 */
#define START_SIM_DATA_TAG             "StartSim"

/**
 * Data tag for publishing changes in VIN
 */
#define VIN_DATA_TAG                   "VIN"

/**
 * Data tag for publishing changes in VIN
 */
#define NEXT_VIN_DATA_TAG              "NextVIN"

/**
 * Data tag for publishing changes in VIN Display
 */
#define VINDISPLAY_DATA_TAG            "VINDisplay"

/**
 * Data tag for publishing changes in VIN
 */
#define VIN_READ_STATUS_TAG             "VINReadStatus"

/**
 * Data tag for Green stack light
 */
#define STACK_LIGHT_GREEN_TAG           "StackLightGreen"

/**
 * Data tag for Yellow stack light
 */
#define STACK_LIGHT_YELLOW_TAG          "StackLightYellow"

/**
 * Data tag for Red stack light
 */
#define STACK_LIGHT_RED_TAG             "StackLightRed"

/**
 * Data tag for Blue stack light
 */
#define STACK_LIGHT_BLUE_TAG            "StackLightBlue"

/**
 * Data tag for publishing request to start vehicle test
 */
#define START_TEST_REQ_DATA_TAG        "StartTestFromPLC"

/**
 * Data tag for publishing request to start vehicle test
 */
#define START_SEQUENCE_PB_DATA_TAG     "StartSequencePushbutton"

/**
 * Data tag for publishing end of vehicle test
 */
#define TEST_COMPLETE_DATA_TAG         "TestComplete"

/**
 * Data tag for publishing changes in vehicle present state
 */
#define VEH_PRESENT_DATA_TAG           "VehiclePresent"

/**
 * Data tag for publishing changes for the driver number
 */
#define DRIVER_NUMBER_DATA_TAG          "DriverNumber"

/**
 * Default driver number tag for the default driver number.
 */
#define DEFAULT_DRIVER_NUMBER_TAG       "DefaultDriverNumber"

/**
 * Data tag for publishing changes for the driver number display value
 */
#define DRIVER_NUMBER_DISPLAY_DATA_TAG  "DriverNumberDisplay"

/**
 * Data tag for publishing changes for the driver number display background color
 */
#define DRIVER_NUMBER_DISPLAY_BGCOLOR_DATA_TAG  "DriverNumberDisplayBGColor"

/**
 * The brake force tag that will allow the user to update
 * the value on the Brake Radial gauge on the GUI.
 */
#define BRAKE_FORCE_VALUE                       "BrakeForceValue"

/**
 * Data tag for raising the rollers/lowering the elevators
 * automatically.
 */
#define RAISE_ROLLS                     "RaiseRolls-LowerElevators"

/**
 * Data tag for lowering the rollers/raising the elevators
 * automatically.
 */
#define LOWER_ROLLS                     "LowerRolls-RaiseElevators"
          
/**
 * Data tag that indicates the status of the vehicle presence sensor
 */
#define INST_VEH_PRESENT_DATA_TAG       "InstVehiclePresent"

/**
 * Data tag for publishing changes in retaining roll state
 */
#define ROLLS_DOWN_DATA_TAG            "RollsDownElevUp"

/**
 * Data tag for publishing changes in retaining roll state
 */
#define ROLLS_UP_DATA_TAG              "RollsUpElevDown"

/**
 * Data tag used to say when the Vehicle Test server is performing a vehicle test
 */
#define TEST_IN_PROG_DATA_TAG           "TestInProgress"

/**
 * Data tag for the status of the commanded phase.
 */
#define PHASE_STATUS                    "PhaseStatus"

/**
 * Data tag used to command a phase.
 */
#define COMMAND_PHASE                   "CommandPhase"

/**
 * Data tag used to say when a vehicle test has completed the foreground testing
 */
#define FOREGROUNDTEST_IN_PROG_DATA_TAG "ForegroundTestInProgress"

/**
 * Data tag for publishing changes in simulate switch state
 */
#define SIM_SWITCH_DATA_TAG            "SimulatorSwitchStatus"

/**
 * Data tag for publishing changes in simulate cycle switch state
 */
#define SIM_CYL_DATA_TAG               "SimulatorSwitchCycleStatus"

/**
 * Data tag for publishing changes in cable connect state
 */
#define CBL_CONNECT_DATA_TAG           "CableConnect"

/**
 * Data tag for telling the PLC that the vehicle cable is connected
 */
#define PLC_CBL_CONNECT_DATA_TAG        "ABSCableConnected"

/**
 * Data tag for telling the PLC that the VME is ready
 */
#define VME_SYSTEM_READY_TAG            "VMESystemReady"

/**
 * Data tag for 6 pin cable connect state
 */
#define CBL_CONNECT_6_PIN               "CableConnect6Pin"

/**
 * Data tag for 8 pin cable connect state
 */
#define CBL_CONNECT_8_PIN               "CableConnect8Pin"

/**
 * Data tag for 16 pin cable connect state
 */
#define CBL_CONNECT_16_PIN              "CableConnect16Pin"

/**
 * Data tag for 16 pin cable connect state
 */
#define PLC_CBL_CONNECT                 "PLCCableConnect"

/**
 * Data tag for zerospeed
 */
#define ZEROSPEED_TAG                   "Zerospeed"

/**
 * Data tag for zerospeed as used by the PLC
 */
#define ZEROSPEED_TAG_TO_PLC            "ZerospeedToPLC"

/**
 * Data tag for LF Direction
 */
#define LF_ROLLER_DIR_TAG              "LeftFrontRollerDirection"

/**
 * Data tag for RF Direction
 */
#define RF_ROLLER_DIR_TAG              "RightFrontRollerDirection"

/**
 * Data tag for LR Direction
 */
#define LR_ROLLER_DIR_TAG              "LeftRearRollerDirection"

/**
 * Data tag for RR Direction
 */
#define RR_ROLLER_DIR_TAG              "RightRearRollerDirection"

/**
 * Data tag for LT Direction
 */
#define LT_ROLLER_DIR_TAG              "LeftTandemRollerDirection"

/**
 * Data tag for RT Direction
 */
#define RT_ROLLER_DIR_TAG              "RightTandemRollerDirection"

/**
 * Tag to use for vehicle build
 */
#define VEHICLE_BUILD_TAG              "VehicleBuild"

/**
 * Tag to use for the next vehicle build
 */
#define NEXT_VEHICLE_BUILD_TAG         "NextVehicleBuild"

/**
 * Tag to use to store the status of the vehicle build record
 */
#define BUILD_RECORD_STATUS_TAG        "VehicleBuildRecordStatus"

/**
 * Tag to use to store if the vehilce is at the barcode scanner
 */
#define VEHICLE_AT_BARCODE_SCANNER_TAG "VehicleAtBarcodeScanner"

/**
 * Tag to use for Body Style
 */
#define BODY_STYLE_TAG                 "BodyStyle"

/**
 * Tag to use for drive axle.
 */
#define DRIVE_AXLE_TAG                 "DriveAxle"

/**
 * Value to set drive axle to for front wheel drive.
 */
#define FRONT_WHEEL_DRIVE_VALUE        "Front"

/**
 * Value to set drive axle to for rear wheel drive.
 */
#define REAR_WHEEL_DRIVE_VALUE         "Rear"

/**
 * Tag to use to tell if the vehicle is equipped with AWD.
 */
#define ALL_WHEEL_DRIVE_TAG            "AllWheelDriveEquipped"

/**
 * Tag to use to tell if the vehicle is equipped with 4WD.
 */
#define FOUR_WHEEL_DRIVE_TAG           "FourWheelDriveEquipped"

/**
 * Data tag for type of transfer case
 */
#define TRANSFER_CASE_TAG              "TransferCase"

#define WHEELBASE_DESIGNATOR_TAG       "WheelbaseDesignator"
/**
 * Data tag for selected wheelbase position
 */
#define WHEEL_BASE_POSITION_TAG        "WheelbasePosition"

/**
 * Data tag for wheelbase in position
 */
#define WHEELBASE_IN_POSITION_TAG           "WheelbaseInPosition"

/**
 * Data tag for wheelbase in position
 */
#define WHEELBASE_IN_POSITION_STATUS_TAG    "WheelbaseInPositionStatus"

/**
 * Data tag for wheelbase in position
 */
#define FRONT_CRADLE_IN_POSITION_TAG           "FrontCradleInPosition"

/**
 * Data tag for wheelbase in position
 */
#define REAR_CRADLE_IN_POSITION_TAG           "RearCradleInPosition"


/**
 * Data tag to use to command the PLC to start/stop moving the wheelbase
 */
#define WHEELBASE_MOVE_TAG                  "WheelbaseStartCommand"
/**
 * Data tag to use to command the PLC to start/stop moving the wheelbase
 */
#define FRONT_CRADLE_MOVE_TAG                  "AdjustFrontCradlePosition"
/**
 * Data tag to use to command the PLC to start/stop moving the wheelbase
 */
#define REAR_CRADLE_MOVE_TAG                  "AdjustRearCradlePosition"
/**
 * Data tag to use to command the PLC to start/stop moving the wheelbase
 */
#define RERELAX_RETROLLS_TAG                  "ReRelaxRetRolls"
/**
 * Data tag for Vehicle Build Record
 */
#define BUILD_RECORD_TAG               "BuildRecord"

/**
 * Data tag for the PFS result string
 */
#define PFS_RESULT_TAG                 "PfsResult"

/**
 * Data tag for RWD selected
 */
#define REAR_WHEEL_DRIVE_SELECTED_TAG  "RWDSelected"

/**
 * Data tag for FWD selected
 */
#define FRONT_WHEEL_DRIVE_SELECTED_TAG "FrontWheelDriveSelected"

/**
 * Data tag for 4WD selected
 */
#define FOUR_WHEEL_DRIVE_SELECTED_TAG  "FourWDSelected"

/**
 * Data tag for Manual transmission selected
 */
#define MANUAL_TRANSMISSION_TAG        "ManualTransmission"

/**
 * Data tag for automatic transmission selected
 */
#define AUTOMATIC_TRANSMISSION_TAG     "AutomaticTransmission"

/**
 * Data tag for the transmission type
 */
#define TRANSMISSION_TYPE_TAG          "Transmission"

/**
 * Data tag for the first prompt box (where operator prompts are displayed)
 */
#define PROMPT_BOX_1                    "PromptBox1"

/**
 * Data tag for the first prompt box background color (where operator prompts are displayed)
 */
#define PROMPT_BOX_1_BGCOLOR            "PromptBox1BGColor"

/**
 * Data tag for the 2nd prompt box (where operator prompts are displayed)
 */
#define PROMPT_BOX_2                    "PromptBox2"

/**
 * Data tag for the first prompt box background color (where operator prompts are displayed)
 */
#define PROMPT_BOX_2_BGCOLOR            "PromptBox2BGColor"

/**
 * Tag name used by the system to reference
 * PromptServer published tag for Prompt Box background color
 */
#define PROMPT_DEFAULT_BGCOLOR_TAG   "PromptDefaultBGColor"
/**
 * Data tag used to switch the Rack monitor screen that is displayed
 */
#define DISPLAY_RACK_MONITOR_SCREEN     "DisplayRackMonitor"

/**
 * Data tag used to switch the Driver monitor screen that is displayed
 */
#define DISPLAY_DRIVER_MONITOR_SCREEN   "DisplayDriverMonitor"

/**
 * Data tag used for writing the LC test results.
 */
#define LOSS_COMPENSATION_VIN           "Loss_Compensation"

/**
 * Data tag for publishing changes in loss compensation switch cycle state
 */
#define LOSSCOMPENSATION_CYCLE_DATA_TAG "LossCompensationSwitchCycleStatus"

/**
 * Data tag used to set system test status to Loss Compensation mode
 */
#define LOSSCOMPENSATION_TAG            "LossCompensation"
#define LOSSCOMPENSATION_TO_PLC_TAG     "LossCompensationBitToPlc"

/**
 * Data tag used to signal the PLC that we are doing a Loss Compensation
 */
#define LC_SWITCH_STATUS_TAG            "LossCompensationSwitchStatus"

/**
 * Data tag used to signal the PLC that we are doing a Loss Compensation
 */
#define LC_SWITCH_STATUS_FROM_PLC_TAG           "LossCompensationSwitchStatusFromPLC"

/**
 * Data tag for publishing changes in loss compensation switch cycle state
 */
#define LOSSCOMPENSATION_CYCLE_FROM_PLC_TAG     "LossCompensationSwitchCycleStatusFromPLC"

/**
 * Data tag used to signal the PLC that we are doing a Loss Compensation
 */
#define LC_SWITCHTOPLC_STATUS_TAG       "LossCompensationSwitchStatusToPLC"

/**
 * Number of tests performed since the last loss compensation cycle.
 */
#define TESTS_SINCE_LOSS_COMP_TAG       "TestsSinceLossCompensation"

/**
 * Data tag used to determine the type of machine that
 * the software is running on.
 */
#define MACHINE_TYPE_2400               "MachineType2400"

/**
 * Data tag used to determine the type of machine that
 * the software is running on.
 */
#define MACHINE_TYPE                    "MachineType"

/**
 * Data tag used to signal the PLC to Clutch the system.
 */
#define MACHINE_CLUTCH                  "Clutch"

/**
 * Data tag used to signal the PLC to Declutch the system.
 */
#define MACHINE_DECLUTCH                "Declutch"

/**
 * The machine loss time of the last LossCompensation that
 * was performed.
 */
#define MACHINE_LOSS_TIME               "MachineLossTime"

/**
 * The machine loss date of the last LossCompensation that
 * was performed.
 */
#define MACHINE_LOSS_DATE               "MachineLossDate"

/**
 * The state of the input server tag.
 */
#define INPUT_SERVER_STATE              "InputServerState"

/**
 * The state of the input server tag.
 */
#define INPUT_SERVER_NORMAL_MODE        "Normal"

/**
 * The state of the input server tag.
 */
#define INPUT_SERVER_TESTRESULT_STATE   "TestResult"

/**
 * The state of the input server tag.
 */
#define INPUT_SERVER_VIN_STATE          "Vin"

/**
 * The tag to read the key that was pressed on the pendant.
 */
#define PENDANT_KEY_PRESS               "KeyPress"

/**
 * The tag to read the key that was pressed on the pendant.
 */
#define PENDANT_CLEAR_KEY_PRESS         "ClearKeyPress"

/**
 * The state of the input server tag.
 */
#define PLC_POWER_SENSE                 "PowerSense"

/**
 * The state of the input server tag.
 */
#define PLC_RACK_FAULT                  "PLCRackFault"

/**
 * The tag to read the number of rollers that the machine has.
 */
#define ROLLER_COUNT                    "RollerCount"

/**
 * The tag to read to get the name and location of the
 * Vehicle TestResult.
 */
#define TEST_RESULT_TAG                 "TestResult"

/**
 * The tag to read to get the name and location of the
 * Vehicle TestResult.
 */
#define TEST_RESULT_TO_PRINT_TAG        "TestResultToPrint"

/**
 * The tag to be published when the Pendant Reprint function
 * is pressed
 */
#define REPRINT_RESULTS                 "ReprintResults"

/**
 * The tag to read to get the software identifier
 */
#define SOFTWARE_ID                     "SoftwareID"


/**
 * Tag to be published when non licensed software
 * is allowed to run. This will trigger a splash screen
 * to notify the users that the software will work for
 * two hours and then require a reboot.
 */
#define INVALID_LICENSE_NOTIFICATION_TAG    "InvalidLicenseNotification"

/**
 * Tag to be published when the two hour time limit
 * has expired while running without a valid license.
 * The system will not run and will require a reboot.
 */
#define INVALID_LICENSE_FATAL_FAULT_TAG     "InvalidLicenseFatalFault"

/**
 * Tag used to notify a process that the ignition
 * key needs to be cycled before the next test is run
 */
#define KEY_CYCLE_NEEDED_TAG            "KeyCycleNeeded"

/**
 * Tag to use for reporting Diagnostic Trouble Codes(DTC).
 * @since Version 1.5
 */
#define DTC_TAG                         "DTC"
#define SET_DTC_TAG                     "SetDTC"
#define CLEAR_DTC_TAG                   "ClearDTC"
#define DTC_CODE                        "DTCCode"
#define COMMON_MODULE_FAULT_TAG         "CommonModuleFault"

/**
 * The tag to read to get the name and location of the
 * Vehicle TestResult to FTP from the system.
 */
#define TEST_RESULT_FTP_TAG             "FTPTestResult"
/**
 * Idle and resume tags for the ACtiveVehicleData threads and VehicleDataBroker.
 */
#define IDLE_ACTIVE_VEHICLE_DATA        "IdleActiveVehicleData"
#define RESUME_ACTIVE_VEHICLE_DATA      "ResumeActiveVehicleData"
#define IDLE_ALL_ACTIVE_DATA_THREADS    "IdleAllActiveDataThreads"
#define RESUME_ALL_ACTIVE_DATA_THREADS  "ResumeAllActiveDataThreads"

/**
 * The default driver number that is used in the system.
 */
#define DEFAULT_DRIVER_NUMBER           "T0000xx"

/**
 * Tag used to enable all roll drives
 */
#define ENABLE_ALL_ROLL_DRIVES          "EnableAllRollDrives"

// current machine losses data tags
#define LF_MACHINE_LOSS_CURRENT_TAG     "LFMachineLossCurrent"
#define RF_MACHINE_LOSS_CURRENT_TAG     "RFMachineLossCurrent"
#define LR_MACHINE_LOSS_CURRENT_TAG     "LRMachineLossCurrent"
#define RR_MACHINE_LOSS_CURRENT_TAG     "RRMachineLossCurrent"

// previous machine losses tags
#define LF_MACHINE_LOSS_PREVIOUS_TAG    "LFMachineLossPrevious"
#define RF_MACHINE_LOSS_PREVIOUS_TAG    "RFMachineLossPrevious"
#define LR_MACHINE_LOSS_PREVIOUS_TAG    "LRMachineLossPrevious"
#define RR_MACHINE_LOSS_PREVIOUS_TAG    "RRMachineLossPrevious"

/**
 * Tag to be published when OkToRoll status changes
 */
#define OKTOROLL_TAG                   "OkToRoll"

/**
 * Tag to be published when a stop current test is issues by FACTS
 */
#define STOP_CURRENT_TEST_TAG          "StopCurrentTest"

/**
 * Tag to be published when commanding a test phase
 */
#define COMMAND_PHASE_TAG              "CommandPhase"

/**
 * Tag to be published when commanding a test phase
 */
#define COMMAND_DCL_PHASE_TAG          "DCLPhase"

/**
 * Tag to be published to indicate the status of the phase that was executed last
 */
#define PHASE_STATUS_TAG               "PhaseStatus"

/**
 * Data tag for wheelbase selected index.
 */
#define WHEELBASE_TAG                  "Wheelbase"

/**
 * Data tag for wheel base position in inchesX10
 */
#define WHEELBASE_INCHESX10            "WheelbasePositionInchesX10"

/**
 * Complete Lamp tag used to manipulate the PLC complete lamp.
 */
#define COMPLETE_LAMP                  "CompleteLamp"

/**
 * Tag to be commanded when the launcher should terminate
 * the system.
 */
#define SYSTEM_SHUTDOWN_DATA_TAG        "Shutdown"

/**
 * Tag to be commanded when the launcher should terminate
 * CCRT, Save Logs and shutdown the system.
 */

#define SYSTEM_SHUTDOWN_SAVELOGS_TAG    "ShutdownSaveLogs"

/**
 * Tag to be commanded when the launcher should terminate
 * CCRT and then shutdown and restart the system.
 */
#define SYSTEM_RESTART_DATA_TAG         "RestartCCRT"
/**
 * Tag to be commanded when the Client is shutdown
 */
#define CLIENT_CLOSED_DATA_TAG         "ClientClosing"

/**
 * Tag to be commanded when the launcher should terminate
 * CCRT,save logs, and then shutdown and restart the system.
 */
#define SYSTEM_RESTART_SAVELOGS_TAG     "RestartCCRTSaveLogs"

/**
 * Tag to be published to indicate the status of the regen drive motor startup timer
 */
#define PLC_REGEN_PWR_TIMER_TAG        "RegenDrivePowerTimerActive"

/**
 * Tag to be published to indicate the status of PLCSystemIsReady bit
 */
#define PLC_SYSTEM_IS_READY_TAG               "PLCSystemIsReady"

/**
 * Tag to be published to indicate if the LF drive is faulted
 */
#define LF_MOTOR_OPERATIONAL_FAULT               "LFRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the RF drive is faulted
 */
#define RF_MOTOR_OPERATIONAL_FAULT               "RFRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the LR drive is faulted
 */
#define LR_MOTOR_OPERATIONAL_FAULT               "LRRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the RR drive is faulted
 */
#define RR_MOTOR_OPERATIONAL_FAULT               "RRRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the LT drive is faulted
 */
#define LT_MOTOR_OPERATIONAL_FAULT               "LTRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the RT drive is faulted
 */
#define RT_MOTOR_OPERATIONAL_FAULT               "RTRollMotorOperationalFault"

/**
 * Tag to be published to indicate if the LF drive has a comm fault
 */
#define LF_MOTOR_COMM_FAULT               "LFRollMotorDriveCommFault"

/**
 * Tag to be published to indicate if the RF drive has a comm fault
 */
#define RF_MOTOR_COMM_FAULT               "RFRollMotorDriveCommFault"

/**
 * Tag to be published to indicate if the LR drive has a comm fault
 */
#define LR_MOTOR_COMM_FAULT               "LRRollMotorDriveCommFault"

/**
 * Tag to be published to indicate if the RR drive has a comm fault
 */
#define RR_MOTOR_COMM_FAULT               "RRRollMotorDriveCommFault"
/**
 * Tag to be published to indicate if the LT drive has a comm fault
 */
#define LT_MOTOR_COMM_FAULT               "LTRollMotorDriveCommFault"

/**
 * Tag to be published to indicate if the RT drive has a comm fault
 */
#define RT_MOTOR_COMM_FAULT               "RTRollMotorDriveCommFault"

/**
 * Tag to be published to indicate if the LF roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define LF_SPEED_MISMATCH_FAULT               "LeftFrontRollSpeedMismatchFault"
/**
 * Tag to be published to indicate if the RF roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define RF_SPEED_MISMATCH_FAULT               "RightFrontRollSpeedMismatchFault"
/**
 * Tag to be published to indicate if the LR roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define LR_SPEED_MISMATCH_FAULT               "LeftRearRollSpeedMismatchFault"
/**
 * Tag to be published to indicate if the RR roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define RR_SPEED_MISMATCH_FAULT               "RightRearRollSpeedMismatchFault"
/**
 * Tag to be published to indicate if the LT roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define LT_SPEED_MISMATCH_FAULT               "LeftTandemRollSpeedMismatchFault"
/**
 * Tag to be published to indicate if the RT roll speed is out of tolerance 
 * with the average of the front rollers
 */
#define RT_SPEED_MISMATCH_FAULT               "RightTandemRollSpeedMismatchFault"

#endif  // BEP_TAG_DEFS_H_INCLUDED
