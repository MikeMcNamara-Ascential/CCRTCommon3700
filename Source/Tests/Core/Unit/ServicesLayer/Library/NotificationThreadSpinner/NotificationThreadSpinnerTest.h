//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTest.h 2     2/15/06 8:08p Cward $
//
// FILE DESCRIPTION:
//   This is the test that will excersize the NotificationThreadSpinner
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/NotificationThreadSpinner/NotificationThreadSpinnerTest.h $
// 
// 2     2/15/06 8:08p Cward
// Simplified Unit Test.
//
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 1     5/31/05 8:44a Gswope
// Initial entry into source safe
//
// 1     11/11/03 3:04p Khykin
// Initial checkin.
//
//*************************************************************************
#ifndef NOTIFICATIONTHREADSPINNERTEST_H
#define NOTIFICATIONTHREADSPINNERTEST_H
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "INotification.h"
#include "BogusCommType.h"
#include "BogusNotificationType.h"


using namespace std;

class NotificationThreadSpinnerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( NotificationThreadSpinnerTest );
    CPPUNIT_TEST( ClearLog );
    CPPUNIT_TEST( NotificationDataClassTest );
    CPPUNIT_TEST( ThreadTester );
    CPPUNIT_TEST( ThreadSpinnerTest );
    CPPUNIT_TEST( ThreadSpinnerTest2 );
    CPPUNIT_TEST( ThreadSpinnerTest3 );
    // the next line can be added if desired for debugging
//    CPPUNIT_TEST( LogDumper );

    CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Class constructor
	 *
	 * @since Version 1.0
	 */
	NotificationThreadSpinnerTest();
	/**
	 * Class destructor
	 *
	 * @since Version 1.0
	 */
    virtual ~NotificationThreadSpinnerTest();

	/**
	 * Do any pre-testing stuff.
	 */
	void setUp();

	/**
	 * Do any post-testing stuff.
	 */
	void tearDown();

/*********************************************************
	ThreadSpinner Tests
 *********************************************************/

	void NotificationDataClassTest(void);
    void ClearLog(void);
    void ThreadTester(void);
    void ThreadSpinnerTest(void);
    void ThreadSpinnerTest2(void);
    void ThreadSpinnerTest3(void);
    void LogDumper(void);

private:

    /**
     * The directory to find the configuration files in.
     */
//    std::string m_configDir;

    /**
     * Variable that holds the FTP_ROOT value prior to testing so
     * it can be restored.
     * @since Version 1.0
     */
//    std::string m_ftpRoot;

    /**
     * Variable that holds the USR_ROOT value prior to testing so
     * it can be restored.
     * @since Version 1.0
     */
//    std::string m_usrRoot;

	/**
	 * The test selector that will be under test.
	 * @since 1.0
	 */
//	INotification *m_ThreadSpinner;

    BogusCommType m_BCT;

};

#endif // NOTIFICATIONTHREADSPINNERTEST_H


