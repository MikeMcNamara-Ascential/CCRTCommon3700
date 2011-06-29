//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for testing Isuzu speed control
//  using Keyword Protocol 2000.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "GenericSpeedControlTC.cpp"
#include "GenericEmissionsModuleTemplate.cpp"
#include "KwpCanProtocolFilter.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *speedControl = NULL;
	
	try
	{   // Parse the command line arguments
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		speedControl = NULL;
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		if (commProtocol == "KeywordProtocol2000") 
		{
			speedControl = new GenericSpeedControlTC<GenericEmissionsModuleTemplate<KwpCanProtocolFilter> >();
		}
		else
		{   // Unknown protocol type
			printf("Could not create Isuzu Speed Control test component - Unsupported protocol type!\n");
		}
		// Check if we should perform the test
		if (speedControl != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the Isuzu Speed Control Test Component\n");
			speedControl->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("Isuzu Speed Control Test Component Running\n");
			speedControl->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("Isuzu Speed Control Test -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Isuzu Speed Control Test -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Isuzu Speed Control Test -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("Isuzu Speed Control Test (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the test component
	if (speedControl != NULL)  delete speedControl;
	speedControl = NULL;
	// Return control
	return (1);
}
