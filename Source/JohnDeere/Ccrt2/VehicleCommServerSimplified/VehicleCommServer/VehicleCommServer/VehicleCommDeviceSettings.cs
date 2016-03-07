using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VehicleCommServer
{
    public class VehicleCommDeviceSettings
    {
        public VehicleCommDeviceSettings()
        {
        }

        public VehicleCommDeviceSettings(string commChannelName, string deviceName, int channelIndex,
            string connectFlag, string baudRate, string protocolID)
        {
            m_commChannelName = commChannelName;
            m_deviceName = deviceName;
            m_channelIndex = channelIndex;
            m_connectFlag = connectFlag;
            m_baudRate = baudRate;
            m_protocolID = protocolID;
        }


        private string m_commChannelName;
        /// <summary>
        /// Get/Set the comm channel name of this device.
        /// </summary>
        public string CommChannelName
        {
            get { return m_commChannelName; }
            set { m_commChannelName = value; }
        }

        private string m_deviceName;
        /// <summary>
        /// Get/Set the comm device name of this device.
        /// </summary>
        public string DeviceName
        {
            get { return m_deviceName; }
            set { m_deviceName = value; }
        }

        private int m_channelIndex;
        /// <summary>
        /// Get/Set the comm device name of this device.
        /// </summary>
        public int ChannelIndex
        {
            get { return m_channelIndex; }
            set { m_channelIndex = value; }
        }

        private string m_connectFlag;
        /// <summary>
        /// Get/Set the comm device name of this device.
        /// </summary>
        public string ConnectFlag
        {
            get { return m_connectFlag; }
            set { m_connectFlag = value; }
        }

        private string m_baudRate;
        /// <summary>
        /// Get/Set the comm device name of this device.
        /// </summary>
        public string BaudRate
        {
            get { return m_baudRate; }
            set { m_baudRate = value; }
        }

        private string m_protocolID;
        /// <summary>
        /// Get/Set the comm device name of this device.
        /// </summary>
        public string ProtocolID
        {
            get { return m_protocolID; }
            set { m_protocolID = value; }
        }
    }
}
