//******************************************************************************
// Description:
//    System screen saver test suite.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverTestSuite.cpp $
// 
// 1     5/01/07 1:48p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SystemScreenSaverTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "SystemScreenSaverTest.h"

SystemScreenSaverTestSuite::SystemScreenSaverTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(SystemScreenSaverTest::suite());
}

