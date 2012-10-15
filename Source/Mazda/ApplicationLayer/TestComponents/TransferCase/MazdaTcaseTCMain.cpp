//****************************************************************************
// DESCRIPTION:
//    Test specific to the Mazda AWD.
//
//============================================================================
//
//    Copyright (c) 2010- Burke E. Porter Machinery Company
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//*************************************************************************
#include "CmdLineProcessor.h"
#include "MazdaTcaseTC.h"

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	CmdLineProcessor	clp;		// command line processor
	GenericTC			*component;	// component object

	try
	{
		clp.ParseArguments(argc, argv);     		// parse the command line
		if(clp.IsDebugOn())	printf("Creating The Mazda Tcase Component\n");
		component = new MazdaTcaseTC();
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The Mazda Tcase Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The Mazda Tcase Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("MazdaTcaseTC -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("MazdaTcaseTC -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("MazdaTcaseTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	delete component;

	if(clp.IsDebugOn())  printf("MazdaTcaseTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}
