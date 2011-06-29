//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/FaultServer/FaultServerMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a  FaultServer.  If no special functionality is needed by a server,
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
//    $Log: /Core/ServicesLayer/Servers/FaultServer/FaultServerMain.cpp $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:32a Khykin
// Initial checkin
// 
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:25p Khykin
// Initial checkin
// 
// 2     6/26/03 1:19p Khykin
// Updated with changes made during the Flatrock debug.
// 
// 7     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 6     4/25/02 4:38p Skypig13
// Updated with new changes
//
// 4     3/20/02 9:09a Dan.erickson
// Changed server definition from dynamic alocation to static
// allocation.
//
// 3     2/18/02 1:07p Dan.erickson
// Removed TerminatSignalHandler method prototype
// and diePig variable.
//
// 2     2/12/02 5:12p Dan.erickson
// Updated because of BepServer restructuring.
//
// 1     11/20/01 10:14a Dan.erickson
// New files for FaultServer
//
//
//*************************************************************************
#include "FaultServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the FaultServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	FaultServer server;
	try
	{
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("FaultServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("FaultServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("FaultServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("FaultServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

FaultServer Version 2.0
%C [options]
options: -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed for setup
	  -c turns on diagnostics in main()

#endif	// __USAGE


