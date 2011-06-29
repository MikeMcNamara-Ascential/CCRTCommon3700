//******************************************************************************
// Description:
//   Test class for testing the GenericABSModuleTemplate class.
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
//     $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/TRW/GenericTrwABSModuleTemplateTest.cpp $
// 
// 1     9/07/07 10:08a Dkrause
// 
// 2     1/18/07 3:22p Rwiersem
// Changes for the 07011901 core release:
//                                                            
// - Added OpenComTest() and CloseComTest().
// 
// 1     10/26/06 1:35p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include <malloc.h>
#include "GenericTrwABSModuleTemplate.cpp"
#include "GenericTrwABSModuleTemplateTest.h"
#include "KeywordProtocolFilter.h"

//=============================================================================
GenericTrwABSModuleTemplateTest::GenericTrwABSModuleTemplateTest(void)
{
}

//=============================================================================
GenericTrwABSModuleTemplateTest::~GenericTrwABSModuleTemplateTest(void)
{
}

//=============================================================================
void GenericTrwABSModuleTemplateTest::setUp()
{
    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to top testing";
        cerr << "dir, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configPath = topTestDir +
        "/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/TRW/Configuration/";
}

//=============================================================================
void GenericTrwABSModuleTemplateTest::tearDown()
{
}

//=============================================================================
void GenericTrwABSModuleTemplateTest::ConstructorTest(void)
{
    GenericTrwABSModuleTemplate<KeywordProtocolFilter> gamt;

    //Make sure to run the core abs test before this one
    CPPUNIT_ASSERT_EQUAL(UINT8(0x00), UINT8(gamt.m_firstFaultRegister));
    CPPUNIT_ASSERT_EQUAL(UINT8(NULL), UINT8(gamt.m_faultReader));
} 
