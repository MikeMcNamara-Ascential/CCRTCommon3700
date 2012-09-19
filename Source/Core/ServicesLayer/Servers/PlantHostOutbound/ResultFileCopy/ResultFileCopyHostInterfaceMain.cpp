//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a host interface
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "ResultFileCopyHostInterface.h"

int main(int argc, char *argv[])
{
    ResultFileCopyHostInterface *hostInt = NULL;
    try
    {
        hostInt = new ResultFileCopyHostInterface();
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
