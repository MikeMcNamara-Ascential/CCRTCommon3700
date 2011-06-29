//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTestSuite.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the ThreadSpinner test that will excersize the NotificationThreadSpinner  
//	 to ensure that it will function properly.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTestSuite.cpp $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     5/31/05 8:44a Gswope
// Initial entry into source safe
// 
//
//*************************************************************************
#include "NotificationThreadSpinnerTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "NotificationThreadSpinnerTest.h"

NotificationThreadSpinnerTestSuite::NotificationThreadSpinnerTestSuite()
{
	// Add the inidividual test fixtures in this directory
	addTest(NotificationThreadSpinnerTest::suite());
}


