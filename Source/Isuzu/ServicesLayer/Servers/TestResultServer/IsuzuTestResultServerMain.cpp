//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/TestResultServer/TestResultServerMain.cpp 1     12/21/03 6:29p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a TestResult Server.  If no special functionality is needed by a server,
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
//    $Log: /Core/ServicesLayer/Servers/TestResultServer/TestResultServerMain.cpp $
// 
// 1     12/21/03 6:29p Cward
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
// 1     6/12/03 3:58p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:37  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:19a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
// 
// 15    10/07/02 1:10p Derickso
// Changed server to pointer from statically declared TestResultServer.
// 
// 14    6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 13    4/25/02 4:46p Skypig13
// Updated with new changes
//
// 11    3/20/02 11:02a Dan.erickson
// Changed allocation of server object from dynamic to static.
//
// 10    2/18/02 7:27p Dan.erickson
// Updated after rework of BepServer.
//
// 9     2/18/02 10:13a Skypig13
// Updated to use new BepServer format
//
// 8     12/03/01 2:45p Dan.erickson
// Modified to use Initialize method that takes filename.
//
// 7     11/20/01 5:36p Dan.erickson
// Moved tag subscription to BepServer
//
// 6     11/09/01 2:17p Dan.erickson
// Changed VehicleTestResult to OverallPassFail.
//
// 4     11/07/01 10:21a Dan.erickson
// Debugged and tested.
//
// 3     11/05/01 4:27p Dan.erickson
// Testing
//
// 2     11/01/01 2:56p Dan.erickson
// Compiles, not tested.
//
// 1     10/24/01 1:23p Dan.erickson
// Created.
//
//
//
//*************************************************************************
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "CmdLineProcessor.h"
#include "IsuzuTestResultServer.h"

/**
 * Main loop for the TestResultServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// Command line processor
	BepComponent *server = new TestResultServer(); 
	try
	{
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn()) printf("TestResultServer (%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();	// process data server requests
	}
	catch(BepException &BepErr)
	{
		printf("TestResultServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("TestResultServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if (server != NULL) delete server;
	server = NULL;
	if(clp.IsDebugOn()) printf("TestResultServer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

TestResultServer Version 2.0
%C [options]
options: -fConfigFile.xml -c
Where ConfigFile.xml is the xml configuration file needed forset up
and -c indicates that prints should be enabled for main loop

#endif	// __USAGE


