//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/PlantHostInbound/PlantHostInboundMain.cpp 1     1/27/04 4:53p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a PlantHostInbound.  
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
//    $Log: /Core/ServicesLayer/Servers/PlantHostInbound/PlantHostInboundMain.cpp $
// 
// 1     1/27/04 4:53p Cward
// 
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
// 
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 2     6/24/03 7:58a Khykin
// Updating with Genes latest changes.
// 
// 1     6/20/03 10:00a Khykin
// Initial checkin.
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
// 1     1/18/03 8:11a Khykin
// Initial Checkin From DCTC
// 
// 2     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 1     4/03/02 2:04p Dan.erickson
// Created.
// 
//*************************************************************************

#include "PlantHostInbound.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the PlantHostInbound
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
		PlantHostInbound server;		// Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("PlantHostInbound(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("PlantHostInbound -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("PlantHostInbound -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("PlantHostInbound(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

PlantHostInbound Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE
