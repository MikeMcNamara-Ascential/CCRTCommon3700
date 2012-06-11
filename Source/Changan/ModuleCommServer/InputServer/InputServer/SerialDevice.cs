using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SerialPortLibrary;

namespace InputServer
{
    public class SerialDevice
    {
        public SerialDevice()
        {
        }

        public SerialDevice(String deviceName, String portName, String baudRate, String dataBits, String parity,
            String stopBits, String readTimeout, String writeTimeout, String monitorData)
        {
            m_deviceName = deviceName;
            m_portName = portName;
            m_baudRate = baudRate;
            m_dataBits = dataBits;
            m_parity = parity;
            m_stopBits = stopBits;
            m_readTimeout = readTimeout;
            m_writeTimeout = writeTimeout;
            m_monitorPortForData = monitorData;
        }
        public PortSettings ConvertToPortSettings()
        {
            PortSettings settings = new PortSettings();
            settings.PortName = PortName;
            settings.BaudRate = Convert.ToUInt16(BaudRate);
            settings.DataParity = CcrtSerialPort.ConvertToParity(DataParity);
            settings.DataBits = Convert.ToUInt16(DataBits);
            settings.DataStopBits = CcrtSerialPort.ConvertToStopBit(DataStopBits);
            settings.ReadTimeout = Convert.ToInt32(ReadTimeout);
            settings.WriteTimeout = Convert.ToInt32(WriteTimeout);
            bool monitorForData;
            if (MonitorPortForData == "True")
            {
                monitorForData = true;
            }
            else
            {
                monitorForData = false;
            }
            settings.MonitorPortForData = monitorForData;
            return settings;
        }
        /// <summary>
        /// Get/Set the name of the serial port.
        /// </summary>
        private String m_deviceName;
        public String DeviceName
        {
            get { return m_deviceName; }
            set { m_deviceName = value; }
        }


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
        private String m_baudRate;
        public String BaudRate
        {
            get { return m_baudRate; }
            set { m_baudRate = value; }
        }

        /// <summary>
        /// Get/Set the data bits for the serial port.
        /// </summary>
        private String m_dataBits;
        public String DataBits
        {
            get { return m_dataBits; }
            set { m_dataBits = value; }
        }

        /// <summary>
        /// Get/Set the data parity for the port.
        /// </summary>
        private String m_parity;
        public String DataParity
        {
            get { return m_parity; }
            set { m_parity = value; }
        }

        /// <summary>
        /// Get/Set the data stop bits for the port.
        /// </summary>
        private String m_stopBits;
        public String DataStopBits
        {
            get { return m_stopBits; }
            set { m_stopBits = value; }
        }

        /// <summary>
        /// Get/Set the timeout to use when reading data from the port.
        /// </summary>
        private String m_readTimeout;
        public String ReadTimeout
        {
            get { return m_readTimeout; }
            set { m_readTimeout = value; }
        }

        /// <summary>
        /// Get/Set the timeout to use when writing data to the port.
        /// </summary>
        private String m_writeTimeout;
        public String WriteTimeout
        {
            get { return m_writeTimeout; }
            set { m_writeTimeout = value; }
        }

        /// <summary>
        /// Get/Set the flag indicating if the port should be polled for incoming data.
        /// </summary>
        private String m_monitorPortForData;
        public String MonitorPortForData
        {
            get { return m_monitorPortForData; }
            set { m_monitorPortForData = value; }
        }
    }
}
