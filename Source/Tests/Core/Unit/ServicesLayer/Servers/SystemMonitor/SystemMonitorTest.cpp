//******************************************************************************
// Description:
//    System Monitor test class.
//
//==============================================================================
//
//     Copyright (c) 2006 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemMonitor/SystemMonitorTest.cpp $
// 
// 2     2/22/07 1:25p Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added reading in of the configuration file.
// - Added assertions for m_sysMonRegistered and m_useFaultServer in
// TestConstructor().
// 
// - Added CheckMachineStatusTest_PowerSenseFaultCleared(),
// CheckMachineStatusTest_RackFaultCleared(), and
// CheckMachineStatusTest_PlcSystemIsReadyFaultCleared().
// 
// - Added a MockFaultServer for the CheckMachineStatus() tests.
// - Added Initialize_UseFaultServer() and
// Initialize_UseFaultServerMissing().
// - Added SetSystemMonitorRegisteredTest() and
// IsSystemMonitorRegisteredTest().
// 
// 1     1/18/07 5:10p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "SystemMonitorTest.h"
#include "MockSystemMonitor.h"
#include "MockNamedDataBroker.h"
#include "MockPromptServer.h"
#include "MockFaultServer.h"

using namespace std;

//=============================================================================
SystemMonitorTest::SystemMonitorTest()
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
        "/Core/Unit/ServicesLayer/Servers/SystemMonitor/Configuration/";
}

//=============================================================================
SystemMonitorTest::~SystemMonitorTest()
{
}

//=============================================================================
void SystemMonitorTest::setUp()
{
    // register the name "core/NamedDataBroker" so systemMonitor can
    // create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    sm = new MockSystemMonitor();
}

//=============================================================================
void SystemMonitorTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    sm->SetStatus(BEP_TERMINATE_STATUS);
    delete sm;
}

//=============================================================================
void SystemMonitorTest::TestConstructor()
{
    CPPUNIT_ASSERT(sm->m_dataBroker == NULL);
    CPPUNIT_ASSERT(sm->m_promptServer == NULL);
    CPPUNIT_ASSERT(sm->m_faultServer == NULL);
    CPPUNIT_ASSERT(sm->m_faultInformation == NULL);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_useRegisterMssg);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,sm->m_vehiclePresHystTime,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,sm->m_lcSwitchHystTime,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,sm->m_cableConnectHystTime,0.01);
    CPPUNIT_ASSERT_EQUAL(string("3600"),sm->m_machineType);
    CPPUNIT_ASSERT(sm->m_updateControl == NULL);
    CPPUNIT_ASSERT_EQUAL(0,sm->m_cableDisconnectCounter);
    CPPUNIT_ASSERT_EQUAL(false,sm->m_abortOnAbortPb);
    CPPUNIT_ASSERT_EQUAL(false,sm->m_sysMonRegistered);
    CPPUNIT_ASSERT_EQUAL(false,sm->m_useFaultServer);
    CPPUNIT_ASSERT_EQUAL(string(SYS_MON_NAME),sm->GetProcessName());
    CPPUNIT_ASSERT_EQUAL(false,sm->IsDebugOn());
}

//=============================================================================
void SystemMonitorTest::ControlDataConstructorTest(void)
{
    ControlData* cd = sm->ControlAllocate();

    CPPUNIT_ASSERT_EQUAL(string("                 "),cd->vehVinReadStatus);
    CPPUNIT_ASSERT_EQUAL(false,cd->startTestPb);
    CPPUNIT_ASSERT_EQUAL(false,cd->startTestCommand);
    CPPUNIT_ASSERT_EQUAL(false,cd->vehiclePresent);
    CPPUNIT_ASSERT_EQUAL(false,cd->rollsDown);
    CPPUNIT_ASSERT_EQUAL(false,cd->rollsUp);
    CPPUNIT_ASSERT_EQUAL(false,cd->cableConnect6Pin);
    CPPUNIT_ASSERT_EQUAL(false,cd->cableConnect8Pin);
    CPPUNIT_ASSERT_EQUAL(false,cd->cableConnect16Pin);
    CPPUNIT_ASSERT_EQUAL(false,cd->cableConnect);
    CPPUNIT_ASSERT_EQUAL(false,cd->calSwitch);
    CPPUNIT_ASSERT_EQUAL(false,cd->calSwitchToPLC);
    CPPUNIT_ASSERT_EQUAL(false,cd->calCycle);
    CPPUNIT_ASSERT_EQUAL(false,cd->calInProgress);
    CPPUNIT_ASSERT_EQUAL(false,cd->calConditionsCorrect);
    CPPUNIT_ASSERT_EQUAL(false,cd->calKeyCycled);
    CPPUNIT_ASSERT_EQUAL(false,cd->simSwitch);
    CPPUNIT_ASSERT_EQUAL(false,cd->simCycle);
    CPPUNIT_ASSERT_EQUAL(false,cd->simInProgress);
    CPPUNIT_ASSERT_EQUAL(false,cd->testStarted);
    CPPUNIT_ASSERT_EQUAL(false,cd->testInProgress);
    CPPUNIT_ASSERT_EQUAL(false,cd->testRequested);
    CPPUNIT_ASSERT_EQUAL(string("Normal"),cd->inputServerState);
    CPPUNIT_ASSERT_EQUAL(string(""),cd->keyPress);
    CPPUNIT_ASSERT_EQUAL(false,cd->powerSense);
    CPPUNIT_ASSERT_EQUAL(false,cd->rackFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->machineFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->reprintRequest);
    CPPUNIT_ASSERT_EQUAL(false,cd->wheelbaseInPosition);
    CPPUNIT_ASSERT_EQUAL(false,cd->vmeSystemReady);
    CPPUNIT_ASSERT_EQUAL(false,cd->zerospeed);
    CPPUNIT_ASSERT_EQUAL(false,cd->abortPb);
    CPPUNIT_ASSERT_EQUAL(false,cd->plcSystemIsReady);
    CPPUNIT_ASSERT_EQUAL(false,cd->lfMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->rfMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->lrMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->rrMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->ltMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->rtMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(false,cd->motorCommFault);

    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::AssignTest(void)
{
    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    cd->vehVinReadStatus = string(17, '1');
    cd->startTestPb = true;
    cd->startTestCommand = true;
    cd->vehiclePresent = true;
    cd->rollsDown = true;
    cd->rollsUp = true;
    cd->cableConnect6Pin = true;
    cd->cableConnect8Pin = true;
    cd->cableConnect16Pin = true;
    cd->cableConnect = true;
    cd->calSwitch = true;
    cd->calSwitchToPLC = true;
    cd->calCycle = true;
    cd->calInProgress = true;
    cd->calConditionsCorrect = true;
    cd->calKeyCycled = true;
    cd->simSwitch = true;
    cd->simCycle = true;
    cd->simInProgress = true;
    cd->testStarted = true;
    cd->testInProgress = true;
    cd->testRequested = true;
    cd->inputServerState = "Normal1";
    cd->keyPress = "1";
    cd->powerSense = true;
    cd->rackFault = true;
    cd->machineFault = true;
    cd->reprintRequest = true;
    cd->wheelbaseInPosition=true;
    cd->vmeSystemReady=true;
    cd->zerospeed=true;
    cd->abortPb = true;
    cd->plcSystemIsReady = true;
    cd->lfMotorOperationalFault = true; cd->rfMotorOperationalFault = true;
    cd->lrMotorOperationalFault = true; cd->rrMotorOperationalFault = true;
    cd->ltMotorOperationalFault = true; cd->rtMotorOperationalFault = true;
    cd->motorCommFault = true;

    sm->m_oldCtrl->Assign(*cd);

    CPPUNIT_ASSERT_EQUAL(string("11111111111111111"),
                         sm->m_oldCtrl->vehVinReadStatus);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->startTestPb);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->startTestCommand);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->vehiclePresent);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rollsDown);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rollsUp);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->cableConnect6Pin);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->cableConnect8Pin);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->cableConnect16Pin);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->cableConnect);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calSwitch);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calSwitchToPLC);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calCycle);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calInProgress);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calConditionsCorrect);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->calKeyCycled);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->simSwitch);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->simCycle);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->simInProgress);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->testStarted);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->testInProgress);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->testRequested);
    CPPUNIT_ASSERT_EQUAL(string("Normal1"),sm->m_oldCtrl->inputServerState);
    CPPUNIT_ASSERT_EQUAL(string("1"),sm->m_oldCtrl->keyPress);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->powerSense);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rackFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->machineFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->reprintRequest);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->wheelbaseInPosition);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->vmeSystemReady);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->zerospeed);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->abortPb);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->plcSystemIsReady);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->lfMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rfMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->lrMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rrMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->ltMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->rtMotorOperationalFault);
    CPPUNIT_ASSERT_EQUAL(true,sm->m_oldCtrl->motorCommFault);

    sm->ControlFree(cd);
}


//=============================================================================
void SystemMonitorTest::GetCurrentControlDataTest(void)
{
    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    XmlNode *vehVinReadStatusNode = new XmlNode(VIN_READ_STATUS_TAG,"1234");
    XmlNode *startTestPbNode = new XmlNode(START_TEST_REQ_DATA_TAG,"1");
    XmlNode *testInProgressNode = new XmlNode(TEST_IN_PROG_DATA_TAG,"1");
    XmlNode *vehiclePresentNode = new XmlNode(INST_VEH_PRESENT_DATA_TAG,"1");
    XmlNode *rollsDownNode = new XmlNode(ROLLS_DOWN_DATA_TAG,"1");
    XmlNode *rollsUpNode = new XmlNode(ROLLS_UP_DATA_TAG,"1");
    XmlNode *calCycleNode = 
        new XmlNode(LOSSCOMPENSATION_CYCLE_FROM_PLC_TAG,"1");
    XmlNode *calSwitchNode = new XmlNode(LC_SWITCH_STATUS_FROM_PLC_TAG,"1");
    XmlNode *calSwitchToPLCNode = new XmlNode(LC_SWITCHTOPLC_STATUS_TAG,"1");
    XmlNode *calInProgressNode = 
        new XmlNode(START_LOSSCOMPENSATION_DATA_TAG,"1");
    XmlNode *cableConnect6PinNode = new XmlNode(CBL_CONNECT_6_PIN,"1");
    XmlNode *cableConnect8PinNode = new XmlNode(CBL_CONNECT_8_PIN,"1");
    XmlNode *cableConnect16PinNode = new XmlNode(CBL_CONNECT_16_PIN,"1");
    XmlNode *inputServerStateNode = new XmlNode(INPUT_SERVER_STATE,"Normal1");
    XmlNode *powerSenseNode = new XmlNode(PLC_POWER_SENSE,"1");
    XmlNode *rackFaultNode = new XmlNode(PLC_RACK_FAULT,"1");
    XmlNode *wheelbaseInPositionNode = 
        new XmlNode(WHEELBASE_IN_POSITION_TAG,"1");
    XmlNode *vmeSystemReadyNode = new XmlNode(VME_SYSTEM_READY_TAG,"1");
    XmlNode *abortPbNode = new XmlNode("AbortTestFromPLC","1");
    XmlNode *plcSystemIsReadyNode = new XmlNode(PLC_SYSTEM_IS_READY_TAG,"1");
    XmlNode *keyPressNode = new XmlNode(PENDANT_KEY_PRESS,"12");

    // Set the subscribe data
    sm->SetData(vehVinReadStatusNode);    sm->SetData(startTestPbNode);
    sm->SetData(testInProgressNode);      sm->SetData(vehiclePresentNode);
    sm->SetData(rollsDownNode);           sm->SetData(rollsUpNode);
    sm->SetData(calCycleNode);            sm->SetData(calSwitchNode);
    sm->SetData(calSwitchToPLCNode);      sm->SetData(calInProgressNode);
    sm->SetData(cableConnect6PinNode);    sm->SetData(cableConnect8PinNode);
    sm->SetData(cableConnect16PinNode);   sm->SetData(inputServerStateNode);
    sm->SetData(powerSenseNode);          sm->SetData(rackFaultNode);
    sm->SetData(wheelbaseInPositionNode); sm->SetData(vmeSystemReadyNode);
    sm->SetData(abortPbNode);             sm->SetData(plcSystemIsReadyNode);
    sm->SetData(keyPressNode);

    sm->m_validVINNeededForStart = false;

    sm->GetCurrentControlData(cd);

    CPPUNIT_ASSERT_EQUAL(string(VALID_VEHICLE_VIN),cd->vehVinReadStatus);
    CPPUNIT_ASSERT_EQUAL(true,cd->startTestPb);
    CPPUNIT_ASSERT_EQUAL(true,cd->testInProgress);
    CPPUNIT_ASSERT_EQUAL(true,cd->vehiclePresent);
    CPPUNIT_ASSERT_EQUAL(true,cd->rollsDown);
    CPPUNIT_ASSERT_EQUAL(true,cd->rollsUp);
    CPPUNIT_ASSERT_EQUAL(true,cd->calCycle);
    CPPUNIT_ASSERT_EQUAL(true,cd->calSwitch);
    CPPUNIT_ASSERT_EQUAL(true,cd->calSwitchToPLC);
    CPPUNIT_ASSERT_EQUAL(true,cd->calInProgress);
    CPPUNIT_ASSERT_EQUAL(false,cd->simSwitch);
    CPPUNIT_ASSERT_EQUAL(false,cd->simCycle);
    CPPUNIT_ASSERT_EQUAL(true,cd->cableConnect6Pin);
    CPPUNIT_ASSERT_EQUAL(true,cd->cableConnect8Pin);
    CPPUNIT_ASSERT_EQUAL(true,cd->cableConnect16Pin);
    CPPUNIT_ASSERT_EQUAL(string("Normal1"),cd->inputServerState);
    CPPUNIT_ASSERT_EQUAL(true,cd->powerSense);
    CPPUNIT_ASSERT_EQUAL(true,cd->rackFault);
    CPPUNIT_ASSERT_EQUAL(true,cd->wheelbaseInPosition);
    CPPUNIT_ASSERT_EQUAL(true,cd->vmeSystemReady);
    CPPUNIT_ASSERT_EQUAL(true,cd->abortPb);
    CPPUNIT_ASSERT_EQUAL(true,cd->cableConnect);
    CPPUNIT_ASSERT_EQUAL(true,cd->plcSystemIsReady);
    CPPUNIT_ASSERT_EQUAL(false,cd->reprintRequest);
    CPPUNIT_ASSERT_EQUAL(string("12"),cd->keyPress);
    CPPUNIT_ASSERT_EQUAL(string("Blank"),sm->GetData(PENDANT_KEY_PRESS));
    CPPUNIT_ASSERT_EQUAL(sm->m_oldCtrl->simInProgress,cd->simInProgress);
    CPPUNIT_ASSERT_EQUAL(sm->m_oldCtrl->testRequested,cd->testRequested);
    CPPUNIT_ASSERT_EQUAL(sm->m_oldCtrl->calKeyCycled,cd->calKeyCycled);
    CPPUNIT_ASSERT_EQUAL(sm->m_oldCtrl->calConditionsCorrect,
                         cd->calConditionsCorrect);

    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::GetCurrentControlDataTest_Reprint(void)
{
    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    XmlNode *testInProgressNode = new XmlNode(TEST_IN_PROG_DATA_TAG,"0");
    XmlNode *rollsDownNode = new XmlNode(ROLLS_DOWN_DATA_TAG,"1");
    XmlNode *keyPressNode = new XmlNode(PENDANT_KEY_PRESS,"FunctionKey1");

    sm->SetData(testInProgressNode);
    sm->SetData(rollsDownNode);
    sm->SetData(keyPressNode);

    sm->GetCurrentControlData(cd);

    CPPUNIT_ASSERT_EQUAL(true,cd->reprintRequest);

    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_NoFaults(void)
{
    ControlData* cd = sm->ControlAllocate();

    // make sure no machine fault conditions are set
    cd->powerSense = true;
    cd->rackFault = false;
    cd->vmeSystemReady = false;
    sm->m_plcSendsPLCSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure there is no machine fault set
    CPPUNIT_ASSERT_EQUAL(false,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_PowerSenseFault(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // send the powersense fault to the fault server
    sm->m_useFaultServer = true;    

    // set the power sense fault and no other faults
    sm->m_oldCtrl->powerSense = true;
    cd->powerSense = false;
    cd->rackFault = false;
    cd->vmeSystemReady = true;
    sm->m_plcSendsPLCSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_setFaultResponse.push_back("Pass");
    mfs->m_setFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure the machine fault is set
    CPPUNIT_ASSERT_EQUAL(true,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(string("PowerSenseLostToIORack"),
                         mfs->m_setFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(string("System power has been lost"),
                         mfs->m_setFaultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(sm->GetProcessName(),mfs->m_setFaultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_setFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(9,mfs->m_setFaultParamSeverity[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_RackFault(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // send the rack fault to the fault server
    sm->m_useFaultServer = true;    

    // set the rack fault and no other faults
    cd->powerSense = true;
    sm->m_oldCtrl->rackFault = false;
    cd->rackFault = true;
    cd->vmeSystemReady = true;
    sm->m_plcSendsPLCSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_setFaultResponse.push_back("Pass");
    mfs->m_setFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure the machine fault is set
    CPPUNIT_ASSERT_EQUAL(true,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(string("PLCRackFault"),mfs->m_setFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(string("PLC rack fault detected"),
                         mfs->m_setFaultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(sm->GetProcessName(),mfs->m_setFaultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_setFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(9,mfs->m_setFaultParamSeverity[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_PlcSystemIsReadyFault(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // initialize the change semaphore
    sm->m_updateControl = new BepSemaphore(1,sm->IsDebugOn(),"SystemMonitor");

    // send the PLC not ready fault to the fault server
    sm->m_useFaultServer = true;    

    // set the PLC is ready fault and no other faults
    cd->powerSense = true;
    cd->rackFault = false;
    cd->vmeSystemReady = true;
    sm->m_plcSendsPLCSystemIsReady = true;
    cd->plcSystemIsReady = false;
    sm->m_oldCtrl->plcSystemIsReady = true;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock prompt server and set the data
    sm->m_promptServer = new MockPromptServer();
    MockPromptServer *mps = (MockPromptServer*)sm->m_promptServer;
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_setFaultResponse.push_back("Pass");
    mfs->m_setFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure the machine fault is set
    CPPUNIT_ASSERT_EQUAL(true,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("0"),mndb->m_writeParamValue[0]);

    // make sure the abort tag was written
    CPPUNIT_ASSERT_EQUAL(string("1"),
                         sm->m_data.getNode(ABORT_DATA_TAG)->getValue());

    // make sure the PLC is ready prompt was displayed
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptCalled);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("Blank"),mps->m_displayPromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptParamPromptBox[1]);
    CPPUNIT_ASSERT_EQUAL(string("PLCSystemIsReady"),
                         mps->m_displayPromptParamPrompt[1]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(string("PLCSystemIsReadyFault"),
                         mfs->m_setFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(string("PLC is not ready"),
                         mfs->m_setFaultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(sm->GetProcessName(),mfs->m_setFaultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_setFaultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(9,mfs->m_setFaultParamSeverity[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_PowerSenseFaultCleared(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // fault server in use
    sm->m_useFaultServer = true;    

    // make sure no machine fault conditions are set
    cd->powerSense = true;
    sm->m_oldCtrl->powerSense = false;
    cd->rackFault = false;
    cd->vmeSystemReady = false;
    sm->m_plcSendsPLCSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure there is no machine fault set
    CPPUNIT_ASSERT_EQUAL(false,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_clearFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("PowerSenseLostToIORack"),
                         mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::CheckMachineStatusTest_RackFaultCleared(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // fault server in use
    sm->m_useFaultServer = true;    

    // make sure no machine fault conditions are set
    cd->powerSense = true;
    sm->m_oldCtrl->powerSense = true;
    cd->rackFault = false;
    sm->m_oldCtrl->rackFault = true;
    cd->vmeSystemReady = false;
    sm->m_plcSendsPLCSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure there is no machine fault set
    CPPUNIT_ASSERT_EQUAL(false,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(1,mfs->m_clearFaultCalled);
    CPPUNIT_ASSERT_EQUAL(string("PLCRackFault"),mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void 
SystemMonitorTest::CheckMachineStatusTest_PlcSystemIsReadyFaultCleared(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->m_faultInformation = document->getChild("Setup/SystemFaults")->Copy();

    ControlData* cd = sm->ControlAllocate();
    sm->m_oldCtrl = sm->ControlAllocate();

    // initialize the change semaphore
    sm->m_updateControl = new BepSemaphore(1,sm->IsDebugOn(),"SystemMonitor");

    // fault server in use
    sm->m_useFaultServer = true;    

    // make sure no machine fault conditions are set
    cd->powerSense = true;
    cd->rackFault = false;
    cd->vmeSystemReady = false;
    sm->m_plcSendsPLCSystemIsReady = true;
    cd->plcSystemIsReady = true;
    sm->m_oldCtrl->plcSystemIsReady = false;

    // create a mock named data broker and set the data
    sm->m_dataBroker = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)sm->m_dataBroker;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock prompt server and set the data
    sm->m_promptServer = new MockPromptServer();
    MockPromptServer *mps = (MockPromptServer*)sm->m_promptServer;
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // create a mock fault server and set the data
    sm->m_faultServer = new MockFaultServer();
    MockFaultServer *mfs = (MockFaultServer*)sm->m_faultServer;
    mfs->m_clearFaultResponse.push_back("Pass");
    mfs->m_clearFaultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->CheckMachineStatus(cd);

    // make sure there is no machine fault set
    CPPUNIT_ASSERT_EQUAL(false,cd->machineFault);

    // make sure the vme system ready tag was written
    CPPUNIT_ASSERT_EQUAL(1,mndb->m_writeCalled);
    CPPUNIT_ASSERT_EQUAL(string(VME_SYSTEM_READY_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);

    // make sure the abort tag was written
    CPPUNIT_ASSERT_EQUAL(string("0"),
                         sm->m_data.getNode(ABORT_DATA_TAG)->getValue());

    // make sure the PLC is ready prompt was cleared
    CPPUNIT_ASSERT_EQUAL(1,mps->m_removePromptCalled);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_removePromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("PLCSystemIsReady"),
                         mps->m_removePromptParamPrompt[0]);

    // make sure the information sent to the fault server was correct
    CPPUNIT_ASSERT_EQUAL(string("PLCSystemIsReadyFault"),
                         mfs->m_clearFaultParamFault[0]);
    CPPUNIT_ASSERT_EQUAL(true,(bool)mfs->m_clearFaultParamIsFinal[0]);

    // free the control data
    sm->ControlFree(cd);
}

//=============================================================================
void SystemMonitorTest::DisplayPromptTestWithColor(void)
{
    // create a mock prompt server and set the data
    sm->m_promptServer = new MockPromptServer();
    MockPromptServer *mps = (MockPromptServer*)sm->m_promptServer;
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->DisplayPrompt(1,"TestTag","white",true,2);

    // make sure DisplayPrompt was called twice
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptCalled);

    // check the data associated with the call to clear the prompt box
    CPPUNIT_ASSERT_EQUAL(1,mps->m_displayPromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("Blank"),mps->m_displayPromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptParamPriorityLevel[0]);

    // check the data associated with the call to display the prompt
    CPPUNIT_ASSERT_EQUAL(1,mps->m_displayPromptParamPromptBox[1]);
    CPPUNIT_ASSERT_EQUAL(string("TestTag"),mps->m_displayPromptParamPrompt[1]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptParamPriorityLevel[1]);

    // check the data associated with setting the background color
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorParamPromptBox);
    CPPUNIT_ASSERT_EQUAL(string("white"),mps->m_setBackgroundColorParamColor);
}

//=============================================================================
void SystemMonitorTest::DisplayPromptTestNoColor(void)
{
    // create a mock prompt server and set the data
    sm->m_promptServer = new MockPromptServer();
    MockPromptServer *mps = (MockPromptServer*)sm->m_promptServer;
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->DisplayPrompt(1,"TestTag",true,2);

    // check the data associated with the call to RemovePrompt()
    CPPUNIT_ASSERT_EQUAL(1,mps->m_removePromptCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_removePromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("Blank"),mps->m_removePromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_removePromptParamPriorityLevel[0]);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorParamPromptBox);
    CPPUNIT_ASSERT_EQUAL(string("white"),mps->m_setBackgroundColorParamColor);

    // check the data associated with the call to display the prompt
    CPPUNIT_ASSERT_EQUAL(1,mps->m_displayPromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("TestTag"),mps->m_displayPromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptParamPriorityLevel[0]);
}

//=============================================================================
void SystemMonitorTest::RemovePromptTest(void)
{
    // create a mock prompt server and set the data
    sm->m_promptServer = new MockPromptServer();
    MockPromptServer *mps = (MockPromptServer*)sm->m_promptServer;
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    sm->RemovePrompt(1,"TestTag","white",2);

    // check the data associated with the call to RemovePrompt()
    CPPUNIT_ASSERT_EQUAL(1,mps->m_removePromptCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_removePromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(string("TestTag"),mps->m_removePromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_removePromptParamPriorityLevel[0]);

    // check the data associated with setting the background color
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_setBackgroundColorParamPromptBox);
    CPPUNIT_ASSERT_EQUAL(string("white"),mps->m_setBackgroundColorParamColor);
}

//=============================================================================
void SystemMonitorTest::Initialize_UseFaultServer(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->Initialize(document);

    CPPUNIT_ASSERT_EQUAL(true,sm->m_useFaultServer);
}

//=============================================================================
void SystemMonitorTest::Initialize_UseFaultServerMissing(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemMonitorConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    sm->Initialize(document);

    CPPUNIT_ASSERT_EQUAL(false,sm->m_useFaultServer);
}

//=============================================================================
void SystemMonitorTest::SetSystemMonitorRegisteredTest(void)
{
    sm->SetSystemMonitorRegistered(true);

    CPPUNIT_ASSERT_EQUAL(true,sm->m_sysMonRegistered);
}

//=============================================================================
void SystemMonitorTest::IsSystemMonitorRegisteredTest(void)
{
    CPPUNIT_ASSERT_EQUAL(false,sm->IsSystemMonitorRegistered());
}
