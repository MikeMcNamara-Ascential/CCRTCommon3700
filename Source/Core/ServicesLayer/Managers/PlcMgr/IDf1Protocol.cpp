//******************************************************************************
// Description:
//    Allen Bradley DF1 serial protocol wrapper class
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/IDf1Protocol.cpp $
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
// CHanged to more closely resemble the NCA DF1 implementation
//
// 6     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 5     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     2/16/05 9:29p Cward
// UPdated DF1 Protocol with many fixes found in the field.
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
// ii*wordsToUpdate] => &buff[ ii*(m_df1BytesPerBlock/2)]
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

#include "IDf1Protocol.h"

IDf1Protocol::IDf1Protocol() :
m_df1BytesPerBlock( 50), m_df1InitCount( 0), m_df1ComDev( -1),
m_df1TnsCounter( 0), m_df1SrcId( 0), m_terminate( false)
{
}

IDf1Protocol::~IDf1Protocol()
{
}

/**
 * Open a DF1 serial PLC
 *
 * @param comDev Handle to the communication device to be used for DF1 communication (i.e. a
 *               socket, a serial port ,etc)
 * @param src    DF1 node ID to be used as the source identifier in DF1 messages
 * @return EOK if successful
 */
int IDf1Protocol::Open( int comDev, uint8_t src)
{
	int	retVal;

	m_terminate = false;

	// Lock so we know our handle is valid
	if( (retVal=m_df1Lock.Acquire()) == EOK)
	{
		m_df1SrcId = src;
		m_df1ComDev = comDev;

		m_df1Lock.Release();
	}

	return( retVal);
}

/**
 * Close an instance of a DF1 PLC
 *
 * @return 0 if successful. Any other value indicates error
 */
int IDf1Protocol::Close()
{
	int retVal = EOK;

	m_terminate = true;

	// Lock so we know our handle is valid
	if( (retVal=m_df1Lock.Acquire()) == EOK)
	{
		m_df1ComDev = -1;
		m_df1Lock.Release();
	}

	return( retVal);
}

/**
 * Initialize data for DF1 communication
 *
 * @param logger ILogger to use for initializing my logger interface
 */
void IDf1Protocol::Init( ILogger &logger)
{
	m_logger.SetVerboseMask( logger.GetVerboseMask());
	m_logger.SetLocalEcho( logger.GetLocalEcho());
	m_logger.SetProcessName( "DF1Protocol");
	m_logger.SetAdditionalLogFileName( logger.GetAdditionalLogFileName());
	if( logger.GetLogStatus())	m_logger.EnableLogger();
}

/**
 * Initialize data for DF1 communication from a configuration node
 *
 * @param df1Config Configuration node to read the DF1 parameters from
 */
void IDf1Protocol::Initialize( const XmlNode *df1Config)
{
    // load the additional configuration items needed by the IDf1Protocol.
    LoadAdditionalConfigurationItems(df1Config);
}

void IDf1Protocol::LoadAdditionalConfigurationItems(const XmlNode *df1Config)
{
	try
	{
		m_logger.Initialize( df1Config);
	}
	catch( ...)
	{
	}

	try
	{
		m_df1BytesPerBlock = atoi( df1Config->getChild("DF1BlockSize")->getValue().c_str());
	}
	catch( ...)
	{
		m_df1BytesPerBlock = 50;
	}
}

/**
 * Read data from a DF1 PLC
 *
 * @param dest   DF1 destination node ID to which the message is addressed
 * @param buff   Buffer into which the data will be read
 * @param count  Number of 32-bit integers to read
 * @param addr
 *
 * @return true if PLC data was read successfully
 */
bool IDf1Protocol::Read( uint8_t dest, uint32_t *buff, uint32_t count, const char *addr)
{
	const int		inBytes = 2*count;
	int 			loopCount = inBytes / m_df1BytesPerBlock;
	int             loopIdx=0, wordsToUpdate=m_df1BytesPerBlock/2;
	int             cnt=5;
	int             byteCount, wordIdx=m_df1BytesPerBlock/2;
	char			plcAddr[ 20];
	uint8_t			plcData[ 1024];
	bool            good = true;

	// Loop 1 additional time if we need to read a partial block of data
	if( (loopCount*m_df1BytesPerBlock) < inBytes)	loopCount++;

	m_logger.Log( LOG_DEV_DATA, "Enter IDf1Protocol::Read(), Bytes: %d, Block Size: %d, Loops: %d\n",
				  inBytes, m_df1BytesPerBlock, loopCount);

	if( m_df1Lock.Acquire() == EOK)
	{
		// Send all blocks until we encounter errors or someone closes us
		do
		{
			// Assume error
			good = false;

			// Calculate how many bytes to read
			byteCount = m_df1BytesPerBlock;					// 50 Bytes per block max
			if(loopIdx == (loopCount-1))							// if last block to process
			{
				byteCount = inBytes % m_df1BytesPerBlock;	// calc remaining bytes
				wordsToUpdate = count;
			}

			cnt = 5;
			while( (cnt-- > 0) && (false == m_terminate) && (false == good))
			{
				// Calculate Address to Read
				sprintf(plcAddr,"%s:%d", addr, loopIdx*wordIdx);		// Base addr is N16:00

				m_logger.Log( LOG_DEV_DATA, "IDf1Protocol::Read( %d, %d, %s, %d)\n", loopIdx,
							  loopCount, plcAddr, wordsToUpdate);

				// Send the DF1 "Read Block" string
                //ews-  the below function also "eats" the ack message.
				if( BufferRead( dest, byteCount/2, plcAddr) > 0)
				{
					int rspLen = 11 + byteCount;

					// Get PLC response and convert to binary
                    // plcData is the scratchpad for the message
					if( CheckBufferRead(plcData, rspLen, &buff[ loopIdx*(m_df1BytesPerBlock/2)], wordsToUpdate) == 0)
					{
						// How many words left to update
						count -= wordsToUpdate;
						// Tell PLC his message was good
						AckBufferRead();
						// Send request for next block
						cnt = 0;
						good = true;
					}
					else
					{
						m_logger.Log( LOG_ERRORS, "\tError in CheckBufferRead( %s)\n", plcAddr);
						AckBufferRead();				// say !ok
						delay(30);
					}
				}
				else
				{
					AckBufferRead();					// say !ok
					delay(30);
				}
			}
		} while( (++loopIdx < loopCount) && (true == good) && (false == m_terminate));

		m_df1Lock.Release();
	}
	else
	{
		good = false;
	}

	m_logger.Log( LOG_DEV_DATA, "Exit IDf1Protocol::Read(), status=%s\n", good == true ? "true" : "false");

	return(good);
}

/**
 * Write data to a DF1 PLC
 *
 * @param dest   DF1 destination node ID to which the message is addressed
 * @param buff   Buffer from which the data will be written
 * @param count  Number of 32-bit integers to write
 * @param addr   PLC Address to write our data to
 *
 * @return true if data was written successfully
 */
bool IDf1Protocol::Write( uint8_t dest, const uint32_t *buff, uint32_t count, const char *addr)
{
	const int		outBytes = 2*count;
	int             junk, loopCount = outBytes / m_df1BytesPerBlock;
	int             loopIdx=0, bytesWritten=0, wordIdx=m_df1BytesPerBlock/2;
	int             cnt = 5;
	short			byteCount;
	char			plcAddr[ 20];
	uint8_t			plcData[ 1024];
	bool            good = true;

	if( (2*count) % m_df1BytesPerBlock)	loopCount++;

	if( m_df1Lock.Acquire() == EOK)
	{
		// Send all blocks until we encounter errors or someone closes us
		do
		{
			// Assume error
			good = false;

			// Calculate how mant bytes to read
			byteCount = m_df1BytesPerBlock;
			// if last block to process
			if(loopIdx == (loopCount-1))
			{
				// calc remaining bytes
				byteCount = outBytes % m_df1BytesPerBlock;
			}

			// Try a few times to send this block
			cnt = 5;
			while( (cnt-- > 0) && (false == good) && (false == m_terminate))
			{
				// Calculate Address to write to
				sprintf(plcAddr,"%s:%d", addr, loopIdx*(wordIdx));

				m_logger.Log( LOG_DEV_DATA, "IDf1Protocol::Write( %d, %d, %s, %d)\n", loopIdx, loopCount, plcAddr, byteCount);

				// Try to write to the PLC
				if( (bytesWritten=BufferWrite( dest, byteCount/2, &buff[ loopIdx*wordIdx], plcAddr)) > 0)
				{
					// Wait for PLC response
					if( (junk=WaitForAllData(plcData, 1000)) > 0)
					{
						// Tell PLC his message was good
						AckBufferRead();
						// Send request for next block
						cnt = 0;
						good = true;
					}
					else
					{
						m_logger.Log( LOG_ERRORS, "\tIDf1Protocol::Write WaitForAllData error: returned %d, TNX: $%04X\n",
									  junk, m_df1TnsCounter);
						AckBufferRead();
						delay(30);
					}
				}
				else
				{
					m_logger.Log( LOG_ERRORS, "\tIDf1Protocol::Write BufferWrite error\n");
					AckBufferRead();
					delay(30);
				}
			}
		} while( (++loopIdx < loopCount) && (true == good) && (false == m_terminate));
		m_df1Lock.Release();
	}
	else
	{
		good = false;
	}

	return( good);
}


bool IDf1Protocol::IsResponseMessageValid( uint8_t *buff, int cnt, short byteCnt)
{
    bool retval = true; // assume everything is ok, any test below can invalidate
	unsigned int localTNS, plcTNS;
    const int TNSOffset = 6;

    m_logger.Log( LOG_FN_ENTRY,"IDf1Protocol::IsResponseMessageValid() - Enter\n");

    // additional checks for length and transaction code
	localTNS = GetTNSCntr();
	plcTNS = buff[TNSOffset+1] * 0x100 + buff[TNSOffset];
	if(plcTNS != localTNS)
	{
        m_logger.Log( LOG_ERRORS,
                      "\tInvalid Transaction number. Sent: %04X, Received: %04X\n",
					  localTNS, plcTNS);
        retval = false;
    }
    if (cnt != byteCnt)
    {
        m_logger.Log( LOG_ERRORS,
                      "\tInvalid Message Length. Expected: %d, Received: %d\n",
                      byteCnt ,cnt);
        retval = false;
    }
    if (ValidateCrcRaw( buff, cnt) != 0)
    {
        m_logger.Log( LOG_ERRORS, "\tInvalid checksum\n");
        retval = false;
    }
    m_logger.Log( LOG_FN_ENTRY,
                  "IDf1Protocol::IsResponseMessageValid() - Exit - Returning: %s\n",
                  retval ? "true" : "false");
    return( retval);
}

/**
 * Receives and validates a PLC DF1 reply message
 *
 * @param buff      Buffer into which the DF1 data should be received
 * @param byteCnt   Maximum number of bytes to receive from the PLC
 * @param plcData   Array into which converted binary data should be put
 * @param dataCount Maximum number of PLC words to convert into the plcData array
 * @return 0 if buffer is valid; any other value indicates an error
 */
long IDf1Protocol::CheckBufferRead( uint8_t *buff, short byteCnt, uint32_t *plcData, int dataCount)
{
	register long   cnt;
	long            msgError = 1;

	// Wait for all data to be received
	if( (cnt=WaitForAllData(buff, byteCnt)) > 0)
	{
        // cnt holds the number of bytes read !
        // above function cleans up double stuffed DLEc characters
		// Validate the DF1 data
        if (IsResponseMessageValid(buff, cnt, byteCnt))
        {
			// Convert DF1 serial bytes to 32-bit integers
			if( ExtractData( buff, cnt, plcData, dataCount) > 0)
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
            m_logger.Log( LOG_ERRORS, "\tInvalid response message, not processed.\n");
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
 * @param destId  DF1 destination node ID to which the message is addressed
 * @param byteCnt Number of integers to transfer
 * @param plcOut  Pointer to start of array of integers to be transferred
 * @param plcAddr PLC Address (i.e. N16:0) into which the integers should be transferred
 * @return Number of bytes transmitted
 */
long IDf1Protocol::BufferWrite( uint8_t destId, short byteCnt, const uint32_t *plcOut, char *plcAddr)
{
	register uint32_t  	nn=0,cnt=0, mm=0;
	register uint8_t	*pp;
	unsigned int		tns,out_buf, bytesWritten=0;

	// Count the extra DLE characters in the data to write
	//	18 = number of additional protocol bytes in the message
	mm = 18 + strlen( plcAddr) + 2*CalculateBufferLen(plcOut, byteCnt);
	if( mm > 0)
	{
		const int	buffSz = mm;
		char		xmtBuff[ buffSz];

		nn = mm = 0;
		pp = (uint8_t*)xmtBuff;
		pp[ nn++] = DLEc;		// <DLE>
		pp[ nn++] = STXc;		// <STX>
		pp[ nn++] = destId;		// <DST>
		pp[ nn++] = m_df1SrcId;	// <SRC>
		pp[ nn++] = 0x0F;		// AB command
		pp[ nn++] = 0x00;		// status - send is always 0

		tns = IncTNSCntr();
		pp[ nn++] = (char)(tns & 0x00FF);
		pp[ nn++] = (char)((tns & 0xFF00) >> 8);

		pp[ nn++] = 0x00;		// AB function #
		pp[ nn++] = 0x00;		// AB packet offset
		pp[ nn++] = 0x00;		// AB packet offset

		pp[ nn++] = byteCnt;	// AB total transactions -low byte in hex
		pp[ nn++] = 0x00;		// AB total transactions -hi byte in hex

		pp[ nn++] = 0x00;		// AB address NULL  (BASE ADDRESS = $N16:00)
		while(plcAddr[mm])	pp[nn++] = plcAddr[mm++];
		pp[ nn++] = 0x00;		// AB address NULL

		cnt = 0;
		while( byteCnt--)
		{
			uint16_t	plcWord = plcOut[ cnt];
			// low byte to buffer
			out_buf = (plcWord & 0x00FF);
			pp[ nn++] = out_buf;
            // the next line double stuffs DLEc characters
			if(out_buf == 0x10)		pp[ nn++] = out_buf;

			// high byte to buffer
			out_buf = ((plcWord & 0xFF00) >> 8);
			pp[ nn++] = out_buf;
            // the next line double stuffs DLEc characters
			if(out_buf == 0x10)		pp[ nn++] = out_buf;

			// next register
			cnt++;
		}

		pp[ nn++] = DLEc;		// <DLE>
		pp[ nn++] = ETXc;		// <ETX>

		// skip first two and last two
		pp[ nn++] = (char)BccCalc( &pp[2], nn - 4);
		if( ValidateCrc( pp, nn) == 0)
		{
			char ackRsp = ENQc;
			char	prefix[ 1024];

			sprintf( prefix, "Writing %d bytes", nn);
			LogPlcString( LOG_DEV_DATA, prefix, pp, nn);

			cnt = 0;
			while( (ACKc != ackRsp) && (cnt++ < 3))
			{
			// Try to send a "Write Block" to the PLC
			if( (bytesWritten = write( m_df1ComDev, pp, nn)) > 0)
			{
					ackRsp = WaitForAck();
					if(ACKc != ackRsp)
					{
					// return error
					bytesWritten = 0;
				}
			}
		}
	}
	}

	return(bytesWritten);
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
uint32_t IDf1Protocol::CalculateBufferLen( const uint32_t *plcOut, uint32_t buffLen)
{
	register uint32_t nn, retVal = buffLen;
	// Count the extra DLE characters in the data to write
	for( nn=0; nn<buffLen; nn++)
	{
		if( (plcOut[ nn] & 0x00FF) == 0x0010)	retVal++;
		if( (plcOut[ nn] & 0xFF00) == 0x1000)	retVal++;
	}

	return( retVal);
}

/**
 * Reads integer data from the PLC
 *
 * @param destId  DF1 destination node ID to which the message is addressed
 * @param byteCnt Number of integers to read
 *      (do NOT pass in 0x08 or 0x10 as byteCnt - bad things could happen)
 * @param plcAddr PLC Address (i.e. N17:0) from which the data should be read
 * @return Number of integers read
 */
long IDf1Protocol::BufferRead( uint8_t destId, short byteCnt, char *plcAddr)
{
	const uint32_t		buffLen = 19 + strlen( plcAddr);
	uint8_t				xmtBuff[ buffLen];
	register uint32_t  	nn=0, mm=0;
	register uint8_t	*pp;
	unsigned int		tns;
	int					bytesRead = 0;

	pp = xmtBuff;
	pp[nn++] = DLEc;		// <DLE>
	pp[nn++] = STXc;		// <STX>
	pp[nn++] = destId;		// <DST>
	pp[nn++] = m_df1SrcId;	// <SRC>
	pp[nn++] = 0x0F;		// AB command
	pp[nn++] = 0x00;		// status - send is always 0

	tns = IncTNSCntr();
	pp[nn++] = (tns & 0x00FF);
	pp[nn++] = ((tns & 0xFF00) >> 8);

	pp[nn++] = 0x01;		// AB function #
	pp[nn++] = 0x00;		// AB packet offset
	pp[nn++] = 0x00;		// AB packet offset

	pp[nn++] = byteCnt; 	// AB total transactions -low byte in hex
	pp[nn++] = 0x00;		// AB total transactions -hi byte in hex

	pp[nn++] = 0x00;		// AB address NULL  (BASE ADDRESS = $N16:00)

	mm = 0;
	while(plcAddr[mm])	pp[nn++] = plcAddr[mm++];
	pp[nn++] = 0x00;		// AB address NULL

	pp[nn++] = 2*byteCnt; 	// message size to be sent back by PLC (2 bytes per word)
	pp[nn++] = DLEc;		// <DLE>
	pp[nn++] = ETXc;		// <ETX>

	// skip first two and last two
	pp[nn++] = (BccCalc ( &pp[2] , nn - 4 ));

	pp[nn] = 0;             // null terminate

	// Make sure our CRC is valid before sending
	if( ValidateCrc( xmtBuff, nn) == 0)
	{
		char    ackRsp = ENQc;
		char	prefix[ 1024];
		int cnt = 0;

		sprintf( prefix, "Sending (%d bytes, %d calc)", nn, buffLen);
		LogPlcString( LOG_DEV_DATA, prefix, pp, nn);

		while( (ackRsp != ACKc) && (cnt++ < 3))
		{
		// Try to send "Read Block" to the PLC
		if( (bytesRead = write( m_df1ComDev, pp, nn)) > 0)
		{
				ackRsp = WaitForAck();
			// If PLC responds with anything other than ACK
				if(ackRsp != ACKc)
				{
				// return error
				bytesRead = 0;
			}
		}
		else if( bytesRead == -1)
		{
			m_logger.Log( LOG_ERRORS, "\tIDf1Protocol::BufferRead ERROR: %s", strerror( errno));
		}
	}
	}

	return(bytesRead);
}

/**
 * Acknowledge a successful read operation by sending ACK to the PLC
 */
void IDf1Protocol::AckBufferRead()
{
	const uint8_t	plcOut[] = { DLEc, ACKc };

	write( m_df1ComDev, plcOut, sizeof( plcOut));
}

/**
 * Notify the PLC of a failed read operation by sending NAK to the PLC
 */
void IDf1Protocol::NAkBufferRead()
{
	const uint8_t	plcOut[] = { DLEc, NAKc};

	write( m_df1ComDev, plcOut, sizeof( plcOut));
}

/**
 * Request re-transmission of the last read operation
 */
void IDf1Protocol::ENQBufferRead()
{
	const uint8_t	plcOut[] = { DLEc, ENQc };

	write( m_df1ComDev, plcOut, sizeof( plcOut));
}

/**
 * Increment the transaction counter
 *
 * @return Incremented transation counter
 */
unsigned int IDf1Protocol::IncTNSCntr()
{
	unsigned int retVal = (uint32_t)-1;
	if( m_df1Lock.Acquire() == EOK)
	{
		m_df1TnsCounter++;
		// Avoid DLE's in the data stream
		if( (m_df1TnsCounter & 0xFF00) == 0x1000)	m_df1TnsCounter += 0x0100;
		if( (m_df1TnsCounter & 0x00FF) == 0x0010)	m_df1TnsCounter += 0x0001;

		retVal = m_df1TnsCounter;
		m_df1Lock.Release();
	}
	return( retVal);
}

/**
 * Get the most recent transaction counter
 *
 * @return transation counter
 */
unsigned int IDf1Protocol::GetTNSCntr()
{
	unsigned int retVal = (uint32_t)-1;
    if (m_df1Lock.Acquire() == EOK)
    {
        retVal = m_df1TnsCounter;
        m_df1Lock.Release();
    }
    return( retVal);
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
unsigned short IDf1Protocol::BccCalc( register uint8_t *pp, register short len)
{
	unsigned short	crc1=0x0000;
	register short	xx;

	for(xx=0; xx<len; xx++)                     // do this many
	{
		if( (pp[xx] == 0x10) && (pp[xx+1] == 0x10) ) xx++;
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
unsigned short IDf1Protocol::BccCalcRaw( register uint8_t *pp, register short len)
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
int IDf1Protocol::WaitForAllData( uint8_t *rxBuff, int count)
{
	int         totalRead=0, bytesRead=0, bytesRemaining=2, bytesAvail=0, loopCount=25;
	uint64_t    timeOut = mSEC_nSEC( 10);
	uint8_t     curData=0, lastData=0, ii;

	m_logger.Log( LOG_FN_ENTRY, "Enter IDf1Protocol::WaitForAllData(), expecting: %d bytes\n", count);

	// Loop until all expected data has been received
	while( (totalRead < count) && (bytesRemaining > 0) && (false == m_terminate) && (loopCount > 0))
	{
		// See if there is any data available to read
		if( (bytesAvail = tcischars( m_df1ComDev)) > 0)
		{
			m_logger.Log( LOG_DEV_DATA, "%d bytes available to read\n", bytesAvail);

			// Only read as much as the caller can handle
			if( bytesAvail > (count - totalRead))
			{
				bytesAvail = count - totalRead;
				m_logger.Log( LOG_DEV_DATA, "Reading max of %d bytes due to buffer size\n", bytesAvail);
			}

			// Read each byte 1 by 1 to check for duplicate ACKs
			for( ii=0; ((ii<bytesAvail) && (bytesRemaining>0) && (totalRead < count)); ii++)
			{
		bytesRead = read( m_df1ComDev, &curData, 1);
		if( bytesRead > 0)
		{
            //ews- at this point, bytesRead == 1
			// Either the first char or not a duplicate DLE, so
			//	put into caller's buffer
            //ews- looks like the above comment is out of date, always put in
            // will be removed below if necessary
					rxBuff[ totalRead++] = curData;

			switch( curData)
			{
			case DLEc:
				// If this is a duplicate DLE
				if( lastData == DLEc)
				{
					// Remove the 2nd DLE from the caller's buffer
					if( totalRead)	totalRead--;

					// Make sure that, when this byte gets shifted into lastData, that it
					//	does not get interpreted as a DLEc. This will allow the ETXc, ACKc
					//	and NAKc cases below to properly to identify the end of the message
					curData = DLEc ^ 0xFF;
				}
						// Not a duplicate DLE
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
				//	is the last character in the packet (the CRC byte). This
				//	is only true if the <DLE> was not a duplicate <DLE>
				if( lastData == DLEc)
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
				if( lastData == DLEc)
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
				if( lastData == DLEc)
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

			default:
				// If we are only expecting one more byte (the CRC byte)
				if( bytesRemaining == 1)
				{
					// Say no more data to come
					bytesRemaining = 0;
				}
				break;
			}

			// Hold onto this latest char
			lastData = curData;
		}
		else if( bytesRead < 0)
		{
					m_logger.Log( LOG_ERRORS, "\tIDf1Protocol::WaitForAllData read ERROR(%d): %s\n",
								  errno, strerror( errno));
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
		}

		// Delay a bit if we did not read anything
		if( (bytesAvail <= 0) || (bytesRead <= 0))
		{
			int logMask = LOG_DEV_DATA;

			// Set a timeout on the read operation to try and detect the end
			TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_NANOSLEEP, NULL, &timeOut, NULL);
			loopCount--;

			// If there is data available and we did not read anything
			if( (bytesAvail > 0) && (bytesRead <= 0))	logMask |= LOG_ERRORS;

			m_logger.Log( logMask, "\tWaitForAllData() Available: %d; Read:%d; Loop %d\n",
						  bytesAvail, bytesRead, loopCount);
		}
	}

	// If we received any data
	if( totalRead)
	{
		LogPlcString( LOG_DETAILED_DATA | LOG_DEV_DATA, "WaitForAllData Received", rxBuff, totalRead);
	}
	else if( (bytesAvail < 0) || (bytesRead < 0))
	{
		m_logger.Log( LOG_ERRORS, "\tIDf1Protocol::WaitForAllData read (%d): %s\n", errno, strerror( errno));
	}

	m_logger.Log( LOG_FN_ENTRY, "Exit IDf1Protocol::WaitForAllData(), %d bytes read\n", totalRead);
	return( totalRead);
}

/**
 * Validates a DF1 CRC
 *
 * @param df1Data DF1 serial string to validate
 * @param df1Size number of bytes in the DF1 serial string
 * @return 0 if CRC is valid, 1 if it is invalid
 */
int IDf1Protocol::ValidateCrc( uint8_t* df1Data, int df1Size)
{
	register uint8_t   *pp = df1Data;
	register long   	nn = 1, cnt = df1Size;
	long            	msgError = 1;
	unsigned int		stx_ptr=0, etx_ptr=0;
	uint8_t				crc;

	// Find message frame
	while(nn < cnt)
	{
		// Find first occurrence of <10> <02>
		if((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))	stx_ptr = nn;
		// Find last occurrence of <10> <03>
		if((pp[nn] == ETXc) && (pp[nn-1] == DLEc))						etx_ptr = nn;

		nn++;
	}

	cnt = (etx_ptr - 2) - (stx_ptr + 1) + 1;
	// If we found a valid frame
	if( (cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(df1Size-1)))
	{
    	// If CRC is valid
    	crc = (uint8_t)BccCalc( &pp[stx_ptr+1], cnt);
    	if((pp[etx_ptr+1]) == crc)
    	{
    		msgError = 0;
    	}
    	else
    	{
    		m_logger.Log( LOG_ERRORS, "\tInvalid CRC (%d, %d, %d) $%02hhX != $%02hhX\n",
    					  df1Size, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);
    		LogPlcString( LOG_ERRORS, "\tCRC Data", df1Data, df1Size);
    	}
	}
	else
	{
		m_logger.Log( LOG_ERRORS, "\tInvalid DF1 string (%d, %d, %d): Invalid DF1 frame\n",
					  df1Size, stx_ptr, etx_ptr);
		LogPlcString( LOG_ERRORS, "\tFrame not found", df1Data, df1Size);
	}
	return( msgError);                               // return no_error
}

/**
 * Validates a DF1 CRC
 *
 * @param df1Data DF1 serial string to validate
 * @param df1Size number of bytes in the DF1 serial string
 * @return 0 if CRC is valid, 1 if it is invalid
 */
int IDf1Protocol::ValidateCrcRaw( uint8_t* df1Data, int df1Size)
{
    register uint8_t   *pp = df1Data;
    register long     nn = 1, cnt = df1Size;
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
    if ((cnt > 0) && (stx_ptr > 0) && (etx_ptr > 0) && (etx_ptr < (unsigned int)(df1Size-1)))
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
                          df1Size, stx_ptr, etx_ptr, pp[etx_ptr+1], crc);
            LogPlcString( LOG_ERRORS, "\tCRC Data", df1Data, df1Size);
        }
    }
    else
    {
        m_logger.Log( LOG_ERRORS, "\tInvalid DF1 string (%d, %d, %d): Invalid DF1 frame\n",
                      df1Size, stx_ptr, etx_ptr);
        LogPlcString( LOG_ERRORS, "\tFrame not found", df1Data, df1Size);
    }
    return( msgError);                               // return no_error
}

/**
 * Extracts PLC data from a DF1 serial string
 *
 * @param df1Data   DF1 serial string to extract data from
 * @param df1Size   Number of bytes in the DF1 serial string
 * @param plcData   Array into which converted data will be written
 * @param dataCount Number of elements in the plcData array
 * @return Number of 32-bit integers converted and written to the plcData array
 */
int IDf1Protocol::ExtractData( uint8_t *df1Data, int df1Size, uint32_t *plcData, int dataCount)
{
	register uint8_t   *pp;
	register long   	nn,cnt;
	uint32_t			stx_ptr,etx_ptr;
	uint16_t			in_buf;

	pp = df1Data;
	cnt = df1Size;
	in_buf = stx_ptr = etx_ptr = 0;

	nn = 1;
	// Find message frame
	while(nn < cnt)
	{
		// Find first occurrence of <10> <02>
		if((pp[nn] == STXc) && (pp[nn-1] == DLEc) && (stx_ptr == 0))	stx_ptr = nn;
		// Find last occurrence of <10> <03>
		if((pp[nn] == ETXc) && (pp[nn-1] == DLEc))						etx_ptr = nn;

		nn++;
	}

	// if valid data bufferthen move string into variables
	if( (plcData != NULL) && ( dataCount > 0))
	{
		cnt = stx_ptr + 7;		// variable data at this position
		nn = 0;
		while( (cnt < df1Size) && (nn < dataCount))	// stop when all bytes are done
		{
			in_buf = (pp[cnt] | (pp[cnt+1] << 8) );	// Combine bytes into word
			plcData[ nn++] = in_buf;				// stuff data into global memory
			cnt += 2;								// go to next word
		}
	}
	return( nn);
}

/**
 * Wait for the PLC to ACK/NAK our commands
 *
 * @return NAKc if NAK received
 *         ACKc if ACK recevied
 *         ENQc if ENQ received
 */
char IDf1Protocol::WaitForAck()
{
	uint8_t	plcData[ 10];
	bool	keepWaiting=true;
	int     enqCount=0, errCnt=0, readIdx=0, readCount=0;
	char    retVal = NAKc;

		// Initialize to junk
		plcData[ 0] = plcData[ 1] = DLEc ^ 0xFF;

	// Wait until and ACK or other recognized command comes in
	while( (keepWaiting) && (false == m_terminate))
	{
		if( (readCount=WaitForAllData(&plcData[ readIdx], 2-readIdx)) > 0)
		{
			readIdx += readCount;
			if( readIdx > 1)
			{
				// We only need 2 bytes for an ACK. If we have at least 2 bytes and it
				// is not an ACK, leave and (possibly) resend our last message. If we did
				// get an ACK, we're done here.
				keepWaiting = false;

				// If this looks like a command
				if( plcData[ 0] == DLEc)
				{
					// If it is an ACK
					if( plcData[ 1] == ACKc)
					{
						m_logger.Log( LOG_DEV_DATA, "\tGot ACKc\n");
					}
					// Not an ACK...check to see what it is
					else
					{
						// Flush our FIFO
						tcflush( m_df1ComDev, TCIOFLUSH);
						if( plcData[ 1] == NAKc)
						{
							m_logger.Log( LOG_ERRORS, "\tGot NAK while waiting for ACK\n");
						}
						else if( plcData[ 1] == ENQc)
						{
							// This is the PLC's way of saying that we haven't ACK'd his last message
							// If this hasn't happened too many time, try to ACK the PLC
							if( enqCount++ < 3)
							{
								AckBufferRead();
								keepWaiting = true;
							}
							m_logger.Log( LOG_ERRORS, "\tGot ENQ while waiting for ACK: %d\n", enqCount);
						}
						else
						{
							// Tell the PLC that we did not like hos last message
							NAkBufferRead();
							m_logger.Log( LOG_ERRORS, "\tGot unknown command while waiting for ACKc: %02hhX\n", plcData[ 1]);
						}
					}
				}
				else
				{
					tcflush( m_df1ComDev, TCIOFLUSH);
					LogPlcString( LOG_ERRORS, "\tERROR did not get ACK", plcData, readIdx);
				}

				// Reset our read pointer back to the start of the buffer
				readIdx = 0;

				retVal = plcData[ 1];

				// Re-initialize to junk
				plcData[ 0] = plcData[ 1] = DLEc ^ 0xFF;
			}
		}
		// If 2nd error, leave
		else if( errCnt++)
		{
			keepWaiting = false;
		}
	}

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
void IDf1Protocol::LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
{
	uint32_t	ii;

	// If any data to print and correct verbosity
	if( m_logger.GetVerboseMask() & logMask)
	{
		std::string	logBuff("");
		char		junk[ 5];

		if( prefix != NULL)		logBuff = std::string( prefix) + std::string( ": ");

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

