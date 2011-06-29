//******************************************************************************
// Description:
//   This is the test suite for the Vehicle Data Server tests.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerTestSuite.cpp $
// 
// 2     8/02/06 8:35p Cward
// Updated VehicleDataServer Tests to test for compatibility with older
// builds
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "VehicleDataServerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "VehicleDataServerTest.h"

VehicleDataServerTestSuite::VehicleDataServerTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(VehicleDataServerTest::suite());
}


