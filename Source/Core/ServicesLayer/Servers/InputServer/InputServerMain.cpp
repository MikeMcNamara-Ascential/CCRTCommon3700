//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/InputServer/InputServerMain.cpp 1     12/21/03 6:42p Cward $
//
// FILE DESCRIPTION:
//  Main loop for an InputServer.  
//	
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Servers/InputServer/InputServerMain.cpp $
// 
// 1     12/21/03 6:42p Cward
// 
// 1     10/07/03 11:32a Khykin
// Initial checkin
// 
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:25p Khykin
// Initial checkin
// 
// 1     6/12/03 4:04p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:51  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:12a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
// 
// 1     5/31/02 3:03p Dan.erickson
// Created and Debugged.
// 
//*************************************************************************

#include "InputServer.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the InputServer
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
		InputServer server;		// Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("InputServer(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("InputServer -%s: Excpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("InputServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("InputServer(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

InputServer Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE
