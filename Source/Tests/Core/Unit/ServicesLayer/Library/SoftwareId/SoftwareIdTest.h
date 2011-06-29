//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SoftwareId/SoftwareIdTest.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//  The Test Class For SoftwareID.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SoftwareId/SoftwareIdTest.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
//*************************************************************************
#ifndef SOFTWAREIDTEST_H
#define SOFTWAREIDTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "DebugLevel.h"

/**
 * Test class for SoftwareId
 *
 */
class SoftwareIdTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( SoftwareIdTest );
    CPPUNIT_TEST( ConstructionTest);
    /* put the rest of the tests here */
    CPPUNIT_TEST_SUITE_END();
	
public:
	/**
	 * Class constructor
	 */
	SoftwareIdTest();
	/**
	 * Class destructor
	 */
	virtual ~SoftwareIdTest();
	
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
	XmlParser Tests
 *********************************************************/	
	
    /**
     * Test that will verify the proper construction of a
     * SoftwareId object.
     */
    void ConstructionTest(void);

    /**
     * Test that will ....
     */
//    void Testname(void);


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
#endif //SOFTWAREIDTEST_H
