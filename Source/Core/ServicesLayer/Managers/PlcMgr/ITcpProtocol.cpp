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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/ITcpProtocol.cpp $
// 
// 8     5/24/06 2:54p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:18p Rwiersem
// Added LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.
//
// 7     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:14p Bmeinke
// Added m_terminate flag to signal the thread when it should stop
// Added an Initialize( XmlNode*) method to read config file data
// CHanged to more closely resemble the NCA TCP implementation
//
// 6     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 5     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     2/16/05 9:29p Cward
// UPdated TCP Protocol with many fixes found in the field.
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
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
// 5     6/26/03 1:41p Bmeinke
// Changed a couple logger statments to LogPlcString()
// Fixed potential bug where the wrong string length was passed into the
// call to LogPlcString() inside WaitForAck()
//
// 4     6/24/03 8:56a Khykin
// Brians latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 3     6/20/03 9:02a Khykin
// Updated Ford code.
//
// 2     6/18/03 6:20p Bmeinke
// Changed the timeout inside WaitForAllData() from 5000ms to 500ms
// Fixed a bug in the the call to CheckBufferRead() inside Read(): &buff[
// ii*wordsToUpdate] => &buff[ ii*(m_tcpBytesPerBlock/2)]
// WaitForAck only tries to read an ACK response twice (was waiting a LONG
// time if no response cameback)
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 7     4/16/03 1:14p Bmeinke
// Modified WaitForAllData() to look for the end of a message based on the
// value of received data insteadof relying on inter-byte timeouts
// Added LogPlcString() method to output a PLC serial data string via the
// logger interface
// General cleanup: removed unused methods, removed commented out blocks
//
// 6     4/10/03 6:41p Bmeinke
// Inside Read() and Write(), reformat the plcAddr buffer sent to the PLC
// each time through the loop (in case of comm errors)
// Added small delays after failed serial transmissions
// Fixed the transaction counter algorithm when DLE characters appear in
// either the upper or lower word of the transaction counter value
//
// 5     3/26/03 10:13p Bmeinke
// Set the name of the additional log file in the Init() method
//
// 4     3/12/03 12:39a Bmeinke
// Initial machine testing for Ford Chicago
// Added a ILogger argument to Init() to initialize our logger interface
// Changed WaitForAllData() to read serial data 1 byte at a time to filter
// duplicate DLE characters
//
// 3     2/14/03 10:26a Bmeinke
// First working version
//
// 2     1/18/03 3:48p Bmeinke
// Maintenance checkin (not fully tested)
//
// 1     12/12/02 1:12p Bmeinke
// Initial check-in
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <sys/select.h>

#include "ITcpProtocol.h"

ITcpProtocol::ITcpProtocol() :
m_tcpBytesPerBlock( 100), m_tcpInitCount( 0), m_tcpComDevRead( -1), m_tcpComDevWrite( -1),
m_terminate( false)
{
}

ITcpProtocol::~ITcpProtocol()
{
}

/**
 * Open a TCP serial PLC
 *
 * @param comDev Handle to the communication device to be used for TCP communication (i.e. a
 *               socket, a serial port ,etc)
 * @param src    TCP node ID to be used as the source identifier in TCP messages
 * @return EOK if successful
 */
int ITcpProtocol::Open( int comDev, uint8_t src)
{
    int retVal;

    m_terminate = false;
    m_logger.Log(LOG_FN_ENTRY, "ITcpProtocol::Open(%d, %d)", comDev, src);

    // Lock so we know our handle is valid
    if ( (retVal=m_tcpLock.Acquire()) == EOK)
    {
        m_tcpComDevRead = m_tcpComDevWrite = comDev;

        m_tcpLock.Release();
    }

    return( retVal);
}

int ITcpProtocol::OpenSeparateRW( int comDevRead, int comDevWrite, uint8_t src)
{
    int retVal;

    m_terminate = false;
    m_logger.Log(LOG_FN_ENTRY, "ITcpProtocol::OpenSeparateRW(%d, %d, %d)", comDevRead, comDevWrite, src);

    // Lock so we know our handle is valid
    if ( (retVal=m_tcpLock.Acquire()) == EOK)
    {
        m_tcpComDevRead = comDevRead;
        m_tcpComDevWrite = comDevWrite;

        m_tcpLock.Release();
    }

    return( retVal);
}

/**
 * Close an instance of a TCP PLC
 *
 * @return 0 if successful. Any other value indicates error
 */
int ITcpProtocol::Close()
{
    int retVal = EOK;

    m_terminate = true;

    // Lock so we know our handle is valid
    if ( (retVal=m_tcpLock.Acquire()) == EOK)
    {
        m_tcpComDevRead = -1;
        m_tcpComDevWrite = -1;
        m_tcpLock.Release();
    }

    return( retVal);
}

/**
 * Initialize data for TCP communication
 *
 * @param logger ILogger to use for initializing my logger interface
 */
void ITcpProtocol::Init( ILogger &logger)
{
    m_logger.SetVerboseMask( logger.GetVerboseMask());
    m_logger.SetLocalEcho( logger.GetLocalEcho());
    m_logger.SetProcessName( "TcpSocketService");
    m_logger.SetAdditionalLogFileName( logger.GetAdditionalLogFileName());
    if ( logger.GetLogStatus())  m_logger.EnableLogger();
}

/**
 * Initialize data for TCP communication from a configuration node
 *
 * @param tcpConfig Configuration node to read the TCP parameters from
 */
void ITcpProtocol::Initialize( const XmlNode *tcpConfig)
{
    // load the additional configuration items needed by the ITcpProtocol.
    LoadAdditionalConfigurationItems(tcpConfig);
}

void ITcpProtocol::LoadAdditionalConfigurationItems(const XmlNode *tcpConfig)
{
    try
    {
        m_logger.Initialize( tcpConfig);
    }
    catch ( ...)
    {
    }

    try
    {
        m_tcpBytesPerBlock = atoi( tcpConfig->getChild("BlockSize")->getValue().c_str());
    }
    catch ( ...)
    {
        m_tcpBytesPerBlock = 100;
    }
    try
    {
        string tcpType = tcpConfig->getChild("TcpType")->getValue();
        if(tcpType == "AB")
            m_tcpType = AB_TYPE;
        else if(tcpType == "Siemens")
            m_tcpType = S7_TYPE;
        else
            m_tcpType = 0;
    }
    catch (...)
    {
    }
}

/**
 * Read data from a TCP PLC
 *
 * @param dest   TCP destination node ID to which the message is addressed
 * @param buff   Buffer into which the data will be read
 * @param count  Number of 32-bit integers to read
 * @param addr
 *
 * @return true if PLC data was read successfully
 */
bool ITcpProtocol::Read( uint8_t dest, uint32_t *buff, uint32_t count, const char *addr)
{
    const int       inBytes = 2*count;
    int             loopCount = inBytes / m_tcpBytesPerBlock;
    int             loopIdx=0, wordsToUpdate=m_tcpBytesPerBlock/2;
    int             cnt=5;
    int             byteCount, wordIdx=m_tcpBytesPerBlock/2;
    char            plcAddr[ 20];
    uint8_t         plcData[ 1024];
    bool            good = true;

    // Loop 1 additional time if we need to read a partial block of data
    if ( (loopCount*m_tcpBytesPerBlock) < inBytes)   loopCount++;

    m_logger.Log( LOG_DEV_DATA, "Enter ITcpProtocol::Read(), Bytes: %d, Block Size: %d, Loops: %d\n",
                  inBytes, m_tcpBytesPerBlock, loopCount);

    if ( m_tcpLock.Acquire() == EOK)
    {
        // Send all blocks until we encounter errors or someone closes us
        do
        {
            // Assume error
            good = false;

            // Calculate how many bytes to read
            byteCount = m_tcpBytesPerBlock;                 // 50 Bytes per block max
            if (loopIdx == (loopCount-1))                            // if last block to process
            {
                byteCount = inBytes % m_tcpBytesPerBlock;   // calc remaining bytes
                wordsToUpdate = count;
            }

            cnt = 5;
            while ( (cnt-- > 0) && (false == m_terminate) && (false == good))
            {
                // Calculate Address to Read
                sprintf(plcAddr,"%s:%d", addr, loopIdx*wordIdx);        // Base addr is N16:00

                m_logger.Log( LOG_DEV_DATA, "ITcpProtocol::Read( %d, %d, %s, %d)\n", loopIdx,
                              loopCount, plcAddr, wordsToUpdate);

                // Send the TCP "Read Block" string
                //ews-  the below function also "eats" the ack message.
                if ( BufferRead( dest, byteCount/2, plcAddr) > 0)
                {
                    int rspLen = 10 + byteCount;

                    // Get PLC response and convert to binary
                    // plcData is the scratchpad for the message
                    if ( CheckBufferRead(plcData, rspLen, &buff[ loopIdx*(m_tcpBytesPerBlock/2)], wordsToUpdate) == 0)
                    {
                        // How many words left to update
                        count -= wordsToUpdate;
                        // Tell PLC his message was good
                        //AckBufferRead();
                        // Send request for next block
                        cnt = 0;
                        good = true;
                    }
                    else
                    {
                        m_logger.Log( LOG_ERRORS, "\tError in CheckBufferRead( %s)\n", plcAddr);
                        //AckBufferRead();                // say !ok
                        delay(30);
                    }
                }
                else
                {
                    //AckBufferRead();                    // say !ok
                    delay(30);
                }
            }
        } while ( (++loopIdx < loopCount) && (true == good) && (false == m_terminate));

        m_tcpLock.Release();
    }
    else
    {
        good = false;
    }

    m_logger.Log( LOG_DEV_DATA, "Exit ITcpProtocol::Read(), status=%s\n", good == true ? "true" : "false");

    return(good);
}

/**
 * Write data to a TCP PLC
 *
 * @param dest   TCP destination node ID to which the message is addressed
 * @param buff   Buffer from which the data will be written
 * @param count  Number of 32-bit integers to write
 * @param addr   PLC Address to write our data to
 *
 * @return true if data was written successfully
 */
bool ITcpProtocol::Write( uint8_t dest, const uint32_t *buff, uint32_t count, const char *addr)
{
    const int       outBytes = 2*count;
    int             junk, loopCount = outBytes / m_tcpBytesPerBlock;
    int             loopIdx=0, bytesWritten=0, wordIdx=m_tcpBytesPerBlock/2;
    int             cnt = 5;
    short           byteCount;
    char            plcAddr[ 20];
    uint8_t         plcData[ 1024];
    bool            good = true;

    if ( (2*count) % m_tcpBytesPerBlock) loopCount++;

    if ( m_tcpLock.Acquire() == EOK)
    {
        // Send all blocks until we encounter errors or someone closes us
        do
        {
            // Assume error
            good = false;

            // Calculate how mant bytes to read
            byteCount = m_tcpBytesPerBlock;
            // if last block to process
            if (loopIdx == (loopCount-1))
            {
                // calc remaining bytes
                byteCount = outBytes % m_tcpBytesPerBlock;
            }

            // Try a few times to send this block
            cnt = 5;
            while ( (cnt-- > 0) && (false == good) && (false == m_terminate))
            {
                // Calculate Address to write to
                sprintf(plcAddr,"%s:%d", addr, loopIdx*(wordIdx));

                m_logger.Log( LOG_DEV_DATA, "ITcpProtocol::Write( %d, %d, %s, %d)\n", loopIdx, loopCount, plcAddr, byteCount);

                // Try to write to the PLC
                if ( (bytesWritten=BufferWrite( dest, byteCount/2, &buff[ loopIdx*wordIdx], plcAddr)) > 0)
                {
                        cnt = 0;
                        good = true;
//                     // Wait for PLC response                                                                   //
//                     if ( (junk=WaitForAllData(plcData, 1000)) > 0)                                             //
//                     {                                                                                          //
//                         // Tell PLC his message was good                                                       //
//                         //AckBufferRead();                                                                     //
//                         // Send request for next block                                                         //
//                         cnt = 0;                                                                               //
//                         good = true;                                                                           //
//                     }                                                                                          //
//                     else                                                                                       //
//                     {                                                                                          //
//                         m_logger.Log( LOG_ERRORS, "\tITcpProtocol::Write WaitForAllData error: returned %d\n", //
//                                       junk);                                                                   //
//                         //AckBufferRead();                                                                     //
//                         delay(30);                                                                             //
//                     }                                                                                          //
                }
                else
                {
                    m_logger.Log( LOG_ERRORS, "\tITcpProtocol::Write BufferWrite error\n");
                    //AckBufferRead();
                    delay(30);
                }
            }
        } while ( (++loopIdx < loopCount) && (true == good) && (false == m_terminate));
        m_tcpLock.Release();
    }
    else
    {
        good = false;
    }

    return( good);
}


// bool ITcpProtocol::IsResponseMessageValid( uint8_t *buff, int cnt, short byteCnt)    //
// {                                                                                    //
//     bool retval = true; // assume everything is ok, any test below can invalidate    //
//     unsigned int localTNS, plcTNS;                                                   //
//     const int TNSOffset = 6;                                                         //
//                                                                                      //
//     m_logger.Log( LOG_FN_ENTRY,"ITcpProtocol::IsResponseMessageValid() - Enter\n");  //
//                                                                                      //
//     // additional checks for length and transaction code                             //
//     localTNS = GetTNSCntr();                                                         //
//     plcTNS = buff[TNSOffset+1] * 0x100 + buff[TNSOffset];                            //
//     if (plcTNS != localTNS)                                                          //
//     {                                                                                //
//         m_logger.Log( LOG_ERRORS,                                                    //
//                       "\tInvalid Transaction number. Sent: %04X, Received: %04X\n",  //
//                       localTNS, plcTNS);                                             //
//         retval = false;                                                              //
//     }                                                                                //
//     if (cnt != byteCnt)                                                              //
//     {                                                                                //
//         m_logger.Log( LOG_ERRORS,                                                    //
//                       "\tInvalid Message Length. Expected: %d, Received: %d\n",      //
//                       byteCnt ,cnt);                                                 //
//         retval = false;                                                              //
//     }                                                                                //
//     if (ValidateCrcRaw( buff, cnt) != 0)                                             //
//     {                                                                                //
//         m_logger.Log( LOG_ERRORS, "\tInvalid checksum\n");                           //
//         retval = false;                                                              //
//     }                                                                                //
//     m_logger.Log( LOG_FN_ENTRY,                                                      //
//                   "ITcpProtocol::IsResponseMessageValid() - Exit - Returning: %s\n", //
//                   retval ? "true" : "false");                                        //
//     return( retval);                                                                 //
// }                                                                                    //

/**
 * Receives and validates a PLC TCP reply message
 *
 * @param buff      Buffer into which the TCP data should be received
 * @param byteCnt   Maximum number of bytes to receive from the PLC
 * @param plcData   Array into which converted binary data should be put
 * @param dataCount Maximum number of PLC words to convert into the plcData array
 * @return 0 if buffer is valid; any other value indicates an error
 */
long ITcpProtocol::CheckBufferRead( uint8_t *buff, short byteCnt, uint32_t *plcData, int dataCount)
{
    register long   cnt;
    long            msgError = 1;

    // Wait for all data to be received
    if ( (cnt=WaitForAllData(buff, byteCnt)) > 0)
    {
        // cnt holds the number of bytes read !
        // above function cleans up double stuffed DLEc characters
        // Validate the TCP data
        
        // Convert TCP serial bytes to 32-bit integers
        if ( ExtractData( buff, cnt, plcData, dataCount) > 0)
        {
            msgError = 0;
        }
        else
        {
            m_logger.Log( LOG_ERRORS, "\tError in ExtractData()\n");
        }
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tError in WaitForAllData(); %d bytes received\n", cnt);
    }
    return( msgError);                               // return no_error
}

/**
 * Transfers integer data to the PLC
 *
 * @param destId  TCP destination node ID to which the message is addressed
 * @param byteCnt Number of integers to transfer
 * @param plcOut  Pointer to start of array of integers to be transferred
 * @param plcAddr PLC Address (i.e. N16:0) into which the integers should be transferred
 * @return Number of bytes transmitted
 */
long ITcpProtocol::BufferWrite( uint8_t destId, short byteCnt, const uint32_t *plcOut, char *plcAddr)
{
    register uint32_t   nn=0,cnt=0, mm=0;
    register uint8_t    *pp;
    unsigned int        tns,out_buf, bytesWritten=0;

    // Count the header + data to send
    mm = MAX_HDR_SIZE + 2*byteCnt;
    if ( mm > 0)
    {
        const int   buffSz = mm;
        char        xmtBuff[ buffSz];

        nn = mm = 0;
        pp = (uint8_t*)xmtBuff;

//         pp[ nn++] = 0x52;       // Tell the PLC to read in N16 words we are writing
//         pp[ nn++] = 0x65;
//         pp[ nn++] = 0x61;
//         pp[ nn++] = 0x64;
//         pp[ nn++] = 0x4E;
//         pp[ nn++] = 0x31;
//         pp[ nn++] = 0x36;
//         pp[ nn++] = 0x00;
//         pp[ nn++] = 0x00;
//         pp[ nn++] = 0x00;
        if(m_tcpType == AB_TYPE)
        {
            strcpy( (char*)pp, AB_WRITE_HDR);       // Tell the PLC to write out N17 words to the port
            nn += AB_SEND_HDR_SIZE;
        }
        else if(m_tcpType == S7_TYPE)
        {
            uint8_t temp[] = S7_WRITE_HDR;
            temp[12] = (uint8_t)((byteCnt) >> 8);     // high byte of length of data
            temp[13] = (uint8_t)(byteCnt);            // low byte of length of data
            memcpy( pp, temp, S7_SEND_HDR_SIZE);
            nn += S7_SEND_HDR_SIZE;
        }

        cnt = 0;
        while ( byteCnt--)
        {
            uint16_t    plcWord = plcOut[ cnt];
#if 0
            if(m_tcpType == S7_TYPE)
            {
                // high byte to buffer
                out_buf = ((plcWord & 0xFF00) >> 8);
                pp[ nn++] = out_buf;
                // the next line double stuffs DLEc characters
                //if (out_buf == 0x10)     pp[ nn++] = out_buf;
    
                // low byte to buffer
                out_buf = (plcWord & 0x00FF);
                pp[ nn++] = out_buf;
                // the next line double stuffs DLEc characters
                //if (out_buf == 0x10)     pp[ nn++] = out_buf;
            }
            else
#else
            {
                // low byte to buffer
                out_buf = (plcWord & 0x00FF);
                pp[ nn++] = out_buf;
                // the next line double stuffs DLEc characters
                //if (out_buf == 0x10)     pp[ nn++] = out_buf;

                // high byte to buffer
                out_buf = ((plcWord & 0xFF00) >> 8);
                pp[ nn++] = out_buf;
                // the next line double stuffs DLEc characters
                //if (out_buf == 0x10)     pp[ nn++] = out_buf;
            }
#endif 
            // next register
            cnt++;
        }

        char    prefix[ 1024];

        sprintf( prefix, "Writing %d bytes on fd = %d", nn, m_tcpComDevWrite);
        LogPlcString( LOG_DEV_DATA, prefix, pp, nn);
        bytesWritten = write( m_tcpComDevWrite, pp, nn);

        m_logger.Log(LOG_DEV_DATA, "Wrote %d bytes\n", bytesWritten);

        if ( bytesWritten < 0)
        {
            m_logger.Log( LOG_ERRORS, "\tITcpProtocol::BufferWrite ERROR: %s", strerror( errno));
            // return error
            bytesWritten = 0;
        }

//         // skip first two and last two                     //
//         //pp[ nn++] = (char)BccCalc( &pp[2], nn - 4);      //
//         if ( ValidateCrc( pp, nn) == 0)                    //
//         {                                                  //
//             char ackRsp = ENQc;                            //
//             char    prefix[ 1024];                         //
//                                                            //
//             sprintf( prefix, "Writing %d bytes", nn);      //
//             LogPlcString( LOG_DEV_DATA, prefix, pp, nn);   //
//                                                            //
//             cnt = 0;                                       //
//             while ( (ACKc != ackRsp) && (cnt++ < 3))       //
//             {                                              //
//                 // Try to send a "Write Block" to the PLC  //
//                                                            //
//             }                                              //
//         }                                                  //
    }

    return(bytesWritten);
}

// /**                                                                                   //
//  * Calculates the number of data bytes in a PLC write message taking into             //
//  * consideration duplicate DLE's                                                      //
//  *                                                                                    //
//  * @param plcOut  Raw data buffer to be written                                       //
//  * @param buffLen Number of items in the plcOut array                                 //
//  * @return Number of items in the data field of a PLC write message for the given     //
//  *         PLC data                                                                   //
//  */                                                                                   //
// uint32_t ITcpProtocol::CalculateBufferLen( const uint32_t *plcOut, uint32_t buffLen)  //
// {                                                                                     //
//     register uint32_t nn, retVal = buffLen;                                           //
//     // Count the extra DLE characters in the data to write                            //
//     for ( nn=0; nn<buffLen; nn++)                                                     //
//     {                                                                                 //
//         if ( (plcOut[ nn] & 0x00FF) == 0x0010)   retVal++;                            //
//         if ( (plcOut[ nn] & 0xFF00) == 0x1000)   retVal++;                            //
//     }                                                                                 //
//                                                                                       //
//     return( retVal);                                                                  //
// }                                                                                     //

/**
 * Reads integer data from the PLC
 *
 * @param destId  TCP destination node ID to which the message is addressed
 * @param byteCnt Number of integers to read
 *      (do NOT pass in 0x08 or 0x10 as byteCnt - bad things could happen)
 * @param plcAddr PLC Address (i.e. N17:0) from which the data should be read
 * @return Number of integers read
 */
long ITcpProtocol::BufferRead( uint8_t destId, short byteCnt, char *plcAddr)
{
    const uint32_t      buffLen = MAX_HDR_SIZE;
    uint8_t             xmtBuff[ buffLen];
    register uint32_t   nn=0, mm=0;
    register uint8_t    *pp;
    unsigned int        tns;
    int                 bytesWritten = 0;

    pp = xmtBuff;

//     pp[ nn++] = 0x57;       // Tell the PLC to write out N17 words to the port so we can read them
//     pp[ nn++] = 0x72;
//     pp[ nn++] = 0x69;
//     pp[ nn++] = 0x74;
//     pp[ nn++] = 0x65;
//     pp[ nn++] = 0x4E;
//     pp[ nn++] = 0x31;
//     pp[ nn++] = 0x37;
//     pp[ nn++] = 0x00;
//     pp[ nn++] = 0x00;
    if(m_tcpType == AB_TYPE)
    {
        strcpy( (char*)pp, AB_READ_HDR);       // Tell the PLC to write out N17 words to the port
        nn += AB_SEND_HDR_SIZE;
    }
    else if(m_tcpType == S7_TYPE)
    {
        uint8_t temp[] = S7_READ_HDR;
        temp[12] = (uint8_t)((byteCnt) >> 8);     // high byte of length of data
        temp[13] = (uint8_t)(byteCnt);            // low byte of length of data
        memcpy( pp, temp, S7_SEND_HDR_SIZE);
        nn += S7_SEND_HDR_SIZE;
    }

    char    prefix[ 1024];

    sprintf( prefix, "Writing %d bytes on fd = %d", nn, m_tcpComDevRead);
    LogPlcString( LOG_DEV_DATA, prefix, pp, nn);
    bytesWritten = write( m_tcpComDevRead, pp, nn);

    m_logger.Log(LOG_DEV_DATA, "Wrote %d bytes\n", bytesWritten);

    if ( bytesWritten < 0)
    {
        m_logger.Log( LOG_ERRORS, "\tITcpProtocol::BufferRead ERROR: %s", strerror( errno));
        // return error
        bytesWritten = 0;
    }

    //if (m_tcpType == S7_TYPE) 
    //{
    //    bytesWritten = 1;
    //}

//     // Make sure our CRC is valid before sending                                                     //
//     if ( ValidateCrc( xmtBuff, nn) == 0)                                                             //
//     {                                                                                                //
//         char    ackRsp = ENQc;                                                                       //
//         char    prefix[ 1024];                                                                       //
//         int cnt = 0;                                                                                 //
//                                                                                                      //
//         sprintf( prefix, "Sending (%d bytes, %d calc)", nn, buffLen);                                //
//         LogPlcString( LOG_DEV_DATA, prefix, pp, nn);                                                 //
//                                                                                                      //
//         while ( (ackRsp != ACKc) && (cnt++ < 3))                                                     //
//         {                                                                                            //
//             // Try to send "Read Block" to the PLC                                                   //
//             if ( (bytesWritten = write( m_tcpComDev, pp, nn)) > 0)                                      //
//             {                                                                                        //
//                 ackRsp = WaitForAck();                                                               //
//                 // If PLC responds with anything other than ACK                                      //
//                 if (ackRsp != ACKc)                                                                  //
//                 {                                                                                    //
//                     // return error                                                                  //
//                     bytesWritten = 0;                                                                   //
//                 }                                                                                    //
//             }                                                                                        //
//             else if ( bytesWritten == -1)                                                               //
//             {                                                                                        //
//                 m_logger.Log( LOG_ERRORS, "\tITcpProtocol::BufferRead ERROR: %s", strerror( errno)); //
//             }                                                                                        //
//         }                                                                                            //
//     }                                                                                                //

    return(bytesWritten);
}

// /**                                                                    //
//  * Acknowledge a successful read operation by sending ACK to the PLC   //
//  */                                                                    //
// void ITcpProtocol::AckBufferRead()                                     //
// {                                                                      //
//     const uint8_t   plcOut[] = { DLEc, ACKc};                          //
//                                                                        //
//     write( m_tcpComDev, plcOut, sizeof( plcOut));                      //
// }                                                                      //
//                                                                        //
// /**                                                                    //
//  * Notify the PLC of a failed read operation by sending NAK to the PLC //
//  */                                                                    //
// void ITcpProtocol::NAkBufferRead()                                     //
// {                                                                      //
//     const uint8_t   plcOut[] = { DLEc, NAKc};                          //
//                                                                        //
//     write( m_tcpComDev, plcOut, sizeof( plcOut));                      //
// }                                                                      //
//                                                                        //
// /**                                                                    //
//  * Request re-transmission of the last read operation                  //
//  */                                                                    //
// void ITcpProtocol::ENQBufferRead()                                     //
// {                                                                      //
//     const uint8_t   plcOut[] = { DLEc, ENQc};                          //
//                                                                        //
//     write( m_tcpComDev, plcOut, sizeof( plcOut));                      //
// }                                                                      //

// /**                                                                             //
//  * Increment the transaction counter                                            //
//  *                                                                              //
//  * @return Incremented transation counter                                       //
//  */                                                                             //
// unsigned int ITcpProtocol::IncTNSCntr()                                         //
// {                                                                               //
//     unsigned int retVal = (uint32_t)-1;                                         //
//     if ( m_tcpLock.Acquire() == EOK)                                            //
//     {                                                                           //
//         m_tcpTnsCounter++;                                                      //
//         // Avoid DLE's in the data stream                                       //
//         if ( (m_tcpTnsCounter & 0xFF00) == 0x1000)   m_tcpTnsCounter += 0x0100; //
//         if ( (m_tcpTnsCounter & 0x00FF) == 0x0010)   m_tcpTnsCounter += 0x0001; //
//                                                                                 //
//         retVal = m_tcpTnsCounter;                                               //
//         m_tcpLock.Release();                                                    //
//     }                                                                           //
//     return( retVal);                                                            //
// }                                                                               //

// /**                                        //
//  * Get the most recent transaction counter //
//  *                                         //
//  * @return transation counter              //
//  */                                        //
// unsigned int ITcpProtocol::GetTNSCntr()    //
// {                                          //
//     unsigned int retVal = (uint32_t)-1;    //
//     if (m_tcpLock.Acquire() == EOK)        //
//     {                                      //
//         retVal = m_tcpTnsCounter;          //
//         m_tcpLock.Release();               //
//     }                                      //
//     return( retVal);                       //
// }                                          //

// /**                                                                                //
//  * Calculates the CRC for a PLC serial transaction                                 //
//  * Do NOT use this for received messages, as the DLEc-DLEc pairs                   //
//  * have already been removed from the message.                                     //
//  * Use BccCalcRaw() instead                                                        //
//  *                                                                                 //
//  * @param pp     Data array to calculate the CRC for                               //
//  * @param len    Number of bytes in the data array                                 //
//  *                                                                                 //
//  * @return CRC for the given data array                                            //
//  */                                                                                //
// unsigned short ITcpProtocol::BccCalc( register uint8_t *pp, register short len)    //
// {                                                                                  //
//     unsigned short  crc1=0x0000;                                                   //
//     register short  xx;                                                            //
//                                                                                    //
//     for (xx=0; xx<len; xx++)                     // do this many                   //
//     {                                                                              //
//         if ( (pp[xx] == 0x10) && (pp[xx+1] == 0x10) ) xx++;                        //
//         crc1 += pp[xx];                                                            //
//     }                                                                              //
//     crc1 &= 0x00FFL;                // xor byte with crc                           //
//     //ews- fixed next line                                                         //
//     crc1 = (0x100 - crc1) & 0xFF;                                                  //
//     return(crc1);                                                                  //
// }                                                                                  //
//                                                                                    //
// /**                                                                                //
//  * Calculates the CRC for a PLC serial transaction                                 //
//  * Do NOT use this for received messages, as the DLEc-DLEc pairs                   //
//  * have already been removed from the message.                                     //
//  * Use BccCalcRaw() instead                                                        //
//  *                                                                                 //
//  * @param pp     Data array to calculate the CRC for                               //
//  * @param len    Number of bytes in the data array                                 //
//  *                                                                                 //
//  * @return CRC for the given data array                                            //
//  */                                                                                //
// unsigned short ITcpProtocol::BccCalcRaw( register uint8_t *pp, register short len) //
// {                                                                                  //
//     unsigned short crc1=0x0000;                                                    //
//     register short xx;                                                             //
//                                                                                    //
//     for (xx=0; xx<len; xx++)                     // do this many                   //
//     {                                                                              //
//         // if( (pp[xx] == 0x10) && (pp[xx+1] == 0x10) ) xx++;                      //
//         crc1 += pp[xx];                                                            //
//     }                                                                              //
//     crc1 &= 0x00FFL;                // xor byte with crc                           //
//     //ews- fixed next line                                                         //
//     crc1 = (0x100 - crc1) & 0xFF;                                                  //
//     return(crc1);                                                                  //
// }                                                                                  //

/**
 * Waits for all reply data from the PLC
 *
 * @param rxBuff Buffer to receive the PLC data into
 * @param count  Number of bytes in the rxBuff paramater
 * @return Number of bytes read
 */
int ITcpProtocol::WaitForAllData( uint8_t *rxBuff, int count)
{
    int         totalRead=0, bytesRead=0, bytesRemaining=2, bytesAvail=0, loopCount=25, headerSize=MAX_HDR_SIZE;
    uint64_t    timeOut = mSEC_nSEC( 10);
    uint8_t     curData=0, lastData=0, ii;
    char        incomingData[1024];

    if(m_tcpType == AB_TYPE)
    {
        headerSize = AB_RCV_HDR_SIZE;
    }
    else if(m_tcpType == S7_TYPE)
    {
        headerSize = S7_RCV_HDR_SIZE;
    }

    char        incomingHeader[headerSize];

    if(m_tcpType == AB_TYPE)
    {
        //incomingHeader = AB_RCV_HDR;
        strcpy(incomingHeader, AB_RCV_HDR);
    }
    else if(m_tcpType == S7_TYPE)
    {
        char byteArray[S7_RCV_HDR_SIZE] = S7_RCV_HDR;
        memcpy(incomingHeader, byteArray, S7_RCV_HDR_SIZE);
    }

    char headerString[headerSize + 1];
    memcpy(headerString, incomingHeader, headerSize);

    m_logger.Log( LOG_FN_ENTRY, "Enter ITcpProtocol::WaitForAllData(), expecting: %d bytes\n", count);

    // Loop until all expected data has been received
    while ( (totalRead < count) && (bytesRemaining > 0) && (false == m_terminate) && (loopCount > 0))
    {
        // See if there is any data available to read
        if ( (bytesAvail = tcischars( m_tcpComDevRead)) > 0)
        {
            m_logger.Log( LOG_DEV_DATA, "%d bytes available to read\n", bytesAvail);

            // Only read as much as the caller can handle
            if ( bytesAvail > (count - totalRead))
            {
                bytesAvail = count - totalRead;
                m_logger.Log( LOG_DEV_DATA, "Looking for max of %d bytes due to buffer size\n", bytesAvail);
            }
            m_logger.Log(LOG_DEV_DATA, "Looking for incoming message header \"%s\"\n",headerString);
            bytesRead = read( m_tcpComDevRead, incomingData, 1024);

            if(bytesRead > 0)
            {
                char *dataBegin = strstr(incomingData, headerString);

                if(totalRead > 0)
                {
                    dataBegin = incomingData;
                }
                if(dataBegin != NULL)
                {
                    long offset = (long)dataBegin - (long)incomingData;             // Get the offset for where our data actually starts
                    for(long ii = 0; (ii + offset) < bytesRead; ii++)
                    {
                        rxBuff[totalRead] = (uint8_t)incomingData[ii + offset];     // Copy bytes to buffer
                        totalRead++;
                    }
                }
            }

            if(bytesRead < 0)
            {
                m_logger.Log( LOG_ERRORS, "\tITcpProtocol::WaitForAllData read ERROR(%d): %s\n",
                    errno, strerror( errno));
                m_logger.Log( LOG_ERRORS, "\t    Expecting: %d received: %d\n", count, totalRead);

                if (totalRead > 0)
                    LogPlcString( LOG_ERRORS, "Current PLC data", rxBuff, totalRead);
                bytesRead = 0;
            }
            //TotalRead is already incremented when copying bytes to buffer
            //totalRead += bytesRead;


            
        }//end if

        // Delay a bit if we did not read anything
        if ( (bytesAvail <= 0) || (bytesRead <= 0))
        {
            int logMask = LOG_DEV_DATA;

            // Set a timeout on the read operation to try and detect the end
            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_NANOSLEEP, NULL, &timeOut, NULL);
            loopCount--;

            // If there is data available and we did not read anything
            if ( (bytesAvail > 0) && (bytesRead <= 0))   logMask |= LOG_ERRORS;

            m_logger.Log( logMask, "\tWaitForAllData() Available: %d; Read:%d; Loop %d\n",
                          bytesAvail, bytesRead, loopCount);
        }
    }//end while

    // If we received any data
    if ( totalRead)
    {
        LogPlcString( LOG_DETAILED_DATA | LOG_DEV_DATA, "WaitForAllData Received", rxBuff, totalRead);
    }
    else if ( (bytesAvail < 0) || (bytesRead < 0))
    {
        m_logger.Log( LOG_ERRORS, "\tITcpProtocol::WaitForAllData read (%d): %s\n", errno, strerror( errno));
    }

    m_logger.Log( LOG_FN_ENTRY, "Exit ITcpProtocol::WaitForAllData(), %d bytes read\n", totalRead);
    return( totalRead);
}

// /**                                                                                            //
//  * Validates a TCP CRC                                                                         //
//  *                                                                                             //
//  * @param tcpData TCP serial string to validate                                                //
//  * @param tcpSize number of bytes in the TCP serial string                                     //
//  * @return 0 if CRC is valid, 1 if it is invalid                                               //
//  */                                                                                            //
// int ITcpProtocol::ValidateCrc( uint8_t* tcpData, int tcpSize)                                  //
// {                                                                                              //
//     register uint8_t   *pp = tcpData;                                                          //
//     register long       nn = 1, cnt = tcpSize;                                                 //
//     long                msgError = 1;                                                          //
//     unsigned int        stx_ptr=0, etx_ptr=0;                                                  //
//     uint8_t             crc;                                                                   //
//                                                                                                //
//     // Find message frame                                                                      //
//     while (nn < cnt)                                                                           //
//     {                                                                                          //
//         // Find first occurrence of <10> <02>                                                  //
//         if ((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))    stx_ptr = nn;         //
//         // Find last occurrence of <10> <03>                                                   //
//         if ((pp[nn] == ETXc) && (pp[nn-1] == DLEc))                      etx_ptr = nn;         //
//                                                                                                //
//         nn++;                                                                                  //
//     }                                                                                          //
//                                                                                                //
//     cnt = (etx_ptr - 2) - (stx_ptr + 1) + 1;                                                   //
//     // If we found a valid frame                                                               //
//     if ( (cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(tcpSize-1))) //
//     {                                                                                          //
//         // If CRC is valid                                                                     //
//         crc = (uint8_t)BccCalc( &pp[stx_ptr+1], cnt);                                          //
//         if ((pp[etx_ptr+1]) == crc)                                                            //
//         {                                                                                      //
//             msgError = 0;                                                                      //
//         }                                                                                      //
//         else                                                                                   //
//         {                                                                                      //
//             m_logger.Log( LOG_ERRORS, "\tInvalid CRC (%d, %d, %d) $%02hhX != $%02hhX\n",       //
//                           tcpSize, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);                      //
//             LogPlcString( LOG_ERRORS, "\tCRC Data", tcpData, tcpSize);                         //
//         }                                                                                      //
//     }                                                                                          //
//     else                                                                                       //
//     {                                                                                          //
//         m_logger.Log( LOG_ERRORS, "\tInvalid TCP string (%d, %d, %d): Invalid TCP frame\n",    //
//                       tcpSize, stx_ptr, etx_ptr);                                              //
//         LogPlcString( LOG_ERRORS, "\tFrame not found", tcpData, tcpSize);                      //
//     }                                                                                          //
//     return( msgError);                               // return no_error                        //
// }                                                                                              //
//                                                                                                //
// /**                                                                                            //
//  * Validates a TCP CRC                                                                         //
//  *                                                                                             //
//  * @param tcpData TCP serial string to validate                                                //
//  * @param tcpSize number of bytes in the TCP serial string                                     //
//  * @return 0 if CRC is valid, 1 if it is invalid                                               //
//  */                                                                                            //
// int ITcpProtocol::ValidateCrcRaw( uint8_t* tcpData, int tcpSize)                               //
// {                                                                                              //
//     register uint8_t   *pp = tcpData;                                                          //
//     register long     nn = 1, cnt = tcpSize;                                                   //
//     long              msgError = 1;                                                            //
//     unsigned int      stx_ptr=0, etx_ptr=0;                                                    //
//     uint8_t           crc;                                                                     //
//                                                                                                //
//     // Find message frame                                                                      //
//     while (nn < cnt)                                                                           //
//     {                                                                                          //
//         // Find first occurrence of <10> <02>                                                  //
//         if ((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))   stx_ptr = nn;          //
//         // Find last occurrence of <10> <03>                                                   //
//         if ((pp[nn] == ETXc) && (pp[nn-1] == DLEc))                  etx_ptr = nn;             //
//                                                                                                //
//         nn++;                                                                                  //
//     }                                                                                          //
//                                                                                                //
//     cnt = (etx_ptr - 2) - (stx_ptr + 1) + 1;                                                   //
//     // If we found a valid frame                                                               //
//     if ((cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(tcpSize-1)))  //
//     {                                                                                          //
//         // If CRC is valid                                                                     //
//         crc = (uint8_t)BccCalcRaw( &pp[stx_ptr+1], cnt);                                       //
//         if ((pp[etx_ptr+1]) == crc)                                                            //
//         {                                                                                      //
//             msgError = 0;                                                                      //
//         }                                                                                      //
//         else                                                                                   //
//         {                                                                                      //
//             m_logger.Log( LOG_ERRORS, "\tInvalid CRC (%d, %d, %d) $%02hhX != $%02hhX\n",       //
//                           tcpSize, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);                      //
//             LogPlcString( LOG_ERRORS, "\tCRC Data", tcpData, tcpSize);                         //
//         }                                                                                      //
//     }                                                                                          //
//     else                                                                                       //
//     {                                                                                          //
//         m_logger.Log( LOG_ERRORS, "\tInvalid TCP string (%d, %d, %d): Invalid TCP frame\n",    //
//                       tcpSize, stx_ptr, etx_ptr);                                              //
//         LogPlcString( LOG_ERRORS, "\tFrame not found", tcpData, tcpSize);                      //
//     }                                                                                          //
//     return( msgError);                               // return no_error                        //
// }                                                                                              //

/**
 * Extracts PLC data from a TCP serial string
 *
 * @param tcpData   TCP serial string to extract data from
 * @param tcpSize   Number of bytes in the TCP serial string
 * @param plcData   Array into which converted data will be written
 * @param dataCount Number of elements in the plcData array
 * @return Number of 32-bit integers converted and written to the plcData array
 */
int ITcpProtocol::ExtractData( uint8_t *tcpData, int tcpSize, uint32_t *plcData, int dataCount)
{
    register uint8_t   *pp;
    register long       nn,cnt;
    uint32_t            stx_ptr,etx_ptr;
    uint16_t            in_buf;
    int                 hdrSize = 0;

    pp = tcpData;
    cnt = tcpSize;
    in_buf = stx_ptr = etx_ptr = 0;

    nn = 0;

    if(m_tcpType == AB_TYPE)
        hdrSize = AB_RCV_HDR_SIZE;
    else if(m_tcpType == S7_TYPE)
        hdrSize = S7_RCV_HDR_SIZE;
    // if valid data bufferthen move string into variables
    if ( (plcData != NULL) && ( dataCount > 0) && (hdrSize < tcpSize))
    {
        cnt = hdrSize;      // variable data at this position
        nn = 0;
        while ( (cnt < tcpSize) && (nn < dataCount)) // stop when all bytes are done
        {
//            if(m_tcpType == S7_TYPE) 
//				in_buf = (pp[cnt+1] | (pp[cnt] << 8) );
//            else 
				in_buf = (pp[cnt] | (pp[cnt+1] << 8) ); // Combine bytes into word
            plcData[ nn++] = in_buf;                // stuff data into global memory
            cnt += 2;                               // go to next word
        }
    }
    return( nn);
}

// /**                                                                                                                           //
//  * Wait for the PLC to ACK/NAK our commands                                                                                   //
//  *                                                                                                                            //
//  * @return NAKc if NAK received                                                                                               //
//  *         ACKc if ACK recevied                                                                                               //
//  *         ENQc if ENQ received                                                                                               //
//  */                                                                                                                           //
// char ITcpProtocol::WaitForAck()                                                                                               //
// {                                                                                                                             //
//     uint8_t plcData[ 10];                                                                                                     //
//     bool    keepWaiting=true;                                                                                                 //
//     int     enqCount=0, errCnt=0, readIdx=0, readCount=0;                                                                     //
//     char    retVal = NAKc;                                                                                                    //
//                                                                                                                               //
//     // Initialize to junk                                                                                                     //
//     plcData[ 0] = plcData[ 1] = DLEc ^ 0xFF;                                                                                  //
//                                                                                                                               //
//     // Wait until and ACK or other recognized command comes in                                                                //
//     while ( (keepWaiting) && (false == m_terminate))                                                                          //
//     {                                                                                                                         //
//         if ( (readCount=WaitForAllData(&plcData[ readIdx], 2-readIdx)) > 0)                                                   //
//         {                                                                                                                     //
//             readIdx += readCount;                                                                                             //
//             if ( readIdx > 1)                                                                                                 //
//             {                                                                                                                 //
//                 // We only need 2 bytes for an ACK. If we have at least 2 bytes and it                                        //
//                 // is not an ACK, leave and (possibly) resend our last message. If we did                                     //
//                 // get an ACK, we're done here.                                                                               //
//                 keepWaiting = false;                                                                                          //
//                                                                                                                               //
//                 // If this looks like a command                                                                               //
//                 if ( plcData[ 0] == DLEc)                                                                                     //
//                 {                                                                                                             //
//                     // If it is an ACK                                                                                        //
//                     if ( plcData[ 1] == ACKc)                                                                                 //
//                     {                                                                                                         //
//                         m_logger.Log( LOG_DEV_DATA, "\tGot ACKc\n");                                                          //
//                     }                                                                                                         //
//                     // Not an ACK...check to see what it is                                                                   //
//                     else                                                                                                      //
//                     {                                                                                                         //
//                         // Flush our FIFO                                                                                     //
//                         tcflush( m_tcpComDev, TCIOFLUSH);                                                                     //
//                         if ( plcData[ 1] == NAKc)                                                                             //
//                         {                                                                                                     //
//                             m_logger.Log( LOG_ERRORS, "\tGot NAK while waiting for ACK\n");                                   //
//                         }                                                                                                     //
//                         else if ( plcData[ 1] == ENQc)                                                                        //
//                         {                                                                                                     //
//                             // This is the PLC's way of saying that we haven't ACK'd his last message                         //
//                             // If this hasn't happened too many time, try to ACK the PLC                                      //
//                             if ( enqCount++ < 3)                                                                              //
//                             {                                                                                                 //
//                                 AckBufferRead();                                                                              //
//                                 keepWaiting = true;                                                                           //
//                             }                                                                                                 //
//                             m_logger.Log( LOG_ERRORS, "\tGot ENQ while waiting for ACK: %d\n", enqCount);                     //
//                         }                                                                                                     //
//                         else                                                                                                  //
//                         {                                                                                                     //
//                             // Tell the PLC that we did not like hos last message                                             //
//                             NAkBufferRead();                                                                                  //
//                             m_logger.Log( LOG_ERRORS, "\tGot unknown command while waiting for ACKc: %02hhX\n", plcData[ 1]); //
//                         }                                                                                                     //
//                     }                                                                                                         //
//                 }                                                                                                             //
//                 else                                                                                                          //
//                 {                                                                                                             //
//                     tcflush( m_tcpComDev, TCIOFLUSH);                                                                         //
//                     LogPlcString( LOG_ERRORS, "\tERROR did not get ACK", plcData, readIdx);                                   //
//                 }                                                                                                             //
//                                                                                                                               //
//                 // Reset our read pointer back to the start of the buffer                                                     //
//                 readIdx = 0;                                                                                                  //
//                                                                                                                               //
//                 retVal = plcData[ 1];                                                                                         //
//                                                                                                                               //
//                 // Re-initialize to junk                                                                                      //
//                 plcData[ 0] = plcData[ 1] = DLEc ^ 0xFF;                                                                      //
//             }                                                                                                                 //
//         }                                                                                                                     //
//         // If 2nd error, leave                                                                                                //
//         else if ( errCnt++)                                                                                                   //
//         {                                                                                                                     //
//             keepWaiting = false;                                                                                              //
//         }                                                                                                                     //
//     }                                                                                                                         //
//                                                                                                                               //
//     return( retVal);                                                                                                          //
// }                                                                                                                             //

/**
 * Logs a PLC response string
 *
 * @param logMask Our logger interface verbosity bit which must be set in order to log
 *                the string
 * @param prefix  String to prepend to the PLC data string when logged
 * @param plcData The PLC data string to be logged
 * @param dataLen The length of the PLC data string
 */
void ITcpProtocol::LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
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

