#include "IpIcmMgrTest.h"
#include "IpIcmMgr.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
IpIcmMgrTest::IpIcmMgrTest()
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
        "/Core/Unit/ServicesLayer/Managers/ICM/IpIcm/Configuration/";
}

//-----------------------------------------------------------------------------
IpIcmMgrTest::~IpIcmMgrTest()
{
}

//-----------------------------------------------------------------------------
void IpIcmMgrTest::setUp()
{
    // get the current FTP_ROOT environment variable
    m_origFtpRoot = getenv("FTP_ROOT");

    // set the FTP_ROOT environment variable to use for testing
    m_newFtpRoot = getenv("TEST_DIR_PATH");
    m_newFtpRoot += "/Core/Unit/ServicesLayer/Managers/ICM/IpIcm/";
    setenv("FTP_ROOT",m_newFtpRoot.c_str(),1);

    iim = new IpIcmMgr();
}

//-----------------------------------------------------------------------------
void IpIcmMgrTest::tearDown()
{
    // restore the FTP_ROOT environment variable
    setenv("FTP_ROOT",m_origFtpRoot.c_str(),1);

    delete iim;
}

//-----------------------------------------------------------------------------
void IpIcmMgrTest::TestConstructor()
{
    CPPUNIT_ASSERT_EQUAL(false,iim->m_encoderPhasing);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_disableSystem);

    CPPUNIT_ASSERT_EQUAL(string(LF_CHANNEL_NAME),iim->m_channelNames[0]);
    CPPUNIT_ASSERT_EQUAL(string(LR_CHANNEL_NAME),iim->m_channelNames[1]);
    CPPUNIT_ASSERT_EQUAL(string(RF_CHANNEL_NAME),iim->m_channelNames[2]);
    CPPUNIT_ASSERT_EQUAL(string(RR_CHANNEL_NAME),iim->m_channelNames[3]);
    CPPUNIT_ASSERT_EQUAL(string(LT_CHANNEL_NAME),iim->m_channelNames[4]);
    CPPUNIT_ASSERT_EQUAL(string(RT_CHANNEL_NAME),iim->m_channelNames[5]);

    CPPUNIT_ASSERT_EQUAL(string("Kspeed"),iim->m_subChannels[0].GetName());
    CPPUNIT_ASSERT_EQUAL(false,iim->m_subChannels[0].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Kaccel"),iim->m_subChannels[1].GetName());
    CPPUNIT_ASSERT_EQUAL(false,iim->m_subChannels[1].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Kforce"),iim->m_subChannels[2].GetName());
    CPPUNIT_ASSERT_EQUAL(false,iim->m_subChannels[2].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Distance"),iim->m_subChannels[3].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[3].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Frequency"),iim->m_subChannels[4].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[4].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Speed"),iim->m_subChannels[5].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[5].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Acceleration"),
                         iim->m_subChannels[6].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[6].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Force"),iim->m_subChannels[7].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[7].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("Loss"),iim->m_subChannels[8].GetName());
    CPPUNIT_ASSERT_EQUAL(false,iim->m_subChannels[8].IsReadOnly());
    CPPUNIT_ASSERT_EQUAL(string("PulseCount"),iim->m_subChannels[9].GetName());
    CPPUNIT_ASSERT_EQUAL(true,iim->m_subChannels[9].IsReadOnly());

    CPPUNIT_ASSERT_EQUAL(string("ArrayDistance"),iim->m_dataArrayNames[0]);
    CPPUNIT_ASSERT_EQUAL(string("ArraySpeed"),iim->m_dataArrayNames[1]);
    CPPUNIT_ASSERT_EQUAL(string("ArrayAccel"),iim->m_dataArrayNames[2]);
    CPPUNIT_ASSERT_EQUAL(string("ArrayForce"),iim->m_dataArrayNames[3]);
    CPPUNIT_ASSERT_EQUAL(string("ArrayFrequency"),iim->m_dataArrayNames[4]);

    CPPUNIT_ASSERT_EQUAL(string("Block/Speed"),iim->m_blockDataNames[0]);
    CPPUNIT_ASSERT_EQUAL(string("Block/Acceleration"),iim->m_blockDataNames[1]);
    CPPUNIT_ASSERT_EQUAL(string("Block/Force"),iim->m_blockDataNames[2]);
    CPPUNIT_ASSERT_EQUAL(string("Block/Loss"),iim->m_blockDataNames[3]);
    CPPUNIT_ASSERT_EQUAL(string("Block/PulseCount"),iim->m_blockDataNames[4]);
    CPPUNIT_ASSERT_EQUAL(string("Block/Drag"),iim->m_blockDataNames[5]);

    CPPUNIT_ASSERT_EQUAL(string("LF"),iim->m_tagList[LF_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RF"),iim->m_tagList[RF_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LR"),iim->m_tagList[LR_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RR"),iim->m_tagList[RR_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LT"),iim->m_tagList[LT_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RT"),iim->m_tagList[RT_CHANNEL_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("Zerospeed"),iim->m_tagList[ZEROSPEED_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("Overspeed"),iim->m_tagList[OVERSPEED_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("TestInProgress"),
                         iim->m_tagList["StartTestTag"]);
    CPPUNIT_ASSERT_EQUAL(string("TestComplete"),iim->m_tagList["EndTestTag"]);
    CPPUNIT_ASSERT_EQUAL(string("TagIndex"),iim->m_tagList[TAG_IDX_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RollerCount"),
                         iim->m_tagList[ROLLER_COUNT_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("ZerospeedToPLC"),
                         iim->m_tagList[ZEROSPEED_LATCH_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LfRollerDirection"),
                         iim->m_tagList[LF_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RfRollerDirection"),
                         iim->m_tagList[RF_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LrRollerDirection"),
                         iim->m_tagList[LR_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RrRollerDirection"),
                         iim->m_tagList[RR_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LtRollerDirection"),
                         iim->m_tagList[LT_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("RtRollerDirection"),
                         iim->m_tagList[RT_ROLL_DIR_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("SampleDelta"),
                         iim->m_tagList[SAMPLE_DELTA_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("AccumulatorTarget"),
                         iim->m_tagList[ACCUM_TARGET_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("PrescaleMode"),
                         iim->m_tagList[PRESCALE_MODE_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("Machine3600Id"),
                         iim->m_tagList[MACH_3600_ID_NAME]);
    CPPUNIT_ASSERT_EQUAL(string("LFWheelSpeed"),iim->m_tagList["LFPlcSpeed"]);
    CPPUNIT_ASSERT_EQUAL(string("RFWheelSpeed"),iim->m_tagList["RFPlcSpeed"]);
    CPPUNIT_ASSERT_EQUAL(string("LRWheelSpeed"),iim->m_tagList["LRPlcSpeed"]);
    CPPUNIT_ASSERT_EQUAL(string("RRWheelSpeed"),iim->m_tagList["RRPlcSpeed"]);
    CPPUNIT_ASSERT_EQUAL(string("HornSignal"),
                         iim->m_tagList["AccelLimitFaultTag"]);
    CPPUNIT_ASSERT_EQUAL(string("LossComponsation"),
                         iim->m_tagList["LossCompensationInProgress"]);

    CPPUNIT_ASSERT(iim->m_distArray == NULL);
    CPPUNIT_ASSERT(iim->m_speedArray == NULL);
    CPPUNIT_ASSERT(iim->m_freqArray == NULL);
    CPPUNIT_ASSERT(iim->m_accelArray == NULL);
    CPPUNIT_ASSERT(iim->m_forceArray == NULL);
    CPPUNIT_ASSERT_EQUAL((uint32_t)120000,iim->m_arraySize);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0,iim->m_lastDataLogIndex);
    CPPUNIT_ASSERT_EQUAL(0,iim->m_baseInstInode);
    CPPUNIT_ASSERT_EQUAL(0,iim->m_baseArrayInode);
    CPPUNIT_ASSERT_EQUAL(0,iim->m_baseBlockInode);
    CPPUNIT_ASSERT(iim->m_dataBroker == NULL);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_zeroSpeed);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_zspdPublished);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_arrayLogActive);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,iim->m_zeroSpeedVal,0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0,iim->m_zeroSpeedCount);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_filteredZerospeed);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0,iim->m_defaultOverSpeedVal,0.01);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_overSpeedActive);
    CPPUNIT_ASSERT_EQUAL(25,iim->m_priority);
    CPPUNIT_ASSERT_EQUAL((uint16_t)4,iim->m_sampleDelta);
    CPPUNIT_ASSERT_EQUAL((uint32_t)20,iim->m_samplePeriod);
    CPPUNIT_ASSERT_EQUAL(-1,iim->m_irqId);
    CPPUNIT_ASSERT_EQUAL(true,iim->m_waitToRegister);
    CPPUNIT_ASSERT_EQUAL((uint8_t)0,iim->m_plcSpeedCount);
    CPPUNIT_ASSERT_EQUAL(false,iim->m_lossComp);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0,iim->m_accelLimit,0.01);
    CPPUNIT_ASSERT_EQUAL(0,iim->m_accelAlarmTime);
    CPPUNIT_ASSERT_EQUAL(12,iim->m_accelAlarmFilterOnFilterCnt);
    CPPUNIT_ASSERT_EQUAL(50,iim->m_accelAlarmFilterOffFilterCnt);
    CPPUNIT_ASSERT_EQUAL(ACCEL_ALARM_OFF,iim->m_accelAlarmState);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,iim->m_accelAlarmOnTime);
    CPPUNIT_ASSERT_EQUAL((uint64_t)0,iim->m_accelAlarmOffTime);
    CPPUNIT_ASSERT_EQUAL(string("-1"),iim->m_alarmValue);
}


//-----------------------------------------------------------------------------
void IpIcmMgrTest::TestReloadConfiguration(void)
{
    string configFile = m_configPath + "IpIcmConfig.xml";

    // Set the configuration file path and name
    iim->SetConfigFile(configFile);

    // Tell the system that we have two ICMs installed
    IpIcm   *ipIcm;
    ipIcm = new IpIcm;
    iim->m_icmIps.push_back(ipIcm);
    iim->m_icmIps.push_back(ipIcm);

    // Put the loss items for all rolls in the data table
    dataMapItem_t lfItem(0,0xFFFF,true,PORT_DIR_OUT,0);
    iim->m_dataTable.insert("LFLoss",lfItem);
    dataMapItem_t rfItem(1,0xFFFF,true,PORT_DIR_OUT,0);
    iim->m_dataTable.insert("RFLoss",rfItem);
    dataMapItem_t lrItem(2,0xFFFF,true,PORT_DIR_OUT,0);
    iim->m_dataTable.insert("LRLoss",lrItem);
    dataMapItem_t rrItem(3,0xFFFF,true,PORT_DIR_OUT,0);
    iim->m_dataTable.insert("RRLoss",rrItem);
    dataMapItem_t ltItem(0,0xFFFF,true,PORT_DIR_OUT,1);
    iim->m_dataTable.insert("LTLoss",ltItem);
    dataMapItem_t rtItem(1,0xFFFF,true,PORT_DIR_OUT,1);
    iim->m_dataTable.insert("RTLoss",rtItem);

    // reserve and resize the data items based on two ICMs
    iim->m_isrFifo.reserve(iim->m_icmIps.size());        
    iim->m_isrFifo.resize(iim->m_icmIps.size());
    iim->m_convFactors.reserve(iim->m_icmIps.size());    
    iim->m_convFactors.resize(iim->m_icmIps.size());

    iim->ReloadConfiguration();

    string compareFileName = getenv("TEST_DIR_PATH");
    compareFileName += "/Core/Unit/ServicesLayer/Managers/ICM/IpIcm/";
    compareFileName += "/Configuration/MachineLosses1.xml";

    CPPUNIT_ASSERT_EQUAL(compareFileName,iim->m_lossFileName);

    CPPUNIT_ASSERT_EQUAL((uint16_t)10,iim->m_sampleDelta);
    CPPUNIT_ASSERT_EQUAL((uint32_t)21,iim->m_samplePeriod);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(11,iim->m_accelLimit,0.01);
    CPPUNIT_ASSERT_EQUAL(2,iim->m_accelAlarmFilterOnFilterCnt);
    CPPUNIT_ASSERT_EQUAL(3,iim->m_accelAlarmFilterOffFilterCnt);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100,iim->m_defaultOverSpeedVal,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2,iim->m_zeroSpeedVal,0.01);

    // LF values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(415.0,
                                 iim->m_rollAttribs[0].rollerFactor[0],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1251,iim->m_rollAttribs[0].encoderPulse[0]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.626,
                                 iim->m_rollAttribs[0].rollDiameter[0],0.0001);

    // RF values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(426.0,
                                 iim->m_rollAttribs[0].rollerFactor[1],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1252,iim->m_rollAttribs[0].encoderPulse[1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.627,
                                 iim->m_rollAttribs[0].rollDiameter[1],0.0001);

    // LR values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(427.0,
                                 iim->m_rollAttribs[0].rollerFactor[2],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1253,iim->m_rollAttribs[0].encoderPulse[2]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.628,
                                 iim->m_rollAttribs[0].rollDiameter[2],0.0001);

    // RR values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(422.0,
                                 iim->m_rollAttribs[0].rollerFactor[3],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1254,iim->m_rollAttribs[0].encoderPulse[3]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.629,
                                 iim->m_rollAttribs[0].rollDiameter[3],0.0001);

    // LT values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(405.0,
                                 iim->m_rollAttribs[1].rollerFactor[0],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1255,iim->m_rollAttribs[1].encoderPulse[0]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.630,
                                 iim->m_rollAttribs[1].rollDiameter[0],0.0001);

    // RT values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(406.0,
                                 iim->m_rollAttribs[1].rollerFactor[1],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1256,iim->m_rollAttribs[1].encoderPulse[1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.631,
                                 iim->m_rollAttribs[1].rollDiameter[1],0.0001);

    // Machine loss values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        2.812159,iim->m_convFactors[0].offset[0].force,0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        11.929525,iim->m_convFactors[0].offset[1].force,0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        3.707658,iim->m_convFactors[0].offset[2].force,0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        15.502554,iim->m_convFactors[0].offset[3].force,0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        5.123456,iim->m_convFactors[1].offset[0].force,0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        6.123456,iim->m_convFactors[1].offset[1].force,0.000001);
}


//-----------------------------------------------------------------------------
void IpIcmMgrTest::TestReloadConfiguration_MissingTags(void)
{
    string configFile = m_configPath + "IpIcmConfigMissingTags.xml";

    // Set the configuration file path and name
    iim->SetConfigFile(configFile);

    iim->ReloadConfiguration();

    string compareFileName = getenv("TEST_DIR_PATH");
    compareFileName += "/Core/Unit/ServicesLayer/Managers/ICM/IpIcm/";
    compareFileName += "/Configuration/Servers/MachineLosses.xml";

    CPPUNIT_ASSERT_EQUAL(compareFileName,iim->m_lossFileName);

    CPPUNIT_ASSERT_EQUAL((uint16_t)4,iim->m_sampleDelta);
    CPPUNIT_ASSERT_EQUAL((uint32_t)20,iim->m_samplePeriod);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0,iim->m_accelLimit,0.01);
    CPPUNIT_ASSERT_EQUAL(12,iim->m_accelAlarmFilterOnFilterCnt);
    CPPUNIT_ASSERT_EQUAL(50,iim->m_accelAlarmFilterOffFilterCnt);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0,iim->m_defaultOverSpeedVal,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1,iim->m_zeroSpeedVal,0.01);

    // LF values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[0].rollerFactor[0],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[0].encoderPulse[0]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[0].rollDiameter[0],0.0001);

    // RF values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[0].rollerFactor[1],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[0].encoderPulse[1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[0].rollDiameter[1],0.0001);

    // LR values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[0].rollerFactor[2],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[0].encoderPulse[2]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[0].rollDiameter[2],0.0001);

    // RR values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[0].rollerFactor[3],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[0].encoderPulse[3]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[0].rollDiameter[3],0.0001);

    // LT values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[1].rollerFactor[0],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[1].encoderPulse[0]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[1].rollDiameter[0],0.0001);

    // RT values
    CPPUNIT_ASSERT_DOUBLES_EQUAL(436.0,
                                 iim->m_rollAttribs[1].rollerFactor[1],0.01);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1250,iim->m_rollAttribs[1].encoderPulse[1]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(35.625,
                                 iim->m_rollAttribs[1].rollDiameter[1],0.0001);
}

