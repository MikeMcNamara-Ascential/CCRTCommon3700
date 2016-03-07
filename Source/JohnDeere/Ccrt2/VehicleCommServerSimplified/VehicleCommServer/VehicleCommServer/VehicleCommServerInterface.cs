using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace VehicleCommServer
{
    public class VehicleCommServerInterface : IVehicleCommServerInterface
    {
        public VehicleCommServerInterface()
        {
            // Create a new interface to the CCRT Database
            //m_dbInterface = new CcrtDataBaseInterface(Properties.Resources.DatabaseServer);
            m_j2534Devices = new List<J2534Device>();
            m_j2534Devices = J2534Detect.GetDeviceList();
            m_j2534DeviceNames = new List<string>();
            m_vehicleCommDevices = new List<CcrtJ2534Device>();
            m_vehicleCommDeviceCollection = new VehicleCommDeviceCollection();   
            PopulateVehicleCommChannels();
        }
        public void OpenVehicleCommSettingsForm()
        {
            VehicleCommSettingsForm frm = new VehicleCommSettingsForm(this);
            frm.ShowDialog();
            //repopulate for any changes that may have been made
            PopulateVehicleCommChannels();
        }
        public void PopulateVehicleCommChannels()
        {
            m_j2534DeviceNames.Clear();
            m_vehicleCommDevices.Clear();
            foreach(J2534Device device in m_j2534Devices)
            {
                List<string> channelNames = new List<string>();
                channelNames = GetCommChannelNames(device.Name);
                CcrtJ2534Device selectedDevice = new CcrtJ2534Device(device);
                m_j2534DeviceNames.Add(device.Name);
                if (channelNames.Count > 0)
                {//channels for this device are configured

                    foreach (string name in channelNames)
                    {
                        selectedDevice.AddChannel(ReadCommChannelSettings(device.Name,name));
                    }

                }
                m_vehicleCommDevices.Add(selectedDevice);
            }
        }

        public List<string> GetCommChannelNames(string deviceName)
        {
            List<string> channelNames = new List<string>();
            if (m_vehicleCommDeviceCollection.CommDeviceSettings != null)
            {
                foreach (VehicleCommDeviceSettings settings in m_vehicleCommDeviceCollection.CommDeviceSettings)
                {
                    if (settings.DeviceName == deviceName)
                    {
                        channelNames.Add(settings.CommChannelName);
                    }
                }
            }
            return channelNames;
        }
        public List<string> GetCommChannelNames()
        {
            List<string> channelNames = new List<string>();
            foreach (VehicleCommDeviceSettings settings in m_vehicleCommDeviceCollection.CommDeviceSettings)
            {

                    channelNames.Add(settings.CommChannelName);

            }
            return channelNames;
        }

        public CcrtJ2534Channel ReadCommChannelSettings(string deviceName, string channelName)
        {//create a channel using settings from xml file
            CcrtJ2534Channel channel = new CcrtJ2534Channel();
            VehicleCommDeviceSettings settings = m_vehicleCommDeviceCollection.Find(channelName, deviceName);
            channel.SetBaudRate(settings.BaudRate);
            channel.SetConnectFlag(settings.ConnectFlag);
            channel.SetProtocolID(settings.ProtocolID);
            channel.ChannelName = channelName;
            return channel;
        }

        public CcrtJ2534Device GetCcrtJ2534Device(string deviceName)
        {
            CcrtJ2534Device selectedDevice = m_vehicleCommDevices.Find(delegate(CcrtJ2534Device dev)
            {
                return dev.DeviceName == deviceName;
            });
            return selectedDevice;
        }
        public bool ProcessMessageCAN(string deviceName, string commChannelName, CcrtJ2534Defs.ECUMessage message, ref List<byte> data)
        {//send message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.ProcessMessageCAN(message, ref data);
        }
        public bool GetECUData(string deviceName, string commChannelName, CcrtJ2534Defs.ECUMessage message,ref List<byte> data)
        {//send message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetECUData(message, ref data);
        }
        public bool GetECUData(string deviceName, string commChannelName, CcrtJ2534Defs.ECUMessage message, ref List<List<byte>> data, bool globalRequest)
        {//send message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetECUData(message, ref data, globalRequest);
        }
        public bool AddMessageFilter(string deviceName, string commChannelName, CcrtJ2534Defs.MessageFilter filter)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.AddFilter(filter);
        }
        public bool StartPeriodicMessage(string deviceName, string commChannelName, CcrtJ2534Defs.ECUMessage message, ref int msgID)
        {//start periodic message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.StartPeriodicMessage(message, ref msgID);
        }
        public bool StopPeriodicMessage(string deviceName, string commChannelName, int msgID)
        {//start periodic message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.StopPeriodicMessage(ref msgID);
        }
        public CcrtJ2534ChannelComm GetCommChannelInterface(string deviceName, string commChannelName)
        {//start periodic message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            
            return dev.GetCommChannelInterface(commChannelName);
        }
        public bool IsVehicleConnected(string deviceName, string commChannelName)
        {//start periodic message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.IsVehicleConnected();
        }

        public bool SetSTMin(string deviceName, string commChannelName, int stMinValue)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.SetSTMIN(stMinValue);
        }
        public bool PerformFiveBaudInit(string deviceName, string commChannelName, byte address)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.PerformFiveBaudInit(address);

        }
        public bool PerformFastInit(string deviceName, string commChannelName, ref List<byte> wakeUpMessage, ref List<byte> ecuData)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.PerformFastInit(ref wakeUpMessage, ref ecuData);
        }
        public bool SetDeviceConfigurationParameter(string deviceName, string commChannelName, ConfigParameter param, int value)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.SetDeviceConfigurationParameter(param, value);
        }
        public bool GetDeviceConfigurationParameter(string deviceName, string commChannelName, ConfigParameter param, ref int value)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetDeviceConfigurationParameter(param, ref value);
        }
        public bool SetDeviceConfiguration(string deviceName, string commChannelName, ref SConfig[] config)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.SetDeviceConfiguration(ref config);
        }
        public bool GetDeviceConfiguration(string deviceName, string commChannelName, ref SConfig[] config)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetDeviceConfiguration(ref config);
        }
        public void ClearResponseBuffer(string deviceName, string commChannelName)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            dev.GetChannel(commChannelName).ChannelComm.ClearResponseBuffer();
        }
        public List<CcrtJ2534Defs.Response> GetRemovedResponsesBuffer(string deviceName, string commChannelName)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetRemovedResponsesBuffer();
        }
        public List<CcrtJ2534Defs.Response> GetResponseBuffer(string deviceName, string commChannelName)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetChannel(commChannelName).ChannelComm.GetResponseBuffer();
        }
        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        //private CcrtDataBaseInterface m_dbInterface;
        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        private List<CcrtJ2534Device> m_vehicleCommDevices;
        /// <summary>
        /// List of connected j2534 devices.
        /// </summary>
        private List<J2534Device> m_j2534Devices;
        /// <summary>
        /// Get/Set the object to list of comm devices.
        /// </summary>
        public List<J2534Device> J2534Devices
        {
            get { return m_j2534Devices; }
            set { m_j2534Devices = value; }
        }
        /// <summary>
        /// List of connected j2534 devices.
        /// </summary>
        private List<string> m_j2534DeviceNames;
        /// <summary>
        /// Getlist of comm device names.
        /// </summary>
        public List<string> GetJ2534DeviceNames()
        {
            return m_j2534DeviceNames;
        }
        public List<J2534Device> GetJ2534Devices()
        {
            return m_j2534Devices;
        }
        public VehicleCommDeviceCollection m_vehicleCommDeviceCollection;

        public VehicleCommDeviceCollection GetVehicleCommDeviceCollection()
        {
            return m_vehicleCommDeviceCollection;
        }
    }
}
