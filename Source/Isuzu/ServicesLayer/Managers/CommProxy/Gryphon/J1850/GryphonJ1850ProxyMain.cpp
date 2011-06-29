//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/J1850/GryphonJ1850ProxyMain.cpp $
// $Author:: $
//   $Date::  $
//
// Description:
//  Logical port driver for J1850 communication on Gryphon
//  This file contains routines that are used only by Chrysler Can
//
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
//==============================================================================
#include "GryphonJ1850Proxy.h"
#include "CmdLineProcessor.h"
#include "XmlParser.h"
//******************************************************************************
int main( int argc, char *argv[])
{
	int retVal = RES_MGR_CMD_NONE;
	XmlParser parser;
	try
	{
		CmdLineProcessor clp(argc, argv);	// Command line processor
		printf("Creating a new GryphonJ1850Proxy object...\n");
		GryphonJ1850Proxy myGryphonChannel(argc,argv);
		// Initialiize the J1850 Comm Proxy
//		myGryphonChannel.Initialize(parser.ReturnXMLDocument(clp.GetConfigFile()));
		printf("Calling the Execute() method of the GryphonJ1850Proxy object...\n");
		while ( retVal != RES_MGR_CMD_TERMINATE)
		{
			retVal = myGryphonChannel.Execute();
		}
	}
	catch(XmlException &e)
	{
		printf("XmlException in GryphonJ1850Proxy - %s\n", e.GetReason());
	}
	catch(BepException &e)
	{
		printf("BepException in GryphonJ1850Proxy - %s\n", e.GetReason());
	}
	catch(...)
	{
		printf("Unknown Exception in GryphonJ1850Proxy\n");
	}
	printf("Execute() method complete, shutting down GryphonJ1850Proxy...\n");
	return 0;
}

/*******************************************************************************
  *** placeholder Usage Block    This needs to be revised when done, ***
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

Gryphon J1850 Proxy

%C [-d] [-e] [-ffile] [-rname] [-vx] [-pSerialPort] [-Pname] &

	-d    Run in demo mode
	-e    Echo logger messages to stdout
	-f    Read config from 'file'
	-r    Register using 'name'
	-v    Use verbose mask 'x' (in hex, see notes)
	-p    Communicate using serial port 'SerialPort' (i.e. /dev/ser1)
	-P    Send pulse to process registered as 'name' when up and running

	Notes:
	-Verbose Mask bit definitions (leading 0s optional):
		00000000: No Log Messages
		00000001: Log Errors
		00000002: Log Function Entry/Exit
		00000004: Log I/O message data
		00000008: Log device general messages
		00000010: Log serial communications
		FFFFFFFF: Log everything
	- Registers name as 'logport/GryphonManager' by default

#endif
*******************************************************************************/
