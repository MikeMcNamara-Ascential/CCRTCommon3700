//******************************************************************************
// Description:
//    Allen Bradley TCP Socket Services wrapper class
//
//==============================================================================
//
// Copyright © 1999 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/ITcpProtocol.h $
// 
// 5     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:17p Rwiersem
// Made the test class a friend.  Made the destructor virtual.  Added
// LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.
//
// 4     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:15p Bmeinke
// Added m_terminate flag to signal the thread when it should stop
// Added an Initialize( XmlNode*) method to read config file data
// Changed to more closely resemble the NCA TCP implementation
// Read() and Write() now return true/false
//
// 3     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     2/16/05 9:29p Cward
// UPdated TCP Protocol with many fixes found in the field.
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
//
// 4     4/16/03 1:15p Bmeinke
// Added LogPlcString() method to output a PLC serial data string via the
// logger interface
// General cleanup: removed unused methods, removed commented out blocks
//
// 3     3/12/03 12:39a Bmeinke
// Initial machine testing for Ford Chicago
// Added a ILogger argument to Init() to initialize our logger interface
//
// 2     1/18/03 3:43p Bmeinke
// Maintenance check-in (not tested yet)
//
// 1     12/12/02 1:13p Bmeinke
// Initial check-in
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _ITCP_PROTOCOL_H_INCLUDED_
#define _ITCP_PROTOCOL_H_INCLUDED_

#include <fcntl.h>
#include <errno.h>
#include <atomic.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/neutrino.h>
#include <termios.h>

#include "ILogger.h"
#include "BepDefs.h"
#include "BepSync.h"

#define     AB_TYPE             0
#define     AB_READ_HDR         "WriteN17"      // Tell PLC to write N17 to the port so we can read it
#define     AB_WRITE_HDR        "ReadN16"       // Tell PLC to read N16 because we just wrote it
#define     AB_RCV_HDR          "SentN17"       // Beginning of PLC's N17 Data message (For AB RSLogix, should be "SentN17". For Siemens S7 300, should be ""
#define     AB_SEND_HDR_SIZE    10              // Size of outgoing message header
#define     AB_RCV_HDR_SIZE     10              // Size of incoming message header

#define     S7_TYPE             1
#define     S7_MEM_AREA         1
#define     S7_READ_DBNO        201
#define     S7_WRITE_DBNO       200
#define     S7_READ_HDR         { 83, 53, 16, 1, 3, 5, 3, 8, S7_MEM_AREA, S7_READ_DBNO, 0, 0, 0, 0, 255, 2 }
#define     S7_WRITE_HDR        { 83, 53, 16, 1, 3, 3, 3, 8, S7_MEM_AREA, S7_WRITE_DBNO, 0, 0, 0, 0, 255, 2 }
#define     S7_RCV_HDR          { 83, 53, 16, 1, 3, 6, 15, 3, 0, 255, 7, 0, 0, 0, 0, 0 }
#define     S7_SEND_HDR_SIZE    16
#define     S7_RCV_HDR_SIZE     16

#define     MAX_HDR_SIZE        16

// #define     NULc    0x00        // Null                  //
// #define     STXc    0x02        // Start Of Text         //
// #define     ETXc    0x03        // End Of Text           //
// #define     EOTc    0x04        // End Of Transmission   //
// #define     ENQc    0x05        // Enquire               //
// #define     ACKc    0x06        // Acknowledge           //
// #define     BELc    0x07        // Bell                  //
// #define     BSc     0x08        // Back Space            //
// #define     LFc     0x0A        // Line Feed             //
// #define     FFc     0x0C        // Form Feed             //
// #define     CRc     0x0D        // Carraige Return       //
// #define     DLEc    0x10                                 //
// #define     DC1c    0x11        // Device Command 1      //
// #define     XONc    0x11        // XON                   //
// #define     DC2c    0x12        // Device Command 2      //
// #define     DC3c    0x13        // Device Command 3      //
// #define     XOFFc   0x13        // XOFF                  //
// #define     DC4c    0x14        // Device Command 4      //
// #define     NAKc    0x15        // Negative Acknowledge  //
// #define     SUBc    0x1A        // ?clr_scr?             //
// #define     EOFc    0x1A        // End Of File           //
// #define     ESCc    0x1B        // Escape                //
// #define     DELc    0x7F        // Delete                //
//                                                          //
// #define     SRCc    0x01        // VME is source         //
// #define     DSTc    0x4F        // PLC is destination    //

    /**
 * Allen bradley TCP protocol wrapper
 *
 * @author Brian Meinke
 * @version 1.0
 * @since Dec 12, 2002
 */
class ITcpProtocol
{
    friend class ITcpProtocolTest;

public:
    /**
     * Default constructor
     */
    ITcpProtocol();

    /**
     * Destructor
     */
    virtual ~ITcpProtocol();

    /**
     * Open a TCP serial PLC
     *
     * @param comDev Handle to the communication device to be used for TCP communication (i.e. a
     *               socket, a serial port ,etc)
     * @param src    TCP node ID to be used as the source identifier in TCP messages
     * @return EOK if successful
     */
    int Open( int comDev, uint8_t src);
    int OpenSeparateRW( int comDevRead, int comDevWrite, uint8_t src);

    /**
     * Close an instance of a TCP PLC
     *
     * @return EOK if successful. Any other value indicates error
     */
    int Close();

    /**
     * Initialize data for TCP communication
     *
     * @param logger ILogger to use for initializing my logger interface
     */
    void Init( ILogger &logger);

    /**
     * Initialize data for TCP communication from a configuration node
     *
     * @param tcpConfig Configuration node to read the TCP parameters from
     */
    void Initialize( const XmlNode *tcpConfig);

    /**
     * Read data from a TCP PLC
     *
     * @param dest   TCP destination node ID to which the message is addressed
     * @param buff   Buffer into which the data will be read
     * @param count  Number of 32-bit integers to read
     * @param addr   PLC address to read data from (default = $N16)
     *
     * @return true if PLC data was read successfully
     */
    bool Read( uint8_t dest, uint32_t *buff, uint32_t count, const char *addr="$N17");

    /**
     * Write data to a TCP PLC
     *
     * @param dest   TCP destination node ID to which the message is addressed
     * @param buff   Buffer from which the data will be written
     * @param count  Number of 32-bit integers to write
     * @param addr   PLC address to write data to (default = $N17)
     *
     * @return true if data was written successfully
     */
    bool Write( uint8_t dest, const uint32_t *buff, uint32_t count, const char *addr="$N16");

protected:
    /**
     * Load the additional configuration items needed by the ITcpProtocol
     * object.
     *
     * @param tcpConfig Configuration node to read the TCP parameters from
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *tcpConfig);

private:

    /**
     * Receives and validates a PLC TCP reply message
     *
     * @param buff      Buffer into which the TCP data should be received
     * @param byteCnt   Maximum number of bytes to receive from the PLC
     * @param plcData   Array into which converted binary data should be put
     * @param dataCount Maximum number of PLC words to convert into the plcData array
     * @return 0 if buffer is valid; any other value indicates an error
     */
    long CheckBufferRead( uint8_t *buff, short byteCnt, uint32_t *plcData, int dataCount);

//     /**                                                                   //
//     * Checks the received message for validity.                           //
//     *                                                                     //
//     * @param buff    pointer to received message buffer                   //
//     * @param cnt     number of characters in buffer                       //
//     * @param byteCnt the number of bytes expected in the response         //
//     *                                                                     //
//     * @return true if message is valid                                    //
//     *         false if there is any problem                               //
//     */                                                                    //
//     bool IsResponseMessageValid( uint8_t *buff, int cnt, short byteCnt);  //

    /**
     * Transfers integer data to the PLC
     *
     * @param destId  TCP destination node ID to which the message is addressed
     * @param byteCnt Number of integers to transfer
     * @param plcOut  Pointer to start of array of integers to be transferred
     * @param plcAddr PLC Address (i.e. N16:0) into which the integers should be transferred
     * @return Number of bytes transmitted
     */
    long BufferWrite( uint8_t destId, short byteCnt, const uint32_t *plcOut, char *plcAddr);

//     /**                                                                                //
//      * Calculates the number of data bytes in a PLC write message taking into          //
//      * consideration duplicate DLE's                                                   //
//      *                                                                                 //
//      * @param plcOut  Raw data buffer to be written                                    //
//      * @param buffLen Number of items in the plcOut array                              //
//      * @return Number of items in the data field of a PLC write message for the given  //
//      *         PLC data                                                                //
//      */                                                                                //
//     uint32_t CalculateBufferLen( const uint32_t *plcOut, uint32_t buffLen);            //

    /**
     * Reads integer data from the PLC
     *
     * @param destId  TCP destination node ID to which the message is addressed
     * @param byteCnt Number of integers to read
     * @param plcAddr PLC Address (i.e. N17:0) from which the data should be read
     * @return Number of integers read
     */
    long BufferRead( uint8_t destId, short byteCnt, char *plcAddr);

//     /**                                                                     //
//      * Acknowledge a successful read operation by sending ACK to the PLC    //
//      */                                                                     //
//     void AckBufferRead();                                                   //
//                                                                             //
//     /**                                                                     //
//      * Notify the PLC of a failed read operation by sending NAK to the PLC  //
//      */                                                                     //
//     void NAkBufferRead();                                                   //
//                                                                             //
//     /**                                                                     //
//      * Request re-transmission of the last read operation                   //
//      */                                                                     //
//     void ENQBufferRead();                                                   //
//                                                                             //
//     /**                                                                     //
//      * Increment the transaction counter                                    //
//      *                                                                      //
//      * @return Incremented transation counter                               //
//      */                                                                     //
//     unsigned int IncTNSCntr();                                              //
//                                                                             //
//     /**                                                                     //
//      * Get the most recent transaction counter                              //
//      *                                                                      //
//      * @return transation counter                                           //
//      */                                                                     //
//     unsigned int GetTNSCntr();                                              //

//     /**                                                                    //
//      * Calculates the CRC for a PLC serial transaction                     //
//      *                                                                     //
//      * @param pp     Data array to calculate the CRC for                   //
//      * @param len    Number of bytes in the data array                     //
//      * @return CRC for the given data array                                //
//      */                                                                    //
//     unsigned short BccCalc( register uint8_t *pp, register short len);     //
//                                                                            //
//     /**                                                                    //
//      * Calculates the CRC for a PLC serial transaction                     //
//      *                                                                     //
//      * @param pp     Data array to calculate the CRC for                   //
//      * @param len    Number of bytes in the data array                     //
//      * @return CRC for the given data array                                //
//      */                                                                    //
//     unsigned short BccCalcRaw( register uint8_t *pp, register short len);  //

    /**
     * Waits for all reply data from the PLC
     *
     * @param rxBuff Buffer to receive the PLC data into
     * @param count  Number of bytes in the rxBuff paramater
     * @return Number of bytes read
     */
    int WaitForAllData( uint8_t *rxBuff, int count);

//     /**                                                         //
//      * Validates a TCP CRC                                      //
//      *                                                          //
//      * @param tcpData TCP serial string to validate             //
//      * @param tcpSize number of bytes in the TCP serial string  //
//      * @return 1 if CRC is valid, 0 if it is invalid            //
//      */                                                         //
//     int ValidateCrc( uint8_t *tcpData, int tcpSize);            //
//                                                                 //
//     /**                                                         //
//      * Validates a TCP CRC                                      //
//      *                                                          //
//      * @param tcpData TCP serial string to validate             //
//      * @param tcpSize number of bytes in the TCP serial string  //
//      * @return 1 if CRC is valid, 0 if it is invalid            //
//      */                                                         //
//     int ValidateCrcRaw( uint8_t *tcpData, int tcpSize);         //

    /**
     * Extracts PLC data from a TCP serial string
     *
     * @param tcpData   TCP serial string to extract data from
     * @param tcpSize   Number of bytes in the TCP serial string
     * @param plcData   Array into which converted data will be written
     * @param dataCount Number of elements in the plcData array
     * @return Number of 32-bit integers converted and written to the plcData array
     */
    int ExtractData( uint8_t *tcpData, int tcpSize, uint32_t *plcData, int dataCount);

//     /**                                          //
//      * Wait for the PLC to ACK/NAK our commands  //
//      *                                           //
//      * @return NAKc if NAK received              //
//      *         ACKs if ACK recevied              //
//      *         ENQc if ENQ received              //
//      */                                          //
//     char WaitForAck();                           //

    /**
     * Logs a PLC response string
     *
     * @param logMask Our logger interface verbosity bit which must be set in order to log
     *                the string
     * @param prefix  String to prepend to the PLC data string when logged
     * @param plcData The PLC data string to be logged
     * @param dataLen The length of the PLC data string
     */
    void LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen);

    /**
     * Object used to print/log data
     */
    ILogger                 m_logger;

    /**
     * Mutex used to lock TCP operations
     */
    BepMutex                m_tcpLock;

    /**
     * Number of bytes to read/write in each TCP transmission
     */
    int                     m_tcpBytesPerBlock;

    /**
     * Flag used to only perform initialization once
     */
    unsigned                m_tcpInitCount;

    /**
     * File descriptor to use for (write to PLC) TCP communication. This could be a handle to a serial port,
     * a socket...just imagine the possibilities
     */
    int                     m_tcpComDevWrite;
    /**
     * File descriptor to use for (read from PLC) TCP communication. This could be a handle to a serial port,
     * a socket...just imagine the possibilities
     */
    int                     m_tcpComDevRead;

//     /**                                       //
//      * DF! transaction counter                //
//      */                                       //
//     uint16_t                m_tcpTnsCounter;  //

//     /**                                                //
//      * TCP source node ID to be used for TCP messages  //
//      */                                                //
//     uint8_t                 m_tcpSrcId;                //

    volatile bool           m_terminate;

    int                     m_tcpType;
};

#endif // _ITCP_PROTOCOL_H_INCLUDED_

