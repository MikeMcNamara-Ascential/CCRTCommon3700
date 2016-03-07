using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace VehicleCommServer
{
    public class CcrtJ2534ChannelComm
    {
        public CcrtJ2534ChannelComm(ref CcrtJ2534Channel.CcrtJ2534ChannelSettings channel)
        {
            m_channel = channel;
            m_connected = false;
            m_protocolSpecDefs = new CcrtJ2534Defs.ProtocolSpecificDefs();
            m_responseBuffer = new List<CcrtJ2534Defs.Response>();
            m_removedResponseBuffer = new List<CcrtJ2534Defs.Response>();
        }
        public bool Connect()
        {//connect to channel
            ErrorCode status = ErrorCode.ERR_FAILED;
            lock (m_j2534Interface)
            {
                status = m_j2534Interface.Connect(m_channel.m_deviceID, m_channel.m_protocolID, m_channel.m_connectFlag, m_channel.m_baudRate, ref m_channelID);
            }
            if (status != ErrorCode.STATUS_NOERROR)
            {
                return false;
            }
            foreach (CcrtJ2534Defs.MessageFilter filter in m_channel.m_filterMessageIDs)
            {
                if (!StartFilter(filter))
                {
                    return false;
                }
            }
            m_protocolSpecDefs.SetProtocolSpecificDefs(m_channel.m_protocolID);
            m_connected = true;
            return true;
        }
        public bool AddFilter(CcrtJ2534Defs.MessageFilter filter)
        {//check to see if filter already exists
            if (!m_connected)
            {
                Connect();
            }
            if (filter.responseIDs == null)
            {//only using single filter borrrring
                foreach (CcrtJ2534Defs.MessageFilter f in m_channel.m_filterMessageIDs)
                {//only response id matters for 15765
                    if (BitConverter.ToString(f.responseID.ToArray()) == BitConverter.ToString(filter.responseID.ToArray()))
                    {
                        return true;
                    }
                }
                m_channel.m_filterMessageIDs.Add(filter);
                return StartFilter(filter);
            }
            else
            {
                foreach (byte[] id in filter.responseIDs)
                {
                    bool filterPresent = false;
                    foreach (CcrtJ2534Defs.MessageFilter f in m_channel.m_filterMessageIDs)
                    {//only response id matters for 15765
                        if (//BitConverter.ToString(f.requestID.ToArray()) == BitConverter.ToString(filter.requestID.ToArray()) &&
                            BitConverter.ToString(f.responseID.ToArray()) == BitConverter.ToString(id))
                        {//filter already active
                            filterPresent = true;
                            break;
                        }

                    }
                    if (!filterPresent)
                    {
                        CcrtJ2534Defs.MessageFilter newFilter = new CcrtJ2534Defs.MessageFilter();
                        newFilter.requestID = filter.requestID;
                        newFilter.responseID = id;
                        m_channel.m_filterMessageIDs.Add(newFilter);
                        StartFilter(newFilter);                        
                    }                    
                }
                return true;
            }
        }
        public bool StartFilter(CcrtJ2534Defs.MessageFilter filter)
        {   //setup filters
            ErrorCode status;
            int filterID = 0;
            PassThruMsg maskMsg = new PassThruMsg();
            PassThruMsg patternMsg = new PassThruMsg();
            PassThruMsg flowControlMsg = new PassThruMsg();
            switch (m_channel.m_protocolID)
            {
                case ProtocolID.ISO15765:

                    maskMsg.protocolID = ProtocolID.ISO15765;
                    maskMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;
                    maskMsg.Data = new byte[] { 0xff, 0xff, 0xff, 0xff };

                    patternMsg.protocolID = ProtocolID.ISO15765;
                    patternMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;

                    flowControlMsg.protocolID = ProtocolID.ISO15765;
                    flowControlMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;
                    patternMsg.Data = filter.responseID;
                    flowControlMsg.Data = filter.requestID;
                    lock (m_j2534Interface)
                    {
                        status = m_j2534Interface.StartMsgFilter(m_channelID, FilterType.FLOW_CONTROL_FILTER, ref maskMsg, ref patternMsg, ref flowControlMsg, ref filterID);
                    }
                    if (ErrorCode.STATUS_NOERROR != status)
                    {
                        lock (m_j2534Interface)
                        {
                            m_j2534Interface.Disconnect(m_channelID);
                        }
                        return false;
                    }
                    break;
                case ProtocolID.ISO14230:

                    maskMsg.protocolID = ProtocolID.ISO14230;
                    maskMsg.txFlags = TxFlag.NONE;
                    maskMsg.Data = new byte[] { 0x80, 0xff, 0xff, 0x00 };

                    patternMsg.protocolID = ProtocolID.ISO14230;
                    patternMsg.txFlags = TxFlag.NONE;

                    patternMsg.Data = new byte[] { 0x80, filter.responseID[0],filter.requestID[0], 0x00};
                    lock (m_j2534Interface)
                    {
                        status = m_j2534Interface.StartMsgFilter(m_channelID, FilterType.PASS_FILTER, ref maskMsg, ref patternMsg, ref filterID);
                    }
                    if (ErrorCode.STATUS_NOERROR != status)
                    {
                        lock (m_j2534Interface)
                        {
                            m_j2534Interface.Disconnect(m_channelID);
                        }
                        return false;
                    }
                    break;
                default:
                    return false;
            }
            return true;
        }
        /// <summary>
        /// Disconnect from channel.
        /// </summary>
        /// <param name=""></param>
        /// <returns>Status of operation</returns>
        public bool Disconnect()
        {
            lock (m_j2534Interface)
            {
                m_j2534Interface.Disconnect(m_channelID);
            }
            ClearResponseBuffer();
            return true;
        }
        public void BuildTransmitMessage(ref List<byte> fullMessage,byte[] requestID, List<byte> txMessage)
        {
            fullMessage.AddRange(requestID);
            fullMessage.AddRange(txMessage);
        }
        private void LockCommPort()
        {
        }
        /// <summary>
        /// Send message to ecu and get response.
        /// </summary>
        /// <param name="ecuMessage">Message structure containing tx message and valid responses</param>
        /// <returns>Status of operation</returns>
        public bool GetECUData(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData)
        {
            if (!m_connected)
            {
                Connect();
            }
            bool msgReceived = false;
            int retryCount = ecuMessage.m_retries;

            while (!msgReceived && retryCount-- > 0)
            {
                if (!SendMessage(ecuMessage))
                {
                    return false;
                }
                switch (m_channel.m_protocolID)
                {
                    case ProtocolID.ISO15765:
                                                //process message
                        if (ecuMessage.m_responseExpected)
                        {
                            //process message
                            msgReceived = ProcessMessageCAN(ecuMessage, ref ecuData);
                            if (msgReceived)
                            {
                                ecuData.RemoveRange(0, ecuMessage.m_messageFilter.requestID.Count());
                            }
                        }
                        else
                        {
                            return true;
                        }
                        break;
                    case ProtocolID.ISO14230:
                        //process message
                        if (ecuMessage.m_responseExpected)
                        {
                            //process message
                            msgReceived = ProcessMessageKWP2000(ecuMessage, ref ecuData);
                            if (msgReceived)
                            {//remove protocol byte (0x8?) tester id, and ecu id from message
                                ecuData.RemoveRange(0, 3);
                            }
                        }
                        else
                        {
                            return true;
                        }
                        break;
                    default:
                        msgReceived = true;
                        break;
                }
                if (!msgReceived)
                {
                    System.Threading.Thread.Sleep(ecuMessage.m_msgRetryDelay);
                }
            }
            //check to see if data matches our id and filter
            return msgReceived;
        }
        /// <summary>
        /// Send message to ecu and get response.
        /// </summary>
        /// <param name="ecuMessage">Message structure containing tx message and valid responses</param>
        /// <param name="ecuData">Storage for response from ECU</param>
        /// <param name="useExtendedAddressing">Flag indicating weather to use 15765 extended address</param>
        /// <returns>Status of operation</returns>
        public bool GetECUData(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<List<byte>> ecuData, bool globalRequest)
        {
            if (!m_connected)
            {
                Connect();
            }
            bool msgReceived = false;
            int retryCount = ecuMessage.m_retries;

            while (!msgReceived && retryCount-- > 0)
            {
                if (!SendMessage(ecuMessage))
                {
                    return false;
                }
                switch (m_channel.m_protocolID)
                {
                    case ProtocolID.ISO15765:
                        //process message
                        if (ecuMessage.m_responseExpected)
                        {
                            msgReceived = ProcessMessagesCAN(ecuMessage, ref ecuData);
                        }
                        else
                        {
                            return true;
                        }
                        //skip removing response ID - may want to use later since multiple ecu's will respond
                        //note: intention is to keep messages in order of filters added as double check
                        break;
                    default:
                        msgReceived = true;
                        break;
                }
                if (!msgReceived)
                {
                    System.Threading.Thread.Sleep(ecuMessage.m_msgRetryDelay);
                }
            }
            //check to see if data matches our id and filter
            return msgReceived;
        }
        public bool ProcessMessageCAN(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData)
        {//determine if our message has been received starting from end of response list]
            if (!m_connected)
            {
                Connect();
            }
            bool responsePending = true;
            int retryCount = ecuMessage.m_responsePendingRetries;
            int noReplyRetryCount = ecuMessage.m_noResponseRetries;

            List<CcrtJ2534Defs.Response> responses = null;
            while ((retryCount-- > 0) && responsePending)
            {//retrieve response from device
                if (!GetResponse(ecuMessage, ref responses))
                {
                    if (noReplyRetryCount != 0)
                    {//error getting message allow multiple attempts
                        responses = null;
                        responsePending = false;
                    }
                    else
                    {
                        responses = null;
                        return false;
                    }
                }
                if (responses != null)
                {
                    for (int i = (responses.Count - 1); i > -1; i--)
                    {//check if message filter matches
                        byte[] messageFilter = responses[i].m_rxMessage.GetRange(0, 4).ToArray();
                        if (BitConverter.ToString(messageFilter) == BitConverter.ToString(ecuMessage.m_messageFilter.responseID))
                        {//ID matches - check identifier
                            if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                ecuMessage.m_txMessage[0] + m_protocolSpecDefs.m_posRspOffset)
                            {//positive response message received
                                //set ecu Data
                                ecuData = new List<byte>(responses[i].m_rxMessage);
                                //remove message from buffer
                                RemoveResponseFromBuffer(responses[i]);
                                responses = null;
                                return true;
                            }
                            //check to see if the message is a negative response for our message ID
                            else if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                m_protocolSpecDefs.m_negRspID))
                            {//neg response received - check for our request id
                                if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 1] ==
                                ecuMessage.m_txMessage[0]))
                                {
                                    if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 2] ==
                                        ecuMessage.m_responsePendingID)
                                    {
                                        responsePending = true;
                                        //remove message from buffer
                                        RemoveResponseFromBuffer(responses[i]);
                                        //exit for loop
                                        break;
                                    }
                                    else
                                    {
                                        responsePending = false;
                                        //remove message from buffer
                                        RemoveResponseFromBuffer(responses[i]);
                                        ecuData = new List<byte>(responses[i].m_rxMessage);
                                        responses = null;
                                        return true;
                                    }
                                }
                                else responsePending = false;
                            }
                            else
                            {//not our message
                                responsePending = false;
                            }
                        }
                    }
                }
                if (responsePending)
                {//delay before next check
                    //reset no response count since we received a reply
                    noReplyRetryCount = ecuMessage.m_noResponseRetries;
                    System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                }
                else
                {//no message filter match found 
                    if (noReplyRetryCount != 0)
                    {//allow multiple attempts
                        responsePending = true;
                        noReplyRetryCount--;
                        System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                    }
                }
            }

            //message did not match any filter
            responses = null;
            return false;
        }

        public bool ProcessMessageKWP2000(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData)
        {//determine if our message has been received starting from end of response list
            bool responsePending = true;
            int retryCount = ecuMessage.m_responsePendingRetries;
            int noReplyRetryCount = ecuMessage.m_noResponseRetries;

            List<CcrtJ2534Defs.Response> responses = null;
            while ((retryCount-- > 0) && responsePending)
            {//retrieve response from device
                if (!GetResponse(ecuMessage, ref responses))
                {
                    if (noReplyRetryCount != 0)
                    {//error getting message allow multiple attempts
                        responses = null;
                        responsePending = false;
                    }
                    else
                    {
                        responses = null;
                        return false;
                    }
                }
                if (responses != null)
                {
                    for (int i = (responses.Count - 1); i > -1; i--)
                    {//check if message filter matches
                        byte[] messageFilter = responses[i].m_rxMessage.GetRange(0, 4).ToArray();
                        if (messageFilter[1] == ecuMessage.m_txMessage[2] && messageFilter[2] == ecuMessage.m_txMessage[1])
                        {//ID matches - check identifier
                            if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                ecuMessage.m_txMessage[3] + m_protocolSpecDefs.m_posRspOffset)
                            {//positive response message received
                                //set ecu Data
                                ecuData = new List<byte>(responses[i].m_rxMessage);
                                //remove message from buffer
                                RemoveResponseFromBuffer(responses[i]);
                                responses = null;
                                return true;
                            }
                            //check to see if the message is a negative response for our message ID
                            else if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                m_protocolSpecDefs.m_negRspID))
                            {//neg response received - check for our request id
                                if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 1] ==
                                ecuMessage.m_txMessage[3]))
                                {
                                    if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 2] ==
                                        ecuMessage.m_responsePendingID)
                                    {
                                        responsePending = true;
                                        //remove message from buffer
                                        RemoveResponseFromBuffer(responses[i]);
                                        //exit for loop
                                        break;
                                    }
                                    else
                                    {
                                        responsePending = false;
                                        //remove message from buffer
                                        RemoveResponseFromBuffer(responses[i]);
                                        ecuData = new List<byte>(responses[i].m_rxMessage);
                                        responses = null;
                                        return true;
                                    }
                                }
                                else responsePending = false;
                            }
                            else
                            {//not our message
                                responsePending = false;
                            }
                        }
                    }
                }
                if (responsePending)
                {//delay before next check
                    //reset no response count since we received a reply
                    noReplyRetryCount = ecuMessage.m_noResponseRetries;
                    System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                }
                else
                {//no message filter match found 
                    if (noReplyRetryCount != 0)
                    {//allow multiple attempts
                        responsePending = true;
                        noReplyRetryCount--;
                        System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                    }
                }
            }

            //message did not match any filter
            responses = null;
            return false;
        }
        //used to process messages which illicit multiple responses
        public bool ProcessMessagesCAN(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<List<byte>> ecuData)
        {//determine if our message has been received starting from end of response list
            bool responsePending = true;
            //keep track of responses in order of message filters
            foreach(byte[] byteArray in ecuMessage.m_messageFilter.responseIDs)
            {
                ecuData.Add(new List<byte>());
            }
            int retryCount = ecuMessage.m_responsePendingRetries;
            int noReplyRetryCount = ecuMessage.m_noResponseRetries;
            List<CcrtJ2534Defs.Response> responses = null;
            while ((retryCount-- > 0) && responsePending)
            {//retrieve response from device
                if (!GetResponse(ecuMessage, ref responses))
                {
                    if (noReplyRetryCount != 0)
                    {//error getting message allow multiple attempts
                        responses = null;
                        responsePending = false;
                    }
                    else
                    {
                        return false;
                    }
                }
                if (responses != null)
                {
                    for (int i = (responses.Count - 1); i > -1; i--)
                    {//check if message filter matches
                        byte[] messageFilter = responses[i].m_rxMessage.GetRange(0, 4).ToArray();
                        for (int filterIndex = 0; filterIndex < ecuMessage.m_messageFilter.responseIDs.Count; filterIndex++)
                        {
                            if (BitConverter.ToString(messageFilter) == BitConverter.ToString(ecuMessage.m_messageFilter.responseIDs[filterIndex]))
                            {//ID matches - check identifier
                                try
                                {
                                    if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                        ecuMessage.m_txMessage[1] + m_protocolSpecDefs.m_posRspOffset)
                                    {//positive response message received
                                        //set ecu Data
                                        ecuData[filterIndex] = new List<byte>(responses[i].m_rxMessage);
                                        //remove message from buffer
                                        RemoveResponseFromBuffer(responses[i]);
                                        return true;
                                    }
                                    //check to see if the message is a negative response for our message ID
                                    else if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex] ==
                                        m_protocolSpecDefs.m_negRspID))
                                    {//neg response received - check for our request id
                                        if ((responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 1] ==
                                        ecuMessage.m_txMessage[0]))
                                        {
                                            if (responses[i].m_rxMessage[m_protocolSpecDefs.m_posRspIndex + 2] ==
                                                ecuMessage.m_responsePendingID)
                                            {
                                                //remove message from buffer
                                                RemoveResponseFromBuffer(responses[i]);
                                                //exit for loop
                                                break;
                                            }
                                            else
                                            {
                                                //remove message from buffer
                                                RemoveResponseFromBuffer(responses[i]);
                                                ecuData[filterIndex] = new List<byte>(responses[i].m_rxMessage);
                                                return true;
                                            }
                                        }
                                    }
                                }
                                catch (ArgumentOutOfRangeException)
                                {// data position exceeded
                                    return false;
                                }
                            }
                        }
                        //add response pending logic
                        foreach (List<byte> data in ecuData)
                        {
                            if (data.Count == 0)
                            {
                                //still waiting for data
                                responsePending = true;
                                break;
                            }
                        }
                    }
                }
                if (responsePending)
                {//delay before next check
                    //reset no response count since we received a reply
                    noReplyRetryCount = ecuMessage.m_noResponseRetries;
                    System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                }
                else
                {//no message filter match found 
                    if (noReplyRetryCount != 0)
                    {//allow multiple attempts
                        responsePending = true;
                        noReplyRetryCount--;
                        System.Threading.Thread.Sleep(ecuMessage.m_responsePendingDelay);
                    }
                }
            }

            //message did not match any filter
            return false;
        }

        /// <summary>
        /// Send Message to the channel.
        /// </summary>
        /// <param name="txMessage">Message to transmit to ECU</param>
        /// <returns>Status of operation</returns>
        public bool SendMessage(CcrtJ2534Defs.ECUMessage ecuMessage)
        {
            List<byte> txMessage = new List<byte>();
            ErrorCode status = ErrorCode.STATUS_NOERROR;
            PassThruMsg txMsg = new PassThruMsg();


            //handle special protocol flags
            switch (m_channel.m_protocolID)
            {
                case ProtocolID.ISO15765:
                    txMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;
                    BuildTransmitMessage(ref txMessage, ecuMessage.m_messageFilter.requestID,
                    ecuMessage.m_txMessage);
                    break;
                case ProtocolID.ISO14230:
                    txMsg.txFlags = TxFlag.NONE;
                    txMessage.AddRange(ecuMessage.m_txMessage);
                    break;
            }
            int numMsgs = 1;
            int timeout = ecuMessage.m_txTimeout;
            txMsg.Data = txMessage.ToArray();
            txMsg.protocolID = m_channel.m_protocolID;
            //send message
            lock (m_j2534Interface)
            {
                status = m_j2534Interface.WriteMsgs(m_channelID, ref txMsg, ref numMsgs, timeout);
            }
            if (status != ErrorCode.STATUS_NOERROR)
            {
                return false;
            }
            return true;
        }
        /// <summary>
        /// Check for valid ECU response.
        /// </summary>
        /// <param name=""></param>
        /// <returns>Get response from channel</returns>
        public bool GetResponse(CcrtJ2534Defs.ECUMessage ecuMessage,ref List<CcrtJ2534Defs.Response> responses)
        {
            int numMsgs = 1;
            int timeOut = ecuMessage.m_rxTimeout;
            ErrorCode status = ErrorCode.STATUS_NOERROR;
            List<PassThruMsg> rxMsgs = new List<PassThruMsg>();
            List<PassThruMsg> fullMsgList = new List<PassThruMsg>();
            //need to set up a timer and check for our message also check for neg. response pending msg
            lock (m_j2534Interface)
            {
                
                while (ErrorCode.STATUS_NOERROR == status)
                {
                    status = m_j2534Interface.ReadMsgs(m_channelID, ref rxMsgs, ref numMsgs, timeOut);
                    fullMsgList.AddRange(rxMsgs);
                    rxMsgs.Clear();
                }
            }
            if (ErrorCode.ERR_BUFFER_EMPTY == status || ErrorCode.ERR_TIMEOUT == status)
            {
                if (fullMsgList.Count > 0)
                {
                    lock (m_responseBuffer)
                    {
                        foreach (PassThruMsg msg in fullMsgList)
                        {
                            //message Ids are sent and received for some reason
                            switch (m_channel.m_protocolID)
                            {
                                case ProtocolID.ISO15765:
                                    if (msg.Data.Count() > ecuMessage.m_messageFilter.requestID.Count())
                                    {
                                        CcrtJ2534Defs.Response response = new CcrtJ2534Defs.Response();
                                        response.m_rxMessage = msg.Data.ToList();
                                        m_responseBuffer.Add(response);
                                    }
                                    break;
                                case ProtocolID.ISO14230:
                                    if (msg.Data.Count() > 3)
                                    {
                                        CcrtJ2534Defs.Response response = new CcrtJ2534Defs.Response();
                                        response.m_rxMessage = msg.Data.ToList();
                                        m_responseBuffer.Add(response);
                                    }
                                    break;
                            }

                        }
                        responses = new List<CcrtJ2534Defs.Response>(m_responseBuffer);
                    }
                    return true;
                }
                else
                {//no data received may have been in previous thread
                    lock (m_responseBuffer)
                    {
                        responses = new List<CcrtJ2534Defs.Response>(m_responseBuffer);
                    }
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
        //Warning - only use periodic message if module does not respond
        public bool StartPeriodicMessage(CcrtJ2534Defs.ECUMessage ecuMessage, ref int msgID)
        {
            if (!m_connected)
            {
                Connect();
            }
            if (!ecuMessage.m_responseExpected)
            {
                PassThruMsg periodicMsg = new PassThruMsg();
                List<byte> transmitMessage = new List<byte>();
                ErrorCode status = ErrorCode.STATUS_NOERROR;
                BuildTransmitMessage(ref transmitMessage, ecuMessage.m_messageFilter.requestID,
                    ecuMessage.m_txMessage);
                periodicMsg.Data = transmitMessage.ToArray();
                //handle special protocol flags
                switch (m_channel.m_protocolID)
                {
                    case ProtocolID.ISO15765:
                        periodicMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;
                        break;
                }
                periodicMsg.protocolID = m_channel.m_protocolID;
                lock (m_j2534Interface)
                {
                    status = m_j2534Interface.StartPeriodicMsg(m_channelID, ref periodicMsg, ref msgID,
                        ecuMessage.m_interval);
                }
                if (status != ErrorCode.STATUS_NOERROR)
                {
                    return false;
                }
                return true;
            }
            //periodic messages only allowed for non responding messages
            return false;
        }
        public bool StopPeriodicMessage(ref int msgID)
        {
            ErrorCode status = ErrorCode.STATUS_NOERROR;
            if (!m_connected)
            {
                Connect();
            }
            lock (m_j2534Interface)
            {
                status = m_j2534Interface.StopPeriodicMsg(m_channelID, msgID);
            }
            if (status != ErrorCode.STATUS_NOERROR)
            {
                return false;
            }
            return true;
        }
        protected int m_channelID;
        /// <summary>
        /// Get/Set the channel ID.
        /// </summary>
        public int ChannelID
        {
            get { return m_channelID; }
            set { m_channelID = value; }
        }
        CcrtJ2534Channel.CcrtJ2534ChannelSettings m_channel;
        private IJ2534Interface m_j2534Interface;
        /// <summary>
        /// Get/Set the channel ID.
        /// </summary>
        public IJ2534Interface J2534Interface
        {
            get { return m_j2534Interface; }
            set { m_j2534Interface = value; }
        }
        private bool m_connected;
        /// <summary>
        /// Get/Set the connected Flag.
        /// </summary>
        public bool Connected
        {
            get { return m_connected; }
            set { m_connected = value; }
        }
        private CcrtJ2534Defs.ProtocolSpecificDefs m_protocolSpecDefs;
        private volatile List<CcrtJ2534Defs.Response> m_responseBuffer;
        private volatile List<CcrtJ2534Defs.Response> m_removedResponseBuffer;
        private void RemoveResponseFromBuffer(CcrtJ2534Defs.Response response)
        {
            lock (m_responseBuffer)
            {
                CcrtJ2534Defs.Response listResponse = m_responseBuffer.Find(delegate(CcrtJ2534Defs.Response res)
                {
                    return BitConverter.ToString(res.m_rxMessage.ToArray()) == BitConverter.ToString(response.m_rxMessage.ToArray());
                });
                m_responseBuffer.Remove(listResponse);
                //save last 50 removed messages for debugging purposes
                CcrtJ2534Defs.Response saveResponse = new CcrtJ2534Defs.Response();
                saveResponse.m_rxMessage = listResponse.m_rxMessage;                
                if (m_removedResponseBuffer.Count > 2)
                {
                    m_removedResponseBuffer.RemoveAt(0);
                }
                m_removedResponseBuffer.Add(saveResponse);
            }
        }
        public void ClearResponseBuffer()
        {
            if (!m_connected)
            {
                Connect();
            }
            lock (m_responseBuffer)
            {
                m_responseBuffer.Clear();
                lock (m_j2534Interface)
                {
                    m_j2534Interface.ClearRxBuffer(m_channelID);
                    m_j2534Interface.ClearTxBuffer(m_channelID);
                }
            }
        }
        public List<CcrtJ2534Defs.Response> GetRemovedResponsesBuffer()
        {//lock on resp. buff this is the only place removed resp. is modified
            lock (m_responseBuffer)
            {
                return m_removedResponseBuffer;
            }
        }
        public List<CcrtJ2534Defs.Response> GetResponseBuffer()
        {//lock on resp. buff this is the only place removed resp. is modified
            lock (m_responseBuffer)
            {
                return m_responseBuffer;
            }
        }
        public bool IsVehicleConnected()
        {
            int voltage=0;
            if (!m_connected)
            {
                Connect();
            }
            lock (m_j2534Interface)
            {
                m_j2534Interface.ReadBatteryVoltage(m_channel.m_deviceID, ref voltage);
            }
            return voltage > (CcrtJ2534Defs.VEHICLE_CONNECTED_VOLTAGE);
        }
        public bool SetSTMIN(int stMinValue)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            lock (m_j2534Interface)
            {
                status = ErrorCode.STATUS_NOERROR == m_j2534Interface.SetConfigParameter(m_channelID,ConfigParameter.ISO15765_STMIN,stMinValue);
            }
            return (status);
        }
        public bool PerformFiveBaudInit(byte address)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            //5 baud Init
            byte keyword1 = new byte();
            byte keyword2 = new byte();
            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.FiveBaudInit(m_channelID, ref address, ref keyword1, ref keyword2);
            return status;
        }
        public bool PerformFastInit(ref List<byte> wakeUpMessage, ref List<byte> ecuData)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            //Fast Init
            PassThruMsg wakeupMsg = new PassThruMsg();
            wakeupMsg.Data = wakeUpMessage.ToArray();
            PassThruMsg wakeupRsp = new PassThruMsg();
            wakeupRsp.Data = new byte[] { };

            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.FastInit(m_channelID, ref wakeupMsg, ref wakeupRsp);
            ecuData = wakeupRsp.Data.ToList();
            return status;
        }
        public bool SetDeviceConfigurationParameter(ConfigParameter param, int value)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.SetConfigParameter(m_channelID, param, value);
            return status;
        }
        public bool GetDeviceConfigurationParameter(ConfigParameter param, ref int value)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.GetConfigParameter(m_channelID, param, ref value);
            return status;
        }
        public bool SetDeviceConfiguration(ref SConfig[] config)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.SetConfig(m_channelID, ref config);
            return status;
        }
        public bool GetDeviceConfiguration(ref SConfig[] config)
        {
            bool status = false;
            if (!m_connected)
            {
                Connect();
            }
            status = ErrorCode.STATUS_NOERROR == m_j2534Interface.GetConfig(m_channelID, ref config);
            return status;
        }
    }
}
