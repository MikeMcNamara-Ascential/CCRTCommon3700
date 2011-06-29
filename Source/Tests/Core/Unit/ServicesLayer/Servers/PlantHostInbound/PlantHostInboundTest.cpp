#include "PlantHostInboundTest.h"
#include "PlantHostInbound.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
PlantHostInboundTest::PlantHostInboundTest()
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
    m_configPath +=
        "/Core/Unit/ServicesLayer/Servers/PlantHostInbound/Configuration/";
}

//-----------------------------------------------------------------------------
PlantHostInboundTest::~PlantHostInboundTest()
{
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::setUp()
{
    // get the current FTP_ROOT environment variable
    m_origFtpRoot = getenv("FTP_ROOT");

    // set the FTP_ROOT environment variable to use for testing
    m_newFtpRoot = getenv("TEST_DIR_PATH");
    m_newFtpRoot += "/Core/Unit/ServicesLayer/Servers/PlantHostInbound/";
    setenv("FTP_ROOT",m_newFtpRoot.c_str(),1);

    phi = new PlantHostInbound();
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::tearDown()
{
    // restore the FTP_ROOT environment variable
    setenv("FTP_ROOT",m_origFtpRoot.c_str(),1);

    delete phi;
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestConstructor(void)
{
    CPPUNIT_ASSERT(phi->m_broker == NULL);
    CPPUNIT_ASSERT_EQUAL(string("/VehicleData/BuildRecords/"),
                         phi->m_vehicleBuildFilePath);
    CPPUNIT_ASSERT_EQUAL(true,phi->m_updateVINReadStatus);
    CPPUNIT_ASSERT_EQUAL(true,phi->m_invalidateBuildRecordOnRead);
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestLoadAdditionalConfigurationItems(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlantHostInboundConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    phi->LoadAdditionalConfigurationItems(document);

    string vehicleBuildPath;
    vehicleBuildPath += getenv("FTP_ROOT");
    vehicleBuildPath += "/Configuration/VehicleTest/BuildRecords1/";
    CPPUNIT_ASSERT_EQUAL(vehicleBuildPath,phi->m_vehicleBuildFilePath);
    CPPUNIT_ASSERT_EQUAL(string("Broadcast"),phi->m_vehicleBuildRecordSource);
    CPPUNIT_ASSERT_EQUAL(string("InputServerState1"),
                         phi->m_inputServerStateTag);
    CPPUNIT_ASSERT_EQUAL(string("NextVehicleBuild1"),phi->m_vehicleBuildTag);
    CPPUNIT_ASSERT_EQUAL(string("BuildRecord1"),phi->m_buildRecordTag);
    CPPUNIT_ASSERT_EQUAL(string("VIN1"),phi->m_vinTag);
    CPPUNIT_ASSERT_EQUAL(string("NextVIN1"),phi->m_nextVinTag);
    CPPUNIT_ASSERT_EQUAL(string("VINDisplay1"),phi->m_vinDisplayTag);
    CPPUNIT_ASSERT_EQUAL(string("VINReadStatus1"),phi->m_vinReadStatusTag);
    CPPUNIT_ASSERT_EQUAL(string("VehiclePresent1"),phi->m_vehiclePresentTag);
    CPPUNIT_ASSERT_EQUAL(string("RollsDownElevUp1"),phi->m_retainersDownTag);
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuildRecordStatus1"),
                         phi->m_vehicleBuildStatusTag);
    CPPUNIT_ASSERT_EQUAL(
        string("Undefined/Automatic"),
        phi->m_buildRecordMap.getNode("Transmission")->getChild("Type00")->getValue());
    CPPUNIT_ASSERT_EQUAL(string("No"),
                         phi->m_fourWheelDrive.getNode("AWD")->getValue());
    CPPUNIT_ASSERT_EQUAL(string("Yes"),
                         phi->m_allWheelDrive.getNode("AWD")->getValue());
    CPPUNIT_ASSERT_EQUAL(
        string("160.5"),
        phi->m_wheelBasePositions.getNode("Vehicle5")->getAttribute("Wheelbase")->getValue());
    CPPUNIT_ASSERT_EQUAL(string("WheelbasePositionInchesX101"),
                         phi->m_wheelbasePositionTag);
    CPPUNIT_ASSERT_EQUAL(false,phi->m_invalidateBuildRecordOnRead);
    CPPUNIT_ASSERT_EQUAL((UINT16)0,phi->m_vehicleBuildFileVinStart);
    CPPUNIT_ASSERT_EQUAL((UINT16)9,phi->m_vehicleBuildFileVinLength);
}

//-----------------------------------------------------------------------------
void
PlantHostInboundTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    XmlParser parser;
    string configFile = m_configPath + "PlantHostInboundConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    phi->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(string("InputServerState"),
                         phi->m_inputServerStateTag);
    CPPUNIT_ASSERT_EQUAL(string("NextVehicleBuild"),phi->m_vehicleBuildTag);
    CPPUNIT_ASSERT_EQUAL(string("BuildRecord"),phi->m_buildRecordTag);
    CPPUNIT_ASSERT_EQUAL(string("VIN"),phi->m_vinTag);
    CPPUNIT_ASSERT_EQUAL(string("NextVIN"),phi->m_nextVinTag);
    CPPUNIT_ASSERT_EQUAL(string("VINDisplay"),phi->m_vinDisplayTag);
    CPPUNIT_ASSERT_EQUAL(string("VINReadStatus"),phi->m_vinReadStatusTag);
    CPPUNIT_ASSERT_EQUAL(string("VehiclePresent"),phi->m_vehiclePresentTag);
    CPPUNIT_ASSERT_EQUAL(string("RollsDownElevUp"),phi->m_retainersDownTag);
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuildRecordStatus"),
                         phi->m_vehicleBuildStatusTag);

    // The node maps should be empty.  So, trying to get a node should throw
    // an XmlException
    CPPUNIT_ASSERT_THROW(phi->m_fourWheelDrive.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(phi->m_allWheelDrive.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(phi->m_wheelBasePositions.getNode(0),XmlException);

    CPPUNIT_ASSERT_EQUAL(string("WheelbasePositionInchesX10"),
                         phi->m_wheelbasePositionTag);
    CPPUNIT_ASSERT_EQUAL(true,phi->m_invalidateBuildRecordOnRead);
    CPPUNIT_ASSERT_EQUAL(uniqueVinStart,phi->m_vehicleBuildFileVinStart);
    CPPUNIT_ASSERT_EQUAL(minVinLength,phi->m_vehicleBuildFileVinLength);
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestLoadVehicleBuildFromFile(void)
{
    // Set the VIN characters to use for a file name
    phi->SetVehicleBuildFileVinStart(0);
    phi->SetVehicleBuildFileVinLength(8);

    // Set the path to the vehicle build files
    phi->SetVehicleBuildFilePath(m_configPath);

    // Load the specified vehicle build file
    phi->LoadVehicleBuildFromFile("12345678901234567",phi->m_vehicleBuild,true);

    // Check the <PerformAbortTest> to see if the vehicle build information was
    // read in correctly
    const XmlNode* abortNode = phi->m_vehicleBuild.getNode("PerformAbortTest");
    CPPUNIT_ASSERT_EQUAL(string("P"),abortNode->getValue());

    // make sure the <VIN> node was filled in from the passed in data
    const XmlNode* vinNode = phi->m_vehicleBuild.getNode("VIN");
    CPPUNIT_ASSERT_EQUAL(string("12345678901234567"),vinNode->getValue());
}

//-----------------------------------------------------------------------------
void 
PlantHostInboundTest::TestTranslateBuildRecord_PerformAbortTestWithChild(void)
{
    string buildRecord("BrakeTest ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode abortNode("PerformAbortTest","BrakeTest");
    abortNode.addChild("StartIndex","0",ATTRIBUTE_NODE);
    abortNode.addChild("Length","9",ATTRIBUTE_NODE);
    abortNode.addChild("Type","LiteralCode",ATTRIBUTE_NODE);
    abortNode.addChild("BrakeTest","BrakeAbort",ELEMENT_NODE);

    // add the node the node map
    specificMap.addNode(&abortNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    CPPUNIT_ASSERT_EQUAL(
        string("BrakeAbort"),
        phi->m_vehicleBuild.getNode("PerformAbortTest")->getValue());
}

//-----------------------------------------------------------------------------
void 
PlantHostInboundTest::TestTranslateBuildRecord_PerformAbortTestNoChild(void)
{
    string buildRecord("BrakeTest ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode abortNode("PerformAbortTest","BrakeTest");
    abortNode.addChild("StartIndex","0",ATTRIBUTE_NODE);
    abortNode.addChild("Length","9",ATTRIBUTE_NODE);
    abortNode.addChild("Type","Code",ATTRIBUTE_NODE);

    // add the node the node map
    specificMap.addNode(&abortNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    CPPUNIT_ASSERT_EQUAL(
        string("Abort"),
        phi->m_vehicleBuild.getNode("PerformAbortTest")->getValue());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestTranslateBuildRecord_WithTranslation(void)
{
    string buildRecord("Pass ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode resultNode("BrakeTest","Pass");
    resultNode.addChild("StartIndex","0",ATTRIBUTE_NODE);
    resultNode.addChild("Length","4",ATTRIBUTE_NODE);
    resultNode.addChild("Type","Code",ATTRIBUTE_NODE);
    resultNode.addChild("TypePass","P",ELEMENT_NODE);

    // add the node the node map
    specificMap.addNode(&resultNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    CPPUNIT_ASSERT_EQUAL(
        string("P"),
        phi->m_vehicleBuild.getNode("BrakeTest")->getValue());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestTranslateBuildRecord_TranslationMissing(void)
{
    string buildRecord("Pass ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode resultNode("BrakeTest","Pass");
    resultNode.addChild("StartIndex","0",ATTRIBUTE_NODE);
    resultNode.addChild("Length","4",ATTRIBUTE_NODE);
    resultNode.addChild("Type","Code",ATTRIBUTE_NODE);

    // add the node the node map
    specificMap.addNode(&resultNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    CPPUNIT_ASSERT_EQUAL(
        string("Undefined"),
        phi->m_vehicleBuild.getNode("BrakeTest")->getValue());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestTranslateBuildRecord_InvalidStartIndex(void)
{
    string buildRecord(" ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode resultNode("BrakeTest","Pass");
    resultNode.addChild("StartIndex","5",ATTRIBUTE_NODE);
    resultNode.addChild("Length","4",ATTRIBUTE_NODE);

    // add the node the node map
    specificMap.addNode(&resultNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    // make sure the node was not added to the vehcile build since the start
    // index was invalid
    CPPUNIT_ASSERT_THROW(phi->m_vehicleBuild.getNode("BrakeTest"),XmlException);
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestTranslateBuildRecord_ExceptionThrown(void)
{
    string buildRecord(" ");
    XmlNodeMap specificMap;

    // create the node to pass to TranslateBuildRecord()
    XmlNode resultNode("BrakeTest","Pass");
    resultNode.addChild("StartIndex","5",ATTRIBUTE_NODE);

    // add the node the node map
    specificMap.addNode(&resultNode);

    // call the method under test
    phi->TranslateBuildRecord(buildRecord,phi->m_vehicleBuild,true,specificMap);

    // make sure the node was not added to the vehcile build since an exeption
    // should have been thrown due to the missing length attribute
    CPPUNIT_ASSERT_THROW(phi->m_vehicleBuild.getNode("BrakeTest"),XmlException);
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::TestTranslateBuildRecordHook(void)
{
    BEP_STATUS_TYPE retVal;
    string fieldType("fieldType");
    string tag("tag");
    string value("value");

    retVal = phi->TranslateBuildRecordHook(&fieldType,tag,value);

    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_NA,retVal);
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::GetAbortPrompt(void)
{
    phi->m_abortStatusPrompt = "AbortPrompt";

    CPPUNIT_ASSERT_EQUAL(string("AbortPrompt"),phi->GetAbortPrompt());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::GetAbortColor(void)
{
    phi->m_abortStatusColor = "red";

    CPPUNIT_ASSERT_EQUAL(string("red"),phi->GetAbortColor());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::GetBlockPrompt(void)
{
    phi->m_testBlockPrompt = "TestBlockPrompt";

    CPPUNIT_ASSERT_EQUAL(string("TestBlockPrompt"),phi->GetBlockPrompt());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::GetShipPrompt(void)
{
    phi->m_shipStatusPrompt = "ShipStatusPrompt";

    CPPUNIT_ASSERT_EQUAL(string("ShipStatusPrompt"),phi->GetShipPrompt());
}

//-----------------------------------------------------------------------------
void PlantHostInboundTest::GetShipStatusColor(void)
{
    phi->m_shipStatusColor = "green";

    CPPUNIT_ASSERT_EQUAL(string("green"),phi->GetShipStatusColor());
}
