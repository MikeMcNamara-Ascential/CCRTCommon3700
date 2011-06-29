//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Axle/ElectronicLockingDifferential/ElectronicLockingDifferentialTCMain.cpp 1     6/09/05 4:44p Derickso $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to test the Electronic Limited
// Slip Differential.
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
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Axle/ElectronicLockingDifferential/ElectronicLockingDifferentialTCMain.cpp $
// 
// 1     6/09/05 4:44p Derickso
// New test component.
// 
// 3     1/03/05 1:44p Mkelly
// 
// 1     11/08/04 1:17p Mkelly
// 
// 1     11/01/04 3:26p Mkelly
// 
// 
//*************************************************************************
#include "TandemLockingDifferentialTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{	// command line processor
    CmdLineProcessor clp;				
	XmlParser parser;
	GenericTC *tldTest;

	try
	{
		// parse the command line
        clp.ParseArguments(argc, argv);     
		// Set the test component pointer to NULL
		tldTest = NULL;
        // Create a tld test component to talk SCI
		if(clp.IsDebugOn())	printf("Creating The TLD Component\n");
		tldTest = new TandemLockingDifferentialTC();
		// Check if we should perform the test
		if (tldTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing The TLD Component\n");
			tldTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("TLD Component Running\n");
			// process until terminated
			tldTest->Run();							
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("TLD -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("TLD -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("TLD -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("TLD (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component and return
	if (tldTest != NULL)  delete tldTest;
	tldTest = NULL;
	return 1;
}
