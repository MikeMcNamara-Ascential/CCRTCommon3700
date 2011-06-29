//*************************************************************************
// Description:
//  Main loop for a BaldorMotorController.  If no special functionality is needed
//  by a server, initialize with configuration file and rock on!
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Baldor/BaldorMotorControllerMain.cpp $
// 
// 2     4/13/06 4:22a Cward
// cleanup
//
// 2     2/17/06 3:48p Bmeinke
// Removed the Revision tag from the __USAGE block
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:41p Cward
//
// 3     7/09/04 3:05p Bmeinke
// Removed #include AllenBradleyMotor.h
//
// 2     7/08/04 10:51a Bmeinke
// Moved the usage block to the top of the file (after main header
// comment)
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
// 1     6/26/03 4:13p Khykin
// Initial checkin.
//
// 5     5/30/03 12:00p Tbochene
// Updated print statements to reflect new process name
//
// 4     5/19/03 3:12p Tbochene
// Updated with latest changes to motor controller
//
// 3     4/08/03 3:52p Tbochene
// Updated diagnostics
//
// 2     9/06/02 11:30a Khykin
// Updating to use a MotorController pointer as the server.
//
// 1     4/03/02 1:25p Kh
// Initial Checkin.
//
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

/**************************************************************************

#ifdef __USAGE

BaldorMotorController

%C [options]
options: -f ConfigFile.xml [-c] [-a MotorType]

ConfigFile.xml is the xml configuration file needed for set up
MotorType specify AllenBradley else will default to Baldor
-c indicates that prints should be enabled for main

#endif	// __USAGE

**************************************************************************/

#include "BaldorMotorController.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the Serial Motor Controller
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	MotorController *controller = NULL;	// Motor controller

	try
	{
		controller = new BaldorMotorController();
			if(clp.IsDebugOn()) printf("Created Baldor Electric Motor Controller\n");

		if(clp.IsDebugOn()) printf("%d, %s Created\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		controller->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn()) printf("%d, %s Initialized\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
		controller->Run();	// Go do stuff
		if(clp.IsDebugOn()) printf("%d, %s Done\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	}
	catch(BepException &e)
	{
		printf("%s -f%s: BepException: %s\n", argv[0], clp.GetConfigFile().c_str(), e.what());
	}
	catch(...)
	{
		printf("%s -f%s: Unknown Exception\n", argv[0], clp.GetConfigFile().c_str());
	}
	if(controller) delete controller;
	if(clp.IsDebugOn()) printf("%s (%d, %s): Terminating\n", argv[0], BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

