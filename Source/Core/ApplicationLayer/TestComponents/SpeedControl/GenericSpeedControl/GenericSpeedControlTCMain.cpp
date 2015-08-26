//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/TestComponents/SpeedControl/GenericSpeedControl/GenericSpeedControlTCMain.cpp 2     5/08/06 7:12p Cward $
//
// FILE DESCRIPTION:
//  Test Component for testing Generic Speed Control systems.
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
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/SpeedControl/GenericSpeedControl/GenericSpeedControlTCMain.cpp $
// 
// 2     5/08/06 7:12p Cward
// conditioned printf statement based on debug option in the command line
// arguments.
//
// 1     2/15/06 6:47p Cward
// Add stand alone Generic Speed Control
//
//*************************************************************************
#include "GenericSpeedControlTC.cpp"
#include "GenericEmissionsModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"
#include "KwpCanProtocolFilter.cpp"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *speedControlTest;

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		speedControlTest = NULL;
		// Create a test component
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commBus = config->getChild("Setup/CommunicationBus")->getValue();
		if(commBus == "CAN")
		{
			speedControlTest = new GenericSpeedControlTC<GenericEmissionsModuleTemplate<KwpCanProtocolFilter> >;
		}
		else
		{
			speedControlTest = new GenericSpeedControlTC<GenericEmissionsModuleTemplate<KeywordProtocolFilter> >;
		}
		// Check if we should perform the test
		if (speedControlTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the SpeedControlTest Component\n");
			speedControlTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("SpeedControlTest Component Running\n");
			speedControlTest->Run();					// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("SpeedControlTest -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("SpeedControlTest -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("SpeedControlTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	if(clp.IsDebugOn())	printf("SpeedControlTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Delete the speed control test
	if (speedControlTest != NULL)  delete speedControlTest;
	speedControlTest = NULL;
	// Return control
	return (1);
}
