//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to test the Delphi 7.4 ABS
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
#include "CmdLineProcessor.h"
#include "MandoAbsTc.cpp"
#include "MandoAbsModule.cpp"
#include "IsoProtocolFilter.h"
#include "KeywordProtocolFilter.h"

int main(int argc, char *argv[])
{   // command line processor
    CmdLineProcessor clp; 
    XmlParser parser;
    GenericTC *brakeTest;
    try
    {   // parse the command line
        clp.ParseArguments(argc, argv);     
        // Set the pointer to NULL
        brakeTest = NULL;
        // Get the config file
        const XmlNode *config = parser.ReturnXMLDocument(clp.GetConfigFile());
        // Determine what type of test component to instantiate
        string commProtocol = config->getChild("Setup/CommunicationProtocol")->getValue();
        if (commProtocol == "KeywordProtocol2000")
        {   // create a Mando ABS component object
            if (clp.IsDebugOn()) printf("Creating The Mando MGH-80 Test Component\n");
            brakeTest = new MandoAbsTc<MandoAbsModule<KeywordProtocolFilter> >;
        }

        else if (commProtocol == "IsoProtocolFilter")
        {
            if (clp.IsDebugOn()) printf("Creating The Mando MGH-60 Test Component\n");
            brakeTest = new MandoAbsTc<MandoAbsModule<IsoProtocolFilter> >;
        }

        else
        {   // Unknown protocol type
            printf("Could not create Mando ABS test component - Unsupported protocol type: %s\n", commProtocol.c_str());
        }
        // Check if we should perform the test
        if (brakeTest != NULL)
        {   // Initialize the new test component
            if (clp.IsDebugOn()) printf("Initializing The Mando ABS Component\n");
            brakeTest->Initialize(clp.GetConfigFile());
            if (clp.IsDebugOn()) printf("Mando ABS Component Running\n");
            // process until terminated
            brakeTest->Run();
        }
    }
    catch (XmlException &XmlErr)
    {
        printf("MandoABS -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
    }
    catch (BepException &BepErr)
    {
        printf("MandoABS -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch (...)
    {
        printf("MandoABS -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if (clp.IsDebugOn())
        printf("Mando 8 ABS (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    // Make sure to delete the test component
    if (brakeTest != NULL)   delete brakeTest;
    brakeTest = NULL;
    // Return to the system
    return 1;
}
