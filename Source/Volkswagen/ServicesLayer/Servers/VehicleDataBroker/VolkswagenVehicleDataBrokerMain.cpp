//*************************************************************************
// FILE DESCRIPTION:
//   Machine data broker that is responsible for maintaining and publishing
//   all of the information related to the vehicle.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "CmdLineProcessor.h"
#include "VolkswagenVehicleDataBroker.h"


int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The VolkswagenVehicleDataBroker\n");
		server = new VolkswagenVehicleDataBroker();		// create a VolkswagenVehicleDataBroker server
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The VolkswagenVehicleDataBroker\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("VolkswagenVehicleDataBroker (%d, %s) Up - Listening\nRunning The VolkswagenVehicleDataBroker\n",
                                    BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("VolkswagenVehicleDataBroker -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("VolkswagenVehicleDataBroker -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The VolkswagenVehicleDataBroker\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("VolkswagenVehicleDataBroker (%d, %s): Terminating\n", 
                                BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

VolkswagenVehicleDataBroker
%C [options]
options:
    -f      Configuration File (Ex. VehicleTestConfig.xml)
    -vx     Verbosity flag, set to level x

notes:
    - After Launch This Must Be Provided The Register Command.

#endif	// __USAGE

