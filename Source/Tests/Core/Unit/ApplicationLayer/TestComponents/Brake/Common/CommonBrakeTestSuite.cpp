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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/CommonBrakeTestSuite.cpp $
// 
// 1     1/18/07 4:04p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "CommonBrakeTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "BaseBrakeToolTest.h"
#include "GenericABSTCTemplateTest.h"

CommonBrakeTestSuite::CommonBrakeTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(BaseBrakeToolTest::suite());
    addTest(GenericABSTCTemplateTest::suite());
}


