//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTestSuite.h 1     7/20/05 8:14p Cward $
//
// FILE DESCRIPTION:
//   This is the test suite for the XmlParser tests.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTestSuite.h $
// 
// 1     7/20/05 8:14p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
// 
//
//*************************************************************************
#ifndef ITCPIPTESTSUITE_H
#define ITCPIPTESTSUITE_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

using namespace std;

class ITcpIpTestSuite : public CppUnit::TestSuite
{
	public:     
        ITcpIpTestSuite();
};

#endif // ITCPIPTESTSUITE_H

