//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/J2190ComDefs.h $
// $Author: Cward $
//   $Date: 12/21/03 6:26p $
//
// Description:
//	J2190 communication Definitions
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
//     $Log: /Core/ServicesLayer/Include/J2190ComDefs.h $
// 
// 1     12/21/03 6:26p Cward
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:16p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:41  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:06a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
// 
// 6     11/15/02 3:01p Bmeinke
// Moved AVT definitions to new AVTCommDefs.h file
// 
// 5     9/25/02 10:56a Bmeinke
// Changed J2190_DIAG_REPLY_MIN_SZ from 5 to 4 (emissions testing)
// 
// 4     9/19/02 5:00p Bmeinke
// Changed J2190_DIAG_REPLY_MIN_SZ from 6 to 5
// 
// 3     5/28/02 2:02p Bmeinke
// Added AvtHeaderStatus_t enum type to define the possible header status
// values contained in AVT reply messages
// 
// 2     4/15/02 1:46p Bmeinke
// Added devctl enum values
// 
// 1     3/22/02 1:16p Bmeinke
// Initial releasae; taken from J2190LogPortDrv.h
// 
//******************************************************************************

#ifndef _J2190_COM_DEFS_H_INCLUDED
#define _J2190_COM_DEFS_H_INCLUDED

#include <inttypes.h>
#include "LogPortMessage.h"

/**
 * Data type used to represent a J2190 module ID
 */
typedef uint8_t					ModuleId_t;
	   
/**
 * Maximum number of bytes in a J2190 message string
 */
#define	J2190_MSSG_SIZE			16

/**
 * First byte of a J2190 diagnostic message reply
 */
#define J2190DIAG_RESP_HDR_BYTE	0x26

/**
 * Minimum number of bytes in a J2190 diagnostic reply message
 */
#define J2190_DIAG_REPLY_MIN_SZ	4

#endif // _J2190_COM_DEFS_H_INCLUDED

