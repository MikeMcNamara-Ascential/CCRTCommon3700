//******************************************************************************
// Description:
//   Test class for testing the GenericABSTCTemplate class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/GenericABSTCTemplateTest.cpp $
// 
// 1     1/18/07 4:04p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
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
#include "GenericABSTCTemplateTest.h"
#include "GenericABSTCTemplate.cpp"
#include "GenericABSModuleTemplate.cpp"
#include "MockProtocolFilter.h"

//=============================================================================
GenericABSTCTemplateTest::GenericABSTCTemplateTest(void)
{
}

//=============================================================================
GenericABSTCTemplateTest::~GenericABSTCTemplateTest(void)
{
}

//=============================================================================
void GenericABSTCTemplateTest::setUp()
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
    m_configDir = topTestDir +
        "/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/Configuration/";

    // register the name "core/MotorController" so GenericABSTCTemplate can
    // create a MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");
}

//=============================================================================
void GenericABSTCTemplateTest::tearDown()
{
    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);
}

//=============================================================================
void GenericABSTCTemplateTest::ConstructorTest(void)
{
    GenericABSTCTemplate<GenericABSModuleTemplate<MockProtocolFilter> > gatt;

    CPPUNIT_ASSERT_EQUAL(false,gatt.m_isFourChannelSystem);
    CPPUNIT_ASSERT_EQUAL(0,gatt.m_absStartIndex);
    CPPUNIT_ASSERT_EQUAL(0,gatt.m_absEndIndex);
    CPPUNIT_ASSERT_EQUAL(false,gatt.m_isESPEquipped);
}

//=============================================================================
void GenericABSTCTemplateTest::SetESPEquippedTest(void)
{
    GenericABSTCTemplate<GenericABSModuleTemplate<MockProtocolFilter> > gatt;

    gatt.SetESPEquipped(true);

    CPPUNIT_ASSERT_EQUAL(true,gatt.m_isESPEquipped);
}

//=============================================================================
void GenericABSTCTemplateTest::IsESPEquippedTest(void)
{
    GenericABSTCTemplate<GenericABSModuleTemplate<MockProtocolFilter> > gatt;

    gatt.m_isESPEquipped = true;

    CPPUNIT_ASSERT_EQUAL(true,gatt.IsESPEquipped());
}
