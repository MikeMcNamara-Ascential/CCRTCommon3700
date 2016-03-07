using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CcrtDataBaseLibrary;
using J2534ChannelLibrary;
using J2534DotNet;

namespace VehicleCommServer
{
    public class VehicleCommServerInterface
    {
        public VehicleCommServerInterface()
        {
            // Create a new interface to the CCRT Database
            DbInterface = new CcrtDataBaseInterface(VehicleCommServer.Properties.Resources.DatabaseServer);
            m_j2534Devices = new List<J2534Device>();
            m_j2534Devices = J2534Detect.GetDeviceList();
            m_j2534DeviceNames = new List<string>();
            m_vehicleCommDevices = new List<CcrtJ2534Device>();
            PopulateVehicleCommChannels();
        }
        public void PopulateVehicleCommChannels()
        {
            foreach(J2534Device device in m_j2534Devices)
            {
                List<string> channelNames = new List<string>();
                channelNames = DbInterface.GetCommChannelNames(device.Name);
                if (channelNames.Count > 0)
                {//channels for this device are configured
                    CcrtJ2534Device selectedDevice = new CcrtJ2534Device(device);
                    m_j2534DeviceNames.Add(device.Name);
                    foreach (string name in channelNames)
                    {
                        selectedDevice.AddChannel(DbInterface.ReadCommChannelSettings(device.Name,name));
                    }
                    m_vehicleCommDevices.Add(selectedDevice);
                }
            }
        }
        public CcrtJ2534Device GetCcrtJ2534Device(string deviceName)
        {
            CcrtJ2534Device selectedDevice = m_vehicleCommDevices.Find(delegate(CcrtJ2534Device dev)
            {
                return dev.DeviceName == deviceName;
            });
            return selectedDevice;
        }
        public bool SendMessage(string deviceName, string commChannelName, J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message)
        {//send message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.SendMessage(commChannelName, message);
        }
        public bool GetECUData(string deviceName, string commChannelName, J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message,ref List<byte> data)
        {//send message to appropriate device
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.GetECUData(commChannelName, message,ref data);
        }
        public bool AddMessageFilter(string deviceName, string commChannelName, CcrtJ2534Defs.MessageFilter filter)
        {
            CcrtJ2534Device dev = GetCcrtJ2534Device(deviceName);
            return dev.AddMessageFilter(commChannelName, filter);
        }

        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        private CcrtDataBaseInterface m_dbInterface;
        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        private List<CcrtJ2534Device> m_vehicleCommDevices;
        /// <summary>
        /// Get/Set the object to interface with the CCRT Database.
        /// </summary>
        public CcrtDataBaseInterface DbInterface
        {
            get { return m_dbInterface; }
            set { m_dbInterface = value; }
        }
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
        /// Get/Set the object to list of comm devices.
        /// </summary>
        public List<string> J2534DeviceNames
        {
            get { return m_j2534DeviceNames; }
            set { m_j2534DeviceNames = value; }
        }


    }
}
