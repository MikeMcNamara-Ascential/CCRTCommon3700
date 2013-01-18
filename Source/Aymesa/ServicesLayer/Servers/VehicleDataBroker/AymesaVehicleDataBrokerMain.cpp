//*************************************************************************
// FILE DESCRIPTION:
//   Vehicle data broker that is responsible for maintaining and publishing
//   all of the information related to the vehicle.
//
//===========================================================================
// Copyright (c) 2013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "CmdLineProcessor.h"
#include "AymesaVehicleDataBroker.h"


int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The AymesaVehicleDataBroker\n");
		server = new AymesaVehicleDataBroker();		// create a AymesaVehicleDataBroker server
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The AymesaVehicleDataBroker\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("AymesaVehicleDataBroker (%d, %s) Up - Listening\nRunning The AymesaVehicleDataBroker\n",
                                    BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("AymesaVehicleDataBroker -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("AymesaVehicleDataBroker -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The AymesaVehicleDataBroker\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("AymesaVehicleDataBroker (%d, %s): Terminating\n", 
                                BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

AymesaVehicleDataBroker
%C [options]
options:
    -f      Configuration File (Ex. VehicleTestConfig.xml)
    -vx     Verbosity flag, set to level x

notes:
    - After Launch This Must Be Provided The Register Command.

#endif	// __USAGE

