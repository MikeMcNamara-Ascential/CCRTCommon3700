//*************************************************************************
// FILE DESCRIPTION:
//  Server to monitor the side slip value and report out to the system.
//
//===========================================================================
// Copyright (c) 2016 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "CmdLineProcessor.h"
#include "SideSlipMonitor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The Side Slip Monitor\n");
		server = new SideSlipMonitor();		// create a Side SLip Monitor server
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The Side Slip Monitor\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("Side Slip Monitor (%d, %s) Up\n",
									BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("Side Slip Monitor -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Side Slip Monitor -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The Side Slip Monitor\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("Side Slip Monitor (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

