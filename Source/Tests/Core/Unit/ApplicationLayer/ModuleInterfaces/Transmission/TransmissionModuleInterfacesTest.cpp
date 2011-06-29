//******************************************************************************
// Description:
//   Test class for testing the Transmission Module Interface class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Transmission/TransmissionModuleInterfacesTest.cpp $
// 
// 2     10/31/07 10:16a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added printing of the test names.
// - Added GetShiftLeverLow3StringTest() and
// SetShiftLeverLow3StringTest().
// - Added SetHevLimpInTest() and GetHevLimpIntest().
// 
// 1     1/18/07 3:41p Rwiersem
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
#include "TransmissionModuleInterfacesTest.h"
#include "GenericTransmissionModuleTemplate.cpp"
#include "MockProtocolFilter.h"

//=============================================================================
TransmissionModuleInterfacesTest::TransmissionModuleInterfacesTest(void)
{
}

//=============================================================================
TransmissionModuleInterfacesTest::~TransmissionModuleInterfacesTest(void)
{
}

//=============================================================================
void TransmissionModuleInterfacesTest::setUp()
{
}

//=============================================================================
void TransmissionModuleInterfacesTest::tearDown()
{
    // move to the next line for the next test name
    cout << endl;
}

//=============================================================================
void TransmissionModuleInterfacesTest::ConstructorTest(void)
{
    cout << " ConstructorTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    CPPUNIT_ASSERT(gtmt.m_gearMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_limpInFaultMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_tccLockupMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_gearLockUpMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_shiftLeverMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_ignoredFaults == NULL);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear1);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear2);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear3);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear4);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear5);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear6);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearReverse);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearPark);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearNeutral);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allGearsObserved);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCNoLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCPartialLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCFullLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allLockStatesObserved);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverDrive);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverLow);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverReverse);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverPark);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverNeutral);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allShiftLeverPositionsObserved);

    // make sure the communication objects are not null
    CPPUNIT_ASSERT(gtmt.m_tccLockupMonitorComm != NULL);
    CPPUNIT_ASSERT(gtmt.m_limpInFaultMonitorComm != NULL);
    CPPUNIT_ASSERT(gtmt.m_gearMonitorComm != NULL);
    CPPUNIT_ASSERT(gtmt.m_gearLockUpMonitorComm != NULL);
    CPPUNIT_ASSERT(gtmt.m_shiftLeverMonitorComm != NULL);
}

//=============================================================================
void TransmissionModuleInterfacesTest::CloseComTest(void)
{
    cout << " CloseComTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    gtmt.CloseCom();

    CPPUNIT_ASSERT(gtmt.m_gearMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_limpInFaultMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_tccLockupMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_gearLockUpMonitor == NULL);
    CPPUNIT_ASSERT(gtmt.m_shiftLeverMonitor == NULL);
}

//=============================================================================
void TransmissionModuleInterfacesTest::OpenComTest(void)
{
    cout << " OpenComTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    gtmt.OpenCom();

    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear1);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear2);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear3);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear4);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear5);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGear6);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearReverse);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearPark);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawGearNeutral);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allGearsObserved);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCNoLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCPartialLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawTCCFullLockState);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allLockStatesObserved);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverDrive);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverLow);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverReverse);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverPark);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_sawShiftLeverNeutral);
    CPPUNIT_ASSERT_EQUAL(false,gtmt.m_allShiftLeverPositionsObserved);
}

//=============================================================================
void TransmissionModuleInterfacesTest::GetShiftLeverLow3StringTest(void)
{
    cout << " GetShiftLeverLow3StringTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;
  
    gtmt.m_shiftLeverLow3 = "Low3";

    CPPUNIT_ASSERT_EQUAL(string("Low3"),gtmt.GetShiftLeverLow3String());
}

//=============================================================================
void TransmissionModuleInterfacesTest::SetShiftLeverLow3StringTest(void)
{
    cout << " SetShiftLeverLow3StringTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    gtmt.SetShiftLeverLow3String("Low3");

    CPPUNIT_ASSERT_EQUAL(string("Low3"),gtmt.m_shiftLeverLow3);
}

//=============================================================================
void TransmissionModuleInterfacesTest::SetHevLimpInTest(void)
{
    cout << " SetHevLimpInTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    gtmt.SetCheckTechmLimpIn(true);

    CPPUNIT_ASSERT_EQUAL(true,gtmt.m_checkTechmLimpIn);
}

//=============================================================================
void TransmissionModuleInterfacesTest::GetHevLimpInTest(void)
{
    cout << " GetHevLimpInTest ";

    GenericTransmissionModuleTemplate<MockProtocolFilter> gtmt;

    gtmt.m_checkTechmLimpIn = true;

    CPPUNIT_ASSERT_EQUAL(true,gtmt.GetCheckTechmLimpIn());
}
