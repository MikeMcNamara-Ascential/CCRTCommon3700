using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SerialPortLibrary
{
    public partial class PortDataForm : Form
    {
        public PortDataForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Display the current port settings.
        /// </summary>
        /// <param name="settings">Current port settings to display.</param>
        /// <param name="portOpen">Flag indicating if the port is open.</param>
        public void DisplayPortSettings(PortSettings settings, Boolean portOpen)
        {
            m_portNameTextBox.Text = settings.PortName;
            m_baudRateTextBox.Text = settings.BaudRate.ToString();
            m_parityTextBox.Text = settings.DataParity.ToString();
            m_dataBitsTextBox.Text = settings.DataBits.ToString();
            m_stopBitsTextBox.Text = settings.DataStopBits.ToString();
            m_rxTimeoutTextBox.Text = settings.ReadTimeout.ToString();
            m_txTimeoutTextBox.Text = settings.WriteTimeout.ToString();
            m_monitorDataCheckBox.CheckState = settings.MonitorPortForData ? CheckState.Checked : CheckState.Unchecked;
            m_headerDataTextBox.Text = settings.HeaderData;
            m_headerLengthTextBox.Text = settings.HeaderDataLength.ToString();
            m_trailerDataTextBox.Text = settings.TrailerData;
            m_trailerLengthTextBox.Text = settings.TrailerDataLength.ToString();
            m_devTypeTextBox.Text = settings.DeviceType;
            SetPortOpenStatus(portOpen);
        }

        /// <summary>
        /// Set the port open check box to the correct state.
        /// </summary>
        /// <param name="portOpen">Flag indicating if the port is open.</param>
        public void SetPortOpenStatus(Boolean portOpen)
        {
            m_portOpenCheckBox.CheckState = portOpen ? CheckState.Checked : CheckState.Unchecked;
        }

        /// <summary>
        /// Display the received data in the port data text box for debug purposes.
        /// </summary>
        /// <param name="direction">Direction the data is going, Rx for received data, Tx for transmitted data.</param>
        /// <param name="data">Data to be displayed in the text box.</param>
        /// <param name="byteCount">Number of bytes in the data.</param>
        delegate void DisplayBytePortDataCallback(String direction, byte[] data, Int32 byteCount);
        public void DisplayPortData(String direction, byte[] data, Int32 byteCount)
        {   // Date stamp the entry
            if (m_portDataTextBox.InvokeRequired)
            {
                DisplayBytePortDataCallback callback = new DisplayBytePortDataCallback(DisplayPortData);
                Invoke(callback, new object[] { direction, data, byteCount });
            }
            else
            {
                String dataString = DateTime.Now + " " + direction + ": ";
                for (int index = 0; index < byteCount; index++)
                {
                    dataString += (" $" + data[index].ToString("X2"));   // Display the hex values
                }
                m_portDataTextBox.AppendText(dataString + Environment.NewLine);
            }
        }

        /// <summary>
        /// Display the received data in the port data text box for debug purposes.
        /// </summary>
        /// <param name="direction">Direction the data is going, Rx for received data, Tx for transmitted data.</param>
        /// <param name="data">Data to be displayed in the text box.</param>
        /// <param name="byteCount">Number of bytes in the data.</param>
        delegate void DisplayCharPortDataCallback(String direction, char[] data, Int32 byteCount);
        public void DisplayPortData(String direction, char[] data, Int32 byteCount)
        {   // Date stamp the entry
            if (m_portDataTextBox.InvokeRequired)
            {
                DisplayCharPortDataCallback callback = new DisplayCharPortDataCallback(DisplayPortData);
                Invoke(callback, new object[] { direction, data, byteCount });
            }
            else
            {
                String dataString = new String(data, 0, byteCount);
                m_portDataTextBox.AppendText(DateTime.Now + "  " + direction + ":  " + dataString + Environment.NewLine);
            }
        }

        /// <summary>
        /// Clear the port data text box.
        /// </summary>
        public void ClearPortDataDisplay()
        {
            m_portDataTextBox.Clear();
        }

        /// <summary>
        /// Clear all data from the text box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_clearDataButton_Click(object sender, EventArgs e)
        {
            ClearPortDataDisplay();
        }
    }
}
