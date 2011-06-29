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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerTestSuite.h $
// 
// 2     8/02/06 8:35p Cward
// Updated VehicleDataServer Tests to test for compatibility with older
// builds
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef VEHICLEDATASERVERTESTSUITE_H
#define VEHICLEDATASERVERTESTSUITE_H
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>


class VehicleDataServerTestSuite : public CppUnit::TestSuite
{
    public:
        VehicleDataServerTestSuite();
};

#endif // VEHICLEDATASERVERTESTSUITE_H

