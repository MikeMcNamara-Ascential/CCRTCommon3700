//******************************************************************************
// Description:
//    Host interface test class.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/HostInterface/HostInterfaceTest.h $
// 
// 1     5/01/07 1:46p Rwiersem
// Changes for the 07050201 core release:
// 
// Intial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef HostInterfaceTest_h
#define HostInterfaceTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class HostInterface;

class HostInterfaceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(HostInterfaceTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigItems);
    CPPUNIT_TEST(TestStoreMachineNumber);
    CPPUNIT_TEST(TestProcessMapNode_Detail);
    CPPUNIT_TEST(TestProcessMapNode_VehicleBuild);
    CPPUNIT_TEST(TestProcessMapNode_VehicleBuildWithLength);
    CPPUNIT_TEST(TestProcessMapNode_ValueString);
    CPPUNIT_TEST(TestProcessMapNode_ValueInt);
    CPPUNIT_TEST(TestProcessMapNode_ValueIntWithLength);
    CPPUNIT_TEST(TestProcessMapNode_ValueFloat);
    CPPUNIT_TEST(TestProcessMapNode_DateTime);
    CPPUNIT_TEST(TestProcessMapNode_DateTimeWithLength);
    CPPUNIT_TEST(TestGetTestDetailValue_String);
    CPPUNIT_TEST(TestGetTestDetailValue_Int);
    CPPUNIT_TEST(TestGetTestDetailValue_Float);
    CPPUNIT_TEST(TestGetTestDetailValue_NoLength);
    CPPUNIT_TEST(TestApplyDataFormatting_String);
    CPPUNIT_TEST(TestApplyDataFormatting_Int);
    CPPUNIT_TEST(TestApplyDataFormatting_Float);
    CPPUNIT_TEST(TestApplyDataFormatting_UnknownType);
    CPPUNIT_TEST(TestApplyDataFormatting_NoLength);
    CPPUNIT_TEST(TestAdjustDataValueLength_StringWithPadding);
    CPPUNIT_TEST(TestAdjustDataValueLength_StringWithTrim);
    CPPUNIT_TEST(TestAdjustDataValueLength_IntWithTrim);
    CPPUNIT_TEST(TestAdjustDataValueLength_IntWithPadding);
    CPPUNIT_TEST(TestAdjustDataValueLength_FloatWithValueTrim);
    CPPUNIT_TEST(TestAdjustDataValueLength_FloatWithPrecisionTrim);
    CPPUNIT_TEST(TestAdjustDataValueLength_FloatWithPadding);
    CPPUNIT_TEST(TestAdjustDataValueLength_UnknownType);
    CPPUNIT_TEST(TestGetFormatAttribute);
    CPPUNIT_TEST(TestGetFormatAttribute_NoFormat);
    CPPUNIT_TEST(TestGetDataTypeAttribute);
    CPPUNIT_TEST(TestGetDataTypeAttribute_NoDataType);
    CPPUNIT_TEST(TestGetDataLengthAttribute);
    CPPUNIT_TEST(TestGetDataLengthAttribute_NoLength);
    CPPUNIT_TEST(TestGetTestResult_NoFormat);
    CPPUNIT_TEST(TestGetTestResult_WithFormat);
    CPPUNIT_TEST(TestGetTestResult_MissingResult);
    CPPUNIT_TEST_SUITE_END();

public:
    HostInterfaceTest();
    ~HostInterfaceTest();

    void setUp();
    void tearDown();
    /**
     * Verify the interface gets created properly.
     */
    void TestConstructor(void);
    /**
     * Verify initialization runs correctly.
     */
    void TestLoadAdditionalConfigItems(void);
    /**
     * Verify the stored machine number is correct.
     */
    void TestStoreMachineNumber(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a detail type.
     */
    void TestProcessMapNode_Detail(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a vehicle build type.
     */
    void TestProcessMapNode_VehicleBuild(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a vehicle build type with a length specified in the attributes.
     */
    void TestProcessMapNode_VehicleBuildWithLength(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a value type with a string result.
     */
    void TestProcessMapNode_ValueString(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a value type with a integer result.
     */
    void TestProcessMapNode_ValueInt(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a value type with a integer result that has the length specified in the
     * attributes.
     */
    void TestProcessMapNode_ValueIntWithLength(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes
     * a value type with a float result.
     */
    void TestProcessMapNode_ValueFloat(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes a
     * date and time type.
     */ 
    void TestProcessMapNode_DateTime(void);
    /**
     * Test that will verify the ProcessMapNode() method properly processes a
     * date and time type with a length specified.
     */ 
    void TestProcessMapNode_DateTimeWithLength(void);
    /**
     * Test that will verify the GetTestDetailValue() method properly processes
     * a string data type.
     */
    void TestGetTestDetailValue_String(void);
    /**
     * Test that will verify the GetTestDetailValue() method properly processes
     * an integer data type.
     */
    void TestGetTestDetailValue_Int(void);
    /**
     * Test that will verify the GetTestDetailValue() method properly processes
     * a float data type.
     */
    void TestGetTestDetailValue_Float(void);
    /**
     * Test that will verify the GetTestDetailValue() method properly handles
     * a formatting string with no length specified.
     */
    void TestGetTestDetailValue_NoLength(void);
    /**
     * Test that will verify the ApplyDataFormatting() method properly handles
     * a string data type.
     */
    void TestApplyDataFormatting_String(void);
    /**
     * Test that will verify the ApplyDataFormatting() method properly handles
     * an integer data type.
     */
    void TestApplyDataFormatting_Int(void);
    /**
     * Test that will verify the ApplyDataFormatting() method properly handles
     * a float data type.
     */
    void TestApplyDataFormatting_Float(void);
    /**
     * Test that will verify the ApplyDataFormatting() method properly handles
     * an unknown data type.
     */
    void TestApplyDataFormatting_UnknownType(void);
    /**
     * Test that will verify the ApplyDataFormatting() method properly handles
     * a formatting string with no length specified.
     */
    void TestApplyDataFormatting_NoLength(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * a string data type that needs padding.
     */
    void TestAdjustDataValueLength_StringWithPadding(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * a string data type that needs trimming.
     */
    void TestAdjustDataValueLength_StringWithTrim(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * an integer data type that needs trimming.
     */
    void TestAdjustDataValueLength_IntWithTrim(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * an integer data type that needs padding.
     */
    void TestAdjustDataValueLength_IntWithPadding(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * a float data type that exceeds the maximum field value and needs to be
     * trimmed.
     */
    void TestAdjustDataValueLength_FloatWithValueTrim(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * a float data type that needs precision trimmed to fit in the specified
     * field length.
     */
    void TestAdjustDataValueLength_FloatWithPrecisionTrim(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * a float data type that needs padding.
     */
    void TestAdjustDataValueLength_FloatWithPadding(void);
    /**
     * Test that will verify the AdjustDataValueLength() method properly handles
     * an unknown data type.
     */
    void TestAdjustDataValueLength_UnknownType(void);
    /**
     * Test that will verify the GetFormatAttribute() method properly returns
     * the format specified in the node attributes.
     */
    void TestGetFormatAttribute(void);
    /**
     * Test that will verify the GetFormatAttribute() method properly returns
     * the default if the format is not specified in the node attributes.
     */
    void TestGetFormatAttribute_NoFormat(void);
    /**
     * Test that will verify the GetDataTypeAttribute() method properly returns
     * the data type specified in the node attributes.
     */
    void TestGetDataTypeAttribute(void);
    /**
     * Test that will verify the GetDataTypeAttribute() method properly returns
     * the default if the data type is not specified in the node attributes.
     */
    void TestGetDataTypeAttribute_NoDataType(void);
    /**
     * Test that will verify the GetDataLengthAttribute() method properly
     * returns the data length specified in the node attributes.
     */
    void TestGetDataLengthAttribute(void);
    /**
     * Test that will verify the GetDataLengthAttribute() method properly
     * returns the default if the data length is not specified in the node
     * attributes.
     */
    void TestGetDataLengthAttribute_NoLength(void);
    /**
     * Test that will verify GetTestResult() properly handles the case where
     * no formatting is specified.
     */ 
    void TestGetTestResult_NoFormat(void);
    /**
     * Test that will verify GetTestResult() properly handles the case where
     * formatting is specified.
     */ 
    void TestGetTestResult_WithFormat(void);
    /**
     * Test that will verify GetTestResult() proplerly handles the case where
     * the test result is missing.
     */ 
    void TestGetTestResult_MissingResult(void);
    /**
     * Host interface object to be tested.
     */
    HostInterface *m_hostInterface;
    /**
     * Path to the config file to be used.
     */
    string m_configPath;
    /** 
     * Task id for the mock named data broker 
     */
    int m_ndbId;
    /** 
     * Used to store the original USR_ROOT environment variable 
     */
    string m_originalUsrRoot;
    /** 
     * Used to store the new USR_ROOT environment variable 
     */
    string m_newUsrRoot;
};

#endif
