//*************************************************************************
// $Header$
//  
//===========================================================================
// Copyright (c) 2012 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// $Log$
//
//*************************************************************************
#include "BepXmlUtils.h"
#include "SystemSetup.h"
#include "CmdLineProcessor.h"
#include "TeslaTestResultServer.h"

/**
 * Main loop for the TeslaTestResultServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // Command line processor
    BepComponent *server = new TeslaTestResultServer(); 

    try
    {
        server->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("TeslaTestResultServer (%d, %s) Up - Listening\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server->Run();
    }
    catch(BepException &BepErr)
    {
        printf("TeslaTestResultServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("TeslaTestResultServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if (server != NULL) delete server;
    server = NULL;
    if(clp.IsDebugOn()) printf("TeslaTestResultServer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}

