//******************************************************************************
// Description:
//	Data type definitions for the input server and input devices
//==============================================================================
//
//     Copyright (c) 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/Include/InputServerDefs.h $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _INPUT_SERVER_DEFS_H_INCLUDED
#define _INPUT_SERVER_DEFS_H_INCLUDED

/**
 * Default name to register serial server with
 */
#define INPUT_SERVER_NAME		            "core/InputServer"

/**
 * XML Node tag for configuration node containing logical to physical port
 * mapping information
 */
#define CFG_INPUT_DEVICE_MAP_TAG	        "InputDeviceMap"

/**
 * XML node tag for configuration node containing logical port configuration
 */
#define CFG_INPUT_DEVICE_TAG		        "InputDevices"

/**
 * XML attribute tag use to specify the name of the input
 * device process executable to use for a input device
 */
#define CFG_INPUT_DEVICE_PROCESS_ATTR_TAG	"process"
/**
 * Prfeix of name to use when registering our process
 */
#define INPUT_DEVICE_NAME_ROOT		        "core/"
/**
 * XML attribute node tag used to specify whether or not a
 * logical port is enabled
 */
#define CFG_INPUT_DEVICE_EN_ATTR_TAG	    "enabled"

/**
 * Used to signal via a conditional variable if a vehicle
 * is present at the barcode scanner
 */
#define AT_BARCODE_SCANNER_VALUE            "1"

/**
 * Used to signal via a conditional variable if a vehicle
 * is not present at the barcode scanner
 */
#define NOT_AT_BARCODE_SCANNER_VALUE        "0"

/**
 * String that is used to indicate that a build
 * record is valid
 */
#define BUILD_RECORD_VALID                  "Valid"

/**
 * String used to indicate the received status
 * for a conditional variable
 */
#define BUILD_RECORD_STATUS_RECEIVED        "True"

/**
 * String used to indicate the not received status
 * for a conditional variable
 */
#define BUILD_RECORD_STATUS_NOT_RECEIVED    "False"

#endif	// _INPUT_SERVER_DEFS_H_INCLUDED

