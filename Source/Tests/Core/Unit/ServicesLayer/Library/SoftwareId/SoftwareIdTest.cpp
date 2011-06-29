//*************************************************************************
// FILE:
// 	Header: SoftwareIdTest.cpp
//
// FILE DESCRIPTION:
//  The Test Class For SoftwareId.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SoftwareId/SoftwareIdTest.cpp $
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

#include <string>
#include "SoftwareIdTest.h"

#include "SoftwareId.h"



SoftwareIdTest::SoftwareIdTest(void)
{
}

SoftwareIdTest::~SoftwareIdTest(void)
{
}

void SoftwareIdTest::setUp()
{
    // Set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_SOFTWAREID");

    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/Core/Unit/ServicesLayer/Library/SoftwareId/ConfigFiles/";
    if(m_debug.On())
        printf("The new configuration directory: %s\n", m_configDir.c_str());


    // Update the FTP_ROOT environment path
    m_ftpRoot = getenv("FTP_ROOT");
    setenv("FTP_ROOT", topTestDir.c_str(), 1);
    // Update the USR_ROOT environment path
    m_usrRoot = getenv("USR_ROOT");
    setenv("USR_ROOT", topTestDir.c_str(), 1);

    if(m_debug.On())
        printf("\nUSR_ROOT was: %s\n          is: %s\n", m_usrRoot.c_str(),topTestDir.c_str());

}

void SoftwareIdTest::tearDown()
{
    if(m_debug.On())
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void SoftwareIdTest::ConstructionTest(void)
{
    SoftwareId *sftw1 = NULL;

	std::string	idString = "";

	if(m_debug.On()) printf("Starting ConstructionTest\n");


    // create using default constructor
    sftw1 = new SoftwareId();	// instantiate a SoftwareId object

	idString += "CcrtR";
	idString += sftw1->GetPartialId("CoreBuild") + "-";
	idString += sftw1->GetPartialId("CustID") + "-";
	idString += sftw1->GetPartialId("CustBuild");


    CPPUNIT_ASSERT_MESSAGE("Default ID incorrect", (sftw1->GetId() == idString));

	if(m_debug.On()) printf("\nSoftware ID <%s>, expected <%s>\n", sftw1->GetId().c_str(), idString.c_str());

	delete sftw1;

	sftw1 = new SoftwareId("TEST", "09122101");

	idString = "";

	idString += "CcrtR";
	idString += sftw1->GetPartialId("CoreBuild") + "-";
	idString += "TEST-09122101";


    CPPUNIT_ASSERT_MESSAGE("Wrong Customer info ID ", (sftw1->GetId() == idString));

	if(m_debug.On()) printf("\nSoftware ID <%s>, expected <%s>\n",sftw1->GetId().c_str(), idString.c_str());

    delete sftw1;

    if(m_debug.On()) printf("ConstructionTest done\n");

}



