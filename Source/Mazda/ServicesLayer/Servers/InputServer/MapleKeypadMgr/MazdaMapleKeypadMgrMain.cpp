//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a 
// Mazda Maple Keypad Input manager
//
//===========================================================================
// Copyright (c) 2013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MazdaMapleKeypadMgr.h"

int main(int argc, char *argv[])
{
    InputDeviceBase *mazdaMapleInput = NULL;
    try 
    {
        mazdaMapleInput = new MazdaMapleKeypadMgr();
        mazdaMapleInput->Initialize(argc, argv);
        mazdaMapleInput->Run();
    }
    catch(XmlException xExcpt)
    {
        printf("MazdaMaplKeypadMgr - XmlException: %s", xExcpt.GetReason());
    }
    catch(BepException bExcpt)
    {
        printf("MazdaMaplKeypadMgr - BepException: %s", bExcpt.GetReason());
    }
    catch(...)
    {
        printf("MazdaMaplKeypadMgr - Unknown exception");
    }

    if(mazdaMapleInput != NULL)  delete mazdaMapleInput;
    return 0;
}
