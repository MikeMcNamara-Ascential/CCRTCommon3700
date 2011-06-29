//*************************************************************************
// FILE:
//    $Header: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/FordSystemMonitorMain.cpp 1     4/25/05 9:27a Bmeinke $
//
// FILE DESCRIPTION:
//    Ford System Monitor Task
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/FordSystemMonitorMain.cpp $
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
// 
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:28p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 2     3/05/04 2:00p Bmeinke
// Moved __USAGE block to top of file to make diffs easier
// 
// 1     7/08/03 1:57p Derickso
// Restructured for new directory structure.
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
/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Tesla System Monitor Task

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
	- $Revision: 1 $
	- $Date: 4/25/05 9:27a $
		
#endif	// __USAGE
*******************************************************************************/

#include "TeslaSystemMonitor.h"

/**
 * Tesla System Monitor main task 
 * 
 * @param argc   Number of arguments inputted.
 * @param argv   Character array of arguments.
 */
int main( int argc, char *argv[])
{
	TeslaSystemMonitor *sysMon;

	while(1)
	{
		sysMon = new TeslaSystemMonitor( argc, argv);
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
