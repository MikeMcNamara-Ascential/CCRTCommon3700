//*************************************************************************
// FILE:
//    $Header: /Isuzu/ApplicationLayer/TestComponents/Machine/IsuzuMachineTCMain.cpp 1     12/21/03 6:33p Cward $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to calibrate the rolls
// tester.
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
//    $Log: /Isuzu/ApplicationLayer/TestComponents/Machine/IsuzuMachineTCMain.cpp $
// 
// 
// 
//*************************************************************************

#include "IsuzuMachineTC.cpp"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;		// command line processor
	GenericTC *component;	// component object

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		if(clp.IsDebugOn())	printf("Creating The Isuzu Machine Component\n");
		component = new IsuzuMachineTC();    // create a machine component component
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The Isuzu Machine Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The Isuzu Machine Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("IsuzuMachineTC -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("IsuzuMachineTC -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("IsuzuMachineTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(component != NULL)	delete component;	// delete the component object
	component = NULL;
	if(clp.IsDebugOn())
		printf("IsuzuMachineTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

	return(0);
}
