//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/Isuzu/ApplicationLayer/TestComponents/Emissions/IsuzuEmissionsTCMain.cpp 
//
// FILE DESCRIPTION:
//  Test Component for testing Isuzu engine controller
//  using Keyword Protocol 2000.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "IsuzuPIMTc.cpp"
#include "GenericEmissionsModuleTemplate.cpp"
#include "KwpCanProtocolFilter.h"
#include "DcxCANKeyword2000ProtocolFilter.h"
#include "FiatCANKeyword2000ProtocolFilter.h"
#include "CmdLineProcessor.h"


int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *emissionsTest = NULL;
	
	try
	{   // Parse the command line arguments
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		emissionsTest = NULL;
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		if (commProtocol == "KeywordProtocol2000") 
		{
			emissionsTest = new IsuzuPIMTc<GenericEmissionsModuleTemplate<KwpCanProtocolFilter> >();
		}
        else if (commProtocol == "DcxCANKeyword2000") 
		{
			emissionsTest = new IsuzuPIMTc<GenericEmissionsModuleTemplate<DcxCANKeyword2000ProtocolFilter> >();
		}
        else if (commProtocol == "FiatCANKeyword2000ProtocolFilter") 
		{
			emissionsTest = new IsuzuPIMTc<GenericEmissionsModuleTemplate<FiatCANKeyword2000ProtocolFilter> >();
		}
		else
		{   // Unknown protocol type
			printf("Could not create Isuzu PIM test component - Unsupported protocol type!\n");
		}
		// Check if we should perform the test
		if (emissionsTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the IsuzuPIMTest Component\n");
			emissionsTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("IsuzuPIMTest Component Running\n");
			emissionsTest->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("IsuzuPIMTest -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("IsuzuPIMTest -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("IsuzuPIMTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("IsuzuPIMTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component
	if (emissionsTest != NULL)  delete emissionsTest;
	emissionsTest = NULL;
	// Return control
	return (1);
}
