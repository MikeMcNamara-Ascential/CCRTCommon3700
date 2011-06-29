//******************************************************************************
// FILE:
//
// FILE DESCRIPTION:
// This file contains the functions necessary to calibrate the rolls
// tester.
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
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

#include "CmdLineProcessor.h"
#include "PhotonLossCompensation.h"

int main(int argc, char *argv[])
{
	CmdLineProcessor 	clp;		// command line processor
    PhotonLossCompensation *object = NULL;	// object
	
	try
	{
		clp.ParseArguments(argc, argv);     // parse the command line
        object = new PhotonLossCompensation();
		if(clp.IsDebugOn())	printf("Creating the Photon Loss Comp Component\n");

		if(clp.IsDebugOn())	printf("Initializing the Photon Loss Comp Component\n");
		object->Initialize(clp.GetConfigFile());

		if(clp.IsDebugOn())	printf("Running the Photon Loss Comp Component\n");
		object->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("Photon Loss Comp -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Photon Loss Comp -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Photon Loss Comp -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

	if(clp.IsDebugOn())
		printf("Photon Loss Comp (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

    return 0;
};
