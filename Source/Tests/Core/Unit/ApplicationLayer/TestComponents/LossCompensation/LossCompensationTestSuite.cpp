//******************************************************************************
// Description:
//   This is the test suite for the Loss Compensation tests.
//
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/LossCompensation/LossCompensationTestSuite.cpp $
// 
// 1     1/18/07 4:14p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "LossCompensationTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "LossCompensationTest.h"

LossCompensationTestSuite::LossCompensationTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(LossCompensationTest::suite());
}


