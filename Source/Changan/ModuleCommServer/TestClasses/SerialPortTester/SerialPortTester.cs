using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SerialPortLibrary;

namespace SerialPortTester
{
    public partial class SerialPortTester : Form
    {
        public SerialPortTester()
        {
            InitializeComponent();
            // Load all available serial ports
            PopulateSerialPorts();
            // Populate the form with default values
            SetFormDefaults();
            // create a new serial port instance
            m_serialPort = new CcrtSerialPort();
        }

        /// <summary>
        /// Populates the serial port combo box with all the valid ports available.
        /// </summary>
        private void PopulateSerialPorts()
        {
            m_serialPortComboBox.Items.AddRange(SerialPort.GetPortNames());
            m_serialPortComboBox.Sorted = true;
            m_serialPortComboBox.SelectedIndex = 0;
        }

        /// <summary>
        /// Set the default values for the form.
        /// </summary>
        private void SetFormDefaults()
        {
            m_baudRateTextBox.Text = "9600";
            m_parityComboBox.SelectedIndex = 1;
            m_dataBitsTextBox.Text = "8";
            m_stopBitsComboBox.SelectedIndex = 0;
            m_rxTimeoutTextBox.Text = "500";
            m_txTimeoutTextBox.Text = "500";
        }

        /// <summary>
        /// Exit the application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_exitButton_Click(object sender, EventArgs e)
        {   // Make sure the serial port is closed before exiting
            m_serialPort.ClosePort();
            Environment.Exit(0);
        }

        /// <summary>
        /// Serial port object.
        /// </summary>
        private CcrtSerialPort m_serialPort;

        /// <summary>
        /// Get the port settings from the form.
        /// </summary>
        /// <returns>Port settings selected in the form.</returns>
        private PortSettings GetPortSettings()
        {
            PortSettings portSettings = new PortSettings();
            portSettings.PortName = m_serialPortComboBox.Items[m_serialPortComboBox.SelectedIndex].ToString();
            portSettings.BaudRate = Convert.ToUInt16(m_baudRateTextBox.Text);
            switch (m_parityComboBox.SelectedIndex)
            {
            case 0:
                portSettings.DataParity = Parity.Even;
                break;

            case 1:
                portSettings.DataParity = Parity.None;
                break;

            case 2:
                portSettings.DataParity = Parity.Odd;
                break;

            default:
                portSettings.DataParity = Parity.None;
                break;
            }
            portSettings.DataBits = Convert.ToUInt16(m_dataBitsTextBox.Text);
            switch (m_stopBitsComboBox.SelectedIndex)
            {
            case 0:
                portSettings.DataStopBits = StopBits.One;
                break;

            case 1:
                portSettings.DataStopBits = StopBits.OnePointFive;
                break;

            case 2:
                portSettings.DataStopBits = StopBits.Two;
                break;

            default:
                portSettings.DataStopBits = StopBits.One;
                break;
            }
            portSettings.ReadTimeout = Convert.ToInt32(m_rxTimeoutTextBox.Text);
            portSettings.WriteTimeout = Convert.ToInt32(m_txTimeoutTextBox.Text);
            portSettings.MonitorPortForData = m_monitorPortCheckBox.Checked;
            return portSettings;
        }

        /// <summary>
        /// Open the selected serial port with the configured settings.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_openPortButton_Click(object sender, EventArgs e)
        {   // Get the port settings from the form
            m_serialPort.SerialPortSettings = GetPortSettings();
            // Open the serial port
            m_serialPort.OpenPort();
        }

        /// <summary>
        /// Close the serial port.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_closePortButton_Click(object sender, EventArgs e)
        {
            m_serialPort.ClosePort();
        }

        /// <summary>
        /// Update the port settings for the selected serial port.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_updatePortButton_Click(object sender, EventArgs e)
        {
            m_serialPort.UpdatePortSettings(GetPortSettings());
        }

        /// <summary>
        /// Display the port data dialog box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_viewPortDataButton_Click(object sender, EventArgs e)
        {
            m_serialPort.ViewSerialPortData();
        }

        /// <summary>
        /// Send the TX data to the serial port.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_txDataButton_Click(object sender, EventArgs e)
        {
            m_serialPort.WriteSerialData(m_txDataTextBox.Text.ToCharArray(), m_txDataTextBox.Text.Length);
        }

        // Read data from the serial port
        private void m_readPortButton_Click(object sender, EventArgs e)
        {
            char[] rxData = new char[4096];
            Int32 byteCount = m_serialPort.ReadSerialData(rxData);
            if (byteCount > 0)
                m_rxdataTextBox.Text = new String(rxData);
            else
                m_rxdataTextBox.Text = "No data received";
        }
    }
}
