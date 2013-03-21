//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/InterCcrtCommServer/InterCcrtCommServerMain.cpp 1     10/26/06 1:21p Bbarrett $
//
// FILE DESCRIPTION:
//  Main loop for a InterCcrtCommServer.  If no special functionality is needed by a server,
//  initialize with configuration file and rock on!
//	
//===========================================================================
// Copyright (c) 2006- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
//*************************************************************************
#include "InterCcrtCommServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the Inter Ccrt comm server
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	InterCcrtCommServer server;
	try
	{
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("InterCcrtCommServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("InterCcrtCommServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("InterCcrtCommServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("InterCcrtCommServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

InterCcrtCommServer Version 1.0
%C [options]
options: -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed for setup
	  -c turns on diagnostics in main()

#endif	// __USAGE


