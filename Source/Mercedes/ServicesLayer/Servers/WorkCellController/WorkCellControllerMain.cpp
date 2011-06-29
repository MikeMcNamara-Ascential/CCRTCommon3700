//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellControllerMain.cpp 1     12/08/04 3:46p Derickso $
//
// FILE DESCRIPTION:
//  Main loop for a WorkCellController.  
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
//    $Log: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellControllerMain.cpp $
// 
// 1     12/08/04 3:46p Derickso
// Created new VSS database.
// 
// 1     12/07/04 4:27p Derickso
// Created new VSS database.
// 
// 1     8/04/04 1:07p Derickso
// 
// 1     1/27/04 9:49a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
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
//    Revision 1.1.1.1  2003/02/15 00:06:43  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:20a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:15a Khykin
// Initial Checkin From DCTC
// 
// 2     6/13/02 2:46p Dan.erickson
// Corrected spelling of Exception.
// 
// 1     4/05/02 7:49a Dan.erickson
// Created.
// 
//*************************************************************************

#include "WorkCellController.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the WorkCellController
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
		WorkCellController server;		// Server object
		server.Initialize(clp.GetConfigFile());	// Initialize server
		if(clp.IsDebugOn()) printf("WorkCellController(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server.Run();
	}
	catch(BepException &BepErr)
	{
		printf("WorkCellController -%s: Exception: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("WorkCellController -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn()) printf("WorkCellController(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	return(0);
}

#ifdef __USAGE

WorkCellController Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE
