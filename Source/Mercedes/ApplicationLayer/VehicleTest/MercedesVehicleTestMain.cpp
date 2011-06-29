//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/VehicleTest/MercedesVehicleTestMain.cpp 1     12/07/04 3:43p Derickso $
//
// FILE DESCRIPTION:
// The Mercedes Vehicle Test is responsible for execuiting a test of a vehicle.
// It creates a Selector to choose the appropriate DriveCurve to sequence based
// on the vehicle build and then creates a Sequencer to run the test.
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
//    $Log: /CCRT/Common/ApplicationLayer/VehicleTest/MercedesVehicleTestMain.cpp $
// 
// 1     12/07/04 3:43p Derickso
// Created new VSS database.
// 
// 1     11/01/04 3:11p Derickso
// Created.
// 
//*************************************************************************

#include "MercedesVehicleTest.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the MercedesVehicleTest server
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp;		// command line processor
	VehicleTest *server;		// Server object

	try
	{
		clp.ParseArguments(argc, argv);			// parse the command line
		if(clp.IsDebugOn())		printf("Creating The Vehicle Server\n");
		server = new MercedesVehicleTest();	// create a vehicle test component
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())		printf("Initializing The Vehicle Server\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())		printf("Running The Vehicle Server\n");
		server->Run();							// process until terminated
	}
	catch(XmlException &e)
	{
		printf("MercedesVehicleTest -%s: XmlException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(BepException &e)
	{
		printf("MercedesVehicleTest -%s: BepException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(...)
	{
		printf("MercedesVehicleTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())	printf("Deleting The MercedesVehicleTest Server\n");
	delete server;
	if(clp.IsDebugOn())  printf("MercedesVehicleTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	
	return(0);
};

/* *****************************************************************************************
 Usage block definition for the MercedesVehicleTest
***************************************************************************************** */
#ifdef __USAGE

MercedesVehicleTest Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

