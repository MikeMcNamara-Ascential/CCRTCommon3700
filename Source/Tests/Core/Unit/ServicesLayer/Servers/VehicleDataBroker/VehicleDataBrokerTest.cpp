#include "VehicleDataBrokerTest.h"
#include "VehicleDataBroker.h"
#include "MockNamedDataBroker.h"

using namespace std;

//-----------------------------------------------------------------------------
VehicleDataBrokerTest::VehicleDataBrokerTest()
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
        "/Core/Unit/ServicesLayer/Servers/VehicleDataBroker/Configuration/";
}

//-----------------------------------------------------------------------------
VehicleDataBrokerTest::~VehicleDataBrokerTest()
{
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::setUp()
{
    // register the name "core/NamedDataBroker" so we can create a mock 
    // NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    // create the test object
    vdb = new VehicleDataBroker();

    // create a mock named data broker
    vdb->m_ndb = new MockNamedDataBroker();
    mndb = (MockNamedDataBroker*)vdb->m_ndb;
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    delete vdb;

    // move to the next line for the next test name
    cout << endl;
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::TestConstructor()
{
    cout << " TestConstructor ";

//    CPPUNIT_ASSERT_EQUAL(false, vdb->m_idleStateForced);
    CPPUNIT_ASSERT(vdb->m_plantHostComm == NULL);
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::TestLoadAdditionalConfigurationItems(void)
{
    cout << " TestLoadAdditionalConfigurationItems ";

    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    vdb->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(
        string(VEHICLE_DATA_BROKER_NAME),
        document->getChild("Setup/Communication/Name")->getValue());
    CPPUNIT_ASSERT_EQUAL(string("core/WorkCellController"),vdb->GetPlantHost());
    CPPUNIT_ASSERT_EQUAL(string("VIN"),vdb->GetVinTag());
    CPPUNIT_ASSERT_EQUAL(string("VINDisplay"),vdb->GetVinDisplayTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumber"),vdb->GetDriverNumberTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumberDisplay"),
                         vdb->GetDriverNumberDisplayTag());
    CPPUNIT_ASSERT_EQUAL(string("StartTest"),vdb->GetStartTestTag());
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuild"),vdb->GetVehicleBuildTag());
    CPPUNIT_ASSERT_EQUAL(string("NextVehicleBuild"),
                         vdb->GetNextVehicleBuildTag());
    CPPUNIT_ASSERT_EQUAL(string("RollsDownElevUp"),vdb->GetRetainersDownTag());
    CPPUNIT_ASSERT_EQUAL(string("CableConnect"),vdb->GetCableConnectTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumberDisplayBGColor"),
                         vdb->GetDriverNumberBGColorTag());
    CPPUNIT_ASSERT_EQUAL(string("OverallTest"),vdb->GetOverallTestTag());

    CPPUNIT_ASSERT_EQUAL(
        string("Yes"),
        vdb->m_dataToPLC.getNode("FrontWheelDriveSelectedToPLC")->getValue());
    CPPUNIT_ASSERT_EQUAL(string("T0000xx"),
                         vdb->Read(vdb->GetDriverNumberTag()));
    CPPUNIT_ASSERT_EQUAL(string("white"),vdb->GetDriverNumberBGColor());
    CPPUNIT_ASSERT_EQUAL(true,vdb->m_useDefDriverNo);    
}

//-----------------------------------------------------------------------------
void 
VehicleDataBrokerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    cout << " TestLoadAdditionalConfigurationItems_MissingTags ";
    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    vdb->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(false,vdb->m_useDefDriverNo);    
}

//-----------------------------------------------------------------------------
void 
VehicleDataBrokerTest::TestLoadAdditionalConfigurationItems_InvalidName(void)
{
    cout << " TestLoadAdditionalConfigurationItems_InvalidName ";

    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfigInvalidName.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    vdb->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(
        string(VEHICLE_DATA_BROKER_NAME),
        document->getChild("Setup/Communication/Name")->getValue());
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::TestUpdatePLC(void)
{
    cout << " TestUpdatePLC ";

    string retVal="Fail";

    // setup the vehicle build data
    XmlNode *vehicleBuild = new XmlNode("VehicleBuild","");
    XmlNode fwd("FrontWheelDriveSelectedToPLC","Yes");
    XmlNode wb("WheelbasePositionInchesX10","1234");

    vehicleBuild->addChild(&fwd);
    vehicleBuild->addChild(&wb);

    // read in the configuration file
    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the data to be sent to the PLC
    vdb->SetPlcData(document->getChild("Setup/DataToPLC"));

    // set the data needed by the mock named data broker
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = vdb->UpdatePLC(vehicleBuild);

    // make sure the correct value was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retVal);

    // make sure the correct data was passed to the MockNamedDataBroker
    CPPUNIT_ASSERT_EQUAL(2,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string("FrontWheelDriveSelectedToPLC"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("WheelbasePositionInchesX10"),mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("1234"),mndb->m_writeParamValue[1]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[1]);
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::TestUpdatePLC_InvalidTag(void)
{
    cout << " TestUpdatePLC_InvalidTag ";

    string retVal="Fail";

    // setup the vehicle build data
    XmlNode *vehicleBuild = new XmlNode("VehicleBuild","");
    XmlNode fwd("FrontWheelDriveSelectedToPLC","Yes");

    vehicleBuild->addChild(&fwd);

    // read in the configuration file
    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfigInvalidName.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the data needed by the mock named data broker
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data to be sent to the PLC
    vdb->SetPlcData(document->getChild("Setup/DataToPLC"));

    // call the method under test
    retVal = vdb->UpdatePLC(vehicleBuild);

    // make sure the correct value was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retVal);

    // make sure the correct data was passed to the MockNamedDataBroker
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string("FrontWheelDriveSelectedToPLC"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mndb->m_writeParamIsFinal[0]);
}

//-----------------------------------------------------------------------------
void VehicleDataBrokerTest::TestUpdatePLC_Fail(void)
{
    cout << " TestUpdatePLC_Fail ";

    string retVal="Pass";

    // setup the vehicle build data
    XmlNode *vehicleBuild = new XmlNode("VehicleBuild","");
    XmlNode fwd("FrontWheelDriveSelectedToPLC","Yes");

    vehicleBuild->addChild(&fwd);

    // read in the configuration file
    XmlParser parser;
    string configFile = m_configPath + "VehicleDataBrokerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the data needed by the mock named data broker
    mndb->m_writeReturnResponse.push_back("FAIL");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_FAILURE);

    // set the data to be sent to the PLC
    vdb->SetPlcData(document->getChild("Setup/DataToPLC"));

    // call the method under test
    retVal = vdb->UpdatePLC(vehicleBuild);

    // make sure the correct value was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAILURE_RESPONSE),retVal);
}
