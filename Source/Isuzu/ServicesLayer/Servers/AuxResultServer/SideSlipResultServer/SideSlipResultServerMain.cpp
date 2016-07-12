//*************************************************************************
// FILE:
//    $Header: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/SideSlipResultServer/SideSlipResultServerMain.cpp 3     11/17/05 5:17p Bmeinke $
//
// FILE DESCRIPTION:
//	Tata External Test Result Processor
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
//    $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/SideSlipResultServer/SideSlipResultServerMain.cpp $
// 
// 3     11/17/05 5:17p Bmeinke
// Added $NoKeywords$ and removed $Revision$ and $Date$ from the USAGE
// block
// 
// 2     9/14/05 4:37p Bmeinke
// Use a CommandLineProcessor object to parse the command line arguments
// Declare our server as a BepServer pointer in order to call
// Initialize(string fileName)
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 2     8/30/05 4:21p Bmeinke
// Fixed the __USAGE section
// 
// 1     8/18/05 3:43p Bmeinke
// Initial check-in (not compiled)
// 
//*************************************************************************
// $NoKeywords: $
//==============================================================================
/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

External Test Result Processor. Used to gather test results from an external 
side slip tester and report them in the vehicle test result file

%C [-c/-e][-d] [-ffile] [-py] [-vx] &

    -c        Enable echo of logger messages to local console
    -e        Enable echo of logger messages to local console
    -d        Run in demo mode
    -ffile    Read config from 'file'
    -py       Run at priority 'y'
    -vx       Use verbose mask 'x' (in hex, see notes)

    Process Dependencies:
      Required:
        NamedDataBroker.qxe (needed to read data tags in the system)
        Comm proxy (needed to receive serial data from the external system)
    
    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        FFFFFFFF: Log everything
    - Registers name as 'core/AuxResultServer' by default
    - Must be run as Super User.
        
#endif  // __USAGE
*******************************************************************************/

#include "CmdLineProcessor.h"
#include "SideSlipResultServer.h"

/**
 * Tata Side Slip Test Result Processor main task 
 * 
 * @param argc   Number of arguments inputted.
 * @param argv   Character array of arguments.
 */
int main( int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepServer	*server;

	if( clp.IsDebugOn())	printf( "SideSlipResultServer starting\n");
	server = new SideSlipResultServer;
	try
	{
		server->Initialize(clp.GetConfigFile());
		server->Run();
	}
	catch(BepException &err)
	{
		printf(" Ooops: %s\n", err.GetReason());
    }

	delete server;
	if( clp.IsDebugOn())	printf( "SideSlipResultServer done\n");

	return( 0);
}
