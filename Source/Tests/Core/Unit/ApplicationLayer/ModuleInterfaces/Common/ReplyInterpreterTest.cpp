//******************************************************************************
// Description:
//   Test class for testing the ReplyInterpreter class.
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/ReplyInterpreterTest.cpp $
// 
// 1     9/22/06 11:26a Rwiersem
// Changes for the 06092201 core release:
//     - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include <malloc.h>
#include "ReplyInterpreter.h"
#include "ReplyInterpreterTest.h"
#include "XmlParser.h"

//=============================================================================
ReplyInterpreterTest::ReplyInterpreterTest(void)
{
}

//=============================================================================
ReplyInterpreterTest::~ReplyInterpreterTest(void)
{
}

//=============================================================================
void ReplyInterpreterTest::setUp()
{
    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to top testing";
        cerr << "dir, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configPath = topTestDir +
        "/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/Configuration/";
}

//=============================================================================
void ReplyInterpreterTest::tearDown()
{
}

//=============================================================================
void ReplyInterpreterTest::ConstructorTest(void)
{
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    ReplyInterpreter ri(document);

    CPPUNIT_ASSERT_EQUAL(string("ReplyInterpretations"),
                         ri.m_rootNodePtr->getName());
}

//=============================================================================
void ReplyInterpreterTest::ConstructorTest_NodeIsNull(void)
{
    try
    {
        ReplyInterpreter ri(NULL);
        CPPUNIT_FAIL("ConstructorTest_NodeIsNull() should have thrown"
                     "an XmlException");
    }
    catch(XmlException& ex)
    {
        // This will catch the XmlException that should be thrown above
    }
}

//=============================================================================
void ReplyInterpreterTest::ConstructorTest_NodeHasNoChildren(void)
{
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfigNoChildren.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    try
    {
        ReplyInterpreter ri(document);
        CPPUNIT_FAIL("ConstructorTest_NodeHasNoChildren() should have thrown"
                     "an XmlException");
    }
    catch(XmlException& ex)
    {
        // This will catch the XmlException that should be thrown above
    }
}

//=============================================================================
void ReplyInterpreterTest::IsElementCharsArray_True(void)
{
    ReplyInterpreter ri;
    bool status = false;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = document->getChild("ReplyInterpretations/CHARSARRAY");

    status = ri.isElementCharsArray(node);

    CPPUNIT_ASSERT_EQUAL(true,status);
}

//=============================================================================
void ReplyInterpreterTest::IsElementCharsArray_False(void)
{
    ReplyInterpreter ri;
    bool status = true;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = document->getChild("ReplyInterpretations/CHARS");

    status = ri.isElementCharsArray(node);

    CPPUNIT_ASSERT_EQUAL(false,status);
}

//=============================================================================
void ReplyInterpreterTest::IsNodeCompare_True(void)
{
    ReplyInterpreter ri;
    bool status = false;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = 
        document->getChild("ReplyInterpretations/CHARSARRAY/COMPARE");

    status = ri.isNodeCompare(node);

    CPPUNIT_ASSERT_EQUAL(true,status);
}

//=============================================================================
void ReplyInterpreterTest::IsNodeCompare_False(void)
{
    ReplyInterpreter ri;
    bool status = true;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = document->getChild("ReplyInterpretations");

    status = ri.isNodeCompare(node);

    CPPUNIT_ASSERT_EQUAL(false,status);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_NodeIsNull(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(NULL,byteArray),
                         XmlException);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_NotCharsArrayNode(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(document,byteArray),
                         XmlException);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_UnknownChild(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = 
        document->getChild("ReplyInterpretations/UnknownChildNode/CHARSARRAY");

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(node,byteArray),
                         XmlException);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_LengthMismatch(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *node = 
        document->getChild("ReplyInterpretations/LengthMismatch/CHARSARRAY");

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(node,byteArray),
                         XmlException);
}

//=============================================================================
void 
ReplyInterpreterTest::StrProcessGrammarCharsArray_UnknownCompareNode(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("UnknownCompareNode/CHARSARRAY");

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(node,byteArray),
                         XmlException);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_EqualsFalse(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("EqualsFalse/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_EqualsTrue(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x31,0x32};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("EqualsTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("23"),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_LessThanTrue(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x30};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("LessThanTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("3"),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_LessThanEqualTrue(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x31,0x31};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("LessThanEqualTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("23"),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_LessThanFalse(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x33};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("LessThanFalse/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_GreaterThanTrue(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x33};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("GreaterThanTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("3"),result);
}

//=============================================================================
void 
ReplyInterpreterTest::StrProcessGrammarCharsArray_GreaterThanEqualTrue(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x33,0x31};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = 
        replyNode->getChild("GreaterThanEqualTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("23"),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_GreaterThanFalse(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x30};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("GreaterThanFalse/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_NotEqualFalse(void)
{
    ReplyInterpreter ri;
    uint8_t temp[] = {0x31};
    SerialString_t byteArray = SerialString_t(temp,sizeof(temp));
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("NotEqualFalse/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string(""),result);
}

//=============================================================================
void ReplyInterpreterTest::StrProcessGrammarCharsArray_NotEqualTrue(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = replyNode->getChild("NotEqualTrue/CHARSARRAY");
    string result;

    result = ri.strProcessGrammarCharsArray(node,byteArray);

    CPPUNIT_ASSERT_EQUAL(string("23"),result);
}

//=============================================================================
void ReplyInterpreterTest::
StrProcessGrammarCharsArray_UnknownComparisonOperator(void)
{
    ReplyInterpreter ri;
    SerialString_t byteArray;
    XmlParser parser;
    string configFile = m_configPath + "ReplyInterpreterConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    const XmlNode *replyNode = document->getChild("ReplyInterpretations");
    const XmlNode *node = 
        replyNode->getChild("UnknownComparisonOperator/CHARSARRAY");

    CPPUNIT_ASSERT_THROW(ri.strProcessGrammarCharsArray(node,byteArray),
                         XmlException);
}
