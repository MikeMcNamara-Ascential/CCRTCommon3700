//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/DataServer/DataServerMain.cpp 2     4/13/06 4:12a Cward $
//
// FILE DESCRIPTION:
//  Main loop for Data Server.  This will be the gateway from the GUI to the realtime system.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/DataServer/DataServerMain.cpp $
// 
// 2     4/13/06 4:12a Cward
// cleanup usage block
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:42p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:23a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:02  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:31a Khykin
// Initial Checkin
//
// 1     1/18/03 7:59a Khykin
// Initial Checkin From DCTC
//
// 12    7/31/02 10:59a Skypig13
// Updated version information
//
// 10    4/29/02 11:26p Skypig13
// Removed signal handler for SIGINT and added SIGPIPE
//
// 9     4/25/02 4:38p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 7     4/02/02 10:53p Skypig13
// New Data server tested and working
//
// 6     2/05/02 12:20a Skypig13
// Update to new BepCommunication structure
//
// 5     12/13/01 2:58p Skypig13
// Fixed diagnostic when execption is thrown
//
// 4     12/13/01 10:28a Skypig13
// Condtioined server up message
//
// 3     12/12/01 11:03p Skypig13
// Update to use Run() method from BepServer()
//
// 2     10/12/01 11:24a Skypig13
// Latest files for DataServer
//
// 1     10/05/01 4:53p Skypig13
// Main loop for data server
//
//*************************************************************************

#include "DataServer.h"
#include "CmdLineProcessor.h"

/**
 * Main loop for the DataServer
 *
 * @param argc   number of command line argumetns
 * @param argv   Command line argumetns
 * @since Version 1.0
 */
int main(int argc, char *argv[])
{
    CmdLineProcessor clp(argc, argv);   // Command line processor
    BepComponent* server = NULL;        // Server
    try
    {
        server = new DataServer();  // Create Server
        signal(SIGPIPE, SIG_IGN);
        server->Initialize(clp.GetConfigFile());
        if(clp.IsDebugOn()) printf("DataServer (%d, %s) Running\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
        server->Run();  // process data server requests
    }
    catch(BepException &BepErr)
    {
        printf("DataServer -%s: Exception: %s\n", clp.GetConfigFile().c_str(), BepErr.what());
    }
    catch(...)
    {
        printf("DataServer -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
    }
    if(server) delete server;
    if(clp.IsDebugOn()) printf("DataServer (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
    return(0);
}

#ifdef __USAGE

DataServer
%C [options]
options: -fConfigFile.xml
Where ConfigFile.xml is the xml configuration file needed for set up

#endif	// __USAGE



