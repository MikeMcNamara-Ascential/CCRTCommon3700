using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Lib.Interfaces
{
    public interface IMainFormView : IBaseInterface
    {
        void SetPrompt1Text(string text);
        void SetPrompt2Text(string text);
        void SetPrompt1BGColor(System.Drawing.Color color);
        void SetPrompt2BGColor(System.Drawing.Color color);
        void SetECMResultBoxBGColor(System.Drawing.Color color);
        void SetTCMResultBoxBGColor(System.Drawing.Color color);
        void CableConnected(bool connected);
        void BuildDataValid(bool valid);
        void SetBarcodeText(string text);
        void ClearBuildData();
        void ClearLogDisplay();
        void DisplayBuildData(string barcode);
        void StartProgressBar();
        void StopProgressBar();
        void SetProgressBarValue(int value);
        void SetProgressBarRange(int min, int max);
        void DisplayTimedDialogBox(string dialogText, string labelText, string buttonText, int timeout);
    }
}
