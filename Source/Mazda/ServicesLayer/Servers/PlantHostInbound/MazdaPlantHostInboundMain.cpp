//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Main loop for a MazdaPlantHostInbound.  
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

#include "MazdaPlantHostInbound.h"
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "BepException.h"
#include "CmdLineProcessor.h"
#include "IBepCommunication.h"

/**
 * Main loop for the MazdaPlantHostInbound
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // Command line processor
    try
    {
        MazdaPlantHostInbound server;        // Server object
        server.Initialize(clp.GetConfigFile()); // Initialize server
        if(clp.IsDebugOn()) printf("PlantHostInbound(%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server.Run();
    }
    catch(BepException &BepErr)
    {
        printf("MazdaPlantHostInbound -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("MazdaPlantHostInbound -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(clp.IsDebugOn()) printf("MazdaPlantHostInbound(%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}
