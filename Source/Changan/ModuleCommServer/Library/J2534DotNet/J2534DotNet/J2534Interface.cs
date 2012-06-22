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
    public class J2534Interface : IJ2534Interface
    {
        private J2534Device m_device;
        private J2534DllWrapper m_wrapper;

        public bool LoadLibrary(J2534Device device)
        {
            m_device = device;
            m_wrapper = new J2534DllWrapper();
            m_wrapper.LoadJ2534Library(m_device.FunctionLibrary);
            return true;
        }

        public bool FreeLibrary()
        {
            return m_wrapper.FreeLibrary();
        }

        public ErrorCode Open(ref int deviceId)
        {
            int nada = 0;
            return (ErrorCode)m_wrapper.PassThruOpen(nada, ref deviceId);
        }

        public ErrorCode Close(int deviceId)
        {
            return (ErrorCode)m_wrapper.PassThruClose(deviceId);
        }

        public ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, BaudRate baudRate, ref int channelId)
        {
            return (ErrorCode)m_wrapper.PassThruConnect(deviceId, (int)protocolId, (int)flags, (int)baudRate, ref channelId);
        }

        public ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, int baudRate, ref int channelId)
        {
            return (ErrorCode)m_wrapper.PassThruConnect(deviceId, (int)protocolId, (int)flags, baudRate, ref channelId);
        }

        public ErrorCode Disconnect(int channelId)
        {
            return (ErrorCode)m_wrapper.PassThruDisconnect(channelId);
        }

        public ErrorCode ReadMsgs(int channelId, ref List<PassThruMsg> msgs, ref int numMsgs, int timeout)
        {
            ErrorCode returnValue;
            // TODO: change function to accept a list? of PassThruMsg
            IntPtr pMsg = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(UnsafePassThruMsg))*50);
            IntPtr pNextMsg = IntPtr.Zero;
            IntPtr[] pMsgs = new IntPtr[50];
            returnValue = (ErrorCode)m_wrapper.PassThruReadMsgs(channelId, pMsg, ref numMsgs, timeout);
            
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                for (int i = 0; i < numMsgs; i++)
                {
                    pNextMsg = (IntPtr)(Marshal.SizeOf(typeof(UnsafePassThruMsg))*i + (int)pMsg);
                    UnsafePassThruMsg uMsg = (UnsafePassThruMsg)Marshal.PtrToStructure(pMsg, typeof(UnsafePassThruMsg));
                    msgs.Add(ConvertPassThruMsg(uMsg));
                }
            }
            //Free allocated memory
            Marshal.FreeHGlobal(pMsg);

            return returnValue;
        }

        public ErrorCode WriteMsgs(int channelId, ref PassThruMsg msg, ref int numMsgs, int timeout)
        {
            UnsafePassThruMsg uMsg = ConvertPassThruMsg(msg);
            // TODO: change function to accept a list? of PassThruMsg
            return (ErrorCode)m_wrapper.PassThruWriteMsgs(channelId, ref uMsg, ref numMsgs, timeout);
        }

        public ErrorCode StartPeriodicMsg(int channelId, ref PassThruMsg msg, ref int msgId, int timeInterval)
        {
            UnsafePassThruMsg uMsg = ConvertPassThruMsg(msg);
            return (ErrorCode)m_wrapper.PassThruStartPeriodicMsg(channelId, ref uMsg, ref msgId, timeInterval);
        }

        public ErrorCode StopPeriodicMsg(int channelId, int msgId)
        {
            return (ErrorCode)m_wrapper.PassThruStopPeriodicMsg(channelId, msgId);
        }

        public ErrorCode StartMsgFilter
        (
            int channelid,
            FilterType filterType,
            ref PassThruMsg maskMsg,
            ref PassThruMsg patternMsg,
            ref PassThruMsg flowControlMsg,
            ref int filterId
        )
        {
            UnsafePassThruMsg uMaskMsg = ConvertPassThruMsg(maskMsg);
            UnsafePassThruMsg uPatternMsg = ConvertPassThruMsg(patternMsg);
            UnsafePassThruMsg uFlowControlMsg = ConvertPassThruMsg(flowControlMsg);
            return (ErrorCode)m_wrapper.PassThruStartMsgFilter
                    (
                        channelid, 
                        (int)filterType,
                        ref uMaskMsg,
                        ref uPatternMsg,
                        ref uFlowControlMsg, 
                        ref filterId
                    );
        }

        public ErrorCode StartMsgFilter
        (
            int channelid,
            FilterType filterType,
            ref PassThruMsg maskMsg,
            ref PassThruMsg patternMsg,
            ref int filterId
        )
        {
            int nada = 0;
            UnsafePassThruMsg uMaskMsg = ConvertPassThruMsg(maskMsg);
            UnsafePassThruMsg uPatternMsg = ConvertPassThruMsg(patternMsg);
            return (ErrorCode)m_wrapper.PassThruStartPassBlockMsgFilter
                    (
                        channelid,
                        (int)filterType,
                        ref uMaskMsg,
                        ref uPatternMsg,
                        nada,
                        ref filterId
                    );
        }

        public ErrorCode StopMsgFilter(int channelId, int filterId)
        {
            return (ErrorCode)m_wrapper.PassThruStopMsgFilter(channelId, filterId);
        }

        public ErrorCode SetProgrammingVoltage(int deviceId, PinNumber pinNumber, int voltage)
        {
            return (ErrorCode)m_wrapper.PassThruSetProgrammingVoltage(deviceId, (int)pinNumber, voltage);
        }

        public ErrorCode ReadVersion(int deviceId, ref string firmwareVersion, ref string dllVersion, ref string apiVersion)
        {
            ErrorCode returnValue;
            IntPtr pFirmwareVersion = Marshal.AllocHGlobal(120);
            IntPtr pDllVersion = Marshal.AllocHGlobal(120);
            IntPtr pApiVersion = Marshal.AllocHGlobal(120);
            returnValue = (ErrorCode)m_wrapper.PassThruReadVersion(deviceId, pFirmwareVersion, pDllVersion, pApiVersion);
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                firmwareVersion = Marshal.PtrToStringAnsi(pFirmwareVersion);
                dllVersion = Marshal.PtrToStringAnsi(pDllVersion);
                apiVersion = Marshal.PtrToStringAnsi(pApiVersion);
            }
            //Free allocated memory
            Marshal.FreeHGlobal(pFirmwareVersion);
            Marshal.FreeHGlobal(pDllVersion);
            Marshal.FreeHGlobal(pApiVersion);
            return returnValue;
        }

        public ErrorCode GetLastError(ref string errorDescription)
        {
            ErrorCode returnValue;
            IntPtr pErrorDescription = Marshal.AllocHGlobal(120);
            returnValue = (ErrorCode)m_wrapper.PassThruGetLastError(pErrorDescription);
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                errorDescription = Marshal.PtrToStringAnsi(pErrorDescription);
            }
            Marshal.FreeHGlobal(pErrorDescription);
            return returnValue;
        }

        public ErrorCode GetConfig(int channelId, ref List<SConfig> config)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.GET_CONFIG, input, output);
        }

        public ErrorCode SetConfig(int channelId, ref List<SConfig> config)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.SET_CONFIG, input, output);
        }

        public ErrorCode ReadBatteryVoltage(int deviceId, ref int voltage)
        {
            ErrorCode returnValue;
            IntPtr input = IntPtr.Zero;
            IntPtr output = Marshal.AllocHGlobal(8);

            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(deviceId, (int)Ioctl.READ_VBATT, input, output);
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                voltage = Marshal.ReadInt32(output);
            }
            Marshal.FreeHGlobal(output);
            return returnValue;
        }

        public ErrorCode FiveBaudInit(int channelId, ref byte targetAddress, ref byte keyword1, ref byte keyword2)
        {
            ErrorCode returnValue;

            SByteArray inputArray = new SByteArray();
            SByteArray outputArray = new SByteArray();
            IntPtr input = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)) + Marshal.SizeOf(typeof (byte))*4);
            IntPtr output = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)) + Marshal.SizeOf(typeof(byte))*4);
            //IntPtr input = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(SByteArray)));
            //IntPtr output = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(SByteArray)));
            unsafe
            {
                byte[] inputByteArray= new byte[]{targetAddress,0x00};
                
                byte[] outputByteArray = new byte[]{0x00,0x00};
                fixed(byte* inByte = &inputByteArray[0])//, byte* outByte = &outputByteArray[0] )
                {
                    fixed (byte* outByte = &outputByteArray[0])
                    {
                        inputArray.NumOfBytes = 1;


                        inputArray.BytePtr = inByte;
                        outputArray.NumOfBytes = 2;

                        outputArray.BytePtr = outByte;
                        Marshal.StructureToPtr(inputArray, input, true);
                        //Marshal.StructureToPtr(outputArray, output, true);

                        returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.FIVE_BAUD_INIT, input, output);
                        /*  We dont care about the key bytes really if (returnValue == ErrorCode.STATUS_NOERROR)
                        {
                            outputArray = (SByteArray)Marshal.PtrToStructure(output, typeof(SByteArray));
                            if (outputArray.BytePtr != null)
                            {
                                keyword1 = (byte)outputArray.BytePtr[0];
                                keyword2 = (byte)outputArray.BytePtr[1];
                            }
                        }*/
                    }
                }
            }

            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            return returnValue;
        }

        public ErrorCode FastInit(int channelId, ref PassThruMsg txMsg, ref PassThruMsg rxMsg)
        {
            ErrorCode returnValue;
            IntPtr input = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(UnsafePassThruMsg)));
            IntPtr output = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(UnsafePassThruMsg)));
            UnsafePassThruMsg usTxMsg = ConvertPassThruMsg(txMsg);

            Marshal.StructureToPtr(usTxMsg, input, true);

            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.FAST_INIT, input, output);
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                UnsafePassThruMsg uMsg = (UnsafePassThruMsg)Marshal.PtrToStructure(output, typeof(UnsafePassThruMsg));
                rxMsg = ConvertPassThruMsg(uMsg);
            }
            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            return returnValue;
        }

        public ErrorCode ClearTxBuffer(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.CLEAR_TX_BUFFER, input, output);
        }

        public ErrorCode ClearRxBuffer(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.CLEAR_RX_BUFFER, input, output);
        }

        public ErrorCode ClearPeriodicMsgs(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.CLEAR_PERIODIC_MSGS, input, output);
        }

        public ErrorCode ClearMsgFilters(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.CLEAR_MSG_FILTERS, input, output);
        }

        public ErrorCode ClearFunctMsgLookupTable(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;

            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.CLEAR_FUNCT_MSG_LOOKUP_TABLE, input, output);
        }

        public ErrorCode AddToFunctMsgLookupTable(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;
            // TODO: fix this
            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.ADD_TO_FUNCT_MSG_LOOKUP_TABLE, input, output);
        }

        public ErrorCode DeleteFromFunctMsgLookupTable(int channelId)
        {
            IntPtr input = IntPtr.Zero;
            IntPtr output = IntPtr.Zero;
            // TODO: fix this
            return (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.DELETE_FROM_FUNCT_MSG_LOOKUP_TABLE, input, output);
        }

        private UnsafePassThruMsg ConvertPassThruMsg(PassThruMsg msg)
        {
            UnsafePassThruMsg uMsg = new UnsafePassThruMsg();

            uMsg.ProtocolID = (int)msg.protocolID;
            uMsg.RxStatus = (int)msg.rxStatus;
            uMsg.Timestamp = msg.Timestamp;
            uMsg.TxFlags = (int)msg.txFlags;
            uMsg.ExtraDataIndex = msg.ExtraDataIndex;
            uMsg.DataSize = msg.Data.Length;
            unsafe
            {
                for (int i = 0; i < msg.Data.Length; i++)
                {
                    uMsg.Data[i] = msg.Data[i];
                }
            }

            return uMsg;
        }

        private PassThruMsg ConvertPassThruMsg(UnsafePassThruMsg uMsg)
        {
            PassThruMsg msg = new PassThruMsg();

            msg.protocolID = (ProtocolID)uMsg.ProtocolID;
            msg.rxStatus = (RxStatus)uMsg.RxStatus;
            msg.Timestamp = uMsg.Timestamp;
            msg.txFlags = (TxFlag)uMsg.TxFlags;
            msg.ExtraDataIndex = uMsg.ExtraDataIndex;
            msg.Data = new byte[uMsg.DataSize];
            unsafe
            {
                for (int i = 0; i < uMsg.DataSize; i++)
                {
                    msg.Data[i] = uMsg.Data[i];
                }
            }

            return msg;
        }
    }
}
