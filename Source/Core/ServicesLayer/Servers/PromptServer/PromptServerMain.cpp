//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/PromptServer/PromptServerMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a  PromptServer.  If no special functionality is needed by a server,
//  initialize with configuration file and rock on!
//	
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Servers/PromptServer/PromptServerMain.cpp $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:29a Khykin
// Initial checkin
// 
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 2     6/26/03 4:26p Khykin
// Updated with Flatrocks latest changes.
// 
// 7     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 6     4/25/02 4:44p Skypig13
// Updated with new changes
//
// 4     2/18/02 1:08p Dan.erickson
// Removed TerminateSignalHandler method prototype and
// diePig variable.
//
// 3     2/12/02 5:14p Dan.erickson
// Updated after restructure of BepServer.
//
// 2     12/03/01 10:06a Dan.erickson
// Modified to use the Initialize method that uses the filename.
//
// 1     11/26/01 4:42p Dan.erickson
// New files for PromptServer.
//
//*************************************************************************
#include "PromptServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the PromptServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	try
	{
		BepComponent *server = new PromptServer();		// Server object
		server->Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("PromptServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();
		if(server) delete server; server = NULL;
	}
	catch(BepException &BepErr)
	{
		printf("PromptServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("PromptServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("PromptServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

PromptServer Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE
