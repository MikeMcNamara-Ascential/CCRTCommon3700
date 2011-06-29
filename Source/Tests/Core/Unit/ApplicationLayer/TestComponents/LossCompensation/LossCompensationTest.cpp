//******************************************************************************
// Description:
//  The test class for testing the Loss Compensation class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/LossCompensation/LossCompensationTest.cpp $
// 
// 1     1/18/07 4:14p Rwiersem
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
#include "LossCompensationTest.h"
#include "LossCompensationTC.h"
#include "MockFaultServer.h"
#include "MockNamedDataBroker.h"

//=============================================================================
LossCompensationTest::LossCompensationTest(void)
{
}

//=============================================================================
LossCompensationTest::~LossCompensationTest(void)
{
}

//=============================================================================
void LossCompensationTest::setUp()
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
        "/Core/Unit/ApplicationLayer/TestComponents/LossCompensation/" +
        "Configuration/";

    // register the name "core/MotorController" so the LossCompensationTC
    // can create a mock MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");

    // register the name "core/NamedDataBroker" so LossCompensationTC can
    // create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    lctc = new LossCompensationTC();

    // create a mock fault server to use for testing
    lctc->m_fault = new MockFaultServer();
    mfs = (MockFaultServer*)lctc->m_fault;

    // create a mock named data broker to use for testin
    lctc->m_ndb = new MockNamedDataBroker();
    mndb = (MockNamedDataBroker*)lctc->m_ndb;
}

//=============================================================================
void LossCompensationTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);

    delete lctc;
}

//=============================================================================
void LossCompensationTest::ConstructorTest(void)
{ 
    CPPUNIT_ASSERT_EQUAL(string(BEP_UNTESTED_STATUS),lctc->m_lcStatus);
}

//=============================================================================
void LossCompensationTest::AnalyzeForcesTest(void)
{
    float machineLoss=0.0;
    INT32 status = BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "LossCompensation.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    lctc->m_currentTestStep = document->getChild("TestSteps/LossCompensation");

    // store the force data
    vector<float> forces;
    forces.push_back(3.0); forces.push_back(3.0); 
    forces.push_back(3.0); forces.push_back(3.0);
    lctc->m_wheelForceArray.push_back(forces); 

    // set the mock fault server return data
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the mock named data broker return data
    for(int ii=0; ii < 4; ii++)
    {
        mndb->m_writeReturnResponse.push_back("Pass");
        mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnDataValue.push_back("0");

    // call the method under test
    status = lctc->AnalyzeForces(0,machineLoss);

    // make sure the method passed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,status);

    // make sure the encoder noise and encoder run out faults were cleared
    CPPUNIT_ASSERT_EQUAL(2,mfs->m_clearFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mfs->m_clearFaultParamFault[1]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[1]);

    // make sure the encoder noise and encode run out flag was set to false
    CPPUNIT_ASSERT_EQUAL(4,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[1]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[1]);

    // make sure the current machine loss value was read
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_readCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_readParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_readParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_getByTagCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_getByTagParamTag[0]);

    // make sure the correct machine loss value was calculated
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,machineLoss,0.001);

    // make sure the new machine loss was written to the GUI
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontCurrentLoss"),
                         mndb->m_writeParamTag[2]);
    CPPUNIT_ASSERT_EQUAL(string("3.000000"),mndb->m_writeParamValue[2]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[2]);

    // make sure the new machine loss was written to the system
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_writeParamTag[3]);
    CPPUNIT_ASSERT_EQUAL(string("3.000000"),mndb->m_writeParamValue[3]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[3]);
}

//=============================================================================
void LossCompensationTest::AnalyzeForcesTest_NoiseError(void)
{
    float machineLoss=0.0;
    INT32 status = BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "LossCompensation.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    lctc->m_currentTestStep = document->getChild("TestSteps/LossCompensation");

    // store the force data
    vector<float> forces;
    forces.push_back(3.0); forces.push_back(3.0); 
    forces.push_back(71.0); forces.push_back(3.0);
    lctc->m_wheelForceArray.push_back(forces); 

    // set the mock fault server return data
    mfs->m_setFaultResponse.push_back("Pass");
    mfs->m_setFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the mock named data broker return data
    for(int ii=0; ii < 4; ii++)
    {
        mndb->m_writeReturnResponse.push_back("Pass");
        mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnDataValue.push_back("0");

    // call the method under test
    status = lctc->AnalyzeForces(0,machineLoss);

    // make sure the method failed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_FAILURE,status);

    // make sure the encoder noise fault was set 
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_setFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mfs->m_setFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_setFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("LeftFront Encoder Noise Error Detected"),
                         mfs->m_setFaultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(lctc->GetComponentName(),mfs->m_setFaultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(-1,mfs->m_setFaultParamSeverity[0]);

    // make sure the encoder run out fault was cleared
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_clearFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);

    // make sure the encoder noise flag was set to true
    CPPUNIT_ASSERT_EQUAL(4,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[0]);

    // make sure the runout flag was set to false
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[1]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[1]);

    // make sure the current machine loss value was read
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_readCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_readParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_readParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_getByTagCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_getByTagParamTag[0]);

    // make sure the correct machine loss value was calculated
    CPPUNIT_ASSERT_DOUBLES_EQUAL(20.0,machineLoss,0.001);

    // make sure the new machine loss was written to the GUI
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontCurrentLoss"),
                         mndb->m_writeParamTag[2]);
    CPPUNIT_ASSERT_EQUAL(string("20.000000"),mndb->m_writeParamValue[2]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[2]);

    // make sure the new machine loss was written to the system
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_writeParamTag[3]);
    CPPUNIT_ASSERT_EQUAL(string("20.000000"),mndb->m_writeParamValue[3]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[3]);
}

//=============================================================================
void LossCompensationTest::AnalyzeForcesTest_RunoutError(void)
{
    float machineLoss=0.0;
    INT32 status = BEP_STATUS_NA;
    XmlParser parser;
    string configFile = m_configDir + "LossCompensationRunoutError.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    lctc->m_currentTestStep = document->getChild("TestSteps/LossCompensation");

    // store the force data
    vector<float> forces;
    forces.push_back(3.0); forces.push_back(13.0); 
    forces.push_back(3.0); forces.push_back(11.0);
    lctc->m_wheelForceArray.push_back(forces); 

    // set the mock fault server return data
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mfs->m_setFaultResponse.push_back("Pass");
    mfs->m_setFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the mock named data broker return data
    for(int ii=0; ii < 4; ii++)
    {
        mndb->m_writeReturnResponse.push_back("Pass");
        mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnDataValue.push_back("0");

    // call the method under test
    status = lctc->AnalyzeForces(0,machineLoss);

    // make sure the method failed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_FAILURE,status);

    // make sure the encoder noise fault was cleared
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_clearFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);

    // make sure the encoder runout fault was set 
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_setFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mfs->m_setFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_setFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("LeftFront Encoder Runout Error Detected"),
                         mfs->m_setFaultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(lctc->GetComponentName(),mfs->m_setFaultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(-1,mfs->m_setFaultParamSeverity[0]);

    // make sure the encoder noise flag was set to false
    CPPUNIT_ASSERT_EQUAL(4,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderNoiseError"),
                         mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[0]);

    // make sure the runout flag was set to true
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontEncoderRunoutError"),
                         mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[1]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[1]);

    // make sure the current machine loss value was read
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_readCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_readParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_readParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_getByTagCalled);
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_getByTagParamTag[0]);

    // make sure the correct machine loss value was calculated
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5,machineLoss,0.001);

    // make sure the new machine loss was written to the GUI
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontCurrentLoss"),
                         mndb->m_writeParamTag[2]);
    CPPUNIT_ASSERT_EQUAL(string("7.500000"),mndb->m_writeParamValue[2]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[2]);

    // make sure the new machine loss was written to the system
    CPPUNIT_ASSERT_EQUAL(string("LeftFrontMachineLoss"),
                         mndb->m_writeParamTag[3]);
    CPPUNIT_ASSERT_EQUAL(string("7.500000"),mndb->m_writeParamValue[3]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[3]);
}
