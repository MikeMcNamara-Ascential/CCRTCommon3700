//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  Main loop for Dsa Server.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************

#include "Gen3Interface.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the Gen3Interface
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // Command line processor
    BepComponent* server = NULL;        // Server
    try
    {
        server = new Gen3Interface();  // Create Server
        signal(SIGPIPE, SIG_IGN);
        server->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("Gen3Interface (%d, %s) Running\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server->Run();  // process data server requests
    }
    catch(BepException &BepErr)
    {
        printf("Gen3Interface -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("Gen3Interface -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(server) delete server;
    if(clp.IsDebugOn()) printf("Gen3Interface (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}

