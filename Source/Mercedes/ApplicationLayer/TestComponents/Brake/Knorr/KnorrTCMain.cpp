//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Knorr/KnorrTCMain.cpp 
//
// FILE DESCRIPTION:
// This file contains the functions necessary to test the Waboc 
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
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Knorr/KnorrTCMain.cpp $
// 
// 
//*************************************************************************

#include "KnorrTC.cpp"
#include "CmdLineProcessor.h"
#include "KnorrModule.cpp"
#include "KeywordProtocolFilter.h"
#include "KwpCanProtocolFilter.h"

int main(int argc, char *argv[])
{   // command line processor
    CmdLineProcessor clp;				
	XmlParser parser;
	GenericTC *brakeTest;
	
	try
	{   // parse the command line
        clp.ParseArguments(argc, argv);
		// Set the pointer to NULL
		brakeTest = NULL;
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		string commBus = config->getChild("Setup/CommunicationBus")->getValue();
		if (commProtocol == "KeywordProtocol2000") 
		{   // Select the correct protocol filter for the bus we are using
			if (!commBus.compare("ISO-K"))
			{   // Running on ISO-K bus
				brakeTest = new KnorrTC<KnorrModule<KeywordProtocolFilter> >;
			}
			else
			{   // We are running on CAN bus
				brakeTest = new KnorrTC<KnorrModule<KwpCanProtocolFilter> >;
			}
		}
		else
		{   // Unknown protocol type
			printf("Could not create Knorr test component - Unsupported protocol type %s!\n", commProtocol.c_str());
		}
		// Check if we should perform the test
		if (brakeTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing The Knorr Component\n");
			brakeTest->Initialize(clp.GetConfigFile());
			if(clp.IsDebugOn())	printf("Knorr Component Running\n");
			// process until terminated
			brakeTest->Run();							
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("KnorrTC -%s: XmlException: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("WabocTC -%s: BepException: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("KnorrTC -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
    if(clp.IsDebugOn())
		printf("KnorrTC (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Make sure to delete the test component
	if (brakeTest != NULL)   delete brakeTest;
	brakeTest = NULL;
	// Return to the system
	return 1;
}
