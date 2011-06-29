//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverMain.cpp 1     5/01/07 1:10p Rwiersem $
//
//===========================================================================
// Copyright (c) 2005-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverMain.cpp $
// 
// 1     5/01/07 1:10p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//*************************************************************************

#include "CmdLineProcessor.h"
#include "SystemScreenSaver.h"

/**
 * Main loop for the SystemScreenSaver
 *
 * @param argc   number of command line arguments
 * @param argv   Command line arguments
 * @return Exit status of process
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // command line processor
    BepComponent *server;               // Server object
    try
    {
        if(clp.IsDebugOn()) printf("Creating The SystemScreenSaver\n");
        server = new SystemScreenSaver();       // create a VehicleData
        if(clp.IsDebugOn()) printf("Initializing The SystemScreenSaver\n");
        server->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn())  printf("SystemScreenSaver (%d, %s) Up - Listening\nRunning The SystemScreenSaver\n",
                                    BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server->Run();                          // process until terminated
    }
    catch(BepException &BepErr)
    {
        printf("SystemScreenSaver -%s: BepException: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("SystemScreenSaver -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(clp.IsDebugOn()) printf("Deleting The SystemScreenSaver\n");
    delete server;
    if(clp.IsDebugOn())  printf("SystemScreenSaver (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
}

#ifdef __USAGE

SystemScreenSaver Version 1.0
%C [options]
options:
    -f      Configuration File (Ex. VehicleTestConfig.xml)
    -vx     Verbosity flag, set to level x

notes:
    - After Launcher This Must Be Provided The Register Command.
    - $Revision: 1 $
    - $Date: 5/01/07 1:10p $

#endif  // __USAGE

