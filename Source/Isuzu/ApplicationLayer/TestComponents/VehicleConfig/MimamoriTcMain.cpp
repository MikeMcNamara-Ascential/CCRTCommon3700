//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for testing Isuzu Mimamori Module.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "MimamoriTc.cpp"
#include "MimamoriModule.cpp"
#include "IsuzuCommonInfoProtocolFilter.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *vehicleConfig = NULL;
	
	try
	{   // Parse the command line arguments
		clp.ParseArguments(argc, argv);     // parse the command line
		vehicleConfig = new MimamoriTc<MimamoriModule<IsuzuCommonInfoProtocolFilter> >();
		// Check if we should perform the test
		if (vehicleConfig != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the Isuzu Vehicle Config Test Component\n");
			vehicleConfig->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("Isuzu Vehicle Config Test Component Running\n");
			vehicleConfig->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Isuzu Vehicle Config Test -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Isuzu Vehicle Config Test -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Isuzu Vehicle Config Test -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Isuzu Vehicle Config Test (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component
	if (vehicleConfig != NULL)  delete vehicleConfig;
	vehicleConfig = NULL;
	// Return control
	return (1);
}
