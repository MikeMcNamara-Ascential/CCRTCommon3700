#region Copyright (c) 2010, Michael Kelly
/* 
 * Copyright (c) 2010, Michael Kelly
 * 
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#endregion License
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace J2534DotNet
{
    public unsafe struct UnsafePassThruMsg
    {
        public int ProtocolID;
        public int RxStatus;
        public int TxFlags;
        public int Timestamp;
        public int DataSize;
        public int ExtraDataIndex;
        public fixed byte Data[4128];
    }

    static class NativeMethods
    {
        [DllImport("kernel32.dll")]
        public static extern IntPtr LoadLibrary(string dllToLoad);

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

        [DllImport("kernel32.dll")]
        public static extern bool FreeLibrary(IntPtr hModule);
    }
    
    public class J2534DllWrapper
    {
        private IntPtr m_pDll;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruOpen(int nada, ref int deviceId);
        public passThruOpen PassThruOpen;
        //extern "C" long WINAPI PassThruOpen
        //(
        //    void *pName
        //    unsigned long *pDeviceID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruClose(int deviceId);
        public passThruClose PassThruClose;
        //extern "C" long WINAPI PassThruClose
        //(
        //unsigned long DeviceID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruConnect(int deviceId, int protocolId, int flags, int baudRate, ref int channelId);
        public passThruConnect PassThruConnect;
        //extern “C” long WINAPI PassThruConnect
        //(
        //unsigned long DeviceID,
        //unsigned long ProtocolID,
        //unsigned long Flags,
        //unsigned long BaudRate,
        //unsigned long *pChannelID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruDisconnect(int channelId);
        public passThruDisconnect PassThruDisconnect;
        //extern “C” long WINAPI PassThruDisconnect
        //(
        //unsigned long ChannelID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruReadMsgs(int channelId, IntPtr pMsgs, ref int numMsgs, int timeout);
        public passThruReadMsgs PassThruReadMsgs;
        //extern “C” long WINAPI PassThruReadMsgs
        //(
        //unsigned long ChannelID,
        //PASSTHRU_MSG *pMsg,
        //unsigned long *pNumMsgs,
        //unsigned long Timeout
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruWriteMsgs(int channelId, ref UnsafePassThruMsg msg, ref int numMsgs, int timeout);
        public passThruWriteMsgs PassThruWriteMsgs;
        //extern “C” long WINAPI PassThruWriteMsgs
        //(
        //unsigned long ChannelID,
        //PASSTHRU_MSG *pMsg,
        //unsigned long *pNumMsgs,
        //unsigned long Timeout
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruStartPeriodicMsg(int channelId, ref UnsafePassThruMsg msg, ref int msgId, int timeInterval);
        public passThruStartPeriodicMsg PassThruStartPeriodicMsg;
        //extern “C” long WINAPI PassThruStartPeriodicMsg
        //(
        //unsigned long ChannelID,
        //PASSTHRU_MSG *pMsg,
        //unsigned long *pMsgID,
        //unsigned long TimeInterval
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruStopPeriodicMsg(int channelId, int msgId);
        public passThruStopPeriodicMsg PassThruStopPeriodicMsg;
        //extern “C” long WINAPI PassThruStopPeriodicMsg
        //(
        //unsigned long ChannelID,
        //unsigned long MsgID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruStartMsgFilter
        (
            int channelid, 
            int filterType, 
            ref UnsafePassThruMsg maskMsg, 
            ref UnsafePassThruMsg patternMsg, 
            ref UnsafePassThruMsg flowControlMsg, 
            ref int filterId
        );
        public passThruStartMsgFilter PassThruStartMsgFilter;
        //extern “C” long WINAPI PassThruStartMsgFilter
        //(
        //unsigned long ChannelID,
        //unsigned long FilterType,
        //PASSTHRU_MSG *pMaskMsg,
        //PASSTHRU_MSG *pPatternMsg,
        //PASSTHRU_MSG *pFlowControlMsg,
        //unsigned long *pFilterID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruStartPassBlockMsgFilter
        (
            int channelid,
            int filterType,
            ref UnsafePassThruMsg uMaskMsg,
            ref UnsafePassThruMsg uPatternMsg,
            int nada,
            ref int filterId
        );
        public passThruStartPassBlockMsgFilter PassThruStartPassBlockMsgFilter;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruStopMsgFilter(int channelId, int filterId);
        public passThruStopMsgFilter PassThruStopMsgFilter;
        //extern “C” long WINAPI PassThruStopMsgFilter
        //(
        //unsigned long ChannelID,
        //unsigned long FilterID
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruSetProgrammingVoltage(int deviceId, int pinNumber, int voltage);
        public passThruSetProgrammingVoltage PassThruSetProgrammingVoltage;
        //extern “C” long WINAPI PassThruSetProgrammingVoltage
        //(
        //unsigned long DeviceID,
        //unsigned long PinNumber,
        //unsigned long Voltage
        //)        

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruReadVersion(int deviceId, IntPtr firmwareVersion, IntPtr dllVersion, IntPtr apiVersion);
        public passThruReadVersion PassThruReadVersion;
        //extern “C” long WINAPI PassThruReadVersion
        //(
        //unsigned long DeviceID
        //char *pFirmwareVersion,
        //char *pDllVersion,
        //char *pApiVersion
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruGetLastError(IntPtr errorDescription);
        public passThruGetLastError PassThruGetLastError;
        //extern “C” long WINAPI PassThruGetLastError
        //(
        //char *pErrorDescription
        //)

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int passThruIoctl(int channelId, int IoctlID, IntPtr input, IntPtr output);
        public passThruIoctl PassThruIoctl;
        //extern “C” long WINAPI PassThruIoctl
        //(
        //unsigned long ChannelID,
        //unsigned long IoctlID,
        //void *pInput,
        //void *pOutput
        //)

        public void LoadJ2534Library(string path)
        {
            m_pDll = NativeMethods.LoadLibrary(path);
            IntPtr pAddressOfFunctionToCall = new IntPtr();

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruOpen");
            PassThruOpen = (passThruOpen)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruOpen));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruClose");
            PassThruClose = (passThruClose)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruClose));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruConnect");
            PassThruConnect = (passThruConnect)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruConnect));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruDisconnect");
            PassThruDisconnect = (passThruDisconnect)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruDisconnect));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruReadMsgs");
            PassThruReadMsgs = (passThruReadMsgs)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruReadMsgs));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruWriteMsgs");
            PassThruWriteMsgs = (passThruWriteMsgs)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruWriteMsgs));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruStartPeriodicMsg");
            PassThruStartPeriodicMsg = (passThruStartPeriodicMsg)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruStartPeriodicMsg));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruStopPeriodicMsg");
            PassThruStopPeriodicMsg = (passThruStopPeriodicMsg)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruStopPeriodicMsg));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruStartMsgFilter");
            PassThruStartMsgFilter = (passThruStartMsgFilter)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruStartMsgFilter));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruStartMsgFilter");
            PassThruStartPassBlockMsgFilter = (passThruStartPassBlockMsgFilter)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruStartPassBlockMsgFilter));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruStopMsgFilter");
            PassThruStopMsgFilter = (passThruStopMsgFilter)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruStopMsgFilter));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruSetProgrammingVoltage");
            PassThruSetProgrammingVoltage = (passThruSetProgrammingVoltage)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruSetProgrammingVoltage));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruReadVersion");
            PassThruReadVersion = (passThruReadVersion)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruReadVersion));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruGetLastError");
            PassThruGetLastError = (passThruGetLastError)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruGetLastError));

            pAddressOfFunctionToCall = NativeMethods.GetProcAddress(m_pDll, "PassThruIoctl");
            PassThruIoctl = (passThruIoctl)Marshal.GetDelegateForFunctionPointer(
                                                                                    pAddressOfFunctionToCall,
                                                                                    typeof(passThruIoctl));
        }

        public bool FreeLibrary()
        {
            return NativeMethods.FreeLibrary(m_pDll);
        }
    }
}
