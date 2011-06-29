//******************************************************************************
// Description:
//   This is the test suite for the Common Module Interface tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/CommonModuleInterfacesTestSuite.h $
// 
// 1     9/22/06 11:26a Rwiersem
// Changes for the 06092201 core release:
//     - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonModuleInterfacesTestSuite_h
#define CommonModuleInterfacesTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class CommonModuleInterfacesTestSuite : public CppUnit::TestSuite
{
    public:
        CommonModuleInterfacesTestSuite();
};

#endif // CommonModuleInterfacesTestSuite_h

