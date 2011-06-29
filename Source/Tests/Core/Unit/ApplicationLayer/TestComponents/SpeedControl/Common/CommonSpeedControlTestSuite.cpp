//******************************************************************************
// Description:
//   This is the test suite for the SpeedControl/Common tests.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/SpeedControl/Common/CommonSpeedControlTestSuite.cpp $
// 
// 1     10/26/06 1:56p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "CommonSpeedControlTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "CommonSpeedControlTest.h"

CommonSpeedControlTestSuite::CommonSpeedControlTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(CommonSpeedControlTest::suite());
}


