//******************************************************************************
// Description:
//  Sound level monitoring test component
//
//==============================================================================
// Copyright (c) 2008 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log$
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SoundLevelTc.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
    CmdLineProcessor    clp;        // command line processor
    GenericTC           *component=NULL;    // component object

    try
    {
        clp.ParseArguments(argc, argv);             // parse the command line
        if(clp.IsDebugOn()) printf("Creating The Sound Level Component\n");
        component = new SoundLevelTc;
        // create, initialize, and process messages for the named data broker
        if(clp.IsDebugOn()) printf("Initializing The Sound Level Component\n");
        component->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("Running The Sound Level Component\n");
        component->Run();                           // process until terminated
    }
    catch(XmlException &XmlErr)
    {
        printf("SoundLevelTc -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
    }
    catch(BepException &BepErr)
    {
        printf("SoundLevelTc -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("SoundLevelTc -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }

    if( NULL != component)  delete component;

    if(clp.IsDebugOn())
        printf("SoundLevelTc (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());

}
