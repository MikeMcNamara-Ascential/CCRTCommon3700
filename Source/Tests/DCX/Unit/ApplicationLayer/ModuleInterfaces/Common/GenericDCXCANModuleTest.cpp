//******************************************************************************
// Description:
//       Runs all DCX CAN module interface tests
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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericDCXCANModuleTest.cpp $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericDCXCANModule.cpp"
#include "GenericDCXCANModuleTest.h"
#include "MockProtocolFilter.h"

//=============================================================================
GenericDCXCANModuleTest::GenericDCXCANModuleTest(void)
{
}

//=============================================================================
GenericDCXCANModuleTest::~GenericDCXCANModuleTest(void)
{
}

//=============================================================================
void GenericDCXCANModuleTest::setUp()
{
}

//=============================================================================
void GenericDCXCANModuleTest::tearDown()
{
}

//=============================================================================
void GenericDCXCANModuleTest::ExceptionTest(void)
{
    // Here be dragons
    UINT8 xThrown = 0;
    FaultVector_t dummy;
    GenericDCXCANModule gdcm(NULL);

    // Test to make sure that the exception is properly thrown when unsupported communication methods are called
    try
    {
        gdcm.ReadBitEncodedFaultRegisters(dummy);
    }
    catch(ModuleException &e)
    {
        xThrown = 1;
    }
    CPPUNIT_ASSERT_MESSAGE("GenericDCXCANModule ReadBitEncodedFaultRegisters exception not thrown.\n",(xThrown == 1)); 

    // Test to make sure that the exception is properly thrown when unsupported communication methods are called
    xThrown = 0;
    try
    {
        gdcm.ReadFaultRegisters(dummy);
    }
    catch(ModuleException &e)
    {
        xThrown = 1;
    }
    CPPUNIT_ASSERT_MESSAGE("GenericDCXCANModule ReadFaultRegisters exception not thrown.\n",(xThrown == 1));


}
