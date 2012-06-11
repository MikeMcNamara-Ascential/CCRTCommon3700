using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Logger;
using J2534ChannelLibrary;
using J2534DotNet;

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
            ActiveJ2534Device = new CcrtJ2534Device(J2534Detect.GetDeviceList().Find(dev => dev.Name.Equals(devName)));
        }

        /// <summary>
        /// Connect to the J2534 device.
        /// </summary>
        public void ConnectJ2534Device()
        {
            ActiveJ2534Device.OpenJ2534Interface();
        }

        /// <summary>
        /// Send the specified message to the module.
        /// </summary>
        /// <param name="module">Module to send the message to.</param>
        /// <param name="messageTag">Message to send to the module.</param>
        /// <returns>Message received from the module.</returns>
        public List<Byte> SendModuleMessage(BrakeModule module, String messageTag)
        {
            Boolean status = false;
            List<Byte> data = new List<Byte>();
            status = ActiveJ2534Device.GetECUData("1", module.MessageTable[messageTag], ref data);
            return data;
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


        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

    }
}
