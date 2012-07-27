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
            ErrorCode ec = (ErrorCode)m_wrapper.PassThruOpen(nada, ref deviceId);

            WriteLog("J2534Interface::Open m_wrapper.PassThruOpen: " + ec.ToString() + " deviceId: " + deviceId);
            return ec;
        }



        public ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, BaudRate baudRate, ref int channelId)
        {
            ErrorCode ec = (ErrorCode)m_wrapper.PassThruConnect(deviceId, (int)protocolId, (int)flags, (int)baudRate, ref channelId);
            WriteLog("J2534Interface::Connect:(1)" + ec.ToString() + " deviceId: " + deviceId + " protocolId: " + protocolId + " flags: " + flags + " baudRate: " + baudRate + " channelId: " + channelId);
            return ec;
        }

        public ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, int baudRate, ref int channelId)
        {
            ErrorCode ec = (ErrorCode)m_wrapper.PassThruConnect(deviceId, (int)protocolId, (int)flags, baudRate, ref channelId);

            WriteLog("J2534Interface::Connect:(2)" + ec.ToString() + " deviceId: " + deviceId + " protocolId: " + protocolId + " flags: " + flags + " baudRate: " + baudRate + " channelId: " + channelId);

            return ec;
        }

        /// <summary>
        /// Run J2534 PassThruDisconnect command (J2534Interface)
        /// </summary>
        /// <param name="channelId">channel id</param>
        /// <returns>ErrorCode</returns>
        public ErrorCode Disconnect(int channelId)
        {
            ErrorCode ec = new ErrorCode();
            ec = (ErrorCode)m_wrapper.PassThruDisconnect(channelId);
            WriteLog("J2534Interface::Disconnect channelId: " + channelId + " ec: " + ec.ToString());
            return ec;
        }

        /// <summary>
        /// Runs J2534 PassThruClose() command (J2534Interface)
        /// </summary>
        /// <param name="deviceId">device id</param>
        /// <returns>ErrorCode</returns>
        public ErrorCode Close(int deviceId)
        {
            ErrorCode ec = (ErrorCode)m_wrapper.PassThruClose(deviceId);
            WriteLog("J2534Interface::Close deviceId: " + deviceId + " ec: " + ec.ToString());
            return ec;
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
        public ErrorCode GetConfigParameter(int channelId, ConfigParameter parameterId, ref int value)
        {
            ErrorCode returnValue;
            SConfigList inputSConfigList = new SConfigList();
            inputSConfigList.NumOfParameters = 1;
            //allocate memory for pointer to SconfigList structure
            IntPtr input = Marshal.AllocHGlobal(1024);
            IntPtr output = IntPtr.Zero;
            //allocate memory for pointer to config array
            inputSConfigList.configPtr = Marshal.AllocHGlobal(1024);

            //create a sconfig structure for input
            SConfig c = new SConfig();
            c.Parameter = (int)parameterId;
            c.Value = 0;
            SConfig[] configArray = new SConfig[1]{c};

            //set pointer to SConfigList structure
            Marshal.StructureToPtr(inputSConfigList, input, true);
            //set pointer to SConfig
            Marshal.WriteInt32(inputSConfigList.configPtr, 0, configArray[0].Parameter);
            Marshal.WriteInt32(inputSConfigList.configPtr, sizeof(int), 0);

            //call wrapper function (parameter's value will be placed in structure)
            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.GET_CONFIG, input, output);
            
            //Get value from structure
            value = Marshal.ReadInt32(inputSConfigList.configPtr, sizeof(int));
            
            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            Marshal.FreeHGlobal(inputSConfigList.configPtr);

            return returnValue;
        }
        public ErrorCode SetConfigParameter(int channelId, ConfigParameter parameterId, int value)
        {
            ErrorCode returnValue;
            SConfigList inputSConfigList = new SConfigList();
            inputSConfigList.NumOfParameters = 1;
            //allocate memory for pointer to SconfigList structure
            IntPtr input = Marshal.AllocHGlobal(1024);
            IntPtr output = IntPtr.Zero;
            //allocate memory for pointer to config array
            inputSConfigList.configPtr = Marshal.AllocHGlobal(1024);

            //create a sconfig structure for input
            SConfig c = new SConfig();
            c.Parameter = (int)parameterId;
            c.Value = value;
            SConfig[] configArray = new SConfig[1] { c };

            //set pointer to SConfigList structure
            Marshal.StructureToPtr(inputSConfigList, input, true);
            //set pointer to SConfig
            Marshal.WriteInt32(inputSConfigList.configPtr, 0, configArray[0].Parameter);
            Marshal.WriteInt32(inputSConfigList.configPtr, sizeof(int), configArray[0].Value);

            //call wrapper function (parameter's value will be placed in structure)
            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.SET_CONFIG, input, output);

            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            Marshal.FreeHGlobal(inputSConfigList.configPtr);

            return returnValue;
        }
        public ErrorCode GetConfig(int channelId, ref SConfig[] configArray)
        {
            ErrorCode returnValue;
            SConfigList inputSConfigList = new SConfigList();

            //allocate memory for pointer to SconfigList structure
            IntPtr input = Marshal.AllocHGlobal(1024);
            IntPtr output = IntPtr.Zero;

            //allocate memory for pointer to list of configs
            inputSConfigList.configPtr = Marshal.AllocHGlobal(1024);
            inputSConfigList.NumOfParameters = configArray.Length;

            //set pointer to input configs
            Marshal.StructureToPtr(inputSConfigList, input, true);

            //set pointer to config list
            for (int i = 0; i < configArray.Length; i++)
            {
                int Base = i * sizeof(int) * 2;
                Marshal.WriteInt32(inputSConfigList.configPtr, Base + 0, configArray[i].Parameter);
                Marshal.WriteInt32(inputSConfigList.configPtr, Base + sizeof(int), 0x00);
            }
            

            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.GET_CONFIG, input, output);

            //set pointer to config list
            for (int i = 0; i < configArray.Length; i++)
            {
                int Base = i * sizeof(int) * 2;
                int param= Marshal.ReadInt32(inputSConfigList.configPtr, Base);
                int val= Marshal.ReadInt32(inputSConfigList.configPtr, Base + sizeof(int));
                configArray[i].Parameter = param;
                configArray[i].Value = val; 
            }
            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            Marshal.FreeHGlobal(inputSConfigList.configPtr);
            return returnValue;        
        }

        public ErrorCode SetConfig(int channelId, ref SConfig[] configArray)
        {
            ErrorCode returnValue;
            SConfigList inputSConfigList = new SConfigList();

            //allocate memory for pointer to SconfigList structure
            IntPtr input = Marshal.AllocHGlobal(1024);
            IntPtr output = IntPtr.Zero;

            //allocate memory for pointer to list of configs
            inputSConfigList.configPtr = Marshal.AllocHGlobal(1024);
            inputSConfigList.NumOfParameters = configArray.Length;

            //set pointer to input configs
            Marshal.StructureToPtr(inputSConfigList, input, true);

            //set pointer to config list
            for (int i = 0; i < configArray.Length; i++)
            {
                int Base = i * sizeof(int) * 2;
                Marshal.WriteInt32(inputSConfigList.configPtr, Base + 0, configArray[i].Parameter);
                Marshal.WriteInt32(inputSConfigList.configPtr, Base + sizeof(int), configArray[i].Value);
            }

            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.SET_CONFIG, input, output);

            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            Marshal.FreeHGlobal(inputSConfigList.configPtr);
            return returnValue;
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

        /*public ErrorCode FiveBaudInit(int channelId, ref byte targetAddress, ref byte keyword1, ref byte keyword2)
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
                    }
                }
            }

            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            return returnValue;
        }*/

        public ErrorCode FiveBaudInit(int channelId, ref byte targetAddress, ref byte keyword1, ref byte keyword2)
        {
            ErrorCode returnValue;

            SByteArray inputSByteArray = new SByteArray();
            SByteArray outputSByteArray = new SByteArray();

            //pointers to SByteArray structure - allocate memory
            IntPtr input = Marshal.AllocHGlobal(1024);
            IntPtr output = Marshal.AllocHGlobal(1024);
            
            //create input and output byte arrays
            int[] inputBytes = new int[] { targetAddress };
            inputSByteArray.NumOfBytes = 1;//must be one
            int[] outputBytes = new int[] { 0x00, 0x00 };
            outputSByteArray.NumOfBytes = 2;



            //pointers to input and output data - allocate memory
            inputSByteArray.BytePtr = Marshal.AllocHGlobal(1024);
            outputSByteArray.BytePtr = Marshal.AllocHGlobal(1024);

            //set pointers to sbyte array structure
            Marshal.StructureToPtr(inputSByteArray, input, true);
            Marshal.StructureToPtr(outputSByteArray, output, true);
            
            //set pointers to input and output data
            Marshal.WriteInt32(inputSByteArray.BytePtr, 0, inputBytes[0]);
            //Marshal.WriteInt32(outputSByteArray.BytePtr, 0, outputBytes[0]);
            //Marshal.WriteInt32(outputSByteArray.BytePtr, sizeof(int), outputBytes[1]);



            returnValue = (ErrorCode)m_wrapper.PassThruIoctl(channelId, (int)Ioctl.FIVE_BAUD_INIT, input, output);

            //Get keyword values from structure
            if (returnValue == ErrorCode.STATUS_NOERROR)
            {
                keyword1 = Marshal.ReadByte(outputSByteArray.BytePtr, 0);
                keyword2 = Marshal.ReadByte(outputSByteArray.BytePtr, sizeof(byte));
            }

            //Free allocated memory
            Marshal.FreeHGlobal(input);
            Marshal.FreeHGlobal(output);
            Marshal.FreeHGlobal(inputSByteArray.BytePtr);
            Marshal.FreeHGlobal(outputSByteArray.BytePtr);
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
        private void WriteLog(string msg)
        {
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"debug.log", true))
            {
                file.WriteLine(DateTime.Now + "\t" + msg);
                file.Close();
            }
        }
    }
}
