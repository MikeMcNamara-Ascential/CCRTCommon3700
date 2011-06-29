//******************************************************************************
// Description:
//   Test class for testing the Vehicle Data Server class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/VehicleDataServer/VehicleDataServerTest.cpp $
// 
// 5     10/31/07 10:29a Rwiersem
// Changes for the 07103101 core release:
// 
// - Removed TestHandlePulse().
// - Changed Read() to GetData() in PublishTest().
// 
// 4     10/26/06 3:16p Rwiersem
// Changes for the 06102701 core release:
// 
// Now register the name "core/NamedDataBroker".
// 
// 3     9/06/06 10:27a Rwiersem
// Changes for the 06090601 core release:
// 
// - Added publishing of PLC wheelbase bits in WheelbaseTest1()
// 
// 2     8/02/06 8:35p Cward
// Updated VehicleDataServer Tests to test for compatibility with older
// builds
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "VehicleDataServerTest.h"
//#include "VehicleDataServer.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
VehicleDataServerTest::VehicleDataServerTest()
{
    // Set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_VEHICLEDATASERVER");
//   m_debug.Set();
//   m_debug.Clear();

    m_debug.LogIf("    VDS Constructor", true);

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
        "/Core/Unit/ServicesLayer/Servers/VehicleDataServer/Configuration/";

    m_debug.LogIf("    VDS Constructor done", true);
}

//-----------------------------------------------------------------------------
VehicleDataServerTest::~VehicleDataServerTest()
{
    m_debug.LogIf("    VDS Destructor done", true);
}

//-----------------------------------------------------------------------------
void VehicleDataServerTest::setUp()
{
    m_debug.LogIf( "Starting setUp", true);

    // register the name "core/NamedDataBroker" so the VehicleDataServer can
    // create a NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    vds = new VehicleDataServer();

    m_debug.LogIf( "setUp done");
}

//-----------------------------------------------------------------------------
void VehicleDataServerTest::tearDown()
{
    m_debug.LogIf("Starting tearDown", true);

    vds->SetStatus(BEP_TERMINATE_STATUS);

    delete vds;

    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    m_debug.LogIf("tearDown done");
}

//-----------------------------------------------------------------------------
void VehicleDataServerTest::TestConstructor()
{
    m_debug.LogIf( "Starting TestConstructor");

    CPPUNIT_ASSERT(vds->m_vehicleBuilds == NULL);
    CPPUNIT_ASSERT(vds->m_thisBuildData == NULL);
    CPPUNIT_ASSERT_EQUAL(false,vds->m_useScalar);
    CPPUNIT_ASSERT_EQUAL(false,vds->m_useDiscrete);

    m_debug.LogIf("TestConstructor done");
}

//-----------------------------------------------------------------------------
void VehicleDataServerTest::TestLoadAdditionalConfigurationItems(void)
{
    m_debug.LogIf("Starting TestLoadAdditionalConfigurationItems");

    XmlParser parser;
    string configFile = m_configPath + "VehicleDataServerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    vds->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(
                        string("Vehicle6"),
                        vds->m_vehicleBuilds->getChild("Build06/VehicleType")->getValue());

    CPPUNIT_ASSERT_EQUAL(true,vds->m_useScalar);
    CPPUNIT_ASSERT_EQUAL(true,vds->m_useDiscrete);
    CPPUNIT_ASSERT_EQUAL(string("1"),vds->m_defaultWheelbase);
    CPPUNIT_ASSERT_EQUAL(2,vds->m_defaultVehicle);
    CPPUNIT_ASSERT_EQUAL(string("HMMA"),vds->m_buildCreationMethod);

    m_debug.LogIf("TestLoadAdditionalConfigurationItems done");
}

//-----------------------------------------------------------------------------
void VehicleDataServerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    m_debug.LogIf("Starting TestLoadAdditionalConfigurationItems_MissingTags");

    XmlParser parser;
    string configFile = m_configPath + "VehicleDataServerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    vds->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(false,vds->m_useScalar);

    // Even though the default value is false, this will be set to true to
    // make sure that at least one wheelbase method is selected.
    CPPUNIT_ASSERT_EQUAL(true,vds->m_useDiscrete);

    CPPUNIT_ASSERT_EQUAL(string("KOREA"),vds->m_buildCreationMethod);

    m_debug.LogIf("TestLoadAdditionalConfigurationItems_MissingTags done");
}

//=============================================================================
// this was an original test
void VehicleDataServerTest::ReadTest(void)
{
    std::string buildString;
    unsigned int pos=0;

    m_debug.LogIf("Starting ReadTest");

    // get config document
    XmlParser parser;
    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer.xml");
    vds->Initialize(document);

    // create a vehicle build XML node
    XmlNode vehicleBuildNode("NextVehicleBuild","1");

    // make sure that if wheelbase position 7 is selected,
    // the <Wheelbase> node is 7 and the <VehicleType> node is Vehicle7
    vds->Publish("WheelbasePosition07Selected", "1");
    vds->Publish("WheelbasePosition08Selected", "0");
    vds->Publish("WheelbasePosition09Selected", "0");
    vds->Publish("WheelbasePosition10Selected", "0");

    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>7</Wheelbase>");

    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> node is not 7",
                           (pos < buildString.npos));
    pos=buildString.find("<VehicleType>Vehicle7</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> node is not Vehicle7",
                           (pos < buildString.npos));

    // make sure that if wheelbase position 8 is selected,
    // the <Wheelbase> node is 8 and the <VehicleType> node is Vehicle8
    vds->Publish("WheelbasePosition07Selected", "0");
    vds->Publish("WheelbasePosition08Selected", "1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>8</Wheelbase>");
    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> node is not 8",
                           (pos < buildString.npos));
    pos=buildString.find("<VehicleType>Vehicle8</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> node is not Vehicle8",
                           (pos < buildString.npos));

    // make sure that if wheelbase position 9 is selected,
    // the <Wheelbase> node is 9 and the <VehicleType> node is Vehicle9
    vds->Publish("WheelbasePosition09Selected", "1");
    vds->Publish("WheelbasePosition08Selected", "0");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>9</Wheelbase>");
    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> node is not 9",
                           (pos < buildString.npos));
    pos=buildString.find("<VehicleType>Vehicle9</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> node is not Vehicle9",
                           (pos < buildString.npos));

    // make sure that if wheelbase position 10 is selected,
    // the <Wheelbase> node is 10 and the <VehicleType> node is Vehicle10
    vds->Publish("WheelbasePosition10Selected", "1");
    vds->Publish("WheelbasePosition09Selected", "0");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>10</Wheelbase>");
    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> node is not 10",
                           (pos < buildString.npos));
    pos=buildString.find("<VehicleType>Vehicle10</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> node is not Vehicle10",
                           (pos < buildString.npos));


    m_debug.LogIf("ReadTest done");
}

//=============================================================================
void VehicleDataServerTest::InitializationTest1(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting InitializationTest1" );

// this loads the Korea configuration file
    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-01.xml");
    vds->Initialize(document);

    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to false",
                           (false == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to true",
                           (true == vds->m_useDiscrete));
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"0\"",
                           ("0" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 0",
                           (0 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"KOREA\"",
                           (0 == stricmp("Korea", vds->m_buildCreationMethod.c_str())));

    m_debug.LogIf("InitializationTest1 done");
}

void VehicleDataServerTest::InitializationTest0(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting InitializationTest0");

// this loads the HMMA configuration file
    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-00.xml");
    vds->Initialize(document);

    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to false",
                           (false == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to true",
                           (true == vds->m_useDiscrete));
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"1\"",
                           ("1" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 1",
                           (1 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"HMMA\"",
                           (0 == stricmp("HMMA", vds->m_buildCreationMethod.c_str())));

    m_debug.LogIf("InitializationTest0 done");
}

void VehicleDataServerTest::InitializationTest2(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting InitializationTest2");

    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-02.xml");
    vds->Initialize(document);

    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to true",
                           (true == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to false",
                           (false == vds->m_useDiscrete));
    // note that the below defaults only apply when we are using discrete,
    //they are NA when using scalar, so are set to 0 by the code.
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"0\"",
                           ("0" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 0",
                           (0 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"HMMA\"",
                           (0 == stricmp("HMMA", vds->m_buildCreationMethod.c_str())));

    m_debug.LogIf("InitializationTest2 done");
}

void VehicleDataServerTest::InitializationTest3(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting InitializationTest3");

    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-03.xml");
    vds->Initialize(document);

    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to true",
                           (true == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to true",
                           (true == vds->m_useDiscrete));
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"3\"",
                           ("3" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 3",
                           (3 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"KOREA\"",
                           (0 == stricmp("HMMA", vds->m_buildCreationMethod.c_str())));

    m_debug.LogIf("InitializationTest3 done");
}



void VehicleDataServerTest::PTHelp1(VehicleDataServer *vds)
{
    string locStr;
    locStr = "TestInProgress";
    printf("\n%s = %s", locStr.c_str(), vds->GetData(locStr).c_str());
    locStr = "NextVIN";
    printf("\n%s = %s", locStr.c_str(), vds->GetData(locStr).c_str());
    locStr = "LatestVIN";
    printf("\n%s = %s", locStr.c_str(), vds->Read(locStr).c_str());
    printf("\nm_storedVIN = %s\n", vds->m_storedVIN.c_str());
}

//=============================================================================
// verifies the proper operation with publication of
// [NextVin] and [TestInProgress].
// note that the root Publish was already tested with the ReadTest() above
void VehicleDataServerTest::PublishTest(void)
{
    XmlParser parser;
    string tempStr;

    m_debug.LogIf("Starting PublishTest");

    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-00.xml");

    vds->Initialize(document);

// look at some initial values (not tested, for debugging use only)
    if(m_debug.On()) PTHelp1(vds);

// by sending TestInProgress= 0 twice, internal data should be cleared.
    vds->Publish("TestInProgress","0");
    if(m_debug.On()) PTHelp1(vds);
    vds->Publish("TestInProgress","0");
    if(m_debug.On()) PTHelp1(vds);

    tempStr = "";
    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not cleared on double /TIP"),
                           (tempStr == vds->GetData("LatestVIN")));

    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not cleared on double /TIP"),
                           (tempStr == vds->m_storedVIN));

    tempStr = "TestVin1";
    vds->Publish("NextVIN",tempStr);
    if(m_debug.On()) PTHelp1(vds);
    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not set to ")+tempStr,
                           (tempStr == vds->GetData("LatestVIN")));
    CPPUNIT_ASSERT_EQUAL(string("Green"),vds->GetData("VINReadStatus"));

    tempStr = "";
    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not cleared at ")+tempStr,
                           (tempStr == vds->m_storedVIN));

    tempStr = "TestVin2";
    vds->Publish("NextVIN",tempStr);
    if(m_debug.On()) PTHelp1(vds);
    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not set to ")+tempStr,
                           (tempStr == vds->GetData("LatestVIN")));
    string holdStr = tempStr; // used in subsequent test steps

    tempStr = "";
    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not cleared at ")+tempStr,
                           (tempStr == vds->m_storedVIN));

    vds->Publish("TestInProgress","1");
    if(m_debug.On()) PTHelp1(vds);

    tempStr = "TestVin3";
    vds->Publish("NextVIN",tempStr);
    if(m_debug.On()) PTHelp1(vds);

    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not set at ")+tempStr,
                           (tempStr == vds->m_storedVIN));
    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not held at ")+tempStr,
                           (holdStr == vds->GetData("LatestVIN")));

    tempStr = "TestVin4";
    vds->Publish("NextVIN",tempStr);
    if(m_debug.On()) PTHelp1(vds);

    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not set at ")+tempStr,
                           (tempStr == vds->m_storedVIN));
    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not held at ")+tempStr,
                           (holdStr == vds->GetData("LatestVIN")));

    vds->Publish("TestInProgress","0");
    if(m_debug.On()) PTHelp1(vds);

    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN updated to ")+tempStr,
                           (tempStr == vds->GetData("LatestVIN")));
    tempStr = "";
    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not cleared at ")+tempStr,
                           (tempStr == vds->m_storedVIN));
    CPPUNIT_ASSERT_EQUAL(string("Green"),vds->GetData("VINReadStatus"));

    vds->Publish("TestInProgress","0");
    if(m_debug.On()) PTHelp1(vds);

    CPPUNIT_ASSERT_MESSAGE(string("LatestVIN not cleared at end "),
                           (tempStr == vds->GetData("LatestVIN")));
    CPPUNIT_ASSERT_MESSAGE(string("m_storedVIN not cleared at end"),
                           (tempStr == vds->m_storedVIN));

    m_debug.LogIf("PublishTest done");
}

// test the various build generation capabilities - Korea style
void VehicleDataServerTest::ReadTest2(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting ReadTest2 - Korea build");

    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-01.xml");
    vds->Initialize(document);

    // make sure we have the correct config file
    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to false",
                           (false == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to true",
                           (true == vds->m_useDiscrete));
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"0\"",
                           ("0" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 0",
                           (0 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"KOREA\"",
                           (0 == stricmp("Korea", vds->m_buildCreationMethod.c_str())));

    vds->Publish("TestInProgress","0");
    vds->Publish("TestInProgress","0");

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition02Selected","0");
    vds->Publish("WheelbasePosition03Selected","0");
    vds->Publish("WheelbasePosition04Selected","0");
    vds->Publish("WheelbasePosition05Selected","0");
    vds->Publish("WheelbasePosition06Selected","0");
    vds->Publish("WheelbasePosition07Selected","0");
    vds->Publish("WheelbasePosition08Selected","0");
    vds->Publish("WheelbasePosition09Selected","0");
    vds->Publish("WheelbasePosition10Selected","0");

    vds->Publish("NextVIN","TestVin-Rt2a");
    vds->Publish("CableConnect","0");
    vds->Publish("ABSTestSelectedFromPLC","1");
    vds->Publish("TractionControlSelected","0");
    std::string buildString;
    unsigned int pos=0;
    XmlNode vehicleBuildNode("NextVehicleBuild","1");

    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>0</Wheelbase>");

    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> should be 0",
                           (pos < buildString.npos));

    vds->Publish("WheelbasePosition01Selected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>MandoMgh25</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be MandoMgh25",
                           (pos < buildString.npos));

    vds->Publish("ABSTestSelectedFromPLC","0");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<Wheelbase>1</Wheelbase>");
    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> should be 1",
                           (pos < buildString.npos));

    pos=buildString.find("<BrakeType>Base Brake</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be 'Base Brake'",
                           (pos < buildString.npos));

    pos=buildString.find("<TractionEnabled>0</TractionEnabled>");
    CPPUNIT_ASSERT_MESSAGE("<TractionEnabled> should be 0",
                           (pos < buildString.npos));

    pos=buildString.find("<VIN>TestVin-Rt2a</VIN>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be 'Base Brake'",
                           (pos < buildString.npos));

    pos=buildString.find("<VehicleType>2WD</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> should be 2WD",
                           (pos < buildString.npos));

    vds->Publish("CableConnect","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>MandoMgh25</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be MandoMgh25 (cable)",
                           (pos < buildString.npos));

    vds->Publish("CableConnect","0");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>Base Brake</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be Base Brake (cable)",
                           (pos < buildString.npos));

    vds->Publish("ABSTestSelectedFromPLC","1");
    vds->Publish("TractionControlSelected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<TractionEnabled>1</TractionEnabled>");
    CPPUNIT_ASSERT_MESSAGE("<TractionEnabled> should be 1",
                           (pos < buildString.npos));

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition07Selected","1");
    buildString = vds->Read(&vehicleBuildNode);

    pos=buildString.find("<Wheelbase>7</Wheelbase>");
    CPPUNIT_ASSERT_MESSAGE("<Wheelbase> should be 7",
                           (pos < buildString.npos));

    m_debug.LogIf("ReadTest2 done");
}

// test the various build generation capabilities - HMMA style
void VehicleDataServerTest::ReadTest3(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting ReadTest3 - HMMA build");

    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-00.xml");
    vds->Initialize(document);

    // make sure we have the correct config file
    CPPUNIT_ASSERT_MESSAGE("m_useScalar is not set to false",
                           (false == vds->m_useScalar));
    CPPUNIT_ASSERT_MESSAGE("m_useDiscrete is not set to true",
                           (true == vds->m_useDiscrete));
    CPPUNIT_ASSERT_MESSAGE("m_defaultWheelbase is not set to \"1\"",
                           ("1" == vds->m_defaultWheelbase));
    CPPUNIT_ASSERT_MESSAGE("m_defaultVehicle is not set to 1",
                           (1 == vds->m_defaultVehicle));
    CPPUNIT_ASSERT_MESSAGE("m_buildCreationMethod is not set to \"HMMA\"",
                           (0 == stricmp("HMMA", vds->m_buildCreationMethod.c_str())));

    vds->Publish("TestInProgress","0");
    vds->Publish("TestInProgress","0");

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition02Selected","0");
    vds->Publish("WheelbasePosition03Selected","0");
    vds->Publish("WheelbasePosition04Selected","0");
    vds->Publish("WheelbasePosition05Selected","0");
    vds->Publish("WheelbasePosition06Selected","0");
    vds->Publish("WheelbasePosition07Selected","0");
    vds->Publish("WheelbasePosition08Selected","0");
    vds->Publish("WheelbasePosition09Selected","0");
    vds->Publish("WheelbasePosition10Selected","0");

    vds->Publish("NextVIN","TestVin-Rt2b");
    vds->Publish("CableConnect","0");
    vds->Publish("ABSTestSelectedFromPLC","1");
    vds->Publish("TractionControlSelected","0");
    std::string buildString;
    unsigned int pos=0;
    XmlNode vehicleBuildNode("NextVehicleBuild","1");

    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>Bosch8</BrakeType>");

    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be Bosch8",
                           (pos < buildString.npos));

    vds->Publish("ABSTestSelectedFromPLC","0");
    buildString = vds->Read(&vehicleBuildNode);

    pos=buildString.find("<AllWheelDriveEquipped>No</AllWheelDriveEquipped>");
    CPPUNIT_ASSERT_MESSAGE("<AllWheelDriveEquipped> should be No",
                           (pos < buildString.npos));

    // sample some of the direct nodes
    pos=buildString.find("<BodyStyle>NF</BodyStyle>");
    CPPUNIT_ASSERT_MESSAGE("<BodyStyle> should be NF",
                           (pos < buildString.npos));
    pos=buildString.find("<Drive>LHD</Drive>");
    CPPUNIT_ASSERT_MESSAGE("<Drive> should be LHD",
                           (pos < buildString.npos));
    pos=buildString.find("<Engine>3.3 5AT</Engine>");
    CPPUNIT_ASSERT_MESSAGE("<Engine> should be 3.3 5AT",
                           (pos < buildString.npos));
    pos=buildString.find("<FrontWheelDriveSelectedToPLC>Yes</FrontWheelDriveSelectedToPLC>");
    CPPUNIT_ASSERT_MESSAGE("<FrontWheelDriveSelectedToPLC> should be Yes",
                           (pos < buildString.npos));
    pos=buildString.find("<VehicleType>NF</VehicleType>");
    CPPUNIT_ASSERT_MESSAGE("<VehicleType> should be NF",
                           (pos < buildString.npos));
    pos=buildString.find("<WheelbasePositionInchesX10>1058</WheelbasePositionInchesX10>");
    CPPUNIT_ASSERT_MESSAGE("<WheelbasePositionInchesX10> should be 1058",
                           (pos < buildString.npos));

// sample the dynamic nodes
    pos=buildString.find("<BrakeType>Base Brakes</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be Base Brakes",
                           (pos < buildString.npos));
    pos=buildString.find("<TractionEnabled>0</TractionEnabled>");
    CPPUNIT_ASSERT_MESSAGE("<TractionEnabled> should be 0",
                           (pos < buildString.npos));
    pos=buildString.find("<VIN>TestVin-Rt2b</VIN>");
    CPPUNIT_ASSERT_MESSAGE("<VIN> should be TestVin-Rt2b",
                           (pos < buildString.npos));

// cable connect test

    vds->Publish("CableConnect","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>Bosch8</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be Bosch8 (cable)",
                           (pos < buildString.npos));

    vds->Publish("CableConnect","0");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<BrakeType>Base Brakes</BrakeType>");
    CPPUNIT_ASSERT_MESSAGE("<BrakeType> should be Base Brake (cable)",
                           (pos < buildString.npos));

    vds->Publish("ABSTestSelectedFromPLC","1");
    vds->Publish("TractionControlSelected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<TractionEnabled>1</TractionEnabled>");
    CPPUNIT_ASSERT_MESSAGE("<TractionEnabled> should be 1",
                           (pos < buildString.npos));

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition07Selected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<WheelbasePositionInchesX10>1060</WheelbasePositionInchesX10>");
    CPPUNIT_ASSERT_MESSAGE("<WheelbasePositionInchesX10> should be 1060",
                           (pos < buildString.npos));

    m_debug.LogIf("ReadTest3 done");
}


void VehicleDataServerTest::WheelbaseTest1(void)
{
    XmlParser parser;

    m_debug.LogIf("Starting Wheelbase Test 1 - Scalar Wheelbase");

    // scalar only
    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-02.xml");
    vds->Initialize(document);

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition02Selected","0");
    vds->Publish("WheelbasePosition03Selected","0");
    vds->Publish("WheelbasePosition04Selected","0");
    vds->Publish("WheelbasePosition05Selected","0");
    vds->Publish("WheelbasePosition06Selected","0");
    vds->Publish("WheelbasePosition07Selected","0");
    vds->Publish("WheelbasePosition08Selected","0");
    vds->Publish("WheelbasePosition09Selected","0");
    vds->Publish("WheelbasePosition10Selected","0");
    vds->Publish("VehicleTypeFromPLC","0");
// note that default for scalar is always 0.

    std::string buildString;
    unsigned int pos=0;
    XmlNode vehicleBuildNode("NextVehicleBuild","1");

    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined0</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined0",
                           (pos < buildString.npos));

    vds->Publish("VehicleTypeFromPLC","3");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined3</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined3",
                           (pos < buildString.npos));

// this next one should be ignored.
    vds->Publish("WheelbasePosition01Selected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined3</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined3",
                           (pos < buildString.npos));

    m_debug.LogIf("WheelbaseTest1 done");
}

void VehicleDataServerTest::WheelbaseTest2(void)
{
    XmlParser parser;

    m_debug.LogIf("Mid Wheelbase Test 2 - Both scalar and discrete");

// both selected
    const XmlNode* document =
        parser.ReturnXMLDocument(m_configPath + "VehicleDataServer-03.xml");
    vds->Initialize(document);

    vds->Publish("WheelbasePosition01Selected","0");
    vds->Publish("WheelbasePosition02Selected","0");
    vds->Publish("WheelbasePosition03Selected","0");
    vds->Publish("WheelbasePosition04Selected","0");
    vds->Publish("WheelbasePosition05Selected","0");
    vds->Publish("WheelbasePosition06Selected","0");
    vds->Publish("WheelbasePosition07Selected","0");
    vds->Publish("WheelbasePosition08Selected","0");
    vds->Publish("WheelbasePosition09Selected","0");
    vds->Publish("WheelbasePosition10Selected","0");
    vds->Publish("VehicleTypeFromPLC","0");

    std::string buildString;
    unsigned int pos=0;
    XmlNode vehicleBuildNode("NextVehicleBuild","1");

    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined03</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined03",
                           (pos < buildString.npos));

    vds->Publish("VehicleTypeFromPLC","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined01</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined01",
                           (pos < buildString.npos));

// this next one should be processed.
    vds->Publish("WheelbasePosition06Selected","1");
    buildString = vds->Read(&vehicleBuildNode);
    pos=buildString.find("<HeadLamp>Undefined06</HeadLamp>");
    CPPUNIT_ASSERT_MESSAGE("<HeadLamp> should be Undefined06",
                           (pos < buildString.npos));

    m_debug.LogIf("WheelbaseTest2 done");
}


