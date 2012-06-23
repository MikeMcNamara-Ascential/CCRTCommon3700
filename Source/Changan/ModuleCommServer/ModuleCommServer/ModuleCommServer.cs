using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;
using J2534ChannelLibrary;
using J2534DotNet;
using Logger;
using QnxCcrtInterface;
using VehicleCommServer;

namespace ModuleCommServer
{
    public partial class ModuleCommServerForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        public ModuleCommServerForm()
        {
            InitializeComponent();
            // Make sure the log folder has been configured
            if (ModuleCommServer.Properties.Settings.Default.LogFolder.Equals("null"))
            {
                SelectLogFolder();
            }
            Log = new CcrtLogger(ModuleCommServer.Properties.Settings.Default.LogFolder,
                                    "ModuleCommServer", m_logTextBox,
                                    ModuleCommServer.Properties.Settings.Default.MaxLogFileCount);
            LoadAbsModuleInfo();
            CommChannels = new List<String>();
            // Create a new Module interface object
            ModInterface = new ModuleInterface(Log);
            PopulateVehicleInterfaceDevices();
            // Display the configred ABS modules in the window
            DisplayConfiguredAbsModules();
            
            // Create a new QNX interface
            CreateQnxInterfaceThread = new Thread(CreateQnxInterface);
            CreateQnxInterfaceThread.Start();

        }

        /// <summary>
        /// Create a new interface to the QNX CCRT system.
        /// </summary>
        public void CreateQnxInterface()
        {   // Make sure this is a valid IP Address
            Int32 attempts = 3;
            Boolean goodIpAddress = false;
            // Set the default IP Address
            SetQnxIpAddress(ModuleCommServer.Properties.Settings.Default.QnxIpAddress);
            Thread.Sleep(2000);
            QnxCcrt = new CcrtInterface(IPAddress.Parse(ModuleCommServer.Properties.Settings.Default.WindowsIpAddress),
                                                        ModuleCommServer.Properties.Settings.Default.QnxCcrtPort, Log);
            Log.Log("Created new QNX CCRT Interface");
            do
            {
                try
                {
                    Ping ping = new Ping();
                    PingReply pingResult = ping.Send(IPAddress.Parse(ModuleCommServer.Properties.Settings.Default.QnxIpAddress));
                    if (pingResult.Status != IPStatus.Success)
                    {   // Could not detect QNX server, allow user to enter new IP Address
                        IpAddressForm ipForm = new IpAddressForm();
                        ipForm.ShowDialog();
                        QnxCcrt.ChangeIpAddress(IPAddress.Parse(ModuleCommServer.Properties.Settings.Default.WindowsIpAddress));
                    }
                    else if (pingResult.Status == IPStatus.Success)
                    {   // Found a possible QNX server
                        SetQnxConnectionStatus(Color.Yellow);
                        Log.Log("Detected QNX server at " + ModuleCommServer.Properties.Settings.Default.QnxIpAddress);
                        goodIpAddress = true;
                    }
                    else
                    {
                        SetQnxConnectionStatus(Color.Red);
                        Log.Log("Could not detect QNX server at " + ModuleCommServer.Properties.Settings.Default.QnxIpAddress);
                    }
                }
                catch
                {
                    SetQnxConnectionStatus(Color.Red);
                    Log.Log("No network connected!");
                }
            } while (!goodIpAddress && (attempts-- > 0));
            // Always start the connection status monitor thread
            // Make sure to kill off the existing connection thread
            if (ConnectionThread != null)
            {
                ConnectionThread.Abort();
                Log.Log("Aborting attempt to connect to QNX");
            }
            // Create a new thread to monitor the connection status to QNX
            ConnectionThread = new Thread(QnxCcrtConnection);
            ConnectionThread.Start();

            // Create a new thread to process the messages coming in from QNX
            QnxConnectionThread = new Thread(ProcessQnxCcrtMessages);
            QnxConnectionThread.Start();
        }

        public System.Windows.Forms.TextBox GetLoggerTextbox()
        {
            return m_logTextBox;
        }

        /// <summary>
        /// Display the configured ABS modules in the main window.
        /// </summary>
        private void DisplayConfiguredAbsModules()
        {
            m_brakeModuleDataGridView.Rows.Clear();
            foreach (BrakeModule module in BrakeModules)
            {
                String[] row = { module.ModuleName, module.CommProtocol, module.CommBus, module.ConfigFile };
                m_brakeModuleDataGridView.Rows.Add(row);
            }
        }

        /// <summary>
        /// Load the ABS module setup info.
        /// </summary>
        private void LoadAbsModuleInfo()
        {
            BrakeModules = new List<BrakeModule>();
            if (File.Exists(ModuleCommServer.Properties.Settings.Default.ModuleSetupInfo))
            {
                using (FileStream file = new FileStream(ModuleCommServer.Properties.Settings.Default.ModuleSetupInfo,
                       FileMode.Open, FileAccess.Read))
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(List<BrakeModule>),
                                                                 new XmlRootAttribute("AbsModules"));
                    BrakeModules = (List<BrakeModule>)serializer.Deserialize(file);
                }
                BrakeModules.Sort((mod1, mod2) => mod1.ModuleName.CompareTo(mod2.ModuleName));
                foreach (BrakeModule module in BrakeModules)
                {
                    module.LoadMessageTable();
                }
            }
        }

        /// <summary>
        /// Populate the list of configured J2534 devices in the window.
        /// </summary>
        private void PopulateVehicleInterfaceDevices()
        {
            m_vehicleInterfaceComboBox.Items.Clear();
            foreach (J2534Device dev in J2534Detect.GetDeviceList())
            {
                m_vehicleInterfaceComboBox.Items.Add(dev.Name);
            }
            // Select the default device
            int devIndex = 0;
            if (!ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice.Equals("null"))
            {
                devIndex = m_vehicleInterfaceComboBox.FindString(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice);
            }
            m_vehicleInterfaceComboBox.SelectedIndex = devIndex;
        }

        /// <summary>
        /// Wait for a message from the QNX CCRT system.
        /// When a message is reeived, it will be process and if necessary, the module response
        /// will be returned to the CCRT QNX system.
        /// </summary>
        private void ProcessQnxCcrtMessages()
        {
            while (!QnxCcrt.QnxConnected) Thread.Sleep(1000);

            while (true)
            {   // Wait for a message from the QNX CCRT System
                String message = QnxCcrt.WaitForQnxCcrtMessage();
                // Handle the Message
                if (message.Length > 0)
                {
                    String[] fields = message.Split(':');
                    switch (fields[0])
                    {
                        case "SetAbsType":
                            ActiveBrakeModule = BrakeModules.Find(module => module.ModuleName.Equals(fields[1]));
                            if (ActiveBrakeModule == null)
                            {
                                MessageBox.Show(fields[1] + ": Brake Module Not Configured!", "Unknown Brake Module",
                                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                            }
                            break;

                        case "Connect":   // Connect to the J2534 device.
                            ModInterface.ConnectJ2534Device();
                            break;

                        case "Disconnect":
                            break;

                        case "SendMessage":
                            List<Byte> response = ModInterface.SendModuleMessage(ActiveBrakeModule, fields[1]);
                            String respMsg = "";
                            foreach (Byte respByte in response)
                            {
                                respMsg += String.Format("{0,02:X2}", respByte);
                            }

                            respMsg = "7F,7F,7F,7F,7F";
                            QnxCcrt.Write("ModuleResponse,", respMsg);
                            //QnxCcrt.Write("ModuleResponse,", "7F,7F,7F,7F,7F");
                            //QnxCcrt.Write("ModuleResponse", "00,00,00,00,00");
                            
                            break;

                        default:
                            break;
                    }
                }
            }
        }

        /// <summary>
        /// Attempt to connect to the QNX CCRT System.
        /// </summary>
        public void QnxCcrtConnection()
        {
            while (!QnxCcrt.QnxConnected) Thread.Sleep(1000);
            if (QnxCcrt.QnxConnected)
            {
                while (true)
                {
                    if (QnxCcrt.QnxConnected && (m_qnxIpTextBox.BackColor != Color.LightGreen))
                    {
                        SetQnxConnectionStatus(Color.LightGreen);
                    }
                    else if (!QnxCcrt.QnxConnected && (m_qnxIpTextBox.BackColor != Color.Red))
                    {
                        SetQnxConnectionStatus(Color.Red);
                    }
                    Thread.Sleep(1000);
                }
            }
        }

        /// <summary>
        /// Save the brake module configuration list to file.
        /// </summary>
        private void SaveBrakeModuleConfig()
        {   // Save the ABS module configuration back to file
            using (FileStream file = new FileStream(ModuleCommServer.Properties.Settings.Default.ModuleSetupInfo,
                                                    FileMode.Create, FileAccess.Write))
            {
                XmlSerializer serializer = new XmlSerializer(typeof(List<BrakeModule>),
                                                             new XmlRootAttribute("AbsModules"));
                serializer.Serialize(file, BrakeModules);
            }
        }

        /// <summary>
        /// Display a dialog for the user to select the folder that will contain the log files.
        /// </summary>
        private void SelectLogFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select folder for log files";
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                ModuleCommServer.Properties.Settings.Default.LogFolder = dlg.SelectedPath;
                ModuleCommServer.Properties.Settings.Default.Save();
            }
        }

        /// <summary>
        /// Set the QNX connection status in a text box.
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




        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of configured ABS modules.
        /// </summary>
        private List<BrakeModule> BrakeModules { get; set; }

        /// <summary>
        /// The active brake module to use for the current test process.
        /// </summary>
        private BrakeModule ActiveBrakeModule { get; set; }

        /// <summary>
        /// List of configured comm channels.
        /// </summary>
        private List<String> CommChannels { get; set; }

        /// <summary>
        /// Thread to handle the connection to the QNX CCRT system.
        /// This will allow the connection to be attempted in the background.
        /// </summary>
        private Thread ConnectionThread { get; set; }

        /// <summary>
        /// Thread to process messages from QNX CCRT system.
        /// </summary>
        private Thread QnxConnectionThread { get; set; }
        

        /// <summary>
        /// Thread to use for creating the QNX CCRT Interface object.
        /// </summary>
        private Thread CreateQnxInterfaceThread { get; set; }

        /// <summary>
        /// Logging object to log our data and happenings.
        /// </summary>
        private CcrtLogger Log { get; set; }

        /// <summary>
        /// Module interface object to handle interfacing with the vehicle.
        /// </summary>
        private ModuleInterface ModInterface { get; set; }

        /// <summary>
        /// Object for interfacing with the QNX CCRT System.
        /// </summary>
        private CcrtInterface QnxCcrt { get; set; }



        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

        delegate void SetBackgroundColorCallback(Color color);
        delegate void SetQnxIpAddressCallback(String ipAddress);
        delegate void AddLogMessageCallback(String message);

        /// <summary>
        /// Bring up the ABS module configuration form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void brakeModulesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BrakeModuleConfigForm frm = new BrakeModuleConfigForm(BrakeModules, ModInterface.GetCommChannels());
            frm.ShowDialog();
            SaveBrakeModuleConfig();
            DisplayConfiguredAbsModules();
        }

        /// <summary>
        /// Exit the Application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Log.CloseLogFile();
            QnxCcrt.Shutdown();
            Environment.Exit(0);
        }

        /// <summary>
        /// Allow the user to configure the IP Addresses for the system.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void iPAddressesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            IpAddressForm frm = new IpAddressForm();
            frm.ShowDialog();
        }

        /// <summary>
        /// Allow the user to select the folder for the log files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void logToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SelectLogFolder();
        }

        /// <summary>
        /// Store the currently selected device as the default device to use.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_vehicleInterfaceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice = m_vehicleInterfaceComboBox.SelectedItem.ToString();
            ModuleCommServer.Properties.Settings.Default.Save();
            ModInterface.ChangeActiveJ2534Device();
        }

        /// <summary>
        /// Configure the vehicle communications.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void vehicleCommunicationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (ModInterface != null)
            {
                CommChannels = ModInterface.ConfigureVehicleComms();
            }
        }

        private void txMonitorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TxMonitorForm TxFrm = new TxMonitorForm();
            TxFrm.ShowDialog();
        }

    }
}
