//******************************************************************************
// Description:
//    Host interface test class.
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/HostInterface/HostInterfaceTest.cpp $
// 
// 1     11/02/07 2:15p Derickso
// Updated check for MessageID return type.
// 
// 1     5/01/07 1:46p Rwiersem
// Changes for the 07050201 core release:
// 
// Intial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "HostInterfaceTest.h"
#include "HostInterface.h"
#include "MockNamedDataBroker.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
HostInterfaceTest::HostInterfaceTest() : CppUnit::TestFixture()
{   // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" 
            << endl;
        exit(1);
    }
    // set the USR_ROOT environment variable so that we can find the
    // configuration files
    m_originalUsrRoot = getenv("USR_ROOT");
    m_newUsrRoot = getenv("TEST_DIR_PATH");
    m_newUsrRoot += 
        "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/HostInterface/";

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += "/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/HostInterface/Configuration/";
}

//-----------------------------------------------------------------------------
HostInterfaceTest::~HostInterfaceTest()
{
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::setUp()
{   
    // set the new USR_ROOT environment variable
    setenv("USR_ROOT",m_newUsrRoot.c_str(),1);

    // register the name "core/NamedDataBroker" so plant host outbound can
    // create an INamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");
    m_hostInterface = new HostInterface();
    m_hostInterface->m_ndb = new MockNamedDataBroker();
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::tearDown()
{   
    // restore the USR_ROOT environment variable
    setenv("USR_ROOT",m_originalUsrRoot.c_str(),1);

    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);
    delete m_hostInterface;
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestConstructor(void)
{
    CPPUNIT_ASSERT_EQUAL(false, m_hostInterface->m_resultReadyToProcess);
    CPPUNIT_ASSERT(NULL == m_hostInterface->m_plantHostComm);
    CPPUNIT_ASSERT(NULL == m_hostInterface->m_testResult);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestLoadAdditionalConfigItems(void)
{   // Set the host interface name
    string interfaceName("SampleInterface");
    m_hostInterface->HostInterfaceName(&interfaceName);
    // Initialize the interface
    XmlParser parser;
    const XmlNode *config = parser.ReturnXMLDocument(m_configPath + "HostInterfaceConfig.xml")->getChild("Setup/HostInterfaces/SampleInterface");
    m_hostInterface->LoadAdditionalConfigurationItems(config);
    // Check the intialized items
    CPPUNIT_ASSERT(!m_hostInterface->HostSystemName().compare("SampleInterface"));
    CPPUNIT_ASSERT_EQUAL(14, m_hostInterface->VinIndex());
    CPPUNIT_ASSERT_EQUAL(2, m_hostInterface->MaxHostResponseTime());
    CPPUNIT_ASSERT_EQUAL(30, m_hostInterface->MaxMsgSendAttempts());
    CPPUNIT_ASSERT_EQUAL((UINT8)0x19, m_hostInterface->MessageId());
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestStoreMachineNumber(void)
{   
    // set the data needed by the MockNamedDataBroker
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)m_hostInterface->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getNextReturnMessage.push_back("PASS");
    mndb->m_getNextReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getNextReturnDataValue.push_back("1234");

    // Command the interface to store the machine number
    m_hostInterface->StoreMachineNumber();

    // Verify the machine number is correct
    CPPUNIT_ASSERT(!m_hostInterface->GetMachineNumber().compare("1234"));
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_Detail(void)
{
    string     result;
    XmlNode*   testResultDescNode = new XmlNode("Field00080","MachineDriverNumber");
    XmlNode    testResultNode("MachineDriverNumber","");
    XmlNodeMap resultMap;
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32      totalSerialNumbers=0;

    testResultDescNode->addChild("Type","Detail",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DetailNode","DriverNumber",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Format","%7.7s",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DataType","String",ATTRIBUTE_NODE);
    testResultDescNode->addChild("ExcludeIfNotTested","No",ATTRIBUTE_NODE);

    testResultNode.addChild("DriverNumber","1234567",ELEMENT_NODE);

    resultMap.addNode(&testResultNode);

    result = m_hostInterface->ProcessMapNode(testResultDescNode,resultMap,vehicleData, 
                                             resultConversions,totalSerialNumbers);
    delete testResultDescNode;

    CPPUNIT_ASSERT_EQUAL(string("1234567"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_VehicleBuild(void)
{
    string     result;
    XmlNodeMap resultMap;
    XmlNode*   vehicleBuildDescNode = new XmlNode("VehicleBuild","VIN");
    XmlNode    vehicleBuildNode("VIN","123456");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32      totalSerialNumbers=0;

    vehicleBuildDescNode->addChild("Type","VehicleBuild",ATTRIBUTE_NODE);
    vehicleBuildDescNode->addChild("Format","%5.5s",ATTRIBUTE_NODE);

    vehicleData.addNode(&vehicleBuildNode);

    result = m_hostInterface->ProcessMapNode(vehicleBuildDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete vehicleBuildDescNode;

    CPPUNIT_ASSERT_EQUAL(string("12345"), result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_VehicleBuildWithLength(void)
{
    string     result;
    XmlNodeMap resultMap;
    XmlNode*   vehicleBuildDescNode = new XmlNode("VehicleBuild","VIN");
    XmlNode    vehicleBuildNode("VIN","123456");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32      totalSerialNumbers=0;

    vehicleBuildDescNode->addChild("Type","VehicleBuild",ATTRIBUTE_NODE);
    vehicleBuildDescNode->addChild("Format","%s",ATTRIBUTE_NODE);
    vehicleBuildDescNode->addChild("Length","4",ATTRIBUTE_NODE);

    vehicleData.addNode(&vehicleBuildNode);

    result = m_hostInterface->ProcessMapNode(vehicleBuildDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete vehicleBuildDescNode;

    CPPUNIT_ASSERT_EQUAL(string("1234"), result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_ValueString(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode* machineNumberDescNode = new XmlNode("MachineNumberDesc","MachineNumber");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    machineNumberDescNode->addChild("Type","Value",ATTRIBUTE_NODE);
    machineNumberDescNode->addChild("Format","%3.3s",ATTRIBUTE_NODE);

    // set the data needed by the MockNamedDataBroker
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)m_hostInterface->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getNextReturnMessage.push_back("PASS");
    mndb->m_getNextReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getNextReturnDataValue.push_back("123");

    result = m_hostInterface->ProcessMapNode(machineNumberDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete machineNumberDescNode;

    CPPUNIT_ASSERT_EQUAL(string("123"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_ValueInt(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode* engineRpmDescNode = new XmlNode("EngineRpmDesc","EngineRPM");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    engineRpmDescNode->addChild("Type","Value",ATTRIBUTE_NODE);
    engineRpmDescNode->addChild("Format","%04d",ATTRIBUTE_NODE);
    engineRpmDescNode->addChild("DataType","Integer",ATTRIBUTE_NODE);

    // set the data needed by the MockNamedDataBroker
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)m_hostInterface->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getNextReturnMessage.push_back("PASS");
    mndb->m_getNextReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getNextReturnDataValue.push_back("0123");

    result = m_hostInterface->ProcessMapNode(engineRpmDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete engineRpmDescNode;

    CPPUNIT_ASSERT_EQUAL(string("0123"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_ValueIntWithLength(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode* engineRpmDescNode = new XmlNode("EngineRpmDesc","EngineRPM");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    engineRpmDescNode->addChild("Type","Value",ATTRIBUTE_NODE);
    engineRpmDescNode->addChild("Format","%d",ATTRIBUTE_NODE);
    engineRpmDescNode->addChild("DataType","Integer",ATTRIBUTE_NODE);
    engineRpmDescNode->addChild("Length","4",ATTRIBUTE_NODE);

    // set the data needed by the MockNamedDataBroker
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)m_hostInterface->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getNextReturnMessage.push_back("PASS");
    mndb->m_getNextReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getNextReturnDataValue.push_back("0123");

    result = m_hostInterface->ProcessMapNode(engineRpmDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete engineRpmDescNode;

    CPPUNIT_ASSERT_EQUAL(string("0123"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_ValueFloat(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode* lfDragDescNode =
        new XmlNode("LfDragDesc","BrakeLeftFrontDragTest");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    lfDragDescNode->addChild("Type","Value",ATTRIBUTE_NODE);
    lfDragDescNode->addChild("Format","%07.2f",ATTRIBUTE_NODE);
    lfDragDescNode->addChild("DataType","Float",ATTRIBUTE_NODE);

    // set the data needed by the MockNamedDataBroker
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)m_hostInterface->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getNextReturnMessage.push_back("PASS");
    mndb->m_getNextReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getNextReturnDataValue.push_back("0123.45");

    result = m_hostInterface->ProcessMapNode(lfDragDescNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);
    delete lfDragDescNode;

    CPPUNIT_ASSERT_EQUAL(string("0123.45"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_DateTime(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode dateTimeNode("DateTimeFormat","%F");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    dateTimeNode.addChild("Type","DateTime",ATTRIBUTE_NODE);

    result = m_hostInterface->ProcessMapNode(&dateTimeNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);

    // store the current date
    time_t systemTime = time(NULL);
    struct tm *currentTime = localtime(&systemTime);
    string dateFormat("%F");
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), dateFormat.c_str(), currentTime);

    CPPUNIT_ASSERT_EQUAL(string(timeBuffer),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestProcessMapNode_DateTimeWithLength(void)
{
    string result;
    XmlNodeMap resultMap;
    XmlNode dateTimeNode("DateTimeFormat","%F%H");
    XmlNodeMap vehicleData;
    XmlNodeMap resultConversions;
    INT32 totalSerialNumbers=0;

    dateTimeNode.addChild("Type","DateTime",ATTRIBUTE_NODE);

    // set to only use the date portion (yyyy-mm-dd)
    dateTimeNode.addChild("Length","10",ATTRIBUTE_NODE);

    result = m_hostInterface->ProcessMapNode(&dateTimeNode,resultMap,vehicleData,
                                             resultConversions,totalSerialNumbers);

    // store the current date
    time_t systemTime = time(NULL);
    struct tm *currentTime = localtime(&systemTime);
    string dateFormat("%F");
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), dateFormat.c_str(), currentTime);

    CPPUNIT_ASSERT_EQUAL(string(timeBuffer),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestDetailValue_String(void)
{
    string     result;
    XmlNode*   testResultDescNode =new XmlNode("Field00080","MachineDriverNumber");
    XmlNode    testResultNode("MachineDriverNumber","");
    XmlNodeMap resultMap;

    testResultDescNode->addChild("Type","Detail",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DetailNode","DriverNumber",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Format","%s",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DataType","String",ATTRIBUTE_NODE);
    testResultDescNode->addChild("ExcludeIfNotTested","No",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Length","7",ATTRIBUTE_NODE);

    testResultNode.addChild("DriverNumber","1234567",ELEMENT_NODE);

    resultMap.addNode(&testResultNode);

    result = m_hostInterface->GetTestDetailValue(resultMap,testResultDescNode);
    delete testResultDescNode;

    CPPUNIT_ASSERT_EQUAL(string("1234567"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestDetailValue_Int(void)
{
    string result;
    XmlNode* testResultDescNode
        = new XmlNode("Field00075","EmissionsCheckIdleSpeed");
    XmlNode testResultNode("EmissionsCheckIdleSpeed","");
    XmlNodeMap resultMap;

    testResultDescNode->addChild("Type","Detail",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DetailNode","EngineRPM",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Format","%04d",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DataType","Integer",ATTRIBUTE_NODE);
    testResultDescNode->addChild("ExcludeIfNotTested","No",ATTRIBUTE_NODE);

    testResultNode.addChild("EngineRPM","1234",ELEMENT_NODE);

    resultMap.addNode(&testResultNode);

    result = m_hostInterface->GetTestDetailValue(resultMap,testResultDescNode);
    delete testResultDescNode;

    CPPUNIT_ASSERT_EQUAL(string("1234"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestDetailValue_Float(void)
{
    string result;
    XmlNode* testResultDescNode =
        new XmlNode("Field0030","BrakeLeftFrontDragTest");
    XmlNode testResultNode("BrakeLeftFrontDragTest","");
    XmlNodeMap resultMap;

    testResultDescNode->addChild("Type","Detail",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DetailNode","DragForce",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Format","%07.2f",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DataType","Float",ATTRIBUTE_NODE);

    testResultNode.addChild("DragForce","1234.56",ELEMENT_NODE);

    resultMap.addNode(&testResultNode);

    result = m_hostInterface->GetTestDetailValue(resultMap,testResultDescNode);
    delete testResultDescNode;

    CPPUNIT_ASSERT_EQUAL(string("1234.56"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestDetailValue_NoLength(void)
{
    string result;
    XmlNode* testResultDescNode
        = new XmlNode("Field0030","BrakeLeftFrontDragTest");
    XmlNode testResultNode("BrakeLeftFrontDragTest","");
    XmlNodeMap resultMap;

    testResultDescNode->addChild("Type","Detail",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DetailNode","DragForce",ATTRIBUTE_NODE);
    testResultDescNode->addChild("Format","%r",ATTRIBUTE_NODE);
    testResultDescNode->addChild("DataType","Float",ATTRIBUTE_NODE);
    testResultDescNode->addChild("ExcludeIfNotTested","Yes",ATTRIBUTE_NODE);

    testResultNode.addChild("DragForce","1234.56",ELEMENT_NODE);

    resultMap.addNode(&testResultNode);

    result = m_hostInterface->GetTestDetailValue(resultMap,testResultDescNode);
    delete testResultDescNode;

    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestApplyDataFormatting_String(void)
{
    string result;

    // test that uses only one letter of the result string and has the length
    // passed in
    result = m_hostInterface->ApplyDataFormatting(typeString,"Pass","%2.2s",1,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("P"),result);

    // test that uses the full result string and gets the length from the format
    result = m_hostInterface->ApplyDataFormatting(typeString,"Pass","%4.4s",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("Pass"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestApplyDataFormatting_Int(void)
{
    string result;

    // test that uses the full field width
    result = m_hostInterface->ApplyDataFormatting(typeInt,"12345","%5d",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("12345"),result);

    // test that should pad with spaces
    result = m_hostInterface->ApplyDataFormatting(typeInt,"1234","%d",5,1.0,0.0," ");
    CPPUNIT_ASSERT_EQUAL(string(" 1234"),result);

    // test that should pad with zeros
    result = m_hostInterface->ApplyDataFormatting(typeInt,"1234","%05d",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("01234"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestApplyDataFormatting_Float(void)
{
    string result;

    // test that uses the full field width
    result = m_hostInterface->ApplyDataFormatting(typeFloat,"1234.56","%.2f",7,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("1234.56"),result);

    // test that should be padded with spaces
    result = m_hostInterface->ApplyDataFormatting(typeFloat,"123.45","%7.2f",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string(" 123.45"),result);

    // test that should be padded with zeros
    result = m_hostInterface->ApplyDataFormatting(typeFloat,"123.45","%07.2f",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("0123.45"),result);

    // test that should add a zero for precision
    result = m_hostInterface->ApplyDataFormatting(typeFloat,"1234.5","%7.2f",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string("1234.50"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestApplyDataFormatting_UnknownType(void)
{
    string result;

    result = m_hostInterface->ApplyDataFormatting("Character","A","%c",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestApplyDataFormatting_NoLength(void)
{
    string result;

    result = m_hostInterface->ApplyDataFormatting(typeInt,"1234","%r",0,1.0,0.0);
    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_StringWithPadding(void)
{
    string result;

    result = m_hostInterface->AdjustDataValueLength(typeString,"Pass",5,"0");
    CPPUNIT_ASSERT_EQUAL(string("0Pass"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_StringWithTrim(void)
{
    string result;

    result = m_hostInterface->AdjustDataValueLength(typeString,"1234",3,"0");
    CPPUNIT_ASSERT_EQUAL(string("123"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_IntWithTrim(void)
{
    string result;

    // test with an integer value that is greater than the specified field
    // width.
    result = m_hostInterface->AdjustDataValueLength(typeInt,"12345",4,"0");
    CPPUNIT_ASSERT_EQUAL(string("9999"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_IntWithPadding(void)
{
    string result;

    result = m_hostInterface->AdjustDataValueLength(typeInt,"123",4,"0");
    CPPUNIT_ASSERT_EQUAL(string("0123"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_FloatWithValueTrim(void)
{
    string result;

    // test with a float value that is greater than the specified field width
    result = m_hostInterface->AdjustDataValueLength(typeFloat,"1234",3,"0");
    CPPUNIT_ASSERT_EQUAL(string("999"),result);
}

//-----------------------------------------------------------------------------
void
HostInterfaceTest::TestAdjustDataValueLength_FloatWithPrecisionTrim(void)
{
    string result;

    // test with a float value that needs precision trimmed to fit in the
    // specified field width
    result = m_hostInterface->AdjustDataValueLength(typeFloat,"1234.56",6,"0");
    CPPUNIT_ASSERT_EQUAL(string("1234.5"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_FloatWithPadding(void)
{
    string result;

    result = m_hostInterface->AdjustDataValueLength(typeFloat,"123.4",6,"0");
    CPPUNIT_ASSERT_EQUAL(string("0123.4"),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestAdjustDataValueLength_UnknownType(void)
{
    string result;

    result = m_hostInterface->AdjustDataValueLength("Character","A",0,"0");
    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetFormatAttribute(void)
{
    string format;
    XmlNode* machineNumberDescNode =
        new XmlNode("MachineNumberDesc","MachineNumber");

    machineNumberDescNode->addChild("Format","%7.7s",ATTRIBUTE_NODE);
    format = m_hostInterface->GetFormatAttribute(machineNumberDescNode);
    delete machineNumberDescNode;

    CPPUNIT_ASSERT_EQUAL(string("%7.7s"),format);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetFormatAttribute_NoFormat(void)
{
    string format;
    XmlNode* machineNumberDescNode =
        new XmlNode("MachineNumberDesc","MachineNumber");

    format = m_hostInterface->GetFormatAttribute(machineNumberDescNode);
    delete machineNumberDescNode;

    // no format specified, so default to %s
    CPPUNIT_ASSERT_EQUAL(string("%s"),format);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetDataTypeAttribute(void)
{
    string dataType;
    XmlNode* machineNumberDescNode =
        new XmlNode("MachineNumberDesc","MachineNumber");

    machineNumberDescNode->addChild("DataType","Integer",ATTRIBUTE_NODE);
    dataType = m_hostInterface->GetDataTypeAttribute(machineNumberDescNode);
    delete machineNumberDescNode;

    CPPUNIT_ASSERT_EQUAL(string("Integer"),dataType);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetDataTypeAttribute_NoDataType(void)
{
    string dataType;
    XmlNode* machineNumberDescNode =
        new XmlNode("MachineNumberDesc","MachineNumber");

    dataType = m_hostInterface->GetDataTypeAttribute(machineNumberDescNode);
    delete machineNumberDescNode;

    // no data type specified, so default to string
    CPPUNIT_ASSERT_EQUAL(string("String"),dataType);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetDataLengthAttribute(void)
{
    INT32 length;
    XmlNode* descNode = new XmlNode("Description","Value");

    descNode->addChild("Length","5",ATTRIBUTE_NODE);
    length = m_hostInterface->GetDataLengthAttribute(descNode);
    delete descNode;

    CPPUNIT_ASSERT_EQUAL(5,length);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetDataLengthAttribute_NoLength(void)
{
    INT32 length;
    XmlNode* descNode = new XmlNode("Description","Value");

    length = m_hostInterface->GetDataLengthAttribute(descNode);
    delete descNode;

    CPPUNIT_ASSERT_EQUAL(0,length);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestResult_NoFormat(void)
{
    XmlNodeMap testResultMap;
    XmlNode resultStringField("TestResult1","Result");
    XmlNodeMap resultConversions;
    string retVal;

    // create the resultNode
    XmlNode resultNode("Result","");
    resultNode.addChild("Result","Pass",ATTRIBUTE_NODE);

    // add the result node to the test result node map
    testResultMap.addNode(&resultNode);

    // create and add the pass conversion
    XmlNode passConversion("Pass","P");
    resultConversions.addNode(&passConversion);

    // call the method under test
    retVal = m_hostInterface->GetTestResult(testResultMap,&resultStringField,
                                resultConversions);

    // make sure the correct conversion was returned
    CPPUNIT_ASSERT_EQUAL(string("P"),retVal);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestResult_WithFormat(void)
{
    XmlNodeMap testResultMap;
    XmlNode resultStringField("TestResult1","Result");
    XmlNodeMap resultConversions;
    string retVal;

    // create the resultNode
    XmlNode resultNode("Result","");
    resultNode.addChild("Result","Pass",ATTRIBUTE_NODE);

    // add the result node to the test result node map
    testResultMap.addNode(&resultNode);

    // create and add the pass conversion
    XmlNode passConversion("Pass","P");
    resultConversions.addNode(&passConversion);

    // add the formatting attribute
    resultStringField.addChild("Format","%2s",ATTRIBUTE_NODE);

    // call the method under test
    retVal = m_hostInterface->GetTestResult(testResultMap,&resultStringField,
                                resultConversions);

    // make sure the correct conversion was returned
    CPPUNIT_ASSERT_EQUAL(string(" P"),retVal);
}

//-----------------------------------------------------------------------------
void HostInterfaceTest::TestGetTestResult_MissingResult(void)
{
    XmlNodeMap testResultMap;
    XmlNode resultStringField("TestResult1","Result");
    XmlNodeMap resultConversions;
    string retVal;

    // call the method under test
    retVal = m_hostInterface->GetTestResult(testResultMap,&resultStringField,
                                resultConversions);

    // make sure the correct result was returned
    CPPUNIT_ASSERT_EQUAL(string("Z"),retVal);
}

