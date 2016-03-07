using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace J2534ChannelLibrary
{
    public class CcrtJ2534Defs
    {
        public struct MessageFilter
        {
            public byte[] requestID;
            public byte[] responseID;
        }
        public class Response
        {
            public Response()
            {
                m_rxMessage = new List<byte>();
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
                m_timeout = 200; //200 ms if not specified
            }
            public MessageFilter m_messageFilter;
            public List<byte> m_txMessage;
            public List<Response> m_responses;
            public int m_timeout;
        }
    }
}
