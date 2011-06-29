//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Transmission/Transmission/GenericTransmissionTCMain.cpp 1     2/15/06 6:48p Cward $
//
// FILE DESCRIPTION:
// This file contains the operations necessary to test a generic Transmission
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Transmission/Transmission/GenericTransmissionTCMain.cpp $
// 
// 1     2/15/06 6:48p Cward
// Add stand alone Generic Transmission Test component
// 
//*************************************************************************
#include "GenericTransmissionTCTemplate.cpp"
#include "GenericTransmissionModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"
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
        // Create a Manual transmission test component to talk SCI
		if(clp.IsDebugOn())	printf("Creating The Generic Transmission Component\n");
		transmissionTest = new GenericTransmissionTCTemplate<GenericTransmissionModuleTemplate<KeywordProtocolFilter> >();
		// Check if we should perform the test
		if (transmissionTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the Generic Transmission Component\n");
			transmissionTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("Generic Transmission Component Running\n");
			// process until terminated
			transmissionTest->Run();							
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Generic Transmission -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Generic Transmission -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Generic Transmission -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("Generic Transmission (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component and return
	if (transmissionTest != NULL)  delete transmissionTest;
	transmissionTest = NULL;
	return 1;
}
