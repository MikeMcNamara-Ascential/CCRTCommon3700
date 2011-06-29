//******************************************************************************
//
// CLASS:
//		Logging
//
// FILE:
//     $Header: /Core/ServicesLayer/Include/LogDefs.h 2     1/16/04 4:45p Cward $
//
// Description:
//     Logger definitions
//
//==============================================================================
//
// FUNCTIONAL DESCRIPTION:
//
//==============================================================================
//
// OPERATIONAL DESCRIPTION:
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
//     $Log: /Core/ServicesLayer/Include/LogDefs.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 2     8/06/03 3:59p Khykin
// Updated with latest changes from Flatrock.
//     Revision 1.4  2003/07/17 22:16:18  crosby
//     Added LOGGER_BUFFER_SIZE
//
//     Revision 1.3  2003/05/09 21:06:53  swieton
//     Moved ServicesLibs/Include -> Include
//
//     Revision 1.1.1.1  2003/05/09 20:24:00  swieton
//     Moving ServicesLayer/* ServicesLayer/ServicesLib/*
//
//     Revision 1.1.1.1  2003/04/30 20:09:03  tenharmsel
//     Source code for CCRT Server
//
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:42  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:07a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 7     10/10/02 10:24a Khykin
// Added new definition for logging verbosity.
//
// 6     8/23/02 11:46a Kh
// Adding new logger definition.
//
// 5     12/12/01 12:20p Brian.meinke
// Forgot comma after LOG_SER_COM enum value
//
// 4     12/12/01 12:18p Brian.meinke
// Added LOG_SER_COM to LogMask enum to log serial communication
//
// 3     4/25/01 4:40p Brian.meinke
// Added LogMask enum
//
// 2     4/19/01 1:54p Brian.meinke
// Changed SPECIAL_FILE_NAME_LEN from 20 to 32
//
// 1     4/19/01 1:46p Brian.meinke
// Common logger definitions
//
//******************************************************************************

#ifndef 	_LOG_DEFS_H_INCLUDED
#define		_LOG_DEFS_H_INCLUDED


// ***********************************************************************
// ************************** system configuration ***********************
// ***********************************************************************

// ***********************************************************************
// ************************** system includes ****************************
// ***********************************************************************

// ***********************************************************************
// ************************** application includes ***********************
// ***********************************************************************

// Include definitions
//#include	"BepDefs.h"
//#include	"Message.h"					// communication
//#include	"Bepos.h"					// need to talk through this

// ***********************************************************************
// ************************** private attributes *************************
// ***********************************************************************


// Log file string deliminater bit
#define		DELIMITER_ID					0x80

// message identification
#define		LOGGING_BASE					(COMMAND_BASE + 0x00010000)
#define		ROLL_LOG_STRING					(LOGGING_BASE + 0x0000)
#define		ROLL_LOAD_CONFIGURATION			(LOGGING_BASE + 0x0001)
#define		ROLL_NEW_TEST_NUMBER			(LOGGING_BASE + 0x0002)
#define		ROLL_ACTIVATE_SPECIAL_LOG		(LOGGING_BASE + 0x0003)
#define		ROLL_DEACTIVATE_SPECIAL_LOG		(LOGGING_BASE + 0x0004)
#define		ROLL_DEACTIVATE_ALL_SPECIAL_LOG	(LOGGING_BASE + 0x0005)
#define		ROLL_LOG_READ_NEXT				(LOGGING_BASE + 0x0006)
#define		ROLL_LOG_READ_CLOSE				(LOGGING_BASE + 0x0007)
#define		ROLL_DITTO_PROCESS				(LOGGING_BASE + 0x0008)

// Size of IInterProcessCommunication buffers for use by LogIfc and LogDaemon
#define LOGGER_BUFFER_SIZE 4096

// Logging IPC message transfer sizes
#define		MSG_SIZE					1024
#define		WHO_SIZE					20
#define		FILE_LEN					64
#define		SPECIAL_FILE_NAME_LEN		32
#define		TOTAL_SIZE					(4+1+WHO_SIZE+1+MSG_SIZE+1+FILE_LEN+2)


/**
 * Mask bits to control what data is logged by the resource managers
 */
typedef enum _LogMask
{
	LOG_NONE = 0x00000000,
	LOG_ERRORS = 0x00000001,
	LOG_FN_ENTRY = 0x00000002,
	LOG_IO_MSSG = 0x00000004,
	LOG_DEV_DATA = 0x00000008,
	LOG_SER_COM = 0x00000010,
	LOG_DETAILED_DATA = 0x00000020,
	LOG_CLASSCOMPONENT_DATA = 0x00000040,
	LOG_ALL = 0xFFFFFFFF
} LogMask;

// ************************************************************************

#endif	// _LOG_DEFS_H_INCLUDED
// END

