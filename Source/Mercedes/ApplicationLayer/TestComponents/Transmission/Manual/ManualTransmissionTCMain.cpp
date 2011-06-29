//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Manual/ManualTransmissionTCMain.cpp 1     2/22/05 3:28p Derickso $
//
// FILE DESCRIPTION:
// This file contains the operations necessary to test the 42RE Transmission
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
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Manual/ManualTransmissionTCMain.cpp $
// 
// 1     2/22/05 3:28p Derickso
// New transmission type.
// 
// 1     11/08/04 1:17p Mkelly
// 
// 1     11/01/04 3:26p Mkelly
// 
// 1     8/04/04 12:38p Derickso
// Dcx common transmission test components
// 
// 1     1/27/04 9:34a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 2     1/10/04 10:18a Derickso
// Updated to support multiple protocols.
// Added deletion of test component prior to exiting.
// 
// 1     12/18/03 7:57a Derickso
// New transmission test components.
// 
//*************************************************************************
#include "ManualTransmissionTC.h"
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
		if(clp.IsDebugOn())	printf("Creating The Manual Transmission Component\n");
		transmissionTest = new ManualTransmissionTC();
		// Check if we should perform the test
		if (transmissionTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing The Manual Transmission Component\n");
			transmissionTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("Manual Transmission Component Running\n");
			// process until terminated
			transmissionTest->Run();							
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Manual Transmission -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Manual Transmission -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Manual Transmission -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("Manual Transmission (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component and return
	if (transmissionTest != NULL)  delete transmissionTest;
	transmissionTest = NULL;
	return 1;
}
