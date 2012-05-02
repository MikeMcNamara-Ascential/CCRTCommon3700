//*************************************************************************
// FILE DESCRIPTION:
//    System Monitor Task
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MahindraSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    SystemMonitor *sysMon = NULL;
    try
    {
        sysMon = new MahindraSystemMonitor(argc, argv);
        sysMon->Run();
    }
    catch(BepException &excpt)
    {
        printf("BepException in MahindraSystemMonitor: %s", excpt.GetReason());
    }
    if(sysMon != NULL)  delete sysMon;
    return 0;
}
