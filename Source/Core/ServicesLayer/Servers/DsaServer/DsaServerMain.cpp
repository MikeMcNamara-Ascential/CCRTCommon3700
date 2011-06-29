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

#include "DsaServer.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the DsaServer
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
        server = new DsaServer();  // Create Server
        signal(SIGPIPE, SIG_IGN);
        server->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("DsaServer (%d, %s) Running\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server->Run();  // process data server requests
    }
    catch(BepException &BepErr)
    {
        printf("DsaServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("DsaServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(server) delete server;
    if(clp.IsDebugOn()) printf("DsaServer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}

#ifdef __USAGE

DsaServer
%C [options]
options: -fConfigFile.xml
Where ConfigFile.xml is the xml configuration file needed for setup

#endif	// __USAGE



