//******************************************************************************
// Description:
//    DAQ interface test class.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/DAQ/DAQInterfaceTest.cpp $
// 
// 1     5/01/07 1:44p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "DAQInterfaceTest.h"
#include "DAQInterface.h"
#include "PlantHostOutbound.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
DAQInterfaceTest::DAQInterfaceTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/DAQ/Configuration/";

    // set the USR_ROOT environment variable so that we can find the
    // configuration files
    m_originalUsrRoot = getenv("USR_ROOT");
    m_newUsrRoot = getenv("TEST_DIR_PATH");
    m_newUsrRoot += "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/DAQ/";
}

//-----------------------------------------------------------------------------
DAQInterfaceTest::~DAQInterfaceTest()
{
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::setUp()
{
    m_daq = new DAQInterface();
    // set the new USR_ROOT environment variable
    setenv("USR_ROOT",m_newUsrRoot.c_str(),1);
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::tearDown()
{
    // restore the USR_ROOT environment variable
    setenv("USR_ROOT",m_originalUsrRoot.c_str(),1);

    delete m_daq;
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::TestConstructor(void)
{
    CPPUNIT_ASSERT_EQUAL(false, m_daq->m_faultReadyToProcess);
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile(m_configPath + "DaqInterfaceConfig.xml");

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *daqNode = document->getChild("Setup/HostInterfaces/DaqInterface");

    m_daq->LoadAdditionalConfigurationItems(daqNode);

    CPPUNIT_ASSERT_EQUAL(5,m_daq->m_machineNumberLength);
    CPPUNIT_ASSERT_EQUAL(string("M"),m_daq->m_measurementSystem);
    CPPUNIT_ASSERT_EQUAL(string("LossCompensation"),
                         m_daq->m_lossCompensationMap.getNode(0)->getValue());

    string testResultHeaderCompareString((char)TEST_RESULT_STX + string(" 0000 M "));
    CPPUNIT_ASSERT_EQUAL(testResultHeaderCompareString,m_daq->m_testResultHeader);

    string lossCompensationHeaderCompareString((char)LOSS_COMPENSATION_STX + string(" 0000 "));
    CPPUNIT_ASSERT_EQUAL(lossCompensationHeaderCompareString,
                         m_daq->m_lossCompensationHeader);

    string machineFaultHeaderCompareString((char)MACHINE_FAULT_STX + string(" 0000 "));
    CPPUNIT_ASSERT_EQUAL(machineFaultHeaderCompareString,
                         m_daq->m_machineFaultHeader);
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile(m_configPath + "DaqInterfaceConfigMissingTags.xml");

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *daqNode = document->getChild("Setup/HostInterfaces/DaqInterface");

    m_daq->LoadAdditionalConfigurationItems(daqNode);

    CPPUNIT_ASSERT_EQUAL(4,m_daq->m_machineNumberLength);
    CPPUNIT_ASSERT_EQUAL(string("E"),m_daq->m_measurementSystem);

    string testResultHeaderCompareString((char)TEST_RESULT_STX + string(" 0000 E "));
    CPPUNIT_ASSERT_EQUAL(testResultHeaderCompareString,m_daq->m_testResultHeader);
}

//-----------------------------------------------------------------------------
void DAQInterfaceTest::TestWriteFaultCode(void)
{   // Create a node to write the data
    XmlNode faultNode(faultCodeTag, "6");
    // Write the data to the interface
    m_daq->Write(&faultNode);
    CPPUNIT_ASSERT_EQUAL(6, m_daq->FaultCode());
    CPPUNIT_ASSERT_EQUAL(true, m_daq->FaultReadyToProcess());
}
