//******************************************************************************
// Description:
//   Test class for testing the Base Brake Tool class.
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/BaseBrakeToolTest.cpp $
// 
// 7     10/31/07 10:17a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added printing of the test names.
// - Added AnalyzeBrakeForcesTest(),
// AnalyzeBrakeForcesTest_AveragingError(), and
// AnalyzeBrakeForcesTest_NoMultiplier().
// 
// 6     2/22/07 11:26a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added an assertion for m_calcBrakeForce in WarmInitializeTest().
// 
// 5     1/18/07 4:02p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_launchForceThread equals true assertion in ConstructorTest().
// - Added WarmInitializeTest() and DeactivateTest().
// 
// 4     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 4:21p Rwiersem
// Removed the code that launched and slayed the electric motor controller
// task.  Added code to register and unregister the task name
// "core/MotorController".  This accomplishes the objective.
//
// 3     3/02/06 12:45a Cward
// Updated test dir path to accomodate core and non core testing (moved
// base point up one level)
//
// 2     8/17/05 8:29p Cward
// changed Electric Motor controller executable to Baldor Motor Controller
//
// 1     7/20/05 8:14p Cward
// Directory structure change
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
#include "BaseBrakeToolTest.h"
#include "BaseBrakeTool.h"
#include "Bepos.h"
#include "MockGenericTc.h"

//=============================================================================
BaseBrakeToolTest::BaseBrakeToolTest(void)
{
}

//=============================================================================
BaseBrakeToolTest::~BaseBrakeToolTest(void)
{
}

//=============================================================================
void BaseBrakeToolTest::setUp()
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

    // register the name "core/MotorController" so base brake tool can
    // create a MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");
}

//=============================================================================
void BaseBrakeToolTest::tearDown()
{
    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);

    // move to the next line for the next test name
    cout << endl;
}

//=============================================================================
void BaseBrakeToolTest::ConstructorTest(void)
{
    cout << " ConstructorTest ";

    BaseBrakeTool bbt(new GenericTC());

    // make sure m_calcBrakeForce is BEP_UNTESTED_RESPONSE
    CPPUNIT_ASSERT_MESSAGE("m_calcBrakeForce is not BEP_UNTESTED_RESPONSE",
                           (bbt.m_calcBrakeForce == BEP_UNTESTED_RESPONSE));

    // make sure m_dragStart is 0
    CPPUNIT_ASSERT_MESSAGE("m_dragStart is not 0",(bbt.m_dragStart == 0));

    // make sure m_dragEnd is 0
    CPPUNIT_ASSERT_MESSAGE("m_dragEnd is not 0",(bbt.m_dragEnd == 0));

    // make sure m_brakeStart is 0
    CPPUNIT_ASSERT_MESSAGE("m_brakeStart is not 0",(bbt.m_brakeStart == 0));

    // make sure m_brakeEnd is 0
    CPPUNIT_ASSERT_MESSAGE("m_brakeEnd is not 0",(bbt.m_brakeEnd == 0));

    // make sure m_readForces is unlocked
    CPPUNIT_ASSERT_MESSAGE("m_readForces is not unlocked",
                           (bbt.m_readForces.CriticalSectionTryEnter() ==
                            BEP_STATUS_SUCCESS));

    // make sure m_semReadCurrentForce is unlocked
    CPPUNIT_ASSERT_MESSAGE(
        "m_semReadCurrentForce is not unlocked",
        (bbt.m_semReadCurrentForce.CriticalSectionTryEnter() == 
                            BEP_STATUS_SUCCESS));

    // make sure m_forceThreadID is -1
    CPPUNIT_ASSERT_MESSAGE("m_forceThreadID is not -1",
                           (bbt.m_forceThreadID == -1));

    // make sure m_updateForceFlag is STOP
    CPPUNIT_ASSERT_MESSAGE("m_updateForceFlag is not STOP",
                           (bbt.m_updateForceFlag == STOP));

    // make sure m_forceValue is 0
    CPPUNIT_ASSERT_MESSAGE("m_forceValue is not 0",(bbt.m_forceValue == 0));

    // make sure m_forceFileOpen is false
    CPPUNIT_ASSERT_MESSAGE("m_forceFileOpen is not false",
                           (bbt.m_forceFileOpen == false));

    CPPUNIT_ASSERT_EQUAL(true,bbt.m_launchForceThread);
}

//============================================================================
void BaseBrakeToolTest::WarmInitializeTest(void)
{
    cout << " WarmInitializeTest ";

    BaseBrakeTool bbt(new GenericTC());

    // don't launch the force thread
    bbt.m_launchForceThread = false;

    bbt.WarmInitialize();

    CPPUNIT_ASSERT_EQUAL(0,bbt.m_dragStart);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_dragEnd);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_brakeStart);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_brakeEnd);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,bbt.m_forceValue,0.01);
    CPPUNIT_ASSERT_EQUAL(string(BEP_UNTESTED_RESPONSE),bbt.m_calcBrakeForce);
    CPPUNIT_ASSERT_EQUAL(STOP,bbt.m_updateForceFlag);
}

//============================================================================
void BaseBrakeToolTest::DeactivateTest(void)
{
    cout << " DeactivateTest ";

    BaseBrakeTool bbt(new GenericTC());

    bbt.Deactivate();

    CPPUNIT_ASSERT_EQUAL(0,bbt.m_dragStart);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_dragEnd);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_brakeStart);
    CPPUNIT_ASSERT_EQUAL(0,bbt.m_brakeEnd);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,bbt.m_forceValue,0.01);
}

//============================================================================
void BaseBrakeToolTest::AnalyzeBrakeForcesTest(void)
{
    cout << " AnalyzeBrakeForcesTest ";

    BaseBrakeTool bbt(new MockGenericTc());
    INT32 retVal=BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "BaseBrakeToolConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the data needed by MockGenericTc
    MockGenericTc *mgtc = (MockGenericTc*)bbt.m_component;
    mgtc->m_readDataArraysReturnValue.push_back(BEP_STATUS_SUCCESS);
    DATAARRAY wheel[4];
    for(int ii=0;ii<5;ii++)
    {
        wheel[0].push_back(ii+100);
        wheel[1].push_back(ii+101);
        wheel[2].push_back(ii+102);
        wheel[3].push_back(ii+103);
    }
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[0]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[1]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[2]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[3]);

    for(int jj=0; jj < 4; jj++)
    {
        // SystemWrite() is called twice per roll
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        // SendSubtestResultWithDetail is called once per roll
        mgtc->m_sendSubtestResultWithDetailReturnValue.push_back(BEP_STATUS_SUCCESS);
    }

    // set the current test step
    mgtc->SetCurrentTestStep(document->getChild("TestSteps/AnalyzeBrakeTest"));

    // set the parameters
    mgtc->m_parameters = 
        (XmlNodeMap)(document->getChild("Setup/Parameters")->getChildren());

    // set the roller count
    mgtc->SetRollerCount(4);

    // call the method under test
    retVal = bbt.AnalyzeBrakeForces(0,5);

    // make sure the correct result was returned
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,retVal);

    // make sure the correct data was passed to ReadDataArrays()
    CPPUNIT_ASSERT_EQUAL(1,mgtc->m_readDataArraysCalled);
    CPPUNIT_ASSERT_EQUAL(string("/dev/ArrayForce"),
                         mgtc->m_readDataArraysParamArray[0]);
    CPPUNIT_ASSERT_EQUAL(0,mgtc->m_readDataArraysParamStartIdx[0]);
    CPPUNIT_ASSERT_EQUAL(5,mgtc->m_readDataArraysParamEndIdx[0]);

    // make sure the correct data was passed to SendSubtestResultWithDetail()
    CPPUNIT_ASSERT_EQUAL(4,mgtc->m_sendSubtestResultWithDetailCalled);

    // these values are the same for all four rolls
    for(int index=0;index < 4; index++)
    {
        CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,mgtc->m_sendSubtestResultWithDetailParamResult[index]);
        CPPUNIT_ASSERT_EQUAL(string("0000"),mgtc->m_sendSubtestResultWithDetailParamCode[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForce"),mgtc->m_sendSubtestResultWithDetailParamFirstDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFirstDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MaxBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamSecondDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamSecondDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamThirdDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamThirdDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFourthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFourthDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFifthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFifthDetailUnits[index]);
    }

    CPPUNIT_ASSERT_EQUAL(string("LeftFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[0]);
    CPPUNIT_ASSERT_EQUAL(string("RightFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[1]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[3]);

    CPPUNIT_ASSERT_EQUAL(string("LeftFront Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("RightFront Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[1]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRear Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightRear Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[3]);

    CPPUNIT_ASSERT_EQUAL(string("102.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("103.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("104.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("105.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("150.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("150.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("100.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("100.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("204.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("206.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("208.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("210.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[3]);
}

//=============================================================================
void BaseBrakeToolTest::AnalyzeBrakeForcesTest_AveragingError(void)
{
    cout << " AnalyzeBrakeForcesTest_AveragingError ";

    BaseBrakeTool bbt(new MockGenericTc());
    INT32 retVal=BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "BaseBrakeToolConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the data needed by MockGenericTc
    MockGenericTc *mgtc = (MockGenericTc*)bbt.m_component;
    mgtc->m_readDataArraysReturnValue.push_back(BEP_STATUS_SUCCESS);

    for(int jj=0; jj < 4; jj++)
    {
        // SystemWrite() is called twice per roll
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        // SendSubtestResultWithDetail is called once per roll
        mgtc->m_sendSubtestResultWithDetailReturnValue.push_back(BEP_STATUS_SUCCESS);
    }

    // set the current test step
    mgtc->SetCurrentTestStep(document->getChild("TestSteps/AnalyzeBrakeTest"));

    // set the parameters
    mgtc->m_parameters = document->getChild("Setup/Parameters")->getChildren();

    // set the fault list
    mgtc->m_faults = document->getChild("Setup/FaultList")->getChildren();

    // set the roller count
    mgtc->SetRollerCount(4);

    // call the method under test
    retVal = bbt.AnalyzeBrakeForces(0,5);

    // make sure the correct result was returned
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_FAILURE,retVal);

    // make sure the correct data was passed to ReadDataArrays()
    CPPUNIT_ASSERT_EQUAL(1,mgtc->m_readDataArraysCalled);
    CPPUNIT_ASSERT_EQUAL(string("/dev/ArrayForce"),
                         mgtc->m_readDataArraysParamArray[0]);
    CPPUNIT_ASSERT_EQUAL(0,mgtc->m_readDataArraysParamStartIdx[0]);
    CPPUNIT_ASSERT_EQUAL(5,mgtc->m_readDataArraysParamEndIdx[0]);

    // make sure the correct data was passed to SystemWrite()
    CPPUNIT_ASSERT_EQUAL(8,mgtc->m_systemWriteCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontForceValue"),mgtc->m_systemWriteParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("-1"),mgtc->m_systemWriteParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontForceBGColor"),mgtc->m_systemWriteParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("Red"),mgtc->m_systemWriteParamValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("RightFrontForceValue"),mgtc->m_systemWriteParamTag[2]);
    CPPUNIT_ASSERT_EQUAL(string("-1"),mgtc->m_systemWriteParamValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightFrontForceBGColor"),mgtc->m_systemWriteParamTag[3]);
    CPPUNIT_ASSERT_EQUAL(string("Red"),mgtc->m_systemWriteParamValue[3]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRearForceValue"),mgtc->m_systemWriteParamTag[4]);
    CPPUNIT_ASSERT_EQUAL(string("-1"),mgtc->m_systemWriteParamValue[4]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRearForceBGColor"),mgtc->m_systemWriteParamTag[5]);
    CPPUNIT_ASSERT_EQUAL(string("Red"),mgtc->m_systemWriteParamValue[5]);
    CPPUNIT_ASSERT_EQUAL(string("RightRearForceValue"),mgtc->m_systemWriteParamTag[6]);
    CPPUNIT_ASSERT_EQUAL(string("-1"),mgtc->m_systemWriteParamValue[6]);
    CPPUNIT_ASSERT_EQUAL(string("RightRearForceBGColor"),mgtc->m_systemWriteParamTag[7]);
    CPPUNIT_ASSERT_EQUAL(string("Red"),mgtc->m_systemWriteParamValue[7]);

    // make sure the correct data was passed to SendSubtestResultWithDetail()
    CPPUNIT_ASSERT_EQUAL(4,mgtc->m_sendSubtestResultWithDetailCalled);

    // these values are the same for all four rolls
    for(int index=0;index < 4; index++)
    {
        CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_FAILURE,mgtc->m_sendSubtestResultWithDetailParamResult[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForce"),mgtc->m_sendSubtestResultWithDetailParamFirstDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string("-1.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFirstDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MaxBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamSecondDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamSecondDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamThirdDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamThirdDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFourthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFourthDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFifthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string("-2.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFifthDetailUnits[index]);
    }

    CPPUNIT_ASSERT_EQUAL(string("LeftFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[0]);
    CPPUNIT_ASSERT_EQUAL(string("RightFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[1]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[3]);

    CPPUNIT_ASSERT_EQUAL(string("Left Front brake force fault"),mgtc->m_sendSubtestResultWithDetailParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("Right Front brake force fault"),mgtc->m_sendSubtestResultWithDetailParamDescription[1]);
    CPPUNIT_ASSERT_EQUAL(string("Left Rear brake force fault"),mgtc->m_sendSubtestResultWithDetailParamDescription[2]);
    CPPUNIT_ASSERT_EQUAL(string("Right Rear brake force fault"),mgtc->m_sendSubtestResultWithDetailParamDescription[3]);

    CPPUNIT_ASSERT_EQUAL(string("0001"),mgtc->m_sendSubtestResultWithDetailParamCode[0]);
    CPPUNIT_ASSERT_EQUAL(string("0002"),mgtc->m_sendSubtestResultWithDetailParamCode[1]);
    CPPUNIT_ASSERT_EQUAL(string("0003"),mgtc->m_sendSubtestResultWithDetailParamCode[2]);
    CPPUNIT_ASSERT_EQUAL(string("0004"),mgtc->m_sendSubtestResultWithDetailParamCode[3]);

    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("150.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("150.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("100.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("100.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[3]);
}

//=============================================================================
void BaseBrakeToolTest::AnalyzeBrakeForcesTest_NoMultiplier(void)
{
    cout << " AnalyzeBrakeForcesTest_NoMultiplier ";

    BaseBrakeTool bbt(new MockGenericTc());
    INT32 retVal=BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "BaseBrakeToolNoMultiplier.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the data needed by MockGenericTc
    MockGenericTc *mgtc = (MockGenericTc*)bbt.m_component;
    mgtc->m_readDataArraysReturnValue.push_back(BEP_STATUS_SUCCESS);
    DATAARRAY wheel[4];
    for(int ii=0;ii<5;ii++)
    {
        wheel[0].push_back(ii+100);
        wheel[1].push_back(ii+101);
        wheel[2].push_back(ii+102);
        wheel[3].push_back(ii+103);
    }
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[0]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[1]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[2]);
    mgtc->m_readDataArraysReturnDataArray.push_back(wheel[3]);

    for(int jj=0; jj < 4; jj++)
    {
        // SystemWrite() is called twice per roll
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        mgtc->m_systemWriteReturnValue.push_back(BEP_STATUS_SUCCESS);
        // SendSubtestResultWithDetail is called once per roll
        mgtc->m_sendSubtestResultWithDetailReturnValue.push_back(BEP_STATUS_SUCCESS);
    }

    // set the current test step
    mgtc->SetCurrentTestStep(document->getChild("TestSteps/AnalyzeBrakeTest"));

    // set the parameters
    mgtc->m_parameters = 
        (XmlNodeMap)(document->getChild("Setup/Parameters")->getChildren());

    // set the roller count
    mgtc->SetRollerCount(4);

    // call the method under test
    retVal = bbt.AnalyzeBrakeForces(0,5);

    // make sure the correct result was returned
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,retVal);

    // make sure the correct data was passed to SendSubtestResultWithDetail()
    CPPUNIT_ASSERT_EQUAL(4,mgtc->m_sendSubtestResultWithDetailCalled);

    // these values are the same for all four rolls
    for(int index=0;index < 4; index++)
    {
        CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,mgtc->m_sendSubtestResultWithDetailParamResult[index]);
        CPPUNIT_ASSERT_EQUAL(string("0000"),mgtc->m_sendSubtestResultWithDetailParamCode[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForce"),mgtc->m_sendSubtestResultWithDetailParamFirstDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFirstDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MaxBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamSecondDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamSecondDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForce"),mgtc->m_sendSubtestResultWithDetailParamThirdDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamThirdDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("MinBrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFourthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFourthDetailUnits[index]);
        CPPUNIT_ASSERT_EQUAL(string("BrakeForceWithMultiplier"),mgtc->m_sendSubtestResultWithDetailParamFifthDetail[index]);
        CPPUNIT_ASSERT_EQUAL(string(unitsLBF),mgtc->m_sendSubtestResultWithDetailParamFifthDetailUnits[index]);
    }

    CPPUNIT_ASSERT_EQUAL(string("LeftFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[0]);
    CPPUNIT_ASSERT_EQUAL(string("RightFrontForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[1]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightRearForceTest"),mgtc->m_sendSubtestResultWithDetailParamTestName[3]);

    CPPUNIT_ASSERT_EQUAL(string("LeftFront Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("RightFront Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[1]);
    CPPUNIT_ASSERT_EQUAL(string("LeftRear Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[2]);
    CPPUNIT_ASSERT_EQUAL(string("RightRear Brake Force Test Result"),mgtc->m_sendSubtestResultWithDetailParamDescription[3]);

    CPPUNIT_ASSERT_EQUAL(string("102.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("103.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("104.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("105.00"),mgtc->m_sendSubtestResultWithDetailParamFirstDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("600"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("400"),mgtc->m_sendSubtestResultWithDetailParamSecondDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("75"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("50"),mgtc->m_sendSubtestResultWithDetailParamThirdDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("75.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("75.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("50.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("50.00"),mgtc->m_sendSubtestResultWithDetailParamFourthDetailValue[3]);

    CPPUNIT_ASSERT_EQUAL(string("102.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("103.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[1]);
    CPPUNIT_ASSERT_EQUAL(string("104.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[2]);
    CPPUNIT_ASSERT_EQUAL(string("105.00"),mgtc->m_sendSubtestResultWithDetailParamFifthDetailValue[3]);
}
