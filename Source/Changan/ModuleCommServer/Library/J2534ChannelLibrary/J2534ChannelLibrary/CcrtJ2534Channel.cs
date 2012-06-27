using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using J2534DotNet;

namespace J2534ChannelLibrary
{
    public class CcrtJ2534Channel
    {
        public CcrtJ2534Channel()
        {
            m_channelSettings = new CcrtJ2534ChannelSettings();
            m_channelComm = new CcrtJ2534ChannelComm(ref m_channelSettings);
        }
        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Channel Communication functions.
        /// </summary>
        private CcrtJ2534ChannelComm m_channelComm;
        /// <summary>
        /// Get/Set the channel comm.
        /// </summary>
        public CcrtJ2534ChannelComm ChannelComm
        {
            get { return m_channelComm; }
            set { m_channelComm = value; }
        }
        /// <summary>
        /// Channel Settings.
        /// </summary>
        private CcrtJ2534ChannelSettings m_channelSettings;
        /// <summary>
        /// User readable channel name.
        /// </summary>
        private string m_channelName;
        /// <summary>
        /// Get/Set the channel Name.
        /// </summary>
        public string ChannelName
        {
            get { return m_channelName; }
            set { m_channelName = value; }
        }
        /// <summary>
        /// Channel index.
        /// </summary>
        private int m_channelIndex;
        /// <summary>
        /// Get/Set the channel index.
        /// </summary>
        public int ChannelIndex
        {
            get { return m_channelIndex; }
            set { m_channelIndex = value; }
        }

        /// <summary>
        /// Get/Set the connection flag.
        /// </summary>
        public ConnectFlag ConnFlag
        {
            get { return m_channelSettings.m_connectFlag; }
            set { m_channelSettings.m_connectFlag = value; }
        }
        public string GetConnectFlag()
        {
            return ConvertFromConnectFlag(ConnFlag);
        }
        public void SetConnectFlag(string flag)
        {
            ConnFlag = ConvertToConnectFlag(flag);
        }
        /// <summary>
        /// Get/Set the baud rate.
        /// </summary>
        public BaudRate BRate
        {
            get { return m_channelSettings.m_baudRate; }
            set { m_channelSettings.m_baudRate = value; }
        }
        public string GetBaudRate()
        {
            return ConvertFromBaudRate(BRate);
        }
        public void SetBaudRate(string baud)
        {
            BRate = ConvertToBaudRate(baud);
        }        
        /// <summary>
        /// Get/Set the Device ID.
        /// </summary>
        public int DeviceID
        {
            get { return m_channelSettings.m_deviceID; }
            set { m_channelSettings.m_deviceID = value; }
        }
        /// <summary>
        /// Get/Set the Device ID.
        /// </summary>
        public void SetJ2534Interface(IJ2534Interface iface)
        {
            m_channelComm.J2534Interface = iface;
        }

        /// <summary>
        /// Get/Set the protocol ID.
        /// </summary>
        public ProtocolID ProtoID
        {
            get { return m_channelSettings.m_protocolID; }
            set { m_channelSettings.m_protocolID = value; }
        }
        public string GetProtocolID()
        {
            return ConvertFromProtocolID(ProtoID);
        }
        public void SetProtocolID(string protocol)
        {
            ProtoID = ConvertToProtocolID(protocol);
        }
        /// <summary>
        /// Add and start a message filter.
        /// </summary>
        /// <param name="filter">Request / response id to be added.</param>
        /// <returns>True/False success / failure</returns>
        public bool AddMessageFilter(CcrtJ2534Defs.MessageFilter filter)
        {
            return m_channelComm.AddFilter(filter);
        }

        //-----------------------------------------------------------------------------------------
        // Utility Conversions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Convert a Protocol ID type to it's string representation.
        /// </summary>
        /// <param name="connFlag">Protocol ID type to convert to string.</param>
        /// <returns>String representation of Protocol ID.</returns>
        public static String ConvertFromProtocolID(ProtocolID pID)
        {
            if (pID == ProtocolID.CAN) return "CAN";
            else if (pID == ProtocolID.ISO14230) return "ISO14230";
            else if (pID == ProtocolID.ISO15765) return "ISO15765";
            else if (pID == ProtocolID.ISO9141) return "ISO9141";
            else if (pID == ProtocolID.J1850PWM) return "J1850PWM";
            else if (pID == ProtocolID.J1850VPW) return "J1850VPW";
            else if (pID == ProtocolID.SCI_A_ENGINE) return "SCI_A_ENGINE";
            else if (pID == ProtocolID.SCI_A_TRANS) return "SCI_A_TRANS";
            else if (pID == ProtocolID.SCI_B_ENGINE) return "SCI_B_ENGINE";
            else if (pID == ProtocolID.SCI_B_TRANS) return "SCI_B_TRANS";
            else return "Unknown";
        }
        /// <summary>
        /// Convert a string to a Protocol ID type.
        /// </summary>
        /// <param name="connFlag">Protocol ID type to convert to string.</param>
        /// <returns>String representation of protocol ID.</returns>
        public static ProtocolID ConvertToProtocolID(string pID)
        {
            if (pID.Equals("CAN")) return ProtocolID.CAN;
            else if (pID.Equals("ISO14230")) return ProtocolID.ISO14230;
            else if (pID.Equals("ISO15765")) return ProtocolID.ISO15765;
            else if (pID.Equals("ISO9141")) return ProtocolID.ISO9141;
            else if (pID.Equals("J1850PWM")) return ProtocolID.J1850PWM;
            else if (pID.Equals("J1850VPW")) return ProtocolID.J1850VPW;
            else if (pID.Equals("SCI_A_ENGINE")) return ProtocolID.SCI_A_ENGINE;
            else if (pID.Equals("SCI_B_ENGINE")) return ProtocolID.SCI_B_ENGINE;
            else if (pID.Equals("SCI_B_TRANS")) return ProtocolID.SCI_B_TRANS;
            else return ProtocolID.CAN;
        }
        /// <summary>
        /// Convert Config parameter typ to a string.
        /// </summary>
        /// <param name="connFlag">string to convert to config parameter</param>
        /// <returns>string representation of config parameter</returns>
        public static String ConvertToConfigParamter(ConfigParameter pID)
        {
            if (pID == ConfigParameter.BIT_SAMPLE_POINT) return "BIT_SAMPLE_POINT";
            else if (pID == ConfigParameter.BS_TX) return "BS_TX";
            else if (pID ==ConfigParameter.DATA_BITS) return "DATA_BITS";
            else if (pID ==ConfigParameter.DATA_RATE) return "DATA_RATE";
            else if (pID ==ConfigParameter.FIVE_BAUD_MOD) return "FIVE_BAUD_MOD";
            else if (pID ==ConfigParameter.ISO15765_BS) return "ISO15765_BS";
            else if (pID ==ConfigParameter.ISO15765_STMIN) return "ISO15765_STMIN";
            else if (pID ==ConfigParameter.ISO15765_WFT_MAX) return "ISO15765_WFT_MAX";
            else if (pID ==ConfigParameter.LOOPBACK) return "LOOPBACK";
            else if (pID ==ConfigParameter.NETWORK_LINE) return "NETWORK_LINE";
            else if (pID ==ConfigParameter.NODE_ADDRESS) return "NODE_ADDRESS";
            else if (pID ==ConfigParameter.P1_MAX) return "P1_MAX";
            else if (pID ==ConfigParameter.P2_MAX) return "P2_MAX";
            else if (pID ==ConfigParameter.P2_MIN) return "P2_MIN";
            else if (pID ==ConfigParameter.P3_MAX) return "P3_MAX";
            else if (pID ==ConfigParameter.P3_MIN) return "P3_MIN";
            else if (pID ==ConfigParameter.P4_MAX) return "P4_MAX";
            else if (pID ==ConfigParameter.P4_MIN) return "P4_MIN";
            else if (pID ==ConfigParameter.PARITY) return "PARITY";
            else if (pID ==ConfigParameter.STMIN_TX) return "STMIN_TX";
            else if (pID ==ConfigParameter.SYNC_JUMP_WIDTH) return "SYNC_JUMP_WIDTH";
            else if (pID ==ConfigParameter.T1_MAX) return "T1_MAX";
            else if (pID ==ConfigParameter.T2_MAX) return "T2_MAX";
            else if (pID ==ConfigParameter.T3_MAX) return "T3_MAX";
            else if (pID ==ConfigParameter.T4_MAX) return "T4_MAX";
            else if (pID ==ConfigParameter.T5_MAX) return "T5_MAX";
            else if (pID ==ConfigParameter.TIDLE) return "TIDLE";
            else if (pID ==ConfigParameter.TINIL) return "TINIL";
            else if (pID ==ConfigParameter.TWUP) return "TWUP";
            else if (pID ==ConfigParameter.unused) return "unused";
            else if (pID ==ConfigParameter.W0) return "W0";
            else if (pID ==ConfigParameter.W1) return "W1";
            else if (pID ==ConfigParameter.W2) return "W2";
            else if (pID ==ConfigParameter.W3) return "W3";
            else if (pID ==ConfigParameter.W4) return "W4";
            else if (pID == ConfigParameter.W5) return"W5";
            else return "DATA_RATE";
        }
        /// <summary>
        /// Convert a string to a Config parameter type.
        /// </summary>
        /// <param name="connFlag">string to convert to config parameter</param>
        /// <returns>config parameter</returns>
        public static ConfigParameter ConvertToConfigParamter(string pID)
        {
            if (pID.Equals("BIT_SAMPLE_POINT")) return ConfigParameter.BIT_SAMPLE_POINT;
            else if (pID.Equals("BS_TX")) return ConfigParameter.BS_TX;
            else if (pID.Equals("DATA_BITS")) return ConfigParameter.DATA_BITS;
            else if (pID.Equals("DATA_RATE")) return ConfigParameter.DATA_RATE;
            else if (pID.Equals("FIVE_BAUD_MOD")) return ConfigParameter.FIVE_BAUD_MOD;
            else if (pID.Equals("ISO15765_BS")) return ConfigParameter.ISO15765_BS;
            else if (pID.Equals("ISO15765_STMIN")) return ConfigParameter.ISO15765_STMIN;
            else if (pID.Equals("ISO15765_WFT_MAX")) return ConfigParameter.ISO15765_WFT_MAX;
            else if (pID.Equals("LOOPBACK")) return ConfigParameter.LOOPBACK;
            else if (pID.Equals("NETWORK_LINE")) return ConfigParameter.NETWORK_LINE;
            else if (pID.Equals("NODE_ADDRESS")) return ConfigParameter.NODE_ADDRESS;
            else if (pID.Equals("P1_MAX")) return ConfigParameter.P1_MAX;
            else if (pID.Equals("P2_MAX")) return ConfigParameter.P2_MAX;
            else if (pID.Equals("P2_MIN")) return ConfigParameter.P2_MIN;
            else if (pID.Equals("P3_MAX")) return ConfigParameter.P3_MAX;
            else if (pID.Equals("P3_MIN")) return ConfigParameter.P3_MIN;
            else if (pID.Equals("P4_MAX")) return ConfigParameter.P4_MAX;
            else if (pID.Equals("P4_MIN")) return ConfigParameter.P4_MIN;
            else if (pID.Equals("PARITY")) return ConfigParameter.PARITY;
            else if (pID.Equals("STMIN_TX")) return ConfigParameter.STMIN_TX;
            else if (pID.Equals("SYNC_JUMP_WIDTH")) return ConfigParameter.SYNC_JUMP_WIDTH;
            else if (pID.Equals("T1_MAX")) return ConfigParameter.T1_MAX;
            else if (pID.Equals("T2_MAX")) return ConfigParameter.T2_MAX;
            else if (pID.Equals("T3_MAX")) return ConfigParameter.T3_MAX;
            else if (pID.Equals("T4_MAX")) return ConfigParameter.T4_MAX;
            else if (pID.Equals("T5_MAX")) return ConfigParameter.T5_MAX;
            else if (pID.Equals("TIDLE")) return ConfigParameter.TIDLE;
            else if (pID.Equals("TINIL")) return ConfigParameter.TINIL;
            else if (pID.Equals("TWUP")) return ConfigParameter.TWUP;
            else if (pID.Equals("unused")) return ConfigParameter.unused;
            else if (pID.Equals("W0")) return ConfigParameter.W0;
            else if (pID.Equals("W1")) return ConfigParameter.W1;
            else if (pID.Equals("W2")) return ConfigParameter.W2;
            else if (pID.Equals("W3")) return ConfigParameter.W3;
            else if (pID.Equals("W4")) return ConfigParameter.W4;
            else if (pID.Equals("W5")) return ConfigParameter.W5;
            else return ConfigParameter.DATA_RATE;
        }
        /// <summary>
        /// Convert a BaudRate type to it's string representation.
        /// </summary>
        /// <param name="connFlag">Baud Rate type to convert to string.</param>
        /// <returns>String representation of BaudRate.</returns>
        public static String ConvertFromBaudRate(BaudRate bRate)
        {
            if (bRate == BaudRate.CAN) return "CAN";
            else if (bRate == BaudRate.CAN_125000) return "CAN_125000";
            else if (bRate == BaudRate.CAN_250000) return "CAN_250000";
            else if (bRate == BaudRate.CAN_500000) return "CAN_500000";
            else if (bRate == BaudRate.ISO14230) return "ISO14230";
            else if (bRate == BaudRate.ISO14230_10000) return "ISO14230_10000";
            else if (bRate == BaudRate.ISO14230_10400) return "ISO14230_10400";
            else if (bRate == BaudRate.ISO15765) return "ISO15765";
            else if (bRate == BaudRate.ISO15765_125000) return "ISO15765_125000";
            else if (bRate == BaudRate.ISO15765_250000) return "ISO15765_250000";
            else if (bRate == BaudRate.ISO15765_500000) return "ISO15765_500000";
            else if (bRate == BaudRate.ISO9141) return "ISO9141";
            else if (bRate == BaudRate.ISO9141_10000) return "ISO9141_10000";
            else if (bRate == BaudRate.ISO9141_10400) return "ISO9141_10400";
            else if (bRate == BaudRate.J1850PWM) return "J1850PWM";
            else if (bRate == BaudRate.J1850PWM_41600) return "J1850PWM_41600";
            else if (bRate == BaudRate.J1850PWM_83300) return "J1850PWM_83300";
            else if (bRate == BaudRate.J1850VPW) return "J1850VPW";
            else if (bRate == BaudRate.J1850VPW_10400) return "J1850VPW_10400";
            else if (bRate == BaudRate.J1850VPW_41600) return "J1850VPW_41600";
            else return "Unknown";
        }
        /// <summary>
        /// Convert a string to a BaudRate type.
        /// </summary>
        /// <param name="connFlag">Baud Rate type to convert to string.</param>
        /// <returns>String representation of baud rate.</returns>
        public static BaudRate ConvertToBaudRate(string bRate)
        {
            if (bRate.Equals("CAN")) return BaudRate.CAN;
            else if (bRate.Equals("CAN_125000")) return BaudRate.CAN_125000;
            else if (bRate.Equals("CAN_250000")) return BaudRate.CAN_250000;
            else if (bRate.Equals("CAN_500000")) return BaudRate.CAN_500000;
            else if (bRate.Equals("ISO14230")) return BaudRate.ISO14230;
            else if (bRate.Equals("ISO14230_10000")) return BaudRate.ISO14230_10000;
            else if (bRate.Equals("ISO14230_10400")) return BaudRate.ISO14230_10400;
            else if (bRate.Equals("ISO15765")) return BaudRate.ISO15765;
            else if (bRate.Equals("ISO15765_125000")) return BaudRate.ISO15765_125000;
            else if (bRate.Equals("ISO15765_250000")) return BaudRate.ISO15765_250000;
            else if (bRate.Equals("ISO15765_500000")) return BaudRate.ISO15765_500000;
            else if (bRate.Equals("ISO9141")) return BaudRate.ISO9141;
            else if (bRate.Equals("ISO9141_10000")) return BaudRate.ISO9141_10000;
            else if (bRate.Equals("ISO9141_10400")) return BaudRate.ISO9141_10400;
            else if (bRate.Equals("J1850PWM")) return BaudRate.J1850PWM;
            else if (bRate.Equals("J1850PWM_41600")) return BaudRate.J1850PWM_41600;
            else if (bRate.Equals("J1850PWM_83300")) return BaudRate.J1850PWM_83300;
            else if (bRate.Equals("J1850VPW")) return BaudRate.J1850VPW;
            else if (bRate.Equals("J1850VPW_10400")) return BaudRate.J1850VPW_10400;
            else if (bRate.Equals("J1850VPW_41600")) return BaudRate.J1850VPW_41600;
            else return BaudRate.CAN;
        }
        /// <summary>
        /// Convert a Connect flag type to it's string representation.
        /// </summary>
        /// <param name="connFlag">Connect Flag type to convert to string.</param>
        /// <returns>String representation of connect flag.</returns>
        public static String ConvertFromConnectFlag(ConnectFlag flag)
        {
            if (flag == ConnectFlag.CAN_29BIT_ID) return "CAN 29 Bit ID";
            else if (flag == ConnectFlag.CAN_ID_BOTH) return "Both";
            else if (flag == ConnectFlag.ISO9141_K_LINE_ONLY) return "ISO9141 K Line Only";
            else if (flag == ConnectFlag.ISO9141_NO_CHECKSUM) return "ISO9141 No Checksum";
            else return "None";
        }
        /// <summary>
        /// Convert a string to a Connect flag type.
        /// </summary>
        /// <param name="connFlag">Connect Flag type to convert to string.</param>
        /// <returns>String representation of connect flag.</returns>
        public static ConnectFlag ConvertToConnectFlag(string flag)
        {
            if (flag.Equals("CAN 29 Bit ID")) return ConnectFlag.CAN_29BIT_ID;
            else if (flag.Equals("Both")) return ConnectFlag.CAN_ID_BOTH;
            else if (flag.Equals("ISO9141 K Line Only")) return ConnectFlag.ISO9141_K_LINE_ONLY;
            else if (flag.Equals("ISO9141 No Checksum")) return ConnectFlag.ISO9141_NO_CHECKSUM;
            else return ConnectFlag.NONE;
        }
    }
}
