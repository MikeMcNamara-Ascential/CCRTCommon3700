//******************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to test the RWAL sensor.
//
//==============================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "MahindraTransmission.cpp"
#include "CmdLineProcessor.h"
//#include "GenericTransmissionModuleTemplate.cpp"
#include "../../../ModuleInterfaces/Transmission/GenericTransmissionModuleTemplate.cpp"
#include "KwpCanProtocolFilter.cpp" //Grabbed a 11-bit KwpCanProtocolFilter.cpp 
#include "DcxCANKeyword2000ProtocolFilter.cpp"
#include "KeywordProtocolFilter.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    GenericTC *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        object = new MahindraTransmission<GenericTransmissionModuleTemplate<KwpCanProtocolFilter> >();
		if(clp.IsDebugOn())	printf("Creating the Mahindra Transmission Component\n");

		if(clp.IsDebugOn())	printf("Initializing the Mahindra Transmission Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the Mahindra Transmission Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("RWAL Brake -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("RWAL Brake -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("RWAL Brake -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("RWAL Brake (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
};
