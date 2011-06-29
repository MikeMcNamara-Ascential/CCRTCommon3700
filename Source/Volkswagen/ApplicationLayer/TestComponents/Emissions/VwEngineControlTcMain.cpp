//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the VW Engine controller.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "VwEngineControlModule.cpp"
#include "GenericTCTemplate.cpp"
#include "CmdLineProcessor.h"
#include "KeywordProtocolFilter.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    GenericTC *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        if(clp.IsDebugOn())	printf("Creating the VW Engine Controller Component\n");
        object = new GenericTCTemplate<VwEngineControlModule<KeywordProtocolFilter> >();

		if(clp.IsDebugOn())	printf("Initializing the VW Engine Controller Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the VW Engine Controller Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("VW Engine Controller -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("VW Engine Controller -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("VW Engine Controller -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("VW Engine Controller (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
};
