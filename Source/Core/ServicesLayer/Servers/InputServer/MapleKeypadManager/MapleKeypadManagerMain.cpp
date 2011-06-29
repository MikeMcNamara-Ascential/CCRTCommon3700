//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a MapleKeypad manager
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/MapleKeypadManager/MapleKeypadManagerMain.cpp $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
//*************************************************************************

#include "MapleKeypadManager.h"


int main(int argc, char *argv[])
{
	InputDeviceBase *mapleKeypad = NULL;	// mapleKeypad object

	try
	{
		printf("Creating The MapleKeypadManager Object\n");
		mapleKeypad = new MapleKeypadManager();    // create a mapleKeypad object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The MapleKeypad Object\n");
		mapleKeypad->Initialize(argc,argv);
		printf("MapleKeypad Object Running\n");
		mapleKeypad->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("MapleKeypadManagerMain: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("MapleKeypadManagerMain: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("MapleKeypadManagerMain: Unknown Exception\n");
	}

	if(mapleKeypad != NULL)	delete mapleKeypad;	// delete the mapleKeypad object
	printf("MapleKeypadManagerMain (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}

#ifdef __USAGE

MapleKeypadManager Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

