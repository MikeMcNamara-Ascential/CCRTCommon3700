using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
//using LoggingInterface;

namespace BomFileProcessor
{
    public class ModelCodeOptionsCollection
    {
        string MODELCODEOPTIONSFILENAME = "C:\\Bom\\ModelCodeOptions.xml";
        /// <summary>
        /// Create a new Model code option collection.
        /// </summary>
        /// <param name="logger">Logger object so messages can be logged.</param>
        public ModelCodeOptionsCollection(Logger logger)
        {
            m_logger = logger;
        }
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add a new model code options item to the list of model code options
        /// </summary>
        /// <param name="modelCode">Model code of options to set</param>
        /// <param name="writeESN">Boolean option to write ESN</param>
        public void Add(String modelCode, string writeESN, string ESNLeadingCharacters)
        {   // If the item already exists, remove it and add this as a new item
            Remove(modelCode);
            // Add the new item to the list
            ModelCodeOptions mco = new ModelCodeOptions(modelCode, writeESN, ESNLeadingCharacters);
            m_modelCodeOptionsList.Add(mco);
            m_logger.Log("INFO:  Added " + modelCode + " to BuildItem list");
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Check if the specified Model Code exists.
        /// </summary>
        /// <param name="itemName">Model Code to check for.</param>
        /// <returns>Flag indicating if the Model Code already exisits.</returns>
        public bool Exists(String modelCode)
        {
            return Find(modelCode) != null ? true : false;
        }

        /// <summary>
        /// Find the specified ModelCode.
        /// </summary>
        /// <param name="modelCode">Model Code to find.</param>
        /// <returns>The specifid ModelCodeOptions or null if it is not in the list</returns>
        public ModelCodeOptions Find(String modelCode)
        {
            foreach (ModelCodeOptions item in m_modelCodeOptionsList)
            {   // If the item is the one we are looking for, return it
                if (item.ModelCode == modelCode) return item;
            }
            // No matching BuildItem
            return null;
        }
        /// <summary>
        /// Load the list Model Code Options.
        /// </summary>
        public void Load()
        {   // Create a new list of model code options.
            m_modelCodeOptionsList = new List<ModelCodeOptions>();
            try
            {   // Load the build item file
                if (File.Exists(MODELCODEOPTIONSFILENAME))
                {
                    XmlSerializer myXmlSerializer = new XmlSerializer(typeof(List<ModelCodeOptions>));
                    using (StreamReader reader = new StreamReader(MODELCODEOPTIONSFILENAME))
                    {
                        m_modelCodeOptionsList = (List<ModelCodeOptions>)myXmlSerializer.Deserialize(reader);
                    }
                }
                else
                {
                    //m_logger.Log("WARNING:  " + MODELCODEOPTIONSFILENAME + " does not exist, please add model code options");
                    File.Create(MODELCODEOPTIONSFILENAME);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception loading model code options, no model code options have been loaded");
            }
        }
        
        /// <summary>
        /// If the specified modelCode exists, remove it from the list of model code options.
        /// </summary>
        /// <param name="modelCode">Name of the modelCode to remove</param>
        public void Remove(String modelCode)
        {
            if (Exists(modelCode))
            {
                m_modelCodeOptionsList.Remove(Find(modelCode));
                m_logger.Log("INFO:  Removed " + modelCode + " from Model Code Options List");
                Save();
            }
        }
        /// <summary>
        /// Save the list of model code options.
        /// </summary>
        public void Save()
        {
            try
            {   // Serialize and save the model code options
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<ModelCodeOptions>));
                using (StreamWriter writer = new StreamWriter(MODELCODEOPTIONSFILENAME))
                {
                    mySerializer.Serialize(writer, m_modelCodeOptionsList);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception saving model code options to file");
            }
        }
        
        /// <summary>
        /// Update existing model code options.
        /// </summary>
        public void UpdateItem(String modelCode, string writeESN, string ESNLeadingChars)
        {   // Find the specified item
            ModelCodeOptions item = Find(modelCode);
            item.ModelCode = modelCode;
            item.WriteESN = writeESN;
            item.ESNLeadingCharacters = ESNLeadingChars;
            Save();
        }
                
        //-----------------------------------------------------------------------------------------
        // Memeber Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of build items to be extracted from the BOM.
        /// </summary>
        static private List<ModelCodeOptions> m_modelCodeOptionsList;
        /// <summary>
        /// Get the list of configured BuildItems.
        /// </summary>
        public List<ModelCodeOptions> ModelCodeOptionsList
        {
            get { return m_modelCodeOptionsList; }
        }
                
        /// <summary>
        /// Logger object to log activities.
        /// </summary>
        public Logger m_logger;
    }
}
