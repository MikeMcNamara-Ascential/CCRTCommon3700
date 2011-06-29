using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace BomFileProcessor
{
    public class BuildItemCollection
    {
        /// <summary>
        /// Create a new build item collection.
        /// </summary>
        /// <param name="logger">Logger object so messages can be logged.</param>
        public BuildItemCollection(Logger logger)
        {
            m_logger = logger;
        }

        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add a new build item to the list of items to be extracted from the BOM.
        /// </summary>
        /// <param name="itemName">Name of the item to be extracted.</param>
        /// <param name="upc">UPC of the item to be extracted.</param>
        /// <param name="fna">FNA of the item to be extracted.</param>
        public void Add(String itemName, String upc, String fna)
        {   // If the item already exists, remove it and add this as a new item
            Remove(itemName);
            // Add the new item to the list
            BuildItem item = new BuildItem(itemName, upc, fna);
            m_buildItems.Add(item);
            m_logger.Log("INFO:  Added " + itemName + " to BuildItem list");
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Check if the specified BuildItem exists.
        /// </summary>
        /// <param name="itemName">Name of the BuildItem to check for.</param>
        /// <returns>Flag indicating if the BuildItem already exisits.</returns>
        public bool Exists(String itemName)
        {
            return Find(itemName) != null ? true : false;
        }

        /// <summary>
        /// Find the specified BuildItem.
        /// </summary>
        /// <param name="itemName">BuildItem to find.</param>
        /// <returns>The specifid BuildItem or null if it is not in the list</returns>
        public BuildItem Find(String itemName)
        {
            foreach (BuildItem item in m_buildItems)
            {   // If the item is the one we are looking for, return it
                if (item.ItemName == itemName) return item;
            }
            // No matching BuildItem
            return null;
        }

        /// <summary>
        /// Determine if the UPC and FNA procided match any item we are to extract from the BOM
        /// </summary>
        /// <param name="upc">UPC of the item to be checked.</param>
        /// <param name="fna">FNA of the item to be checked.</param>
        /// <returns>Flag indicating if the UPC and FNA match an item to be extracted.</returns>
        public BuildItem FindItem(String upc, String fna)
        {
            foreach (BuildItem item in m_buildItems)
            {
                if ((item.UPC == upc) && (item.FNA == fna))
                {
                    return item;
                }
            }
            return null;
        }

        /// <summary>
        /// Load the list of build items to extract from the BOM.
        /// </summary>
        public void Load()
        {   // Create a new list of build items.
            m_buildItems = new List<BuildItem>();
            try
            {   // Load the build item file
                if (File.Exists(BomFileProcessor.Properties.Resources.BuidlItemFileName))
                {
                    XmlSerializer myXmlSerializer = new XmlSerializer(typeof(List<BuildItem>));
                    using (StreamReader reader = new StreamReader(BomFileProcessor.Properties.Resources.BuidlItemFileName))
                    {
                        m_buildItems = (List<BuildItem>)myXmlSerializer.Deserialize(reader);
                    }
                }
                else
                {
                    m_logger.Log("WARNING:  " + BomFileProcessor.Properties.Resources.BuidlItemFileName + " does not exist, please add build items");
                    File.Create(BomFileProcessor.Properties.Resources.BuidlItemFileName);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception loading build items, no build items have been loaded");
            }
        }

        /// <summary>
        /// If the specified item exists, remove it from the list of BuildItems.
        /// </summary>
        /// <param name="itemName">Name of the BuildItem to remove</param>
        public void Remove(String itemName)
        {
            if (Exists(itemName))
            {
                m_buildItems.Remove(Find(itemName));
                m_logger.Log("INFO:  Removed " + itemName + " from BuildItem list");
                Save();
            }
        }

        /// <summary>
        /// Save the list of items to be extracted from the BOM.
        /// </summary>
        public void Save()
        {
            try
            {   // Serialize and save the build items
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<BuildItem>));
                using (StreamWriter writer = new StreamWriter(BomFileProcessor.Properties.Resources.BuidlItemFileName))
                {
                    mySerializer.Serialize(writer, m_buildItems);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception saving build items to file");
            }
        }

        /// <summary>
        /// Get the items from the table and update the items in the list.
        /// </summary>
        public void UpdateItem(String itemName, String upc, String fna)
        {   // Find the specified item
            BuildItem item = Find(itemName);
            item.ItemName = itemName;
            item.UPC = upc;
            item.FNA = fna;
            Save();
        }


        //-----------------------------------------------------------------------------------------
        // Memeber Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of build items to be extracted from the BOM.
        /// </summary>
        static private List<BuildItem> m_buildItems;
        /// <summary>
        /// Get the list of configured BuildItems.
        /// </summary>
        public List<BuildItem> BuildItems
        {
            get { return m_buildItems; }
        }

        /// <summary>
        /// Logger object to log activities.
        /// </summary>
        static private Logger m_logger;
    }
}
