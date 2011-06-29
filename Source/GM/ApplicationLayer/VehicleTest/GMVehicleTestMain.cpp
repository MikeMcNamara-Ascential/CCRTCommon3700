//*************************************************************************
// FILE:
//    $Header: /CMake Core/Source/Ford/ApplicationLayer/VehicleTest/FordVehicleTestMain.cpp 1     4/25/05 9:27a Bmeinke $
//
// FILE DESCRIPTION:
// The Vehicle Test is responsible for execuiting a test of a vehilce.  It
// creates a Selector to choose the appropriate DriveCurve to sequence based
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
//    $Log: /CMake Core/Source/Ford/ApplicationLayer/VehicleTest/FordVehicleTestMain.cpp $
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
// 
// 1     6/04/04 1:08p Bmeinke
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 1     7/08/03 1:40p Derickso
// Restructured for new directory structure.
// 
// 1     6/20/03 9:46a Khykin
// Initial checkin
// 
// 1     6/20/03 9:22a Khykin
// Initial checkin.
// 
// 1     4/22/03 10:32a Khykin
// Initial Working Checkin.
// 
// 2     4/07/03 9:20a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:10  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 11:16a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:47a Khykin
// Initial Checkin
// 
// 4     3/25/02 6:19p Kh
// Updating due to changes in the base classes.
// 
// 3     12/27/01 10:19a Kh
// Updating the main loop.
// 
// 2     12/20/01 10:19a Kh
// Initial Testing Complete.
// 
// 1     11/30/01 8:16a Kh
// Initial Checkin.
// 
//*************************************************************************

#include "GMVehicleTest.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the VehicleTest server
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor *clp=NULL;			// command line processor
	GMVehicleTest *server=NULL;		// Server object

	try
	{
		clp = new CmdLineProcessor(argc, argv, true);			// parse the command line
//		if(clp->IsDebugOn())
			printf("Creating The GM Vehicle Server\n");
		server = new GMVehicleTest();			// create a vehicle test component
		// create, initialize, and process messages for the named data broker
//		if(clp->IsDebugOn())		
			printf("Initializing The GM Vehicle Server: %s\n", clp->GetConfigFile().c_str()); fflush(stdout);
			BposSleep(1000);
		server->Initialize(clp->GetConfigFile());
		if(clp->IsDebugOn())		printf("Running The GM Vehicle Server\n");
		server->Run();							// process until terminated
	}
	catch(XmlException &e)
	{
		printf("GMVehicleTest -%s: XmlException: %s\n", clp->GetConfigFile().c_str(), e.what());
	}
	catch(BepException &e)
	{
		printf("GMVehicleTest -%s: BepException: %s\n", clp->GetConfigFile().c_str(), e.what());
	}
	catch(...)
	{
		printf("VehicleTest -%s: Unknown Exception\n", clp->GetConfigFile().c_str());
	}

	if(clp->IsDebugOn())	printf("Deleting The VehicleTest Server\n");
	if(server != NULL)		delete server;
	if(clp->IsDebugOn())  	printf("VehicleTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp->GetConfigFile().c_str());
	if(clp != NULL)			delete clp;
	return(0);
};

#ifdef __USAGE

VehicleTest Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

