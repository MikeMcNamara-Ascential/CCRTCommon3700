//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/SystemMonitor/SystemMonitorMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//    System Monitor Task
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
//    $Log: /Core/ServicesLayer/Servers/SystemMonitor/SystemMonitorMain.cpp $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:29a Khykin
// Initial checkin
// 
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
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
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
// 
//*************************************************************************

#include "ChinaSystemMonitor.h"

/**
 * System Monitor main task 
 * 
 * @param argc   Number of arguments inputted.
 * @param argv   Character array of arguments.
 */
int main( int argc, char *argv[])
{
	ChinaSystemMonitor *sysMon;

	while(1)
	{
		sysMon = new ChinaSystemMonitor( argc, argv);
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

#ifdef __USAGE

Launcher Version 1.0

%C [-f file]

options: 
	
	-f 		indicates the XML configuration file to use for initialization


Note:	The launcher must be compiled as root due to the types of
		tasks that are managed.
		
#endif	// __USAGE


