//*************************************************************************
// FILE DESCRIPTION:
//   Interface to the Windows based CCRT system.
//   This class allows reading and writting data from the Windows based 
//   CCRT system so all data available to the QNX CCRT system can be available
//   to the Windows based CCRT system.  This is mainly intended to assist
//   the migration of CCRT from QNX to Windows.
//
//===========================================================================
// Copyright (c) 2011 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "CmdLineProcessor.h"
#include "WinCcrtInterface.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object
	try
	{
		if(clp.IsDebugOn())	printf("Creating The WinCcrtInterface\n");
		server = new WinCcrtInterface();		// create a WinCcrtInterface server
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The WinCcrtInterface\n");
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())  printf("WinCcrtInterface (%d, %s) Up - Listening\nRunning The WinCcrtInterface\n",
									BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
	}
	catch(BepException &BepErr)
	{
		printf("WinCcrtInterface -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("WinCcrtInterface -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Deleting The WinCcrtInterface\n");
	if( NULL != server)	delete server;
	if(clp.IsDebugOn())  printf("WinCcrtInterface (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

