//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/SystemCommunicationTestSuite.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the test suite that will excersize all of the System Communication
//  tests to ensure that they will function properly.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/SystemCommunicationTestSuite.cpp $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
//
//*************************************************************************
#include "SystemCommunicationTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "ITcpIp/ITcpIpTestSuite.h"

SystemCommunicationTestSuite::SystemCommunicationTestSuite()
{
	addTest(new ITcpIpTestSuite());
}


