//******************************************************************************
// Description:
//   Test class for testing the VehicleTest library class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/VehicleTest/Library/VehicleTestLibraryTest.cpp $
// 
// 1     10/26/06 1:59p Rwiersem
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
#include "VehicleTest.h"
#include "VehicleTestLibraryTest.h"

//=============================================================================
VehicleTestLibraryTest::VehicleTestLibraryTest(void)
{
}

//=============================================================================
VehicleTestLibraryTest::~VehicleTestLibraryTest(void)
{
}

//=============================================================================
void VehicleTestLibraryTest::setUp()
{
    // register the name "core/NamedDataBroker" so machineTC can
    // create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    vt = new VehicleTest();
}

//=============================================================================
void VehicleTestLibraryTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete vt;
}

//=============================================================================
void VehicleTestLibraryTest::ConstructorTest(void)
{
    // the critical section enter should fail because the count is 0
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         vt->m_semCommandTest.CriticalSectionEnter(1));

    // The count on the semaphore should be set to 1.  So, the first 
    // CriticalSectionEnter() should be successful and the second one should
    // fail
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,
                         vt->m_semVehicleBuild.CriticalSectionEnter(1));
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         vt->m_semVehicleBuild.CriticalSectionEnter(1));

    CPPUNIT_ASSERT(vt->m_faultServer == NULL);
    CPPUNIT_ASSERT_EQUAL(1000,vt->m_maxTestNumber);
    CPPUNIT_ASSERT(vt->m_sequencerComm == NULL);
    CPPUNIT_ASSERT_EQUAL(string(""),vt->m_fault);
    CPPUNIT_ASSERT_EQUAL(false,vt->m_terminateComponentsOnTestComplete);
    CPPUNIT_ASSERT_EQUAL(false,vt->m_useVinAsTestLogSuffix);
    CPPUNIT_ASSERT_EQUAL(string(BEP_RUNNING_STATUS),vt->GetStatus());
    CPPUNIT_ASSERT(vt->m_vehicleBuild == NULL);
    CPPUNIT_ASSERT_EQUAL(0,vt->m_testNumber);
}

