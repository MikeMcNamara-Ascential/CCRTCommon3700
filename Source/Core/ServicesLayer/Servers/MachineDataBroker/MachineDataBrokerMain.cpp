//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/MachineDataBroker/MachineDataBrokerMain.cpp 2     4/13/06 4:17a Cward $
//
// FILE DESCRIPTION:
//   Machine data broker that is responsible for maintaining and publishing
//   all of the information related to the machine.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MachineDataBroker/MachineDataBrokerMain.cpp $
// 
// 2     4/13/06 4:17a Cward
// Init server
//
// 2     3/20/06 10:59a Bmeinke
// Initialize server to NULL
//
// 1     12/21/03 6:41p Cward
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:03p Khykin
// Initial checkin.
//
// 1     4/22/03 9:48a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:01  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:14a Khykin
// Initial Checkin
//
// 1     1/18/03 8:09a Khykin
// Initial Checkin From DCTC
//
// 3     4/29/02 6:06p Kh
// Updating usage information.
//
// 2     2/21/02 2:08p Skypig13
// Update to new BepServer format
//
// 1     12/28/01 12:12p Kh
// Initial Checkin.
//
//*************************************************************************

#include "CmdLineProcessor.h"
#include "MachineDataBroker.h"

/**
 * Main loop for the MachineDataBroker server
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @return Exit status of process
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The MachineDataBroker\n");
		server = new MachineDataBroker();		// create a MachineDataBroker server
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The MachineDataBroker\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("MachineDataBroker (%d, %s) Up - Listening\nRunning The MachineDataBroker\n",
									BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("MachineDataBroker -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("MachineDataBroker -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The MachineDataBroker\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("MachineDataBroker (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

MachineDataBroker
%C [options]
options:
    -f      Configuration File (Ex. VehicleTestConfig.xml)
    -vx     Verbosity flag, set to level x

notes:
    - After Launcher This Must Be Provided The Register Command.

#endif	// __USAGE

