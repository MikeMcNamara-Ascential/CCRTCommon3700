using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace J2534ChannelLibrary
{
    public class CcrtJ2534ChannelSettings
    {
        public CcrtJ2534ChannelSettings()
        {
            m_filterMessageIDs = new List<CcrtJ2534Defs.MessageFilter>();
        }
        public ProtocolID m_protocolID;
        public BaudRate m_baudRate;
        public ConnectFlag m_connectFlag;
        public int m_deviceID;
        public List<CcrtJ2534Defs.MessageFilter> m_filterMessageIDs;
    }
}
