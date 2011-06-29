//******************************************************************************
// Description:
//   This is the test suite for the Common/Machine tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Machine/Common/CommonMachineTestSuite.h $
// 
// 1     9/06/06 10:17a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonMachineTestSuite_h
#define CommonMachineTestSuite_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>


class CommonMachineTestSuite : public CppUnit::TestSuite
{
    public:
        CommonMachineTestSuite();
};

#endif // CommonMachineTestSuite_h

