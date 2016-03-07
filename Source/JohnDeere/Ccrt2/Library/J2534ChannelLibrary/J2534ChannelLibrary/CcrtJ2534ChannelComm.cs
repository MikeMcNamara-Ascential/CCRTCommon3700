using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace J2534ChannelLibrary
{
    public class CcrtJ2534ChannelComm
    {
        public CcrtJ2534ChannelComm(ref CcrtJ2534ChannelSettings channel)
        {
            m_channel = channel;
            m_connected = false;
        }
        public bool Connect()
        {//connect to channel
            ErrorCode status = m_j2534Interface.Connect(m_channel.m_deviceID, m_channel.m_protocolID, m_channel.m_connectFlag, m_channel.m_baudRate, ref m_channelID);

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
            m_connected = true;
            return true;
        }
        public bool AddFilter(CcrtJ2534Defs.MessageFilter filter)
        {//check to see if filter already exists
            foreach (CcrtJ2534Defs.MessageFilter f in m_channel.m_filterMessageIDs)
            {
                if (BitConverter.ToString(f.requestID.ToArray()) == BitConverter.ToString(filter.requestID.ToArray()))
                {
                    return true;
                }
            }
            m_channel.m_filterMessageIDs.Add(filter);
            return StartFilter(filter);
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
                    status = m_j2534Interface.StartMsgFilter(m_channelID, FilterType.FLOW_CONTROL_FILTER, ref maskMsg, ref patternMsg, ref flowControlMsg, ref filterID);
                    if (ErrorCode.STATUS_NOERROR != status)
                    {
                        m_j2534Interface.Disconnect(m_channelID);
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
            m_j2534Interface.Disconnect(m_channelID);
            return true;
        }

        /// <summary>
        /// Send message to ecu and get response.
        /// </summary>
        /// <param name="ecuMessage">Message structure containing tx message and valid responses</param>
        /// <returns>Status of operation</returns>
        public bool GetECUData(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData)
        {
            List<byte> transmitMessage = new List<byte>();
            transmitMessage.AddRange(ecuMessage.m_messageFilter.requestID);
            transmitMessage.AddRange(ecuMessage.m_txMessage);

            if (!SendMessage(transmitMessage))
            {
                return false;
            }
            if (!GetResponse(ecuMessage.m_responses, ref ecuData))
            {
                return false;
            }
            ecuData.RemoveRange(0, ecuMessage.m_messageFilter.requestID.Count());
            return true;
        }
        /// <summary>
        /// Send Message to the channel.
        /// </summary>
        /// <param name="txMessage">Message to transmit to ECU</param>
        /// <returns>Status of operation</returns>
        public bool SendMessage(List<byte> txMessage)
        {
            ErrorCode status = ErrorCode.STATUS_NOERROR;
            PassThruMsg txMsg = new PassThruMsg();
            int numMsgs = 1;
            int timeout = 50;
            txMsg.Data = txMessage.ToArray();
            //handle special protocol flags
            switch (m_channel.m_protocolID)
            {
                case ProtocolID.ISO15765:
                    txMsg.txFlags = TxFlag.ISO15765_FRAME_PAD;
                    break;
            }
            txMsg.protocolID = m_channel.m_protocolID;
            //send message
            status = m_j2534Interface.WriteMsgs(m_channelID, ref txMsg, ref numMsgs, timeout);
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
        public bool GetResponse(List<CcrtJ2534Defs.Response> responses, ref List<byte> value)
        {
            int numMsgs = 1;
            int timeOut = 200;
            ErrorCode status = ErrorCode.STATUS_NOERROR;
            List<PassThruMsg> rxMsgs = new List<PassThruMsg>();
            //need to set up a timer and check for our message also check for neg. response pending msg
            while (ErrorCode.STATUS_NOERROR == status)
            {
                status = m_j2534Interface.ReadMsgs(m_channelID, ref rxMsgs, ref numMsgs, timeOut);
            }
            if (ErrorCode.ERR_BUFFER_EMPTY == status || ErrorCode.ERR_TIMEOUT == status)
            {
                if (rxMsgs.Count > 1)
                {  //[txMsg.Data.Length -1]
                    // Select the last value
                    value = rxMsgs[rxMsgs.Count - 1].Data.ToList();

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

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
        CcrtJ2534ChannelSettings m_channel;
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
    }
}
