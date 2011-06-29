//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/LibraryTestSuite.cpp 3     10/31/07 10:23a Rwiersem $
//
// FILE DESCRIPTION:
//   This is the test suite that will excersize all of the library tests
//   to ensure that they will function properly.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/LibraryTestSuite.cpp $
// 
// 3     10/31/07 10:23a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added GeneralUtilites test suite.
// 
// 2     10/26/06 2:08p Rwiersem
// Changes for the 06102701 core release:
// 
// Added BepServerTest.
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     1/06/05 3:57p Cward
//
// 1     1/12/04 2:54p Khykin
// Initial checkin.
//
//
//*************************************************************************
#include "LibraryTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "BepServerTest.h"
#include "SystemCommunication/SystemCommunicationTestSuite.h"
#include "XmlProcessor/XMLParserTestSuite.h"
#include "Debuggable/DebuggableTestSuite.h"
#include "SoftwareId/SoftwareIdTestSuite.h"
//#include "NotificationThreadSpinner/NotificationThreadSpinnerTestSuite.h"
#include "GeneralUtilities/GeneralUtilitiesTestSuite.h"

LibraryTestSuite::LibraryTestSuite()
{
    addTest(BepServerTest::suite());
    addTest(new SystemCommunicationTestSuite());
    addTest(new XMLParserTestSuite());
    addTest(new DebuggableTestSuite());
    addTest(new SoftwareIdTestSuite());
//  addTest(new NotificationThreadSpinnerTestSuite());
    addTest(new GeneralUtilitiesTestSuite());
}


