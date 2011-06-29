//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLNodeMapTest.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//  The Test Class For XmlNodeMap.
//	
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLNodeMapTest.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
//*************************************************************************
#ifndef XMLNODEMAPTEST_H
#define XMLNODEMAPTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "XmlParser.h"
#include "DebugLevel.h"

/**
 * Test class for XmlNodeMap
 *
 * @author Kevin Hykin
 * @version 1.0
 * @since Version 1.0
 */
class XMLNodeMapTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( XMLNodeMapTest );
    CPPUNIT_TEST( DuplicateNodeWithMapClear);
    CPPUNIT_TEST( DeleteNodeTestWithClear);
    CPPUNIT_TEST( MultipleClearTest);
    CPPUNIT_TEST( NodeMapCopyAndClearTest);
    CPPUNIT_TEST( AddSameObjectTwice);
    /* put the rest of the tests here */
    CPPUNIT_TEST_SUITE_END();
	
public:
	/**
	 * Class constructor
	 *
	 * @since Version 1.0
	 */
	XMLNodeMapTest();
	/**
	 * Class destructor
	 * @since Version 1.0
	 */
	virtual ~XMLNodeMapTest();
	
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
     * Test that will make sure that if an XMLNodeMap has duplicate
     * nodes added too it, it will perform a clear successfully.
     */
    void DuplicateNodeWithMapClear(void);

    /**
     * Test that will make sure that if an XMLNodeMap has nodes
     * added too it, it will perform a delNode and then a clear
     * successfully.
     */
    void DeleteNodeTestWithClear(void);

    /**
     * Test that will populate a map and clear it multiple
     * to ensure that this is an acceptable situation.
     */
    void MultipleClearTest(void);

    /**
     * This test will populate a node map, copy it, and then
     * delete it multiple times to make sure there are no issues.
     */
    void NodeMapCopyAndClearTest(void);

    /**
     * This test will add the same object twiced and validate that
     * the object still exists.
     */
    void AddSameObjectTwice(void);

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
#endif //XMLNODEMAPTEST_H
