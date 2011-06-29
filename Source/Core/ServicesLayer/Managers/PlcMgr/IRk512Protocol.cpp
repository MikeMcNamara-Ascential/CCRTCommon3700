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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/IRk512Protocol.cpp $
// 
// 2     2/22/07 11:06a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added handling for duplicate DLE characters in BufferWrite().
// 
// 1     1/18/07 2:51p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <sys/select.h>

#include "IRk512Protocol.h"

IRk512Protocol::IRk512Protocol() : m_rk512BytesPerBlock( 128),
m_rk512InitCount( 0), m_rk512ComDev( -1),
m_rk512TnsCounter( 0), m_rk512SrcId( 0)
{
}

IRk512Protocol::~IRk512Protocol()
{
}

/**
 * Open a rk512 serial PLC
 *
 * @param comDev Handle to the communication device to be used for Rk512 communication (i.e. a
 *               socket, a serial port ,etc)
 * @return EOK if successful
 */
int IRk512Protocol::Open( int comDev)
{
    int retVal;

    // Lock so we know our handle is valid
    if ( (retVal=m_rk512Lock.Acquire()) == EOK)
    {
        m_rk512ComDev = comDev;

        m_rk512Lock.Release();
    }

    return( retVal);
}

/**
 * Close an instance of a rk512 PLC
 *
 * @return 0 if successful. Any other value indicates error
 */
int IRk512Protocol::Close()
{
    int retVal = EOK;

    // Lock so we know our handle is valid
    if ( (retVal=m_rk512Lock.Acquire()) == EOK)
    {
        m_rk512ComDev = -1;
        m_rk512Lock.Release();
    }

    return( retVal);
}

/**
 * Initialize data for Rk512 communication
 *
 * @param logger ILogger to use for initializing my logger interface
 */
void IRk512Protocol::Init( ILogger &logger)
{
    m_logger.SetVerboseMask( logger.GetVerboseMask());
    m_logger.SetLocalEcho( logger.GetLocalEcho());
    m_logger.SetProcessName( "Rk512Protocol");
    m_logger.SetAdditionalLogFileName( logger.GetAdditionalLogFileName());
    if ( logger.GetLogStatus())  m_logger.EnableLogger();
}

/**
 * Read data from a Rk512 PLC
 *
 * @param dest   Rk512 destination node ID to which the message is addressed
 * @param buff   Buffer into which the data will be read
 * @param count  Number of 32-bit integers to read
 * @param addr   PLC data block address to read from
 * 
 * @return Number of 32-bit integers read
 */
int IRk512Protocol::Read(uint32_t *buff, uint32_t wordCount, uint8_t readAddress, uint8_t offset)
{
    int             cnt = 5;
    int             good = 0;
    uint8_t         plcData[ 1024];


    m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::Read()- wordCount=0x%02x, readAddress=0x%02x, offset=0x%02x\n", wordCount, readAddress, offset);

    if ( m_rk512Lock.Acquire() == EOK)
    {

        do
        {
            // Send the Rk512 "Read Block" string
            //ews-  the below function also "eats" the ack message.

            if ( BufferRead( wordCount, readAddress, offset) > 0)
            {
                int rspLen = 7 + wordCount*2;    //Expected response length.

                // Get PLC response and convert to binary
                // plcData is the scratchpad for the message
                //if ( CheckBufferRead(plcData, rspLen, &buff[wordCount], wordCount) == 0)
                if ( CheckBufferRead(plcData, rspLen, buff, wordCount) == 0)
                {
                    m_logger.Log( LOG_DEV_DATA, "Read() plcData[0] = 0x%04x, plcData[1] = 0x%04x, plcData[2] = 0x%04x\n", plcData[0], plcData[1], plcData[2]);
                    m_logger.Log( LOG_DEV_DATA, "Read() buff[0] = 0x%04x, buff[1] = 0x%04x, buff[2] = 0x%04x\n", buff[0], buff[1], buff[2]);
                    AckBufferRead();                // Say OK
                    cnt = 0;                            // leave
                    good = 1;
                    break;                              // done
                }
                else
                {
                    m_logger.Log( LOG_ERRORS, "\tError in CheckBufferRead()\n");
                    AckBufferRead();                // say !ok
                    delay(30);
                    cnt--;                              // another failed
                }
            }
            else
            {
                m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::Read()- BufferRead() return <= 0 - sending ack\n");
                AckBufferRead();                    // say !ok
                delay(30);
                cnt--;                                  //   another failed
            }
        }
        while (cnt);                                     // 5 tries


        m_rk512Lock.Release();
    }

    return(good);
}

/**
 * Write data to a Rk512 PLC
 *
 * @param dest   Rk512 destination node ID to which the message is addressed
 * @param buff   Buffer from which the data will be written
 * @param count  Number of 32-bit integers to write
 * @param addr   PLC data block address to write to
 *
 * @return Number of 32-bit integers written
 */
//int IRk512Protocol::Write( uint8_t dest, const uint32_t *buff, uint32_t count, const char *addr)
int IRk512Protocol::Write(const uint32_t *buff, uint32_t wordCount, uint8_t writeAddress, uint8_t offset)
{
    int             cnt = 2;
    int             good = 0;
    uint8_t         plcData[ 1024]; 

    if ( m_rk512Lock.Acquire() == EOK)
    {
        // Try a few times
        do
        {
            m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::Write()\n");

            // Try to write to the PLC
            if ( BufferWrite( wordCount, writeAddress, offset, buff) > 0)
            {
                int junk;
                // Wait for PLC response
                if ( (junk=WaitForAllData(plcData, 1000)) > 0)
                {
                    AckBufferRead();
                    cnt = 0;                            //   leave
                    good = 1;                           //   good
                    break;                              //   done
                }
                else
                {
                    m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::Write WaitForAllData error: returned %d, TNX: $%04X\n",
                                  junk, m_rk512TnsCounter);
                    AckBufferRead();
                    delay(30);
                    cnt--;                              //   another failed
                }
            }
            else
            {
                m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::Write BufferWrite error\n");
                AckBufferRead();
                delay(30);
                cnt--;                                  //   another failed
            }
        }
        while (cnt);

        m_rk512Lock.Release();
    }

    return( good);
}


bool IRk512Protocol::IsResponseMessageValid( uint8_t *buff, int cnt, short byteCnt)
{
    bool retval = true; // assume everything is ok, any test below can invalidate

    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::IsResponseMessageValid() - Enter\n");

    // additional checks for length and transaction code
    if (cnt != byteCnt)
    {
        m_logger.Log( LOG_ERRORS,
                      "\tInvalid Message Length. Expected: %d, Received: %d\n",
                      byteCnt ,cnt);
        retval = false;
    }
    //MAM 2/6/06 - come back to this later
    /*
    if (ValidateCrcRaw( buff, cnt) != 0)
    {
        m_logger.Log( LOG_ERRORS, "\tInvalid checksum\n");
        retval = false;
    }
    */
    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::IsResponseMessageValid() - NOT VALIDATING CRC\n");

    m_logger.Log( LOG_FN_ENTRY,
                  "IRk512Protocol::IsResponseMessageValid() - Exit - Returning: %s\n",
                  retval ? "true" : "false");
    return( retval);
}

/**
 * Receives and validates a PLC Rk512 reply message
 *
 * @param buff      Buffer into which the Rk512 data should be received
 * @param byteCnt   Maximum number of bytes to receive from the PLC
 * @param plcData   Array into which converted binary data should be put
 * @param dataCount Maximum number of PLC words to convert into the plcData array
 * @return 0 if buffer is valid; any other value indicates an error
 */
long IRk512Protocol::CheckBufferRead( uint8_t *buff, short byteCnt, uint32_t *plcData, int count)
{
    register long   totalRead;
    long            msgError = 1;

    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::CheckBufferRead() - waiting for all data\n");
    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::CheckBufferRead() - byteCnt = %d, count = %d\n", byteCnt, count);

    // Wait for all data to be received
    if ( (totalRead=WaitForAllData(buff, byteCnt)) > 0)
    {
        // totalRead holds the number of bytes read !
        // above function cleans up double stuffed DLEc characters
        // Validate the Rk512 data
        if (IsResponseMessageValid(buff, totalRead, byteCnt))
        {
            // Convert Rk512 serial bytes to 32-bit integers
            if ( ExtractData( buff, totalRead, plcData, count) == BEP_STATUS_SUCCESS)
            {
                m_logger.Log( LOG_DEV_DATA, "CheckBufferRead() plcData[0] = 0x%04x, plcData[1] = 0x%04x, plcData[2] = 0x%04x\n", plcData[0], plcData[1], plcData[2]);
                msgError = 0;
            }
            else
            {
                m_logger.Log( LOG_ERRORS, "\tError in ExtractData()\n");
            }
        }
        else
        {
            m_logger.Log( LOG_ERRORS, "\tInvalid response message, not processed.\n");
        }
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tError in WaitForAllData(); %d bytes received\n", totalRead);
    }
    return( msgError);                               // return no_error
}

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
//BufferWrite( wordCount, writeAddress, offset, &buff[wordCount]) > 0

long IRk512Protocol::BufferWrite( short wordCount, short db, uint8_t dw, const uint32_t *buff)
{
    const uint32_t      buffLen = 128;  // Maximum length of any single RK512 Send message
    uint8_t             xmtBuff[ buffLen];
    register uint32_t   nn=0;
    register uint8_t    *pp;
    bool                goodWrite = false;
    char                ackRsp = ENQc;
    char                prefix[ 1024];
    int                 cnt = 0;
    uint8_t             c, bcc;

    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::BufferWrite() SEND\n");        

    const uint8_t   stxOut[] = { STXc}; //(STX = 0x02)    
    sprintf( prefix, "BufferWrite() sending Start code (STX)");
    LogPlcString( LOG_DEV_DATA, prefix, stxOut, 1);

    // Populate transmit buffer
    pp = xmtBuff;        
    bcc = c = 0x00;
    //build message header
    pp[nn++] = 0x00;
    pp[nn++] = 0x00;

    c = 0x41;               //41h="A"=SEND cmd;
    pp[nn++] = c;
    bcc ^= c;               //CRC calc

    c = 0x44;                                       
    pp[nn++] = c;           //data block
    bcc ^= c;

    c = db;             //typical: c = datablock 200 = 0xC8;             
    pp[nn++] = c;       //data block ID
    bcc ^= c;

    c = dw;             //typical: c = offset 0 = 0x00;         
    pp[nn++] = c;       //data offset
    bcc ^= c;

    c = ((wordCount)>>8);    //length (high byte)            
    pp[nn++] = c;
    bcc ^= c;

    c = ((wordCount)&0xff); ; // length (low byte)   28     
    // currently, memory map specifies 28 (0x1C) words
    pp[nn++] = c;
    bcc ^= c;

    pp[nn++] = 0xFF;        // byte number of interprocessor communication flag
    bcc ^= 0xFF;
    pp[nn++] = 0xFF;        // bit number of interprocessor communication flag, CPU number
    bcc ^= 0xFF;

    // Step through buff array and send bytes
    for (int ii=0; ii<wordCount; ii++)
    {
            c = buff[ii] & 0x00FF;
            bcc ^= c;
            pp[nn++] = c;

            if(DLEc == c)
            {
                // Add duplicate DLE
                bcc ^= c;
                pp[nn++] = c;
            }

            c = buff[ii] >> 8;
            bcc ^= c;
            pp[nn++] = c;
            
            if(DLEc == c)
            {
                // Add duplicate DLE
                bcc ^= c;
                pp[nn++] = c;
            }
            m_logger.Log( LOG_DEV_DATA, "Word %02d: 0x%02x%02x", ii, pp[nn - 1], pp[nn - 2]);
    }

    m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::BufferWrite() 3\n", goodWrite);

    //end code - DLE ETX
    c = DLEc;         
    pp[nn++] = c;
    bcc ^= c;

    c = ETXc;         
    pp[nn++] = c;
    bcc ^= c;

    // skip first two and last two
    //pp[nn++] = (BccCalc ( &pp[2] , nn - 4 ));

    // TEMP
    pp[nn++] = bcc; //final CRC value

    //come back to the crc check later
    // Make sure our CRC is valid before sending
    //if ( ValidateCrc( xmtBuff, nn) == 0) 
    //{               
    cnt = 0;
    ackRsp = ENQc;

    sprintf( prefix, "Sending (%d bytes, %d calc)\n", nn, buffLen);
    LogPlcString( LOG_DEV_DATA, prefix, pp, nn);        

    // Attempt transmission 2 times
    for (int ii=0; ii<2 && goodWrite == false; ii++)
    {
        // Begin connection setup
        // Send start code STX (0x02)
        if (write( m_rk512ComDev, stxOut, sizeof( stxOut)) > 0)
        {
            // Wait for positive acknowledgment DLE (0x10)
            ackRsp = WaitForAck(1);            
            if (ackRsp == DLEc)
            {
                delay(30);
                // Saw DLE, begin sending User Data
                if ( write( m_rk512ComDev, pp, nn) > 0)
                {
                    // Wait for positive acknowledgment DLE (0x10)
                    ackRsp = WaitForAck(2);                               
                    if (ackRsp == DLEc)
                    {
                        // Saw DLE, write complete
                        goodWrite = true;
                        m_logger.Log( LOG_DEV_DATA, "\tIRk512Protocol::BufferWrite valid write");
                    }
                }
                else
                {
                    // return error, attempt another send if retries remain
                    goodWrite = false;
                    m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::BufferWrite (User Data) ERROR: %s", strerror( errno));
                }
            }
            else
            {
                // return error, attempt another send if retries remain
                goodWrite = false;
            }
        }
        else
        {
            goodWrite = false;
            m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::BufferWrite (Connection Setup) ERROR: %s", strerror( errno));
        }

    }

    return(goodWrite ? 1 : 0);
}

/**
 * Calculates the number of data bytes in a PLC write message taking into
 * consideration duplicate DLE's
 *
 * @param plcOut  Raw data buffer to be written
 * @param buffLen Number of items in the plcOut array
 * @return Number of items in the data field of a PLC write message for the given
 *         PLC data
 */
uint32_t IRk512Protocol::CalculateBufferLen( const uint32_t *plcOut, uint32_t buffLen)
{
    register uint32_t nn, retVal = buffLen;
    // Count the extra DLE characters in the data to write
    for ( nn=0; nn<buffLen; nn++)
    {
        if ( (plcOut[ nn] & 0x00FF) == 0x0010)   retVal++;
        if ( (plcOut[ nn] & 0xFF00) == 0x1000)   retVal++;
    }

    return( retVal);
}

/**
 * Reads integer data from the PLC
 *
 * @param destId  Rk512 destination node ID to which the message is addressed
 * @param byteCnt Number of integers to read
 *      (do NOT pass in 0x08 or 0x10 as byteCnt - bad things could happen)
 * @param plcAddr PLC Address from which the data should be read
 * @return Number of integers read
 */
long IRk512Protocol::BufferRead( short wordCount, short db, uint8_t dw)
{
    const uint32_t      buffLen = 20;   //?????
    uint8_t             xmtBuff[ buffLen];
    register uint32_t   nn=0;
    register uint8_t    *pp;
    int                 goodWrite = 0;
    char                ackRsp = ENQc;
    char                prefix[ 1024];
    int                 cnt = 0;
    uint8_t             c, bcc;

    m_logger.Log( LOG_FN_ENTRY,"IRk512Protocol::BufferRead() Fetch\n");        

    const uint8_t   stxOut[] = { STXc}; //(STX = 0x02)    
    sprintf( prefix, "BufferRead() sending Start code (STX)");
    LogPlcString( LOG_DEV_DATA, prefix, stxOut, 1);

    //send start code (0x02) and wait for DLE ack (0x10) up to 3 times
    while ( (ackRsp != DLEc) && (cnt++ < 3))
    {
        // Try to send Start code (STX) to the PLC
        if ((goodWrite = write( m_rk512ComDev, stxOut, sizeof( stxOut)) > 0))
        {
            ackRsp = WaitForAck(1);            
            // If PLC responds with anything other than DLE (0x10)
            if (ackRsp != DLEc)
            {
                // return error
                goodWrite = 0;
            }
        }
        else if ( goodWrite == -1)
        {
            m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::BufferRead ERROR: %s", strerror( errno));
        }
    }

    m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::BufferRead() goodWrite = %d\n", goodWrite);

    //if we sent out the STX and got back a DLE then
    //send the rest of the fetch message
    if (goodWrite > 0)
    {
        pp = xmtBuff;        
        bcc = c = 0x00;
        //build message header
        pp[nn++] = 0x00;
        pp[nn++] = 0x00;

        c = 0x45;               //45h="E"=FETCH cmd;         
        pp[nn++] = c;
        bcc ^= c;               //CRC calc

        c = 0x44;                                       
        pp[nn++] = c;           //data block
        bcc ^= c;

        c = db;         
        //c = 0xC9;              
        pp[nn++] = c;        //data block ID
        bcc ^= c;

        c = dw;                                         
        //c = 0x00;         
        pp[nn++] = c;        //data offset
        bcc ^= c;

        c = ((wordCount)>>8);    //length (high byte)     
        //c = 0x00;        
        pp[nn++] = c;
        bcc ^= c;

        c = ((wordCount)&0xff); ; // length (low byte)      
        //c = 0x1C;
        // currently, memory map specifies 28 (0x1C) words
        pp[nn++] = c;
        bcc ^= c;

        pp[nn++] = 0xFF;        // byte number of interprocessor communication flag
        pp[nn++] = 0xFF;        // bit number of interprocessor communication flag, CPU number
        //no crc???

        //end code - DLE ETX
        c = DLEc;         
        pp[nn++] = c;
        bcc ^= c;

        c = ETXc;         
        pp[nn++] = c;
        bcc ^= c;

        // skip first two and last two
        //pp[nn++] = (BccCalc ( &pp[2] , nn - 4 ));

        // TEMP
        pp[nn++] = bcc; //final CRC value

        //come back to the crc check later
        // Make sure our CRC is valid before sending
        //if ( ValidateCrc( xmtBuff, nn) == 0) 
        //{               
        cnt = 0;
        ackRsp = ENQc;

        sprintf( prefix, "Sending (%d bytes, %d calc)\n", nn, buffLen);
        LogPlcString( LOG_DEV_DATA, prefix, pp, nn);        

        //send the fetch message and wait for a DLE STX ack
        while ( (ackRsp != DLEc) && (cnt++ < 3))
        {
            if ( (goodWrite = write( m_rk512ComDev, pp, nn)) > 0)
            {
                ackRsp = WaitForAck(2);                
                // If first byte of PLC response is anything other than DLE ACK               
                if (ackRsp != DLEc)
                {
                    // return error
                    goodWrite = 0;
                }
            }
            else if ( goodWrite == -1)
            {
                m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::BufferRead ERROR: %s", strerror( errno));
            }
        }
        //}

    }   //end if (goodWrite > 0)

    return(goodWrite);
}

/**
 * Acknowledge a successful read operation by sending ACK to the PLC
 */
void IRk512Protocol::AckBufferRead()
{
    //rk512 ack is single DLE (0x10)
    //const uint8_t   plcOut[] = { DLEc, ACKc};
    const uint8_t   plcOut[] = { DLEc};
    write( m_rk512ComDev, plcOut, sizeof( plcOut));
}

/**
 * Notify the PLC of a failed read operation by sending NAK to the PLC
 */
void IRk512Protocol::NAkBufferRead()
{
    const uint8_t   plcOut[] = { DLEc, NAKc};

    write( m_rk512ComDev, plcOut, sizeof( plcOut));
}

/**
 * Request re-transmission of the last read operation
 */
void IRk512Protocol::ENQBufferRead()
{
    const uint8_t   plcOut[] = { DLEc, ENQc};

    write( m_rk512ComDev, plcOut, sizeof( plcOut));
}

/**
 * Calculates the CRC for a PLC serial transaction
 * Do NOT use this for received messages, as the DLEc-DLEc pairs
 * have already been removed from the message.
 * Use BccCalcRaw() instead
 *
 * @param pp     Data array to calculate the CRC for
 * @param len    Number of bytes in the data array
 *
 * @return CRC for the given data array
 */
unsigned short IRk512Protocol::BccCalc( register uint8_t *pp, register short len)
{
    unsigned short  crc1=0x0000;
    register short  xx;

    for (xx=0; xx<len; xx++)                     // do this many
    {
        if ( (pp[xx] == 0x10) && (pp[xx+1] == 0x10) ) xx++;
        crc1 += pp[xx];
    }
    crc1 &= 0x00FFL;                // xor byte with crc
    //ews- fixed next line
    crc1 = (0x100 - crc1) & 0xFF;
    return(crc1);
}

/**
 * Calculates the CRC for a PLC serial transaction
 * Do NOT use this for received messages, as the DLEc-DLEc pairs
 * have already been removed from the message.
 * Use BccCalcRaw() instead
 *
 * @param pp     Data array to calculate the CRC for
 * @param len    Number of bytes in the data array
 *
 * @return CRC for the given data array
 */
unsigned short IRk512Protocol::BccCalcRaw( register uint8_t *pp, register short len)
{
    unsigned short crc1=0x0000;
    register short xx;

    for (xx=0; xx<len; xx++)                     // do this many
    {
        // if( (pp[xx] == 0x10) && (pp[xx+1] == 0x10) ) xx++;
        crc1 += pp[xx];
    }
    crc1 &= 0x00FFL;                // xor byte with crc
    //ews- fixed next line
    crc1 = (0x100 - crc1) & 0xFF;
    return(crc1);
}

/**
 * Waits for all reply data from the PLC
 *
 * @param rxBuff Buffer to receive the PLC data into
 * @param count  Number of bytes in the rxBuff paramater
 * @return Number of bytes read
 */
int IRk512Protocol::WaitForAllData( uint8_t *rxBuff, int count)
{
    int         totalRead=0, bytesRead=0, bytesRemaining=2;
    uint64_t    timeOut = mSEC_nSEC( 500);
    uint8_t     curData=0, lastData=0;

    m_logger.Log( LOG_FN_ENTRY, "IRk512Protocol::WaitForAllData() - Enter - Expecting: %d bytes\n", count);
    // Loop until all expected data has been received
    while ( (totalRead < count) && (bytesRemaining > 0))
    {
        // Set a timeout on the read operation to try and detect the end
        TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_REPLY, NULL, &timeOut, NULL);
        bytesRead = read( m_rk512ComDev, &curData, 1);
        if ( bytesRead > 0)
        {
            //ews- at this point, bytesRead == 1
            // Either the first char or not a duplicate DLE, so
            //  put into caller's buffer
            //ews- looks like the above comment is out of date, always put in
            // will be removed below if necessary
            rxBuff[ totalRead] = curData;
            totalRead += bytesRead;

            switch ( curData)
            {
            case DLEc:
                // If this is a duplicate DLE
                if ( lastData == DLEc)
                {
                    // Remove the 2nd DLE from the caller's buffer
                    if ( totalRead)  totalRead--;

                    // Make sure that, when this byte gets shifted into lastData, that it
                    //  does not get interpreted as a DLEc. This will allow the ETXc, ACKc
                    //  and NAKc cases below to properly to identify the end of the message
                    curData = DLEc ^ 0xFF;
                }
                else
                {
                    if (bytesRemaining == 1)
                    {
                        bytesRemaining = 0;
                    }
                }
                break;
            case ETXc:
                // a <DLE><ETX> sequence indicates that the next character
                //  is the last character in the packet (the CRC byte). This
                //  is only true if the <DLE> was not a duplicate <DLE>
                if ( lastData == DLEc)
                {
                    // Mark the next byte as the last byte
                    bytesRemaining = 1;
                }
                else
                {
                    if (bytesRemaining == 1)
                    {
                        bytesRemaining = 0;
                    }
                }
                break;
            case ACKc:
                // If the last character was a DLE, this is the end of an ACKc message
                if ( lastData == DLEc)
                {
                    bytesRemaining = 0;
                }
                else
                {
                    if (bytesRemaining == 1)
                    {
                        bytesRemaining = 0;
                    }
                }
                break;
            case NAKc:
                // If the last character was a DLE, this is the end of an NAKc message
                if ( lastData == DLEc)
                {
                    bytesRemaining = 0;
                }
                else
                {
                    if (bytesRemaining == 1)
                    {
                        bytesRemaining = 0;
                    }
                }
                break;

                //MAM 2/8/06
            case STXc:
                // If the last character was a DLE, this is the start of a response message
                if ( lastData == DLEc)
                {
                    bytesRemaining = 0;
                    //we need to respond to STX (0X02) with a DLE (0X10)                        
                    const uint8_t   stxOut[] = { DLEc};
                    char    prefix[ 1024];
                    int bytesWritten = 0;
                    sprintf( prefix, "IRk512Protocol::WaitForAllData(): got DLE/STX - sending DLE resp");
                    LogPlcString( LOG_DEV_DATA, prefix, stxOut, 1);
                    bytesWritten = write( m_rk512ComDev, stxOut, sizeof( stxOut));
                    m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::WaitForAllData(): bytesWritten = %X \n", bytesWritten);                        
                }
                else
                {
                    if (bytesRemaining == 1)
                    {
                        bytesRemaining = 0;
                    }
                }
                break;

            default:
                // If we are only expecting one more byte (the CRC byte)
                if ( bytesRemaining == 1)
                {
                    // Say no more data to come
                    bytesRemaining = 0;
                }
                break;
            }

            // Hold onto this latest char
            lastData = curData;
        }
        else if ( bytesRead < 0)
        {
            m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::WaitForAllData read ERROR(%d): %s\n", errno, strerror( errno));
            m_logger.Log( LOG_ERRORS, "\t    Expecting: %d received: %d\n", count, totalRead);

            if (totalRead > 0)
                LogPlcString( LOG_ERRORS, "Current PLC data", rxBuff, totalRead);
            break;
        }
        else
        {   //ews- bytesRead = 0: must have timed out
            break;
        }
    }

    // If we received any data
    if ( totalRead)
    {
        LogPlcString( LOG_DETAILED_DATA | LOG_DEV_DATA, "WaitForAllData Received", rxBuff, totalRead);
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tIRk512Protocol::WaitForAllData read (%d): %s\n", errno, strerror( errno));
    }

    m_logger.Log( LOG_FN_ENTRY, "IRk512Protocol::WaitForAllData - Exit - Returning %d bytes for parent to process \n", totalRead);
    return( totalRead);
}

/**
 * Validates a Rk512 CRC
 *
 * @param rk512Data Rk512 serial string to validate
 * @param rk512Size number of bytes in the Rk512 serial string
 * @return 0 if CRC is valid, 1 if it is invalid
 */
int IRk512Protocol::ValidateCrc( uint8_t* rk512Data, int rk512Size)
{
    register uint8_t   *pp = rk512Data;
    register long       nn = 1, cnt = rk512Size;
    long                msgError = 1;
    unsigned int        stx_ptr=0, etx_ptr=0;
    uint8_t             crc;

    // Find message frame
    while (nn < cnt)
    {
        // Find first occurrence of <10> <02>
        if ((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))    stx_ptr = nn;
        // Find last occurrence of <10> <03>
        if ((pp[nn] == ETXc) && (pp[nn-1] == DLEc))                      etx_ptr = nn;

        nn++;
    }

    cnt = (etx_ptr - 2) - (stx_ptr + 1) + 1;
    // If we found a valid frame
    if ( (cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(rk512Size-1)))
    {
        // If CRC is valid
        crc = (uint8_t)BccCalc( &pp[stx_ptr+1], cnt);
        if ((pp[etx_ptr+1]) == crc)
        {
            msgError = 0;
        }
        else
        {
            m_logger.Log( LOG_ERRORS, "\tInvalid CRC (%d, %d, %d) $%02hhX != $%02hhX\n",
                          rk512Size, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);
            LogPlcString( LOG_ERRORS, "\tCRC Data", rk512Data, rk512Size);
        }
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tInvalid Rk512 string (%d, %d, %d): Invalid Rk512 frame\n",
                      rk512Size, stx_ptr, etx_ptr);
        LogPlcString( LOG_ERRORS, "\tFrame not found", rk512Data, rk512Size);
    }
    return( msgError);                               // return no_error
}

/**
 * Validates a Rk512 CRC
 *
 * @param rk512Data Rk512 serial string to validate
 * @param rk512Size number of bytes in the Rk512 serial string
 * @return 0 if CRC is valid, 1 if it is invalid
 */
int IRk512Protocol::ValidateCrcRaw( uint8_t* rk512Data, int rk512Size)
{
    register uint8_t   *pp = rk512Data;
    register long     nn = 1, cnt = rk512Size;
    long              msgError = 1;
    unsigned int      stx_ptr=0, etx_ptr=0;
    uint8_t           crc;

    // Find message frame
    while (nn < cnt)
    {
        // Find first occurrence of <10> <02>
        if ((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))   stx_ptr = nn;
        // Find last occurrence of <10> <03>
        if ((pp[nn] == ETXc) && (pp[nn-1] == DLEc))                  etx_ptr = nn;

        nn++;
    }

    cnt = (etx_ptr - 2) - (stx_ptr + 1) + 1;
    // If we found a valid frame
    if ((cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(rk512Size-1)))
    {
        // If CRC is valid
        crc = (uint8_t)BccCalcRaw( &pp[stx_ptr+1], cnt);
        if ((pp[etx_ptr+1]) == crc)
        {
            msgError = 0;
        }
        else
        {
            m_logger.Log( LOG_ERRORS, "\tInvalid CRC (%d, %d, %d) $%02hhX != $%02hhX\n",
                          rk512Size, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);
            LogPlcString( LOG_ERRORS, "\tCRC Data", rk512Data, rk512Size);
        }
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tInvalid Rk512 string (%d, %d, %d): Invalid Rk512 frame\n",
                      rk512Size, stx_ptr, etx_ptr);
        LogPlcString( LOG_ERRORS, "\tFrame not found", rk512Data, rk512Size);
    }
    return( msgError);                               // return no_error
}

/**
 * Extracts PLC data from a Rk512 serial string
 *
 * @param rk512Data   Rk512 serial string to extract data from
 * @param rk512Size   Number of bytes in the Rk512 serial string
 * @param plcData   Array into which converted data will be written
 * @param dataCount Number of elements in the plcData array
 * @return Number of 32-bit integers converted and written to the plcData array
 */
int IRk512Protocol::ExtractData( uint8_t *rk512Data, int rk512Size, uint32_t *plcData, int wordCount)
{
    int             status = BEP_STATUS_SUCCESS;
    register long   nn;

    nn = 0;
    // Find message frame
    while (nn < 4)
    {
        if (rk512Data[nn] != NULc)      status = BEP_STATUS_ERROR;
        nn++;
    }

    // if valid data bufferthen move string into variables
    if ( (plcData != NULL) && ( wordCount > 0) && (status == BEP_STATUS_SUCCESS))
    {

        nn = 0;
        m_logger.Log( LOG_ERRORS, "Begin Extracting Data\n");
        for (int ii=2; ii < wordCount + 2; ii++)
        {
            plcData[ii-2] = rk512Data[ii*2] | rk512Data[ii*2 + 1] << 8;

            m_logger.Log( LOG_ERRORS, "Word %02d: 0x%04x",ii-2, plcData[ii-2]);
        }

    }
    else
    {
        status = BEP_STATUS_ERROR;
    }
    return( status);
}

/**
 * Wait for the PLC to ACK/NAK our commands
 *
 * @return NAKc if NAK received
 *         ACKc if ACK recevied
 *         ENQc if ENQ received
 */
//char IRk512Protocol::WaitForAck()
char IRk512Protocol::WaitForAck(int numbytes)
{
    char    readCount, retVal = NAKc;
    uint8_t plcData[ 10];
    bool    keepWaiting=true;
    int     enqCount=0, errCnt=0, readIdx=0;

    // Wait until and ACK or other recognized command comes in
    while ( keepWaiting)
    {
        // Initialize to junk
        plcData[ 0] = plcData[ 1] = DLEc ^ 0xFF;        
        //if ( (readCount=WaitForAllData(&plcData[ readIdx], 2)) > 0)
        //if ( (readCount=WaitForAllData(&plcData[ readIdx], 1)) > 0)
        if ( (readCount=WaitForAllData(&plcData[ readIdx], numbytes)) > 0)
        {
            readIdx += readCount;
            //if ( readIdx > 1)
            if ( readIdx > 0)
            {
                keepWaiting = false;
                // If this looks like a command
                if ( plcData[ 0] == DLEc)
                {
                    m_logger.Log( LOG_DEV_DATA, "\tGot DLE ACKc\n");
                    keepWaiting = false;
                }
                else
                {
                    tcflush( m_rk512ComDev, TCIOFLUSH);
                    if ( plcData[ 0] == NAKc)
                    {
                        m_logger.Log( LOG_ERRORS, "\tGot NAK while waiting for DLE ACK\n");
                    }
                    else
                    {
                        if ( plcData[ 0] == NAKc)
                        {
                            m_logger.Log( LOG_ERRORS, "\tGot NAK while waiting for DLE ACK\n");
                        }
                        else if ( plcData[ 0] == ENQc)
                        {
                            m_logger.Log( LOG_ERRORS, "\tGot ENQ while waiting for DLE ACK\n");
                            // Not sure if Siemens PLC behaves the same way, but I'm going to leave
                            // this in for now.
                            // This is the PLC's way of saying that we haven't ACK'd his last message
                            // If this hasn't happened too many time, try to ACK the PLC
                            if ( enqCount++ < 3)
                            {
                                AckBufferRead();
                                keepWaiting = true;

                                // Reset our read pointer back to the start of the buffer
                                readIdx = 0;
                            }
                        }
                        else
                        {
                            m_logger.Log( LOG_ERRORS, "\tGot unknown command while waiting for ACKc: %02hhX\n", plcData[ 1]);
                        }

                    }

                }
                retVal = plcData[ 0];
            } //end if ( readIdx > 1)
            else
            {
                tcflush( m_rk512ComDev, TCIOFLUSH);
                LogPlcString( LOG_ERRORS, "\tERROR did not get ACK", plcData, readIdx);
            }
        }
        // If 2nd error, leave
        else if ( errCnt++)
        {
            keepWaiting = false;
        }
    }

    m_logger.Log( LOG_DEV_DATA, "IRk512Protocol::WaitForAck()- return value = %X \n",retVal);
    return( retVal);
}

/**
 * Logs a PLC response string
 *
 * @param logMask Our logger interface verbosity bit which must be set in order to log
 *                the string
 * @param prefix  String to prepend to the PLC data string when logged
 * @param plcData The PLC data string to be logged
 * @param dataLen The length of the PLC data string
 */
void IRk512Protocol::LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
{
    uint32_t    ii;

    // If any data to print and correct verbosity
    if ( m_logger.GetVerboseMask() & logMask)
    {
        std::string logBuff("");
        char        junk[ 5];

        if ( prefix != NULL)     logBuff = std::string( prefix) + std::string( ": ");

        for ( ii=0; ii<dataLen; ii++)
        {
            sprintf( junk,  "$%02hhX ", plcData[ ii]);
            logBuff += std::string( junk);
        }

        if ( logBuff.length() > 0)
        {
            logBuff += std::string( "\n");
            m_logger.Log( logMask, logBuff.c_str());
        }
    }
}

