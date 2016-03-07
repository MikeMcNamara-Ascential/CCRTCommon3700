using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.IO;

namespace VehicleCommServer
{
    public class VehicleCommDeviceCollection
    {
        /// <summary>
        /// Create a vehicle comm device collection.
        /// </summary>
        /// <param name=""></param>
        public VehicleCommDeviceCollection()
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
        public void Add(string commChannelName, string deviceName, int channelIndex, string connectFlag, 
            string baudRate, string protocolID)
        {   // Remove the item so it can be re-added as a new item
            RemoveDeviceSettings(commChannelName,deviceName);
            // Create a new option and add it to the list
            VehicleCommDeviceSettings settings = new VehicleCommDeviceSettings(commChannelName, 
            deviceName,channelIndex,connectFlag, baudRate, protocolID);
            m_vehicleCommDevices.Add(settings);
            //m_logger.Log("INFO:  Added " + OptionName + ":" + optionValue + " to model code " + modelCode);
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Find the vehicle comm device settings for the specified com channel name.
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        public VehicleCommDeviceSettings Find(string commChannelName, string deviceName)
        {   // Search the list of model codes
            try
            {
                foreach (VehicleCommDeviceSettings setting in m_vehicleCommDevices)
                {   // Return the option if this is the model code we are looking for
                    if ((setting.CommChannelName == commChannelName) &&
                        setting.DeviceName == deviceName) return setting;
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
            m_vehicleCommDevices = new List<VehicleCommDeviceSettings>();
            try
            {   // Load the option association file
                if (File.Exists("VehicleCommDeviceSettings.xml"))
                {   // Read the options from the file
                    XmlSerializer mySerializer = new XmlSerializer(typeof(List<VehicleCommDeviceSettings>));
                    using (StreamReader reader = new StreamReader("VehicleCommDeviceSettings.xml"))
                    {
                        m_vehicleCommDevices = (List<VehicleCommDeviceSettings>)mySerializer.Deserialize(reader);
                    }
                }
                else
                {
                    //m_logger.Log("WARNING:  " + OptionName + ".xml does not exist, please add associations for " + OptionName);
                    File.Create("VehicleCommDeviceSettings.xml");
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
        public void RemoveDeviceSettings(string commChannelName, string deviceName)
        {
            VehicleCommDeviceSettings settings = Find(commChannelName, deviceName);
            if (settings != null)
            {   // Remove the item from the list
                m_vehicleCommDevices.Remove(settings);
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
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<VehicleCommDeviceSettings>));
                using (StreamWriter writer = new StreamWriter("VehicleCommDeviceSettings.xml"))
                {
                    mySerializer.Serialize(writer, m_vehicleCommDevices);
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
        public void UpdateVehicleDeviceSettings(string commChannelName, string deviceName, 
            int channelIndex, string connectFlag,
            string baudRate, string protocolID)
        {
            VehicleCommDeviceSettings settings = Find(commChannelName, deviceName);
            settings.ChannelIndex = channelIndex;
            settings.ConnectFlag = connectFlag;
            settings.BaudRate = baudRate;
            settings.ProtocolID = protocolID;
            Save();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of vehicle options available for the vehicle.
        /// </summary>
        private List<VehicleCommDeviceSettings> m_vehicleCommDevices;

        /// <summary>
        /// Get the list of configured options.
        /// </summary>
        public List<VehicleCommDeviceSettings> CommDeviceSettings
        {
            get { return m_vehicleCommDevices; }
        }

    }
}
