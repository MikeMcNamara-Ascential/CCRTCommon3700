using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;

namespace SerialPortLibrary
{
    public class PortSettings
    {
        /// <summary>
        /// Get/Set the name of the serial port.
        /// </summary>
        private String m_portName;
        public String PortName
        {
            get { return m_portName; }
            set { m_portName = value; }
        }

        /// <summary>
        /// Get/Set the baud rate for the serial port.
        /// </summary>
        private UInt16 m_baudRate;
        public UInt16 BaudRate
        {
            get { return m_baudRate; }
            set { m_baudRate = value; }
        }

        /// <summary>
        /// Get/Set the data bits for the serial port.
        /// </summary>
        private UInt16 m_dataBits;
        public UInt16 DataBits
        {
            get { return m_dataBits; }
            set { m_dataBits = value; }
        }

        /// <summary>
        /// Get/Set the data parity for the port.
        /// </summary>
        private Parity m_parity;
        public Parity DataParity
        {
            get { return m_parity; }
            set { m_parity = value; }
        }

        /// <summary>
        /// Get/Set the data stop bits for the port.
        /// </summary>
        private StopBits m_stopBits;
        public StopBits DataStopBits
        {
            get { return m_stopBits; }
            set { m_stopBits = value; }
        }

        /// <summary>
        /// Get/Set the timeout to use when reading data from the port.
        /// </summary>
        private Int32 m_readTimeout;
        public Int32 ReadTimeout
        {
            get { return m_readTimeout; }
            set { m_readTimeout = value; }
        }

        /// <summary>
        /// Get/Set the timeout to use when writing data to the port.
        /// </summary>
        private Int32 m_writeTimeout;
        public Int32 WriteTimeout
        {
            get { return m_writeTimeout; }
            set { m_writeTimeout = value; }
        }

        /// <summary>
        /// Get/Set the flag indicating if the port should be polled for incoming data.
        /// </summary>
        private Boolean m_monitorPortForData;
        public Boolean MonitorPortForData
        {
            get { return m_monitorPortForData; }
            set { m_monitorPortForData = value; }
        }

        /// <summary>
        /// Get/Set the device type connect to the port.
        /// </summary>
        public String DeviceType { get; set; }

        /// <summary>
        /// Get/Set the header data that will come in as part of the data stream.
        /// </summary>
        private String m_headerData;
        public String HeaderData
        {
            get { return m_headerData; }
            set { m_headerData = value; }
        }

        /// <summary>
        /// Get/Set the number of characters contained in the header data.
        /// </summary>
        private Int32 m_headerDataLength;
        public Int32 HeaderDataLength
        {
            get { return m_headerDataLength; }
            set { m_headerDataLength = value; }
        }

        /// <summary>
        /// Get/Set the trailer data that will come in as part of the data stream.
        /// </summary>
        private String m_trailerData;
        public String TrailerData
        {
            get { return m_trailerData; }
            set { m_trailerData = value; }
        }

        /// <summary>
        /// Get/Set the number of characters in the trailer data.
        /// </summary>
        private Int32 m_trailerDataLength;
        public Int32 TrailerDataLength
        {
            get { return m_trailerDataLength; }
            set { m_trailerDataLength = value; }
        }
    }
}
