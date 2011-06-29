//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/Tools/ConfigChecker/ConfigCheckerTest.h 1     7/20/05 8:13p Cward $
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/Tools/ConfigChecker/ConfigCheckerTest.h $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     1/06/05 3:56p Cward
// 
//
//*************************************************************************
#ifndef CONFIGCHECKERTEST_H
#define CONFIGCHECKERTEST_H
//==============================================================================


#include <cppunit/extensions/HelperMacros.h>
#include <string>

// add includes for the tested class

using namespace std;
                         
class ConfigCheckerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ConfigCheckerTest );
	CPPUNIT_TEST( FirstTest );                
    CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Class constructor
	 *
	 * @since Version 1.0
	 */
	ConfigCheckerTest();
	/**
	 * Class destructor
	 *
	 * @since Version 1.0
	 */
    virtual ~ConfigCheckerTest();
	
	/**
	 * Do any pre-testing stuff.
	 */
	void setUp();
	
	/**
	 * Do any post-testing stuff.
	 */
	void tearDown();
	
/*********************************************************
	ConfigChecker Tests
 *********************************************************/	
	
    /**
     * This test 
     * @since Version 1.0
     */
	void FirstTest(void);
	
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
	 * The config checker that will be under test.
	 * @since 1.0
	 */
	ConfigChecker *m_configChecker;	

};

#endif // CONFIGCHECKERTEST_H


