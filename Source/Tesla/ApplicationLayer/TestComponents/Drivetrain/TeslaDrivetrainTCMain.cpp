//******************************************************************************
// FILE:
//    $Header: /CMake GM/Source/Tesla/ApplicationLayer/TestComponents/Drivetrain/TeslaDrivetrainTCMain.cpp 1     4/25/05 9:27a Bmeinke $
//
// FILE DESCRIPTION:
// Tesla specific machine component
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake GM/Source/GM/ApplicationLayer/TestComponents/Drivetrain/TeslaDrivetrainTCMain.cpp $
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for GM
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 1     8/14/03 7:37a Khykin
// Initial checkin after name conversion from TestComponent to TC.
// 
// 1     8/13/03 6:32a Khykin
// Updated with new directory structure changes.
// 
// 1     7/08/03 1:40p Derickso
// Restructured for new directory structure.
// 
// 1     6/24/03 7:08a Khykin
// Initial checkin.  These were done to add stack light functionality.
// 
// 1     6/20/03 9:44a Khykin
// Initial checkin
// 
// 1     6/20/03 9:20a Khykin
// Initial checkin.
// 
// 1     4/22/03 10:30a Khykin
// Initial Working Checkin.
// 
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:04  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 11:13a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:23a Khykin
// Initial Checkin
// 
// 5     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
// 
// 3     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
// 
// 2     3/06/02 2:03p Kh
// Updating for use with TestComponents.
// 
// 1     2/06/02 12:19a Kh
// Initial Checkin.
// 
// 1     1/16/02 3:53p Russ.ross
// Checking in compiling LossCompensation files...
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TeslaDrivetrainTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor	clp;		// command line processor
	GenericTC			*component;	// component object

	try
	{
		clp.ParseArguments(argc, argv);     		// parse the command line
		if(clp.IsDebugOn())	printf("Creating The Drivetrain Component\n");
		component = new TeslaDrivetrainTC();
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The Drivetrain Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The Drivetrain Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("TeslaDrivetrainTC -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("TeslaDrivetrainTC -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("TeslaDrivetrainTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	delete component;

	if(clp.IsDebugOn())
		printf("TeslaDrivetrainTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

}
