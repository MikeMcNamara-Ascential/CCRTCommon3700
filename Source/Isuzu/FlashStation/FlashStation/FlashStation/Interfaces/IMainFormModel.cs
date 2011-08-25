using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using LoggingInterface;
using Common.Classes;
using System.Drawing;

namespace Common.Lib.Interfaces
{
    public interface IMainFormModel : IBaseInterface
    {

        void CommandState(StateName state);

        Status GetStatus();
        string GetPrompt1();
        string GetPrompt2();
        Color GetPrompt1BGColor();
        void SetPrompt1BGColor(Color color);
        Color GetPrompt2BGColor();
        void SetPrompt2BGColor(Color color);
        Color GetECMResultBGColor();
        Color GetTCMResultBGColor();
        int GetProgressBarValue();
        int GetProgressBarRangeMin();
        int GetProgressBarRangeMax();
        List<ECUBuildData> GetBuildData();
        void SetPrompt1(string prompt);
        void SetPrompt2(string prompt);
        string GetBarcode();
        List<string> GetAvailableJ2534Devices();
        List<string> GetAvailableBarcodeInputDevices();
        List<string> GetAvailableChannels(string deviceName);
        string GetDeviceName();
        string GetBarcodeInputDeviceName();
        string GetChannelName();
        void SetDeviceName(string deviceName);
        void SetBarcodeInputDevice(string deviceName);
        void SetChannelName(string channelName);
        void SetBarcode(string barcode);
        void OpenVehicleCommSettings();
        void OpenSerialDeviceCommSettings();
        void UpdateAvailableBarcodeInputDevices();
        bool IsCableConnected();
        bool ValidBuildDataReceived();
        void Terminate();
        void SetStatus(Status status);
        void SetLogger(Logger logger);
        void ReportData();
        void ToggleForcedCableConnect();
        void ToggleUsingForcedCableConnect();
        void SetPerformTCMFlash(bool perform);
        void SetPerformECMFlash(bool perform);
        void SetDisplayDisconnectBatteryBox(bool display);
        bool GetDisplayDisconnectBatteryBox();
    }
}
