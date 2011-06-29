//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLNodeMapTest.cpp 3     5/24/06 3:02p Cward $
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/XmlProcessor/XMLNodeMapTest.cpp $
// 
// 3     5/24/06 3:02p Cward
// Updated to remove compilation warnings. 
//
// 2     3/02/06 12:45a Cward
// Updated test dir path to accomodate core and non core testing (moved
// base point up one level)
//
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     1/06/05 3:57p Cward
//
//
//*************************************************************************
#include <unistd.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include "XMLNodeMapTest.h"

XMLNodeMapTest::XMLNodeMapTest(void)
{
}
XMLNodeMapTest::~XMLNodeMapTest(void)
{
}

void XMLNodeMapTest::setUp()
{
    // Set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_XMLNODEMAP");

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

void XMLNodeMapTest::tearDown()
{
    if(m_debug.On())
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);
}

void XMLNodeMapTest::DuplicateNodeWithMapClear(void)
{
    // XmlNodeMap to use for test
    XmlNodeMap myNodeMap;

    // parent node used for the test
    XmlNode *firstNode = NULL;
    XmlNode *secondNode = NULL;

    try
    {
        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue")), newValue2(XML_T("NewValue2"));

        // add duplicates forever
        for(int ii = 0; ii < 1000; ii++)
        {
            // add the first node to the list
            firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
            myNodeMap.addNode(firstNode);

            // add the second node to the list
            secondNode = new XmlNode(newNode, newValue2, NULL, ELEMENT_NODE);
            myNodeMap.addNode(secondNode);
        }

        myNodeMap.clear(true);
    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("DuplicateNodeWithMapClear XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    if(m_debug.On()) printf("DuplicateNodeWithMapClear done");
}

void XMLNodeMapTest::DeleteNodeTestWithClear(void)
{
    // XmlNodeMap to use for test
    XmlNodeMap myNodeMap;

    // node used for the test
    XmlNode *firstNode = NULL;

    try
    {
        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue"));

        // add duplicates forever
        for(int ii = 0; ii < 1000; ii++)
        {
            // add the first node to the list
            firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
            myNodeMap.addNode(firstNode);
            myNodeMap.delNode(firstNode->getName());
        }

        myNodeMap.clear(true);

    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("DeleteNodeTestWithClear XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    if(m_debug.On()) printf("DeleteNodeTestWithClear done");
}

void XMLNodeMapTest::MultipleClearTest(void)
{
    // XmlNodeMap to use for test
    XmlNodeMap myNodeMap;

    // node used for the test
    XmlNode *firstNode = NULL;

    try
    {
        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue"));

        // add duplicates forever
        for(int ii = 0; ii < 1000; ii++)
        {
            // add the first node to the list
            firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
            myNodeMap.addNode(firstNode);
        }

        for(int ii = 0; ii < 10000; ii++)
            myNodeMap.clear(true);
    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("MultipleClearTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    if(m_debug.On()) printf("MultipleClearTest done");
}

void XMLNodeMapTest::NodeMapCopyAndClearTest(void)
{
    // XmlNodeMap to use for test
    XmlNodeMap myNodeMap;
    XmlNodeMap myNewNodeMap;

    // node used for the test
    XmlNode *firstNode = NULL;

    try
    {
        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue"));

        // add duplicates forever
        for(int ii = 0; ii < 1000; ii++)
        {
            // add the first node to the list
            firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);
            myNodeMap.addNode(firstNode);
        }
        // do the copy
        myNewNodeMap = myNodeMap;

        for(int ii = 0; ii < 10000; ii++)
            myNodeMap.clear(true);

        for(int ii = 0; ii < 10000; ii++)
            myNewNodeMap.clear();
    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("MultipleClearTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    if(m_debug.On()) printf("MultipleClearTest done");
}

void XMLNodeMapTest::AddSameObjectTwice(void)
{
    // XmlNodeMap to use for test
    XmlNodeMap myNodeMap;

    // node used for the test
    XmlNode *firstNode = NULL;

    try
    {

        // Memory leak test
        struct mallinfo memInfo1, memInfo2;
        // get the memory at the beginning
        memInfo1 = mallinfo();

        // create duplicate nodes for the list
        XmlString newNode(XML_T("NewNode")), newValue(XML_T("NewValue"));
        // create a node to stick into the map
        firstNode = new XmlNode(newNode, newValue, NULL, ELEMENT_NODE);

        // add duplicates forever
        for(int ii = 0; ii < 3; ii++)
        {
            if(m_debug.On())   printf("The value of the pointer %p\n", firstNode);
            myNodeMap.addNode(firstNode);
        }

        // get the memory at the end
        memInfo2 = mallinfo();
        // check them to be equal
        CPPUNIT_ASSERT_DOUBLES_EQUAL(memInfo1.arena, memInfo2.arena, 100);

        myNodeMap.clear(true);
    }
    catch(XmlException &e)
    {
        if(m_debug.On()) printf("MultipleClearTest XmlException: %s, Line: %d, File: %s\n",
                         e.what(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());
    }

    if(m_debug.On()) printf("MultipleClearTest done");
}



