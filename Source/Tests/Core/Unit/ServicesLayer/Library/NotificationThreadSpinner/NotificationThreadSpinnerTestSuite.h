//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTestSuite.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the test suite for the NotificationThreadSpinner tests.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTestSuite.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     5/31/05 8:44a Gswope
// Initial entry into source safe
// 
//
//*************************************************************************
#ifndef NOTIFICATIONTHREADSPINNERTESTSUITE_H
#define NOTIFICATIONTHREADSPINNERTESTSUITE_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

using namespace std;

class NotificationThreadSpinnerTestSuite : public CppUnit::TestSuite
{
	public:
        NotificationThreadSpinnerTestSuite();
};

#endif // NOTIFICATIONTHREADSPINNERTESTSUITE_H

