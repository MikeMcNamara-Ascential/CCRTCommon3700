//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/LogDcmd.h $
// $Author: Cward $
//   $Date: 12/21/03 6:27p $
//
// Description:
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
//     $Log: /Core/ServicesLayer/Include/LogDcmd.h $
// 
// 1     12/21/03 6:27p Cward
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
// 1     6/12/03 4:16p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:41  tbochene
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
// 1     10/31/02 11:07a Bmeinke
// devctl() command ID's and messages for multi-threaded logger resource
// manager
// 
//
//******************************************************************************

#ifndef _LOG_DCMD_H_
#define _LOG_DCMD_H_

#include <devctl.h>

/**
 * devtl command ID's for the logger
 */
typedef enum
{
	/**
	 * Start a new test log (test number has changed)
	 */
	LOG_NEW_TEST_NUMBER		= 0,
	
	/**
	 * Opens (starts) a new log file for a client
	 */
	LOG_OPEN_SPECIAL_LOG,
	
	/**
	 * Close a client specific log file
	 */
	LOG_CLOSE_SEPCIAL_LOG,
	
	/**
	 * Echo received data to another process
	 */
	LOG_DITTO_PROCESS,
} LoggerDcmd_t;


/**
 * Logger devctl message data for test numebr change. Used to start
 * a log file for a new test 
 */
typedef struct _logNewTestNo
{
	/**
	 * Test number for new test
	 */
	uint32_t	newTestNumber;

} LogNewTestNo_t;

/**
 * Logger devctl command data used to open (start) a client specific 
 * log file
 */
typedef struct _logOpenSpecialLog
{
	/**
	 * Number of characters in the log file name
	 */
	uint32_t	fileNameLength;
	/**
	 * Full path to the log file to be opened/started
	 */
	char		fileName[1];
} LogOpenSpecialLog_t;

/**
 * Logger devctl command data used to forward all received logger write commands
 * to another process (an output filter)
 */
typedef struct _logDittoProcess
{
	/**
	 * Number of characters in the log file name
	 */
	uint32_t	procNameLength;
	
	/**
	 * Full path to the log file to be opened/started
	 */
	char		procName[1];

} LogDittoProcess_t;

/**
 * Logger devctl message data
 */
typedef union
{
	LogNewTestNo_t			newTestNoCmd;
	LogOpenSpecialLog_t		openLogCmd;
	LogDittoProcess_t		dittoProcCmd;
} LogDevctlMsg_t;


#define DCMD_LOG_NEW_TEST	__DIOT(_DCMD_MISC, LOG_NEW_TEST_NUMBER, LogNewTestNo_t)
#define DCMD_LOG_OPEN_LOG	__DIOT(_DCMD_MISC, LOG_OPEN_SPECIAL_LOG, LogOpenSpecialLog_t)
#define DCMD_LOG_CLOSE_LOG	__DIOT(_DCMD_MISC, LOG_CLOSE_SEPCIAL_LOG, LogOpenSpecialLog_t)
#define DCMD_LOG_DITTO_PROC	__DIOT(_DCMD_MISC, LOG_DITTO_PROCESS, LogDittoProcess_t)


#endif // _LOG_DCMD_H_
