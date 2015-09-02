//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for testing TPM systems.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "TpmsTc.cpp"
#include "GenericModuleTemplate.cpp"
#include "KwpCanProtocolFilter.cpp"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *tpmsTc = NULL;

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		tpmsTc = new TpmsTc<GenericModuleTemplate<KwpCanProtocolFilter> >;
		// Check if we should perform the test
		if (tpmsTc != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the tpmsTc Component\n");
			tpmsTc->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("tpmsTc Component Running\n");
			tpmsTc->Run();					// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("tpmsTc -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("tpmsTc -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("tpmsTc -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("tpmsTc (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the speed control test
	if (tpmsTc != NULL)  delete tpmsTc;
	tpmsTc = NULL;
	// Return control
	return (1);
}
