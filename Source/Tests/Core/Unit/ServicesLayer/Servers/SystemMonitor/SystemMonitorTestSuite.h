//******************************************************************************
// Description:
//    System Monitor test suite.
//
//==============================================================================
//
//     Copyright (c) 2006 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemMonitor/SystemMonitorTestSuite.h $
// 
// 1     1/18/07 5:10p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SystemMonitorTestSuite_h
#define SystemMonitorTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

class SystemMonitorTestSuite : public CppUnit::TestSuite
{
    public:
        SystemMonitorTestSuite();
};

#endif // SystemMonitorTestSuite_h

