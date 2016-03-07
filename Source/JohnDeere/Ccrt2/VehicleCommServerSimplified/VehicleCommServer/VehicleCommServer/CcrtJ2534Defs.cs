using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VehicleCommServer
{
    public class CcrtJ2534Defs
    {
        //voltage in mv
        public static int VEHICLE_CONNECTED_VOLTAGE = 11000;
        public struct MessageFilter
        {
            public byte[] requestID;
            public byte[] responseID;
            //used for messages which illicit multiple response IDs
            public List<byte[]> responseIDs;
        }
        public class Response
        {
            public Response()
            {
                m_rxMessage = new List<byte>();
            }
            ~Response()
            {
                m_rxMessage.Clear();
                m_rxMessage = null;
            }

            public List<byte> m_rxMessage;
        }
        public class ECUMessage
        {
            public ECUMessage()
            {
                m_messageFilter = new MessageFilter();
                m_txMessage = new List<byte>();
                m_responses = new List<Response>();
                //default message settings
                m_txTimeout = 500;
                m_rxTimeout = 200;
                m_interval = 250;
                m_retries = 3;
                m_msgRetryDelay = 100;
                m_noResponseRetries = 3;
                m_responsePendingRetries = 3;
                m_responsePendingDelay = 100;
                m_responsePendingID = 0x78;
                m_responseExpected = true;
            }
            ~ECUMessage()
            {
                m_txMessage.Clear();
                m_responses.Clear();
                m_txMessage = null;
                m_responses = null;
                if (m_messageFilter.responseIDs != null)
                {
                    m_messageFilter.responseIDs.Clear();
                    m_messageFilter.responseIDs = null;
                }
            }
            public MessageFilter m_messageFilter;
            public List<byte> m_txMessage;
            public List<Response> m_responses;
            public int m_txTimeout;
            public int m_rxTimeout;
            public int m_interval;
            public int m_retries;
            public int m_msgRetryDelay;
            public int m_noResponseRetries;
            public int m_responsePendingRetries;
            public int m_responsePendingDelay;
            public byte m_responsePendingID;
            public bool m_responseExpected;

        }
        public class ProtocolSpecificDefs
        {
            /// <summary>
            /// Response index of positive response identifier.
            /// </summary>
            public int m_posRspIndex;
            /// <summary>
            /// Offset from message id indicating positive message received.
            /// </summary>
            public byte m_posRspOffset;
            /// <summary>
            /// Negative response message ID.
            /// </summary>
            public byte m_negRspID;
            public void SetProtocolSpecificDefs(J2534DotNet.ProtocolID protocol)
            {
                switch (protocol)
                {
                    case J2534DotNet.ProtocolID.ISO15765:
                        m_negRspID = 0x7f;
                        m_posRspIndex = 4;
                        m_posRspOffset = 0x40;
                        break;
                    case J2534DotNet.ProtocolID.ISO14230:
                        m_negRspID = 0x7f;
                        m_posRspIndex = 3;
                        m_posRspOffset = 0x40;
                        break;
                    default:
                        break;
                }
            }
        }

    }
}
