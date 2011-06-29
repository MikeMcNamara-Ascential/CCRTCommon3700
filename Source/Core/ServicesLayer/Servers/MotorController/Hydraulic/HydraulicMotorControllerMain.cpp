//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorControllerMain.cpp 2     4/13/06 4:23a Cward $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorControllerMain.cpp $
// 
// 2     4/13/06 4:23a Cward
// cleanup usage block
//
// 2     2/17/06 3:43p Bmeinke
// Removed the version from the __USAGE block
//
// 1     5/11/05 10:06p Cward
//
// 1     5/11/05 9:39p Cward
// Initial Version
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 1     12/21/03 6:41p Cward
//
// 2     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 3     5/19/03 12:25p Khykin
// Updated with Tims latest changes.
//
// 4     4/29/02 4:23p Skypig13
// Removed #include <sys/netmgr.h>
//
// 3     3/07/02 6:51p Skypig13
// Update to new BepServer structure
//
// 2     12/21/01 1:29p Kh
// Updating the main loop.
//
//
//*************************************************************************

#include "HydraulicMotorController.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the Hydraulic Motor Controller
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 2.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	BepComponent *server=NULL;				// Server object

	try
	{
		server = new HydraulicMotorController();
		if(clp.IsDebugOn()) printf("HydraulicMotorController (%d, %s) Up - Initializing\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn()) printf("HydraulicMotorController (%d, %s) Up - Run\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		server->Run();							// process until terminated
		if(clp.IsDebugOn()) printf("HydraulicMotorController (%d, %s) Up - All Done\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	}
	catch(XmlException &e)
	{
		printf("HydraulicMotorController.qxe -f%s: XmlException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(BepException &e)
	{
		printf("HydraulicMotorController.qxe -f%s: BepException: %s\n", clp.GetConfigFile().c_str(), e.what());
	}
	catch(...)
	{
		printf("HydraulicMotorController.qxe -f%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if( NULL != server)	delete server;
	printf("MotorController (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

HydraulicMotorController
%C [options]
options: -fConfigFile.xml -c

Where ConfigFile.xml is the xml configuration file needed forset up
and -c indicates that prints should be enabled for main loop

#endif	// __USAGE



