//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/PlantHostOutbound/PlantHostOutboundMain.cpp 3     5/08/06 7:44p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a PlantHostOutbound object.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PlantHostOutbound/PlantHostOutboundMain.cpp $
// 
// 3     5/08/06 7:44p Cward
// Fixed reload capability
//
// 1     4/27/06 1:10p Rwiersem
// Removed passing of INamedDataBroker pointer to the constructor.
//
// 2     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 2:05p Rwiersem
// Now pass an INamedDataBroker object to PlantHostOutbound.  This allows
// us to mock the INamedDataBroker object for testing.
//
// 1     1/27/04 4:53p Cward
//
// 1     8/21/03 9:09a Derickso
// 05WK CCRT Project
//
// 2     8/07/03 2:42p Khykin
// Updated with changes from Flatrock.
//
// 2     6/24/03 7:59a Khykin
// Updated with Genes latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 1     6/12/03 4:00p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:09  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:16a Khykin
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

#include "PlantHostOutbound.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"
#include "INamedDataBroker.h"

/**
 * Main loop for the PlantHostOutbound
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
        PlantHostOutbound server;      // Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("PlantHostOutbound(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("PlantHostOutbound -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("PlantHostOutbound -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("PlantHostOutbound(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

PlantHostOutbound Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE
