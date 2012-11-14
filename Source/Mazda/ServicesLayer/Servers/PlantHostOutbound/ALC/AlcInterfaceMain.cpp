//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a host interface
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostOutbound/PFS/PfsInterfaceMain.cpp $
// 
// 1     3/23/07 3:09p Derickso
// New PFS Interface main program.
// 
//*************************************************************************
#include "ALCInterface.h"

int main(int argc, char *argv[])
{
    ALCInterface *hostInt = NULL;
    try
    {
        hostInt = new ALCInterface();
        hostInt->Initialize(argc, argv);
        hostInt->Run();
    }
    catch(...)
    {   // Nothing to do, just exit
    }
    // Cleanup and go away
    if(hostInt != NULL)  delete hostInt;
    hostInt = NULL;
    return 0;
}
