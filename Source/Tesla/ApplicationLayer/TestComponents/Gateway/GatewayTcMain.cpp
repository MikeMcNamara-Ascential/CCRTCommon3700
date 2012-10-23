//==============================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================

#include "GatewayTc.cpp"
#include "CmdLineProcessor.h"
#include "GatewayModule.cpp"
#include "KwpCanProtocolFilter.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    GenericTC *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        object = new GatewayTc<GatewayModule<KwpCanProtocolFilter> >();
		if(clp.IsDebugOn())	printf("Creating the Tesla Gateway Component\n");

		if(clp.IsDebugOn())	printf("Initializing the Tesla Gateway Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the Tesla Gateway Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("Tesla Gateway -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Tesla Gateway -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Tesla Gateway -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("Tesla Gateway (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
};
