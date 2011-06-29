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
//     $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/Teves/TevesMk20EModuleTest.cpp $
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
#include "TevesMk20EModule.cpp"
#include "TevesMk20EModuleTest.h"
#include "KeywordProtocolFilter.h"

//=============================================================================
TevesMk20EModuleTest::TevesMk20EModuleTest(void)
{
}

//=============================================================================
TevesMk20EModuleTest::~TevesMk20EModuleTest(void)
{
}

//=============================================================================
void TevesMk20EModuleTest::setUp()
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
        "/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/Teves/Configuration/";
}

//=============================================================================
void TevesMk20EModuleTest::tearDown()
{
}

//=============================================================================
void TevesMk20EModuleTest::ConstructorTest(void)
{
    TevesMk20EModule<KeywordProtocolFilter> gamt;

    CPPUNIT_ASSERT_EQUAL(NULL,gamt.m_faultReader);
    CPPUNIT_ASSERT_EQUAL(NULL,gamt.m_valveController);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_waitForSpontaneousReset);
}
