//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/VehicleTestMain.cpp 2     4/13/06 3:28a Cward $
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/VehicleTestMain.cpp $
// 
// 2     4/13/06 3:28a Cward
// Initialize server to NULL and do not delete it if it is NULL
//
// 2     2/15/06 3:45p Bmeinke
// Initialize server to NULL and do not delete it if it is NULL
//
// 1     12/21/03 6:33p Cward
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:45a Khykin
// Initial checkin
//
// 1     6/20/03 9:21a Khykin
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

#include "VehicleTest.h"
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
	CmdLineProcessor clp;		// command line processor
	VehicleTest *server = NULL;	// Server object

	try
	{
		clp.ParseArguments(argc, argv);			// parse the command line
		if(clp.IsDebugOn())		printf("Creating The Vehicle Server\n");
		server = new VehicleTest();				// create a vehicle test component
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())		printf("Initializing The Vehicle Server\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())		printf("Running The Vehicle Server\n");
		server->Run();							// process until terminated
	}
	catch(XmlException &e)
	{
		printf("VehicleTest -%s: XmlException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(BepException &e)
	{
		printf("VehicleTest -%s: BepException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(...)
	{
		printf("VehicleTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())	printf("Deleting The VehicleTest Server\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("VehicleTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

	return(0);
};

/* *****************************************************************************************
 Usage block definition for the VehicleTest
***************************************************************************************** */
#ifdef __USAGE

VehicleTest Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

