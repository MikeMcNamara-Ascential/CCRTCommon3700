using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SerialPortLibrary;
using System.Xml.Serialization;
using System.IO;

namespace InputServer
{
    class SerialDeviceCollection
    {
        /// <summary>
        /// Create a vehicle comm device collection.
        /// </summary>
        /// <param name=""></param>
        public SerialDeviceCollection()
        {
            Load();
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add a new vehicle comm device.
        /// </summary>
        /// <param name=""></param>
        /// <param name=""></param>
        public void Add(String deviceName, String portName, String baudRate, String dataBits, String parity,
            String stopBits, String readTimeout, String writeTimeout, String monitorData)
        {   // Remove the item so it can be re-added as a new item
            RemoveDeviceSettings(deviceName);
            SerialDevice sd = new SerialDevice(deviceName, portName, baudRate, dataBits, parity, stopBits, readTimeout, writeTimeout, monitorData);
            m_serialDevices.Add(sd);
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Find the vehicle comm device settings for the specified port name.
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        public SerialDevice Find(string deviceName)
        {   // Search the list of model codes
            try
            {
                foreach (SerialDevice device in m_serialDevices)
                {   // Return the option if this is the model code we are looking for
                    if (device.DeviceName == deviceName) return device;
                }
            }
            catch
            {//no devices set up currently
                return null;
            }
            // No matching model code.
            return null;
        }

        /// <summary>
        /// Load the settings for this vehicle com device
        /// </summary>
        public void Load()
        {   // Create a new list of options
            m_serialDevices = new List<SerialDevice>();
            try
            {   // Load the option association file
                if (File.Exists("SerialDeviceSettings.xml"))
                {   // Read the options from the file
                    XmlSerializer mySerializer = new XmlSerializer(typeof(List<SerialDevice>));
                    using (StreamReader reader = new StreamReader("SerialDeviceSettings.xml"))
                    {
                        m_serialDevices = (List<SerialDevice>)mySerializer.Deserialize(reader);
                    }
                }
                else
                {
                    //m_logger.Log("WARNING:  " + OptionName + ".xml does not exist, please add associations for " + OptionName);
                    File.Create("SerialDeviceSettings.xml");
                }
            }
            catch
            {
                //m_logger.Log("ERROR:  Excpetion loading option associations for " + OptionName + ", no options have been loaded");
            }
        }

        /// <summary>
        /// If the device settings exists for the specified comm channel name, remove it from the list.
        /// </summary>
        /// <param name="modelCode"></param>
        public void RemoveDeviceSettings(string deviceName)
        {
            SerialDevice device = Find(deviceName);
            if (device != null)
            {   // Remove the item from the list
                m_serialDevices.Remove(device);
                //m_logger.Log("INFO:  Removed " + OptionName + ":" + option.OptionValue + " from model code: " + modelCode);
                // Save the updated list
                Save();
            }
        }

        /// <summary>
        /// Save the option list to file.
        /// </summary>
        public void Save()
        {
            try
            {   // Serialize and save the option list
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<SerialDevice>));
                using (StreamWriter writer = new StreamWriter("SerialDeviceSettings.xml"))
                {
                    mySerializer.Serialize(writer, m_serialDevices);
                }
            }
            catch
            {
                //m_logger.Log("ERROR:  Exception saving " + OptionName + " option file");
            }
        }

        /// <summary>
        /// Update the specified option association.
        /// </summary>
        /// <param name="modelCode">Model code to be updated.</param>
        /// <param name="optionValue">New value for the model code.</param>
        public void UpdateSerialDeviceSettings(String deviceName, String portName, String baudRate, String dataBits, String parity,
            String stopBits, String readTimeout, String writeTimeout, String monitorData)
        {
            SerialDevice device = Find(deviceName);
            device.DeviceName = deviceName;
            device.PortName = portName;
            device.BaudRate = baudRate;
            device.DataBits = dataBits;
            device.DataParity = parity;
            device.DataStopBits = stopBits;
            device.ReadTimeout = readTimeout;
            device.WriteTimeout = writeTimeout;
            device.MonitorPortForData = monitorData;
            Save();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of vehicle options available for the vehicle.
        /// </summary>
        private List<SerialDevice> m_serialDevices;

        /// <summary>
        /// Get the list of configured options.
        /// </summary>
        public List<SerialDevice> SerialDeviceSettings
        {
            get { return m_serialDevices; }
        }
    }
}
