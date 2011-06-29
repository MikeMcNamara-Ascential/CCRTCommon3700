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
//==============================================================================
#include <sys/select.h>
#include "IMelsecProtocol.h"

IMelsecProtocol::IMelsecProtocol() :
m_melsecComDev( -1), m_stationNumber(0), m_pcNumber(0xFF)
{
}

IMelsecProtocol::~IMelsecProtocol()
{
}

/**
 * Open a Melsec serial PLC
 *
 * @param comDev Handle to the communication device to be used for Melsec communication (i.e. a
 *               socket, a serial port ,etc)
 * 
 * @return EOK if successful
 */
int IMelsecProtocol::Open(int comDev)
{
    int retVal;

    // Lock so we know our handle is valid
    if( (retVal=m_melsecLock.Acquire()) == EOK)
    {
        m_melsecComDev = comDev;

        m_melsecLock.Release();
    }

    return( retVal);
}

/**
 * Open a Melsec serial PLC with I/O address parametrs
 *
 * @param comDev Handle to the communication device to be used for Melsec communication (i.e. a
 *               socket, a serial port ,etc)
 * 
 * @return EOK if successful
 */
int IMelsecProtocol::Open(int comDev, string outputAddress, string inputAddress)
{
    int retVal;

    // Lock so we know our handle is valid
    if( (retVal=m_melsecLock.Acquire()) == EOK)
    {
        m_melsecComDev = comDev;
        m_melsecOutAddr = outputAddress;
        m_melsecInAddr = inputAddress;

        m_melsecLock.Release();
    }

    return( retVal);
}

/**
 * Close an instance of a Melsec PLC
 *
 * @return 0 if successful. Any other value indicates error
 */
int IMelsecProtocol::Close()
{
    int retVal = EOK;

    // Lock so we know our handle is valid
    if( (retVal=m_melsecLock.Acquire()) == EOK)
    {
        m_melsecComDev = -1;
        m_melsecLock.Release();
    }

    return( retVal);
}

/**
 * Initialize data for Melsec communication
 *
 * @param logger ILogger to use for initializing my logger interface
 */
void IMelsecProtocol::Init( ILogger &logger)
{
    m_logger.SetVerboseMask( logger.GetVerboseMask());
    m_logger.SetLocalEcho( logger.GetLocalEcho());
    m_logger.SetProcessName( "MelsecProtocol");
    m_logger.SetAdditionalLogFileName( logger.GetAdditionalLogFileName());
    if( logger.GetLogStatus())  m_logger.EnableLogger();
}

/**
 * Read data from a Melsec PLC
 *
 * @param buff   Buffer into which the data will be read
 * @param count  Number of 32-bit integers to read
 * 
 * @return 1 - good, 0 - error
 */
int IMelsecProtocol::Read( uint32_t *buff, uint32_t count)
{
    uint8_t plcData[128];
    int tries=5,good=0;

    if(m_melsecLock.Acquire() == EOK)
    {
        do
        {
            if(BufferRead(plcData,count) == 0)    // send the buffer read message
            {
                if(CheckBufferRead(plcData,buff) == 0)
                {
                    AckBufferRead();
                    tries = 0;         // no retries needed
                    good = 1;          // read successful
                    break;             // all done
                }
                else
                {
                    m_logger.Log(LOG_ERRORS,"Bad response to BufferRead\n");
                    NakBufferRead();   // bad message
                    BposSleep(300);    // short break
                    tries--;           // this attempt failed
                    ClearPlc();        // clear error
                }
            }
            else
            {
                m_logger.Log(LOG_ERRORS,"No response to BufferRead\n");
                BposSleep(300);        // short break
                tries--;               // this attempt failed
                ClearPlc();            // clear error
            }
        }
        while(tries);

        m_melsecLock.Release();
    }
    else m_logger.Log(LOG_ERRORS,"Could not acquire m_melsecLock in Read()\n");

    return(good);                      // return status
}

/**
 * Write data to a Melsec PLC
 *
 * @param buff   Buffer from which the data will be written
 * @param count  Number of 32-bit integers to write
 * 
 * @return Number of 32-bit integers written
 */
int IMelsecProtocol::Write(const uint32_t *buff, uint32_t count)
{
    int tries=5,good=0;

    if(m_melsecLock.Acquire() == EOK)
    {
        do
        {
            if(BufferWrite(count,buff) == 0)    // send the buffer write message
            {
                tries = 0;                 // no retries needed
                good = 1;                  // read successful
            }
            else
            {
                m_logger.Log(LOG_ERRORS,"No response to BufferWrite\n");
                BposSleep(300);            // short break
                tries--;                   // this attempt failed
                ClearPlc();                // clear error
            }
        }
        while(tries);

        m_melsecLock.Release();
    }
    else m_logger.Log(LOG_ERRORS,"Could not acquire m_melsecLock in Read()\n");

    return(good);                      // return status
}

/**
 * Receives and validates a PLC Melsec reply message
 *
 * @param buff   Buffer into which the Melsec data should be received
 * @param count  Number of 32-bit integers to read
 * 
 * @return 0 if buffer is valid; any other value indicates an error
 */
long IMelsecProtocol::BufferRead(uint8_t *buff,uint32_t count)
{
    register uint8_t    *pp;
    uint8_t             xmtBuff[32];
    register uint32_t   nn=0,mm=0;
    unsigned short      chksum=0;
    int                 tries=0,status=0;
    bool                responseReceived=false;

    pp = xmtBuff;
    pp[nn++] = ENQc;                   // Enquire
    sprintf((char *)&pp[nn],"%02X",m_stationNumber); nn += 2;    // Station Number
    sprintf((char *)&pp[nn],"%02X",m_pcNumber); nn +=2;    // PC Number
    sprintf((char *)&pp[nn],"WR"); nn += 2;    // Word Read
    sprintf((char *)&pp[nn],"3"); nn += 1;    // extra 30ms wait
    sprintf((char *)&pp[nn],m_melsecInAddr.c_str()); nn += 5;    // Start Address for reading from PLC
    sprintf((char *)&pp[nn],"%02X",count); nn += 2;    // Number of words

    // calculate the check sum
    for(mm=1;mm<nn;mm++) chksum += pp[mm];
    chksum &= 0x00FF;                  // only low bits
    sprintf((char *)&pp[nn],"%02X",chksum); nn += 2;    // add checksum

    pp[nn++] = CRc;                    // <CR>
    pp[nn++] = LFc;                    // <LF>

    pp[nn] = 0;                      // null terminate

    LogPlcString(LOG_DEV_DATA,"",pp,nn);

    // while tries left and response not received
    while((tries++ < 3) && (responseReceived == false))
    {
        status = write(m_melsecComDev,xmtBuff,nn);    // send out the message
        if(status > 0)                 // if message sent successfully
        {
            responseReceived = WaitForResponse(buff);    // wait for the response
            if(responseReceived == false) status = -1;
            else status = 0;
        }
        else if(status == -1)          // error status
        {
            m_logger.Log(LOG_ERRORS,"IMelsecProtocol::BufferRead ERROR: %s", strerror( errno));
        }
    }

    return(status);
}

/**
 * Receives and validates a PLC Melsec reply message
 *
 * @param buff      Buffer into which the Melsec data should be received
 * @param plcData   Array into which converted binary data should be put
 * 
 * @return 0 if buffer is valid; any other value indicates an error
 */
long IMelsecProtocol::CheckBufferRead( uint8_t *buff, uint32_t *plcData)
{
    short cnt=0,nn=0,error=0,ii=0;
    long  chksum=0,value=0;

    // find the end of the message
    while(buff[cnt] != ETXc)
    {
        cnt++;
        if(cnt >= 128) return(1);    // if no ETX found, return error
    }

    for(ii=1;ii<=cnt;ii++) chksum += buff[ii];
    cnt++;                             // move past ETX

    // verify check sum
    if((chksum & 0x00FF) != HexToLong(&buff[cnt],2))
    {
        m_logger.Log(LOG_ERRORS,"Checksum incorrect in CheckBufferRead()\n");
        return(1);
    }
    cnt--;

    // verify <etx>
    if(buff[cnt] != ETXc)
    {
        m_logger.Log(LOG_ERRORS,"<etx> missing in CheckBufferRead()\n");
        return(1);
    }

    // verify <stx>
    nn = 0;
    if(buff[nn] != STXc)
    {
        m_logger.Log(LOG_ERRORS,"<stx> missing in CheckBufferRead()\n");
        return(1);
    }
    nn++;

    // verify station
    if(m_stationNumber != HexToLong(&buff[nn],2))
    {
        m_logger.Log(LOG_ERRORS,"Station number incorrect in CheckBufferRead()\n");
        return(1);
    }
    nn += 2;

    // verify pcNumber
    if(m_pcNumber != HexToLong(&buff[nn],2))
    {
        m_logger.Log(LOG_ERRORS,"PC number incorrect in CheckBufferRead()\n");
        return(1);
    }
    nn += 2;

    ii = 0;
    while(nn < cnt)
    {
        value = HexToLong(&buff[nn],4);       // convert next 4 bytes
        if(value < 0) error = 1;
        else plcData[ii++] = value;    // store data
        nn += 4;                       // next group
    }

    return(error);
}

/**
 * Transfers integer data to the PLC
 *
 * @param count Number of words to transfer
 * @param plcOut  Pointer to start of array of integers to be transferred
 * 
 * @return Number of bytes transmitted
 */
long IMelsecProtocol::BufferWrite(short count, const uint32_t *plcOut)
{
    register uint8_t    *pp;
    uint8_t             rxBuff[128];
    bool                responseReceived=false;
    short               ii=0,nn=0,status=0,tries=0,hold=0,mm=0;
    uint8_t             xmtBuff[256];    
    uint8_t             tempBuff[8];

    pp = xmtBuff;

    pp[nn++] = ENQc;                   // Enquire
    sprintf((char *)&pp[nn],"%02X",m_stationNumber); nn += 2;    // Station Number
    sprintf((char *)&pp[nn],"%02X",m_pcNumber); nn += 2;    // PC Number
    sprintf((char *)&pp[nn],"WW"); nn += 2;    // Word Write
    sprintf((char *)&pp[nn],"3"); nn += 1;     // extra 30ms wait
    sprintf((char *)&pp[nn],m_melsecOutAddr.c_str()); nn += 5; // Start Addres for writing to PLC
    hold = nn;                         // Mark position
    sprintf((char*)&pp[nn],"  "); nn += 2;    //   reserve count

    for(ii=0;ii<count;ii++)
    {
        sprintf((char*)&pp[nn],"%04X",plcOut[ii]);   
        nn += 4;
    }

    mm = (nn - (hold + 2)) / 4;        // update count
    sprintf((char *)tempBuff,"%02X",mm);
    pp[hold++] = tempBuff[0];
    pp[hold++] = tempBuff[1];

    hold = 0;                          // init check sum 
    // calc check sum 
    for(mm=1;mm<nn;mm++) hold += pp[mm];
    hold &= 0x00FF;                    // only low bits
    sprintf((char*)&pp[nn],"%02X",hold); nn += 2;    // put in chk sum

    pp[nn++] = CRc;                    // <CR>
    pp[nn++] = LFc;                    // <LF>
    pp[nn] = 0;                        // null terminate

    LogPlcString(LOG_DEV_DATA,"",pp,nn);

    // while tries left and response not received
    while((tries++ < 3) && (responseReceived == false))
    {
        status = write(m_melsecComDev,xmtBuff,nn);    // send out the message
        if(status > 0)                 // if message sent successfully
        {
            responseReceived = WaitForResponse(rxBuff);    // wait for the response
            if(responseReceived == false) status = -1;
            else status = 0;
        }
        else if(status == -1)          // error status
        {
            m_logger.Log(LOG_ERRORS,"IMelsecProtocol::BufferRead ERROR: %s", strerror( errno));
        }
    }

    return(status);
}

/**
 * Wait for the PLC to respond to our commands
 *
 * @param rxBuff - buffer to store received data
 * 
 * @return true  - response received
 *         false - no response
 */
bool IMelsecProtocol::WaitForResponse(uint8_t *rxBuff)
{
    bool        responseReceived=false;
    bool        lfReceived=false;
    bool        keepWaiting=true;
    uint8_t     curData=0;
    int         bytesRead=0,totalRead=0;
    uint64_t    timeOut = mSEC_nSEC(500);

    while(keepWaiting == true)
    {
        // Set a timeout on the read operation to try and detect the end
        TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_REPLY, NULL, &timeOut, NULL);
        bytesRead = read( m_melsecComDev, &curData, 1);
        if(bytesRead > 0)
        {
            rxBuff[totalRead] = curData;
            totalRead += bytesRead;
            if(curData == LFc) lfReceived = true;

            if(lfReceived == true)
            {
                keepWaiting = false;
                responseReceived = true;
            }
        }
        else if( bytesRead < 0)
        {
            m_logger.Log(LOG_ERRORS, "IMelsecProtocol::WaitForAllData read ERROR(%d): %s\n", errno, strerror( errno));

            if(totalRead > 0)
            {
                LogPlcString(LOG_ERRORS, "Current PLC data", rxBuff, totalRead);
            }
            keepWaiting = false;
        }
        else                           // no data received
        {
            keepWaiting = false;
        }
    }

    if(totalRead)                      // If we received any data
    {
        LogPlcString(LOG_DEV_DATA, "WaitForResponse Received", rxBuff, totalRead);
    }
    else
    {
        m_logger.Log(LOG_ERRORS, "IMelsecProtocol::WaitForResponse read (%d): %s\n", errno, strerror( errno));
    }

    return(responseReceived);
}

/**
 * Acknowledge a successful read operation by sending ACK to the PLC
 */
void IMelsecProtocol::AckBufferRead()
{
    uint8_t plcOut[8];
    short nn=0;

    plcOut[nn++] = ACKc;               // Acknowledge
    sprintf((char *)&plcOut[nn],"%02X",m_stationNumber); nn += 2;    // Station Number
    sprintf((char *)&plcOut[nn],"%02X",m_pcNumber); nn += 2;    // PC Number
    plcOut[nn++] = CRc;                // <CR>
    plcOut[nn++] = LFc;                // <LF>
    plcOut[nn] = 0;                    // null terminate

    write(m_melsecComDev,plcOut,nn);
}

/**
 * Notify the PLC of a failed read operation by sending NAK to the PLC
 */
void IMelsecProtocol::NakBufferRead()
{
    uint8_t plcOut[8];
    short nn=0;

    plcOut[nn++] = NAKc;               // Not-Acknowledge
    sprintf((char *)&plcOut[nn],"%02X",m_stationNumber); nn += 2;    // Station Number
    sprintf((char *)&plcOut[nn],"%02X",m_pcNumber); nn += 2;    // PC Number
    plcOut[nn++] = CRc;                // <CR>
    plcOut[nn++] = LFc;                // <LF>
    plcOut[nn] = 0;                    // null terminate

    write(m_melsecComDev,plcOut,nn);
}

/**
 * Send a message to clear a PLC error
 */
void IMelsecProtocol::ClearPlc()
{
    uint8_t plcOut[8];
    short nn=0;

    plcOut[nn++] = FFc;                // <FF>
    plcOut[nn++] = CRc;                // <CR>
    plcOut[nn++] = LFc;                // <LF>
    plcOut[nn] = 0;                    // null terminate

    write(m_melsecComDev,plcOut,nn);
}

/**
 * Convert ASCII hex data to a long
 * 
 * @param buff - buffer with ASCII data
 * @param cnt - number of ASCII characters
 * 
 * @return the converted long value
 */ 
long IMelsecProtocol::HexToLong(uint8_t *buff,long cnt)
{
    int  ii=0;
    long retVal=0,curVal=0;

    while(cnt--)                       // do specified number of characters
    {
        curVal = buff[ii++];           // get next character
        if(('0' <= curVal) && (curVal <= '9'))    // number range
        {
            curVal = curVal - '0';
        }
        else if(('A' <= curVal) && (curVal <= 'F'))    // letter range
        {
            curVal = curVal - 'A' + 10;
        }
        else if(curVal == ' ')         // space
        {
            curVal = 0;
        }
        else return(-1);               // invalid character

        retVal = (retVal << 4) + curVal;    // update return value
    }

    return(retVal);
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
void IMelsecProtocol::LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
{
    uint32_t    ii;

    // If any data to print and correct verbosity
    if( m_logger.GetVerboseMask() & logMask)
    {
        std::string logBuff("");
        char        junk[ 5];

        if( prefix != NULL)     logBuff = std::string( prefix) + std::string( ": ");

        for( ii=0; ii<dataLen; ii++)
        {
            sprintf( junk,  "$%02hhX ", plcData[ ii]);
            logBuff += std::string( junk);
        }

        if( logBuff.length() > 0)
        {
            logBuff += std::string( "\n");
            m_logger.Log( logMask, logBuff.c_str());
        }
    }
}

