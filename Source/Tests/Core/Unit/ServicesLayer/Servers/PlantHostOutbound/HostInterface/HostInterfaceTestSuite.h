//******************************************************************************
// Description:
//    Host interface test suite.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/HostInterface/HostInterfaceTestSuite.h $
// 
// 1     5/01/07 1:46p Rwiersem
// Changes for the 07050201 core release:
// 
// Intial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef HostInterfaceTestSuite_h
#define HostInterfaceTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

class HostInterfaceTestSuite : public CppUnit::TestSuite
{
public:
    HostInterfaceTestSuite();
};

#endif 
