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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemMonitor/SystemMonitorTest.h $
// 
// 2     2/22/07 1:26p Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added CheckMachineStatusTest_PowerSenseFaultCleared(),
// CheckMachineStatusTest_RackFaultCleared(), and
// CheckMachineStatusTest_PlcSystemIsReadyFaultCleared().
// 
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
#ifndef SystemMonitorTest_h
#define SystemMonitorTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class MockSystemMonitor;

class SystemMonitorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SystemMonitorTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(ControlDataConstructorTest);
    CPPUNIT_TEST(AssignTest);
    CPPUNIT_TEST(GetCurrentControlDataTest);
    CPPUNIT_TEST(GetCurrentControlDataTest_Reprint);
    CPPUNIT_TEST(CheckMachineStatusTest_NoFaults);
    CPPUNIT_TEST(CheckMachineStatusTest_PowerSenseFault);
    CPPUNIT_TEST(CheckMachineStatusTest_RackFault);
    CPPUNIT_TEST(CheckMachineStatusTest_PlcSystemIsReadyFault);
    CPPUNIT_TEST(CheckMachineStatusTest_PowerSenseFaultCleared);
    CPPUNIT_TEST(CheckMachineStatusTest_RackFaultCleared);
    CPPUNIT_TEST(CheckMachineStatusTest_PlcSystemIsReadyFaultCleared);
    CPPUNIT_TEST(DisplayPromptTestWithColor);
    CPPUNIT_TEST(DisplayPromptTestNoColor);
    CPPUNIT_TEST(RemovePromptTest);
    CPPUNIT_TEST(Initialize_UseFaultServer);
    CPPUNIT_TEST(Initialize_UseFaultServerMissing);
    CPPUNIT_TEST(SetSystemMonitorRegisteredTest);
    CPPUNIT_TEST(IsSystemMonitorRegisteredTest);
    CPPUNIT_TEST_SUITE_END();

public:
    SystemMonitorTest();
    ~SystemMonitorTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the SystemMonitor object is constructed properly.
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the ControlData object is constructed properly.
     */ 
    void ControlDataConstructorTest(void);
    /**
     * Test that will verify Assign() properly copies a ControlData object.
     */ 
    void AssignTest(void);
    /**
     * Test that will verify GetCurrentControlData() properly sets the 
     * ControlData values.
     */ 
    void GetCurrentControlDataTest(void);
    /**
     * Test that will verify GetCurrentControlData() proplerly sets the reprint
     * flag when the conditions are correct.
     */ 
    void GetCurrentControlDataTest_Reprint(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where there are no faults.
     */ 
    void CheckMachineStatusTest_NoFaults(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where there is a power sense fault.
     */ 
    void CheckMachineStatusTest_PowerSenseFault(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where there is a rack fault.
     */ 
    void CheckMachineStatusTest_RackFault(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where there is a PLC system is ready fault.
     */ 
    void CheckMachineStatusTest_PlcSystemIsReadyFault(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where a power sense fault has just cleared.
     */ 
    void CheckMachineStatusTest_PowerSenseFaultCleared(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where a rack fault has just cleared.
     */ 
    void CheckMachineStatusTest_RackFaultCleared(void);
    /**
     * Test that will verify CheckMachineStatus() properly handles the case
     * where the PLC is ready fault just cleared.
     */ 
    void CheckMachineStatusTest_PlcSystemIsReadyFaultCleared(void);
    /**
     * Test that will verify the DisplayPrompt() that takes a color works as
     * expected.
     */ 
    void DisplayPromptTestWithColor(void);
    /**
     * Test that will verify the DisplayPrompt() that doesn't take a color works 
     * as expected.
     */ 
    void DisplayPromptTestNoColor(void);
    /** 
     * Test that will verify RemovePrompt() works as expected. 
     */ 
    void RemovePromptTest(void);
    /**
     * Test that will verify Initialize() properly reads in the UseFaultServer
     * parameter.
     */ 
    void Initialize_UseFaultServer(void);
    /**
     * Test that will verify Initialize() properly sets m_useFaultServer to
     * its default value when the tag is missing from the configuration file.
     */ 
    void Initialize_UseFaultServerMissing(void);
    /**
     * Test that will verify SetSystemMonitorRegistered() properly sets the
     * member variable that tells if the system monitor has registered.
     */ 
    void SetSystemMonitorRegisteredTest(void);
    /**
     * Test that will verify IsSystemMonitorRegistered() properly returns the
     * member variable that tells if the system monitor has registered.
     */ 
    void IsSystemMonitorRegisteredTest(void);

private:
    MockSystemMonitor *sm;

    /** Task id for the mock named data broker */
    int m_ndbId;
    /** The path to the test configuration files */
    string m_configPath;
};
#endif // SystemMonitorTest_h
