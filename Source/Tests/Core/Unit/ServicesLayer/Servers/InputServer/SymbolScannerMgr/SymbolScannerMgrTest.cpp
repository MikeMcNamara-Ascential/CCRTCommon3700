//******************************************************************************
// Description:
//   Test class for testing the SymbolScannerMgr class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/InputServer/SymbolScannerMgr/SymbolScannerMgrTest.cpp $
// 
// 2     2/22/07 12:45p Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added MockNamedDataBroker object.
// - Added assertions for reading in the m_maxVinLength configuration
// item.
// - Added EvaluateData_UseMaxVinLength() and EvaluateData_UseVinLength().
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "SymbolScannerMgrTest.h"
#include "SymbolScannerMgr.h"
#include "MockNamedDataBroker.h"

using namespace std;

//-----------------------------------------------------------------------------
SymbolScannerMgrTest::SymbolScannerMgrTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" 
            << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/InputServer/"
        "SymbolScannerMgr/Configuration/";
}

//-----------------------------------------------------------------------------
SymbolScannerMgrTest::~SymbolScannerMgrTest()
{
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::setUp()
{
    // register the name "core/NamedDataBroker" so GenericEmissionsTCTemplate
    // can create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    ssm = new SymbolScannerMgr();
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete ssm;
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::TestConstructor()
{
    CPPUNIT_ASSERT(ssm->m_scannerComm == NULL);
    CPPUNIT_ASSERT_EQUAL((INT16)0,ssm->m_badDataPosition);
    CPPUNIT_ASSERT_EQUAL((UINT8)0x31,ssm->m_badCharacterReplacement);
    CPPUNIT_ASSERT_EQUAL((UINT8)0x00,ssm->m_badDataIndicator);
    CPPUNIT_ASSERT_EQUAL(true,ssm->m_lookForTerminator);
    CPPUNIT_ASSERT_EQUAL(false,ssm->m_correctBadScannerData);
    CPPUNIT_ASSERT_EQUAL(17,ssm->m_vinLength);
    CPPUNIT_ASSERT_EQUAL(7,ssm->m_driverNumberLength);
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SymbolScannerMgrConfig.xml";

    // this will prevent the object from trying to open the logical port
    ssm->m_scannerComm = new SerialChannel();

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/Handheld");

    ssm->LoadAdditionalConfigurationItems(configNode);

    CPPUNIT_ASSERT_EQUAL(129,ssm->GetScannerBufferSize());
    CPPUNIT_ASSERT_EQUAL(3000,ssm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL((INT16)4,ssm->GetMaxRetries());
    CPPUNIT_ASSERT_EQUAL(18,ssm->m_vinLength);
    CPPUNIT_ASSERT_EQUAL(21,ssm->m_maxVinLength);
    CPPUNIT_ASSERT_EQUAL(8,ssm->m_driverNumberLength);
    CPPUNIT_ASSERT_EQUAL(false,ssm->LookForTerminationCharacter());
    CPPUNIT_ASSERT_EQUAL((char)0x30,ssm->m_terminationCharacter);
    CPPUNIT_ASSERT_EQUAL(true,ssm->CorrectBadScannerData());
    CPPUNIT_ASSERT_EQUAL((INT16)1,ssm->BadDataIndex());
    CPPUNIT_ASSERT_EQUAL((UINT8)2,ssm->BadDataIndicator());
    CPPUNIT_ASSERT_EQUAL((UINT8)0x31,ssm->BadDataReplacement());
}                                                        

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SymbolScannerMgrConfigMissingTags.xml";

    // this will prevent the object from trying to open the logical port
    ssm->m_scannerComm = new SerialChannel();

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *configNode =
        document->getChild("Setup/InputDevices/Handheld");

    ssm->LoadAdditionalConfigurationItems(configNode);
  
    CPPUNIT_ASSERT_EQUAL(128,ssm->GetScannerBufferSize());
    CPPUNIT_ASSERT_EQUAL(10,ssm->GetDataDelay());
    CPPUNIT_ASSERT_EQUAL((INT16)3,ssm->GetMaxRetries());
    CPPUNIT_ASSERT_EQUAL(17,ssm->m_vinLength);
    CPPUNIT_ASSERT_EQUAL(17,ssm->m_maxVinLength);
    CPPUNIT_ASSERT_EQUAL(7,ssm->m_driverNumberLength);
    CPPUNIT_ASSERT_EQUAL(true,ssm->LookForTerminationCharacter());
    CPPUNIT_ASSERT_EQUAL((char)0x0D,ssm->m_terminationCharacter);
    CPPUNIT_ASSERT_EQUAL(false,ssm->CorrectBadScannerData());
    CPPUNIT_ASSERT_EQUAL((INT16)0,ssm->BadDataIndex());
    CPPUNIT_ASSERT_EQUAL((UINT8)0,ssm->BadDataIndicator());
    CPPUNIT_ASSERT_EQUAL((UINT8)0,ssm->BadDataReplacement());
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::EvaluateData_UseMaxVinLength(void)
{
    uint8_t temp[] = {0x31,0x32,0x33,0x34};
    SerialString_t vinString(temp,sizeof(temp));

    // setup the parameter values
    ssm->m_lookForTerminator = false;
    ssm->m_maxVinLength = 4;
    ssm->m_vinLength = 1;
    ssm->m_correctBadScannerData = false;

    // create the mock objects
    ssm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)ssm->m_dataBroker;

    // set the named data broker values
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    ssm->EvaluateData(vinString,4);

    // make sure the correct data was written to the named data broker
    CPPUNIT_ASSERT_EQUAL(2,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VINDISPLAY_DATA_TAG),
                         mndb->m_writeParamNodeName[0]);
    CPPUNIT_ASSERT_EQUAL(string("1234"),mndb->m_writeParamNodeValue[0]);
    CPPUNIT_ASSERT_EQUAL(string(NEXT_VIN_DATA_TAG),
                         mndb->m_writeParamNodeName[1]);
    CPPUNIT_ASSERT_EQUAL(string("1234"),mndb->m_writeParamNodeValue[1]);
}

//-----------------------------------------------------------------------------
void SymbolScannerMgrTest::EvaluateData_UseVinLength(void)
{
    uint8_t temp[] = {0x31,0x32,0x33};
    SerialString_t vinString(temp,sizeof(temp));

    // setup the parameter values
    ssm->m_lookForTerminator = false;
    ssm->m_maxVinLength = 4;
    ssm->m_vinLength = 1;
    ssm->m_correctBadScannerData = false;

    // create the mock objects
    ssm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)ssm->m_dataBroker;

    // set the named data broker values
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    ssm->EvaluateData(vinString,3);

    // make sure the correct data was written to the named data broker
    CPPUNIT_ASSERT_EQUAL(2,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VINDISPLAY_DATA_TAG),
                         mndb->m_writeParamNodeName[0]);
    CPPUNIT_ASSERT_EQUAL(string("3"),mndb->m_writeParamNodeValue[0]);
    CPPUNIT_ASSERT_EQUAL(string(NEXT_VIN_DATA_TAG),
                         mndb->m_writeParamNodeName[1]);
    CPPUNIT_ASSERT_EQUAL(string("3"),mndb->m_writeParamNodeValue[1]);
}
