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
    public interface IJ2534Interface
    {
        bool LoadLibrary(J2534Device device);
        bool FreeLibrary();
        ErrorCode Open(ref int deviceId);
        ErrorCode Close(int deviceId);
        ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, BaudRate baudRate, ref int channelId);
        ErrorCode Connect(int deviceId, ProtocolID protocolId, ConnectFlag flags, int baudRate, ref int channelId);
        ErrorCode Disconnect(int channelId);
        ErrorCode ReadMsgs(int channelId, ref List<PassThruMsg> msgs, ref int numMsgs, int timeout);
        ErrorCode WriteMsgs(int channelId, ref PassThruMsg msg, ref int numMsgs, int timeout);
        ErrorCode StartPeriodicMsg(int channelId, ref PassThruMsg msg, ref int msgId, int timeInterval);
        ErrorCode StopPeriodicMsg(int channelId, int msgId);
        ErrorCode StartMsgFilter
        (
            int channelid,
            FilterType filterType,
            ref PassThruMsg maskMsg,
            ref PassThruMsg patternMsg,
            ref PassThruMsg flowControlMsg,
            ref int filterId
        );
        ErrorCode StartMsgFilter
        (
            int channelid,
            FilterType filterType,
            ref PassThruMsg maskMsg,
            ref PassThruMsg patternMsg,
            ref int filterId
        );
        ErrorCode StopMsgFilter(int channelId, int filterId);
        ErrorCode SetProgrammingVoltage(int deviceId, PinNumber pinNumber, int voltage);
        ErrorCode ReadVersion(int deviceId, ref string firmwareVersion, ref string dllVersion, ref string apiVersion);
        ErrorCode GetLastError(ref string errorDescription);
        ErrorCode GetConfig(int channelId, ref SConfig[] config);
        ErrorCode GetConfigParameter(int channelId, ConfigParameter parameter, ref int value);
        ErrorCode SetConfigParameter(int channelId, ConfigParameter parameterId, int value);
        ErrorCode SetConfig(int channelId, ref SConfig[] config);
        ErrorCode ReadBatteryVoltage(int deviceId, ref int voltage);
        ErrorCode FiveBaudInit(int channelId, ref byte targetAddress, ref byte keyword1, ref byte keyword2);
        ErrorCode FastInit(int channelId, ref PassThruMsg txMsg, ref PassThruMsg rxMsg);
        ErrorCode ClearTxBuffer(int channelId);
        ErrorCode ClearRxBuffer(int channelId);
        ErrorCode ClearPeriodicMsgs(int channelId);
        ErrorCode ClearMsgFilters(int channelId);
        ErrorCode ClearFunctMsgLookupTable(int channelId);
        ErrorCode AddToFunctMsgLookupTable(int channelId);
        ErrorCode DeleteFromFunctMsgLookupTable(int channelId);
    }
}
