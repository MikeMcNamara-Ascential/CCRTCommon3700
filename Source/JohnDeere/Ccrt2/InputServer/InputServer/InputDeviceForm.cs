using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO.Ports;
using System.Text;
using System.Windows.Forms;
using CcrtDataBaseLibrary;
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
        public InputDeviceForm(CcrtDataBaseInterface dbInterface, Localizer localizer)
        {
            InitializeComponent();
            StringLocalizer = localizer;
            // Set the text in the local language on the form
            UpdateDisplayLanguage();
            SetDefaults();
            // Save the interface to the CCRT Database
            DbInterface = dbInterface;
            // Load the configured input devices
            LoadInputDevices();
        }

        /// <summary>
        /// Display the port settings for the device at the given index.
        /// </summary>
        /// <param name="deviceIndex">Index of the selected device.</param>
        private void DisplayInputDevice(Int32 deviceIndex)
        {
            if ((m_inputDeviceComboBox.Items.Count >= deviceIndex) && 
                (m_inputDeviceComboBox.Items.Count > 0) &&
                (m_inputDeviceComboBox.Items[deviceIndex] != null))
            {   // Display the device name at the selected index
                m_inputDeviceComboBox.SelectedIndex = deviceIndex;
                // Get the port settings for the selected item
                PortSettings pSettings = DbInterface.ReadPortSettings(InputServer.Properties.Resources.ServerName,
                                                                      m_inputDeviceComboBox.SelectedItem.ToString());

                // Display the correct combo box settings
                foreach (DataGridViewRow row in m_dropDownPortSettingsDataGridView.Rows)
                {
                    Int32 index = 0;
                    String rowLabel = row.Cells[0].Value.ToString();
                    String itemName = "Unknown";
                    if (rowLabel == PortRowName)           itemName = pSettings.PortName;
                    else if (rowLabel == ParityRowName)    itemName = pSettings.DataParity.ToString();
                    else if (rowLabel == StopBitsRowName)  itemName = pSettings.DataStopBits.ToString();
                    index = ((DataGridViewComboBoxCell)row.Cells[1]).Items.IndexOf(itemName);
                    row.Cells[1].Value = ((DataGridViewComboBoxCell)row.Cells[1]).Items[index];
                }
                // Display the remaining port settings
                foreach (DataGridViewRow row in m_textItemPortSettingsDataGridView.Rows)
                {
                    String rowLabel = row.Cells[0].Value.ToString();
                    String value = "Unknown";
                    if (rowLabel == BaudRowName)                value = pSettings.BaudRate.ToString();
                    else if (rowLabel == DataBitsRowName)       value = pSettings.DataBits.ToString();
                    else if (rowLabel == RxTimeRowName)         value = pSettings.ReadTimeout.ToString();
                    else if (rowLabel == TxTimeRowName)         value = pSettings.WriteTimeout.ToString();
                    else if (rowLabel == HeaderDataRowName)     value = pSettings.HeaderData;
                    else if (rowLabel == HeaderLengthRowName)   value = pSettings.HeaderDataLength.ToString();
                    else if (rowLabel == TrailerDataRowName)    value = pSettings.TrailerData;
                    else if (rowLabel == TrailerLengthRowName)  value = pSettings.TrailerDataLength.ToString();
                    row.Cells[1].Value = value;
                }
                m_monitorDataCheckBox.Checked = pSettings.MonitorPortForData;
                Int32 devTypeIndex = m_devTypeComboBox.FindString(pSettings.DeviceType);
                m_devTypeComboBox.SelectedIndex = (devTypeIndex >= 0) ? devTypeIndex : 0;
            }
        }

        /// <summary>
        /// Get the port settings from the form selected by the user.
        /// </summary>
        /// <returns>PortSettings selected by the user.</returns>
        private PortSettings GetUserSelectedPortSettings()
        {
            PortSettings settings = new PortSettings();
            foreach (DataGridViewRow row in m_dropDownPortSettingsDataGridView.Rows)
            {
                String currentValue = ((DataGridViewComboBoxCell)row.Cells[1]).Value.ToString();
                String rowName = row.Cells[0].Value.ToString();
                if (rowName == PortRowName)
                {
                    settings.PortName = currentValue;
                }
                else if (rowName == ParityRowName)
                {
                    settings.DataParity = CcrtSerialPort.ConvertToParity(currentValue);
                }
                else if (rowName == StopBitsRowName)
                {
                    StopBits sb = StopBits.None;
                    if (currentValue == "None") sb = StopBits.None;
                    else if (currentValue == "One") sb = StopBits.One;
                    else if (currentValue == "OnePointFive") sb = StopBits.OnePointFive;
                    else if (currentValue == "Two") sb = StopBits.Two;
                    settings.DataStopBits = sb;
                }
            }
            foreach (DataGridViewRow row in m_textItemPortSettingsDataGridView.Rows)
            {
                String currentValue = row.Cells[1].Value.ToString();
                String rowName = row.Cells[0].Value.ToString();
                if (rowName == BaudRowName) settings.BaudRate = Convert.ToUInt16(currentValue);
                else if (rowName == DataBitsRowName) settings.DataBits = Convert.ToUInt16(currentValue);
                else if (rowName == RxTimeRowName) settings.ReadTimeout = Convert.ToInt32(currentValue);
                else if (rowName == TxTimeRowName) settings.WriteTimeout = Convert.ToInt32(currentValue);
                else if (rowName == HeaderDataRowName) settings.HeaderData = currentValue;
                else if (rowName == HeaderLengthRowName) settings.HeaderDataLength = Convert.ToInt32(currentValue);
                else if (rowName == TrailerDataRowName) settings.TrailerData = currentValue;
                else if (rowName == TrailerLengthRowName) settings.TrailerDataLength = Convert.ToInt32(currentValue);
            }
            settings.DeviceType = m_devTypeComboBox.SelectedItem.ToString();
            settings.MonitorPortForData = m_monitorDataCheckBox.Checked;
            return settings;
        }

        /// <summary>
        /// Load the input devices.
        /// The devices will be read from the database and loaded into the drop down list.
        /// </summary>
        private void LoadInputDevices()
        {   // Populate the drop down list with the currently configured input devices
            m_inputDeviceComboBox.Items.Clear();
            foreach (String devName in DbInterface.GetSerialDeviceNames(InputServer.Properties.Resources.ServerName))
            {
                m_inputDeviceComboBox.Items.Add(devName);
            }
            // Display the first item in the list
            DisplayInputDevice(0);
        }

        /// <summary>
        /// Set the defaults for the form.
        /// </summary>
        private void SetDefaults()
        {
            m_monitorDataCheckBox.CheckState = CheckState.Unchecked;
            m_devTypeComboBox.SelectedIndex = 0;
            // Clear the port settings grids before starting
            m_dropDownPortSettingsDataGridView.Rows.Clear();
            m_textItemPortSettingsDataGridView.Rows.Clear();
            // Populate the combo-box data grid view
            DataGridViewRow portRow = new DataGridViewRow();
            portRow.CreateCells(m_dropDownPortSettingsDataGridView);
            portRow.Cells[0].Value = PortRowName;
            DataGridViewComboBoxCell ports = new DataGridViewComboBoxCell();
            ports.Items.AddRange(SerialPort.GetPortNames());
            ports.Value = ports.Items[0];
            portRow.Cells[1] = ports;
            m_dropDownPortSettingsDataGridView.Rows.Add(portRow);
            DataGridViewRow parityRow = new DataGridViewRow();
            parityRow.CreateCells(m_dropDownPortSettingsDataGridView);
            parityRow.Cells[0].Value = ParityRowName;
            DataGridViewComboBoxCell parity = new DataGridViewComboBoxCell();
            String[] parityOptions = { "Even", "None", "Odd" };
            parity.Items.AddRange(parityOptions);
            parity.Value = parity.Items[0];
            parityRow.Cells[1] = parity;
            m_dropDownPortSettingsDataGridView.Rows.Add(parityRow);
            DataGridViewRow stopBitsRow = new DataGridViewRow();
            stopBitsRow.CreateCells(m_dropDownPortSettingsDataGridView);
            stopBitsRow.Cells[0].Value = StopBitsRowName;
            DataGridViewComboBoxCell stopBits = new DataGridViewComboBoxCell();
            String[] stopBitsOptions = { "None", "One", "OnePointFive", "Two" };
            stopBits.Items.AddRange(stopBitsOptions);
            stopBits.Value = stopBits.Items[0];
            stopBitsRow.Cells[1] = stopBits;
            m_dropDownPortSettingsDataGridView.Rows.Add(stopBitsRow);
            // Populate the text sections
            String[] baudRow = { BaudRowName, "9600" };
            m_textItemPortSettingsDataGridView.Rows.Add(baudRow);
            String[] dataBitsRow = { DataBitsRowName, "8" };
            m_textItemPortSettingsDataGridView.Rows.Add(dataBitsRow);
            String[] readRow = { RxTimeRowName, "500" };
            m_textItemPortSettingsDataGridView.Rows.Add(readRow);
            String[] writeRow = { TxTimeRowName, "500" };
            m_textItemPortSettingsDataGridView.Rows.Add(writeRow);
            String[] headDataRow = { HeaderDataRowName, "" };
            m_textItemPortSettingsDataGridView.Rows.Add(headDataRow);
            String[] headerLengthRow = { HeaderLengthRowName, "0" };
            m_textItemPortSettingsDataGridView.Rows.Add(headerLengthRow);
            String[] trailerDataRow = { TrailerDataRowName, "" };
            m_textItemPortSettingsDataGridView.Rows.Add(trailerDataRow);
            String[] trailerLengthRow = { TrailerLengthRowName, "0" };
            m_textItemPortSettingsDataGridView.Rows.Add(trailerLengthRow);
        }

        /// <summary>
        /// Display the text on the form in the local language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            this.Text = StringLocalizer.GetLocalizedString("DeviceConfigTitle");
            m_inputDeviceLabel.Text = StringLocalizer.GetLocalizedString("InputDevicesLabel");
            m_devTypeLabel.Text = StringLocalizer.GetLocalizedString("DeviceTypeColumn");
            m_portSettingsLabel.Text = StringLocalizer.GetLocalizedString("PortSettingsMenuItem");
            m_monitorDataCheckBox.Text = StringLocalizer.GetLocalizedString("MonitorDataLabel");
            m_closeButton.Text = StringLocalizer.GetLocalizedString("CloseLabel");
            m_saveSettingsButton.Text = StringLocalizer.GetLocalizedString("SaveSettingsLabel");
            m_removeDeviceButton.Text = StringLocalizer.GetLocalizedString("RemoveDeviceLabel");
        }

    

        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------
        
        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        private CcrtDataBaseInterface m_dbInterface;

        /// <summary>
        /// Get/Set the object to interface with the CCRT Database.
        /// </summary>
        private CcrtDataBaseInterface DbInterface
        {
            get { return m_dbInterface; }
            set { m_dbInterface = value; }
        }

        /// <summary>
        /// Get/Set the localizer for translating text to the display.
        /// </summary>
        private Localizer StringLocalizer { get; set; }

        /// <summary>
        /// Get the current translated value of the name of the communication port row.
        /// </summary>
        private String PortRowName
        {
            get { return StringLocalizer.GetLocalizedString("PortLabel"); }
        }

        /// <summary>
        /// Get the current translated value of the name of the parity row.
        /// </summary>
        private String ParityRowName
        {
            get { return StringLocalizer.GetLocalizedString("ParityLabel"); }
        }

        /// <summary>
        /// Get the current translated value of the name of the stop bits row.
        /// </summary>
        private String StopBitsRowName
        {
            get { return StringLocalizer.GetLocalizedString("StopBitsLabel"); }
        }

        /// <summary>
        /// Get the name of the Baud Rate row label.
        /// </summary>
        private String BaudRowName
        {
            get { return StringLocalizer.GetLocalizedString("BaudRateLabel"); }
        }

        /// <summary>
        /// Get the name of the Data Bits row label.
        /// </summary>
        private String DataBitsRowName
        {
            get { return StringLocalizer.GetLocalizedString("DataBitsLabel"); }
        }

        /// <summary>
        /// Get the name of the Receive Timeout row label.
        /// </summary>
        private String RxTimeRowName
        {
            get { return StringLocalizer.GetLocalizedString("ReadTimeoutLabel"); }
        }

        /// <summary>
        /// Get the name of the Transmit Timeout row label.
        /// </summary>
        private String TxTimeRowName
        {
            get { return StringLocalizer.GetLocalizedString("WriteTimeoutLabel"); }
        }

        /// <summary>
        /// Get the name of the Header Data row label.
        /// </summary>
        private String HeaderDataRowName
        {
            get { return StringLocalizer.GetLocalizedString("HeaderDataLabel"); }
        }

        /// <summary>
        /// Get the name of the Header Length row label.
        /// </summary>
        private String HeaderLengthRowName
        {
            get { return StringLocalizer.GetLocalizedString("HeaderLengthLabel"); }
        }

        /// <summary>
        /// Get the name of the Trailer Data row label.
        /// </summary>
        private String TrailerDataRowName
        {
            get { return StringLocalizer.GetLocalizedString("TrailerDataLabel"); }
        }

        /// <summary>
        /// Get the name of the Trailer Length row label.
        /// </summary>
        private String TrailerLengthRowName
        {
            get { return StringLocalizer.GetLocalizedString("TrailerLengthLabel"); }
        }
        

        
        //-----------------------------------------------------------------------------------------
        // Class Callbacks
        //-----------------------------------------------------------------------------------------

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
            if (m_inputDeviceComboBox.SelectedItem != null)
            {
                DbInterface.RemoveSerialPortSetting(InputServer.Properties.Resources.ServerName,
                                                    m_inputDeviceComboBox.SelectedItem.ToString());
                m_inputDeviceComboBox.Items.Remove(m_inputDeviceComboBox.SelectedItem);
                // Display the remaining items
                DisplayInputDevice(0);
            }
        }

        /// <summary>
        /// Save the settings and add them to the database
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveSettingsButton_Click(object sender, EventArgs e)
        {   // Get all the port setting items from the form
            PortSettings settings = GetUserSelectedPortSettings();
            String devName = m_inputDeviceComboBox.Text;
            if (m_inputDeviceComboBox.FindString(devName) != -1)
            {   // Input device already exists, just update the item settings in the data base
                DbInterface.UpdateSerialPortSettings(InputServer.Properties.Resources.ServerName,
                                                     devName, settings);
            }
            else
            {   // This is a new item, so add the item to the database
                DbInterface.AddSerialPortSettings(InputServer.Properties.Resources.ServerName,
                                                  devName, settings);
                LoadInputDevices();
                DisplayInputDevice(m_inputDeviceComboBox.FindString(devName));
            }
        }

    }
}
