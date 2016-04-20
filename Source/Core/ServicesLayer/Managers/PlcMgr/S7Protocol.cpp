//******************************************************************************
// Description:
//    Siemens S7 protocol wrapper class
//
//==============================================================================
//
// Copyright © 2015 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "S7Protocol.h"

//-------------------------------------------------------------------------------------------------
S7Protocol::S7Protocol() : m_terminate(false), m_s7Comm(NULL)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
S7Protocol::~S7Protocol()
{   // Close the connection to the PLC if needed
	if(m_s7Comm != NULL)
	{
		m_s7Comm->Disconnect();
		delete m_s7Comm;
	}
}

//-------------------------------------------------------------------------------------------------
int S7Protocol::Close()
{
	int retVal = EOK;
	m_terminate = true;
	if((retVal = m_s7Lock.Acquire()) == EOK)
	{
		if(m_s7Comm != NULL)
		{
			m_s7Comm->Disconnect();
		}
		m_s7Lock.Release();
	}
	return retVal;
}

//-------------------------------------------------------------------------------------------------
void S7Protocol::Initialize(const XmlNode *config)
{
	m_logger.Initialize(config);
	m_logger.Log(LOG_DEV_DATA, "S7Protocol::Initialize() - Enter");
	try
	{
		m_ipAddress = config->getChild("IpAddress")->getValue();
		m_s7Comm = new TS7Client();
		m_s7Comm->ConnectTo(m_ipAddress.c_str(), 0, 2);
	}
	catch(XmlException &excpt)
	{
		m_logger.Log(LOG_ERRORS, "Error setting up S7 Comms - %s", excpt.GetReason());
	}
	m_logger.Log(LOG_DEV_DATA, "S7Protocol::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
bool S7Protocol::Read(uint32_t *buff, uint32_t count, int dbNum/*=1*/)
{
	m_logger.Log(LOG_FN_ENTRY, "S7Protocol::Read() - DB Num: %d, Words: %d - Enter", dbNum, count);
	bool goodRead = false;
	byte dbData[512];
	if(m_s7Lock.Acquire() == EOK)
	{
		int attempts = 5;
		while((attempts-- > 0) && !m_terminate && !goodRead)
		{
			if(!m_s7Comm->Connected())
			{
				m_s7Comm->Connect();
			}
			goodRead = m_s7Comm->DBRead(dbNum, 0, count*2, dbData) == 0;
			if(!goodRead)  BposSleep(10);
		}
		m_s7Lock.Release();
		if(goodRead)
		{
			char prefix[1024];
			sprintf(prefix, "Bytes from PLC: ");
			LogPlcString(LOG_DEV_DATA, prefix, dbData, count*2);
			ExtractData(dbData, count*2, buff, count);
		}
	}
	m_logger.Log(LOG_FN_ENTRY, "S7Protocol::Read() - status: %s - Exit", goodRead ? "True" : "False");
	return goodRead;
}

//-------------------------------------------------------------------------------------------------
bool S7Protocol::Write(const uint32_t *buff, uint32_t count, int dbNum/*=1*/)
{
    m_logger.Log(LOG_FN_ENTRY, "S7Protocol::Write() - DB Num: %d, Words: %d - Enter", dbNum, count);
	bool goodWrite = false;
	byte plcData[count*2];
	short numWords = count;
	register UINT32 cnt = 0, nn = 0;
	char                prefix[ 1024];
	// Load data into buffer
	while(numWords--)
	{
		UINT16 plcWord = buff[cnt];
		plcData[nn++] = (plcWord & 0x00FF);
		plcData[nn++] = ((plcWord & 0xFF00) >> 8);
		cnt++;
	}
	sprintf( prefix, "Writing %d bytes to DB %d", nn, dbNum);
	LogPlcString( LOG_DEV_DATA, prefix, plcData, nn);
	// Send the data to the PLC
	if(m_s7Lock.Acquire() == EOK)
	{
		int attempts = 5;
		while((attempts-- > 0) && !goodWrite && !m_terminate)
		{
			goodWrite = m_s7Comm->DBWrite(dbNum, 0, count*2, plcData) == 0;
			if(!goodWrite)  BposSleep(10);
		}
		m_s7Lock.Release();
	}
	m_logger.Log(LOG_FN_ENTRY, "S7Protocol::Write() - status %s - Exit", goodWrite ? "True" : "False");
	return goodWrite;
}

//-------------------------------------------------------------------------------------------------
int S7Protocol::ExtractData(byte rawData[], int byteCount, uint32_t *plcData, int dataCount)
{
	register long cnt=0, nn=0;
	if((plcData != NULL) && (dataCount > 0))
	{
		while((cnt < byteCount) && (nn < dataCount))
		{
			UINT16 word = (rawData[cnt] | (rawData[cnt+1] << 8));
			plcData[nn++] = word;
			cnt += 2;
		}
	}
	return nn;
}

//-------------------------------------------------------------------------------------------------
void S7Protocol::LogPlcString( uint32_t logMask, const char *prefix, 
							   const uint8_t *plcData, const uint32_t dataLen)
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

