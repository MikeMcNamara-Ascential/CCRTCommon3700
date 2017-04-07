using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using SerialPortLibrary;


namespace TeslaHostSimulator
{
    public partial class PortSetupForm : Form
    {
        public PortSetupForm()
        {
            InitializeComponent();
            m_portSettings = null;
        }

        private void PortSetupForm_Load(object sender, EventArgs e)
        {   // Populate the combo box with available comm ports
            m_serialPortComboBox.Items.AddRange(SerialPort.GetPortNames());
            if (m_portSettings == null)
            {
                if (m_serialPortComboBox.Items.Count > 0)
                {
                    m_serialPortComboBox.SelectedIndex = 0;
                }
                m_baudRateTextBox.Text = "19200";
                m_parityTextBox.Text = Parity.None.ToString();
                m_dataBitsTextBox.Text = "8";
                m_stopBitsTextBox.Text = CcrtSerialPort.ConvertFromStopBit(StopBits.One).ToString();
            }
            else
            {
                m_serialPortComboBox.SelectedIndex = m_serialPortComboBox.Items.IndexOf(m_portSettings.PortName);
                m_baudRateTextBox.Text = m_portSettings.BaudRate.ToString();
                m_parityTextBox.Text = m_portSettings.DataParity.ToString();
                m_dataBitsTextBox.Text = m_portSettings.DataBits.ToString();
                m_stopBitsTextBox.Text = CcrtSerialPort.ConvertFromStopBit(m_portSettings.DataStopBits).ToString();
            }
        }

        private void m_saveButton_Click(object sender, EventArgs e)
        {
            if (m_portSettings == null)
            {
                m_portSettings = new PortSettings();
                m_portSettings.DeviceType = "CCRT";
                m_portSettings.HeaderData = "0x02";
                m_portSettings.HeaderDataLength = 1;
                m_portSettings.MonitorPortForData = true;
                m_portSettings.ReadTimeout = 25;
                m_portSettings.TrailerData = "0x03";
                m_portSettings.TrailerDataLength = 1;
                m_portSettings.WriteTimeout = 500;
            }
            m_portSettings.PortName = m_serialPortComboBox.SelectedItem.ToString();
            m_portSettings.BaudRate = Convert.ToUInt16(m_baudRateTextBox.Text);
            m_portSettings.DataParity = CcrtSerialPort.ConvertToParity(m_parityTextBox.Text);
            m_portSettings.DataBits = Convert.ToUInt16(m_dataBitsTextBox.Text);
            m_portSettings.DataStopBits = CcrtSerialPort.ConvertToStopBit(Convert.ToSingle(m_stopBitsTextBox.Text));
        }



        public PortSettings m_portSettings { get; set; }

    }
}
