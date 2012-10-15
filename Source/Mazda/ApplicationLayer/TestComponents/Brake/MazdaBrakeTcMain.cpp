//******************************************************************************
// FILE:
//
// FILE DESCRIPTION:
//
//==============================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
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

#include "MazdaBrakeTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    GenericTC *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        object = new MazdaBrakeTC();
		if(clp.IsDebugOn())	printf("Creating the Mazda Brake Component\n");

		if(clp.IsDebugOn())	printf("Initializing the Mazda Brake Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the Mazda Brake Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("Mazda Brake -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Mazda Brake -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Mazda Brake -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("Mazda Brake (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    if(object != NULL) 
    {
        delete object;
        object = NULL;
    }

    return 0;
};
