//******************************************************************************
// Description:
//    Melsec serial protocol wrapper class
//
//==============================================================================
//
// Copyright © 2005 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
//******************************************************************************
#ifndef _IMELSEC_PROTOCOL_H_INCLUDED_
#define _IMELSEC_PROTOCOL_H_INCLUDED_

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

#define     NULc	0x00        // Null
#define     STXc	0x02        // Start Of Text
#define     ETXc	0x03        // End Of Text
#define     EOTc	0x04        // End Of Transmission
#define     ENQc	0x05        // Enquire
#define     ACKc	0x06        // Acknowledge
#define     BELc	0x07        // Bell
#define     BSc		0x08        // Back Space
#define     LFc		0x0A        // Line Feed
#define     FFc		0x0C        // Form Feed
#define     CRc		0x0D        // Carraige Return
#define		DLEc	0x10
#define     DC1c	0x11        // Device Command 1
#define     XONc	0x11        // XON
#define     DC2c	0x12        // Device Command 2
#define     DC3c	0x13        // Device Command 3
#define     XOFFc	0x13        // XOFF
#define     DC4c	0x14        // Device Command 4
#define     NAKc	0x15        // Negative Acknowledge
#define     SUBc	0x1A        // ?clr_scr?
#define     EOFc	0x1A        // End Of File
#define     ESCc	0x1B        // Escape
#define     DELc	0x7F        // Delete

#define		SRCc	0x01		// VME is source
#define		DSTc	0x4F		// PLC is destination

class IMelsecProtocolTest;

/**
 * Melsec protocol wrapper
 */
class IMelsecProtocol
{
    friend class MelsecProtocolTest;

public:
    /**
     * Default constructor
     */
    IMelsecProtocol();

    /**
     * Destructor
     */
    virtual ~IMelsecProtocol();

    /**
     * Open a Melsec serial PLC
     *
     * @param comDev Handle to the communication device to be used for Melsec communication (i.e. a
     *               socket, a serial port ,etc)
     * 
     * @return EOK if successful
     */
    int Open(int comDev);

    /**
     * Open a Melsec serial PLC with a base address for PLC I/O reads & writes
     *
     * @param comDev Handle to the communication device to be used for Melsec communication (i.e. a
     *               socket, a serial port ,etc)
     * 
     * @return EOK if successful
     */
    int Open(int comDev, string inAddr, string outAddr);

    /**
     * Close an instance of a Melsec PLC
     *
     * @return EOK if successful. Any other value indicates error
     */
    int Close();

    /**
     * Initialize data for Melsec communication
     *
     * @param logger ILogger to use for initializing my logger interface
     */
    void Init( ILogger &logger);

    /**
     * Read data from a Melsec PLC
     *
     * @param buff   Buffer into which the data will be read
     * @param count  Number of 32-bit integers to read
     *
     * @return 1 - good, 0 - error
     */
    int Read(uint32_t *buff, uint32_t count);

    /**
     * Write data to a Melsec PLC
     *
     * @param buff   Buffer from which the data will be written
     * @param count  Number of 32-bit integers to write
     *
     * @return 1 - good, 0 - error
     */
    int Write(const uint32_t *buff, uint32_t count);

private:

    /**
     * Reads integer data from the PLC
     * 
     * @param buff - message returned from the PLC
     *
     * @return Number of integers read
     */
    long BufferRead(uint8_t *buff,uint32_t count);

    /**
     * Receives and validates a PLC Melsec reply message
     *
     * @param buff      Buffer into which the Melsec data should be received
     * @param plcData   Array into which converted binary data should be put
     * 
     * @return 0 if buffer is valid; any other value indicates an error
     */
    long CheckBufferRead( uint8_t *buff, uint32_t *plcData);

    /**
     * Transfers integer data to the PLC
     *
     * @param byteCnt Number of integers to transfer
     * @param plcOut  Pointer to start of array of integers to be transferred
     * 
     * @return Number of bytes transmitted
     */
    long BufferWrite(short byteCnt, const uint32_t *plcOut);

    /**
     * Wait for the PLC to respond to our commands
     * 
     * @param rxBuff - storage for the returned data
     *
     * @return true  - response received
     *         false - no response
     */
    bool WaitForResponse(uint8_t *rxBuff);

    /**
     * Acknowledge a successful read operation by sending ACK to the PLC
     */
    void AckBufferRead();

    /**
     * Notify the PLC of a failed read operation by sending NAK to the PLC
     */
    void NakBufferRead();

    /**
     * Send a message to clear a PLC error
     */
    void ClearPlc();

    /**
     * Convert ASCII hex data to a long
     * 
     * @param buff - buffer with ASCII data
     * @param cnt - number of ASCII characters
     * 
     * @return the converted long value
     */ 
    long HexToLong(uint8_t *buff,long cnt);

    /**
     * Logs a PLC response string
     *
     * @param logMask Our logger interface verbosity bit which must be set in order to log
     *                the string
     * @param prefix  String to prepend to the PLC data string when logged
     * @param plcData The PLC data string to be logged
     * @param dataLen The length of the PLC data string
     */
    virtual void LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen);

    /**
     * Object used to print/log data
     */
    ILogger                 m_logger;

    /**
     * Mutex used to lock Melsec operations
     */
    BepMutex                m_melsecLock;

    /**
     * The base address for performing PLC input reads
     */
    string                  m_melsecInAddr;

    /**
     * The base address for performing PLC output writes
     */
    string                  m_melsecOutAddr;

    /**
     * File descriptor to use for Melsec communication. This could be a handle to a serial port,
     * a socket...just imagine the possibilities
     */
    int                     m_melsecComDev;

    /**
     * Melsec PLC station number
     */ 
    short                   m_stationNumber;

    /**
     * Our PC number for Melsec PLC communication
     */
    short                   m_pcNumber;
};

#endif // _IMELSEC_PROTOCOL_H_INCLUDED_

