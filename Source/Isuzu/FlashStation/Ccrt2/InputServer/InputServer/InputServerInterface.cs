using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SerialPortLibrary;


namespace InputServer
{
    public class InputServerInterface : IInputServerInterface
    {
        public InputServerInterface()
        {
            m_serialDeviceCollection = new SerialDeviceCollection();
            m_availableSerialDevices = new List<string>();
            PopulateAvailableSerialDeviceList();
        }
        private SerialDeviceCollection m_serialDeviceCollection;
        public List<string> GetAvailableSerialDeviceNames()
        {
            return m_availableSerialDevices;
        }
        public void PopulateAvailableSerialDeviceList()
        {
            foreach(SerialDevice sd in m_serialDeviceCollection.SerialDeviceSettings)
            {
                m_availableSerialDevices.Add(sd.DeviceName);
            }
        }
        public PortSettings GetSerialDevicePortSettings(string deviceName)
        {
            return (m_serialDeviceCollection.Find(deviceName).ConvertToPortSettings());
        }
        public void OpenInputDeviceForm()
        {
            InputDeviceForm frm = new InputDeviceForm();
            frm.ShowDialog();
        }
        private List<string> m_availableSerialDevices;
    }

}
