//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/GenericServer/BepServerMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a base BepServer.  If no special functionality is needed by a server,
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
//    $Log: /Core/ServicesLayer/Servers/GenericServer/BepServerMain.cpp $
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
// 1     6/12/03 4:53p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:40  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 6:25a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
// 
// 12    8/23/02 8:26a Kh
// Synchronizing the pit machine with VSS.
// 
// 11    6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 10    3/20/02 9:05a Dan.erickson
// Change server object from a pointer to a static define.
// 
// 9     2/04/02 11:39p Skypig13
// Removed compiler warnings and streamline
//
// 8     12/12/01 11:03p Skypig13
// Updated to use Run() method
//
// 7     10/08/01 10:01p Skypig13
// Fixed the printouts for start and end so they will get printed
// if a verbosity is passsed in at the commad line
//
// 6     10/05/01 9:39a Skypig13
// Removed SystemSetup object and conditioned printouts of server up and
// terminating
//
// 5     10/02/01 11:38p Skypig13
// Removed #if 0 around old message handler.
//
// 4     9/28/01 5:04p Skypig13
// Updates to use thread
//
// 3     9/26/01 8:51a Skypig13
// Resolved confilct between TCP/IP and IPC handle requests
//
// 2     9/18/01 2:36p Skypig13
// Added id as parameter to Initialize() method of grunt
//
// 1     8/30/01 11:15a Skypig13
// New BepSever base class
//
// 1     8/30/01 9:18a Skypig13
// New base server
//
//
//*************************************************************************
#include "BepServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the BepServer
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
		BepServer server;		// Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("BepServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("BepServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("BepServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("BepServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

BepServer Version 4.0
%C [options]
options: -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed forset up
-c turns on diagnostics in main()

#endif	// __USAGE


