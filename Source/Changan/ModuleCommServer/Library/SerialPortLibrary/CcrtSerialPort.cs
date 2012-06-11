using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace SerialPortLibrary
{
    public class CcrtSerialPort
    {
        public CcrtSerialPort()
        {   // Create a data buffer for incoming data
            m_rxDataBuffer = new byte[4096];
            m_bytesRead = 0;
            // Create a port data form for debug and investigation
            m_portDataForm = new PortDataForm();
        }
        public void ShowPortDataForm()
        {
            m_portDataForm.Show();
        }

        /// <summary>
        /// Open the serial port.
        /// </summary>
        /// <returns>Flag indicating if the port was successfully opened.</returns>
        public Boolean OpenPort()
        {   // Make sure there is a serial port to open
            if (m_serialPort == null)
            {   // Make sure we have port settings
                if (m_portSettings == null)
                {   // Warn the user to configure the port settings first
                    MessageBox.Show("Port settings not configured!" + Environment.NewLine + "Enter Port Settings",
                                    "Port Open Failure", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                }
                // Create a new serial port with the specified settings
                m_serialPort = new SerialPort(m_portSettings.PortName, m_portSettings.BaudRate, m_portSettings.DataParity,
                                              m_portSettings.DataBits, m_portSettings.DataStopBits);
                // Set the read and write timeouts
                m_serialPort.ReadTimeout = m_portSettings.ReadTimeout;
                m_serialPort.WriteTimeout = m_portSettings.WriteTimeout;
                m_serialPort.DiscardNull = true;
            }
            // Update the display with the current port settings
            m_portDataForm.DisplayPortSettings(m_portSettings, m_serialPort.IsOpen);
            // Make sure to close the serial port if it is already open
            if (m_serialPort.IsOpen) 
            {   // Close the serial port
                ClosePort();
                // Wait a bit for the port to actually close
                Thread.Sleep(500);
            }
            // Open the serial port
            DialogResult retryOpen = DialogResult.No;
            do
            {
                String errorMessage = " ";
                try
                {
                    m_serialPort.Open();
                    // Create a thread for monitoring the port for incoming data
                    if (m_portSettings.MonitorPortForData) StartRxDataMonitor();
                    m_portDataForm.SetPortOpenStatus(m_serialPort.IsOpen);
                }
                catch (InvalidOperationException ioeExcpt)
                {
                    errorMessage = ioeExcpt.Message;
                }
                catch (ArgumentOutOfRangeException aorExcpt)
                {
                    errorMessage = aorExcpt.Message;
                }
                catch (IOException ioExcpt)
                {
                    errorMessage = ioExcpt.Message;
                }
                catch (UnauthorizedAccessException uaExcpt)
                {
                    errorMessage = uaExcpt.Message;
                }
                // Display an error box with the error message if the port was not opened
                if (!m_serialPort.IsOpen)
                {
                    retryOpen = MessageBox.Show("Error opening " + m_portSettings.PortName + ":" + Environment.NewLine + errorMessage,
                                                "Port Open Error", MessageBoxButtons.RetryCancel, MessageBoxIcon.Error);
                }
            } while (!m_serialPort.IsOpen && (retryOpen == DialogResult.Retry));
            // Return the port open status
            return m_serialPort.IsOpen;
        }

        /// <summary>
        /// Close the serial port.
        /// </summary>
        /// <returns>Flag indicating of the port was successfully closed.</returns>
        public Boolean ClosePort()
        {   // Make sure there is a serial port
            Boolean portClosed = false;
            if (m_serialPort == null)
            {   // There is currently no assigned serial port
                portClosed = true;
            }
            else if (m_serialPort.IsOpen)
            {   // Close the serial port
                try
                {
                    StopRxDataMonitor();
                    m_serialPort.Close();
                    Thread.Sleep(500);
                }
                catch (InvalidOperationException)
                {   // Nothing to really do.  The port is not open.
                }
                portClosed = !m_serialPort.IsOpen;
            }
            else
            {   // Port is not open
                portClosed = true;
            }
            // Update the display with the port open status
            m_portDataForm.SetPortOpenStatus(!portClosed);
            // Return the port closed status
            return portClosed;
        }

        /// <summary>
        /// Update the port settings for the serial port.
        /// The port will first be closed.  The new settings will then be saved.  Finally, a new
        /// serial port will be created and opened with the new settings.
        /// </summary>
        /// <param name="newPortSettings">New port settings.</param>
        /// <returns>Flag indicating if the port settings were successfully updated.</returns>
        public Boolean UpdatePortSettings(PortSettings newPortSettings)
        {
            DialogResult retryUpdate = DialogResult.No;
            Boolean portSettingsUpdated = false;
            do
            {   // First close the port so the new settings can be saved
                if (ClosePort())
                {   // The port has been closed, save the new port settings
                    m_portSettings = newPortSettings;
                    // Create a new serial port with the new settings
                    m_serialPort = new SerialPort(m_portSettings.PortName, m_portSettings.BaudRate, m_portSettings.DataParity,
                                                  m_portSettings.DataBits, m_portSettings.DataStopBits);
                    // Set the read and write timeouts
                    m_serialPort.ReadTimeout = m_portSettings.ReadTimeout;
                    m_serialPort.WriteTimeout = m_portSettings.WriteTimeout;
                    m_serialPort.DiscardNull = true;
                    // open the serial port
                    portSettingsUpdated = OpenPort();
                    // Display the new settings in the serial port window
                    m_portDataForm.DisplayPortSettings(m_portSettings, m_serialPort.IsOpen);
                }
                else
                {   // Could not close the port, alert the user
                    retryUpdate = MessageBox.Show("Could Not Close the Serial Port for Updating the Port Settings",
                                                  "POrt Close Failure", MessageBoxButtons.RetryCancel, MessageBoxIcon.Warning);
                }
            } while ((retryUpdate == DialogResult.Retry) && !portSettingsUpdated);
            // Return the port settings updated flag to the calling function
            return portSettingsUpdated;
        }

        /// <summary>
        /// Update the port settings for the serial port.
        /// The port will first be closed.  The new settings will then be saved.  Finally, a new
        /// serial port will be created and opened with the new settings.
        /// </summary>
        /// <param name="portName">Name of the physical serial port.</param>
        /// <param name="baudRate">Baud rate for the serial port.</param>
        /// <param name="dataParity">Parity setting.</param>
        /// <param name="dataBits">Number of data bits.</param>
        /// <param name="dataStopBits">Number of stop bits.</param>
        /// <returns>Flag indicating if the port settings were successfully updated.</returns>
        public Boolean UpdatePortSettings(String portName, UInt16 baudRate, Parity dataParity,
                                          UInt16 dataBits, StopBits dataStopBits, 
                                          Int32 readTimeout, Int32 writeTimeout, Boolean monitorPortForData)
        {   // Create a new PortSettings object with the specified settings
            PortSettings portSettings = new PortSettings();
            portSettings.PortName = portName;
            portSettings.BaudRate = baudRate;
            portSettings.DataParity = dataParity;
            portSettings.DataBits = dataBits;
            portSettings.DataStopBits = dataStopBits;
            portSettings.ReadTimeout = readTimeout;
            portSettings.WriteTimeout = writeTimeout;
            portSettings.MonitorPortForData = monitorPortForData;
            return UpdatePortSettings(portSettings);
        }

        /// <summary>
        /// Start a thread for monitoring the serial port for incoming data.
        /// </summary>
        public void StartRxDataMonitor()
        {
            if ((m_rxDataThread == null) || !m_rxDataThread.IsAlive) m_rxDataThread = new Thread(MonitorSerialPort);
            MonitorPort = true;
            m_rxDataThread.Start();
        }

        /// <summary>
        /// Stop the thread that is monitoring the serial port for incoming data.
        /// </summary>
        public void StopRxDataMonitor()
        {
            if (m_rxDataThread != null)
            {
                MonitorPort = false;
                m_rxDataThread.Join();
            }
        }

        /// <summary>
        /// Read data from the serial port receive buffer.
        /// </summary>
        /// <param name="rxData">Buffer to place the received data into.</param>
        /// <returns>Number of bytes read from the port.</returns>
        public Int32 ReadSerialData(byte[] rxData)
        {
            Int32 byteCount = m_serialPort.Read(rxData, 0, m_serialPort.BytesToRead);
            if (byteCount > 0) m_portDataForm.DisplayPortData("Rx", rxData, byteCount);
            return byteCount;
        }

        /// <summary>
        /// Read data from the serial port receive buffer.
        /// </summary>
        /// <param name="rxData">Buffer to place the received data into.</param>
        /// <returns>Number of bytes read from the port.</returns>
        public Int32 ReadSerialData(byte[] rxData, int length)
        {
            Int32 byteCount = 0;
            try
            {
                byteCount = m_serialPort.Read(rxData, 0, length);
                if (byteCount > 0) m_portDataForm.DisplayPortData("Rx", rxData, byteCount);
            }
            catch
            {//timeout
            }
            return byteCount;
        }

        /// <summary>
        /// Read data from the serial port receive buffer.
        /// </summary>
        /// <param name="rxData">Buffer to place the received data into.</param>
        /// <returns>Number of characters read from the port.</returns>
        public Int32 ReadSerialData(char[] rxData)
        {
            Int32 byteCount = m_serialPort.Read(rxData, 0, m_serialPort.BytesToRead);
            if (byteCount > 0) m_portDataForm.DisplayPortData("Rx", rxData, byteCount);
            return byteCount;
        }

        /// <summary>
        /// Write the specified data to the serial port.
        /// </summary>
        /// <param name="txData">Data bytes to write out the serial port.</param>
        public void WriteSerialData(byte[] txData, Int32 byteCount)
        {
            m_portDataForm.DisplayPortData("Tx", txData, byteCount);
            m_serialPort.Write(txData, 0, txData.Length);
        }

        /// <summary>
        /// Write the specified data to the serial port.
        /// </summary>
        /// <param name="txData">Characters to write out the serial port.</param>
        public void WriteSerialData(char[] txData, Int32 byteCount)
        {
            m_portDataForm.DisplayPortData("Tx", txData, byteCount);
            m_serialPort.Write(txData, 0, txData.Length);
        }

        /// <summary>
        /// Display the port configuration data window.
        /// </summary>
        public void ViewSerialPortData()
        {   // Make sure the data section is clear before displaying the form
            if (!m_portDataForm.Visible)
            {
                m_portDataForm.ClearPortDataDisplay();
                m_portDataForm.DisplayPortSettings(m_portSettings, m_serialPort.IsOpen);
                m_portDataForm.Show();
            }
            else
            {
                m_portDataForm.BringToFront();
            }
        }

        /// <summary>
        /// Convert a Parity type to it's string representation.
        /// </summary>
        /// <param name="parity">Parity type to convert to string.</param>
        /// <returns>String representation of parity type.</returns>
        public static String ConvertFromParity(Parity parity)
        {
            return parity.ToString();
        }

        /// <summary>
        /// Convert a string to a parity type.
        /// </summary>
        /// <param name="strParity">String representation of a parity type.</param>
        /// <returns>Parity type.</returns>
        public static Parity ConvertToParity(String strParity)
        {
            Parity parity;
            if (strParity.Equals("Even")) parity = Parity.Even;
            else if (strParity.Equals("Mark")) parity = Parity.Mark;
            else if (strParity.Equals("None")) parity = Parity.None;
            else if (strParity.Equals("Odd")) parity = Parity.Odd;
            else if (strParity.Equals("Space")) parity = Parity.Space;
            else parity = Parity.None;
            return parity;
        }

        /// <summary>
        /// Convert the StopBits type to a float.
        /// </summary>
        /// <param name="sBits">StopBits type to be converted to float.</param>
        /// <returns>Float representation of StopBits provided.</returns>
        public static double ConvertFromStopBit(StopBits sBits)
        {
            double stopBits;
            switch (sBits)
            {
            case StopBits.None:
                stopBits = 0.0;
                break;

            case StopBits.One:
                stopBits = 1.0;
                break;

            case StopBits.OnePointFive:
                stopBits = 1.5;
                break;

            case StopBits.Two:
                stopBits = 2.0;
                break;

            default:
                stopBits = 1.0;
                break;
            }
            return stopBits;
        }

        /// <summary>
        /// Convert the float representation of the StopBits to a StopBits type.
        /// </summary>
        /// <param name="sBits">Float representation of the StopBits.</param>
        /// <returns>StopBits data type.</returns>
        public static StopBits ConvertToStopBit(double sBits)
        {
            StopBits stopBits;
            if (sBits == 0.0) stopBits = StopBits.None;
            else if (sBits == 1.0) stopBits = StopBits.One;
            else if (sBits == 1.5) stopBits = StopBits.OnePointFive;
            else if (sBits == 2.0) stopBits = StopBits.Two;
            else stopBits = StopBits.One;
            return stopBits;
        }

        /// <summary>
        /// Convert the float representation of the StopBits to a StopBits type.
        /// </summary>
        /// <param name="sBits">Float representation of the StopBits.</param>
        /// <returns>StopBits data type.</returns>
        public static StopBits ConvertToStopBit(string sBits)
        {
            StopBits stopBits;
            if (sBits == "0.0") stopBits = StopBits.None;
            else if (sBits == "None") stopBits = StopBits.None;
            else if (sBits == "1.0") stopBits = StopBits.One;
            else if (sBits == "1.5") stopBits = StopBits.OnePointFive;
            else if (sBits == "2.0") stopBits = StopBits.Two;
            else stopBits = StopBits.One;
            return stopBits;
        }

        /// <summary>
        /// Monitor the serial port for incoming data.
        /// </summary>
        public static void MonitorSerialPort()
        {   // Keep monitoring the port until the system is stopped
            byte[] dataBuffer = new byte[4096];   // Allow 4K of data to be received
            int bytesRead = 0;
            while(m_serialPort.IsOpen && MonitorPort)
            {
                try
                {   // Read the bytes from the port
                    //bytesRead = m_serialPort.Read(dataBuffer, 0, m_serialPort.BytesToRead);
                    bytesRead = m_serialPort.Read(dataBuffer, 0, dataBuffer.Length);
                    if (bytesRead > 0)
                    {   // Store the received data
                        m_portDataForm.DisplayPortData("Rx", dataBuffer, m_bytesRead);
                        dataBuffer.CopyTo(m_rxDataBuffer, 0);
                        m_bytesRead = bytesRead;
                        //m_rxDataBuffer. Concat(dataBuffer);
                        // Clear the data buffer
                        dataBuffer.Initialize();
                    }
                }
                catch(TimeoutException)
                {   // Nothing special, just no data ready to be read
                }
            }
        }

        /// <summary>
        /// Get the data currently avaiable from the serial port.
        /// The receive buffer will be cleared after reading the received data.
        /// </summary>
        public byte[] RxData
        {
            get
            {
                byte[] temp = m_rxDataBuffer;
                m_rxDataBuffer.Initialize();
                return temp;
            }
        }

        /// <summary>
        /// Get/Set the port settings for this serial port.
        /// </summary>
        public PortSettings SerialPortSettings
        {
            get { return m_portSettings; }
            set { m_portSettings = value; }
        }

        /// <summary>
        /// Get the serial port object being used.
        /// </summary>
        public SerialPort SerialPortObject
        {
            get { return m_serialPort; }
        }

        /// <summary>
        /// Buffer to hold data received from the serial port.
        /// </summary>
        private static byte[] m_rxDataBuffer;
        
        /// <summary>
        /// Value indicating bytes read to the data buffer
        /// </summary>
        private static int m_bytesRead;
        /// <summary>
        /// Get/Set the port settings for this serial port.
        /// </summary>
        public int BytesRead
        {
            get { return m_bytesRead; }
            set { m_bytesRead = value; }
        }
        /// <summary>
        /// Settings for the serial port.
        /// </summary>
        private PortSettings m_portSettings;

        /// <summary>
        /// Object to interface with the physical serial port.
        /// </summary>
        private static SerialPort m_serialPort;

        /// <summary>
        /// thread to monitor the port for incoming data.
        /// </summary>
        private Thread m_rxDataThread;

        /// <summary>
        /// Form for viewing port data.
        /// </summary>
        private static PortDataForm m_portDataForm;

        /// <summary>
        /// Get/Set the flag indicating if the serial port should be monitored for data.
        /// </summary>
        private static Boolean m_monitorPort;
        private static Boolean MonitorPort
        {
            get { return m_monitorPort; }
            set { m_monitorPort = value; }
        }
    }
}
