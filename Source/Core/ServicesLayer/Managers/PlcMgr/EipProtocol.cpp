//******************************************************************************
// Description:
//    Allen Bradley EIP protocol wrapper class
//
//==============================================================================
//
// Copyright © 21013- Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <sys/select.h>
#include "EipProtocol.h"

//---------------------------------------------------------------------------------------------------------------------
EipProtocol::EipProtocol() : m_terminate( false)
{
}

//---------------------------------------------------------------------------------------------------------------------
EipProtocol::~EipProtocol()
{
}

//---------------------------------------------------------------------------------------------------------------------
int EipProtocol::Close()
{
    int retVal = EOK;
    m_terminate = true;
    // Lock so we know our handle is valid
    if( (retVal=m_df1Lock.Acquire()) == EOK)
    {
        m_df1Lock.Release();
        m_eipComm->Close();
    }
    return( retVal);
}

//---------------------------------------------------------------------------------------------------------------------
void EipProtocol::Initialize( const XmlNode *df1Config)
{
    // load the additional configuration items needed by the EipProtocol.
    LoadAdditionalConfigurationItems(df1Config);
    m_eipComm = new EipWrapper(&m_logger);
    m_eipComm->Initialize();
}

//---------------------------------------------------------------------------------------------------------------------
void EipProtocol::LoadAdditionalConfigurationItems(const XmlNode *df1Config)
{
    try
    {
        m_logger.Initialize( df1Config);
    }
    catch( ...)
    {
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool EipProtocol::Read(uint32_t *buff, uint32_t count, const string &ipAddress, const char *addr)
{
    const int       inBytes = 2*count;
    int             cnt=5;
    string          plcAddr(addr);
    bool            good = false;

    m_logger.Log(LOG_DEV_DATA, "Enter EipProtocol::Read(), Tag: %s, Bytes: %d", addr, inBytes);

    if( m_df1Lock.Acquire() == EOK)
    {
        while( (cnt-- > 0) && (false == m_terminate) && (false == good))
        {
            m_logger.Log(LOG_DEV_DATA, "EipProtocol::Reading tag %s, words: %d", plcAddr.c_str(), count);
            // Send the Read Tag string
            good = (BufferRead(count, ipAddress, plcAddr, buff) >= 0);
            m_logger.Log(LOG_ERRORS, "\tEipProtocol::Read status %s", good ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE);
			if(!good)  BposSleep(10);
        }

        m_df1Lock.Release();
    }

    m_logger.Log( LOG_DEV_DATA, "Exit EipProtocol::Read(), status=%s\n", good ? "true" : "false");

    return(good);
}

//---------------------------------------------------------------------------------------------------------------------
bool EipProtocol::Write(const uint32_t *buff, uint32_t count, const string &ipAddress, const char *addr)
{
    int             bytesWritten=0, cnt = 5;
    string          plcAddr(addr);
    bool            good = false;

    m_logger.Log( LOG_DEV_DATA, "EipProtocol::Write() , Tag: %s, Words: %d, Bytes: %d", plcAddr.c_str(), count, count*2);

    if( m_df1Lock.Acquire() == EOK)
    {   // Try a few times to send this block
        while((cnt-- > 0) && (false == good) && (false == m_terminate))
        {   // Try to write to the PLC
            if((bytesWritten=BufferWrite(count, buff, ipAddress, plcAddr)) > 0)
            {
                good = true;
            }
            else
            {
                m_logger.Log( LOG_ERRORS, "\tEipProtocol::Write BufferWrite error");
				BposSleep(10);
            }
        }
        m_df1Lock.Release();
    }

    m_logger.Log(LOG_DEV_DATA, "Exit EipProtocol::Write(), status: %s, bytes written: %d", 
                 good ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE, bytesWritten);
    return(good);
}

//---------------------------------------------------------------------------------------------------------------------
long EipProtocol::BufferWrite(short byteCnt, const uint32_t *plcOut, const string &plcIpAddr, const string &plcTag)
{
    register uint32_t   cnt=0, nn=0;
    short               numWords = byteCnt;
    uint8_t             plcData[byteCnt*2];
    char                prefix[ 1024];
    // Break words down into individual bytes
    while(numWords--)
    {
        uint16_t    plcWord = plcOut[ cnt];
        // low byte to buffer
        plcData[nn++] = (plcWord & 0x00FF);
        // high byte to buffer
        plcData[nn++] = ((plcWord & 0xFF00) >> 8);
        // next register
        cnt++;
    }
    sprintf( prefix, "Writing %d bytes to %s", nn, plcTag.c_str());
    LogPlcString( LOG_DEV_DATA, prefix, plcData, nn);
    // Send the data to the PLC
    INT32 retStat = -1;
    if(m_eipComm->SendPlcData(plcData, byteCnt*2, plcIpAddr, plcTag))
    {
        retStat = nn;
    }
    m_logger.Log(LOG_DEV_DATA, "Wrote %d bytes to the PLC", retStat);
    return retStat;
}

//---------------------------------------------------------------------------------------------------------------------
long EipProtocol::BufferRead(short byteCnt, const string &plcIpAddr, const string &plcTag, uint32_t *plcData)
{
    uint8_t readBuf[512];
    INT32   bytesRead = -1;
    m_logger.Log(LOG_DEV_DATA, "Reading PLC Tag: %s, %d words", plcTag.c_str(), byteCnt);
    INT32 retStat = m_eipComm->ReceivePlcData(readBuf, byteCnt*2, bytesRead, plcIpAddr, plcTag);
    if(retStat == 0)
    {
        ExtractData(readBuf, bytesRead, plcData, byteCnt);
        char prefix[1024];
        sprintf(prefix, "Bytes from PLC: ");
        LogPlcString(LOG_DEV_DATA, prefix, readBuf, bytesRead);
    }
    return(bytesRead);
}

//---------------------------------------------------------------------------------------------------------------------
int EipProtocol::ExtractData( uint8_t *rawData, int byteCount, uint32_t *plcData, int dataCount)
{
    register uint8_t   *pp;
    register long       nn=0,cnt=0;
    uint32_t            stx_ptr,etx_ptr;
    uint16_t            in_buf;

    pp = rawData;
    cnt = byteCount;
    in_buf = stx_ptr = etx_ptr = 0;

    // if valid data buffer then move string into variables
    if( (plcData != NULL) && ( dataCount > 0))
    {
        //if(pp[4] != abDINT) // make sure we are receiving 16 bit integers back
        //{
            cnt = stx_ptr;
            nn = 0;
            while( (cnt < byteCount) && (nn < dataCount)) // stop when all bytes are done
            {
                in_buf = (pp[cnt] | (pp[cnt+1] << 8) ); // Combine bytes into word
                plcData[ nn++] = in_buf;                // stuff data into global memory
                cnt += 2;                               // go to next word
            }
        //}
    }
    return( nn);
}

//---------------------------------------------------------------------------------------------------------------------
void EipProtocol::LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
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

