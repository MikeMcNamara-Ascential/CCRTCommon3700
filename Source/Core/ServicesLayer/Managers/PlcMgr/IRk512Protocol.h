//******************************************************************************
// Description:
//    Siemens Rk512 serial protocol wrapper class
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/IRk512Protocol.h $
// 
// 2     10/30/07 4:04p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     1/18/07 2:51p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _IRK512_PROTOCOL_H_INCLUDED_
#define _IRK512_PROTOCOL_H_INCLUDED_

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

#define     NULc    0x00        // Null
#define     STXc    0x02        // Start Of Text
#define     ETXc    0x03        // End Of Text
#define     EOTc    0x04        // End Of Transmission
#define     ENQc    0x05        // Enquire
#define     ACKc    0x06        // Acknowledge
#define     BELc    0x07        // Bell
#define     BSc     0x08        // Back Space
#define     LFc     0x0A        // Line Feed
#define     FFc     0x0C        // Form Feed
#define     CRc     0x0D        // Carraige Return
#define     DLEc    0x10
#define     DC1c    0x11        // Device Command 1
#define     XONc    0x11        // XON
#define     DC2c    0x12        // Device Command 2
#define     DC3c    0x13        // Device Command 3
#define     XOFFc   0x13        // XOFF
#define     DC4c    0x14        // Device Command 4
#define     NAKc    0x15        // Negative Acknowledge
#define     SUBc    0x1A        // ?clr_scr?
#define     EOFc    0x1A        // End Of File
#define     ESCc    0x1B        // Escape
#define     DELc    0x7F        // Delete

#define     SRCc    0x01        // VME is source
#define     DSTc    0x4F        // PLC is destination

/**
 * Siemens Rk512 protocol wrapper
 *
 * @author EPatterson MMcNamara
 * @version 1.0
 * @since Jan 31, 2006
 */
class IRk512Protocol
{
    friend class IRk512ProtocolTest;

public:
    /**
     * Default constructor
     */
    IRk512Protocol();

    /**
     * Destructor
     */
    ~IRk512Protocol();

    /**
     * Open a Rk512 serial PLC
     *
     * @param comDev Handle to the communication device to be used for Rk512 communication (i.e. a
     *               socket, a serial port ,etc)
     * @return EOK if successful
     */
    int Open( int comDev);    

    /**
     * Close an instance of a Rk512 PLC
     *
     * @return EOK if successful. Any other value indicates error
     */
    int Close();

    /**
     * Initialize data for Rk512 communication
     *
     * @param logger ILogger to use for initializing my logger interface
     */
    void Init( ILogger &logger);

    /**
     * Read data from a Rk512 PLC
     *
     * @param buff   Buffer into which the data will be read
     * @param count  Number of 32-bit integers to read
     * @param addr   PLC address to read data from (default = $N16)
     * @param offset The data offset from the base address.
     *
     * @return Number of 32-bit integers read
     */
    //int Read( uint8_t dest, uint32_t *buff, uint32_t count, const char *addr="$N17");
    //Siemens PLC uses input "data block" instead of N17 address
    int Read(uint32_t *buff, uint32_t count, uint8_t addr, uint8_t offset);

    /**
     * Write data to a Rk512 PLC
     *
     * @param buff         Buffer from which the data will be written
     * @param wordCount    Number of 32-bit integers to write
     * @param writeAddress PLC address to write data to (default = $N17)
     * @param offset       The data offset from the base address.
     *
     * @return Number of 32-bit integers written
     */
    //int Write( uint8_t dest, const uint32_t *buff, uint32_t count, const char *addr="$N16");
    //Siemens PLC uses output "data block" instead of N16 address
    int Write(const uint32_t *buff, uint32_t wordCount, uint8_t writeAddress, uint8_t offset);

private:

    /**
     * Receives and validates a PLC Rk512 reply message
     *
     * @param buff      Buffer into which the Rk512 data should be received
     * @param byteCnt   Maximum number of bytes to receive from the PLC
     * @param plcData   Array into which converted binary data should be put
     * @param dataCount Maximum number of PLC words to convert into the plcData array
     * @return 0 if buffer is valid; any other value indicates an error
     */
    long CheckBufferRead( uint8_t *buff, short byteCnt, uint32_t *plcData, int dataCount);

    /**
    * Checks the received message for validity.
    *
    * @param buff    pointer to received message buffer
    * @param cnt     number of characters in buffer
    * @param byteCnt the number of bytes expected in the response
    *
    * @return true if message is valid
    *         false if there is any problem
    */
    bool IsResponseMessageValid( uint8_t *buff, int cnt, short byteCnt);

    /**
     * Transfers integer data to the PLC
     *
     * @param destId  Rk512 destination node ID to which the message is addressed
     * @param byteCnt Number of integers to transfer
     * @param plcOut  Pointer to start of array of integers to be transferred
     * @param plcAddr PLC Address (i.e. N16:0) into which the integers should be transferred
     * @return Number of bytes transmitted
     * 
     */
    long BufferWrite( short wordCount, short db, uint8_t dw,const uint32_t *buff);    

    /**
     * Calculates the number of data bytes in a PLC write message taking into
     * consideration duplicate DLE's
     *
     * @param plcOut  Raw data buffer to be written
     * @param buffLen Number of items in the plcOut array
     * @return Number of items in the data field of a PLC write message for the given
     *         PLC data
     */
    uint32_t CalculateBufferLen( const uint32_t *plcOut, uint32_t buffLen);

    /**
     * Reads integer data from the PLC
     *
     * @param destId  Rk512 destination node ID to which the message is addressed
     * @param byteCnt Number of integers to read
     * @param plcAddr PLC Address (i.e. N17:0) from which the data should be read
     * @return Number of integers read
     */
    //long BufferRead( uint8_t destId, short byteCnt, char *plcAddr);
    long BufferRead(short byteCnt, short dw, uint8_t db);    

    /**
     * Acknowledge a successful read operation by sending ACK to the PLC
     */
    void AckBufferRead();

    /**
     * Notify the PLC of a failed read operation by sending NAK to the PLC
     */
    void NAkBufferRead();

    /**
     * Request re-transmission of the last read operation
     */
    void ENQBufferRead();

    /**
     * Calculates the CRC for a PLC serial transaction
     *
     * @param pp     Data array to calculate the CRC for
     * @param len    Number of bytes in the data array
     * @return CRC for the given data array
     */
    unsigned short BccCalc( register uint8_t *pp, register short len);

    /**
     * Calculates the CRC for a PLC serial transaction
     *
     * @param pp     Data array to calculate the CRC for
     * @param len    Number of bytes in the data array
     * @return CRC for the given data array
     */
    unsigned short BccCalcRaw( register uint8_t *pp, register short len);

    /**
     * Waits for all reply data from the PLC
     *
     * @param rxBuff Buffer to receive the PLC data into
     * @param count  Number of bytes in the rxBuff paramater
     * @return Number of bytes read
     */
    int WaitForAllData( uint8_t *rxBuff, int count);

    /**
     * Validates a Rk512 CRC
     *
     * @param rk512Data Rk512 serial string to validate
     * @param rk512Size number of bytes in the Rk512 serial string
     * @return 1 if CRC is valid, 0 if it is invalid
     */
    int ValidateCrc( uint8_t *rk512Data, int rk512Size);

    /**
     * Validates a Rk512 CRC
     *
     * @param rk512Data Rk512 serial string to validate
     * @param rk512Size number of bytes in the Rk512 serial string
     * @return 1 if CRC is valid, 0 if it is invalid
     */
    int ValidateCrcRaw( uint8_t *rk512Data, int rk512Size);

    /**
     * Extracts PLC data from a Rk512 serial string
     *
     * @param rk512Data   Rk512 serial string to extract data from
     * @param rk512Size   Number of bytes in the Rk512 serial string
     * @param plcData   Array into which converted data will be written
     * @param dataCount Number of elements in the plcData array
     * @return Number of 32-bit integers converted and written to the plcData array
     */
    int ExtractData( uint8_t *rk512Data, int rk512Size, uint32_t *plcData, int dataCount);

    /**
     * Wait for the PLC to ACK/NAK our commands
     *
     * @return NAKc if NAK received
     *         ACKs if ACK recevied
     *         ENQc if ENQ received
     */
    //char WaitForAck();   
    char WaitForAck(int numbytes);   

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
     * Mutex used to lock Rk512 operations
     */
    BepMutex                m_rk512Lock;

    /**
     * Number of bytes to read/write in each Rk512 transmission
     */
    const int               m_rk512BytesPerBlock;

    /**
     * Flag used to only perform initialization once
     */
    unsigned                m_rk512InitCount;

    /**
     * File descriptor to use for Rk512 communication. This could be a handle to a serial port,
     * a socket...just imagine the possibilities
     */
    int                     m_rk512ComDev;

    /**
     * DF! transaction counter
     */
    uint16_t                m_rk512TnsCounter;

    /**
     * Rk512 source node ID to be used for Rk512 messages
     */
    uint8_t                 m_rk512SrcId;

};

#endif // _IRk512_PROTOCOL_H_INCLUDED_

