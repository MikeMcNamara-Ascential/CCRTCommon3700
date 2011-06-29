//*************************************************************************
// Description:
//   Defines used with the motor controller
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Include/MotorControllerDefs.h $
// 
// 1     11/02/07 12:13p Derickso
// Added torque constant tag for motor loading.
// 
// 4     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 3     4/29/05 8:01a Gpattison
// Added new define RESET_MAX_BOOST_PRESSURE to allow an external
// process to reset the Max boost pressure to its original value
// at anytime
//
// 3     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     8/30/04 10:42a Bmeinke
// Added defines for the Motor loading constants
//
// 2     6/10/04 11:32a Bmeinke
// Added COMMAND_MOTOR_LOADING #define to allow external processes to
// enable/disable motor loading
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     8/12/03 2:50p Khykin
// Updated with new directory update changes.
//
// 1     8/07/03 8:01a Khykin
// Updated with changes from Flatrock.
//
// 1     6/26/03 9:18a Tbochene
// Defines for using and controlling the Motor Controller
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#ifndef MotorControllerDefs_h
#define MotorControllerDefs_h

#define MOTOR_CONTROLLER_NAME "core/MotorController"

// Standard Motor Modes
#define IDLE_MODE					"Idle"
#define BOOST_MODE 					"Boost"
#define SPEED_MODE					"Speed"
#define TORQUE_MODE					"Torque"
#define NO_LIMIT_TORQUE_MODE        "NoLimitTorque"
#define MOTOR_MODE 					"MotorMode"
#define QUICK_BRAKE_MODE			"QuickBrake"
#define REAR_BRAKE_ASSIST_MODE		"RearBrakeAssist"
#define LOSSCOMPENSATION_MODE		"LossCompensation"
#define INDIVIDUAL_CONTROL_MODE		"IndividualControl"
#define UPLOAD_PARAMETERS_MODE		"UploadDriveParameters"
#define MOTOR_LOADING_ACCEL_K		"MotorLoadingAccelConst"
#define MOTOR_LOADING_SPEED_K		"MotorLoadingSpeedConst"
#define MOTOR_LOADING_TORQUE_K		"MotorLoadingTorqueConst"

// Motor commands
#define COMMAND_SPEED				"CommandSpeed"		// Same speed to all
#define COMMAND_TORQUE				"CommandTorque"		// Same torque to all
#define COMMAND_CLUTCH				"CommandClutch"		// Command machine to cluthc
#define COMMAND_MOTOR_DELAY			"CommandMotorDelay"	// Command delay between motors executing commands (e.g. stagger-start)
#define COMMAND_MOTOR_LOADING		"CommandMotorLoading"	// Command motor loading on/off ("1"/"0")
#define MAX_BOOST_PRESSURE_VALUE    "MaxBoostPressureValue" // Command the maximum boost pressure allowed (float)
#define RESET_MAX_BOOST_PRESSURE    "ResetMaxBoostPressure" // Command the maximum boost pressure to the absolute allowed by the config file

// PLC Motor Control Tags
const string PlcMotorModeTag("MotorMode");
const string PlcSpeedRefTag("SpeedRefRpm");
const string PlcTorqueRefTag("TorqueRefPercent");
const string PlcPosTorqueLimitTag("PosTorqueLimit");
const string PlcNegTorqueLimitTag("NegTorqueLimit");

const string PlcMotorModeFreeRoll("0");
const string PlcMotorModeSpeed("1");
const string PlcMotorModeTorque("2");

#endif // MotorControllerDefs_h

