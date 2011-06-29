//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Brake/BaseBrake/GenericBaseBrakeTCMain.cpp 2     5/08/06 7:00p Cward $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to calibrate the rolls
// tester.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Brake/BaseBrake/GenericBaseBrakeTCMain.cpp $
// 
// 2     5/08/06 7:00p Cward
// conditioned printf based on debug option
//
// 1     12/21/03 6:33p Cward
//
// 1     9/24/03 6:30a Derickso
// New Base Brake test to fit in Templatized architecture.
//
// 1     8/21/03 8:58a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:41a Khykin
// Initial checkin after name conversion from TestComponent to TC.
//
// 2     8/08/03 11:13a Khykin
// Added a return value.
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//
// 1     6/20/03 9:45a Khykin
// Initial checkin
//
// 1     6/20/03 9:20a Khykin
// Initial checkin.
//
// 1     4/22/03 10:28a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:00  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:12a Khykin
// Initial Checkin
//
// 1     1/18/03 7:20a Khykin
// Initial Checkin
//
// 5     8/23/02 8:03a Kh
// Synchronizing the pit machine with VSS.
//
// 3     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
//
// 2     3/06/02 1:58p Kh
// Updating for using with TestComponents.
//
// 1     2/06/02 12:15a Kh
// Initial checkin.
//
//
//*************************************************************************

#include "GenericBaseBrakeTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	GenericTC *component = NULL;	// Component object

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		if(clp.IsDebugOn())	printf("Creating The BaseBrakeTC Component\n");
		component = new GenericBaseBrakeTC();    // create a loss compensation component component
		// create, initialize, and process messages for the named data broker
		if(clp.IsDebugOn())	printf("Initializing The BaseBrakeTestComponent Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("BaseBrakeTestComponent Component Running\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("BaseBrakeTestComponent -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("BaseBrakeTestComponent -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("BaseBrakeTestComponent -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(component != NULL)	delete component;	// delete the component object
	if(clp.IsDebugOn())
		printf("BaseBrakeTestComponent (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

	return(0);
}
