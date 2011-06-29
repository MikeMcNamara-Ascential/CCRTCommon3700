//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  Test Component for testing DaimlerChrysler NGC4 engine controller
//  using Keyword Protocol 2000.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
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
#include "TachographTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *tachoTest;
	
	try
	{   // Parse the command line arguments
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		tachoTest = new TachographTC();
		// Check if we should perform the test
		if (tachoTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the Tachograph Component\n");
			tachoTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("Tachograph Component Running\n");
			tachoTest->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Tachograph -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Tachograph -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Tachograph -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	printf("Tachograph (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component
	if (tachoTest != NULL)  delete tachoTest;
	tachoTest = NULL;
	// Return control
	return (1);
}
