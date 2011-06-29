//******************************************************************************
// Description:
//   This is the test suite for the Brake/Common tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/CommonBrakeTestSuite.h $
// 
// 1     1/18/07 4:04p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonBrakeTestSuite_h
#define CommonBrakeTestSuite_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>


class CommonBrakeTestSuite : public CppUnit::TestSuite
{
    public:
        CommonBrakeTestSuite();
};

#endif // CommonBrakeTestSuite_h

