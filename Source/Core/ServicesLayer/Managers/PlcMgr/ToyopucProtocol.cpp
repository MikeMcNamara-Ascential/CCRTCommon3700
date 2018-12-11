//******************************************************************************
// Description:
//    Toyopuc protocol wrapper class
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
#include "ToyopucProtocol.h"

//---------------------------------------------------------------------------------------------------------------------
ToyopucProtocol::ToyopucProtocol() : m_terminate(false)
{
}

//---------------------------------------------------------------------------------------------------------------------
ToyopucProtocol::~ToyopucProtocol()
{
}

//---------------------------------------------------------------------------------------------------------------------
int ToyopucProtocol::Close()
{
   int retVal = EOK;
   m_terminate = true;
   // Lock so we know our handle is valid
   if ((retVal = m_df1Lock.Acquire()) == EOK)
   {
      m_df1Lock.Release();
      //m_toyoComm.Close();
   }
   return (retVal);
}

//---------------------------------------------------------------------------------------------------------------------
void ToyopucProtocol::Initialize(const XmlNode *df1Config)
{
   // load the additional configuration items needed by the ToyopucProtocol.
   LoadAdditionalConfigurationItems(df1Config);
   //m_toyoComm = new ToyopucWrapper(&m_logger);

   m_toyoComm.Initialize(GetPlcCommSetup());
}

//---------------------------------------------------------------------------------------------------------------------
void ToyopucProtocol::LoadAdditionalConfigurationItems(const XmlNode *df1Config)
{
   try
   {
      m_logger.Initialize(df1Config);
   }
   catch (...)
   {
   }
   try
   {
      SetPlcCommSetup(df1Config->getChild("Setup/PLCCommunication"));
   }
   catch (...)
   {
      m_logger.Log(LOG_ERRORS, "PLC Comm Setup Failed");
   }
}

//---------------------------------------------------------------------------------------------------------------------
bool ToyopucProtocol::Read(uint32_t *buff, uint32_t count, const string& ipAddress, const char *addr)
{
   const int       inBytes = 2 * count;
   int             cnt = 5;
   string          plcAddr(addr);
   bool            good = false;

   m_logger.Log(LOG_DEV_DATA, "Enter ToyopucProtocol::Read(), Tag: %s, Bytes: %d", addr, inBytes);

   if (m_df1Lock.Acquire() == EOK)
   {
      while ((cnt-- > 0) && (false == m_terminate) && (false == good))
      {
         m_logger.Log(LOG_DEV_DATA, "ToyopucProtocol::Reading tag %s, words: %d", plcAddr.c_str(), count);
         // Send the Read Tag string
         good = (BufferRead(inBytes, ipAddress, plcAddr, buff) >= 0);
         m_logger.Log(LOG_ERRORS, "\tToyopucProtocol::Read status %s", good ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE);
         if (!good)  BposSleep(10);
      }

      m_df1Lock.Release();
   }

   m_logger.Log(LOG_DEV_DATA, "Exit ToyopucProtocol::Read(), status=%s\n", good ? "true" : "false");

   return (good);
}

//---------------------------------------------------------------------------------------------------------------------
bool ToyopucProtocol::Write(const uint32_t *buff, uint32_t count, const string& ipAddress, const char *addr)
{
   int             bytesWritten = 0, cnt = 5;
   string          plcAddr(addr);
   bool            good = false;

   m_logger.Log(LOG_DEV_DATA, "ToyopucProtocol::Write() , Tag: %s, Words: %d, Bytes: %d", plcAddr.c_str(), count, count * 2);

   if (m_df1Lock.Acquire() == EOK)
   {   // Try a few times to send this block
      while ((cnt-- > 0) && (false == good) && (false == m_terminate))
      {   // Try to write to the PLC
         if ((bytesWritten = BufferWrite(count, buff, ipAddress, plcAddr)) > 0)
         {
            good = true;
         }
         else
         {
            m_logger.Log(LOG_ERRORS, "\tToyopucProtocol::Write BufferWrite error");
            BposSleep(10);
         }
      }
      m_df1Lock.Release();
   }

   m_logger.Log(LOG_DEV_DATA, "Exit ToyopucProtocol::Write(), status: %s, bytes written: %d",
                good ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE, bytesWritten);
   return (good);
}

//---------------------------------------------------------------------------------------------------------------------
long ToyopucProtocol::BufferWrite(short byteCnt, const uint32_t *plcOut, const string& plcIpAddr, const string& plcTag)
{
   register uint32_t   cnt = 0, nn = 0;
   short               numWords = byteCnt;
   uint8_t             plcData[byteCnt * 2];
   char                prefix[1024];
   // Break words down into individual bytes
   while (numWords--)
   {
      uint16_t    plcWord = plcOut[cnt];
      // low byte to buffer
      plcData[nn++] = (plcWord & 0x00FF);
      // high byte to buffer
      plcData[nn++] = ((plcWord & 0xFF00) >> 8);
      // next register
      cnt++;
   }
   sprintf(prefix, "Writing %d bytes to %s", nn, plcTag.c_str());
   LogPlcString(LOG_DEV_DATA, prefix, plcData, nn);
   // Send the data to the PLC
   INT32 retStat = -1;
   if (BEP_STATUS_SUCCESS == SendPlcData(plcData, byteCnt * 2))
   {
      retStat = nn;
   }
   else
   {
      retStat = 0;
   }
   m_logger.Log(LOG_DEV_DATA, "Wrote %d bytes to the PLC", retStat);
   return retStat;
}

//---------------------------------------------------------------------------------------------------------------------
long ToyopucProtocol::BufferRead(short byteCnt, const string& plcIpAddr, const string& plcTag, uint32_t *plcData)
{
   uint8_t readBuf[512];
   memset(readBuf, 0, sizeof(readBuf));
   INT32   bytesRead = -1, status = 0;
   unsigned char countChar = (unsigned char)byteCnt;
   unsigned char req[] = { '\x00', '\x00', '\x06', '\x00', '\x96', '\x01', '\x00', '\x22', countChar, '\x00' };
   //string request(req);

   m_logger.Log(LOG_DEV_DATA, "Reading PLC Tag: %s, %d words", plcTag.c_str(), byteCnt);
   // 0x00 0x00 (Begin Read Message)
   // 0x06 0x00 (Send a 6 byte message)
   // 0x96 0x01 (Read PLC Command)
   // 0x00 0x22 (Read Location)
   // 0x
   //request = "" + (char)NULc + (char)NULc + (char)0x06 + (char)0x00 + (char)tRPLC + (char)0x01 + (char)0x00 + (char)tINP + (char)byteCnt + (char)0x00;
   // INT32 retStat = m_toyoComm.ReceivePlcData(readBuf, byteCnt*2, bytesRead, plcIpAddr, plcTag);
   // clear incoming buffer
   m_toyoComm.ResetConnection();
   m_logger.Log(LOG_DEV_DATA, "Cleared incoming buffer Toyopuc");
   status = m_toyoComm.Send(req, 10);
   m_logger.Log(LOG_DEV_DATA, "Request for data sent. Status: %d\n", status);
   if (status != BEP_STATUS_SUCCESS)
   {
      m_logger.Log(LOG_DEV_DATA, "Error requesting plc data");
   }
   if (status == BEP_STATUS_SUCCESS)
   {
      status = GetPlcData(readBuf, bytesRead);
      ExtractData(readBuf, byteCnt, plcData, byteCnt);
      char prefix[1024];
      sprintf(prefix, "Bytes from PLC: ");
      LogPlcString(LOG_DEV_DATA, prefix, readBuf, bytesRead);
   }
   return (bytesRead);
}

BEP_STATUS_TYPE ToyopucProtocol::GetPlcData(uint8_t *response, INT32& numBytes)
{
   SerialString_t plcResponse;
   INT32 numBytesRead;
   BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
   m_logger.Log(LOG_FN_ENTRY, "Enter ToyopucProtocol::GetPlcData\n");
   // Temporarily setting to 3000 ms as max response time for prototype
   numBytesRead = m_toyoComm.ReadPort(plcResponse, 500, m_toyoComm.GetTimeoutTime());
   m_logger.Log("Got <0x%02X> from PLC [ACK:0x%02X, NAK:0x%02X] - %d total bytes\n", plcResponse[0], tACK, tNAK, numBytesRead);
   if (numBytesRead > 0)
   {
      numBytes = numBytesRead;
      // Previously had if !(numBytesRead > 1)?? Not sure why it was there but figure I should note it anyway.
      if ((tACK == plcResponse[0]))
      {
         // Log the data we received from the workcell controller
         char buf[1024];
         string logMessage("Complete response from PLC:\n");

         //for (UINT16 index = 0; index < numBytesRead; index++)
         //{   // Create the log message
             //logMessage +=
         //   string logLine = CreateMessage(buf, sizeof(buf), "\t\tindex: %03d -- %c <0x%02X>", //\n",
         //                                  index, isprint(plcResponse[index]) ? plcResponse[index] : '?', plcResponse[index]);
         //   m_logger.Log(LOG_DEV_DATA, "%s", logLine.c_str());
         //}
         //Log(LOG_DEV_DATA, "%s\n", logMessage.c_str());
         status = BEP_STATUS_SUCCESS;
      }
      else if (tNAK == plcResponse[0])
      {   // Display a prompt letting everyone know a NAK was received
         string resp;
         m_logger.Log(LOG_DEV_DATA, "Recieved NAK from PLC\n");
         status = BEP_STATUS_FAILURE;
      }
      else
      {   // Broadcast has no information for the requested vin
         m_logger.Log(LOG_DEV_DATA, "PLC error");
         status = BEP_STATUS_NA;
      }
      m_logger.Log(LOG_DEV_DATA, "Preparing response for processing..");
      if (numBytesRead > 1) for (UINT16 index = 1; index < numBytesRead; index++) response[index-1] = (char)plcResponse[index];
      else
      {
         m_logger.Log(LOG_DEV_DATA, "No data to process beyond fist byte... Just returning ACK/NAK");
         response += (char)plcResponse[0];
      }
   }
   else
   {
      m_logger.Log(LOG_ERRORS, "Timeout waiting for a response from the PLC\n");
   }
   m_logger.Log(LOG_FN_ENTRY, "Exit ToyopucProtocol::GetPlcData");
   return status;
}

BEP_STATUS_TYPE ToyopucProtocol::SendPlcData(uint8_t *plcData, int numBytes)
{
   SerialString_t plcResponse;
   INT32 readId = 0;
   INT32 retStat = -1, status = 0, numBytesRead = 0;
   m_logger.Log(LOG_DEV_DATA, "ToyopucProtocol::SendPlcData() enter\n");
   int fullMsgSize = (numBytes + 8);
   int interm = numBytes + 4;
   char msgSize = '0' + interm;
   m_logger.Log("msgSize: %d numBytes: %d fullMsgSize: %d", msgSize, numBytes, fullMsgSize);
   // YEAH I HARDCODED THE SIZE VALUE.  SORRY.
   unsigned char msgHeader[] = { '\x00', '\x00', '\x74', '\x00', '\x97', '\x01', '\x00', '\x20' };
   unsigned char fullMessage[fullMsgSize];

   memset(fullMessage, 0, sizeof(fullMessage));

   // set message header
   for (int i = 0; i < 8; i++)
   {
      m_logger.Log("Setting byte: %d", i);
      fullMessage[i] = msgHeader[i];
   }
   int c = 0;
   // set PLC bytes to remaining values
   for (int x = 8; x < numBytes + 8; x++)
   {
      fullMessage[x] = (unsigned char)plcData[c];
      //m_logger.Log("Setting byte: %d of full message", x);
      c++;
   }

   m_toyoComm.ResetConnection();
   m_logger.Log(LOG_DEV_DATA, "Cleared incoming buffer Toyopuc");

   status = m_toyoComm.Send(fullMessage, fullMsgSize);
   if (status != BEP_STATUS_SUCCESS)
   {
      m_logger.Log(LOG_DEV_DATA, "Error sending plc data");
      return BEP_STATUS_ERROR;
   }

   else
   {
      BposSleep(30);
      numBytesRead = m_toyoComm.ReadPort(plcResponse, 1000, m_toyoComm.GetTimeoutTime());
      m_logger.Log("Got <0x%02X> from PLC [ACK:0x%02X, NAK:0x%02X] - %d total bytes\n", plcResponse[0], tACK, tNAK, numBytesRead);
      if (plcResponse[0] != tACK)
      {
         m_logger.Log(LOG_ERRORS, "Error sending data to PLC\n");
         return BEP_STATUS_ERROR;
      }
      else
      {
         m_logger.Log(LOG_ERRORS, "Successful send status from the PLC: %d", status);
      }
   }
   m_logger.Log(LOG_DEV_DATA, "ToyopucProtocol::SendPlcData() exit\n");
   return BEP_STATUS_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
int ToyopucProtocol::ExtractData(uint8_t *rawData, int byteCount, uint32_t *plcData, int dataCount)
{
   register uint8_t   *pp;
   register long       nn = 0, cnt = 0;
   uint32_t            stx_ptr, etx_ptr;
   uint16_t            in_buf;

   pp = rawData;
   cnt = byteCount;
   in_buf = stx_ptr = etx_ptr = 0;

   // if valid data buffer then move string into variables
   if ((plcData != NULL) && (dataCount > 0))
   {
      //if(pp[4] != abDINT) // make sure we are receiving 16 bit integers back
      //{
      cnt = 4;
      // changed from 0 to start at actual PLC data and not include header info
      nn = 0;
      while ((cnt < byteCount) && (nn < dataCount)) // stop when all bytes are done
      {
         in_buf = (pp[cnt] | (pp[cnt + 1] << 8)); // Combine bytes into word
         plcData[nn++] = in_buf;                // stuff data into global memory
         cnt += 2;                               // go to next word
      }
      //}
   }
   return (nn);
}

//---------------------------------------------------------------------------------------------------------------------
void ToyopucProtocol::LogPlcString(uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen)
{
   uint32_t    ii;

   // If any data to print and correct verbosity
   if (m_logger.GetVerboseMask() & logMask)
   {
      std::string logBuff("");
      char        junk[5];

      if (prefix != NULL)     logBuff = std::string(prefix) + std::string(": ");

      for (ii = 0; ii < dataLen; ii++)
      {
         sprintf(junk,  "$%02hhX ", plcData[ii]);
         logBuff += std::string(junk);
      }

      if (logBuff.length() > 0)
      {
         logBuff += std::string("\n");
         m_logger.Log(logMask, logBuff.c_str());
      }
   }
}

//-------------------------------------------------------------------------------------------------
inline void ToyopucProtocol::SetPlcCommSetup(const XmlNode *setup)
{
   if (setup != NULL) m_plcCommSetup = setup->Copy();
   else m_plcCommSetup = NULL;
}

//-------------------------------------------------------------------------------------------------
inline const XmlNode* ToyopucProtocol::GetPlcCommSetup(void)
{
   return (m_plcCommSetup);
}

