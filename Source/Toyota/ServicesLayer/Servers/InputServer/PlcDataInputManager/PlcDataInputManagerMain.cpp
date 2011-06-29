//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a PLC Data Input manager
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "PlcDataInputManager.h"

int main(int argc, char *argv[])
{
    InputDeviceBase *plcInput = NULL;
    try 
    {
        plcInput = new PlcDataInputManager();
        plcInput->Initialize(argc, argv);
        plcInput->Run();
    }
    catch(XmlException xExcpt)
    {
        printf("PlcDataInputManager - XmlException: %s", xExcpt.GetReason());
    }
    catch(BepException bExcpt)
    {
        printf("PlcDataInputManager - BepException: %s", bExcpt.GetReason());
    }
    catch(...)
    {
        printf("PlcDataInputManager - Unknown exception");
    }

    if(plcInput != NULL)  delete plcInput;
    return 0;
}
