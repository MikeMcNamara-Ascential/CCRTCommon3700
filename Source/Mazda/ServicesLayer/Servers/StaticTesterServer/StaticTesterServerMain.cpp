//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterServerMain.cpp 1     10/26/06 1:21p Bbarrett $
//
// FILE DESCRIPTION:
//  Main loop for a StaticTesterServer.  If no special functionality is needed by a server,
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
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/StaticTesterServer/StaticTesterServerMain.cpp $
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 1     8/30/06 4:42p Derickso
// New static tester server.
//
//*************************************************************************
#include "StaticTesterServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the FaultServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	StaticTesterServer server;
	try
	{
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("StaticTesterServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("StaticTesterServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("StaticTesterServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("StaticTesterServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

StaticTesterServer Version 1.0
%C [options]
options: -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed for setup
	  -c turns on diagnostics in main()

#endif	// __USAGE


