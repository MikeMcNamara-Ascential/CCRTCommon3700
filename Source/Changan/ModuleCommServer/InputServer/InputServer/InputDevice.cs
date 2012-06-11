using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SerialPortLibrary;

namespace InputServer
{
    public class InputDevice
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// The parameters will be saved off to member variables.  A new CCRT serial port will be
        /// created using the supplied port settings.  The serial port will then be opened.
        /// </summary>
        /// <param name="deviceName"></param>
        /// <param name="portSettings"></param>
        public InputDevice(String deviceName, PortSettings portSettings)
        {   // Save the device name
            DeviceName = deviceName;
            // Create a new serial port and save the settings
            m_serialPort = new CcrtSerialPort();
            m_serialPort.SerialPortSettings = portSettings;
            m_serialPort.OpenPort();
        }


        //-----------------------------------------------------------------------------------------
        // Class Variable/Properties
        //-----------------------------------------------------------------------------------------
        
        /// <summary>
        /// Serial port object to use for communications.
        /// </summary>
        private CcrtSerialPort m_serialPort;

        /// <summary>
        /// Name of the input device.
        /// </summary>
        private String m_deviceName;

        /// <summary>
        /// Get/Set the name of the input device.
        /// </summary>
        public String DeviceName
        {
            get { return m_deviceName; }
            set { m_deviceName = value; }
        }



        //-----------------------------------------------------------------------------------------
        // Class Callbacks
        //-----------------------------------------------------------------------------------------

    }
}
