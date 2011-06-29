//*************************************************************************
// Description:
//   Test suite for the tests in the TaskMon directory.
//
//===========================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Managers/TaskMon/TaskMonTestSuite.cpp $
// 
// 1     10/26/06 2:52p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#include "TaskMonTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "Common/CommonTaskMonTestSuite.h"

TaskMonTestSuite::TaskMonTestSuite()
{
    addTest(new CommonTaskMonTestSuite());
}

