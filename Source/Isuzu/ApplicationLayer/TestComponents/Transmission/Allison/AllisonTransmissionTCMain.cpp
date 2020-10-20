//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/Transmission/ALLISON/AllisonransmissionTCMain.cpp 1     3/29/07 1:51p Mkelly $
//
// FILE DESCRIPTION:
//  Test Component for testing DaimlerChrysler A580 Transmission on CAN bus
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/Transmission/ALLISON/AllisonTransmissionTCMain.cpp $
// 
//
//*************************************************************************
#include "AllisonTransmissionTC.cpp"
#include "CmdLineProcessor.h"
#include "AllisonTransmissionModule.cpp"
#include "KwpCanProtocolFilter.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor clp;				// command line processor
	XmlParser parser;
	GenericTC *transmissionTest;

	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
		// Set the pointer to NULL
		transmissionTest = NULL;
		// Get the config file
		const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
		// Determine what type of test component to instantiate
		string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
		if (commProtocol == "KeywordProtocol2000") 
		{	// Create a test component
			transmissionTest = new AllisonTransmissionTC<AllisonTransmissionModule<KwpCanProtocolFilter> >;
		}
		else
		{   // Unknown protocol type
			printf("Could not create Allison Transmission test component - Unsupported protocol type!\n");
		}
		// Check if we should perform the test
		if (transmissionTest != NULL)
		{	// Initialize the new test component
			if(clp.IsDebugOn())	printf("Initializing the AllisonKwpTransmissionTest Component\n");
			transmissionTest->Initialize(clp.GetConfigFile());
			// Start the test running
			if(clp.IsDebugOn())	printf("AllisonKwpTransmissionTest Component Running\n");
			transmissionTest->Run();							// process until terminated
		}
	}
	catch(XmlException &XmlErr)
	{
		printf("AllisonKwpTransmissionTest -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("AllisonKwpTransmissionTest -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("AllisonKwpTransmissionTest -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}
	printf("AllisonKwpTransmissionTest (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// Get rid of the test component
	if (transmissionTest != NULL)  delete transmissionTest;
	transmissionTest = NULL;
	// Return control
	return (1);
}
