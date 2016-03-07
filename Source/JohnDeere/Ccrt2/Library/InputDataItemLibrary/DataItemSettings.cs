using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InputDataItemLibrary
{
    public class DataItemSettings
    {
        //-----------------------------------------------------------------------------------------
        // Class Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Get/Set the name of the data item.
        /// </summary>
        public String DataName
        {
            get { return m_dataName; }
            set { m_dataName = value; }
        }

        /// <summary>
        /// Get/Set the minimum length of the data item.
        /// </summary>
        public Int32 MinDataLength
        {
            get { return m_minLength; }
            set { m_minLength = value; }
        }

        /// <summary>
        /// Get/Set the maximum length of the data item.
        /// </summary>
        public Int32 MaxDataLength
        {
            get { return m_maxLength; }
            set { m_maxLength = value; }
        }


        //-----------------------------------------------------------------------------------------
        // Class Variables
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Name of the data item.
        /// </summary>
        private String m_dataName;

        /// <summary>
        /// Minimum length of the data item.
        /// </summary>
        private Int32 m_minLength;

        /// <summary>
        /// Maximum length of the data item.
        /// </summary>
        private Int32 m_maxLength;

    }
}
