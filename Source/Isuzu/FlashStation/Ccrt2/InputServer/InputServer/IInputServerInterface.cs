using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SerialPortLibrary;

namespace InputServer
{
    public interface IInputServerInterface
    {
        List<string> GetAvailableSerialDeviceNames();
        PortSettings GetSerialDevicePortSettings(string deviceName);
        void OpenInputDeviceForm();
    }
}
