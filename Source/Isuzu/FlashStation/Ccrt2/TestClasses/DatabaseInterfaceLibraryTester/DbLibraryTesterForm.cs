using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using CcrtDataBaseLibrary;
using SerialPortLibrary;

namespace DatabaseInterfaceLibraryTester
{
    public partial class DbLibraryTesterForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor
        /// </summary>
        public DbLibraryTesterForm()
        {
            InitializeComponent();
            SetFormDefaults();
            // Create a new database interface and connect to the database
            DbInterface = new CcrtDataBaseInterface("E:\\Projects\\CcrtDatabase\\CcrtDatabase.mdf");
            // Display the initial contents of the database
            DisplayDatabaseEntries();
        }

        /// <summary>
        /// Set all the default values in the form.
        /// </summary>
        private void SetFormDefaults()
        {   // Load and display the port names
            m_serialPortComboBox.Items.Clear();
            m_serialPortComboBox.Sorted = true;
            m_serialPortComboBox.Items.AddRange(SerialPort.GetPortNames());
            m_serialPortComboBox.SelectedIndex = 0;
            // Set the remaining defaults
            m_baudRateTextBox.Text = "9600";
            m_parityComboBox.SelectedIndex = 0;
            m_dataBitsTextBox.Text = "8";
            m_stopBitsComboBox.SelectedIndex = 0;
            m_monitorDataCheckBox.CheckState = CheckState.Unchecked;
            m_rxTimeoutTextBox.Text = "500";
            m_txTimeoutTextBox.Text = "250";
        }

        /// <summary>
        /// Get the settings selected by the user.
        /// </summary>
        /// <returns></returns>
        private PortSettings GetSelectedPortSettings()
        {
            PortSettings settings = new PortSettings();
            settings.PortName = m_serialPortComboBox.SelectedItem.ToString();
            settings.BaudRate = Convert.ToUInt16(m_baudRateTextBox.Text);
            settings.DataBits = Convert.ToUInt16(m_dataBitsTextBox.Text);
            switch(m_parityComboBox.SelectedIndex)
            {
            case 0:  // Even
                settings.DataParity = Parity.Even;
                break;

            case 1:  // None
                settings.DataParity = Parity.None;
                break;

            case 2:  // Odd
                settings.DataParity = Parity.Odd;
                break;

            default:  // None
                settings.DataParity = Parity.None;
                break;
            }
            switch(m_stopBitsComboBox.SelectedIndex)
            {
            case 0:  // None
                settings.DataStopBits = StopBits.None;
                break;

            case 1:  // 1
                settings.DataStopBits = StopBits.One;
                break;

            case 2:  // 1.5
                settings.DataStopBits = StopBits.OnePointFive;
                break;

            case 3:  // 2
                settings.DataStopBits = StopBits.Two;
                break;

            default:  // 1
                settings.DataStopBits = StopBits.One;
                break;
            }
            settings.MonitorPortForData = (m_monitorDataCheckBox.CheckState == CheckState.Checked);
            settings.ReadTimeout = Convert.ToInt32(m_rxTimeoutTextBox.Text);
            settings.WriteTimeout = Convert.ToInt32(m_txTimeoutTextBox.Text);
            return settings;
        }

        /// <summary>
        /// Display the contents of the database.
        /// </summary>
        private void DisplayDatabaseEntries()
        {   // All entries from the database
            List<PortSettings> portSettings = DbInterface.ReadPortSettings(DbTesterServerName);
            List<String> devNames = DbInterface.GetSerialDeviceNames(DbTesterServerName);
            // Clear the display first
            m_portSettingsDataGridView.Rows.Clear();
            Int16 index = 0;
            foreach (PortSettings setting in portSettings)
            {
                DataGridViewRow displayRow = new DataGridViewRow();
                displayRow.CreateCells(m_portSettingsDataGridView);
                displayRow.Cells[m_portSettingsDataGridView.Columns["DeviceName"].Index].Value = devNames[index++];
                displayRow.Cells[m_portSettingsDataGridView.Columns["PortName"].Index].Value = setting.PortName;
                displayRow.Cells[m_portSettingsDataGridView.Columns["BaudRate"].Index].Value = setting.BaudRate;
                displayRow.Cells[m_portSettingsDataGridView.Columns["ParitySetting"].Index].Value = CcrtSerialPort.ConvertFromParity(setting.DataParity);
                displayRow.Cells[m_portSettingsDataGridView.Columns["DataBits"].Index].Value = setting.DataBits.ToString();
                displayRow.Cells[m_portSettingsDataGridView.Columns["StopBitSetting"].Index].Value = CcrtSerialPort.ConvertFromStopBit(setting.DataStopBits);
                displayRow.Cells[m_portSettingsDataGridView.Columns["RxTimeout"].Index].Value = setting.ReadTimeout.ToString();
                displayRow.Cells[m_portSettingsDataGridView.Columns["TxTimeout"].Index].Value = setting.WriteTimeout.ToString();
                displayRow.Cells[m_portSettingsDataGridView.Columns["MonitorData"].Index].Value = setting.MonitorPortForData ? "True" : "False";
                m_portSettingsDataGridView.Rows.Add(displayRow);
            }
        }

        // ----------------------------------------------------------------------------------------
        // Class Variables and Properties
        // ----------------------------------------------------------------------------------------
        
        /// <summary>
        /// Ccrt database interface.
        /// </summary>
        private CcrtDataBaseInterface m_dbInterface;

        /// <summary>
        /// Get/Set the interface to the CCRT database.
        /// </summary>
        private CcrtDataBaseInterface DbInterface
        {
            get { return m_dbInterface; }
            set { m_dbInterface = value; }
        }

        /// <summary>
        /// Get the name of the server for testing purposes
        /// </summary>
        private const String m_serverName = "DbTester";
        private String DbTesterServerName
        {
            get { return m_serverName; }
        }


        // ----------------------------------------------------------------------------------------
        // Class Callbacks
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Add the selected data to the database
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addEntryButton_Click(object sender, EventArgs e)
        {   // Get the port settings from the form
            PortSettings settings = GetSelectedPortSettings();
            DbInterface.AddSerialPortSettings(DbTesterServerName, m_devNameTextBox.Text, settings);
            DisplayDatabaseEntries();
        }

        private void m_updatePortSettingsButton_Click(object sender, EventArgs e)
        {   // Get the port settings from the form
            PortSettings settings = GetSelectedPortSettings();
            DbInterface.UpdateSerialPortSettings(DbTesterServerName, m_devNameTextBox.Text, settings);
            DisplayDatabaseEntries();
        }

        private void m_removePortButton_Click(object sender, EventArgs e)
        {   // Remove the item from the database
            DbInterface.RemoveSerialPortSetting(DbTesterServerName, m_devNameTextBox.Text);
            DisplayDatabaseEntries();
        }

        private void m_displayViewerButton_Click(object sender, EventArgs e)
        {   // Display the database viewer
            DbInterface.DisplayDatabaseViewer();
        }
    }
}
