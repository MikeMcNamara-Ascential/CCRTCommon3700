//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTest.h 1     7/20/05 8:14p Cward $
//
// FILE DESCRIPTION:
//   The test for the ITcpIp class.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTest.h 1     7/20/05 8:14p Cward $
//
//*************************************************************************
#ifndef ITCP_IP_TESTER_H
#define ITCP_IP_TESTER_H
//==============================================================================
// Standard includes
#include <string>
#include <stdlib.h>
//#include <iostream>
//#include <fstream>
//#include <errno.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
#include <cppunit/extensions/HelperMacros.h>

// System includes
//#include "Bepos.h"
//#include "BepException.h"
//#include "XmlParser.h"
//#include "CmdLineProcessor.h"
//#include "ITcpIp.h"
#include "DebugLevel.h"



#define TESTER_PROCESS_NAME		"ITcpIpTest"
#define TESTER_SERVER_ADDRESS	"localhost"
								
								
class ITcpIpTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ITcpIpTest );
    CPPUNIT_TEST( ManualConfigurationTest);
    CPPUNIT_TEST( BadTypeTest);
    CPPUNIT_TEST( BadPortTest);
    CPPUNIT_TEST( BadAddressTest);
    CPPUNIT_TEST( StandardInitializationClientTest);
    CPPUNIT_TEST( StandardInitializationServerTest);
    CPPUNIT_TEST( CustomInitializationClientTest);
    CPPUNIT_TEST( CustomInitializationServerTest);
    CPPUNIT_TEST( XmlInitializationClientTest);
    CPPUNIT_TEST( XmlInitializationServerTest);
    CPPUNIT_TEST( ClientWriteAndReadSmallMessage);
    CPPUNIT_TEST( ClientWriteAndReadLargeMessage);
    /* put the rest of the tests here */
    CPPUNIT_TEST_SUITE_END();

//CPPUNIT_TEST( ServerReadAndWriteMessages);
//CPPUNIT_TEST( SendMessageBadStartFrame);
//CPPUNIT_TEST( SendMessageBadEndFrame);
//CPPUNIT_TEST( InitializeClientNoServer);
//CPPUNIT_TEST( InitializeWithOpenSocket);
//CPPUNIT_TEST( InitializeWithOpenSocketNoConfig);
//CPPUNIT_TEST( InitializeWithBadOpenSocket);
//CPPUNIT_TEST( SetBacklog);

public:
	/**
	 * Class constructor
	 *
	 * @since Version 1.0
	 */
	ITcpIpTest(void);
	
    /**
	 * Class constructor
	 *
	 * @since Version 1.0
	 */
	virtual ~ITcpIpTest(void);
	
	/**
	 * Setup is run before each test, use it to
	 * set up the member ivars for use during
	 * the test
	 */
	void setUp();
	
	/**
	 * TearDown is called after each test, use it
	 * if you need to clean things up after your
	 * test
	 */
	void tearDown();

/*********************************************************
	ITcpIp Tests
 *********************************************************/	
	
	/**
	 * This test will exercise the methods for manually configuring
	 * the component.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void ManualConfigurationTest(void);
	
	/**
	 * This test will attempt to set a bad type and initialize
	 * a TCP/IP object.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void BadTypeTest(void);
	
	/**
	 * This test will attempt to set a bad port and initialize
	 * a client TCP/IP object.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void BadPortTest(void);
	
	/**
	 * This test will attempt to set a bad address and initialize
	 * a client TCP/IP object.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void BadAddressTest(void);
	
	/**
	 * This test will exercise the components standard initialization
	 * method for a client object.  It will set up a TCP/IP
	 * interface component and then verify that it was able
	 * to initialize using the standard initialization method
	 * when properly configured as a client.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void StandardInitializationClientTest(void);
	
	/**
	 * This test will exercise the components standard initialization
	 * method for a server object.  It will set up a TCP/IP
	 * interface component and then verify that it was able
	 * to initialize using the standard initialization method
	 * when properly configured as a server.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void StandardInitializationServerTest(void);
	
	/**
	 * This test will exercise the components custom initialization
	 * method for a client object.  Each of the configuration
	 * parameters will be passed in the initialization
	 * method and the component will be checked for successful
	 * client setup.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void CustomInitializationClientTest(void);
	
	/**
	 * This test will exercise the component custom initialization
	 * method for a server object.  Each of the configuration
	 * parameters will be passed in the initialization
	 * method and the component will be checked for successful
	 * server setup.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void CustomInitializationServerTest(void);
	
	/**
	 * This test will exercise the components initialization
	 * method that takes an XML node.  It will pass a client
	 * setup and validate the it sets up the component
	 * correctly.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void XmlInitializationClientTest(void);
	
	/**
	 * This test will exercise the components initialization
	 * method that takes an XML node.  It will pass a server
	 * setup and validate the it sets up the component
	 * correctly.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void XmlInitializationServerTest(void);
	
	/**
	 * This tests the instantiation of a client communication
	 * object and the writing of small messages using the TCP/IP.
	 * It will connect to a specified server that will echo
	 * back all of the commands sent.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void ClientWriteAndReadSmallMessage(void);
	
	/**
	 * This tests the instantiation of a client communication
	 * object and the writing of large messages using the TCP/IP.
	 * It will connect to a specified server that will echo
	 * back all of the commands sent.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void ClientWriteAndReadLargeMessage(void);
	
	/**
	 * This tests the instantiation of a server communication
	 * object and the reading and writing of messages using
	 * TCP/IP.  It will launch a client that will connect and
	 * attempt to write and read messages.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void ServerReadAndWriteMessages(void);
	
	/**
	 * This tests the write method to ensure that a message
	 * with a bad start tag will not be sent.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void SendMessageBadStartFrame(void);
	
	/**
	 * This tests the write method to ensure that a message
	 * with a bad end tag will not be sent.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void SendMessageBadEndFrame(void);
	
	/**
	 * This tests the instantiation of a client object when
	 * the requested server does not exist.  It should throw
	 * and exception that specifies the problem.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void InitializeClientNoServer(void);

	/**
	 * This test initializes a TCP/IP communication object
	 * with an already opened socket.  The test verifies
	 * the ability of the object to read and write to the
	 * opened socket.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void InitializeWithOpenSocket(void);
	
	/**
	 * This test attempts to initialize a component with
	 * a valid socket but with a bad configuration file.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void InitializeWithOpenSocketNoConfig(void);
	
	/**
	 * This test passes a bad socket descriptor to the object
	 * and verifies that it will identify it and not attempt
	 * to function.
	 *
	 * @param configFile The name of the configuration file.
	 */
	void InitializeWithBadOpenSocket(void);
	/**
	 * Tests setting up the backlog number of incomming test requests
	 *
	 * @param configFile Configuration file used to set up test
	 * @since Version 1.0
	 */
	void SetBacklog(void);

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
     * Debug variable that determines whether additional debug
     * information should be displayed.
     */
    DebugLevel m_debug;

};

#endif // ITCP_IP_H


