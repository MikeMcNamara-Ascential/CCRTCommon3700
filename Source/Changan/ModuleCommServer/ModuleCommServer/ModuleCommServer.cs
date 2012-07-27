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
using System.Diagnostics;
namespace ModuleCommServer
{
    public partial class ModuleCommServerForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        public ModuleCommServerForm()
        {

            WriteLog("-------------------------");
            InitializeComponent();
            bool initXmlFiles = false;
            try
            {

                initXmlFiles = ModuleCommServer.Properties.Settings.Default.LogFolder.Equals("null");
            }
            catch (System.Configuration.ConfigurationException e)
            {
                Debug.WriteLine("error loading default values: " + e.ToString());
            }

            // Make sure the log folder has been configured
            if (initXmlFiles)
            {
                SelectLogFolder();
            }
            Log = new CcrtLogger(ModuleCommServer.Properties.Settings.Default.LogFolder,
                                    "ModuleCommServer", m_logTextBox,
                                    ModuleCommServer.Properties.Settings.Default.MaxLogFileCount);
            Log.Log("LoadAbsModuleInfo");
            LoadAbsModuleInfo();
            Log.Log("new List<String>()");
            CommChannels = new List<String>();
            // Create a new Module interface object
            Log.Log("new ModuleInterface(Log)");
            ModInterface = new ModuleInterface(Log);
            Log.Log("PopulateVehicleInterfaceDevices");
            PopulateVehicleInterfaceDevices();
            // Display the configred ABS modules in the window
            Log.Log("DisplayConfiguredAbsModules");
            DisplayConfiguredAbsModules();

            // Create a new QNX interface
            Log.Log("CreateQnxInterfaceThread");
            CreateQnxInterfaceThread = new Thread(CreateQnxInterface);
            Log.Log("CreateQnxInterfaceThread -start");
            CreateQnxInterfaceThread.Start();
            Log.Log("CreateQnxInterfaceThread - init finished.");
            m_disconnectedCmdStatus = false;

        }

        /// <summary>
        /// Create a new interface to the QNX CCRT system.
        /// </summary>
        public void CreateQnxInterface()
        {   // Make sure this is a valid IP Address
            Int32 attempts = 3;
            Boolean goodIpAddress = false;
            // Set the default IP Address
            Log.Log("setting default ip addr");
            SetQnxIpAddress(ModuleCommServer.Properties.Settings.Default.QnxIpAddress);
            Thread.Sleep(2000);
            Log.Log("creating new ccrt interface");
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
                Log.Log("Filename: " + ModuleCommServer.Properties.Settings.Default.ModuleSetupInfo.ToString());
                using (FileStream file = new FileStream(ModuleCommServer.Properties.Settings.Default.ModuleSetupInfo,
                       FileMode.Open, FileAccess.Read))
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(List<BrakeModule>),
                                                                 new XmlRootAttribute("AbsModules"));
                    BrakeModules = (List<BrakeModule>)serializer.Deserialize(file);
                }
                Log.Log("LoadAbsModuleInfo(): 1");
                BrakeModules.Sort((mod1, mod2) => mod1.ModuleName.CompareTo(mod2.ModuleName));
                Log.Log("LoadAbsModuleInfo(): 2");
                foreach (BrakeModule module in BrakeModules)
                {
                    Log.Log("module: " + module.ModuleName);
                    module.setLogger(Log);
                    module.LoadMessageTable();
                }
            }
        }

        /// <summary>
        /// Populate the list of configured J2534 devices in the window.
        /// </summary>
        private void PopulateVehicleInterfaceDevices()
        {
            try
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
            catch (Exception e)
            {
                Log.Log("Could not find VehicleInterface Devices");
            }
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
                if (QnxCcrt.QnxConnected)
                {
                    String message = QnxCcrt.WaitForQnxCcrtMessage();

                    ProcessQnxCcrtMessage(message);
                }
            }
        }

        private void ProcessQnxCcrtMessage(string message)
        {
            string respMsg = "";
            // Handle the Message
            if (message.Length > 0)
            {
                Log.Log("ProcessQnxCcrtMessage: " + message + "\n");
                String[] fields = message.Split(':');
                int control = 0;
                string cmd = "";
                string value = "";
                bool process = false;
                foreach (string msg in fields)
                {
                    if (msg.Length > 0)
                    {
                        if (0 == control % 2)
                        {
                            cmd = msg.Trim();
                            /*
                            if (cmd == "Disconnect")
                            {
                                process = true;
                                control++;
                            }
                             * */
                        }
                        else
                        {
                            process = true;
                            value = msg.Trim();
                        }

                        control++;

                        if (process)
                        {
                            
                            switch (cmd)
                            {
                                
                                case "SetAbsType":
                                    ActiveBrakeModule = BrakeModules.Find(module => module.ModuleName.Equals(value));
                                    if (ActiveBrakeModule == null)
                                    {
                                        MessageBox.Show(msg + ": Brake Module Not Configured!", "Unknown Brake Module",
                                                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    }

                                    ModInterface.SetActiveBrakeModule(ActiveBrakeModule);
                                    
                                    respMsg = "FF,03," + ((ModInterface.GetActiveBrakeModule() == null) ? "10" : "00") + ",00";
                                    WriteToQNX("ModuleResponse", respMsg);
                                    //WriteToQNX("ModuleResponse", "FF,01,00,01");
                                    break;

                                case "Connect":   // Connect to the J2534 device.
                                    bool status = false;

                                    WriteLog("ModInterface.ConnectJ2534Device()");


                                    ModInterface.ConnectJ2534Device();

                                    Log.Log("Connection type: " + value);
                                    // perform init for module
                                    status = ModInterface.InitilizeVehicleModuleInterface(value, ActiveBrakeModule);
                                    if (status)
                                    {
                                        ModInterface.InitiateKeepAliveMessage();
                                        Log.Log("Module keep alive messages started");
                                    }
                                    else
                                    {
                                        Log.Log("Could not initialize the vehicle");
                                    }
                                    //WriteToQNX("ModuleResponse", "FF,02,00,01");
                                    respMsg = "FF,04," + (status && (ModInterface.IsVehicleInterfaceConnected()) ? "00" : "02") + ",00";
                                    WriteToQNX("ModuleResponse", respMsg);
                                    break;

                                case "Disconnect":
                                    m_disconnectedCmdStatus = ModInterface.ShutdownModuleInterface();
                                    WriteToQNX("ModuleResponse", "FF,02," + (((m_disconnectedCmdStatus) ? "00" : "10") + ",00"));
                                    Log.Log("Disconnecting from brake module");
                                    break;

                                case "SendMessage":
                                    List<Byte> response;

                                    if (chBx_simulateVehicle.Checked)
                                        respMsg = checkForModuleDebugCmd(value);

                                    if ( respMsg.Length == 0 )
                                    switch (value)
                                    {
                                        case "WinCcrtBrakeStatus":
                                            respMsg = "FF,03," + ((ModInterface.GetActiveBrakeModule() == null) ? "10" : "00" )  + ",00";
                                            break;
                                        case "WinCcrtConnectStatus":
                                            // if vehicle is connected, send true (00) else send false (10)
                                            respMsg = "FF,04," + ((ModInterface.IsVehicleInterfaceConnected()) ? "00" : "02") + ",00";
                                            break;
                                        case "WinCcrtDisconnectStatus":
                                            respMsg = "FF,05," + ((m_disconnectedCmdStatus) ? "10" : "00" ) + ",00";
                                            break;
                                        case "StartModuleKeepAlive":
                                            ModInterface.StartKeepAliveMessage();
                                            respMsg = "7E,7E";
                                                break;
                                        case "StopModuleKeepAlive":
                                            ModInterface.StopKeepAliveMessage();
                                            respMsg = "7E,7E";
                                            break;
                                        case "IsCableConnected":
                                            respMsg = "FF,05," + ((ModInterface.IsVehicleInterfaceConnected()) ? "01" : "00") + ",00";
                                            break;
                                        default:
                                            response = ModInterface.SendModuleMessage(ActiveBrakeModule, value);
                                            foreach (Byte respByte in response)
                                            {
                                                respMsg += String.Format("{0,02:X2},", respByte);
                                            }
                                            if (respMsg.Length > 1)
                                            {
                                                respMsg = respMsg.Remove(respMsg.Length - 1);
                                            }
                                            break;
                                    }

                                    WriteToQNX("ModuleResponse", respMsg);

                                    break;

                                default:
                                    Log.Log("No activity for cmd: " + cmd + " value: " + value);
                                    break;
                            }

                            process = false;
                        }
                    }
                }
            }
        }
        private string checkForModuleDebugCmd(string value)
        {
            string rVal = "";

            switch (value)
            {
                case "EnterDiagnosticMode":         rVal = "50,89"; break;
                case "RequestCoding":               rVal = "71,B8,01,05"; break;
                case "SendCoding":                  rVal = "7B,9A"; break;
                case "StartEolSession":             rVal = "50,83"; break;
                case "ReadEcuId":                   rVal = "5A,54,20,4D,4B,37,30,4D,20,20,20,20,20"; break;    //rVal = "5A,9B,00"; break;
                case "ReadFaults":                  rVal = "58,00"; break;
                case "ClearFaults":                 rVal = "54,FF,00"; break;
                case "ReadBrakeSwitch":             rVal = "61,03,25,00,A7,25,00,88,25,00,87"; break; //rVal = "50,00,35"; break;
                case "ReadWheelSpeed":              rVal = "50,00,35"; break;
                case "RunPumpMotor":                rVal = "70,80,00"; break;
                case "ActuateLeftFrontReduction":   rVal = "70,80,00"; break;
                case "ActuateLeftFrontRecovery":    rVal = "70,80,00"; break;
                case "ActuateRightFrontReduction":  rVal = "70,80,00"; break;
                case "ActuateRightFrontRecovery":   rVal = "70,80,00"; break;
                case "ActuateLeftRearReduction":    rVal = "70,80,00"; break;
                case "ActuateLeftRearRecovery":     rVal = "70,80,00"; break;
                case "ActuateRightRearReduction":   rVal = "70,80,00"; break;
                case "ActuateRightRearRecovery":    rVal = "70,80,00"; break;
                case "MotorOff":                    rVal = "70,80,00"; break;
            }
            

            return rVal;
        }
        public void WriteToQNX(string type, string msg)
        {
            if (msg.Length > 1)
            {
                Log.Log("Sending to QNX: " + msg + "\n");
                if (chBx_txToQnx.Checked)
                {
                    QnxCcrt.Write(type, msg + ";");
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
                        WriteLog("Lost QnxCcrt Connection");
                        SetQnxConnectionStatus(Color.Red);
                        // Shutdown everything and attempt to reconnect to it.
                        QnxCcrt.Shutdown();
                        QnxCcrt.ConnectTCP();
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

        private bool m_disconnectedCmdStatus { get; set; }

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

        private void ShutdownAndExit()
        {
            ModInterface.ShutdownModuleInterface();


            if (Log != null)
            {
                Log.CloseLogFile();
            }

            if (QnxCcrt != null)
            {
                QnxCcrt.Shutdown();
            }

            Environment.Exit(0);

            WriteLog("Close-------------------------");
        }

        /// <summary>
        /// Exit the Application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ShutdownAndExit();
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
            //ModInterface.ChangeActiveJ2534Device();
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

        private void button1_Click(object sender, EventArgs e)
        {
            string msg = txtBox_txMsg.Text;
            Log.Log("User Tx: \"" + msg + "\"");
            ProcessQnxCcrtMessage(msg);
        }

        private void m_startThreadButton_Click(object sender, EventArgs e)
        {
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            string msg = txtBox_MsgToQnx.Text;
            WriteToQNX("ModuleResponse", msg);
        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            ModInterface.StartKeepAliveMessage();
        }
        private void WriteLog(string msg)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"debug.log", true))
            {
                file.WriteLine(DateTime.Now + "\t" + msg);
                file.Close();
            }
        }

        private void ModuleCommServerForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            ShutdownAndExit();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ModInterface.StopKeepAliveMessage();
        }

        private void chBxHideKeepAlive_CheckedChanged(object sender, EventArgs e)
        {
            if (chBxHideKeepAlive.Checked == true)
            {
                ModInterface.m_HideKeepAliveMsg = true;
            }
            else
            {
                ModInterface.m_HideKeepAliveMsg = false;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {

        }
    }
}
