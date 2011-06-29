//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLParserTest.h 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//  The Test Class For Xml Parser.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLParserTest.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:57p Cward
//
// 1     1/12/04 2:53p Khykin
// Initial checkin.
//
// 11    9/12/02 5:55p Tbochene
// Updated to allow skipped test reporting
//
// 10    6/04/02 6:33p Skypig13
// Added GetChildsChildTest()
//
// 9     6/28/00 4:46p Skypig13
// Added AddNodeToAnotherTest()
//
// 8     6/28/00 10:53a Skypig13
// Changed cout to printf, and added test for adding an XML node
// as a value of a node in another XML document
//
// 7     5/22/01 3:07p Skypig13
// Add delChild() test
//
// 5     4/25/00 9:40a Skypig13
//
// 4     4/24/00 6:21p Skypig13
//
//*************************************************************************
#ifndef XMLPARSERTEST_H
#define XMLPARSERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "XmlParser.h"
#include "DebugLevel.h"
/**
 * Test class for Xml Parser
 *
 * @author Tim Bochenek
 * @version 2.2
 * @since Version 1.0
 */
class XMLParserTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( XMLParserTest );
    CPPUNIT_TEST( NotWellFormedTest);
    CPPUNIT_TEST( GetChildNotPresentTest);
    CPPUNIT_TEST( AddNodeTest);
    CPPUNIT_TEST( WriteValueFromFileTest);
    CPPUNIT_TEST( WriteAttributeTest);
    CPPUNIT_TEST( AddManyNodesTest);
    CPPUNIT_TEST( DeleteNodeTest);
    CPPUNIT_TEST( AddXmlDocumentAsChildNodeValueTest);
    CPPUNIT_TEST( AddNodeToAnotherTest);
    CPPUNIT_TEST( GetChildsChildTest);
    CPPUNIT_TEST( AddDuplicateNodeTest);
    CPPUNIT_TEST( DuplicateNodeMemoryLeakTest);
    CPPUNIT_TEST( ReturnXMLDocMemoryLeakTest);
    /* put the rest of the tests here */
    CPPUNIT_TEST_SUITE_END();
	
public:
	/**
	 * Class constructor
	 *
	 * @param name     Name of test class
	 * @since Version 1.0
	 */
	XMLParserTest();
	/**
	 * Class destructor
	 * @since Version 1.0
	 */
	virtual ~XMLParserTest();
	
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
	 * Tests the effects of getting a non-existent node
	 * @since Version 1.0
	 */
	void GetChildNotPresentTest(void);
	/**
	 * This function tests that the expat forces a not
     * well-formed document to be parsable.
	 * @since Version 1.0
	 */
	void NotWellFormedTest(void);
	/**
	 * This function tests writing a node to an XML document
	 * @since Version 1.0
	 */
	void AddNodeTest(void);
	/**
	 * This function tests writing an attribute to a node in an XML document
	 * @since Version 1.0
	 */
	void WriteAttributeTest(void);
	/**
	 * Test changing the value of a node from an xml file.
	 * @since Version 1.0
	 */
	void WriteValueFromFileTest(void);
	/**
	 * Test adding many nodes to an xml file.
	 * @since Version 1.0
	 */
	void AddManyNodesTest(void);
	/**
	 * Tests the ability to delete a node from an Xml document
	 *
	 * @since Version 2.0
	 */
	void DeleteNodeTest(void);
	/**
	 * Tests the ability to add an Xml document as a value in a child node
	 *
	 * @since Version 2.1
	 */
	void AddXmlDocumentAsChildNodeValueTest(void);
	/**
	 * Tests the ability to add an Xml node as a child of another node
	 *
	 * @since Version 2.2
	 */
	void AddNodeToAnotherTest(void);
	/**
	 * Tests the ability to get a node under a child with getChild()
	 *
	 * @since Version 2.2
	 */
	void GetChildsChildTest(void);
	/**
	 * Tests to make sure that the memory is handled correctly if
     * a duplicate node is added to a XML document
	 *
	 * @since Version 2.3
	 */
    void AddDuplicateNodeTest(void);
	/**
	 * Tests to make sure that there is not a memory leak when
     * many duplicate nodes are to an XML document
	 *
	 * @since Version 2.3
	 */
    void DuplicateNodeMemoryLeakTest(void);
	/**
	 * Tests to make sure that there is not a memory leak when
     * calls are made to Return an XML document
	 */
	void XMLParserTest::ReturnXMLDocMemoryLeakTest(void);


protected:
	/**
	 * Adds a child node to the parent.
	 *
	 * @param Parent    Parent node
	 * @param NewName   Name of new node
	 * @param NewValue  Value to be inserted into new node
	 * @param NextValue Value to cahnge new node to, after creating it
	 * @param NumChildren Number of children to create (child, grandchild, great-grandchild, etc.)
	 * @since Version 1.0
	 */
	void AddNodes(XmlNode *Parent, const XmlString &NewName, const XmlString &NewValue, const XmlString &NextValue, const int NumChildren);

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
#endif //XMLPARSERTEST_H
