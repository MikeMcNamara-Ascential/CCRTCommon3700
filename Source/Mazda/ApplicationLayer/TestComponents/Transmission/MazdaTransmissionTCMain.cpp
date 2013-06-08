//******************************************************************************
// Description:
//  Mazda transmission component class
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "MazdaTransmissionTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{	// command line processor
    CmdLineProcessor clp;				
	XmlParser parser;
	GenericTC *transmissionTest;

	try
	{
		// parse the command line
        clp.ParseArguments(argc, argv);     
		// Set the test component pointer to NULL
		transmissionTest = NULL;
		if(clp.IsDebugOn())	printf("Creating The Mazda Transmission Component\n");
		transmissionTest = new MazdaTransmissionTC();
		// Check if we should perform the test
		if (transmissionTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing The Mazda Transmission Component\n");
			transmissionTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("Mazda Transmission Component Running\n");
			// process until terminated
			transmissionTest->Run();							
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Mazda Transmission -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Mazda Transmission -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Mazda Transmission -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("Mazda Transmission (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component and return
	if (transmissionTest != NULL)  delete transmissionTest;
	transmissionTest = NULL;
	return 1;
}
