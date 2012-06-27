using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace J2534ChannelLibrary
{
    public interface ICcrtJ2534ChannelComm
    {
        bool Connect();
        bool AddFilter(CcrtJ2534Defs.MessageFilter filter);
        bool StartFilter(CcrtJ2534Defs.MessageFilter filter);
        bool Disconnect();
        bool GetECUData(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData);
        bool GetECUData(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<List<byte>> ecuData, bool globalRequest);
        bool ProcessMessageCAN(CcrtJ2534Defs.ECUMessage ecuMessage, ref List<byte> ecuData);
        bool SendMessage(CcrtJ2534Defs.ECUMessage ecuMessage);
        bool GetResponse(CcrtJ2534Defs.ECUMessage ecuMessage,ref List<CcrtJ2534Defs.Response> responses);
        bool StartPeriodicMessage(CcrtJ2534Defs.ECUMessage ecuMessage, ref int msgID);
        bool StopPeriodicMessage(ref int msgID);
        bool IsVehicleConnected();
        bool SetSTMIN(int stMinValue);
        bool SetDeviceConfigurationParameter(ConfigParameter param, int value);
        bool GetDeviceConfigurationParameter(ConfigParameter param, ref int value);
        bool SetDeviceConfiguration(ref SConfig[] config);
        bool GetDeviceConfiguration(ref SConfig[] config);
        void ClearResponseBuffer();
        bool PerformFiveBaudInit(byte address);
        bool PerformFastInit(ref List<byte> wakeUpMessage, ref List<byte> ecuData);
        List<CcrtJ2534Defs.Response> GetResponseBuffer();
        List<CcrtJ2534Defs.Response> GetRemovedResponsesBuffer();
    }
}
