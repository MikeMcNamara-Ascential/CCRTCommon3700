using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BomFileProcessor
{
    public class ModelCodeOptions
    {
        public ModelCodeOptions()
        {
        }
                
        /// <summary>
        /// Create a new Model Code Options item.
        /// </summary>
        /// <param name="modelCode">model code</param>
        /// <param name="writeESN">UPC number of the item.</param>
        public ModelCodeOptions(String modelCode, string writeESN, string esnLeadingChars)
        {
            ModelCode = modelCode;
            WriteESN = writeESN;
            ESNLeadingCharacters = esnLeadingChars;
        }

        /// <summary>
        /// Model Code.
        /// </summary>
        private String m_modelCode;
        /// <summary>
        /// Get/Set the name of the build item.
        /// </summary>
        public String ModelCode
        {
            get { return m_modelCode; }
            set { m_modelCode = value; }
        }

        /// <summary>
        /// Boolean write ESN value
        /// </summary>
        private String m_writeESN;
        /// <summary>
        /// Get/Set the write ESN value.
        /// </summary>
        public String WriteESN
        {
            get { return m_writeESN; }
            set { m_writeESN = value; }
        }

        /// <summary>
        /// String to verify leading ESN characters
        /// </summary>
        private String m_esnLeadingCharacters;
        /// <summary>
        /// Get/Set the write ESN leading character values.
        /// </summary>
        public String ESNLeadingCharacters
        {
            get { return m_esnLeadingCharacters; }
            set { m_esnLeadingCharacters = value; }
        }
    }
}
