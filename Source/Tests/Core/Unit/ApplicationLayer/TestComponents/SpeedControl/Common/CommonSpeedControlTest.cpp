//******************************************************************************
// Description:
//   Test class for testing the SpeedControl/Common class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/SpeedControl/Common/CommonSpeedControlTest.cpp $
// 
// 2     1/18/07 4:44p Rwiersem
// Changes for the 07011901 core release:
//       
// - Added MockGenericEmissionsModuleTemplate.
// - Removed WarmInitializeTest().
// 
// 1     10/26/06 1:56p Rwiersem
// Changes for the 06102701 core release:
// 
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
#include "CommonSpeedControlTest.h"
#include "GenericSpeedControlTC.cpp"
#include "MockGenericEmissionsModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"
#include "MockTestResultServer.h"

//=============================================================================
CommonSpeedControlTest::CommonSpeedControlTest(void)
{
}

//=============================================================================
CommonSpeedControlTest::~CommonSpeedControlTest(void)
{
}

//=============================================================================
void CommonSpeedControlTest::setUp()
{
    // register the name "core/MotorController" so machineTC can
    // create a mock MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");
}

//=============================================================================
void CommonSpeedControlTest::tearDown()
{
    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);
}

//=============================================================================
void CommonSpeedControlTest::ConstructorTest(void)
{ 
    GenericSpeedControlTC
        <MockGenericEmissionsModuleTemplate<KeywordProtocolFilter> > gsc;

    CPPUNIT_ASSERT_EQUAL(false,gsc.m_isETCVehicle);
    CPPUNIT_ASSERT_EQUAL(false,gsc.m_previousPass);
    CPPUNIT_ASSERT_EQUAL(true,gsc.m_speedControlEquipped);
}
