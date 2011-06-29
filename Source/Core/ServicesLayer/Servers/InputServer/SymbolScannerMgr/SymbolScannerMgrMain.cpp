//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to handle a symbol scanner
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/SymbolScannerMgr/SymbolScannerMgrMain.cpp $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
//*************************************************************************

#include "SymbolScannerMgr.h"


int main(int argc, char *argv[])
{
	InputDeviceBase *scanner = NULL;	// Scanner object

	try
	{
		printf("Creating The SymbolScannerMgr Object\n");
		scanner = new SymbolScannerMgr();    // create a scanner object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The Scanner Object\n");
		scanner->Initialize(argc,argv);
		printf("Scanner Object Running\n");
		scanner->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("SymbolScannerMgrObject: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("SymbolScannerMgrObject: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("SymbolScannerMgrObject: Unknown Exception\n");
	}

	if(scanner != NULL)	delete scanner;	// delete the scanner object
	printf("SymbolScannerMgrObject (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}

#ifdef __USAGE

SymbolScannerMgr
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

