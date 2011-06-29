//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ServicesLayer/Servers/SystemMonitor/MercedesSystemMonitorMain.cpp 1     12/08/04 4:19p Derickso $
//
// FILE DESCRIPTION:
//    Mercedes System Monitor Task
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ServicesLayer/Servers/SystemMonitor/MercedesSystemMonitorMain.cpp $
// 
// 1     12/08/04 4:19p Derickso
// Created new VSS database.
// 
// 1     8/04/04 1:16p Derickso
// 
// 1     1/27/04 9:48a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
// Initial checkin
// 
// 1     6/27/03 8:21a Khykin
// Update from SysMon to SystemMonitor.
// 
// 1     6/20/03 9:55a Khykin
// Initial checkin.
// 
// 1     6/12/03 3:58p Khykin
// Initial checkin.
// 
//*************************************************************************

#include "MercedesSystemMonitor.h"

/**
 * Mercedes System Monitor main task 
 * 
 * @param argc   Number of arguments inputted.
 * @param argv   Character array of arguments.
 */
int main( int argc, char *argv[])
{
	MercedesSystemMonitor *sysMon;

	while(1)
	{
		sysMon = new MercedesSystemMonitor( argc, argv);
		try
		{
			sysMon->Run();
		}
		catch(BepException &err)
		{
			printf(" Ooops: %s\n", err.GetReason());
			break;
		}

		delete sysMon;
	}

	return( 0);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
*******************************************************************************/
#ifdef __USAGE

Mercedes System Monitor Task

%C [-c/-e][-d] [-ffile] [-py] [-vx] [-r] &

	-c        Enable echo of logger messages to local console
	-d        Run in demo mode
	-e        Enable echo of logger messages to local console
	-ffile    Read config from 'file'
	-py       Run at priority 'y'
	-vx       Use verbose mask 'x' (in hex, see notes)
	-r        Dont wait for Register pulse

	Process Dependencies:
	  Required:
		NamedDataBroker.qxe (needed to read data tags in the system)
		PromptServer.qxe    (needed for prompt display)
		FaultServer.qxe     (fault reporting)
	  Optional:
		PlcMgr.qxe          (needed to supply machine I/O data)
		DioMgr.qxe          (needed to supply machine I/O data)
	
	Notes:
	-Verbose Mask bit definitions (leading 0s optional):
		00000000: No Log Messages
		00000001: Log Errors
		00000002: Log Function Entry/Exit
		00000004: Log I/O message data
		00000008: Log device general messages
		FFFFFFFF: Log everything
	- Registers name as 'rolls/SystemMonitor' by default
	- Must be run as Super User.
	- $Revision: 3 $
	- $Date: 8/29/05 1:08p $
		
#endif	// __USAGE


