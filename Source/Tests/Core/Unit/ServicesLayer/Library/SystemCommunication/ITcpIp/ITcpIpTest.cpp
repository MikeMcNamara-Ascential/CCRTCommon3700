//*************************************************************************
// Description:
//   This is a stub for the named data broker to test out the data servers
//   and the data server launcher.
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
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ITcpIpTest.cpp 3     5/24/06 3:02p Cward $
//
//*************************************************************************

#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include "ITcpIpTest.h"

// Standard includes
//#include <string>
//#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
//#include <cppunit/extensions/HelperMacros.h>
#if _NTO_VERSION >= 630
    #include <errno.h>
    #include <arpa/inet.h>
#endif


// System includes
#include "Bepos.h"
#include "XmlParser.h"
#include "ITcpIp.h"


void *ClientStub(void *arg);			// client thread function
void *ServerStub(void *arg);			// server thread function
void *DumbServerStub(void *arg);		// dumb server thread function
const std::string ReadMessage(int sock);	// method to read a message from a TCP/IP socket


typedef struct
{
    int port;                       // the port the server is listening too
    std::string serverAddresss;     // the address of the server
    std::string message;            // the message to send to the server
}ClientInformation;

ITcpIpTest::ITcpIpTest(void)
{
};

ITcpIpTest::~ITcpIpTest(void)
{
};

void ITcpIpTest::setUp()
{
    // set up debug variable based on environment var
    m_debug.ViaEnv("DEBUG_TEST_ITCPIP");

    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/Core/Unit/ServicesLayer/Library/SystemCommunication/ITcpIp/ConfigFiles/";
    if(m_debug.On())
        printf("The new configuration directory: %s\n", m_configDir.c_str());


    // Update the FTP_ROOT environment path
    m_ftpRoot = getenv("FTP_ROOT");
    setenv("FTP_ROOT", topTestDir.c_str(), 1);
    // Update the USR_ROOT environment path
    m_usrRoot = getenv("USR_ROOT");
    setenv("USR_ROOT", topTestDir.c_str(), 1);
}

void ITcpIpTest::tearDown()
{
    if(m_debug.On())
        printf("\n\nSetting the FTP and USR ROOTS\n");
    // Restore the environment variables
    setenv("FTP_ROOT", m_ftpRoot.c_str(), 1);
    setenv("USR_ROOT", m_usrRoot.c_str(), 1);
}

void ITcpIpTest::ManualConfigurationTest(void)
{
    try
	{
		if(m_debug.On()) cout << "Entering ManualConfigurationTest()" << endl;
		ITcpIp comp;	// instantiate a TCP/IP Interface component
		// verify m_debug set/get methods
		// boolean
		comp.SetDebug(true);
        CPPUNIT_ASSERT_MESSAGE("Unable to set debug to true", (comp.IsDebugOn() == true));
		comp.SetDebug(false);
		CPPUNIT_ASSERT_MESSAGE("Unable to set debug to false", (comp.IsDebugOn() == false));
		// string
		comp.SetDebug(std::string("On"));
		CPPUNIT_ASSERT_MESSAGE("Unable to set debug to On with string", (comp.IsDebugOn() == true));
		comp.SetDebug(std::string("Off"));
		CPPUNIT_ASSERT_MESSAGE("Unable to set debug to Off with string", (comp.IsDebugOn() == false));
		// character array
		comp.SetDebug("On");
		CPPUNIT_ASSERT_MESSAGE("Unable to set debug to On with char array", (comp.IsDebugOn() == true));
		comp.SetDebug("Off");
		CPPUNIT_ASSERT_MESSAGE("Unable to set debug to Off with char array", (comp.IsDebugOn() == false));
		// invalid entry
		comp.SetDebug(true);
		comp.SetDebug("Junk");
		CPPUNIT_ASSERT_MESSAGE("Invalid entry handled incorrectly", (comp.IsDebugOn() == false));
		// verify address methods
		comp.SetIPAddress(TESTER_SERVER_ADDRESS);
		const std::string address = comp.GetIPAddress();
		CPPUNIT_ASSERT_MESSAGE("Address not correctly set", (address == TESTER_SERVER_ADDRESS));
		// verify port methods
		comp.SetPort(10000);
		const INT16 port = comp.GetPort();
//		CPPUNIT_ASSERT_MESSAGE("Port not correctly set", (port == 11000));
		CPPUNIT_ASSERT_MESSAGE("Port not correctly set", (port == 10000));
		// verify socket methods
		comp.SetSock(5);
		const INT16 sock =comp.GetSock();
		CPPUNIT_ASSERT_MESSAGE("Socket not correctly set", (sock == 5));
		// verify type methods
		comp.SetType(std::string("Client"));
		const std::string type = comp.GetType();
		CPPUNIT_ASSERT_MESSAGE("Type not correctly set", (type == "Client"));
		// verify the server methods
		struct sockaddr_in server;
		server.sin_family = AF_INET;			// set the domain to the Internet
		server.sin_addr.s_addr = INADDR_ANY;	// set the socket to accept any valid address
		server.sin_port = htons(10000);			// set the port number
		comp.SetServer(server);					// update the object
		// test all of the conditions in the object
		CPPUNIT_ASSERT_MESSAGE("Family not set correctly", (comp.GetServer().sin_family == AF_INET));
		CPPUNIT_ASSERT_MESSAGE("Addres not set correctly to INADDR_ANY", (comp.GetServer().sin_addr.s_addr == INADDR_ANY));
		CPPUNIT_ASSERT_MESSAGE("Port not set correctly", (comp.GetServer().sin_port == htons(10000)));
		// test all of the set and get methods for the start and end of text
	 	comp.SetFrameStart(BEP_XML_ROOT);					// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);					// set the end tag of a message
		CPPUNIT_ASSERT_MESSAGE("Frame Start not set correctly", (comp.GetFrameStart() == BEP_XML_ROOT));
		CPPUNIT_ASSERT_MESSAGE("Frame End not set correctly", (comp.GetFrameEnd() == BEP_XML_ROOT_END));
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "ManualConfigurationInitializationTest Error: " << e.Reason() << endl;
		char buffer[256];
        sprintf(buffer, "ManualConfigurationInitializationTest XmlException: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::BadTypeTest(void)
{
	try
	{
		if(m_debug.On())   printf("Entering BadTypeTest()\n");
        ITcpIp comp;	                                    // instantiate a TCP/IP Interface component
		if(m_debug.On())   comp.SetDebug(true);
        comp.SetIPAddress(std::string(TESTER_SERVER_ADDRESS));
		comp.SetPort(20200);
		comp.SetFrameStart(BEP_XML_ROOT);					// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);					// set the end tag of a message
		comp.Initialize("Wacko");
		if(m_debug.On()) printf("Component Initialized\n");
		// if it makes it here it is a bad thing
        CPPUNIT_FAIL("Invalid Configuration Not Detected");
    }
	catch(XmlException &e)
	{
		char buffer[256];
        sprintf(buffer, "BadTypeTest XmlException: %s", e.what());
		CPPUNIT_FAIL(buffer);
	}
	catch(BepException &e)	// we expect this test to throw a BepException due to improper config
	{
		if(m_debug.On()) cout << "BadTypeTest Error: " << e.Reason() << endl;
	}
};

void ITcpIpTest::BadPortTest(void)
{
	try
	{
		if(m_debug.On())   printf("Entering BadPortTest()\n");
        // instantiate a TCP/IP Interface component
        ITcpIp comp;
        if(m_debug.On())   comp.SetDebug(true);
		// set a know good IP address
		comp.SetIPAddress(std::string(TESTER_SERVER_ADDRESS));
		if(m_debug.On())	puts("IP Set(TESTER_SERVER_ADDRESS)");
		// attempt to set a bad port
		comp.SetPort(100);
		// if we made it here it is bad!
        char buffer[256];
        sprintf(buffer, "BadPortTest Unexpected Condition");
		CPPUNIT_FAIL(buffer);
	}
	catch(XmlException &e)
	{
		char buffer[256];
        sprintf(buffer, "BadPortTest XmlException: %s", e.what());
		CPPUNIT_FAIL(buffer);
	}
	catch(BepException &e)
	{   // we expect this test to throw a BepException due to improper config
        if(m_debug.On())   printf("BadPortTest Expected Error: %s\n", e.Reason().c_str());
		CPPUNIT_ASSERT_MESSAGE("Invalid Error Message", (strstr(e.what(), "Invalid Port Number")  != NULL));
	}
};

void ITcpIpTest::BadAddressTest(void)
{
	try
	{
		if(m_debug.On()) cout << "Entering BadAddressTest()" << endl;
		ITcpIp comp;	// instantiate a TCP/IP Interface component
        if(m_debug.On())   comp.SetDebug(true);
		comp.SetIPAddress(std::string(""));
		comp.SetPort(20210);
		comp.SetFrameStart(BEP_XML_ROOT);
		comp.SetFrameEnd(BEP_XML_ROOT_END);
		comp.Initialize("Client");
		// if we made it here it is bad!
        char buffer[256];
        sprintf(buffer, "BadAddressTest Unexpected Condition");
		CPPUNIT_FAIL(buffer);
	}
	catch(XmlException &e)
	{
		if(m_debug.On())   printf("BadAddressTest Error: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "BadAddressTest XmlException: %s", e.what());
		CPPUNIT_FAIL(buffer);
	}
	catch(BepException &e)	// we expect this test to throw a BepException due to improper config
	{
		if(m_debug.On())   printf("BadAddressTest Error: %s\n", e.what());
		CPPUNIT_ASSERT_MESSAGE("Invalid Error Message", (strstr(e.what(), "Invalid IP Address")  != NULL));
	}
};

void ITcpIpTest::StandardInitializationClientTest(void)
{
	if(m_debug.On()) printf("Entering StandardInitializationClientTest()\n");
	INT16 port = 20000;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;					// create the thread priority
		// start the server stub on port 20000
        int status = pthread_create( NULL, &attr, DumbServerStub, (void *) &port);
        CPPUNIT_ASSERT_MESSAGE("Unable to create server thread", (status == EOK));
		delay(100);																		// give it some time to start

		// setup and initialize a TCP/IP component
		ITcpIp comp;	// instantiate a TCP/IP Interface component
		if(m_debug.On())   comp.SetDebug(true);
        comp.SetIPAddress(std::string(TESTER_SERVER_ADDRESS));	// set the IP address of the machine for the test
		comp.SetPort(port);										// set the port of the server
		comp.SetFrameStart(BEP_XML_ROOT);						// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);						// set the end tag of a message
		// initialize the interface for client operation
		comp.Initialize("Client", -1, "", m_debug.On());
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Client"));                           // verify that it is a client object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));							    // verify it has connected to the correct port
		CPPUNIT_ASSERT_MESSAGE("Invalid IPAddress", (comp.GetIPAddress() == TESTER_SERVER_ADDRESS));	// verify it is connected to the correct server
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));						        // verify that the client was able to open a socket
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame Start", (comp.GetFrameStart() == BEP_XML_ROOT));	        // validate start message
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame End", (comp.GetFrameEnd() == BEP_XML_ROOT_END));          // validate end message
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("StandardInitializationClientTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "StandardInitializationClientTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
	}
};

void ITcpIpTest::StandardInitializationServerTest(void)
{
	if(m_debug.On()) cout << "Entering StandardInitializationServerTest()" << endl;
	INT16 port = 20010;

    try
	{   // setup and initialize a TCP/IP component
		ITcpIp comp;							// instantiate a TCP/IP Interface component
		comp.SetPort(port);						// set the port of the server
		comp.SetFrameStart(BEP_XML_ROOT);		// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);		// set the end tag of a message
		comp.Initialize("Server");				// initialize the interface for client operation
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Server"));   // verify that it is a server object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));		// verify it has connected to the correct port
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));		// verify that the server was able to open a socket
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("StandardInitializationServerTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "StandardInitializationServerTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::CustomInitializationClientTest(void)
{
	if(m_debug.On()) cout << "Entering CustomInitializationClientTest()" << endl;
	INT16 port = 20020;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;				// create the thread priority
		int status = pthread_create( NULL, &attr, DumbServerStub, (void *) &port);
        CPPUNIT_ASSERT_MESSAGE("Unable to create server thread", (status == EOK));
		delay(100);																	// give it some time to start

		// setup and initialize a TCP/IP component
		ITcpIp comp;
		comp.SetFrameStart(BEP_XML_ROOT);						// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);						// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);	// initialize the full component
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Client"));                           // verify that it is a client object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));							    // verify it has connected to the correct port
		CPPUNIT_ASSERT_MESSAGE("Invalid IPAddress", (comp.GetIPAddress() == TESTER_SERVER_ADDRESS));	// verify it is connected to the correct server
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));						        // verify that the client was able to open a socket
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("CustomInitializationClientTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "CustomInitializationClientTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::CustomInitializationServerTest(void)
{
	if(m_debug.On()) cout << "Entering CustomInitializationServerTest()" << endl;
	INT16 port = 20030;

	try
	{	// setup and initialize a TCP/IP component
		ITcpIp comp;
		comp.SetFrameStart(BEP_XML_ROOT);		// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);		// set the end tag of a message
		comp.Initialize("Server", port);		// initialize the full component
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Server"));   // verify that it is a server object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));		// verify it has connected to the correct port
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));		// verify that the server was able to open a socket
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("CustomInitializationServerTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "CustomInitializationServerTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::XmlInitializationClientTest(void)
{
	if(m_debug.On()) cout << "Entering XmlInitializationClientTest()" << endl;
	INT16 port = 20100;
    std::string configFile = m_configDir + "ClientConfig.xml";

	try
	{
        // launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;				// create the thread priority
		int status = pthread_create( NULL, &attr, DumbServerStub, (void *) &port);	// start the server stub on port 20000
        CPPUNIT_ASSERT_MESSAGE("Unable to create server thread", (status == EOK));
		delay(100);																	// give it some time to start

		// setup and initialize a TCP/IP component
		XmlParser parser;
		ITcpIp comp;
		const XmlNode *document = parser.ReturnXMLDocument(configFile)->getChildren().getNode("Setup");
        comp.Initialize(document->getChildren().getNode("TcpIp"));	// initialize the full component
		// verify the initialization
		if(m_debug.On())	cout << "Configuration: " << comp.GetType() << ", " << comp.GetPort() << ", " << comp.GetIPAddress() << ", " <<
							comp.GetSock() << ", " << comp.GetFrameStart() << ", " << comp.GetFrameEnd() << endl;
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Client"));                           // verify that it is a client object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));							    // verify it has connected to the correct port
		CPPUNIT_ASSERT_MESSAGE("Invalid IPAddress", (comp.GetIPAddress() == TESTER_SERVER_ADDRESS));	// verify it is connected to the correct server
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));						        // verify that the client was able to open a socket
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame Start", (comp.GetFrameStart() == BEP_XML_ROOT));	        // validate start message
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame End", (comp.GetFrameEnd() == BEP_XML_ROOT_END));          // validate end message
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("XmlInitializationClientTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "XmlInitializationClientTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::XmlInitializationServerTest(void)
{
	if(m_debug.On()) cout << "Entering XmlInitializationServerTest()" << endl;
	INT16 port = 20110;
    std::string configFile = m_configDir + "ServerConfig.xml";

	try
	{	// setup and initialize a TCP/IP component
		XmlParser parser;
		ITcpIp comp;
		const XmlNode *document = parser.ReturnXMLDocument(configFile)->getChildren().getNode("Setup");
        comp.Initialize(document->getChildren().getNode("TcpIp"));	// initialize the full component
		// verify the initialization
		CPPUNIT_ASSERT_MESSAGE("Invalid Type", (comp.GetType() == "Server"));                           // verify that it is a server object
		CPPUNIT_ASSERT_MESSAGE("Invalid Port", (comp.GetPort() == port));							    // verify it has connected to the correct port
        CPPUNIT_ASSERT_MESSAGE("Invalid Socket", (comp.GetSock() >= 0));						        // verify that the server was able to open a socket
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame Start", (comp.GetFrameStart() == BEP_XML_ROOT));	        // validate start message
		CPPUNIT_ASSERT_MESSAGE("Invalid Frame End", (comp.GetFrameEnd() == BEP_XML_ROOT_END));          // validate end message
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("XmlInitializationServerTest Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "XmlInitializationServerTest Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
};

void ITcpIpTest::ClientWriteAndReadSmallMessage(void)
{
	INT16 port = 20040;
    std::string configFile = m_configDir + "SmallMessage.xml";
    // parse the configuration file
    XmlParser parser;
//    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering ClientWriteAndReadSmallMessage()" << endl;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;	            // create the thread priority
		int status = pthread_create( NULL, &attr, ServerStub, (void *) &port);		// start the server stub
        CPPUNIT_ASSERT_MESSAGE("Unable to create server thread", (status == EOK));
		delay(100);														            // give it some time to start

		// instantiate a client TCP/IP object
		ITcpIp comp;
        comp.SetFrameStart(BEP_XML_ROOT);						// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);						// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);
		if(m_debug.On())   comp.SetDebug(true);


		// load up the message to send
		std::string smallMessage = LoadFile(configFile);
        if(m_debug.On()) printf("RAWSM Write: %d, Length: %d, %s\n", comp.GetSock(), smallMessage.length(), smallMessage.c_str());
        // because the file could contain junk at the end make sure it is just the message
		std::string::size_type firstPos = smallMessage.find(comp.GetFrameStart());	// check for the start of frame
		if(firstPos != 0)  firstPos--;									// move the first pointer to the beginning
		if(m_debug.On())   printf("First Position: %d\n", firstPos);
        std::string::size_type secondPos = smallMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		if(m_debug.On())   printf("Second Position: %d\n", secondPos);
        smallMessage = smallMessage.substr(firstPos, secondPos);
		if(m_debug.On())   printf("Filtered Write: %d, Length: %d, %s\n", comp.GetSock(), smallMessage.length(), smallMessage.c_str());

        // send a message to the server
        comp.Write(smallMessage, comp.GetSock());
		if(m_debug.On())   printf("Wrote on %d: %s\n", comp.GetSock(), smallMessage.c_str());
        // read a message that came from the server
		std::string message;
		status = comp.Read(message, comp.GetSock());	// verify a valid message was received
		if(m_debug.On())   printf("Read on %d, length: %d, %s\n", comp.GetSock(), message.length(), message.c_str());
        CPPUNIT_ASSERT_MESSAGE("Invalid Message Received", (status != -1));
		if(m_debug.On()) printf("RAWSM Read: %d, Length: %d, %s\n", comp.GetSock(), message.length(), message.c_str());

        // verify the correct message was received
        CPPUNIT_ASSERT_MESSAGE("Invalid message response received", (message == smallMessage));
	}
	catch(BepException &e)
	{
		if(m_debug.On())   printf("ClientWriteAndReadSmallMessage Exception: %s\n", e.what());
		char buffer[256];
        sprintf(buffer, "ClientWriteAndReadSmallMessage Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
    delay(100);
};

void ITcpIpTest::ClientWriteAndReadLargeMessage(void)
{
    INT16 port = 20050;
    std::string configFile = m_configDir + "LargeMessage.xml";
    // parse the configuration file
    XmlParser parser;
//    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On())   cout << "Entering ClientWriteAndReadLargeMessage()" << endl;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;	// create the thread priority
		pthread_create( NULL, &attr, ServerStub, (void *) &port);		// start the server stub on port 20000
		delay(100);														// give it some time to start
		if(m_debug.On())   cout << "Server Launched" << endl;

		// instantiate a client TCP/IP object
		ITcpIp comp;
		if(m_debug.On())   cout << "Component Created" << endl;
		comp.SetFrameStart(BEP_XML_ROOT);					// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);					// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);
		if(m_debug.On())   cout << "Initialized" << endl;
		// load up the message to send
		std::string largeMessage = LoadFile(configFile);
		// send a message to the server
		if(m_debug.On())   cout << "Message: " << largeMessage.length() << " ," << largeMessage << endl;
		comp.Write(largeMessage, comp.GetSock());		// write the message to the server
		if(m_debug.On())   cout << "Message Written" << endl;
		// read a message that came from the server
		std::string message;
		if(m_debug.On())   cout << "Reading" << endl;
		int readStatus = comp.Read(message, comp.GetSock());
        CPPUNIT_ASSERT_MESSAGE("Unable to read data", (readStatus != -1));
		if(m_debug.On())   cout << "Read: " << message.length() << " ,"<< message << endl;
		std::string::size_type firstPos = largeMessage.find(comp.GetFrameStart());	// check for the start of frame
		firstPos--;									// move the first pointer to the beginning
		std::string::size_type secondPos = largeMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		largeMessage = largeMessage.substr(firstPos, secondPos);
		CPPUNIT_ASSERT_MESSAGE("Invalid message received", (message == largeMessage));
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "ClientWriteAndReadLargeMessage Error: " << e.Reason() << endl;
		char buffer[256];
        sprintf(buffer, "ClientWriteAndReadLargeMessage Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
    delay(1000);
};


void ITcpIpTest::ServerReadAndWriteMessages(void)
{	// initialize the server
#if 0
    INT16 port = 20060;
	INT16 sock;
    std::string configFile = m_configDir + "LargeMessage.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering ServerReadAndWriteMessages()" << endl;
	std::string::size_type firstPos;
	std::string::size_type secondPos;

	try
	{	// setup and initialize a TCP/IP component
		ITcpIp comp;
		comp.SetFrameStart(BEP_XML_ROOT);		// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);		// set the end tag of a message
		comp.Initialize("Server", port);		// initialize the full component

		// start the client that will connect to the server
		if(m_debug.On()) cout << "Starting Client Stub" << endl;
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;				// create the thread priority
		int status = pthread_create( NULL, &attr, ClientStub, (void *) &port);
        CPPUNIT_ASSERT_MESSAGE("Unable to create the client stub", status == EOK);	// start the server stub
		delay(100);																	// give it some time to start
		if(m_debug.On()) cout << "Client Stub Launched" << endl;

		std::string message;
		std::string largeMessage = LoadFile(configFile);			// load the large test message
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);	// load the small test message
		// read the small message from the client
		if(m_debug.On()) cout << "Process The Request: ";
		sock = comp.ProcessRequest ();			  					// wait for a client connect
		if(m_debug.On()) cout << sock << endl;
		assert(comp.Read(message, sock) != -1);						// read the message
		if(m_debug.On()) cout << "First Message Read: " << message << endl;

		// just parse out the message, no junk
		firstPos = smallMessage.find(comp.GetFrameStart());	// check for the start of frame
		firstPos--;									// move the first pointer to the beginning
		secondPos = smallMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		smallMessage = smallMessage.substr(firstPos, secondPos);
		assert(message == smallMessage);							// validate that it is the small message

		comp.Write(message, sock);									// write the message back
		if(m_debug.On()) cout << "First Message Written" << endl;
		assert(true);
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		assert(comp.Read(message, sock) != -1);						// verify a valid message was received
		if(m_debug.On()) cout << "First Message Verified: " << message << endl;

		// just parse out the message, no junk
		firstPos = smallMessage.find(comp.GetFrameStart());	// check for the start of frame
		firstPos--;									// move the first pointer to the beginning
		secondPos = smallMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		smallMessage = smallMessage.substr(firstPos, secondPos);
		assert(message == smallMessage);							// validate that it is the small message

		// read the large message from the client
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		assert(comp.Read(message, sock) != -1);						// read the next message form the client
		if(m_debug.On()) cout << "Server Second Message Read: " << message << endl;

		// just parse out the message, no junk
		firstPos = largeMessage.find(comp.GetFrameStart());	// check for the start of frame
		firstPos--;									// move the first pointer to the beginning
		secondPos = largeMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		largeMessage = largeMessage.substr(firstPos, secondPos);
		assert(message == largeMessage);							// validate that it is the large message

		if(m_debug.On()) cout << "Server Writing Second Message" << endl;
		comp.Write(message, sock);									// write the message back
		assert(true);
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		if(m_debug.On()) cout << "Server Reading Second Verification Message" << endl;
		assert(comp.Read(message, sock) != -1);						// verify a valid message was received
		if(m_debug.On()) cout << "Second Message Verified: " << message << endl;

		// just parse out the message, no junk
		firstPos = largeMessage.find(comp.GetFrameStart());	// check for the start of frame
		firstPos--;									// move the first pointer to the beginning
		secondPos = largeMessage.find(comp.GetFrameEnd());	// check for the end of frame
		secondPos += comp.GetFrameEnd().length() + 1;	// move the second pointer to the end
		largeMessage = largeMessage.substr(firstPos, secondPos);
		assert(message == largeMessage);							// validate that it is the large message
		if(m_debug.On()) cout << "ServerReadAndWriteMessages Test Complete" << endl;
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "ServerReadAndWriteMessages Error: " << e.Reason() << endl;
		char buffer[256];
        sprintf(buffer, "ServerReadAndWriteMessages Exception: %s", e.what());
		CPPUNIT_FAIL(buffer);
    }
#endif
};

/**
 * This tests the write method to ensure that a message
 * with a bad start tag will not be sent.
 *
 * @param configFile The name of the configuration file.
 */
void ITcpIpTest::SendMessageBadStartFrame(void)
{
#if 0
	INT16 port = 20300;
    std::string configFile = m_configDir + "ITcpIpTestConfig.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering SendMessageBadStartFrame()" << endl;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;	// create the thread priority
		pthread_create( NULL, &attr, ServerStub, (void *) &port);		// start the server stub on port 20000
		delay(100);														// give it some time to start
		if(m_debug.On()) cout << "Server Launched" << endl;

		// instantiate a client TCP/IP object
		ITcpIp comp;
		if(m_debug.On()) cout << "Component Created" << endl;
		comp.SetFrameStart("Junk");										// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);								// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);
		if(m_debug.On()) cout << "Initialized" << endl;
		assert(true);													// assert the component is set up
		// load up the message to send
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);
		// send a message to the server
		if(m_debug.On()) cout << "Message: " << smallMessage.length() << " ," << smallMessage << endl;
		comp.Write(smallMessage, comp.GetSock());		// write the message to the server
		assert(false);									// write should throw an exception, if not bad
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "SendMessageBadStartFrame Error: " << e.Reason() << endl;
		throw CppUnitException("SendMessageBadStartFrame Error: " + e.Reason(), __LINE__, __FILE__);
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "BepException: " << e.Reason() << endl;
		assert(e.Reason() == "Invalid Message Start");	// the exception thrown should be "Invalid Message Start"
	}
#endif
};

/**
 * This tests the write method to ensure that a message
 * with a bad end tag will not be sent.
 *
 * @param configFile The name of the configuration file.
 */
void ITcpIpTest::SendMessageBadEndFrame(void)
{
#if 0
	INT16 port = 20310;
    std::string configFile = m_configDir + "ITcpIpTestConfig.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering SendMessageBadEndFrame()" << endl;

	try
	{	// launch the server that the client will connect to
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;	// create the thread priority
		pthread_create( NULL, &attr, ServerStub, (void *) &port);		// start the server stub on port 20000
		delay(100);														// give it some time to start
		if(m_debug.On()) cout << "Server Launched" << endl;

		// instantiate a client TCP/IP object
		ITcpIp comp;
		if(m_debug.On()) cout << "Component Created" << endl;
		comp.SetFrameStart(BEP_XML_ROOT);								// set the start tag of a message
		comp.SetFrameEnd("Junk");										// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);
		if(m_debug.On()) cout << "Initialized" << endl;
		assert(true);													// assert the component is set up
		// load up the message to send
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);
		// send a message to the server
		if(m_debug.On()) cout << "Message: " << smallMessage.length() << " ," << smallMessage << endl;
		comp.Write(smallMessage, comp.GetSock());		// write the message to the server
		assert(false);									// write should throw an exception, if not bad
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "SendMessageBadEndFrame Error: " << e.Reason() << endl;
		throw CppUnitException("SendMessageBadEndFrame Error: " + e.Reason(), __LINE__, __FILE__);
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "BepException: " << e.Reason() << endl;
		assert(e.Reason() == "Invalid Message End");	// the exception thrown should be "Invalid Message Start"
	}
#endif
};

/**
 * This tests the instantiation of a client object when
 * the requested server does not exist.  It should throw
 * and exception that specifies the problem.
 *
 * @param configFile The name of the configuration file.
 */
void ITcpIpTest::InitializeClientNoServer(void)
{
#if 0
	INT16 port = 20300;
    std::string configFile = m_configDir + "ITcpIpTestConfig.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering InitializeClientNoServer()" << endl;

	try
	{
		if(m_debug.On()) cout << "Server Not Launched" << endl;

		// instantiate a client TCP/IP object
		ITcpIp comp;
		if(m_debug.On()) cout << "Client Created" << endl;
		comp.SetFrameStart(BEP_XML_ROOT);						// set the start tag of a message
		comp.SetFrameEnd(BEP_XML_ROOT_END);						// set the end tag of a message
		comp.Initialize("Client", port, TESTER_SERVER_ADDRESS);	// try to initialize but should not be able to
		assert(false);											// if an exception is not thrown this is bad
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "Test Expected BepException: " << e.Reason() << endl;
		assert(true);											// the exception thrown should be "Invalid Message Start"
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "InitializeClientNoServer Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeClientNoServer Error: " + e.Reason(), __LINE__, __FILE__);
	}
#endif
};

void ITcpIpTest::InitializeWithOpenSocket(void)
{
#if 0
	ITcpIp comp;
    std::string configFile = m_configDir + "ServerConfig1.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering InitializeWithOpenSocket()" << endl;
	std::string::size_type firstPos;
	std::string::size_type secondPos;

	try
	{	// setup and initialize a TCP/IP component
		XmlParser parser;
		const XmlNode *document = parser.ReturnXMLDocument(configFile);
		comp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp"));
		// start the client that will connect to the server
		if(m_debug.On())	cout << "Starting Client Stub" << endl;
		INT16 port = comp.GetPort();													// get the port number
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;					// create the thread priority
		assert(pthread_create( NULL, &attr, ClientStub, (void *) &port) == EOK );		// start the server stub
		delay(100);																		// give it some time to start
		if(m_debug.On()) cout << "Client Stub Launched" << endl;

		std::string message;
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);	// load the small test message
		// read the small message from the client
		if(m_debug.On()) cout << "Process The Request: ";
		INT32 sock = comp.ProcessRequest ();	 			   	  					// wait for a client connect
		if(m_debug.On()) cout << sock << endl;

		// now, create a new object and pass the socket to it
		ITcpIp newComp;
		newComp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp"), sock);
		assert(newComp.Read(message) != -1);						// read the message
		if(m_debug.On()) cout << "First Message Read: " << message << endl;

		// just parse out the message, no junk
		firstPos = smallMessage.find(newComp.GetFrameStart());		// check for the start of frame
		firstPos--;													// move the first pointer to the beginning
		secondPos = smallMessage.find(newComp.GetFrameEnd());		// check for the end of frame
		secondPos += newComp.GetFrameEnd().length() + 1;			// move the second pointer to the end
		smallMessage = smallMessage.substr(firstPos, secondPos);
		assert(message == smallMessage);							// validate that it is the small message

		newComp.Write(message);										// write the message back
		if(m_debug.On()) cout << "First Message Written" << endl;
		assert(true);
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		assert(newComp.Read(message) != -1);						// verify a valid message was received
		if(m_debug.On()) cout << "First Message Verified: " << message << endl;

		// just parse out the message, no junk
		firstPos = smallMessage.find(newComp.GetFrameStart());		// check for the start of frame
		firstPos--;													// move the first pointer to the beginning
		secondPos = smallMessage.find(newComp.GetFrameEnd());		// check for the end of frame
		secondPos += newComp.GetFrameEnd().length() + 1;			// move the second pointer to the end
		smallMessage = smallMessage.substr(firstPos, secondPos);
		assert(message == smallMessage);							// validate that it is the small message

		// read the large message from the client
		std::string largeMessage = LoadFile(LARGE_TEST_MESSAGE);	// load the large test message
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		assert(newComp.Read(message) != -1);						// read the next message form the client
		if(m_debug.On()) cout << "Server Second Message Read: " << message << endl;

		// just parse out the message, no junk
		firstPos = largeMessage.find(newComp.GetFrameStart());		// check for the start of frame
		firstPos--;													// move the first pointer to the beginning
		secondPos = largeMessage.find(newComp.GetFrameEnd());		// check for the end of frame
		secondPos += newComp.GetFrameEnd().length() + 1;			// move the second pointer to the end
		largeMessage = largeMessage.substr(firstPos, secondPos);
		assert(message == largeMessage);							// validate that it is the large message

		if(m_debug.On()) cout << "Server Writing Second Message" << endl;
		newComp.Write(message);										// write the message back
		assert(true);
		message.erase();											// erase the message object
		assert(message == "");										// validate that is is erased
		if(m_debug.On()) cout << "Server Reading Second Verification Message" << endl;
		assert(newComp.Read(message) != -1);						// verify a valid message was received
		if(m_debug.On()) cout << "Second Message Verified: " << message << endl;

		// just parse out the message, no junk
		firstPos = largeMessage.find(newComp.GetFrameStart());		// check for the start of frame
		firstPos--;													// move the first pointer to the beginning
		secondPos = largeMessage.find(newComp.GetFrameEnd());		// check for the end of frame
		secondPos += newComp.GetFrameEnd().length() + 1;			// move the second pointer to the end
		largeMessage = largeMessage.substr(firstPos, secondPos);
		assert(message == largeMessage);							// validate that it is the large message
		if(m_debug.On()) cout << "InitializeWithOpenSocket Test Complete" << endl;
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "InitializeWithOpenSocket Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeWithOpenSocket Error: " + e.Reason(), __LINE__, __FILE__);
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "InitializeWithOpenSocket Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeWithOpenSocket Error: " + e.Reason(), __LINE__, __FILE__);
	}
#endif
};

/**
 * This test attempts to initialize a component with
 * a valid socket but with a bad configuration file.
 *
 * @param configFile The name of the configuration file.
 */
void ITcpIpTest::InitializeWithOpenSocketNoConfig(void)
{
#if 0
	ITcpIp comp;
    std::string configFile = m_configDir + "ServerConfig2.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering InitializeWithOpenSocketNoConfig()" << endl;

	try
	{	// setup and initialize a TCP/IP component
		XmlParser parser;
		const XmlNode *document = parser.ReturnXMLDocument(configFile);
		comp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp"));

		// start the client that will connect to the server
		if(m_debug.On()) 	cout << "Starting Client Stub" << endl;
		INT16 port = comp.GetPort();								// get the port number
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;					// create the thread priority
		assert(pthread_create( NULL, &attr, ClientStub, (void *) &port) == EOK );		// start the server stub
		delay(100);																		// give it some time to start
		if(m_debug.On()) cout << "Client Stub Launched" << endl;

		std::string message;
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);	// load the small test message
		// read the small message from the client
		if(m_debug.On()) cout << "Process The Request: ";
		INT32 sock = comp.ProcessRequest ();	 			   	  					// wait for a client connect
		if(m_debug.On()) cout << sock << endl;

		// now, create a new object and pass the socket to it
		ITcpIp newComp;
		newComp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp1"), sock);
		assert(newComp.Read(message) != -1);						// ensure we can still read the message
		// validate that the mesage received is at least part of the whole
		assert(smallMessage.find(message) != std::string::npos);
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "InitializeWithOpenSocketNoConfig Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeWithOpenSocketNoConfig Error: " + e.Reason(), __LINE__, __FILE__);
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "Expected InitializeWithOpenSocketNoConfig Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeWithOpenSocketNoConfig Error: " + e.Reason(), __LINE__, __FILE__);
	}
#endif
};

/**
 * This test passes a bad socket descriptor to the object
 * and verifies that it will identify it and not attempt
 * to function.
 *
 * @param configFile The name of the configuration file.
 */
void ITcpIpTest::InitializeWithBadOpenSocket(void)
{
#if 0
	ITcpIp comp;
    std::string configFile = m_configDir + "ServerConfig3.xml";
    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	if(m_debug.On()) cout << "Entering InitializeWithBadOpenSocket()" << endl;

	try
	{	// setup and initialize a TCP/IP component
		XmlParser parser;
		const XmlNode *document = parser.ReturnXMLDocument(configFile);
		comp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp"));

		// start the client that will connect to the server
		if(m_debug.On()) 	cout << "Starting Client Stub" << endl;
		INT16 port = comp.GetPort();								// get the port number
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		attr.param.sched_priority = getpriority(PRIO_PROCESS, 0) + 1;					// create the thread priority
		assert(pthread_create( NULL, &attr, ClientStub, (void *) &port) == EOK );		// start the server stub
		delay(100);																		// give it some time to start
		if(m_debug.On()) cout << "Client Stub Launched" << endl;

		std::string message;
		std::string smallMessage = LoadFile(SMALL_TEST_MESSAGE);	// load the small test message
		// read the small message from the client
		if(m_debug.On()) cout << "Process The Request: ";
		INT32 sock = comp.ProcessRequest ();	 			   	  					// wait for a client connect
		if(m_debug.On()) cout << sock << endl;

		// now, create a new object and pass the socket to it
		ITcpIp newComp;
		newComp.Initialize(document->getChildren().getNode("Setup")->getChildren().getNode("TcpIp"), (sock+1));
		assert(newComp.Read(message) == -1);						// read the message
	}
	catch(XmlException &e)
	{
		if(m_debug.On()) cout << "InitializeWithBadOpenSocket Error: " << e.Reason() << endl;
		throw CppUnitException("InitializeWithBadOpenSocket Error: " + e.Reason(), __LINE__, __FILE__);
	}
	catch(BepException &e)
	{
		if(m_debug.On()) cout << "Expected InitializeWithBadOpenSocket Error: " << e.Reason() << endl;
		std::string::size_type firstPos;		// string position finders
		std::string fault = e.Reason();			// the string
		firstPos = fault.find("Read: The file descriptor, fildes, isn\'t a valid file descriptor open for reading");
		assert(firstPos != std::string::npos);
	}
#endif
};

void ITcpIpTest::SetBacklog(void)
{
    if(m_debug.On()) printf("SetBackLog()\n");

    std::string configFile = m_configDir + "BacklogServerConfig.xml";

    // parse the configuration file
    XmlParser parser;
    const XmlNode *doc = parser.ReturnXMLDocument(configFile);

	ITcpIp tcp0, tcp1("Server", 12345, "localhost", -1, m_debug.On(), "", "", 10);

	try
	{	// setup and initialize a TCP/IP component
		tcp0.Initialize(doc->getChildren().getNode("Setup"));

		CPPUNIT_ASSERT_MESSAGE("Unexpected value in tcp0 backlog", (tcp0.GetBacklog() == atol(doc->getChild("Setup")->getChild("Backlog")->getValue().c_str())));
		CPPUNIT_ASSERT_MESSAGE("Unexpected value in tcp1 backlog", (tcp1.GetBacklog() == 10));

		if(m_debug.On()) printf("SetBackLog(%s) done\n", configFile.c_str());
	}
	catch(BepException &e)
	{
		char buffer[256];
        sprintf(buffer, "SetBacklog XmlException: %s", e.what());
		CPPUNIT_FAIL(buffer);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
// The following are non class routines needed to support testing
//
////////////////////////////////////////////////////////////////////////////////////////

/**
 * This method will be used to retrieve will formed
 * XML messages from the open socket.
 *
 * The method will be looking for the <BEPXmlMssg>
 * headder and the </BEPXmlMssg> terminator for a
 * valid XML message.
 *
 * @param sock   The socket the client is on.
 * @return The full string that was received.
 */
const std::string ReadMessage(int sock)
{
    DebugLevel dbg;
	std::string message;				// the full message that was received
	std::string receivedData;		// message buffer
	char buf[1024];	// the buffer to read in the data
	int rval;					// the return status
	int done=0;					// done flag
	std::string::size_type firstPos;		// string position finders
	std::string::size_type secondPos;		// second string position finders

    dbg.ViaEnv("DEBUG_TEST_ITCPIP_READMSG");

	// sit in this loop until the full message is received or
	// an error occurrs
	while(!done)
	{   // read from the socket
		memset(buf, 0, sizeof(buf));
		if((rval = read(sock, buf,  sizeof(buf)-1)) > 0)
		{	// store the message
			if(dbg.On()) cout << "Received: " << rval << " ," << buf << endl;
			receivedData += buf;
			if(dbg.On()) cout << "Full Message: " << receivedData << endl;
			// look for the beginning of an XML message
			// Warning: Do not use find_last_of() or find_first_of() string methods they dont work!
			firstPos = receivedData.find(BEP_XML_ROOT);
			if(firstPos != std::string::npos)		// check if the beginning was found
			{	// look for the end
				--firstPos;
				if(dbg.On()) cout << "First Position: " << firstPos << endl;
				// Warning: Do not use find_last_of() or find_first_of() string methods they dont work!
				secondPos = receivedData.find(BEP_XML_ROOT_END);	// find the string and move to the end
				if(dbg.On()) cout << "Second Position: " << secondPos << " ," << std::string::npos << endl;
				if((secondPos != std::string::npos) && (secondPos != 1))   // if the full message is received
				{
					secondPos += 12;												// move the second pointer to the end
					int number = secondPos - firstPos + 1;							// the number of characters to read
					message = receivedData.substr(firstPos, number);				// store the sub string
					if(dbg.On()) cout << "TCP Message: " << message << endl;			// store the message
					receivedData.erase(0, secondPos+1);								// remove up to the end of the string
					if(dbg.On()) cout << "receivedData: " << receivedData << endl;		// show the message left
					done = 1;														// flag the read complete
				}
			}
		}
		else	// error is detected
		{
			switch(errno)
			{
				// The read operation was interrupted by a signal, and either no data was transferred,
				// or the resource manager responsible for that file does not report partial transfers.
				// this is a special case which requires the message to be cleared and re-read.
				case EINTR:
					if(dbg.On()) cout << "ReadMessage: The read operation was interrupted by a signal, and either no data was transferred, or the resource manager responsible for that file doesn't report partial transfers. " << endl;
					// reset the received data string
					receivedData.erase(receivedData.begin(), receivedData.end());
					break;

				// The O_NONBLOCK flag is set for the file descriptor,
				// and the process would be delayed in the read operation.
				case EAGAIN:
					if(dbg.On()) cout << "ReadMessage: The O_NONBLOCK flag is set for the file descriptor, and the process would be delayed in the read operation. " << endl;
					done = 1;
					break;

				// The file descriptor, GetSock() returned, is not a valid file descriptor open for reading.
				case EBADF:
					if(dbg.On()) cout << "ReadMessage: The file descriptor, fildes, isn't a valid file descriptor open for reading. " << endl;
					done = 1;
					break;

				// A physical I/O error occurred (for example, a bad block on a disk).
				// The precise meaning is device-dependent.
				case EIO:
					if(dbg.On()) cout << "ReadMessage: A physical I/O error occurred (for example, a bad block on a disk). The precise meaning is device-dependent. " << endl;
					done = 1;
					break;

				//The read() function isn't implemented for the filesystem specified by filedes.
				case ENOSYS:
					if(dbg.On()) cout << "ReadMessage: The read() function isn't implemented for the filesystem specified by filedes. " << endl;
					done = 1;
					break;

				// unknown error occurred
				default:
					if(dbg.On()) cout << "ReadMessage: " << rval << " Unknown Error: " << errno << endl;
					done = 1;
					break;
			}
		}
	}
	return(message);
};

/**
 * This is a client function that will be installed in a thread
 * to receive messages and then connect to a server and send
 * the received message.  This will test the receive portion
 * of the TCP/IP object.  Then the server will echo back what
 * was sent and the client will validate that it was what was
 * sent and then send a "SUCCESS" message back to the server
 */
void *ClientStub(void *arg)
{
    DebugLevel          dbg;
	int 			    sock=0;					// socket that the client will be listening on
    ClientInformation   *info = (ClientInformation *)  arg;
	INT16 			    port = info->port;
    std::string         &message = info->message;
    std::string         &serverAddress = info->serverAddresss;

    dbg.ViaEnv("DEBUG_TEST_ITCPIP_CLIENT_STUB");

    if(dbg.On()) cout << "Entering ClientStub()" << endl;

	try
	{
		struct sockaddr_in  server;			// the server information
		struct hostent      *hp;			// reference to the host information

		// Create socket
		if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw BepException("could not opening stream socket");
		if(dbg.On()) cout << "Socket Created" << endl;

		// Connect socket using name specified by command line.
		server.sin_family = AF_INET;
		if(dbg.On()) cout << "Get Host: " << serverAddress << endl;
		if((hp = gethostbyname(serverAddress.c_str())) == 0)
			throw BepException("%s: Unknown Host", serverAddress.c_str());
		if(dbg.On()) cout << "Host Found" << endl;
		memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
		if(dbg.On()) cout << "Connecting To Port: " << port << endl;
		server.sin_port = htons(port);
		if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
			throw BepException("unable to connect to connecting stream socket");
		if(dbg.On()) cout << "Connection Made" << endl;


        // write the messages to the server, read the response and write it back
		if(dbg.On()) printf("Stub Client Write: %d, Length: %d, %s\n",  sock, message.length(), message.c_str());
		if(write(sock, message.c_str(), message.length()) < 0)
			throw BepException("unable to write small message on stream socket");
		std::string readMessage = ReadMessage(sock);
		if(dbg.On()) printf("Stub Client Read: %d, Length: %d, %s\n",  sock, readMessage.length(), readMessage.c_str());
		if(write(sock, readMessage.c_str(), readMessage.length()) < 0)
			throw BepException("unable to write read small message on stream socket");

#if 0
        // write the large messages to the server, read the response and write it back
		std::string largeMessage = LoadFile(LARGE_TEST_MESSAGE);
		if(dbg.On()) printf("Stub Client Second Write: %d, Length: %d, %s\n",  sock, smallMessage.length(), smallMessage.c_str());
		if(write(sock, largeMessage.c_str(), largeMessage.length()) < 0)
			throw BepException("unable to write first large message on stream socket");
		message = ReadMessage(sock);
		if(dbg.On()) printf("Stub Client Second Read: %d, Length: %d, %s\n",  sock, smallMessage.length(), smallMessage.c_str());
		if(write(sock, message.c_str(), message.length()) < 0)
			throw BepException("unable to write read large message on stream socket");
		if(dbg.On())	cout << "ClientStub Complete" << endl;
#endif
    }
	catch(XmlException &e)
	{
		if(dbg.On()) cout << "ClientStub Xml Exception Error: " << e.Reason() << endl;
	}
	catch(BepException &e)
	{
		if(dbg.On()) cout << "ClientStub BepException Error: " << e.Reason() << endl;
	}
	catch(...)
	{
		if(dbg.On()) cout << "ClientStub Unknown Exception" << endl;
	}

	if(sock > 0)
		close(sock);

    return(NULL);
};

/**
 * This is a server function that will be installed in a thread
 * to receive TCP/IP messages and echo them back to the client
 * that sent it.  It will receive the port number that it should
 * listen to.
 */
void *ServerStub(void *arg)
{
    DebugLevel dbg;
    dbg.ViaEnv("DEBUG_TEST_ITCPIP_SERVER_STUB");

	if(dbg.On()) cout << "Entering ServerStub()" << endl;
	INT16 sock = 0;

	try
	{
		INT16 *port = (INT16 *) arg;	// get the port to listen to
		if(dbg.On())   cout << "Port: " << *port << endl;
		// request a socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(dbg.On())   cout << "Sock: " << sock << endl;
		if(sock < 0)	// if request failed
			throw BepException("Opening Stream Socket");
		else			// set up the socket
		{	// Name socket
			struct sockaddr_in server;
			server.sin_family = AF_INET;				// set the domain to the Internet
			server.sin_addr.s_addr = INADDR_ANY;		// set the socket to accept any valid address
			server.sin_port = htons(*port);				// re-define the server port to network addressing
			if(bind(sock, (struct sockaddr *)&server, sizeof(server)))
				throw BepException("Binding Stream Socket");
			if(dbg.On())   cout << "Bound, " << endl;
			listen(sock, 5);							// Listen for requests for data servers
			if(dbg.On())   cout << "Listening, " << endl;
			INT16 msgsock = accept(sock, 0, 0);				// accept a connection
			if(dbg.On())   cout << "Accept, " << endl;
			if(msgsock == -1)								// if there was an error,
			{
				if(errno == 4)	// if just a child disconnection, log
					if(dbg.On()) cout << "Child Disconnect" << endl;
				else			// else not good
					if(dbg.On()) cout << "Error in accept" << endl;
			}
			else
			{
				std::string buffer = ReadMessage(msgsock);		// retrieve the client request
				if(dbg.On())   printf("Stub Server Read: %d, Length: %d, %s\n",  msgsock, buffer.length(), buffer.c_str());
				if(buffer != "")				// if message was successfully sent
				{
					if(write(msgsock, buffer.c_str(), buffer.length()) < 0)
						if(dbg.On())   cout << "unable to write on stream socket" << endl;
				}
				else
					if(dbg.On()) cout << "Server Error: blank message sent" << endl;
				close(msgsock);
                if(dbg.On())   printf("closed: %d\n", msgsock);
			}
		}
	}
	catch(XmlException &e)
	{
		if(dbg.On()) cout << "ServerStub XmlException Error: " << e.Reason() << endl;
	}
	catch(BepException &e)
	{
		if(dbg.On()) cout << "ServerStub BepException Error: " << e.Reason() << endl;
	}
	catch(...)
	{
		if(dbg.On()) cout << "ServerStub Unknown Exception" << endl;
	}

	if(sock > 0)    close(sock);

	return(NULL);
};

/**
 * This is a server function that will be installed in a thread
 * to receive TCP/IP connections for client initialization
 * tests.
 */
void *DumbServerStub(void *arg)
{
    DebugLevel dbg;
    dbg.ViaEnv("DEBUG_TEST_ITCPIP_DUMB_SERVER");

    if(dbg.On()) cout << "Entering ServerStub()" << endl;
	INT16 sock;

	try
	{
		INT16 *port = (INT16 *) arg;	// get the port to listen to
		if(dbg.On()) cout << "Port: " << *port << endl;
		// request a socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(dbg.On()) cout << "Sock: " << sock << endl;
		if(sock < 0)	// if request failed
			throw BepException("Opening Stream Socket");
		else			// set up the socket
		{	// Name socket
			struct sockaddr_in server;
			server.sin_family = AF_INET;				// set the domain to the Internet
			server.sin_addr.s_addr = INADDR_ANY;		// set the socket to accept any valid address
			server.sin_port = htons(*port);				// re-define the server port to network addressing
			if(bind(sock, (struct sockaddr *)&server, sizeof(server)))
				throw BepException("Binding Stream Socket");
			if(dbg.On())   cout << "Bound, " << endl;
			listen(sock, 5);	    						// Listen for requests for data servers
			if(dbg.On())   cout << "Listening, " << endl;
			INT16 msgsock = accept(sock, 0, 0);				// accept a connection
			if(dbg.On()) cout << "Accept, " << endl;
			if(msgsock == -1)								// if there was an error,
			{
				if(errno == 4)	// if just a child disconnection, log
					if(dbg.On()) cout << "Child Disconnect" << endl;
				else			// else not good
					if(dbg.On()) cout << "Error in accept" << endl;
			}
			else
			{
				if(dbg.On()) cout << "DumbServer Connection Received" << endl;
				close(msgsock);
			}
		}
	}
	catch(XmlException &e)
	{
		if(dbg.On()) cout << "ServerStub XmlException Error: " << e.Reason() << endl;
	}
	catch(BepException &e)
	{
		if(dbg.On()) cout << "ServerStub BepException Error: " << e.Reason() << endl;
	}
	catch(...)
	{
		if(dbg.On()) cout << "ServerStub Unknown Exception" << endl;
	}

	if(sock > 0)
		close(sock);
	return(NULL);
};

