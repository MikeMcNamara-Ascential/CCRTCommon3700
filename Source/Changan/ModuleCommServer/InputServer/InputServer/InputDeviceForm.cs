using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO.Ports;
using System.Text;
using System.Windows.Forms;
using SerialPortLibrary;

namespace InputServer
{
    public partial class InputDeviceForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        public InputDeviceForm()
        {
            InitializeComponent();
            PopulatePortComboBox();
            SetDefaults();
            m_serialDeviceCollection = new SerialDeviceCollection();
            // Load the configured input devices
            LoadInputDevices();
        }
        SerialDeviceCollection m_serialDeviceCollection;
        /// <summary>
        /// Populate the port combo box with the valid ports for this system.
        /// </summary>
        private void PopulatePortComboBox()
        {   // Clear the port combo box
            m_portComboBox.Items.Clear();
            // Add the port names to the combo box
            m_portComboBox.Items.AddRange(SerialPort.GetPortNames());
            // Select the first item in the list
            m_portComboBox.Sorted = true;
            m_portComboBox.SelectedIndex = 0;
        }

        /// <summary>
        /// Set the defaults for the form.
        /// </summary>
        private void SetDefaults()
        {
            m_parityComboBox.SelectedIndex = 0;
            m_stopBitsComboBox.SelectedIndex = 0;
            m_dataBitsTextBox.Text = "8";
            m_baudRateTextBox.Text = "9600";
            m_rxTimeoutTextBox.Text = "500";
            m_txTimeoutTextBox.Text = "500";
            m_monitorDataCheckBox.CheckState = CheckState.Unchecked;
        }

        /// <summary>
        /// Load the input devices.
        /// The devices will be read from the database and loaded into the drop down list.
        /// </summary>
        private void LoadInputDevices()
        {   // Populate the drop down list with the currently configured input devices
            m_inputDeviceComboBox.Items.Clear();
            foreach (SerialDevice sd in m_serialDeviceCollection.SerialDeviceSettings)
            {
                m_inputDeviceComboBox.Items.Add(sd.DeviceName);
            }
            // Display the first item in the list
            DisplayInputDevice(0);
        }

        /// <summary>
        /// Display the port settings for the device at the given index.
        /// </summary>
        /// <param name="deviceIndex">Index of the selected device.</param>
        private void DisplayInputDevice(Int32 deviceIndex)
        {
            if ((m_inputDeviceComboBox.Items.Count >= deviceIndex) && (m_inputDeviceComboBox.Items.Count > 0))
            {   // Display the device name at the selected index
                m_inputDeviceComboBox.SelectedIndex = deviceIndex;
                // Get the port settings for the selected item
                SerialDevice sd = m_serialDeviceCollection.Find(m_inputDeviceComboBox.SelectedItem.ToString());
                m_portComboBox.SelectedIndex = m_portComboBox.FindString(sd.PortName);
                m_baudRateTextBox.Text = sd.BaudRate;
                m_parityComboBox.SelectedIndex = m_parityComboBox.FindString(sd.DataParity);
                m_dataBitsTextBox.Text = sd.DataBits;
                m_stopBitsComboBox.SelectedIndex = m_stopBitsComboBox.FindString(sd.DataStopBits);
                m_rxTimeoutTextBox.Text = sd.ReadTimeout;
                m_txTimeoutTextBox.Text = sd.WriteTimeout;
                bool monitorForData;
                if (sd.MonitorPortForData == "True")
                {
                    monitorForData = true;
                }
                else
                {
                    monitorForData = false;
                }
                m_monitorDataCheckBox.Checked = monitorForData;
            }
        }

        /// <summary>
        /// Get the port settings from the form selected by the user.
        /// </summary>
        /// <returns>PortSettings selected by the user.</returns>
        private PortSettings GetUserSelectedPortSettings()
        {
            PortSettings settings = new PortSettings();
            settings.PortName = m_portComboBox.Text;
            settings.BaudRate = Convert.ToUInt16(m_baudRateTextBox.Text);
            settings.DataParity = CcrtSerialPort.ConvertToParity(m_parityComboBox.SelectedItem.ToString());
            settings.DataBits = Convert.ToUInt16(m_dataBitsTextBox.Text);
            settings.DataStopBits = CcrtSerialPort.ConvertToStopBit(Convert.ToDouble(m_stopBitsComboBox.SelectedItem));
            settings.ReadTimeout = Convert.ToInt32(m_rxTimeoutTextBox.Text);
            settings.WriteTimeout = Convert.ToInt32(m_txTimeoutTextBox.Text);
            settings.MonitorPortForData = m_monitorDataCheckBox.Checked;
            return settings;
        }

    

        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------
        
        
        
        //-----------------------------------------------------------------------------------------
        // Class Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Save the settings and add them to the database
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveSettingsButton_Click(object sender, EventArgs e)
        {   // Get all the port setting items from the form
            string monitorForData;
            if (m_monitorDataCheckBox.Checked)
            {
                monitorForData = "True";
            }
            else
            {
                monitorForData = "False";
            }
            if (m_inputDeviceComboBox.FindString(m_inputDeviceComboBox.Text) != -1)
            {   // Input device already exists, just update the item settings in the data base

                m_serialDeviceCollection.UpdateSerialDeviceSettings(m_inputDeviceComboBox.Text, m_portComboBox.Text,
                    m_baudRateTextBox.Text, m_dataBitsTextBox.Text, m_parityComboBox.SelectedItem.ToString(),
                    m_stopBitsComboBox.SelectedItem.ToString(), m_rxTimeoutTextBox.Text, m_txTimeoutTextBox.Text, monitorForData);
            }
            else
            {   // This is a new item, so add the item to the database
                m_serialDeviceCollection.Add(m_inputDeviceComboBox.Text, m_portComboBox.Text,
                    m_baudRateTextBox.Text, m_dataBitsTextBox.Text, m_parityComboBox.SelectedItem.ToString(),
                    m_stopBitsComboBox.SelectedItem.ToString(), m_rxTimeoutTextBox.Text, m_txTimeoutTextBox.Text, monitorForData);
                string addedDevice = m_inputDeviceComboBox.Text;
                LoadInputDevices();
                DisplayInputDevice(m_inputDeviceComboBox.FindString(addedDevice));
            }
        }

        /// <summary>
        /// Display the settings for the selected item
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_inputDeviceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayInputDevice(m_inputDeviceComboBox.SelectedIndex);
        }

        /// <summary>
        /// Remove the selected device from the database.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeDeviceButton_Click(object sender, EventArgs e)
        {   // Remove the selected item from the database
            m_serialDeviceCollection.RemoveDeviceSettings(m_inputDeviceComboBox.SelectedItem.ToString());
            m_inputDeviceComboBox.Items.Remove(m_inputDeviceComboBox.SelectedItem);
            // Display the remaining items
            DisplayInputDevice(0);
        }
    }
}
