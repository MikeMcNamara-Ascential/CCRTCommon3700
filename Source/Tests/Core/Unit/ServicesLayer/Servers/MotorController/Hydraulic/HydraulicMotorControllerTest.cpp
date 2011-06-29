//******************************************************************************
// Description:
//  The test class for testing the HydraulicMotorController class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/MotorController/Hydraulic/HydraulicMotorControllerTest.cpp $
// 
// 2     1/18/07 5:00p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added MockNamedDataBroker.
// - Added an assertion for m_checkVehiclePresence equals false in 
// TestLoadAdditionalConfigurationItems().
// - Added TestBoost_DisableVehiclePresent() and 
// TestBoost_EnableVehiclePresent().
// - Removed TestHandlePulse().
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "HydraulicMotorControllerTest.h"
#include "HydraulicMotorController.h"
#include "MockNamedDataBroker.h"

using namespace std;

//-----------------------------------------------------------------------------
HydraulicMotorControllerTest::HydraulicMotorControllerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/MotorController/Hydraulic/Configuration/";
}

//-----------------------------------------------------------------------------
HydraulicMotorControllerTest::~HydraulicMotorControllerTest()
{
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::setUp()
{
    hmc = new HydraulicMotorController();

    // register the name "core/NamedDataBroker" so HydraulicMotorController can
    // create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete hmc;
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::TestConstructor()
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0,hmc->m_minimumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_absMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_currentMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,hmc->m_minimumSpeedForBoost,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.095,hmc->m_psiConversion,0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-1,hmc->m_previousPressure,0.1);
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "HydraulicMotorControllerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    hmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(600.0,hmc->m_minimumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_absMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(600.0,hmc->m_boostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.095,hmc->m_psiConversion,0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_currentMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_EQUAL(false,hmc->m_checkVehiclePresence);
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "HydraulicMotorControllerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    hmc->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(50.0,hmc->m_minimumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_absMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(600.0,hmc->m_boostPressure,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.095,hmc->m_psiConversion,0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(800.0,hmc->m_currentMaximumBoostPressure,0.1);
    CPPUNIT_ASSERT_EQUAL(true,hmc->m_checkVehiclePresence);
}

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::TestBoost_DisableVehiclePresent(void)
{
    INT32 retVal;

    // set the other conditions that could disable boost to false
    hmc->m_minimumSpeedForBoost = 2.0;
    hmc->m_speeds[LeftFront] = 3.0; 
    hmc->m_speeds[LeftRear] = 3.0; 
    hmc->SetData("Zerospeed","0");
    hmc->SetData("Clutch","1");
    hmc->SetData("RollsUpElevatorDown","1");
    hmc->SetData("LossCompensationSwitchStatus","0");

    // set the conditions to disable boost due to losing vehicle presence
    hmc->SetData("VehiclePresence","0");
    hmc->m_checkVehiclePresence = 1;

    // create a MockNamedDataBroker
    hmc->m_dataComm = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)hmc->m_dataComm;

    // set the data needed by the mock named data broker
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back(BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = hmc->Boost();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,retVal);

    // make sure the boost pressure was set to 0
    CPPUNIT_ASSERT_EQUAL(string("BoostPressureOut"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);

    // make sure the boost status was set to off
    CPPUNIT_ASSERT_EQUAL(string("Boost"),mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[1]);
}                                                             

//-----------------------------------------------------------------------------
void HydraulicMotorControllerTest::TestBoost_EnableVehiclePresent(void)
{
    INT32 retVal;

    // set the other conditions that could disable boost to false
    hmc->m_minimumSpeedForBoost = 2.0;
    hmc->m_speeds[LeftFront] = 3.0; 
    hmc->m_speeds[RightFront] = 3.0; 
    hmc->m_speeds[LeftRear] = 3.0; 
    hmc->m_speeds[RightRear] = 3.0; 
    hmc->SetData("Zerospeed","0");
    hmc->SetData("Clutch","1");
    hmc->SetData("RollsUpElevatorDown","1");
    hmc->SetData("LossCompensationSwitchStatus","0");
    hmc->SetData("DriveWheel","Front");
    hmc->m_boost = false;

    // set vehicle presence to 0 and the parameter to not enabled
    hmc->SetData("VehiclePresence","0");
    hmc->m_checkVehiclePresence = 0;

    // create a MockNamedDataBroker
    hmc->m_dataComm = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)hmc->m_dataComm;

    // set the data needed by the mock named data broker
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back(BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = hmc->Boost();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,retVal);

    // since the speeds are the same, make sure the boost pressure was set to 0
    CPPUNIT_ASSERT_EQUAL(string("BoostPressureOut"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);

    // make sure the boost status was set to on
    CPPUNIT_ASSERT_EQUAL(string("Boost"),mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[1]);
}
