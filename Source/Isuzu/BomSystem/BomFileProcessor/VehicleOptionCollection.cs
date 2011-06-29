using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace BomFileProcessor
{
    public class VehicleOptionCollection
    {
        /// <summary>
        /// Create a vehicle option collection.
        /// </summary>
        /// <param name="logger">Logger object so activities can be logged.</param>
        /// <param name="optionName">Vehicle option name for ID purposes.</param>
        public VehicleOptionCollection(Logger logger, String optionName)
        {   // Save the logger
            m_logger = logger;
            OptionName = optionName;
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add a new vehicle option association.
        /// </summary>
        /// <param name="modelCode">Model Code this option applies to.</param>
        /// <param name="optionValue">Value of the vehicle option to add</param>
        public void Add(String modelCode, String optionValue)
        {   // Remove the item so it can be re-added as a new item
            RemoveOption(modelCode);
            // Create a new option and add it to the list
            VehicleOption option = new VehicleOption(modelCode, optionValue);
            m_vehicleOptions.Add(option);
            m_logger.Log("INFO:  Added " + OptionName + ":" + optionValue + " to model code " + modelCode);
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Find the option value for the specified model code.
        /// </summary>
        /// <param name="modelCode"></param>
        /// <returns></returns>
        public VehicleOption Find(String modelCode)
        {   // Search the list of model codes
            foreach (VehicleOption option in m_vehicleOptions)
            {   // Return the option if this is the model code we are looking for
                if (option.ModelCode == modelCode) return option;
            }
            // No matching model code.
            return null;
        }

        /// <summary>
        /// Load the option associations for this vehicle option
        /// </summary>
        public void Load()
        {   // Create a new list of options
            m_vehicleOptions = new List<VehicleOption>();
            try
            {   // Load the option association file
                if (File.Exists(OptionName + ".xml"))
                {   // Read the options from the file
                    XmlSerializer mySerializer = new XmlSerializer(typeof(List<VehicleOption>));
                    using (StreamReader reader = new StreamReader(OptionName + ".xml"))
                    {
                        m_vehicleOptions = (List<VehicleOption>)mySerializer.Deserialize(reader);
                    }
                }
                else
                {
                    m_logger.Log("WARNING:  " + OptionName + ".xml does not exist, please add associations for " + OptionName);
                    File.Create(OptionName + ".xml");
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception loading option associations for " + OptionName + ", no options have been loaded");
            }
        }

        /// <summary>
        /// If the option exists for the specified model code, remove it from the list.
        /// </summary>
        /// <param name="modelCode">Model code to remove from the option list.</param>
        public void RemoveOption(String modelCode)
        {
            VehicleOption option = Find(modelCode);
            if (option != null)
            {   // Remove the item from the list
                m_vehicleOptions.Remove(option);
                m_logger.Log("INFO:  Removed " + OptionName + ":" + option.OptionValue + " from model code: " + modelCode);
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
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<VehicleOption>));
                using (StreamWriter writer = new StreamWriter(OptionName + ".xml"))
                {
                    mySerializer.Serialize(writer, m_vehicleOptions);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception saving " + OptionName + " option file");
            }
        }

        /// <summary>
        /// Update the specified option association.
        /// </summary>
        /// <param name="modelCode">Model code to be updated.</param>
        /// <param name="optionValue">New value for the model code.</param>
        public void UpdateOption(String modelCode, String optionValue)
        {
            VehicleOption option = Find(modelCode);
            option.OptionValue = optionValue;
            Save();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Vehicle option name for this collection.
        /// </summary>
        private String m_optionName;
        /// <summary>
        /// Get/Set the vehicle option name for this collection.
        /// </summary>
        public String OptionName
        {
            get { return m_optionName; }
            set { m_optionName = value; }
        }

        /// <summary>
        /// List of vehicle options available for the vehicle.
        /// </summary>
        private List<VehicleOption> m_vehicleOptions;

        /// <summary>
        /// Get the list of configured options.
        /// </summary>
        public List<VehicleOption> Options
        {
            get { return m_vehicleOptions; }
        }

        /// <summary>
        /// Logger object to use for logging activities.
        /// </summary>
        private Logger m_logger;
    }
}
