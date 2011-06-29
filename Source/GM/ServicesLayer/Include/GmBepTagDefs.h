//******************************************************************************
//
// $Archive: /CMake Core/Source/Gm/ServicesLayer/Include/GmBepTagDefs.h $
// $Author: Bmeinke $
//   $Date: 4/25/05 9:27a $
//
// Description:
//    Standard data tag definitions for Gm system communication
//
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
//     $Log: /CMake Core/Source/Gm/ServicesLayer/Include/GmBepTagDefs.h $
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Gm
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 1     7/08/03 1:57p Derickso
// Restructured for new directory structure.
// 
// 2     6/24/03 7:45a Khykin
// Added more tag definitions.
// 
// 1     6/12/03 4:27p Khykin
// Initial checkin.
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "BepTagDefs.h"

#ifndef GM_BEP_TAG_DEFS_H_INCLUDED
#define GM_BEP_TAG_DEFS_H_INCLUDED

/**
 * Tag used for getting the numeric representation of the Vehicle Type
 */
#define VEHICLE_TYPE_NUMERIC			"VehicleTypeNumeric"

/**
 * Data tag for Green stack light
 */
#define STACK_LIGHT_GREEN_TAG		   	"StackLightGreen"

/**
 * Data tag for Yellow stack light
 */
#define STACK_LIGHT_YELLOW_TAG		   	"StackLightYellow"

/**
 * Data tag for Red stack light
 */
#define STACK_LIGHT_RED_TAG			   	"StackLightRed"

/**
 * Data tag for Blue stack light
 */
#define STACK_LIGHT_BLUE_TAG		   	"StackLightBlue"

/**
 * Tag to be published when commanding a test phase
 */
#define COMMAND_DVT_PHASE_TAG           "DVTPhase"


#endif	// GM_BEP_TAG_DEFS_H_INCLUDED
