//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerMain.cpp 2     4/13/06 4:30a Cward $
//
// FILE DESCRIPTION:
//   Vehicle data Server handles the vehicle build data
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerMain.cpp $
// 
// 2     4/13/06 4:30a Cward
// cleanup usage block
//
// 1     12/21/03 6:29p Cward
//
// 1     10/07/03 11:28a Khykin
// Initial checkin
//
// 1     8/29/03 11:27a Khykin
// Initial checkin.
//
// 1     8/22/03 3:19p Rwiersem
// Initial version.
//
//*************************************************************************

#include "CmdLineProcessor.h"
#include "VehicleDataServer.h"

/**
 * Main loop for the VehicleDataServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @return Exit status of process
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The VehicleDataServer\n");
		server = new VehicleDataServer();		// create a VehicleData
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The VehicleDataServer\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("VehicleDataServer (%d, %s) Up - Listening\nRunning The VehicleDataServer\n",
									BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("VehicleDataServer -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("VehicleDataServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The VehicleDataServer\n");
	delete server;
	if(clp.IsDebugOn())  printf("VehicleDataServer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

VinServer Version 1.0
%C [options]
options:
    -f      Configuration File (Ex. VehicleTestConfig.xml)
    -vx     Verbosity flag, set to level x

notes:
    - After Launcher This Must Be Provided The Register Command.

#endif	// __USAGE

