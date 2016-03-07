using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using SerialPortLibrary;
using InputDataItemLibrary;
using Logger;
using QnxCcrtInterface;

namespace InputServer
{
    public class InputDevice
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// The parameters will be saved off to member variables.  A new CCRT serial port will be
        /// created using the supplied port settings.  The serial port will then be opened.
        /// </summary>
        /// <param name="deviceName">Name of the input device.</param>
        /// <param name="portSettings">Port settings for this input device.</param>
        /// <param name="dataGridRow">Data grid view row used for displaying info from this device.</param>
        /// <param name="columns">List of columns in the data grid we will be updating with status information.</param>
        /// <param name="dataItems">List of valid data items this device is looking for.</param>
        /// <param name="logFile">Logger object to use for logging information.</param>
        /// <param name="qnxCcrtInterface">QNX CCRT system interface object.</param>
        public InputDevice(String deviceName, PortSettings portSettings, List<InputDataItem> dataItems,
                           DataGridViewRow dataGridRow, DataGridViewColumnCollection columns, CcrtLogger logFile,
                           ref CcrtInterface qnxCcrtInterface, InputServerForm inputServer)
        {   // Save the device name
            DeviceName = deviceName;
            InputDeviceRow = dataGridRow;
            ColumnNames = columns;
            // Save the data items
            DataItems = dataItems;
            LogFile = logFile;
            QnxCcrtSystemInterface = qnxCcrtInterface;
            InSrvr = inputServer;
            // Create a new serial port and save the settings
            LogFile.Log("Opening " + portSettings.PortName, DeviceName);
            m_serialPort = new CcrtSerialPort();
            m_serialPort.SerialPortSettings = portSettings;
            if (m_serialPort.OpenPort())
            {
                m_serialPort.ViewSerialPortData(true);
                LogFile.Log(portSettings.PortName + " opened", DeviceName);
            }
            // Update our row with the new information
            UpdateDataGridRow("DevName", DeviceName, Color.White);
            UpdateDataGridRow("DevType", portSettings.DeviceType, Color.White);
            UpdateDataGridRow("DataItem", "Unknown", Color.White);
            UpdateDataGridRow("RshStatus", "Not Sent", Color.LightBlue);
            // Kick off the monitoring thread
            LogFile.Log("Starting device data monitor...", DeviceName);
            m_devDataThread = new Thread(MonitorInputData);
            m_devDataThread.Start();
            LogFile.Log("Device data monitor started", DeviceName);
        }

        /// <summary>
        /// Close the InputDevice.
        /// </summary>
        public virtual void CloseInputDevice()
        {
            LogFile.Log("Closing " + m_serialPort.SerialPortSettings.PortName, DeviceName);
            m_serialPort.ViewSerialPortData(false);
            m_serialPort.ClosePort();
            LogFile.Log(m_serialPort.SerialPortSettings.PortName + " closed", DeviceName);
        }

        /// <summary>
        /// Place the received data on the display of this input device.
        /// </summary>
        /// <param name="data">Data to display on the input device.</param>
        public virtual void DisplayInputDeviceData(object data)
        {   // Nothing generic to do.  Many input devices have no display.
        }

        /// <summary>
        /// Get the flag indicating if the device has been opened successfully.
        /// </summary>
        /// <returns>Flag indicating if the port is open.</returns>
        public Boolean IsOpen()
        {
            return m_serialPort.IsPortOpen();
        }

        /// <summary>
        /// Get data from the input device while the port is open.
        /// </summary>
        public void MonitorInputData()
        {   // Keep monitoring the device while the port remains open
            char[] data = new char[4096];
            SerialData tempData = new SerialData();
            Int32 totalBytes = 0;
            while (IsOpen())
            {   
                do
                {   // Get the most recent data on the port
                    tempData = m_serialPort.RxData;
                    // If data was found, wait a bit and check for more
                    if (tempData.ByteCount > 0)
                    {   // Save off the data and update the total number of bytes
                        for (Int32 index = 0; index < tempData.ByteCount; index++)
                        {
                            data[totalBytes + index] = Convert.ToChar(tempData.Data[index]);
                        }
                        totalBytes += tempData.ByteCount;
                        // Wait a bit before checking again
                        Thread.Sleep(InputServer.Properties.Settings.Default.DataInputTimeout);
                    }
                } while ((tempData.ByteCount > 0) && IsOpen());
                // If we actually have some data, process it
                if (totalBytes > 0)
                {
                    if (!ProcessDeviceData(data, totalBytes))
                    {   // Unknown data item, just display contents
                        String unknownDataValue = new String(data, 0, totalBytes);
                        UpdateDataGridRow("DataItem", "Unknown", Color.Red);
                        UpdateDataGridRow("InputData", unknownDataValue, Color.Red);
                        UpdateDataGridRow("RshStatus", "Not Sent", Color.Red);
                        LogFile.Log("Received unknown data type: {" + unknownDataValue + "}", DeviceName);
                    }
                    // Clear out the data and reset the counters
                    data.Initialize();
                    totalBytes = 0;
                }
                // Wait for more data
                Thread.Sleep(InputServer.Properties.Settings.Default.DataInputTimeout);
            }
        }

        /// <summary>
        /// Process the data received from the device.
        /// The type of data will be identified and displayed on the main form.
        /// </summary>
        /// <param name="data">Data received from the input device.</param>
        public virtual Boolean ProcessDeviceData(char[] data, Int32 byteCount)
        {   // Determine what data item this is
            Boolean processingComplete = false;
            foreach (InputDataItem item in DataItems)
            {
                if (item.ValidateInputData(data, byteCount))
                {
                    UpdateDataGridRow("DataItem", item.ItemSettings.DataName, Color.White);
                    String dataValue = new String(data, 0, byteCount);
                    UpdateDataGridRow("InputData", dataValue, Color.White);
                    LogFile.Log("Received data - Type: " + item.ItemSettings.DataName + "\tValue: " + dataValue, DeviceName);
                    Int32 result = -1;
                    // Send the data item to QNX
                    try
                    {
                        result = QnxCcrtSystemInterface.Write(item.ItemSettings.DataName, dataValue);
                        if (result == 0)
                        {
                            UpdateDataGridRow("RshStatus", "Sent", Color.LightGreen);
                        }
                        else
                        {
                            UpdateDataGridRow("RshStatus", "Error Sending", Color.Red);
                        }
                    }
                    catch (Exception excpt)
                    {
                        LogFile.Log("ERROR: Exception during ProcessDeviceData - " + excpt.Message);
                    }
                    processingComplete = true;
                    LogFile.Log("Sent data to QNX CCRT System: " + item.ItemSettings.DataName + "," + dataValue + "\t Result: " + 
                                (result == 0 ? "Success" : "Failure"), DeviceName);
                    // Let the InputServer know new data was received
                    InSrvr.NewInputDataReceived(item.ItemSettings.DataName, dataValue);
                }
            }
            return processingComplete;
        }

        /// <summary>
        /// Send a message to the input device.
        /// </summary>
        /// <param name="message">Message to send to the input device.</param>
        /// <param name="byteCount">Number of bytes in the message to send to the input device.</param>
        public void SendDeviceMessage(char message)
        {
            char[] msg = new char[1];
            msg[0] = message;
            SendDeviceMessage(msg, 1);
        }

        /// <summary>
        /// Send a message to the input device.
        /// </summary>
        /// <param name="message">Message to send to the input device.</param>
        /// <param name="byteCount">Number of bytes in the message to send to the input device.</param>
        public void SendDeviceMessage(char[] message, Int32 byteCount)
        {
            String msg = new String(message);
            LogFile.Log("Sending message to device: {" + msg + "} - bytes: " + byteCount.ToString(), DeviceName);
            m_serialPort.WriteSerialData(message, byteCount);
        }

        /// <summary>
        /// Update the data grid view row with the specified information.
        /// </summary>
        /// <param name="columnName">Column to update.</param>
        /// <param name="data">Data to insert into the column.</param>
        private void UpdateDataGridRow(String columnName, String data, Color backColor)
        {
            InputDeviceRow.Cells[ColumnNames[columnName].Index].Value = data;
            InputDeviceRow.Cells[ColumnNames[columnName].Index].Style.BackColor = backColor;
            LogFile.Log("Updating grid column " + columnName + " with " + data + " and back color " + backColor.ToString(), DeviceName);
        }

        //-----------------------------------------------------------------------------------------
        // Class Variable/Properties
        //-----------------------------------------------------------------------------------------
        
        /// <summary>
        /// Serial port object to use for communications.
        /// </summary>
        private CcrtSerialPort m_serialPort;

        /// <summary>
        /// Data Items configured in the system.
        /// </summary>
        private List<InputDataItem> m_dataItems;

        /// <summary>
        /// Get/Set the list of valid data items.
        /// </summary>
        private List<InputDataItem> DataItems
        {
            get { return m_dataItems; }
            set { m_dataItems = value; }
        }

        /// <summary>
        /// Name of the input device.
        /// </summary>
        private String m_deviceName;

        /// <summary>
        /// Get/Set the name of the input device.
        /// </summary>
        public String DeviceName
        {
            get { return m_deviceName; }
            set { m_deviceName = value; }
        }

        /// <summary>
        /// Data grid row to use for displaying information from this input device.
        /// </summary>
        private DataGridViewRow m_inputDeviceDataGridRow;

        /// <summary>
        /// Get/Set the data grid view row being used for this input device.
        /// </summary>
        private DataGridViewRow InputDeviceRow
        {
            get { return m_inputDeviceDataGridRow; }
            set { m_inputDeviceDataGridRow = value; }
        }

        /// <summary>
        /// Column names for the cells in our data grid row.
        /// </summary>
        private DataGridViewColumnCollection m_columnNames;

        /// <summary>
        /// Get/Set the list of column names in our data grid row.
        /// </summary>
        private DataGridViewColumnCollection ColumnNames
        {
            get { return m_columnNames; }
            set { m_columnNames = value; }
        }

        /// <summary>
        /// Thread to use for monitoring for data from the device.
        /// </summary>
        private Thread m_devDataThread;

        /// <summary>
        /// Get/Set the logger object.
        /// </summary>
        public CcrtLogger LogFile { get; set; }

        /// <summary>
        /// Get/Set the interface object for the QNX CCRT system.
        /// </summary>
        private CcrtInterface QnxCcrtSystemInterface { get; set; }

        /// <summary>
        /// Get/Set the InputServer object that created this.  Any data that needs to go 
        /// </summary>
        private InputServerForm InSrvr { get; set; }

        //-----------------------------------------------------------------------------------------
        // Class Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

    }
}
