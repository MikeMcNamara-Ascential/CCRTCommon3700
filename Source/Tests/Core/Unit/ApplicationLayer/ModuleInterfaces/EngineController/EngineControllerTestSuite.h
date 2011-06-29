//******************************************************************************
// Description:
//   This is the test suite for the Engine Controller Module Interface tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/EngineController/EngineControllerTestSuite.h $
// 
// 1     9/06/06 10:11a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef EngineControllerTestSuite_h
#define EngineControllerTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class EngineControllerTestSuite : public CppUnit::TestSuite
{
    public:
        EngineControllerTestSuite();
};

#endif // EngineControllerTestSuite_h

