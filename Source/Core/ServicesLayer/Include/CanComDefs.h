//******************************************************************************
// Description:
//	CAN communication Definitions
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/CanComDefs.h $
// 
// 3     7/20/05 8:46p Cward
// Added support for new short form transmit messages
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 2     6/26/03 1:28p Khykin
// Updated with Brians latest changes.
//
// 2     6/24/03 10:00a Bmeinke
// Changed the ModuleId_t typedef from a uint8_t to a unit16_t
// Added INVALID_CAN_ID define
//
// 1     6/03/03 1:22p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _CAN_COM_DEFS_H_INCLUDED
#define _CAN_COM_DEFS_H_INCLUDED

#include <inttypes.h>
#include "LogPortMessage.h"
#include "AVTCommDefs.h"

/**
 * Data type used to represent a CAN module ID
 */
typedef uint16_t				ModuleId_t;

/**
 * Value used to represent an invalid CAN module/node ID
 */
#define INVALID_CAN_ID			0xFFFF

/**
 * First byte of a CAN diagnostic message reply
 */
#define CANDIAG_RESP_HDR_BYTE	0x06

/**
 * Minimum number of bytes in a CAN diagnostic reply message
 */
#define CAN_DIAG_REPLY_MIN_SZ	3

#endif // _CAN_COM_DEFS_H_INCLUDED

