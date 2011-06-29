//******************************************************************************
// Description:
//   This is the test suite for the Brake Module Interface tests.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Brake/BrakeModuleInterfacesTestSuite.cpp $
// 
// 1     10/26/06 1:35p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "BrakeModuleInterfacesTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "GenericABSModuleTemplateTest.h"

BrakeModuleInterfacesTestSuite::BrakeModuleInterfacesTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(GenericABSModuleTemplateTest::suite());
}


