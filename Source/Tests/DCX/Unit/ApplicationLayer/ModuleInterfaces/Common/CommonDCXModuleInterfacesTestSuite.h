//******************************************************************************
// Description:
//       Runs all DCX specific common module interface tests
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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/CommonDCXModuleInterfacesTestSuite.h $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonDCXModuleInterfacesTestSuite_h
#define CommonDCXModuleInterfacesTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>

class CommonDCXModuleInterfacesTestSuite : public CppUnit::TestSuite
{
    public:
        CommonDCXModuleInterfacesTestSuite();
};

#endif // CommonModuleInterfacesTestSuite_h

