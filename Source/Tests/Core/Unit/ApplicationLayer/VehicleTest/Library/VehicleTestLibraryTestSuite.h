//******************************************************************************
// Description:
//   This is the test suite for the Vehicle Test Library tests.
//
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/VehicleTest/Library/VehicleTestLibraryTestSuite.h $
// 
// 1     10/26/06 1:59p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef VehicleTestLibraryTestSuite_h
#define VehicleTestLibraryTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class VehicleTestLibraryTestSuite : public CppUnit::TestSuite
{
    public:
        VehicleTestLibraryTestSuite();
};

#endif // VehicleTestLibraryTestSuite_h

