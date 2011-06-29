//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a pendant manager
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/PendantManager/PendantManagerMain.cpp $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
//*************************************************************************

#include "PendantManager.h"


int main(int argc, char *argv[])
{
	InputDeviceBase *pendant = NULL;	// pendant object

	try
	{
		printf("Creating The PendantManager Object\n");
		pendant = new PendantManager();    // create a pendant object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The Pendant Object\n");
		pendant->Initialize(argc,argv);
		printf("Pendant Object Running\n");
		pendant->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("PendantManagerMain: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("PendantManagerMain: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("PendantManagerMain: Unknown Exception\n");
	}

	if(pendant != NULL)	delete pendant;	// delete the pendant object
	printf("PendantManagerMain (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}


#ifdef __USAGE

PendantManager
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

