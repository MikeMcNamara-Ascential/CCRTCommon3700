//******************************************************************************
// Description:
//  The Test Class For Testing the Melsec PLC Class.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /RossWorking/Tests/Korea/Unit/ServicesLayer/Managers/PlcMgr/MelsecProtocolTest.h $
// 
// 2     7/31/06 3:41p Rwiersem
// Added more test cases.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MELSECPROTOCOLTEST_H
#define MELSECPROTOCOLTEST_H
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"
#include "IMelsecProtocol.h"

/**
 * Test class for
 *
 */
class MelsecProtocolTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MelsecProtocolTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(OpenTest);
    CPPUNIT_TEST(CloseTest);
    CPPUNIT_TEST(Read_MutexFailure);
    CPPUNIT_TEST(Write_MutexFailure);
    CPPUNIT_TEST(HexToLongTest);
    CPPUNIT_TEST(CheckBufferReadTest);
    CPPUNIT_TEST(CheckBufferRead_NoEtx);
    CPPUNIT_TEST(CheckBufferRead_BadChecksum);
    CPPUNIT_TEST(CheckBufferRead_EtxByte);
    CPPUNIT_TEST(CheckBufferRead_StxByte);
    CPPUNIT_TEST(CheckBufferRead_BadStationNumber);
    CPPUNIT_TEST(CheckBufferRead_BadPcNumber);
    CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * Class constructor
	 */
	MelsecProtocolTest();

    /**
	 * Class destructor
	 */
	virtual ~MelsecProtocolTest();
	
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

//*********************************************************
//	 Test Methods
//*********************************************************

    /**
     * Test that will verify that the object is correctly initialized on construction
     */
    void ConstructorTest(void);

    /**
     * Test that will verify the serial port was set correctly.
     */ 
    void OpenTest(void);

    /**
     * Test that will verify the serial port was close properly.
     */
    void CloseTest(void);

    /**
     * Test that will verify the Read() method returns an error if it 
     * can't lock the mutex.
     */ 
    void Read_MutexFailure(void);

    /**
     * Test that will verify the Read() method returns an error if the
     * call to BufferRead() fails.
     */ 
    void Read_BufferReadFailure(void);

    /**
     * Test that will verify the Read() method returns an error if the
     * call to ChekcBufferRead() fails.
     */ 
    void Read_CheckBufferReadFailure(void);

    /**
     * Test that will verify the Write() method works properly
     */
    void WriteTest(void);

    /**
     * Test that will verify the Write() method returns an error if it
     * can't lock the mutex.
     */ 
    void Write_MutexFailure(void);

    /**
     * Test that will verify the Write() method returns an error if the
     * call to BufferWrite() fails.
     */ 
    void Write_BufferWriteFailure(void);

    /**
     * Test that will verify the BufferRead method works properly.
     */ 
    void BufferReadTest(void);

    /**
     * Test that will verify BufferRead returns an error if there
     * is an error in Write()
     */ 
    void BufferRead_WriteError(void);

    /**
     * Test that will verify BufferRead returns an error if there
     * is no response to the message
     */ 
    void BufferRead_NoResponse(void);

    /**
     * Test that will verify the BufferWrite method works properly.
     */ 
    void BufferWriteTest(void);

    /**
     * Test that will verify BufferWrite returns an error if there
     * is an error in Write()
     */ 
    void BufferWrite_WriteError(void);

    /**
     * Test that will verify BufferWrite returns an error if there
     * is no response to the message
     */ 
    void BufferWrite_NoResponse(void);

    /**
     * Test that will verify the WaitForResponse method works properly.
     */ 
    void WaitForResponseTest(void);

    /**
     * Test that will verify WaitForResponse returns an error if Read()
     * detects an error.
     */ 
    void WaitForResponse_ReadError(void);

    /**
     * Test that will verify WaitForResponse returns an error if no
     * response was received.
     */ 
    void WaitForResponse_NoResponse(void);

    /**
     * Test that will verify the HexToLong method.
     */
    void HexToLongTest(void);

    /**
     * Test that will verify the CheckBufferRead properly processes the
     * response to a buffer read message.
     */ 
    void CheckBufferReadTest(void);

    /**
     * Test that will verify that CheckBufferRead returns an error if 
     * there is no ETX in the response from the PLC.
     */ 
    void CheckBufferRead_NoEtx(void);

    /**
     * Test that will verify that CheckBufferRead returns an error if
     * the checksum is bad;
     */ 
    void CheckBufferRead_BadChecksum(void);

    /**
     * Test that will verify CheckBufferRead returns an error if
     * the ETX byte is in the wrong location.
     */ 
    void CheckBufferRead_EtxByte(void);

    /**
     * Test that will verify CheckBufferRead returns an error if
     * the STX byte is missing.
     */ 
    void CheckBufferRead_StxByte(void);

    /**
     * Test that will verify CheckBufferRead returns an error if
     * the station number is bad.
     */ 
    void CheckBufferRead_BadStationNumber(void);

    /**
     * Test that will verify CheckBufferRead returns an error if
     * the pc number is bad.
     */ 
    void CheckBufferRead_BadPcNumber(void);

    /**
     * Test that will verify AckBufferRead sends the correct message
     * to the PLC
     */ 
    void AckBufferReadTest(void);

    /**
     * Test that will verify NakBufferRead sends the correct message
     * to the PLC
     */ 
    void NakBufferReadTest(void);

   /**
     * Test that will verify ClearPlc sends the correct message
     * to the PLC
     */ 
    void ClearPlcTest(void);

private:
    /**
     * MockIMelsecProtocol pointer
     */
    IMelsecProtocol* imp;

    /**
     * The directory to find the configuration files in.
     */
    std::string m_configDir;

    /**
     * Variable that holds the FTP_ROOT value prior to testing so
     * it can be restored.
     */
    std::string m_ftpRoot;

    /**
     * Variable that holds the USR_ROOT value prior to testing so
     * it can be restored.
     */
    std::string m_usrRoot;

    /**
     * Variable that holds the PWD value prior to testing so
     * it can be restored.
     */
    std::string m_usrPwd;

    /**
     * Variable that holds the current debug level
     */
    DebugLevel m_debug;
};
#endif // MELSECPROTOCOLTEST_H
