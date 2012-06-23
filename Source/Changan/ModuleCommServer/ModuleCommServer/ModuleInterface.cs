using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Logger;
using J2534ChannelLibrary;
using J2534DotNet;
using VehicleCommServer;

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
            ChangeActiveJ2534Device();
        }

        /// <summary>
        /// Change the active J2534 device we are connected to.
        /// </summary>
        public void ChangeActiveJ2534Device()
        {
            String devName = ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice;
            List<J2534Device> test = J2534Detect.GetDeviceList();
            ActiveJ2534Device = new CcrtJ2534Device(J2534Detect.GetDeviceList().Find(dev => dev.Name.Equals(devName)));
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
        /// Connect to the J2534 device.
        /// </summary>
        public void ConnectJ2534Device()
        {   // Create a new vehicle comm server interface
            VehicleInterface = new VehicleCommServerInterface();
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

                    VehicleInterface.ClearResponseBuffer(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice,
                                                         module.CommBus);
                    VehicleInterface.GetECUData(ModuleCommServer.Properties.Settings.Default.VehicleInterfaceDevice,
                                                module.CommBus, module.MessageTable[messageTag], ref response);
                }
                catch (Exception e)
                {
                    string exceptionMsg = e.ToString();
                }
            }

            return response;
        }




        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

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
