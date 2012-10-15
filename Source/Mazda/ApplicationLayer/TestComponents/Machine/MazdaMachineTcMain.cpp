//******************************************************************************
// FILE DESCRIPTION:
// Mazda specific machine component
//
//==============================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "CmdLineProcessor.h"
#include "MazdaMachineTC.h"

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	CmdLineProcessor	clp;		// command line processor
	GenericTC			*component;	// component object

	try
	{
		clp.ParseArguments(argc, argv);     		// parse the command line
		if(clp.IsDebugOn())	printf("Creating The Machine Component\n");
		component = new MazdaMachineTC();
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The Machine Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The Machine Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("MazdaMachineTC -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("MazdaMachineTC -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("MazdaMachineTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	delete component;

	if(clp.IsDebugOn())  printf("MazdaMachineTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}
