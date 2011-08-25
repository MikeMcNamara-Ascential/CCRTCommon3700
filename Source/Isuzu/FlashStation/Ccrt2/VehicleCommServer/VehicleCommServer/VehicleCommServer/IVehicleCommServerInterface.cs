using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534ChannelLibrary;
using J2534DotNet;

namespace VehicleCommServer
{
    public interface IVehicleCommServerInterface
    {
        void PopulateVehicleCommChannels();
        CcrtJ2534Device GetCcrtJ2534Device(string deviceName);
        bool ProcessMessageCAN(string deviceName, string commChannelName,
    J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message, ref List<byte> data);
        bool GetECUData(string deviceName, string commChannelName, 
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message, ref List<byte> data);
        bool GetECUData(string deviceName, string commChannelName,
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message, ref List<List<byte>> data, bool globalRequest);
        bool AddMessageFilter(string deviceName, string commChannelName, CcrtJ2534Defs.MessageFilter filter);
        List<string> GetCommChannelNames();
        List<string> GetCommChannelNames(string deviceName);
        List<string> GetJ2534DeviceNames();
        List<J2534Device> GetJ2534Devices();
        bool StopPeriodicMessage(string deviceName, string commChannelName, int msgID);
        bool StartPeriodicMessage(string deviceName, string commChannelName,
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message, ref int msgID);
        bool IsVehicleConnected(string deviceName, string commChannelName);
        void OpenVehicleCommSettingsForm();
        bool SetSTMin(string deviceName, string commChannelName, int stMinValue);
        void ClearResponseBuffer(string deviceName, string commChannelName);
        VehicleCommDeviceCollection GetVehicleCommDeviceCollection();
        List<CcrtJ2534Defs.Response> GetResponseBuffer(string deviceName, string commChannelName);
        List<CcrtJ2534Defs.Response> GetRemovedResponsesBuffer(string deviceName, string commChannelName);
    }
}
