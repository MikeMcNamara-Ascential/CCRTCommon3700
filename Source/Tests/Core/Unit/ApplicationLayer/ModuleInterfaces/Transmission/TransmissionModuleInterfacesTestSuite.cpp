//******************************************************************************
// Description:
//   This is the test suite for the Tranmsission Module Interface tests.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Transmission/TransmissionModuleInterfacesTestSuite.cpp $
// 
// 1     1/18/07 3:41p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TransmissionModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "TransmissionModuleInterfacesTest.h"

TransmissionModuleInterfacesTestSuite::TransmissionModuleInterfacesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(TransmissionModuleInterfacesTest::suite());
}

