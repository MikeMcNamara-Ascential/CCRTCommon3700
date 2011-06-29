//******************************************************************************
// Description:
//   This is the test suite for the speed control tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/SpeedControl/SpeedControlTestSuite.cpp $
// 
// 1     10/26/06 1:55p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SpeedControlTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonSpeedControlTestSuite.h"

SpeedControlTestSuite::SpeedControlTestSuite()
{
    addTest(new CommonSpeedControlTestSuite());
}


