//*************************************************************************
// Description:
//  Main loop for an Allen Bradley MotorController.  If no special functionality
//  is needed  by a server, initialize with configuration file and rock on!
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/AB/ABMotorControllerMain.cpp $
// 
// 2     4/13/06 4:21a Cward
// cleanup usage block
//
// 2     2/17/06 3:42p Bmeinke
// Removed the Revision tag from the __USAGE block
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 1     6/11/04 4:12p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:00a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 2     1/13/04 4:50p Bmeinke
// Removed #include "./Motor/BaldorMotor.h" and #include
// "./Motor/AllenBradleyMotor.h"
//
// 1     12/29/03 9:19a Bmeinke
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#include "ABMotorController.h"
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
 * @since 1.0
 */
int main(int argc, char *argv[])
{
	CmdLineProcessor clp(argc, argv);	// command line processor
	MotorController *controller = NULL;	// Motor controller

	try
	{
		controller = new ABMotorController;
		if(clp.IsDebugOn()) printf("Created AllenBradley Electric Motor Controller\n");

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


/*
#ifdef __USAGE

ABMotorController

%C [options]
options: -f ConfigFile.xml [-c]

ConfigFile.xml is the xml configuration file needed for set up
-c indicates that prints should be enabled for main

#endif	// __USAGE

*/
