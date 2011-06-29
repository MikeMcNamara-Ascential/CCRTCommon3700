//******************************************************************************
// Description:
//  The Test Class For Testing the Base Brake Tool Class.
//
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/BaseBrakeToolTest.h $
// 
// 4     10/31/07 10:18a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added AnalyzeBrakeForcesTest(),
// AnalyzeBrakeForcesTest_AveragingError(), and
// AnalyzeBrakeForcesTest_NoMultiplier().
// 
// 3     1/18/07 4:02p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitializeTest() and DeactivateTest().
// 
// 2     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 4:15p Rwiersem
// Added m_motorControllerId to keep track of the id returned when
// registering "core/MotorController".
//
// 1     7/20/05 8:14p Cward
// Directory structure change
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BASEBRAKETOOLTEST_H
#define BASEBRAKETOOLTEST_H
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

/**
 * Test class for
 *
 */
class BaseBrakeToolTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BaseBrakeToolTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(WarmInitializeTest);
    CPPUNIT_TEST(DeactivateTest);
    CPPUNIT_TEST(AnalyzeBrakeForcesTest);
    CPPUNIT_TEST(AnalyzeBrakeForcesTest_AveragingError);
    CPPUNIT_TEST(AnalyzeBrakeForcesTest_NoMultiplier);
    CPPUNIT_TEST_SUITE_END();

public:
    /**
     * Class constructor
     */
    BaseBrakeToolTest();

    /**
     * Class destructor
     */
    virtual ~BaseBrakeToolTest();

    /**
     * Setup is run before each test, use it to
     * set up the member ivars for use during
     * the test
     */
    void setUp();

    /**
     * TearDown is called after each test, use it
     * if you need to clean things up after your
     * test
     */
    void tearDown();

    /**
     * Test that will verify that the object is correctly initialized on construction
     */
    void ConstructorTest(void);

    /**
     * Test that will verify WarmInitialize() properly sets the member 
     * variables to their default values.
     */ 
    void WarmInitializeTest(void);

    /**
     * Test that will verify Deactivate() properly sets the member variables
     * to their default values.
     */ 
    void DeactivateTest(void);

    /**
     * Test that will verify AnalyzeBrakeForces() properly handles the 
     * successful case.
     */ 
    void AnalyzeBrakeForcesTest(void);

    /**
     * Test that will verify AnalyzeBrakeForces() properly handles the case
     * where there is an error averaging the brake forces.
     */ 
    void AnalyzeBrakeForcesTest_AveragingError(void);

    /**
     * Test that will verify AnalyzeBrakeForces() properly handles the case
     * where no multiplier parameter exists.
     */ 
    void AnalyzeBrakeForcesTest_NoMultiplier(void);

private:
    /**
     * The directory to find the configuration files in.
     */
    std::string m_configDir;

    /** Task id for the mock motor controller */
    int m_motorControllerId;
};
#endif //BASEBRAKETOOLTEST_H
