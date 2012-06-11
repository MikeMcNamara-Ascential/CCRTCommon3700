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

        /// <summary>
        /// Get/Set the data header string.
        /// </summary>
        public String HeaderData
        {
            get { return m_headerData; }
            set { m_headerData = value; }
        }

        /// <summary>
        /// Get/Set the length of the incoming data header.
        /// </summary>
        public Int32 HeaderDataLength
        {
            get { return m_headerDataLength; }
            set { m_headerDataLength = value; }
        }

        /// <summary>
        /// Get/Set the data trailer string.
        /// </summary>
        public String TrailerData
        {
            get { return m_trailerData; }
            set { m_trailerData = value; }
        }

        /// <summary>
        /// Get/Set the length of the incoming data trailer.
        /// </summary>
        public Int32 TrailerDataLength
        {
            get { return m_trailerDataLength; }
            set { m_trailerDataLength = value; }
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

        /// <summary>
        /// Header data found at the start of the incoming data.
        /// </summary>
        private String m_headerData;

        /// <summary>
        /// Length of the data header string.
        /// </summary>
        private Int32 m_headerDataLength;

        /// <summary>
        /// Trailer data found at the end of the incoming data.
        /// </summary>
        private String m_trailerData;

        /// <summary>
        /// Length of the data trailer string.
        /// </summary>
        private Int32 m_trailerDataLength;
    }
}
