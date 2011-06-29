//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to interface with the gas analyzer.
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
//-----------------------------------------------------------------------------
#include "GasAnalyzerTC.cpp"
#include "GenericModuleTemplate.cpp"
#include "IsoProtocolFilter.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;		// command line processor
	GenericTC *component;	// Component object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		if(clp.IsDebugOn())	printf("Creating The GasAnalyzer Component\n");
		component = new GasAnalyzerTC<GenericModuleTemplate<IsoProtocolFilter> >();    // create a GasAnalyzer component
		if(clp.IsDebugOn())	printf("Initializing The GasAnalyzer Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The GasAnalyzer Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("GasAnalyzer -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("GasAnalyzer -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("GasAnalyzer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(component != NULL)	delete component;	// delete the component object
	component = NULL;
	if(clp.IsDebugOn())
		printf("GasAnalyzer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

	return(0);
}
