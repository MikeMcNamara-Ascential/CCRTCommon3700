//******************************************************************************
// FILE:
//
// FILE DESCRIPTION:
// This file contains the functions necessary to calibrate the rolls
// tester.
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TevesAbsTc.cpp"
#include "CmdLineProcessor.h"
#include "TevesAbsModule.cpp"
#include "KeywordProtocolFilter.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    GenericTC *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        object = new TevesAbsTc<TevesAbsModule<KeywordProtocolFilter> >();
		if(clp.IsDebugOn())	printf("Creating the Tesla Brake Component\n");

		if(clp.IsDebugOn())	printf("Initializing the Tesla Brake Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the Tesla Brake Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("Tesla Brake -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Tesla Brake -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Tesla Brake -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("Tesla Brake (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
};
