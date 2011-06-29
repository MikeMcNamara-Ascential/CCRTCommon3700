//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Mercedes Speed Control.
//
//===========================================================================
// Copyright (c) 2011- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MercedesSpeedControlTc.cpp"
#include "GenericEmissionsModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *speedControlTest;

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		speedControlTest = NULL;
		// Create a test component
		speedControlTest = new MercedesSpeedControlTc<GenericEmissionsModuleTemplate<KeywordProtocolFilter> >;
		// Check if we should perform the test
		if (speedControlTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the SpeedControlTest Component\n");
			speedControlTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("SpeedControlTest Component Running\n");
			speedControlTest->Run();					// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("SpeedControlTest -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("SpeedControlTest -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("SpeedControlTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("SpeedControlTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the speed control test
	if (speedControlTest != NULL)  delete speedControlTest;
	speedControlTest = NULL;
	// Return control
	return (1);
}
