//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to handle a symbol scanner
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MahindraScannerManager.h"


int main(int argc, char *argv[])
{
	MahindraScannerManager *scanner = NULL;	// Scanner object

	try
	{
		printf("Creating The MahindraSymbolScannerMgr Object\n");
		scanner = new MahindraScannerManager();    // create a scanner object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The Scanner Object\n");
		scanner->Initialize(argc,argv);
		printf("Scanner Object Running\n");
		scanner->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("MahindraSymbolScannerMgrObject: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("MahindraSymbolScannerMgrObject: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("MahindraSymbolScannerMgrObject: Unknown Exception\n");
	}

	if(scanner != NULL)	delete scanner;	// delete the scanner object
	printf("MahindraSymbolScannerMgrObject (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}

