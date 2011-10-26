//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Bosch8TCMain.cpp 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to test the Bosch 8 ABS.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Bosch8TCMain.cpp $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// Changed Include from KeywordProtocolFilter.h to KeywordProtocolFilter.cpp
// so templates will work from the cpp file.
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
//===========================================================================
// Bosch8 test component.
// Bus:       ISO
// Protocol:  Keyword 2000 Protocol
// Customer:  HMMA
//*************************************************************************

#include "Bosch8TC.cpp"
#include "CmdLineProcessor.h"
#include "../../../ModuleInterfaces/Brake/Bosch8/Bosch8Module.cpp"
#include "KeywordProtocolFilter.cpp"

int main(int argc, char* argv[])
{
    CmdLineProcessor clp;

    try
    {
        clp.ParseArguments(argc,argv); // parse the command line
        if(clp.IsDebugOn()) printf("Creating the Bosch8 Test Component\n");
        // create a Bosch8 component object
        Bosch8TC<Bosch8Module<KeywordProtocolFilter> >bosch8Test;
        if(clp.IsDebugOn()) printf("Initializing the Bosch8 component\n");
        bosch8Test.Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("Bosch8 component running\n");
        bosch8Test.Run();              // process until terminated
    }
    catch(XmlException &XmlError)
    {
        printf("Bosch8TC -%s: XmlException: %s\n",clp.GetConfigFile().c_str(),
               XmlError.what());
    }
    catch(BepException &BepError)
    {
        printf("Bosch8TC -%s: BepException: %s\n",clp.GetConfigFile().c_str(),
               BepError.what());
    }
    catch(...)
    {
        printf("Bosch8TC -%s: Unknown Exception\n",clp.GetConfigFile().c_str());
    }
    if(clp.IsDebugOn())
    {
        printf("Bosch8TC (%d, %s): Terminating\n",BposGetMyTaskId(),
               clp.GetConfigFile().c_str());
    }
}

