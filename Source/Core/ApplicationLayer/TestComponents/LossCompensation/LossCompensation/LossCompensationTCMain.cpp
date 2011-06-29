//*************************************************************************
// FILE:
//    $Header: /Core/ApplicationLayer/TestComponents/LossCompensation/LossCompensation/LossCompensationTCMain.cpp 1     12/21/03 6:33p Cward $
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
//    $Log: /Core/ApplicationLayer/TestComponents/LossCompensation/LossCompensation/LossCompensationTCMain.cpp $
// 
// 1     12/21/03 6:33p Cward
// 
// 2     9/22/03 5:35p Derickso
// Updated to use GenericTC instead of TestComponent.
// 
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
// 
// 1     8/14/03 7:53a Khykin
// Updated with new naming convention from TestComponent to TC.
// 
// 2     8/06/03 1:11p Khykin
// Updating to return a value.
// 
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
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
//    Revision 1.1.1.1  2003/02/15 00:03:03  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 11:13a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:22a Khykin
// Initial Checkin
// 
// 5     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
// 
// 3     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
// 
// 2     3/06/02 2:00p Kh
// Updating main for use with TestComponents.
// 
// 1     1/16/02 3:53p Russ.ross
// Checking in compiling LossCompensation files...
// 
// 
//*************************************************************************

#include "LossCompensationTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;		// command line processor
	GenericTC *component;	// Component object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		if(clp.IsDebugOn())	printf("Creating The LossCompensationTC Component\n");
		component = new LossCompensationTC();    // create a loss compensation component
		if(clp.IsDebugOn())	printf("Initializing The LossCompensationTC Component\n");
		component->Initialize(clp.GetConfigFile());
		if(clp.IsDebugOn())	printf("Running The LossCompensationTC Component\n");
		component->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("LossCompensationTC -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("LossCompensationTC -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("LossCompensationTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(component != NULL)	delete component;	// delete the component object
	component = NULL;
	if(clp.IsDebugOn())
		printf("LossCompensationTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

	return(0);
};
