//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Main loop for a ChinaPlantHostInbound.  
//  
//===========================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
// 
//*************************************************************************

#include "./Include/ChinaPlantHostInbound.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the ChinaPlantHostInbound
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // Command line processor
    try
    {
        ChinaPlantHostInbound server;        // Server object
        server.Initialize(clp.GetConfigFile()); // Initialize server
        if(clp.IsDebugOn()) printf("PlantHostInbound(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server.Run();
    }
    catch(BepException &BepErr)
    {
        printf("ChinaPlantHostInbound -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("ChinaPlantHostInbound -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(clp.IsDebugOn()) printf("ChinaPlantHostInbound(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}

#ifdef __USAGE

ChinaPlantHostInbound Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed for set up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif  // __USAGE
