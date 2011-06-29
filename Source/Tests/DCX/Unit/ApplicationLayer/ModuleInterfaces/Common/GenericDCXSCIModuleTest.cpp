//******************************************************************************
// Description:
//       Runs all DCX SCI module interface tests
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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericDCXSCIModuleTest.cpp $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericDCXSCIModule.cpp"
#include "GenericDCXSCIModuleTest.h"
#include "MockProtocolFilter.h"

//=============================================================================
GenericDCXSCIModuleTest::GenericDCXSCIModuleTest(void)
{
}

//=============================================================================
GenericDCXSCIModuleTest::~GenericDCXSCIModuleTest(void)
{
}

//=============================================================================
void GenericDCXSCIModuleTest::setUp()
{
}

//=============================================================================
void GenericDCXSCIModuleTest::tearDown()
{
}

//=============================================================================
void GenericDCXSCIModuleTest::ExceptionTest(void)
{
    // Here be dragons
    UINT8 xThrown = 0;
    FaultVector_t dummy;
    GenericDCXSCIModule gdsm(NULL);

    // Test to make sure that the exception is properly thrown when unsupported communication methods are called
    xThrown = 0;
    try
    {
        gdsm.ReadBitEncodedFaultRegisters(dummy);
    }
    catch(ModuleException &e)
    {
        xThrown = 1;
    }
    CPPUNIT_ASSERT_MESSAGE("GenericDCXSCIModule ReadBitEncodedFaultRegisters exception not thrown.\n",(xThrown == 1));

    // Test to make sure that the exception is properly thrown when unsupported communication methods are called
    xThrown = 0;
    try
    {
        gdsm.ReadFaultRegisters(dummy);
    }
    catch(ModuleException &e)
    {
        xThrown = 1;
    }
    CPPUNIT_ASSERT_MESSAGE("GenericDCXSCIModule ReadFaultRegisters exception not thrown.\n",(xThrown == 1));

    
}
