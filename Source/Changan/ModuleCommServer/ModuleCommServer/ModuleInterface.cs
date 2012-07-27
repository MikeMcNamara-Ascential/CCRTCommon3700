using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Logger;
using J2534ChannelLibrary;
using J2534DotNet;
using VehicleCommServer;
using System.Threading;

namespace ModuleCommServer
{
    public class ModuleInterface
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Create a new module interface class.
        /// </summary>
        public ModuleInterface(CcrtLogger logger)
        {
            Log = logger;
            //ChangeActiveJ2534Device();
            m_threadID = 0;
            m_HideKeepAliveMsg = false;
            m_J2534DeviceName = "DPA5";
        }

        /// <summary>
        /// Change the active J2534 device we are connected to.
        /// </summary>
        public void ChangeActiveJ2534Device()
        {
            String devName = ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice;
            if (ActiveJ2534Device == null || ActiveJ2534Device.DeviceName != devName)
            {
                ActiveJ2534Device = new CcrtJ2534Device(J2534Detect.GetDeviceList().Find(dev => dev.Name.Equals(devName)));
                m_J2534DeviceName = ActiveJ2534Device.DeviceName;
            }
            else
            {
                Log.Log("Will use the active J2534 device.");
            }
        }

        /// <summary>
        /// Setup the vehicle communications.
        /// </summary>
        /// <returns>List of configured channels.</returns>
        public List<String> ConfigureVehicleComms()
        {
            if (VehicleInterface == null)
            {
                VehicleInterface = new VehicleCommServerInterface();
            }
            VehicleInterface.OpenVehicleCommSettingsForm();
            return VehicleInterface.GetCommChannelNames();
        }

        /// <summary>
        /// Connect to the J2534 device. Creates the VehicleCommServerInterface object.
        /// </summary>
        public void ConnectJ2534Device()
        {   // Create a new vehicle comm server interface
            if (VehicleInterface == null)
            {

                WriteLog("Creating VehicleInterface");


                VehicleInterface = new VehicleCommServerInterface();
            }
            else
            {

                WriteLog("VehicleInterface already created.");

                Log.Log("ConnectJ2534Device - VehicleInterface already created");
            }
        }

        public bool InitilizeVehicleModuleInterface(string initType, BrakeModule module)
        {
            bool rVal = false;

            m_VehicleChannelName = module.CommBus;
            // add message filter before running
            bool status = false;

            VehicleInterface.SetDeviceConfigurationParameter(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice, module.CommBus, J2534DotNet.ConfigParameter.BIT_SAMPLE_POINT, 0);


            WriteLog("adding message filter");

            status = VehicleInterface.AddMessageFilter(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice, module.CommBus, module.MessageTable["EnterDiagnosticMode"].m_messageFilter);

            if (status)
            {
                switch (initType)
                {
                    case "Fast":
                        try
                        {
                            List<byte> initRequest = module.MessageTable["FastInit"].m_txMessage;
                            List<byte> initResponse = new List<byte>();
                            Log.Log("FastInit Tx: [" + BitConverter.ToString(initRequest.ToArray()) + "]");

                            rVal = VehicleInterface.PerformFastInit(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice, module.CommBus, ref initRequest, ref initResponse);
                            if (rVal && initResponse != null)
                                Log.Log("FastInit Rx: [" + BitConverter.ToString(initResponse.ToArray()) + "]");
                            else
                                Log.Log("FastIni: Problem with init function.");
                        }
                        catch (Exception e)
                        {
                            Log.Log("Could not filnd \"FastInit\" message node in module config file. " + e.ToString());
                        }
                        Log.Log("Result of fast init: " + rVal);
                        break;

                    case "FiveBaud":

                        List<byte> fiveBaudInit = new List<byte>();
                        try
                        {
                            fiveBaudInit = module.MessageTable["FiveBaudInit"].m_txMessage;

                        }
                        catch (Exception e)
                        {
                            Log.Log("Error looking up tag: FiveBaudInit." + e.ToString() + "\nConfig file: " + module.ConfigFile);
                        }

                        if (fiveBaudInit.Count <= 1)
                        {
                            try
                            {
                                Log.Log("Performing Five baud init");

                                WriteLog("ModuleInterface: m_selectedDevice: " + ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice + " module.CommBus: " + module.CommBus + "msg: " + fiveBaudInit.First().ToString());


                                rVal = VehicleInterface.PerformFiveBaudInit(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice, module.CommBus, fiveBaudInit.First());
                            }
                            catch (Exception e)
                            {
                                Log.Log("Could not find \"FiveBaudInit\" message node in module config file. " + e.ToString());
                            }
                        }
                        break;

                    default:
                        Log.Log("Could not find vehicle init method");
                        break;
                }
            }
            else
            {
                Log.Log("Failed to add message filter");
            }

            return rVal;

        }

        /// <summary>
        /// Get the currently configured comm channels.
        /// </summary>
        /// <returns>Currently configured comm channels.</returns>
        public List<String> GetCommChannels()
        {
            if (VehicleInterface == null)
            {
                VehicleInterface = new VehicleCommServerInterface();
            }
            return VehicleInterface.GetCommChannelNames();
        }

        /// <summary>
        /// Send the specified message to the module.
        /// </summary>
        /// <param name="module">Module to send the message to.</param>
        /// <param name="messageTag">Message to send to the module.</param>
        /// <returns>Message received from the module.</returns>
        public List<Byte> SendModuleMessage(BrakeModule module, String messageTag)
        {
            
            List<Byte> response = new List<Byte>();
            Log.Log("Sending Message: " + messageTag);
            if (VehicleInterface != null)
            {
                try
                {
                    Log.Log("\t Tx(" + m_msgId + ": [" + BitConverter.ToString(module.MessageTable[messageTag].m_txMessage.ToArray()) + "]\n");

                    VehicleInterface.AddMessageFilter(GetVehicleCommDeviceName(), GetVehicleChannelName(), module.MessageTable[messageTag].m_messageFilter);
                    VehicleInterface.ClearResponseBuffer(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice,
                                                             module.CommBus);
                    VehicleInterface.GetECUData(GetVehicleCommDeviceName(),
                                                    GetVehicleChannelName(), module.MessageTable[messageTag], ref response);
                }
                catch (Exception e)
                {
                    Log.Log("ERROR (ModuleInterface:SendModuleMessage): " + e.ToString());
                }
            }
            else
            {
                Log.Log("VehicleInterface is null");
            }

            Log.Log("\t Rx(" + m_msgId + ": [" + BitConverter.ToString(response.ToArray()) + "]");

            return response;
        }
        public bool ShutdownModuleInterface()
        {
            bool rVal = false;
            TerminateKeepAliveMessage();

            if (ActiveJ2534Device != null && m_ActiveBrakeModule != null)
            {
                WriteLog("Disconnecting from devices");
                VehicleInterface.CloseVehicleCommServer(GetVehicleCommDeviceName(), GetVehicleChannelName());
                
                rVal = true;
            }
            else
            {
                Log.Log("No vehicle interface to close");
                Log.Log("ActiveJ2534Device is Null? " + ((ActiveJ2534Device == null) ? "yes" : "no") + "m_ActiveBrakeModule is null? " + ((m_ActiveBrakeModule == null) ? "yes" : "no"));
            }

            return rVal;

        }
        public void InitiateKeepAliveMessage()
        {
            m_stopMessageThreads = false;
            ThreadStart taskDelegate = new ThreadStart(ModuleKeepAliveThread);
            m_KeepAliveTaskThread = new Thread(taskDelegate);
            m_KeepAliveTaskThread.IsBackground = true;
            m_KeepAliveTaskThread.Start();
        }
        public void TerminateKeepAliveMessage()
        {
            m_threadID--;
            m_stopMessageThreads = true;
            if (m_KeepAliveTaskThread != null)
                m_KeepAliveTaskThread.Abort();
            Log.Log("Stopped Keep Alive Messages");
        }

        public void StartKeepAliveMessage()
        {
            m_keepAliveMessageThread = true;
            Log.Log("Started thread: " + m_threadID);
        }
        public void StopKeepAliveMessage()
        {
            m_keepAliveMessageThread = false;
            Log.Log("Suspended thread: " + m_threadID);
        }

        private void ModuleKeepAliveThread()
        {
            Log.Log("ModuleKeepAliveThread - Enter");
            int threadId = m_threadID++;
            bool status = false;
            string channel = GetVehicleChannelName();
            string device = GetVehicleCommDeviceName();

            status = VehicleInterface.AddMessageFilter(device, channel,
                                       m_ActiveBrakeModule.MessageTable["ModuleKeepAlive"].m_messageFilter);

            
            Log.Log(threadId.ToString() + "Message Filter " + (status ? "Added" : "Not Added"));

            List<byte> response = new List<byte>();

            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = new J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage();
            List<byte> txMessage = new List<byte>();
            txMessage.Add(0x81);
            txMessage.Add(0x28);
            txMessage.Add(0xF1);
            txMessage.Add(0x3E);

            message.m_txMessage = txMessage;
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage messageb = m_ActiveBrakeModule.MessageTable["ModuleKeepAlive"]; 
            // hack to control execution of thread
            while (!m_stopMessageThreads)
            {
                while (m_keepAliveMessageThread)
                {
                    if (!m_HideKeepAliveMsg)
                    {
                        Log.Log(threadId.ToString() + " shoot keep alive m_stopMessageThreads:" + m_stopMessageThreads);
                    }
                    VehicleInterface.ClearResponseBuffer(device, channel);
                    status = VehicleInterface.GetECUData(device, channel, message, ref response);

                    if (!m_HideKeepAliveMsg)
                    {
                        Log.Log(threadId.ToString() + " Sending keep alive: " + BitConverter.ToString(m_ActiveBrakeModule.MessageTable["ModuleKeepAlive"].m_txMessage.ToArray()));
                    }

                    if (status)
                    {
                        if (!m_HideKeepAliveMsg)
                        {
                            Log.Log(threadId.ToString() + " Message Successful");
                            Log.Log(threadId.ToString() + " Received: [" + BitConverter.ToString(response.ToArray()) + "]");
                        }
                    }
                    else
                    {
                        if (!m_HideKeepAliveMsg)
                        {
                            Log.Log(threadId.ToString() + " Message Failure");
                        }
                    }

                    Log.Log("Keep Alive pulse");
                    // sleep for the specified time
                    Thread.Sleep(m_ActiveBrakeModule.KeepAliveInterval);
                }
                Thread.Sleep(800);
            }

            Log.Log("ModuleKeepAliveThread - Exit");

        }

        public void SetActiveBrakeModule(BrakeModule module)
        {
            m_ActiveBrakeModule = module;
        }
        public BrakeModule GetActiveBrakeModule()
        {
            return m_ActiveBrakeModule;
        }
        public string GetVehicleChannelName()
        {
            string rVal = "";
            if (m_VehicleChannelName != null)
                rVal = m_VehicleChannelName;
            return rVal;
        }

        public string GetVehicleCommDeviceName()
        {
            string rVal = "";
            if (m_J2534DeviceName != null)
                rVal = m_J2534DeviceName;
            return rVal;
        }

        private void WriteLog(string msg)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"debug.log", true))
            {
                file.WriteLine(DateTime.Now + "\t" + msg);
                file.Close();
            }
        }

        public bool IsVehicleInterfaceConnected()
        {
            bool rVal = false;
            if (VehicleInterface != null)
            {

                rVal = VehicleInterface.IsVehicleConnected(GetVehicleCommDeviceName(), GetVehicleChannelName());
            }
            else
            {
                Log.Log("VehicleInterface is Null");
            }

            Log.Log("IsVehicleInterfaceConnected: " + rVal);
            return rVal;
        }

        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        private Thread m_KeepAliveTaskThread { get; set; }
        public bool m_HideKeepAliveMsg { get; set; }
        private int m_msgId { get; set; }

        private BrakeModule m_ActiveBrakeModule { get; set; }

        private bool m_stopMessageThreads { get; set; }
        private bool m_keepAliveMessageThread { get; set; }
        private int m_threadID;

        /// <summary>
        /// The current channel that the J2534 device is using
        /// </summary>
        private string m_VehicleChannelName { get; set; }

        /// <summary>
        /// The current name of the J2534 device
        /// </summary>
        private string m_J2534DeviceName { get; set; }

        /// <summary>
        /// List of avaiable J2534 devices.
        /// </summary>
        public CcrtJ2534Device ActiveJ2534Device { get; private set; }

        /// <summary>
        /// Logger object to use for logging data and events.
        /// </summary>
        private CcrtLogger Log { get; set; }

        /// <summary>
        /// Interface to the vehicle comm server object.
        /// </summary>
        private VehicleCommServerInterface VehicleInterface { get; set; }


        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

    }
}
