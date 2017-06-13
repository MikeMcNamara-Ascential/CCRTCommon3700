//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to test the Advics ABS
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include "CmdLineProcessor.h"
#include "AdvicsABSTC.cpp"
#include "AdvicsISOABSModule.cpp"
#include "IsoProtocolFilter.h"

int main(int argc, char *argv[])
{	// command line processor
    CmdLineProcessor clp;				
	GenericTC *brakeTest;

	try
	{	// parse the command line
        clp.ParseArguments(argc, argv);     
		// Set the pointer to NULL
		brakeTest = NULL;
		// create a Advics component object
        if(clp.IsDebugOn())	printf("Creating the Advics ABS Test Component\n");
        brakeTest = new AdvicsABSTC<AdvicsISOABSModule<IsoProtocolFilter> >;
		// Check if we should perform the test
		if (brakeTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the Advics ABS Component\n");
			brakeTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("Advics ABS Component Running\n");
			// process until terminated
			brakeTest->Run();
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Advics ABS -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Advics ABS -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Advics ABS -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("Advics ABS (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Make sure to delete the test component
	if (brakeTest != NULL)   delete brakeTest;
	brakeTest = NULL;
	// Return to the system
	return 1;
}
