using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace VehicleCommServer
{
    public class CcrtJ2534Device
    {
        /// <summary>
        /// Constructor.
        /// </summary>
        public CcrtJ2534Device(J2534Device device)
        {
            m_channels = new List<CcrtJ2534Channel>();
            m_deviceName = device.Name;
            m_device = device;
            m_j2534Interface = new J2534Interface();
            OpenJ2534Interface();
        }
        public void OpenJ2534Interface()
        {
            lock (m_j2534Interface)
            {
                m_j2534Interface.LoadLibrary(m_device);
                m_j2534Interface.Open(ref m_deviceID);
            }
        }
        public void AddChannel(CcrtJ2534Channel channel)
        {
            channel.DeviceID = m_deviceID;
            channel.SetJ2534Interface(m_j2534Interface);
            m_channels.Add(channel);
        }
        private string m_deviceName;
        /// <summary>
        /// Get/Set the Device name.
        /// </summary>
        public string DeviceName
        {
            get { return m_deviceName; }
            set { m_deviceName = value; }
        }
        private int m_deviceID;
        /// <summary>
        /// Get/Set the Device ID.
        /// </summary>
        public int DeviceID
        {
            get { return m_deviceID; }
            set { m_deviceID = value; }
        }
        private List<CcrtJ2534Channel> m_channels;
        /// <summary>
        /// Get the list of channels.
        /// </summary>
        public List<CcrtJ2534Channel> Channels
        {
            get { return m_channels; }
        }
        public CcrtJ2534Channel GetChannel(string channelName)
        {
            CcrtJ2534Channel channel = m_channels.Find(delegate(CcrtJ2534Channel ch)
            {
                return ch.ChannelName == channelName;
            });
            return channel;
        }
        public CcrtJ2534ChannelComm GetCommChannelInterface(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm;
        }



        private J2534Interface m_j2534Interface;
        private J2534Device m_device;
    }
}
