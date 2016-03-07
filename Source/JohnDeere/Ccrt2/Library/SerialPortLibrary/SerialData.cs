using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialPortLibrary
{
    public class SerialData
    {
        /// <summary>
        /// Create a new SerialData item with a 4K buffer and the byte count set to 0.
        /// </summary>
        public SerialData()
        {
            Data = new byte[4096];
            ByteCount = 0;
        }

        /// <summary>
        /// Make a new SerialData item from the provided data array and byte count.
        /// </summary>
        /// <param name="data"></param>
        /// <param name="byteCount"></param>
        public SerialData(byte[] data, Int32 byteCount)
        {
            Data = data;
            ByteCount = byteCount;
        }

        /// <summary>
        /// Array of data bytes.
        /// </summary>
        private byte[] m_data;

        /// <summary>
        /// Get/Set the data.
        /// </summary>
        public byte[] Data
        {
            get { return m_data; }
            set { m_data = value; }
        }

        /// <summary>
        /// Number of data bytes in the array.
        /// </summary>
        private Int32 m_byteCount;

        /// <summary>
        /// Get/Set the number of bytes in the data array.
        /// </summary>
        public Int32 ByteCount
        {
            get { return m_byteCount; }
            set { m_byteCount = value; }
        }
    }
}
