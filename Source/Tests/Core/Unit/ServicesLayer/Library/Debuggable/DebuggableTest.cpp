//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/Debuggable/DebuggableTest.cpp 2     3/02/06 12:45a Cward $
//
// FILE DESCRIPTION:
//  The Test Class For Debuggable.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/Debuggable/DebuggableTest.cpp $
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
//*************************************************************************
#include <unistd.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include "DebuggableTest.h"

#include "Debuggable.h"


////////////////////////////////////////////////////////////////////////////////
// derived class to test protected functions
class Dbggbl : public Debuggable
{
public:
	/**
	 * Class constructor
	 */
	Dbggbl()
    {
    }
	/**
	 * Class destructor
	 */
	virtual ~Dbggbl()
    {
    }

    /**
     *
     */
    void ExtSetDebug(const std::string &strval)
    {
        SetDebug( strval );
    }

    /**
     *
     */
    void ExtSetDebug(const char *chval)
    {
        SetDebug( chval );
    }

    /**
     *
     */
    void ExtSetDebug(const bool bval)
    {
        SetDebug( bval );
    }

    /**
     *
     */
    void ExtSetDebugLevel(const char *chval)
    {
        SetDebugLevel( chval );
    }

    /**
     *
     */
    void ExtSetDebugLevel(const int value)
    {
        SetDebugLevel( value );
    }

    /**
     *
     */
    void ExtSetDebugLevel(const std::string &strval)
    {
        SetDebugLevel( strval );
    }

private:
};

///////////////////////////////////////////////////////////////////////////////////



DebuggableTest::DebuggableTest(void)
{
}

DebuggableTest::~DebuggableTest(void)
{
}

void DebuggableTest::setUp()
{
    // Set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_DEBUGGABLE");

    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/Core/Unit/ServicesLayer/Library/Debuggable/ConfigFiles/";
    if(m_debug.On())
        printf("The new configuration directory: %s\n", m_configDir.c_str());


    // Update the FTP_ROOT environment path
    m_ftpRoot = getenv("FTP_ROOT");
    setenv("FTP_ROOT", topTestDir.c_str(), 1);
    // Update the USR_ROOT environment path
    m_usrRoot = getenv("USR_ROOT");
    setenv("USR_ROOT", topTestDir.c_str(), 1);
}

void DebuggableTest::tearDown()
{
    if(m_debug.On())
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void DebuggableTest::DefaultConstructorTest(void)
{
    Debuggable *dbg = NULL;

    if(m_debug.On()) printf("Starting DefaultConstructorTest\n");

    // create using default constructor
    dbg = new Debuggable();	// instantiate a default debuggable object
    // default debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("Default debug is true", (dbg->IsDebugOn() == false));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Default debug level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));

    delete dbg;

    if(m_debug.On()) printf("DefaultConstructorTest done\n");

}

void DebuggableTest::BoolContructorTest(void)
{
    Debuggable *dbg = NULL;

    if(m_debug.On()) printf("Starting BoolConstructorTest\n");

    dbg = new Debuggable(true);	// instantiate a bool initialized debuggable object
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("Initiialized debug is false", (dbg->IsDebugOn() == true));
    // debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));

    delete dbg;

    dbg = new Debuggable(false);	// instantiate a bool initialized debuggable object
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("Debug is true", (dbg->IsDebugOn() == false));
    // debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));

    delete dbg;

    if(m_debug.On()) printf("BoolConstructorTest done\n");

}

void DebuggableTest::IntContructorTest(void)
{
    Debuggable *dbg = NULL;

    if(m_debug.On()) printf("Starting IntConstructorTest\n");

    dbg = new Debuggable(0);	// instantiate an int initialized debuggable object
    // default debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("Debug is false", (dbg->IsDebugOn() == true));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));

    delete dbg;

    dbg = new Debuggable(-24784);	// instantiate an int initialized debuggable object
    // default debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("Debug is true", (dbg->IsDebugOn() == true));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not -24784", ((dbg->IsDebugOn(-24784) && (!(dbg->IsDebugOn(-24783)))) == true));

    delete dbg;

    dbg = new Debuggable(18259);	// instantiate an int initialized debuggable object
    // default debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("Debug is true", (dbg->IsDebugOn() == true));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not 18259", ((dbg->IsDebugOn(18259) && (!(dbg->IsDebugOn(18260)))) == true));

    delete dbg;

    if(m_debug.On()) printf("IntConstructorTest done\n");

}


// using derived class to get at non public functions.

void DebuggableTest::SetFunctionsTest(void)
{
    Dbggbl *dbg = NULL;

    if(m_debug.On()) printf("Starting SetFunctionsTest\n");

    // create using default constructor
    dbg = new Dbggbl();	// instantiate an extended default debuggable object

    // default debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("Debug is true", (dbg->IsDebugOn() == false));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("Debug level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));

    dbg->ExtSetDebug( std::string("On") );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebug(string &) not working for On case", (dbg->IsDebugOn() == true));

    dbg->ExtSetDebug( std::string("Off") );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebug(string &) not working for Off case", (dbg->IsDebugOn() == false));


    dbg->ExtSetDebug( "On" );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebug(char *) not working for On case", (dbg->IsDebugOn() == true));

    dbg->ExtSetDebug( "Off" );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebug(char *) not working for Off case", (dbg->IsDebugOn() == false));


    dbg->ExtSetDebug( true );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebug(bool) not working for On case", (dbg->IsDebugOn() == true));

    dbg->ExtSetDebug( false );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebug(bool) not working for Off case", (dbg->IsDebugOn() == false));

// test Set Debug Level Functions

    // int version

    dbg->ExtSetDebugLevel( -5 );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) not working for negative case", (dbg->IsDebugOn() == false));
    // debug level should be -5
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) level is not -5", ((dbg->IsDebugOn(-5) && (!(dbg->IsDebugOn(-4)))) == true));

    dbg->ExtSetDebugLevel( 3 );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) not working for positive case", (dbg->IsDebugOn() == true));
    // debug level should be 3
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) level is not 3", ((dbg->IsDebugOn(3) && (!(dbg->IsDebugOn(4)))) == true));

    dbg->ExtSetDebugLevel( 0 );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) not working for zero case", (dbg->IsDebugOn() == false));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(int) level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));


    // char * version

    dbg->ExtSetDebugLevel( "1250" );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) not working for positive case", (dbg->IsDebugOn() == true));
    // debug level should be 1250
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) level is not 1250", ((dbg->IsDebugOn(1250) && (!(dbg->IsDebugOn(1251)))) == true));

    dbg->ExtSetDebugLevel( "-45" );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) not working for negative case", (dbg->IsDebugOn() == false));
    // debug level should be -45
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) level is not -45", ((dbg->IsDebugOn(-45) && (!(dbg->IsDebugOn(-44)))) == true));

    dbg->ExtSetDebugLevel( "0" );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) not working for zero case", (dbg->IsDebugOn() == false));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));


    // std string version

    std::string strval("");

    strval = "9975";

    dbg->ExtSetDebugLevel( strval );
    // debug flag should be true
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) not working for positive case", (dbg->IsDebugOn() == true));
    // debug level should be 1250
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) level is not 9975", ((dbg->IsDebugOn(9975) && (!(dbg->IsDebugOn(9976)))) == true));

    strval = "-6395";

    dbg->ExtSetDebugLevel( strval );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) not working for negative case", (dbg->IsDebugOn() == false));
    // debug level should be -6395
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) level is not -6395", ((dbg->IsDebugOn(-6395) && (!(dbg->IsDebugOn(-6394)))) == true));

    strval = "0";

    dbg->ExtSetDebugLevel( strval );
    // debug flag should be false
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) not working for zero case", (dbg->IsDebugOn() == false));
    // default debug level should be zero
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) level is not zero", ((dbg->IsDebugOn(0) && (!(dbg->IsDebugOn(1)))) == true));


    delete dbg;

    if(m_debug.On()) printf("SetFunctionsTest done\n");
}


void DebuggableTest::InvalidValuesTest(void)
{
    Dbggbl *dbg = NULL;
    std::string strval("");

    if(m_debug.On()) printf("Starting InvalidValuesTest\n");

// call with null string value

    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(string &) with null string\n");
        dbg->ExtSetDebugLevel( strval );
        if(m_debug.On()) printf("SetDebugLevel(string &) with null string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(string &) not handling null string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with null string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with null string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;


// call with non numeric string value
    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    strval = "Invalid Value";
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(string &) with Invalid string\n");
        dbg->ExtSetDebugLevel( strval );
        if(m_debug.On()) printf("SetDebugLevel(string &) with Invalid string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(string &) not handling non-numeric string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with Invalid string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with Invalid string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;

// call with very large numeric string value
    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    strval = "3890087123456230957809720963896293869825986983243423789123777783475762669900";
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(string &) with very large numeric string\n");
        dbg->ExtSetDebugLevel( strval );
        if(m_debug.On()) printf("SetDebugLevel(string &) with very large numeric string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(string &) not handling very large numeric string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with very large numeric string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(string &) with very large numeric string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;


// call with null string char * value

    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(char *) with null string\n");
        dbg->ExtSetDebugLevel( "" );
        if(m_debug.On()) printf("SetDebugLevel(char *) with null string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(char *) not handling null string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with null string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with null string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;


// call with non numeric string char * value
    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    strval = "Invalid Value";
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(char *) with Invalid string\n");
        dbg->ExtSetDebugLevel( "Another Invalid Value" );
        if(m_debug.On()) printf("SetDebugLevel(char *) with Invalid string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(char *) not handling non-numeric string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with Invalid string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with Invalid string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;

// call with very large numeric string char * value
    dbg = new Dbggbl();	// instantiate an extended default debuggable object
    dbg->ExtSetDebugLevel( 2 ); // set up initial condition
    try
    {
        if(m_debug.On()) printf("Calling SetDebugLevel(char *) with very large numeric string\n");
        dbg->ExtSetDebugLevel( "547726357657652987295837430923907970003" );
        if(m_debug.On()) printf("SetDebugLevel(char *) with very large numeric string returned\n");
    }
    catch(exception &e)
    {
        CPPUNIT_FAIL(std::string("SetDebugLevel(char *) not handling very large numeric string (exception=" + std::string(e.what()) + ")"));
    }
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with very large numeric string changes debug val", (dbg->IsDebugOn() == true));
    // debug level should be 2
    CPPUNIT_ASSERT_MESSAGE("SetDebugLevel(char *) with very large numeric string level is not 2", ((dbg->IsDebugOn(2) && (!(dbg->IsDebugOn(3)))) == true));
    delete dbg;


    if(m_debug.On()) printf("InvalidValuesTest done\n");
}
