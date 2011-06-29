//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBrokerMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//  The Named Data Broker is the broker for requests for data by processes.
//  It is responsible for receiving the requests for providers of data and
//	returning the appropriate server name to the client
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
//    $Log: /Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBrokerMain.cpp $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:29a Khykin
// Initial checkin
// 
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:25p Khykin
// Initial checkin
// 
// 1     6/12/03 4:01p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:08  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:15a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:10a Khykin
// Initial Checkin From DCTC
// 
// 8     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 7     4/24/02 9:02p Skypig13
// Removed sys/netmgr
//    Revision 1.2  2002/04/23 21:36:56  erickson
//    removed include of netmgr
//
//    Revision 1.1.1.1  2002/04/08 19:49:01  erickson
//    started
//
//
// 6     2/05/02 12:53a Skypig13
// Changes in BepServer and IBepCommunication as well as removing compiler
// warnings
//
// 5     12/12/01 11:21p Skypig13
// Update to use Run() method from BepServer for better shut-down
// reliability
//
// 4     11/15/01 7:14p Skypig13
// Updated to use new structure inherited from BepServer
//
// 3     8/24/01 8:32a Skypig13
// Removed conditional expression for printouts if an exception was raised
//
// 2     8/23/01 10:01a Skypig13
// Added diagnostic print for terminating
//
// 1     7/24/01 12:24p Skypig13
// New main loop
//
//
//*************************************************************************
#include "BepXmlUtils.h"
#include "BepException.h"
#include "NamedDataBroker.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the Named Data Broker process
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	BepComponent* server = NULL;		// Server
	try
	{
		server = new NamedDataBroker();	// Create Server
		server->Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("NamedDataBroker(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();
	}
	catch(BepException &BepErr)
	{
		printf("NamedDataBroker -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("NamedDataBroker -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(server) delete server; server = NULL;
	if(clp.IsDebugOn()) printf("NamedDataBroker(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

Nammed Data Broker Version 4.0

%C -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed for set up.
	  -c turns on diagnostics in main()

#endif	// __USAGE

