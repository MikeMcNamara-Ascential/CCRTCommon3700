using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Resources;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using CcrtDataBaseLibrary;
using SerialPortLibrary;
using InputDataItemLibrary;
using Logger;
using QnxCcrtInterface;


namespace InputServer
{
    public partial class InputServerForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        public InputServerForm()
        {
            InitializeComponent();
            m_qnxInterface = null;
            // Create a new logger object
            LogFile = new CcrtLogger(InputServer.Properties.Settings.Default.LogFilePath,
                                     InputServer.Properties.Resources.ServerName,
                                     m_logTextBox,
                                     InputServer.Properties.Settings.Default.LogFileCount);
            DisplayLocalizer = new Localizer(InputServer.Properties.Settings.Default.CurrentLanguage, LogFile);
            UpdateDisplayLanguage();
            StartupMessageWindow strtWindow = new StartupMessageWindow(DisplayLocalizer);
            strtWindow.DisplayStartupMessage(DisplayLocalizer.GetLocalizedString("OpenLogFile"), Color.Black);
            strtWindow.Visible = true;
            // Set the BEP logo on the form
            Icon = Icon.FromHandle(InputServer.Properties.Resources.BepLogo.GetHicon());
            // Connect to the database to load input devices
            strtWindow.DisplayStartupMessage(DisplayLocalizer.GetLocalizedString("ConnectDatabase"), Color.Black);
            if (!File.Exists(InputServer.Properties.Settings.Default.DatabaseName))
            {   // Database location not found, allow user to locate it
                LocateDatabase();
            }
            // Make sure there is a database file to be opened
            if (File.Exists(InputServer.Properties.Settings.Default.DatabaseName))
            {
                DbInterface = new CcrtDataBaseInterface(InputServer.Properties.Settings.Default.DatabaseName);
                LogFile.Log("Connected to CCRT Database");
            }
            else
            {   // No defined database, exit the application with a warning message
                MessageBox.Show(DisplayLocalizer.GetLocalizedString("NoDatabaseFile") + Environment.NewLine +
                                DisplayLocalizer.GetLocalizedString("ExitInputServer"), 
                                DisplayLocalizer.GetLocalizedString("CouldNotFindDatabase"), 
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                LogFile.Log("Could not find database file to open - exiting");
                LogFile.CloseLogFile();
                Environment.Exit(0);
            }
            // Open all configured serial devices
            strtWindow.DisplayStartupMessage(DisplayLocalizer.GetLocalizedString("OpenInputDevices"), Color.Black);
            m_inputDevices = new List<InputDevice>();
            strtWindow.Visible = false;
            // Create a new QNX interface
            CreateQnxInterfaceThread = new Thread(CreateQnxInterface);
            CreateQnxInterfaceThread.Start();
        }

        /// <summary>
        /// Add a new row the Input Device data grid.
        /// </summary>
        /// <param name="row">Row to be added to the form.</param>
        public void AddInputDeviceRowToDataGrid(DataGridViewRow row)
        {
            if (m_inputDevDataGridView.InvokeRequired)
            {   // Create a new callback to prevent cross thread issues
                AddInputDeviceRowsCallback callback = new AddInputDeviceRowsCallback(AddInputDeviceRowToDataGrid);
                m_inputDevDataGridView.Invoke(callback, new object[] { row });
            }
            else
            {
                m_inputDevDataGridView.Rows.Add(row);
            }
        }

        /// <summary>
        /// Attempt to connect to the QNX CCRT System.
        /// </summary>
        public void QnxCcrtConnection()
        {
            while (!QnxCcrtSystemInterface.QnxConnected) Thread.Sleep(1000);
            if (QnxCcrtSystemInterface.QnxConnected)
            {
                while (true)
                {
                    if (QnxCcrtSystemInterface.QnxConnected && (m_qnxIpTextBox.BackColor != Color.LightGreen))
                    {
                        SetQnxConnectionStatus(Color.LightGreen);
                    }
                    else if (!QnxCcrtSystemInterface.QnxConnected && (m_qnxIpTextBox.BackColor != Color.Red))
                    {
                        SetQnxConnectionStatus(Color.Red);
                    }
                    Thread.Sleep(1000);
                }
            }
        }

        /// <summary>
        /// Create a new interface to the QNX CCRT system.
        /// </summary>
        public void CreateQnxInterface()
        {   // Make sure this is a valid IP Address
            Int32 attempts = 3;
            Boolean goodIpAddress = false;
            // Set the default IP Address
            SetQnxIpAddress(InputServer.Properties.Settings.Default.QnxIpAddress);
            Thread.Sleep(2000);
            QnxCcrtSystemInterface = new CcrtInterface(IPAddress.Parse(InputServer.Properties.Settings.Default.QnxIpAddress),
                                                       InputServer.Properties.Settings.Default.QnxCcrtPort, LogFile);
            LogFile.Log("Created new QNX CCRT Interface");
            // open the devices now that the QNX interface has been created
            OpenInputDevices();
            do
            {
                try
                {   
                    Ping ping = new Ping();
                    PingReply pingResult = ping.Send(IPAddress.Parse(InputServer.Properties.Settings.Default.QnxIpAddress));
                    if (pingResult.Status != IPStatus.Success)
                    {   // Could not detect QNX server, allow user to enter new IP Address
                        IpAddressForm ipForm = new IpAddressForm(DisplayLocalizer);
                        ipForm.ShowDialog();
                        QnxCcrtSystemInterface.ChangeIpAddress(IPAddress.Parse(InputServer.Properties.Settings.Default.WindowsIpAddress));
                    }
                    else if (pingResult.Status == IPStatus.Success)
                    {   // Found a possible QNX server
                        SetQnxConnectionStatus(Color.Yellow);
                        LogFile.Log("Detected QNX server at " + InputServer.Properties.Settings.Default.QnxIpAddress);
                        goodIpAddress = true;
                    }
                    else
                    {
                        SetQnxConnectionStatus(Color.Red);
                        LogFile.Log("Could not detect QNX server at " + InputServer.Properties.Settings.Default.QnxIpAddress);
                    }
                }
                catch
                {
                    SetQnxConnectionStatus(Color.Red);
                    LogFile.Log("No network connected!");
                }
            } while (!goodIpAddress && (attempts-- > 0));
            // Always start the connection status monitor thread
            // Make sure to kill off the existing connection thread
            if (ConnectionThread != null)
            {
                ConnectionThread.Abort();
            }
            // Create a new thread to attempt to connect to CCRT
            ConnectionThread = new Thread(QnxCcrtConnection);
            ConnectionThread.Start();
        }

          /// <summary>
        /// Present the user with a dialog to locate the database file.
        /// </summary>
        private void LocateDatabase()
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = "s3db";
            dlg.FileName = "CcrtDb.s3db";
            dlg.Filter =  "Database files (*.s3db)|*.s3db|All files (*.*)|*.*";
            dlg.InitialDirectory = "C:\\CCRT\\Database\\";
            dlg.Multiselect = false;
            dlg.Title = "Select CCRT Database File";
            String databaseFile = "";
            Int32 attempts = 3;
            do
            {   // Find the database file
                if (DialogResult.OK == dlg.ShowDialog())
                {   // Get the file selected
                    databaseFile = dlg.FileName;
                    InputServer.Properties.Settings.Default.DatabaseName = dlg.FileName;
                    InputServer.Properties.Settings.Default.Save();
                    LogFile.Log("User selected new database file: " + dlg.FileName);
                }
            } while ((databaseFile == "") && (attempts-- > 0));
        }

        /// <summary>
        /// New input data has been received, notify any subscribers.
        /// </summary>
        /// <param name="dataType">Type of input data received.</param>
        /// <param name="dataValue">Data value received.</param>
        public void NewInputDataReceived(String dataType, String dataValue)
        {   // Create a new thread to handle publishing this data
            NewInputDataEventArgs args = new NewInputDataEventArgs(dataType, dataValue);
            Thread notifyThread = new Thread(OnNewInputDataEvent);
            notifyThread.Start(args);
        }

        /// <summary>
        /// Open all configured input devices.
        /// </summary>
        private void OpenInputDevices()
        {   // Get a list of all configured serial devices
            List<String> devices = DbInterface.GetSerialDeviceNames(InputServer.Properties.Resources.ServerName);
            // Clear the grid
            m_inputDevDataGridView.Rows.Clear();
            foreach (String device in devices)
            {   // Get the port settings for this device
                PortSettings settings = DbInterface.ReadPortSettings(InputServer.Properties.Resources.ServerName, device);
                List<InputDataItem> dataItems = DbInterface.ReadDataItemSettings();
                LogFile.Log("Read port settings and data items for device: " + device);
                // Create a new input device
                DataGridViewRow row = new DataGridViewRow();
                row.CreateCells(m_inputDevDataGridView);
                DataGridViewColumnCollection columns = m_inputDevDataGridView.Columns;
                AddInputDeviceRowToDataGrid(row);
                InputDevice dev = null;
                if (settings.DeviceType == "Maple Keypad")
                {
                    LogFile.Log("Creating new MapleKeypadDevice for device: " + device);
                    dev = new MapleKeypadDevice(device, settings, dataItems,
                                                m_inputDevDataGridView.Rows[m_inputDevDataGridView.Rows.Count - 1],
                                                columns, LogFile, ref m_qnxInterface, this);
                }
                else
                {
                    LogFile.Log("Creating new InputDevice for device: " + device);
                    dev = new InputDevice(device, settings, dataItems,
                                          m_inputDevDataGridView.Rows[m_inputDevDataGridView.Rows.Count - 1],
                                          columns, LogFile, ref m_qnxInterface, this);
                }
                InDevices.Add(dev);
            }
        }

        /// <summary>
        /// Reopen the input devices.
        /// The current input devices will be closed and then reopened with the current settings.
        /// </summary>
        private void ReopenInputDevices()
        {   // Reopen ports as the settings may have changed
            foreach (InputDevice dev in InDevices)
            {
                dev.CloseInputDevice();
                LogFile.Log("Closed InputDevice: " + dev.DeviceName);
            }
            InDevices.Clear();
            OpenInputDevices();
        }

        /// <summary>
        /// Set the QNX connection status.
        /// </summary>
        /// <param name="color">Color to use for the current status.</param>
        private void SetQnxConnectionStatus(Color color)
        {
            if (m_qnxIpTextBox.InvokeRequired)
            {   // Create a new callback to prevent cross thread issues
                SetBackgroundColorCallback callback = new SetBackgroundColorCallback(SetQnxConnectionStatus);
                m_qnxIpTextBox.Invoke(callback, new object[] { color });
            }
            else
            {   // Just update the status
                m_qnxIpTextBox.BackColor = color;
            }
        }

        /// <summary>
        /// Set the QNX IP Address.
        /// </summary>
        /// <param name="ipAddress">IP Address to display in the text box.</param>
        private void SetQnxIpAddress(String ipAddress)
        {
            if (m_qnxIpTextBox.InvokeRequired)
            {   // Create a new callback to prevent cross thread issues
                SetQnxIpAddressCallback callback = new SetQnxIpAddressCallback(SetQnxIpAddress);
                m_qnxIpTextBox.Invoke(callback, new object[] { ipAddress });
            }
            else
            {   // Just update the text
                m_qnxIpTextBox.Text = ipAddress;
            }
        }


        /// <summary>
        /// Update the strings on the form to the selected localized language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("InputServerWindowTitle");
            fileToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("FileMenu");
            configureToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ConfigureMenu");
            viewToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ViewMenu");
            helpToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("HelpMenu");
            m_qnxIpLabel.Text = DisplayLocalizer.GetLocalizedString("QnxIpAddressLabel");
            m_inputDevLabel.Text = DisplayLocalizer.GetLocalizedString("InputDevicesLabel");
            m_logMessagesLabel.Text = DisplayLocalizer.GetLocalizedString("LogMessagesLabel");
            m_inputDevDataGridView.Columns["DevName"].HeaderText = DisplayLocalizer.GetLocalizedString("DeviceNameColumn");
            m_inputDevDataGridView.Columns["DevType"].HeaderText = DisplayLocalizer.GetLocalizedString("DeviceTypeColumn");
            m_inputDevDataGridView.Columns["DataItem"].HeaderText = DisplayLocalizer.GetLocalizedString("DataItemColumn");
            m_inputDevDataGridView.Columns["InputData"].HeaderText = DisplayLocalizer.GetLocalizedString("InputDataColumn");
            m_inputDevDataGridView.Columns["RshStatus"].HeaderText = DisplayLocalizer.GetLocalizedString("SendToQnxColumn");
            exitToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ExitMenuItem");
            addInputDeviceToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("PortSettingsMenuItem");
            dataItemsToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("DataItemsMenuItem");
            databaseLocationToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("DbLocationMenuItem");
            qNXIPAddressToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("QnxIpAddressMenuItem");
            logFileDirectoryToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("LogDirMenuItem");
            maximumLogFilesToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("MaxLogFilesMenuItem");
            displayLanguageToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("DisplayLanguageMenuItem");
            databaseToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("DatabaseMenuItem");
            logFileToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("CurrentLogMenuItem");
            previousLogFilesToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("PreviousLogMenuItem");
            aboutToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("AboutMenuItem");
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
        /// List of active input devices.
        /// </summary>
        private List<InputDevice> m_inputDevices;

        /// <summary>
        /// Get the list of input devices.
        /// </summary>
        private List<InputDevice> InDevices
        {
            get { return m_inputDevices; }
        }

        /// <summary>
        /// Logger object to log files.
        /// </summary>
        private CcrtLogger m_logger;

        /// <summary>
        /// Get/Set the logger object for this process.
        /// </summary>
        private CcrtLogger LogFile
        {
            get { return m_logger; }
            set { m_logger = value; }
        }

        /// <summary>
        /// Get/Set the interface to the QNX CCRT system.
        /// </summary>
        private CcrtInterface QnxCcrtSystemInterface { 
            get{return m_qnxInterface;}
            set { m_qnxInterface = value; }
        }
        private CcrtInterface m_qnxInterface;

        /// <summary>
        /// Thread to handle the connection to the QNX CCRT system.
        /// This will allow the connection to be attempted in the background.
        /// </summary>
        private Thread ConnectionThread { get; set; }

        /// <summary>
        /// Thread to use for creating the QNX CCRT Interface object.
        /// </summary>
        private Thread CreateQnxInterfaceThread { get; set; }

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }


        // ----------------------------------------------------------------------------------------
        // Class Callbacks, Delegates and Events
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Exit the application when the user selects the exit menu item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Close all input devices
            foreach (InputDevice dev in InDevices)
            {
                dev.CloseInputDevice();
                LogFile.Log("Closed InputDevice: " + dev.DeviceName);
            }
            // Close the connection to the QNX CCRT system
            if (QnxCcrtSystemInterface != null)
            {
                QnxCcrtSystemInterface.Shutdown();
            }
            // Close the connection to the database
            DbInterface.CloseDatabaseConnection();
            LogFile.Log("Closed database connection");
            LogFile.CloseLogFile();
            Environment.Exit(0);
        }

        /// <summary>
        /// Display the input device configuration form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addInputDeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            InputDeviceForm inputDeviceForm = new InputDeviceForm(DbInterface, DisplayLocalizer);
            inputDeviceForm.ShowDialog();
            LogFile.Log("Input devices updated, closing devices and re-opening with updated settings");
            ReopenInputDevices();
        }

        /// <summary>
        /// Allow the user to select the database location.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void databaseLocationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LocateDatabase();
            // Close the existing database and open the new one
            if (File.Exists(InputServer.Properties.Settings.Default.DatabaseName))
            {   // Close the current database
                LogFile.Log("New database file selected, reopening database connection");
                DbInterface.CloseDatabaseConnection();
                // Open the new database
                DbInterface = new CcrtDataBaseInterface(InputServer.Properties.Settings.Default.DatabaseName);
                LogFile.Log("Opened database connection");
            }
        }

        /// <summary>
        /// Display the database contents.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void databaseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DbInterface.ConnectToDatabase();
            DbInterface.DisplayDatabaseViewer();
        }

        /// <summary>
        /// Display the Input Data Item configuration form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dataItemsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            InputDataItemForm dataItemForm = new InputDataItemForm(DbInterface, DisplayLocalizer);
            dataItemForm.ShowDialog();
            LogFile.Log("Data Items updated, reopening input devices with updated input item list");
            ReopenInputDevices();
        }

        /// <summary>
        /// Allow the user to select the IP Address of the QNX server.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void qNXIPAddressToolStripMenuItem_Click(object sender, EventArgs e)
        {
            IpAddressForm ipForm = new IpAddressForm(DisplayLocalizer);
            ipForm.ShowDialog();
            // Update the IP Address in the display
            m_qnxIpTextBox.Text = InputServer.Properties.Settings.Default.QnxIpAddress;
            if (QnxCcrtSystemInterface != null)
            {
                QnxCcrtSystemInterface.ChangeIpAddress(IPAddress.Parse(InputServer.Properties.Settings.Default.WindowsIpAddress));
            }
        }

        /// <summary>
        /// Display the About dialog box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            InputServerAboutBox about = new InputServerAboutBox();
            about.ShowDialog();
        }

        /// <summary>
        /// Allow the user to select the location of the log files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void logFileDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String logName = LogFile.SelectLogFilePath();
            if ((logName.Length > 0) && (logName != InputServer.Properties.Settings.Default.LogFilePath))
            {   // Log file location has changed, save the updated setting
                InputServer.Properties.Settings.Default.LogFilePath = logName;
                InputServer.Properties.Settings.Default.Save();
            }
        }

        /// <summary>
        /// Allow the user to view the current log file or a previous log file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void logFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LogFile.ViewCurrentLogFile();
        }

        /// <summary>
        /// Display a dialog box so the user can update the maximum number of log files to keep.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void maximumLogFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MaxLogFileInputForm frm = new MaxLogFileInputForm(LogFile, DisplayLocalizer);
            frm.ShowDialog();
        }

        /// <summary>
        /// Allow the user to select previous log files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void previousLogFilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LogFile.ViewPreviousLogFile();
        }

        /// <summary>
        /// Display the form to allow the user to set the localization form.
        /// After the form is closed, the text on the form will be updated to the new localized text.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void displayLanguageToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Have the localizer handle updating the display language
            DisplayLocalizer.SwitchLocale();
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Event that will be raised when new input data is received
        /// </summary>
        public event EventHandler<NewInputDataEventArgs> NewInputDataEvent;

        /// <summary>
        /// Raise the event for all subscribers that new input data has been received.
        /// </summary>
        /// <param name="args">New input data arguments.</param>
        public virtual void OnNewInputDataEvent(object args)
        {
            Thread.Sleep(1000);   // Give time for the objects to settle out
            NewInputDataEventArgs tempArgs = (NewInputDataEventArgs)args;
            EventHandler<NewInputDataEventArgs> handler = NewInputDataEvent;
            // Make sure there are subscribers
            if (handler != null)
            {   // Raise the event
                handler(this, tempArgs);
            }
        }

        delegate void SetBackgroundColorCallback(Color color);
        delegate void SetQnxIpAddressCallback(String ipAddress);
        delegate void AddInputDeviceRowsCallback(DataGridViewRow row);
    }


    /// <summary>
    /// Calss for holding the arguments to a new data input event.
    /// </summary>
    public class NewInputDataEventArgs : EventArgs
    {
        public NewInputDataEventArgs(String dataType, String data)
        {
            DataType = dataType;
            Data = data;
        }

        /// <summary>
        /// Type of data just input
        /// </summary>
        public String DataType { get; private set; }

        /// <summary>
        /// New data value
        /// </summary>
        public String Data { get; private set; }
    }

}
