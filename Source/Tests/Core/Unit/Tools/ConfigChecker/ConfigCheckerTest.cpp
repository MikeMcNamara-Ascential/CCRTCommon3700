//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/Tools/ConfigChecker/ConfigCheckerTest.cpp 1     7/20/05 8:13p Cward $
//
// FILE DESCRIPTION:
//   This is the test that will excersize the config checker
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/Tools/ConfigChecker/ConfigCheckerTest.cpp $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:56p Cward
// 
// 1     11/11/03 3:05p Khykin
// Initial checkin.
//
//*************************************************************************
#include "ConfigCheckerTest.h"

ConfigCheckerTest::ConfigCheckerTest() : m_configChecker(NULL)
{
}

ConfigCheckerTest::~ConfigCheckerTest()
{
    if(DEBUG) 
        printf("ConfigCheckerTest::~ConfigCheckerTest done\n");

}

void ConfigCheckerTest::setUp()
{
    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/System/ApplicationLayer/ConfigCheckers/ConfigCheckerTest/Configuration/";
    if(DEBUG) 
        printf("The new configuration directory: %s\n", m_configDir.c_str());


    // Update the FTP_ROOT environment path
    m_ftpRoot = getenv("FTP_ROOT");
    setenv("FTP_ROOT", topTestDir.c_str(), 1);
    // Update the USR_ROOT environment path
    m_usrRoot = getenv("USR_ROOT");
    setenv("USR_ROOT", topTestDir.c_str(), 1);

    // create a new foreground component proxy for testing
    if(m_configChecker != NULL)
        delete m_configChecker;
    m_configChecker = new ConfigChecker();
}

void ConfigCheckerTest::tearDown()
{
    if(DEBUG) 
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);

    // delete the foreground component proxy
    if(m_configChecker != NULL)
    {
        delete m_configChecker;
        m_configChecker = NULL;
    }
    
}

void ConfigCheckerTest::FirstTest()
{
    XmlParser parser;               // parser used to parse the configuration file
    const XmlNode *document = NULL; // pointer to the root node of the configuration file

    try
    {
        if(DEBUG) printf("Test: Entering VerifyObjectiveResults()\n");

        // check the default value of the overall result
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testSkip);
        // set the current objective name
        m_configChecker->SetObjectiveName("First");
        // update the result as passed
        m_configChecker->SetOverallResult(testPass);
        // check the current status == passed
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testPass);
        // set a new current objective name
        m_configChecker->SetObjectiveName("Second");
        // update the result as skip
        m_configChecker->SetOverallResult(testSkip);
        // check the current status == passed
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testPass);
        // set the current objective name
        m_configChecker->SetObjectiveName("Third");
        // update the result as passed
        m_configChecker->SetOverallResult(testPass);
        // check the current status == passed
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testPass);
        // set the current objective name
        m_configChecker->SetObjectiveName("Fourth");
        // update the result as failed
        m_configChecker->SetOverallResult(testFail);
        // check the current status == failed
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testFail);
        // update the result as passed
        m_configChecker->SetOverallResult(testPass);
        // check the current status == passed
        CPPUNIT_ASSERT(m_configChecker->GetOverallResult() == testPass);
    }
    catch(XmlException &e)
    {
        printf("VerifyObjectiveResults XmlException: %s, Line: %ld, File: %s\n", 
                         e.Reason().c_str(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());  // report the failure
    }
    catch(BepException &e)
    {
        printf("VerifyObjectiveResults BepException: %s, Line: %ld, File: %s\n", 
                         e.Reason().c_str(), __LINE__, __FILE__);
        CPPUNIT_FAIL(e.what());  // report the failure
    }
    if(DEBUG) printf("\n\nTest: Complete\n");

}


