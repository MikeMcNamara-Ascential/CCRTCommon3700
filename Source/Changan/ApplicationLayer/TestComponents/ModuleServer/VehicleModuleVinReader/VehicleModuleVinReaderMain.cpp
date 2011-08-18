//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a CLV295 Scanner
// manager
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/ModuleServer/VehicleModuleVinReader/VehicleModuleVinReaderMain.cpp $
// 
// 1     11/13/06 3:54p Bbarrett
// Add Module VIN Reader.
// 
// 1     8/29/06 12:26p Jsemann
// 
// 1     3/03/06 12:32p Jsemann
// 3/3/06
// 
// 1     10/31/05 1:22p Jsemann
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
//*************************************************************************

#include "VehicleModuleVinReader.h"


int main(int argc, char *argv[])
{
	InputDeviceBase *scanner = NULL;	// scanner object

	try
	{
		printf("Creating The VehicleModuleVinReader Object\n");
		scanner = new VehicleModuleVinReader();    // create a scanner object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The VehicleModuleVinReader Object\n");
		scanner->Initialize(argc,argv);
		printf("VehicleModuleVinReader Object Running\n");
		scanner->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("VehicleModuleVinReaderMain: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("VehicleModuleVinReaderMain: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("VehicleModuleVinReaderMain: Unknown Exception\n");
	}

	if(scanner != NULL)	delete scanner;	// delete the scanner object
	printf("VehicleModuleVinReaderMain (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}

#ifdef __USAGE

VehicleModuleVinReader
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

