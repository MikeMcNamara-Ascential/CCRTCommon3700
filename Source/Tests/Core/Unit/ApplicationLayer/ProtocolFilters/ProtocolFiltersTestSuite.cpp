//******************************************************************************
// Description:
//   This is the test suite for the protocol filter tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ProtocolFilters/ProtocolFiltersTestSuite.cpp $
// 
// 1     10/26/06 1:43p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ProtocolFiltersTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonProtocolFiltersTestSuite.h"

ProtocolFiltersTestSuite::ProtocolFiltersTestSuite()
{
    addTest(new CommonProtocolFiltersTestSuite());
}


