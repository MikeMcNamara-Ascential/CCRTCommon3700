using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BomFileProcessor
{
    public class BuildItem
    {
        public BuildItem()
        {
        }

        /// <summary>
        /// Create a new build item to be extracted from the BOM.
        /// </summary>
        /// <param name="itemName">Name of the item to extract.</param>
        /// <param name="upc">UPC number of the item.</param>
        /// <param name="fna">FNA number of the item.</param>
        public BuildItem(String itemName, String upc, String fna)
        {
            ItemName = itemName;
            UPC = upc;
            FNA = fna;
        }

        /// <summary>
        /// Name of the build item.
        /// </summary>
        private String m_itemName;
        /// <summary>
        /// Get/Set the name of the build item.
        /// </summary>
        public String ItemName
        {
            get { return m_itemName; }
            set { m_itemName = value; }
        }

        /// <summary>
        /// The UPC number for the item
        /// </summary>
        private String m_upc;
        /// <summary>
        /// Get/Set the UPC number for the build item.
        /// </summary>
        public String UPC
        {
            get { return m_upc; }
            set { m_upc = value; }
        }

        /// <summary>
        /// The FNA number for the item.
        /// </summary>
        private String m_fna;
        /// <summary>
        /// Get/Set the FNA number for the build item.
        /// </summary>
        public String FNA
        {
            get { return m_fna; }
            set { m_fna = value; }
        }
    }
}
