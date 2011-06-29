//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLParserTest.cpp 4     5/24/06 3:02p Cward $
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLParserTest.cpp $
// 
// 4     5/24/06 3:02p Cward
// Updated to remove compilation warnings. 
//
// 3     3/02/06 12:45a Cward
// Updated test dir path to accomodate core and non core testing (moved
// base point up one level)
//
// 2     8/17/05 8:33p Cward
// Compilation warnings
//
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     1/06/05 3:57p Cward
//
// 1     1/12/04 2:53p Khykin
// Initial checkin.
//
// 18    9/12/02 5:55p Tbochene
// Updated to allow skipped test reporting
//
// 17    6/04/02 6:33p Skypig13
// Added GetChildsChildTest()
//
// 16    2/04/02 10:10p Skypig13
// Updated to use new IBepCommunication and removed compiler warnings
//
// 15    10/08/01 7:18p Skypig13
// Changed configuration files to be relative to user root path
//
// 14    8/31/01 4:32p Skypig13
// Removed printouts in NotValidTest() associated with new parser
// printouts
//
// 13    8/23/01 2:10p Skypig13
// Added additional tests for handling not well-formed documents.
//
// 12    6/28/00 4:48p Skypig13
// Added AddNodeToAnotherTest()
//
// 11    6/28/00 10:53a Skypig13
// Changed cout to printf, and added test for adding an XML node
// as a value of a node in another XML document
//
// 10    6/05/01 8:17a Skypig13
// Rename children node names in DeleteNodeTest() to conform to new Xml
// standards.
//
// 9     5/22/01 3:07p Skypig13
// Add delChild() test
//
// 8     5/15/01 9:28a Skypig13
// Convert to new TestCase inheritance
//
// 6     5/01/01 3:13p Skypig13
// Tweak to match coding standards
//
// 5     4/25/00 9:40a Skypig13
//
// 4     4/24/00 6:21p Skypig13
//
//*************************************************************************
#include <unistd.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include "XMLParserTest.h"

XMLParserTest::XMLParserTest(void)
{
}
XMLParserTest::~XMLParserTest(void)
{
}

void XMLParserTest::setUp()
{
    // Set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_XMLPARSER");

    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/Core/Unit/ServicesLayer/Library/XmlProcessor/ConfigFiles/";
    if(m_debug.On())
        printf("The new configuration directory: %s\n", m_configDir.c_str());


    // Update the FTP_ROOT environment path
    m_ftpRoot = getenv("FTP_ROOT");
    setenv("FTP_ROOT", topTestDir.c_str(), 1);
    // Update the USR_ROOT environment path
    m_usrRoot = getenv("USR_ROOT");
    setenv("USR_ROOT", topTestDir.c_str(), 1);
}

void XMLParserTest::tearDown()
{
    if(m_debug.On())
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);
}
void XMLParserTest::GetChildNotPresentTest(void)
{
	try
	{
        // create the configuration file
        std::string configFile = m_configDir + "GetChildNotPresentTest.xml";
        if(m_debug.On()) printf("Using configuration file: %s\n", configFile.c_str());

        // parse the configuration file
        XmlParser parser;
        const XmlNode *doc = parser.ReturnXMLDocument(configFile);

        // There should be no children and an exception should be raised
        CPPUNIT_ASSERT_MESSAGE("Unexpected Children In Document", !doc->getChildren().size());

        if(m_debug.On()) printf("GetChildNotPresentTest (%s) done", configFile.c_str());
	}
	catch(XmlException &XmlErr)
	{	// This exception is what is expected
		std::string ExpectedError("No children found in file GetChildNotPresentTest.xml");
		CPPUNIT_ASSERT_MESSAGE("Unexpected Error Message", XmlErr.Reason() == ExpectedError);
	}
}
void XMLParserTest::NotWellFormedTest(void)
{
    try
    {
        // create the configuration file
        std::string configFile = m_configDir + "NotWellFormedTest.xml";
        if(m_debug.On()) printf("Using configuration file: %s\n", configFile.c_str());

    	// parse the configuration file
        XmlParser parser;
    	const XmlNode *doc = parser.ReturnXMLDocument(configFile);

    	CPPUNIT_ASSERT_MESSAGE("Document Is Null", doc != NULL);
    	CPPUNIT_ASSERT_MESSAGE("Document Size Incorrect", doc->getChildren().size() == 1);
    	CPPUNIT_ASSERT_MESSAGE("Document Root Node Incorrect", doc->getChildren().getNode(0)->getName() == "Test");
    	if(m_debug.On()) printf("NotWellFormedTest (%s) done", configFile.c_str());
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("GetChildsChildTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}
void XMLParserTest::AddNodeTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;

    try
    {
        // create a parent node
        parentNode = new XmlNode(XML_T("Parent"), XML_T("ParentValue"), NULL, ELEMENT_NODE);
        // create XML Strings to add the the XML node
    	const XmlString NewName(XML_T("NewNode")), NewValue(XML_T("Test")), NextValue(XML_T("Next"));
        // add the node to the parent
        parentNode->addChild(NewName, NewValue, ELEMENT_NODE);
        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getChildren().size() > 0));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Name Read", NewName, parentNode->getChildren().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Value Read", NewValue, parentNode->getChildren().getNode(0)->getValue());
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("AddNodeTest done");
    }
    catch(XmlException &e)
    {
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("AddNodeTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
	catch(...)
	{
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        throw;
    }
}
void XMLParserTest::WriteValueFromFileTest(void)
{
    try
    {
        // create the configuration file
        std::string configFile = m_configDir + "WriteValueFromFileTest.xml";
        if(m_debug.On()) printf("Using configuration file: %s\n", configFile.c_str());

    	// parse the configuration file
        XmlParser parser;
    	const XmlNode *doc = parser.ReturnXMLDocument(configFile);

        // Make sure we have children
        CPPUNIT_ASSERT_MESSAGE("Error No Children Exist", doc->getChildren().size() > 0);
    	// Make sure the child has a valid value
        XmlNode *child = (XmlNode *)doc->getChildren().getNode(0);
        CPPUNIT_ASSERT_MESSAGE("Invalid Child Value", (child->getValue().c_str() != ""));
        // create and set the new value
    	XmlString TestValue = XML_T("Test");
    	child->setValue(TestValue);
    	// read the value and validate it
        XmlString TestNextValue = (XmlString)child->getValue();
        CPPUNIT_ASSERT_MESSAGE("Invalid Node Value", TestValue == TestNextValue);
        if(m_debug.On()) printf("WriteValueFromFileTest (%s) done", configFile.c_str());
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("WriteValueFromFileTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}
void XMLParserTest::WriteAttributeTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;

    try
    {
        // create a parent node
        parentNode = new XmlNode(XML_T("Parent"), XML_T("ParentValue"), NULL, ELEMENT_NODE);
        // create XML Strings to add the the XML node
    	const XmlString NewName(XML_T("NewAttribute")), NewValue(XML_T("Test")), NextValue(XML_T("Next"));
        // add the node to the parent
        parentNode->addChild(NewName, NewValue, ATTRIBUTE_NODE);
        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getAttributes().size() > 0));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Attribute Name Read", NewName, parentNode->getAttributes().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Attribute Value Read", NewValue, parentNode->getAttributes().getNode(0)->getValue());
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("WriteAttributeTest done");
	}
    catch(XmlException &e)
    {
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("WriteValueFromFileTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
	catch(...)
	{
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        throw;
    }
}
void XMLParserTest::AddManyNodesTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;

    try
    {
        // create the configuration file
    	XmlParser parser;
    	const XmlString ParentName(XML_T("Parent")),
                        ParentValue(XML_T("ParentValue")),
                    	NewName(XML_T("NewNodeName")),
                    	NewValue(XML_T("TestValue")),
                    	NextValue(XML_T("NextValue")),
                    	xmlRoot(XML_T("Root"));
        parentNode = new XmlNode("Root", "", NULL, ELEMENT_NODE);
    	AddNodes(parentNode, NewName, NewValue, NextValue, 10);
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("AddManyNodesTest done");
	}
    catch(XmlException &e)
    {
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("AddManyNodesTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
	catch(...)
	{
    	if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        throw;
    }
}
void XMLParserTest::DeleteNodeTest(void)
{
    try
    {
        // create the configuration file
        std::string configFile = m_configDir + "DeleteNodeTest.xml";
        if(m_debug.On()) printf("Using configuration file: %s\n", configFile.c_str());

    	XmlParser parser;
    	XmlNode *doc = (XmlNode *)parser.ReturnXMLDocument(configFile);
        XmlNode *parent;
    	// Do we have children
        CPPUNIT_ASSERT_MESSAGE("Expected Children Do Not Exist", doc->getChildren().size() == 1);
    	// Do we have grandchildren
        CPPUNIT_ASSERT_MESSAGE("Expected Child of Children Do Not Exist", doc->getChildren().getNode("Child")->getChildren().size() == 1);
    	parent = (XmlNode *)doc->getChildren().getNode("Child");
    	parent->delChild("Grandchild");
    	// Do we still have children
        CPPUNIT_ASSERT_MESSAGE("Parent Does Not Exist After Parent delChild", doc->getChildren().size() == 1);
    	// Do we have grandchildren
        CPPUNIT_ASSERT_MESSAGE("Child Still Exists After delChild", parent->getChildren().size() == 0);
    	if(m_debug.On()) printf("DeleteNodeTest (%s) done", configFile.c_str());
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("DeleteNodeTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}
void XMLParserTest::AddXmlDocumentAsChildNodeValueTest(void)
{
    try
    {
        // create the configuration file
        std::string configFile = m_configDir + "AddXmlDocumentAsChildNodeValueTest.xml";
        if(m_debug.On()) printf("Using configuration file: %s\n", configFile.c_str());

    	XmlParser parser, anotherParser;
    	std::string testString, oldString;
        XmlNode *child, *test, *newDoc;
    	XmlNode *doc = (XmlNode *)parser.ReturnXMLDocument(configFile);
    	doc->writeNode(oldString);

    	child = (XmlNode *)doc->getChildren().getNode("Test");
    	child->addChild("Document", doc->ToString().c_str(), ELEMENT_NODE);

    	anotherParser.Parse(doc->ToString().c_str(), doc->ToString().length(), true, "");
    	// Get newly parsed document
        newDoc = (XmlNode *)anotherParser.getDocElement();
        CPPUNIT_ASSERT_MESSAGE("Null Document After Parse", newDoc != NULL);
    	test = (XmlNode *)newDoc->getChildren().getNode("Test")->getChildren().getNode("Document");
        // Should not get here unless document is not empty
        CPPUNIT_ASSERT_MESSAGE("Document Node Does Not Exist", test != NULL);
    	test->getChildren().getNode(BEP_XML_ROOT)->writeNode(testString);
    	CPPUNIT_ASSERT_MESSAGE("Created File and Parsed File Do Not Match", oldString == testString);
    	if(m_debug.On()) printf("AddXmlDocumentAsChildNodeValueTest (%s) done", configFile.c_str());
    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("AddXmlDocumentAsChildNodeValueTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}
void XMLParserTest::AddNodeToAnotherTest(void)
{	// 2 node pointers
	XmlElement  *root1=NULL, *root2=NULL;
	try
	{	// Create nodes
		std::string docString, childString;	// Strings for nodes

		root1 = new XmlElement( XML_T("ROOT1"), XML_T(""), NULL);
		root2 = new XmlElement( XML_T("ROOT2"), XML_T(""), NULL);
		// Make sure they got created
        CPPUNIT_ASSERT_MESSAGE("ROOT1 Element Not Created", root1 != NULL);
		CPPUNIT_ASSERT_MESSAGE("ROOT2 Element Not Created", root2 != NULL);
		// Add children to node 1
		XmlNode *child1 = root1->addChild( XML_T("Child11"), XML_T(""), ELEMENT_NODE);
		root1->addChild( XML_T("Child12"), XML_T(""), ELEMENT_NODE);
		// Add children to node 2
		root2->addChild( XML_T("Child21"), XML_T(""), ELEMENT_NODE);
		root2->addChild( XML_T("Child22"), XML_T(""), ELEMENT_NODE);
		root2->writeNode(docString);	// Get string representation of node
		// Print values
		if(m_debug.On())	printf("Doc1: %s\n", root1->ToString().c_str());
		if(m_debug.On())	printf("Doc2: %s\n", root2->ToString().c_str());

		XmlNode *copy = root2->Copy();	// Copy node 2
		child1->addChild( copy);		// Add the copy to node 1
		// Get child node (node that was added)
		XmlNode *test = (XmlNode *)root1->getChildren().getNode("Child11")->getChildren().getNode(0);
		test->writeNode(childString);	// Get string representation
		// Did the copy work
        CPPUNIT_ASSERT_MESSAGE("Documents Do Not Match", docString == childString);

		if(m_debug.On())	printf("Doc1: %s\n", root1->ToString().c_str());
		delete root1;	// Delete node 1
		root1 = NULL;	// Set it to null
		// Make sure it got deleted
        CPPUNIT_ASSERT_MESSAGE("Delete Of Root 1 Not Successful", (root1 == NULL));

		if(m_debug.On())	printf("Doc2: %s\n", root2->ToString().c_str());
		delete root2;	// Delete node 2
		root2 = NULL;	// Set it to null
		// Make sure it got deleted
        CPPUNIT_ASSERT_MESSAGE("Delete Of Root 2 Not Successful", (root2 == NULL));
		if(m_debug.On())	printf("AddNodeToAnotherTest done");
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("AddNodeToAnotherTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
		if(root1) delete root1;	// If root 1 still exists
		if(root2) delete root2;	// If root 2 still exists
    }
	catch(...)
	{
		if(root1) delete root1;	// If root 1 still exists
		if(root2) delete root2;	// If root 2 still exists
        throw;
	}
}
void XMLParserTest::GetChildsChildTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;

    try
    {

        // create a parent node
        parentNode = new XmlNode(XML_T("Parent"), XML_T("ParentValue"), NULL, ELEMENT_NODE);

        // create XML Strings to add the the XML node
    	const XmlString NewName(XML_T("NewNode")), NewValue(XML_T("Test"));
        // add the node to the parent
        parentNode->addChild(NewName, NewValue, ELEMENT_NODE);
        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getChildren().size() > 0));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Name Read", NewName, parentNode->getChildren().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Value Read", NewValue, parentNode->getChildren().getNode(0)->getValue());

        // create XML Strings to add the the XML node
    	const XmlString ChildName(XML_T("NewChildNode")), ChildValue(XML_T("Test"));
        // add the node to the parent
        XmlNode *child = const_cast<XmlNode *>(parentNode->getChild(NewName));
        child->addChild(ChildName, ChildValue, ELEMENT_NODE);

        // test the ability to get the childs child

        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getChild(NewName)->getChildren().size() > 0));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Name Read", NewName, parentNode->getChildren().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Value Read", NewValue, parentNode->getChildren().getNode(0)->getValue());

        // test the ability to get the childs child
        XmlNode *grandChild = const_cast<XmlNode *> (parentNode->getChild("NewNode/NewChildNode"));
        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (grandChild != NULL));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Grand Child Node Name Read", ChildName, grandChild->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Grand Child Node Value Read", ChildValue, grandChild->getValue());

        if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
        if(m_debug.On()) printf("AddNodeTest done");


	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("GetChildsChildTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}
void XMLParserTest::AddDuplicateNodeTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;

    try
    {
        // create a parent node
        parentNode = new XmlNode(XML_T("Parent"), XML_T("ParentValue"), NULL, ELEMENT_NODE);

        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue")), newValue2(XML_T("NewValue2"));
        XmlNode *firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
        XmlNode *secondNode = new XmlNode(newNode, newValue2, NULL, ELEMENT_NODE);

        // add the first node to the list
        parentNode->addChild(firstNode);

        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getChildren().size() == 1));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Name Read", newNode, parentNode->getChildren().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Value Read", newValue, parentNode->getChildren().getNode(0)->getValue());

        // add the first node to the list
        parentNode->addChild(secondNode);

        // test for the child
        CPPUNIT_ASSERT_MESSAGE("Child Not Added", (parentNode->getChildren().size() == 1));
        // Test if node name is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Name Read", newNode, parentNode->getChildren().getNode(0)->getName());
        // Test if node value is correct
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid Node Value Read", newValue2, parentNode->getChildren().getNode(0)->getValue());

        // test to determine if the nodes are now null
        try
        {
            CPPUNIT_ASSERT_MESSAGE("First Node Not Deleted", (firstNode->getName() != ""));
        }
        catch(...)
        { // this is good
        }
        CPPUNIT_ASSERT_MESSAGE("Second Child Deleted", (secondNode != NULL));

        if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }

        try
        {
            CPPUNIT_ASSERT_MESSAGE("Second Node Not Deleted", (secondNode->getName() != ""));
        }
        catch(...)
        { // this is good
        }

        if(m_debug.On()) printf("AddDuplicateNodeTest done");
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("GetChildsChildTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }
}


void XMLParserTest::DuplicateNodeMemoryLeakTest(void)
{
    // parent node used for the test
    XmlNode *parentNode = NULL;
    XmlNode *firstNode = NULL;
    XmlNode *secondNode = NULL;

    // Memory leak test
    struct mallinfo memInfo1, memInfo2;
    // get the memory at the beginning
    memInfo1 = mallinfo();

    try
    {
        // create a parent node
        parentNode = new XmlNode(XML_T("Parent"), XML_T("ParentValue"), NULL, ELEMENT_NODE);

        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue")), newValue2(XML_T("NewValue2"));

        for(int ii = 0; ii < 1000; ii++)
        {
            // add the first node to the list
            firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
            parentNode->addChild(firstNode);

            // add the second node to the list
            secondNode = new XmlNode(newNode, newValue2, NULL, ELEMENT_NODE);
            parentNode->addChild(secondNode);
        }


        if(parentNode != NULL)
        {
            delete parentNode;
            parentNode = NULL;
        }
	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("DuplicateNodeMemoryLeakTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    // get the memory at the end
    memInfo2 = mallinfo();
    // check them to be equal
    CPPUNIT_ASSERT_DOUBLES_EQUAL(memInfo1.arena, memInfo2.arena, 100);

#if 0
    printf( "\nMemory Stats:\n");
    printf( "%34.34s: %10d\t%10d\n", "Arena Size", memInfo1.arena, memInfo2.arena);
    printf( "%34.34s: %10d\t%10d\n", "Big Blocks in use", memInfo1.ordblks,  memInfo2.ordblks);
    printf( "%34.34s: %10d\t%10d\n", "Small blocks in use", memInfo1.smblks, memInfo2.smblks);
    printf( "%34.34s: %10d\t%10d\n", "Header blocks in use", memInfo1.hblks, memInfo2.hblks);
    printf( "%34.34s: %10d\t%10d\n", "Space in header block headers", memInfo1.hblkhd, memInfo2.hblkhd);
    printf( "%34.34s: %10d\t%10d\n", "Space in small blocks in use", memInfo1.usmblks, memInfo2.usmblks);
    printf( "%34.34s: %10d\t%10d\n", "Memory in free small blocks", memInfo1.fsmblks, memInfo2.fsmblks);
    printf( "%34.34s: %10d\t%10d\n", "Space in big blocks in use", memInfo1.uordblks, memInfo2.uordblks);
    printf( "%34.34s: %10d\t%10d\n", "Memory in free big blocks", memInfo1.fordblks, memInfo2.fordblks);
    printf( "%34.34s: %10d\t%10d\n", "KeepCost", memInfo1.keepcost, memInfo2.keepcost);
    printf( "\n\n");
#endif

    if(m_debug.On()) printf("DuplicateNodeMemoryLeakTest done");
}


void XMLParserTest::ReturnXMLDocMemoryLeakTest(void)
{

    // Memory leak test
    struct mallinfo memInfo1, memInfo2, memInfo3, memInfo4;
    // get the memory at the beginning

    try
    {

		std::string configFile = m_configDir + "ReturnXMLDocMemLeakTest.xml";
		{
			// create an XML Parser
			XmlParser tstParser;
			{
				// get memory info position 1
				memInfo1 = mallinfo();
				const XmlNode *tstConfig = tstParser.ReturnXMLDocument(configFile)->getChild("Setup/Communication");

                if(tstConfig == NULL)
                {
                    if(m_debug.On())
                    {
                        printf("ReturnXMLDocMemLeakTest tstConfig is NULL");
                    }
                }
				// get memory info position 2
				memInfo2 = mallinfo();
			}
			// get mem info after tstConfig out of scope
			memInfo3 = mallinfo();

		}
		// get mem info after tstParser goes out of scope
		memInfo4 = mallinfo();

//		printf( "%34.34s: %10d\t%10d\t%10d\t%10d\n", "Arena Size", memInfo1.arena, memInfo2.arena, memInfo3.arena, memInfo4.arena);


	}
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("ReturnXMLDocMemoryLeakTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    // check them to be equal
    CPPUNIT_ASSERT_DOUBLES_EQUAL(memInfo1.arena, memInfo4.arena, 100);


    if(m_debug.On()) printf("ReturnXMLDocMemoryLeakTest done");
}

void XMLParserTest::AddNodes(XmlNode *Parent, const XmlString &NewName, const XmlString &NewValue,
							 const XmlString &NextValue, const int NumChildren)
{
	XmlString Attribute, Value;
	XmlNode *NewNode;
	char Temp[10];
	for(int ii = 0; ii < NumChildren; ii++)
	{
		memset(Temp, 0, sizeof(Temp));
		itoa(ii, Temp, 10);
		XmlString TempString(Temp);
		Attribute   = XML_T("Attribute")+ TempString;
		Value       = XML_T("Value")    + TempString;
		NewNode = new XmlNode(NewName + TempString, NewValue + TempString, Parent);
		NewNode->addChild(Attribute, Value, ATTRIBUTE_NODE);
		CPPUNIT_ASSERT_MESSAGE("Child Not Added", (Parent->getChildren().size() > 0));
		const XmlString TestValue = Parent->getChildren().getNode(NewName + TempString)->getValue();
		// Test if node value was read out properly
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid TestValue Read", (std::string)(NewValue + TempString), (std::string)TestValue);

		XmlNode *child = (XmlNode *)Parent->getChildren().getNode(NewName + TempString);
		child->setValue(NextValue + TempString);

		const XmlString TestNextValue = child->getValue();
		// Test if node value was read out properly
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Invlaid TestNextValue Read", (std::string)NextValue + TempString, (std::string)TestNextValue);

		const XmlNodeMap Attribs = child->getAttributes();
		CPPUNIT_ASSERT_MESSAGE("Attributes Not Added", (Attribs.size() > 0));

		const XmlString TestAttribValue = Attribs.getNode(Attribute)->getValue();
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid TestAttribValue Read", (std::string)Value, (std::string)TestAttribValue);
		Parent = (XmlNode *)Parent->getChildren().getNode(0);
	}
}


