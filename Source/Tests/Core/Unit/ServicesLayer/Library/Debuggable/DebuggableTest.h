//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/Debuggable/DebuggableTest.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//  The Test Class For Debuggable.
//	
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/Debuggable/DebuggableTest.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
//*************************************************************************
#ifndef DEBUGGABLETEST_H
#define DEBUGGABLETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "DebugLevel.h"

/**
 * Test class for
 *
 */
class DebuggableTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( DebuggableTest );
    CPPUNIT_TEST( DefaultConstructorTest);
    CPPUNIT_TEST( BoolContructorTest);
    CPPUNIT_TEST( IntContructorTest);
    CPPUNIT_TEST( SetFunctionsTest);
    CPPUNIT_TEST( InvalidValuesTest);
    /* put the rest of the tests here */
    CPPUNIT_TEST_SUITE_END();
	
public:
	/**
	 * Class constructor
	 */
	DebuggableTest();
	/**
	 * Class destructor
	 */
	virtual ~DebuggableTest();
	
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

/*********************************************************
	Debuggable Tests
 *********************************************************/	
	
    /**
     * Tests that the Default Constructor sets the ivars
     * as expected.
     */
    void DefaultConstructorTest(void);

    /**
     * Test that bool parameter form of the Constructor set
     * the ivars as expected.
     */
    void BoolContructorTest(void);

    /**
     * Test that integer parameter form of the
     * Constructor sets the ivars as expected.
     */
    void IntContructorTest(void);

    /**
     * Test set functions
     */
    void SetFunctionsTest(void);

    /**
     * Test set functions
     */
    void InvalidValuesTest(void);


private:
    /**
     * The directory to find the configuration files in.
     */
    std::string m_configDir;

    /**
     * Variable that holds the FTP_ROOT value prior to testing so
     * it can be restored.
     * @since Version 1.0
     */
    std::string m_ftpRoot;

    /**
     * Variable that holds the USR_ROOT value prior to testing so
     * it can be restored.
     * @since Version 1.0
     */
    std::string m_usrRoot;

    /**
     * Variable that holds the current debug level
     */
    DebugLevel m_debug;

};
#endif //DEBUGGABLETEST_H
