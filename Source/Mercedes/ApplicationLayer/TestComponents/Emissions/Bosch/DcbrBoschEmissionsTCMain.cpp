//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Emissions/DcbrBosch/DcbrBoschEmissionsTCMain.cpp 1     2/22/05 2:17p Derickso $
//
// FILE DESCRIPTION:
//  Test Component for testing DaimlerChrysler NGC-III engine controller
//  using Keyword Protocol 2000.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Emissions/DcbrBosch/DcbrBoschEmissionsTCMain.cpp $
// 
//*************************************************************************
#include "BoschEmissionsModule.cpp"
#include "DcbrBoschEmissionsTC.cpp"
#include "KeywordProtocolFilter.h"
#include "KwpCanProtocolFilter.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *emissionsTest;
	
	try
	{   // Parse the command line arguments
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		emissionsTest = NULL;
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		string commBus = config->getChild("Setup/CommunicationBus")->getValue();
		if (commProtocol == "KeywordProtocol2000") 
		{   // Select the correct protocol filter for the bus we are using
			if (!commBus.compare("ISO-K"))
			{   // Running on ISO-K bus
				emissionsTest = new DcbrBoschEmissionsTC<BoschEmissionsModule<KeywordProtocolFilter> >;
			}
			else
			{   // We are running on CAN bus
				emissionsTest = new DcbrBoschEmissionsTC<BoschEmissionsModule<KwpCanProtocolFilter> >;
			}
		}
		else
		{   // Unknown protocol type
			printf("Could not create DCBR Bosch Emissions test component - Unsupported protocol type!\n");
		}
		// Check if we should perform the test
		if (emissionsTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the DcbrBoschEmissionsTest Component\n");
			emissionsTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("DcbrBoschEmissionsTest Component Running\n");
			emissionsTest->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("DcbrBoschEmissionsTest -%s: XmlException: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("DcbrBoschEmissionsTest -%s: BepException: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("DcbrBoschEmissionsTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	printf("DcbrBoschEmissionsTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component
	if (emissionsTest != NULL)  delete emissionsTest;
	emissionsTest = NULL;
	// Return control
	return (1);
}
