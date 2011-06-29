//******************************************************************************
// Description:
//  The Test Class For Testing the ReplyInterpreter Class.
//
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/ReplyInterpreterTest.h $
// 
// 1     9/22/06 11:26a Rwiersem
// Changes for the 06092201 core release:
//     - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ReplyInterpreterTest_h
#define ReplyInterpreterTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class ReplyInterpreterTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ReplyInterpreterTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(ConstructorTest_NodeIsNull);
    CPPUNIT_TEST(ConstructorTest_NodeHasNoChildren);
    CPPUNIT_TEST(IsElementCharsArray_True);
    CPPUNIT_TEST(IsElementCharsArray_False);
    CPPUNIT_TEST(IsNodeCompare_True);
    CPPUNIT_TEST(IsNodeCompare_False);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_NodeIsNull);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_NotCharsArrayNode);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_UnknownChild);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_LengthMismatch);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_UnknownCompareNode);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_EqualsFalse);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_EqualsTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_LessThanTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_LessThanEqualTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_LessThanFalse);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_GreaterThanTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_GreaterThanEqualTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_GreaterThanFalse);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_NotEqualFalse);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_NotEqualTrue);
    CPPUNIT_TEST(StrProcessGrammarCharsArray_UnknownComparisonOperator);
    CPPUNIT_TEST_SUITE_END();

public:
    ReplyInterpreterTest();
    virtual ~ReplyInterpreterTest();
    /**
     * Setup is run before each test, use it to set up the member ivars for use 
     * during the test
     */
    void setUp();
    /**
     * TearDown is called after each test, use it if you need to clean things up 
     * after your test
     */
    void tearDown();
    /**
     * Test that will verify that the object is correctly initialized on 
     * construction.
     */
    void ConstructorTest(void);
    /**
     * Test that will verify the constructor properly handles the case where
     * the passed in XmlNode is NULL.
     */ 
    void ConstructorTest_NodeIsNull(void);
    /**
     * Test that will verify the constructor properly handles the case where
     * the passed in XmlNode has no children.
     */ 
    void ConstructorTest_NodeHasNoChildren(void);
    /**
     * Test that will verify the IsElementCharsArray() method returns the 
     * proper value for the true case.
     */ 
    void IsElementCharsArray_True(void);
    /**
     * Test that will verify the IsElementCharsArray() method returns the 
     * proper value for the false case.
     */ 
    void IsElementCharsArray_False(void);
    /**
     * Test that will verify the IsNodeCompare() method returns the proper
     * value for the true case.
     */ 
    void IsNodeCompare_True(void);
    /**
     * Test that will verify the IsNodeCompare() method returns the proper 
     * value for the false case.
     */ 
    void IsNodeCompare_False(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the passed in XmlNode is NULL.
     */ 
    void StrProcessGrammarCharsArray_NodeIsNull(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the passed in XmlNode is not a CHARSARRAY node.
     */
    void StrProcessGrammarCharsArray_NotCharsArrayNode(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where there is an unknown child node name.
     */ 
    void StrProcessGrammarCharsArray_UnknownChild(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method proplery
     * handles the case where the length don't match.
     */ 
    void StrProcessGrammarCharsArray_LengthMismatch(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where there is an unknown <Substitute> child node name.
     */ 
    void StrProcessGrammarCharsArray_UnknownCompareNode(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "equals" and the
     * result is false.
     */ 
    void StrProcessGrammarCharsArray_EqualsFalse(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "equals" and the
     * result is true.
     */ 
    void StrProcessGrammarCharsArray_EqualsTrue(void);
    /**
     * Test that will verify the strProcessGrammerCharsArray() method properly
     * handles the case where the comparision operator is "less than" and the
     * result is true.
     */ 
    void StrProcessGrammarCharsArray_LessThanTrue(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "less than" and one
     * of the databytes is equal to the compare value and the ending result
     * is true.
     */ 
    void StrProcessGrammarCharsArray_LessThanEqualTrue(void);
    /**
     * Test that will verify the strProcessGrammerCharsArray() method properly
     * handles the case where the comparision operator is "less than" and the
     * result is false.
     */
    void StrProcessGrammarCharsArray_LessThanFalse(void);
    /**
     * Test that will verify the strProcessGrammerCharsArray() method properly
     * handles the case where the comparision operator is "greater than" and the
     * result is true.
     */ 
    void StrProcessGrammarCharsArray_GreaterThanTrue(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "greater than" and one
     * of the databytes is equal to the compare value and the ending result
     * is true.
     */ 
    void StrProcessGrammarCharsArray_GreaterThanEqualTrue(void);
    /**
     * Test that will verify the strProcessGrammerCharsArray() method properly
     * handles the case where the comparision operator is "greater than" and the
     * result is false.
     */
    void StrProcessGrammarCharsArray_GreaterThanFalse(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "not equal" and the
     * result is false.
     */ 
    void StrProcessGrammarCharsArray_NotEqualFalse(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparision operator is "not equal" and the
     * result is true.
     */ 
    void StrProcessGrammarCharsArray_NotEqualTrue(void);
    /**
     * Test that will verify the strProcessGrammarCharsArray() method properly
     * handles the case where the comparison operator in unknown.
     */ 
    void StrProcessGrammarCharsArray_UnknownComparisonOperator(void);


private:
    /** The path to the configuration files. */
    std::string m_configPath;
};
#endif //ReplyInterpreterTest_h
