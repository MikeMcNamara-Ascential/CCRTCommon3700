using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace J2534ChannelLibrary
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
        public bool ProcessMessageCAN(string channelName, CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> data)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.ProcessMessageCAN(ecuMessage, ref data);
        }
        public bool GetECUData(string channelName, CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> data)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.GetECUData(ecuMessage,ref data);
        }
        public bool GetECUData(string channelName, CcrtJ2534Defs.ECUMessage ecuMessage, ref List<List<byte>> data, bool globalRequest)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.GetECUData(ecuMessage, ref data, globalRequest);
        }
        public bool AddMessageFilter(string channelName, CcrtJ2534Defs.MessageFilter filter)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.AddMessageFilter(filter);
        }
        public bool StartPeriodicMessage(string channelName, CcrtJ2534Defs.ECUMessage ecuMessage, ref int msgID)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.StartPeriodicMessage(ecuMessage, ref msgID);
        }
        public bool StopPeriodicMessage(string channelName, int msgID)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.StopPeriodicMessage(ref msgID);
        }
        public ICcrtJ2534ChannelComm GetCommChannelInterface(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm;
        }
        public bool IsVehicleConnected(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.IsVehicleConnected();
        }
        public bool SetSTMin(string channelName, int stMinValue)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.SetSTMIN(stMinValue);
        }
        public bool PerformFiveBaudInit(string channelName, byte address)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.PerformFiveBaudInit(address);
        }
        public bool PerformFastInit(string channelName, ref List<byte> wakeUpMessage, ref List<byte> ecuData)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.PerformFastInit(ref wakeUpMessage, ref ecuData);
        }
        public bool SetDeviceConfigurationParameter(string channelName, ConfigParameter param, int value)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.SetDeviceConfigurationParameter(param, value);
        }
        public bool GetDeviceConfigurationParameter(string channelName, ConfigParameter param, ref int value)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.GetDeviceConfigurationParameter(param,ref value);
        }
        public bool SetDeviceConfiguration(string channelName, ref SConfig[] config)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.SetDeviceConfiguration(ref config);
        }
        public bool GetDeviceConfiguration(string channelName, ref SConfig[] config)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            return channel.ChannelComm.GetDeviceConfiguration(ref config);
        }



        public void ClearRxBuffer(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            if (!channel.ChannelComm.Connected)
            {
                channel.ChannelComm.Connect();
            }
            channel.ChannelComm.ClearResponseBuffer();
        }
        public List<CcrtJ2534Defs.Response> GetRemovedResponsesBuffer(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            return channel.ChannelComm.GetRemovedResponsesBuffer();
        }
        public List<CcrtJ2534Defs.Response> GetResponseBuffer(string channelName)
        {
            CcrtJ2534Channel channel = GetChannel(channelName);
            return channel.ChannelComm.GetResponseBuffer();
        }

        private J2534Interface m_j2534Interface;
        private J2534Device m_device;
    }
}
