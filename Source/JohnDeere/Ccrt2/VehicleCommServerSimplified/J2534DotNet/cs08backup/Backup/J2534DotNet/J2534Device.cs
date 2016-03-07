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

namespace J2534DotNet
{
    public class J2534Device
    {
        private string m_vendor;
        public string Vendor
        {
            get { return m_vendor; }
            set { m_vendor = value; }
        }

        private string m_name;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; }
        }

        private string m_functionLibrary;
        public string FunctionLibrary
        {
            get { return m_functionLibrary; }
            set { m_functionLibrary = value; }
        }

        private string m_configApplication;
        public string ConfigApplication
        {
            get { return m_configApplication; }
            set { m_configApplication = value; }
        }

        private int m_canChannels;
        public int CANChannels
        {
            get { return m_canChannels; }
            set { m_canChannels = value; }
        }

        public bool IsCANSupported
        {
            get { return (m_canChannels > 0 ? true : false); }
        }

        private int m_iso15765Channels;
        public int ISO15765Channels
        {
            get { return m_iso15765Channels; }
            set { m_iso15765Channels = value; }
        }

        public bool IsISO15765Supported
        {
            get { return (m_iso15765Channels > 0 ? true : false); }
        }

        private int m_j1850pwmChannels;
        public int J1850PWMChannels
        {
            get { return m_j1850pwmChannels; }
            set { m_j1850pwmChannels = value; }
        }

        public bool IsJ1850PWMSupported
        {
            get { return (m_j1850pwmChannels > 0 ? true : false); }
        }

        private int m_j1850vpwChannels;
        public int J1850VPWChannels
        {
            get { return m_j1850vpwChannels; }
            set { m_j1850vpwChannels = value; }
        }

        public bool IsJ1850VPWSupported
        {
            get { return (m_j1850vpwChannels > 0 ? true : false); }
        }

        private int m_iso9141Channels;
        public int ISO9141Channels
        {
            get { return m_iso9141Channels; }
            set { m_iso9141Channels = value; }
        }

        public bool IsISO9141Supported
        {
            get { return (m_iso9141Channels > 0 ? true : false); }
        }

        private int m_iso14230Channels;
        public int ISO14230Channels
        {
            get { return m_iso14230Channels; }
            set { m_iso14230Channels = value; }
        }

        public bool IsISO14230Supported
        {
            get { return (m_iso14230Channels > 0 ? true : false); }
        }

        private int m_sciAEngineChannels;
        public int SCI_A_ENGINEChannels
        {
            get { return m_sciAEngineChannels; }
            set { m_sciAEngineChannels = value; }
        }

        public bool IsSCI_A_ENGINESupported
        {
            get { return (m_sciAEngineChannels > 0 ? true : false); }
        }

        private int m_sciATransChannels;
        public int SCI_A_TRANSChannels
        {
            get { return m_sciATransChannels; }
            set { m_sciATransChannels = value; }
        }

        public bool IsSCI_A_TRANSSupported
        {
            get { return (m_sciATransChannels > 0 ? true : false); }
        }

        private int m_sciBEngineChannels;
        public int SCI_B_ENGINEChannels
        {
            get { return m_sciBEngineChannels; }
            set { m_sciBEngineChannels = value; }
        }

        public bool IsSCI_B_ENGINESupported
        {
            get { return (m_sciBEngineChannels > 0 ? true : false); }
        }

        private int m_sciBTransChannels;
        public int SCI_B_TRANSChannels
        {
            get { return m_sciBTransChannels; }
            set { m_sciBTransChannels = value; }
        }

        public bool IsSCI_B_TRANSSupported
        {
            get { return (m_sciBTransChannels > 0 ? true : false); }
        }

        public override string ToString()
        {
            return m_name;
        }
    }
}
