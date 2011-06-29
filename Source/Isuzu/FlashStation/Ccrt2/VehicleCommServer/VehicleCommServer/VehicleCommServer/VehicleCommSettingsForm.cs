using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using J2534ChannelLibrary;
using J2534DotNet;

namespace VehicleCommServer
{
    public partial class VehicleCommSettingsForm : Form
    {
        IVehicleCommServerInterface m_vcsInterface;
        public VehicleCommSettingsForm(IVehicleCommServerInterface vcsInterface)
        {
            InitializeComponent();
            m_vcsInterface = vcsInterface;

            PopulateDeviceComboBox();
            PopulateProtocolComboBox();
            PopulateConnectFlagComboBox();
            PopulateBaudRateComboBox();
            PopulateChannelIndexComboBox();
            PopulateCommChannelComboBox();
        }
        /// <summary>
        /// Populate the Communication Channel Combo box with database items.
        /// </summary>
        private void PopulateDeviceComboBox()
        {   // Clear the port combo box
            m_deviceComboBox.Items.Clear();
            // Add the device names to the combo box
            foreach (string device in m_vcsInterface.GetJ2534DeviceNames())
            {
                m_deviceComboBox.Items.Add(device);
            }
            // Select the first item in the list
            m_deviceComboBox.Sorted = true;
            m_deviceComboBox.SelectedIndex = 0;
        }
        /// <summary>
        /// Populate the Communication Channel Combo box with database items.
        /// </summary>
        private void PopulateCommChannelComboBox()
        {   // Clear the channel name combo box
            m_commNameComboBox.Items.Clear();
            m_commNameComboBox.Text = "";
            // Add the channel names to the combo box
            foreach (string chName in m_vcsInterface.GetCommChannelNames(m_selectedDevice.Name))
            {
                m_commNameComboBox.Items.Add(chName);
            }
            // Select the first item in the list
            m_commNameComboBox.Sorted = true;
            if (m_commNameComboBox.Items.Count > 0)
            {
                m_commNameComboBox.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Populate the Protocol Combo box with possible items.
        /// </summary>
        private void PopulateProtocolComboBox()
        {   // Clear the protocol combo box
            m_protocolComboBox.Items.Clear();
            // Add the possible protocol names to the combo box
            if (m_selectedDevice.IsCANSupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.CAN));
            if (m_selectedDevice.IsISO14230Supported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO14230));
            if (m_selectedDevice.IsISO15765Supported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO15765));
            if (m_selectedDevice.IsISO9141Supported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO9141));
            if (m_selectedDevice.IsJ1850PWMSupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.J1850PWM));
            if (m_selectedDevice.IsJ1850VPWSupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.J1850VPW));
            if (m_selectedDevice.IsSCI_A_ENGINESupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_A_ENGINE));
            if (m_selectedDevice.IsSCI_A_TRANSSupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_A_TRANS));
            if (m_selectedDevice.IsSCI_B_ENGINESupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_B_ENGINE));
            if (m_selectedDevice.IsSCI_B_TRANSSupported) m_protocolComboBox.Items.Add(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_B_TRANS));

            // Select the first item in the list
            m_protocolComboBox.Sorted = true;
            m_protocolComboBox.SelectedIndex = 0;
        }
        /// <summary>
        /// Populate the Connect Flag Combo box with possible items.
        /// </summary>
        private void PopulateConnectFlagComboBox()
        {   // Clear the connect flag combo box
            m_connectFlagComboBox.Items.Clear();
            // Add the connect flag values to the combo box
            m_connectFlagComboBox.Items.Add(CcrtJ2534Channel.ConvertFromConnectFlag(ConnectFlag.CAN_29BIT_ID));
            m_connectFlagComboBox.Items.Add(CcrtJ2534Channel.ConvertFromConnectFlag(ConnectFlag.CAN_ID_BOTH));
            m_connectFlagComboBox.Items.Add(CcrtJ2534Channel.ConvertFromConnectFlag(ConnectFlag.ISO9141_K_LINE_ONLY));
            m_connectFlagComboBox.Items.Add(CcrtJ2534Channel.ConvertFromConnectFlag(ConnectFlag.ISO9141_NO_CHECKSUM));
            m_connectFlagComboBox.Items.Add(CcrtJ2534Channel.ConvertFromConnectFlag(ConnectFlag.NONE));
            // Select the first item in the list
            m_connectFlagComboBox.Sorted = true;
            m_connectFlagComboBox.SelectedIndex = 0;
        }
        /// <summary>
        /// Populate the Baud Rate Combo box with possible items.
        /// </summary>
        private void PopulateBaudRateComboBox()
        {   // Clear the baud rate combo box
            m_baudRateComboBox.Items.Clear();
            // Add the baud rates to the combo box
            //strings placed because enum values for some values are the same
            m_baudRateComboBox.Items.Add("CAN");
            m_baudRateComboBox.Items.Add("CAN_125000");
            m_baudRateComboBox.Items.Add("CAN_250000");
            m_baudRateComboBox.Items.Add("CAN_500000");
            m_baudRateComboBox.Items.Add("ISO14230");
            m_baudRateComboBox.Items.Add("ISO14230_10000");
            m_baudRateComboBox.Items.Add("ISO14230_10400");
            m_baudRateComboBox.Items.Add("ISO15765");
            m_baudRateComboBox.Items.Add("ISO15765_125000");
            m_baudRateComboBox.Items.Add("ISO15765_250000");
            m_baudRateComboBox.Items.Add("ISO15765_500000");
            m_baudRateComboBox.Items.Add("ISO9141");
            m_baudRateComboBox.Items.Add("ISO9141_10000");
            m_baudRateComboBox.Items.Add("ISO9141_10400");
            m_baudRateComboBox.Items.Add("J1850PWM");
            m_baudRateComboBox.Items.Add("J1850PWM_41600");
            m_baudRateComboBox.Items.Add("J1850PWM_83300");
            m_baudRateComboBox.Items.Add("J1850VPW");
            m_baudRateComboBox.Items.Add("J1850VPW_10400");
            m_baudRateComboBox.Items.Add("J1850VPW_41600");
            // Select the first item in the list
            m_baudRateComboBox.Sorted = true;
            m_baudRateComboBox.SelectedIndex = 0;
        }
        /// <summary>
        /// Populate the Channel index Combo box with possible items.
        /// </summary>
        private void PopulateChannelIndexComboBox()
        {// Clear the channel index combo box
            m_channelComboBox.Items.Clear();
            int channelsSupported = 0;
            // Add possible channel index selections to the combo box
            if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.CAN)))
            {
                channelsSupported = m_selectedDevice.CANChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO14230)))
            {
                channelsSupported = m_selectedDevice.ISO14230Channels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO15765)))
            {
                channelsSupported = m_selectedDevice.ISO15765Channels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.ISO9141)))
            {
                channelsSupported = m_selectedDevice.ISO9141Channels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.J1850PWM)))
            {
                channelsSupported = m_selectedDevice.J1850PWMChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.J1850VPW)))
            {
                channelsSupported = m_selectedDevice.J1850VPWChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_A_ENGINE)))
            {
                channelsSupported = m_selectedDevice.SCI_A_ENGINEChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_A_TRANS)))
            {
                channelsSupported = m_selectedDevice.SCI_A_TRANSChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_B_ENGINE)))
            {
                channelsSupported = m_selectedDevice.SCI_B_ENGINEChannels;
            }
            else if (m_protocolComboBox.SelectedItem.Equals(CcrtJ2534Channel.ConvertFromProtocolID(ProtocolID.SCI_B_TRANS)))
            {
                channelsSupported = m_selectedDevice.SCI_B_TRANSChannels;
            }
            for (int x = 0; x < channelsSupported; x++)
            {
                m_channelComboBox.Items.Add((x+1).ToString());
            }
            // Select the first item in the list
            m_channelComboBox.Sorted = true;
            m_channelComboBox.SelectedIndex = 0;
        }
        /// <summary>
        /// Populate the Communication Channel Combo box with database items.
        /// </summary>
        private void DisplayCommChannelSettings()
        {   //select settings for each combo box
            if (m_commNameComboBox.SelectedItem != null)
            {
                VehicleCommDeviceSettings settings = m_vcsInterface.GetVehicleCommDeviceCollection().Find(m_commNameComboBox.SelectedItem.ToString(),m_deviceComboBox.SelectedItem.ToString());
                m_protocolComboBox.SelectedItem = settings.ProtocolID;
                m_baudRateComboBox.SelectedItem = settings.BaudRate;
                m_channelComboBox.SelectedItem = settings.ChannelIndex;
                m_connectFlagComboBox.SelectedItem = settings.ConnectFlag;
  
                //CcrtJ2534Channel channel = m_vcsInterface.GetDataBaseInterface().ReadCommChannelSettings(m_deviceComboBox.SelectedItem.ToString(),
                //                                                                m_commNameComboBox.SelectedItem.ToString());
                //m_protocolComboBox.SelectedItem = CcrtJ2534Channel.ConvertFromProtocolID(channel.ProtoID);
                //m_baudRateComboBox.SelectedItem = CcrtJ2534Channel.ConvertFromBaudRate(channel.BRate);
                //m_channelComboBox.SelectedItem = channel.ChannelIndex.ToString();
                //m_connectFlagComboBox.SelectedItem = CcrtJ2534Channel.ConvertFromConnectFlag(channel.ConnFlag);
            }
            
        }
        /// <summary>
        /// Get the comm channel settings from the form selected by the user.
        /// </summary>
        /// <returns>Comm channel settings selected by the user.</returns>
        private CcrtJ2534Channel GetUserSelectedCommChannelSettings()
        {
            ProtocolID pID = CcrtJ2534Channel.ConvertToProtocolID(m_protocolComboBox.SelectedItem.ToString());

            CcrtJ2534Channel settings = new CcrtJ2534Channel();
            settings.BRate = CcrtJ2534Channel.ConvertToBaudRate(m_baudRateComboBox.SelectedItem.ToString());
            settings.ConnFlag = CcrtJ2534Channel.ConvertToConnectFlag(m_connectFlagComboBox.SelectedItem.ToString());
            settings.ProtoID = CcrtJ2534Channel.ConvertToProtocolID(m_protocolComboBox.SelectedItem.ToString());
            settings.ChannelIndex = Convert.ToInt16(m_channelComboBox.SelectedItem.ToString());
            settings.ChannelName = m_commNameComboBox.Text;
            return settings;
        }


        /// <summary>
        /// Index to selected device in list
        /// </summary>
        private J2534Device m_selectedDevice;



        //-----------------------------------------------------------------------------------------
        // Class Callbacks
        //-----------------------------------------------------------------------------------------
        private void m_closeButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void m_deviceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_selectedDevice = m_vcsInterface.GetJ2534Devices().Find(delegate(J2534Device dev)
            {
                return dev.Name == m_deviceComboBox.SelectedItem.ToString();
            });
            PopulateCommChannelComboBox();
            //Update list of possible protocol selections for this device
            PopulateProtocolComboBox();
        }

        private void m_commNameComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayCommChannelSettings();
        }

        private void m_protocolComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {//Update list of possible channels
            PopulateChannelIndexComboBox();
        }

        private void m_removeDeviceButton_Click(object sender, EventArgs e)
        {
            // Remove the selected item from the database
            if (m_commNameComboBox.SelectedItem != null && m_commNameComboBox.Text != "")
            {
                m_vcsInterface.GetVehicleCommDeviceCollection().RemoveDeviceSettings(m_commNameComboBox.SelectedItem.ToString(),
                    m_deviceComboBox.SelectedItem.ToString());
                m_commNameComboBox.Items.Remove(m_commNameComboBox.SelectedItem);
            }
            // Display the remaining items
            if (m_commNameComboBox.Items.Count > 0)
            {
                m_commNameComboBox.SelectedIndex = 0;
            }
            else
            {
                m_commNameComboBox.Text = "";
            }
            
        }

        private void m_saveSettingsButton_Click(object sender, EventArgs e)
        {   // Get all the comm channel setting items from the form
            CcrtJ2534Channel settings = GetUserSelectedCommChannelSettings();
            String commChannelName = m_commNameComboBox.Text;
            if (m_commNameComboBox.FindString(commChannelName) != -1)
            {   // Input device already exists, just update the item settings in the data base
                m_vcsInterface.GetVehicleCommDeviceCollection().UpdateVehicleDeviceSettings(commChannelName, m_selectedDevice.Name,
                    Convert.ToInt16(m_channelComboBox.SelectedItem.ToString()), m_connectFlagComboBox.SelectedItem.ToString(),
                    m_baudRateComboBox.SelectedItem.ToString(), m_protocolComboBox.SelectedItem.ToString());
            }
            else
            {   // This is a new item, so add the item to the database
                if (commChannelName != "")
                {
                    m_vcsInterface.GetVehicleCommDeviceCollection().Add(commChannelName, m_selectedDevice.Name,
                    Convert.ToInt16(m_channelComboBox.SelectedItem.ToString()), m_connectFlagComboBox.SelectedItem.ToString(),
                    m_baudRateComboBox.SelectedItem.ToString(), m_protocolComboBox.SelectedItem.ToString());
                    PopulateCommChannelComboBox();
                    m_commNameComboBox.SelectedItem = commChannelName;
                }
                else
                {//error we need a name for the channel
                }
            }
        }
    }
}
