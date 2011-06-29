//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Servers/VehicleDataBroker/VehicleDataBrokerMain.cpp 1     12/21/03 6:29p Cward $
//
// FILE DESCRIPTION:
//  Main loop for a VehicleDataBroker.  If no special functionality is needed by a server,
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
//    $Log: /Core/ServicesLayer/Servers/VehicleDataBroker/VehicleDataBrokerMain.cpp $
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
// 1     6/12/03 3:57p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:52a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:40  tbochene
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
// 6     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 5     4/25/02 4:47p Skypig13
// Updated with new changes
//
// 3     3/25/02 9:09a Dan.erickson
// Changed allocation of server object from dynamic to static.
//
// 2     2/18/02 1:09p Dan.erickson
// Reworked aftr restructure of BepServer.
//
// 1     12/06/01 3:17p Dan.erickson
// Created.
//
//*************************************************************************
#include "VehicleDataBroker.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the VehicleDataBroker
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
		VehicleDataBroker server;		// Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("VehicleDataBroker(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("VehicleDataBroker -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("VehicleDataBroker -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("VehicleDataBroker(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

VehicleDataBroker Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE


