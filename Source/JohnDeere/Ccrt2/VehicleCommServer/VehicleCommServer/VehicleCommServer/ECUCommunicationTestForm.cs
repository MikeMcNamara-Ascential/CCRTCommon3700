using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using J2534ChannelLibrary;

namespace VehicleCommServer
{
    public partial class ECUCommunicationTestForm : Form
    {
        VehicleCommServerInterface m_vcsInterface;
        public ECUCommunicationTestForm()
        {
            InitializeComponent();
            m_vcsInterface = new VehicleCommServerInterface();
            PopulateDeviceComboBox();
            m_moduleIDDataGridView.Rows.Add();
            m_responseIDDataGridView.Rows.Add();
            //zero out visible dgview cells
            foreach (DataGridViewCell cell in m_moduleIDDataGridView.Rows[0].Cells)
            {
                cell.Value = "00";
            }
            foreach (DataGridViewCell cell in m_responseIDDataGridView.Rows[0].Cells)
            {
                cell.Value = "00";
            }
        }

        private void Log(string line)
        {
                m_textBoxStatus.AppendText(line + "\r\n");
        }
        private void PopulateDeviceComboBox()
        {
            m_deviceComboBox.Items.Clear();
            m_deviceComboBox.Text = "";
            // Add the channel names to the combo box
            foreach (string name in m_vcsInterface.J2534DeviceNames)
            {
                m_deviceComboBox.Items.Add(name);
            }
            // Select the first item in the list
            m_deviceComboBox.Sorted = true;
            if (m_deviceComboBox.Items.Count > 0)
            {
                m_deviceComboBox.SelectedIndex = 0;
            }
        }
        private void PopulateChannelComboBox()
        {
            m_channelComboBox.Items.Clear();
            m_channelComboBox.Text = "";
            // Add the channel names to the combo box
            foreach (string chName in m_vcsInterface.DbInterface.GetCommChannelNames())
            {
                m_channelComboBox.Items.Add(chName);
            }
            // Select the first item in the list
            m_channelComboBox.Sorted = true;
            if (m_channelComboBox.Items.Count > 0)
            {
                m_channelComboBox.SelectedIndex = 0;
            }
        }

        private void m_deviceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            PopulateChannelComboBox();
        }
        private J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage CreateECUMessageFromInput()
        {
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = new CcrtJ2534Defs.ECUMessage();
            DataGridViewRow row = m_moduleIDDataGridView.Rows[0];
            //add module request ID
            List<byte> moduleID = new List<byte>();
            foreach (DataGridViewCell cell in row.Cells)
            {
                moduleID.Add(Convert.ToByte(Int16.Parse(cell.Value.ToString(), System.Globalization.NumberStyles.HexNumber))); 
            }
            message.m_messageFilter.requestID = moduleID.ToArray();
            //add module response ID
            List<byte> responseID = new List<byte>();
            row = m_responseIDDataGridView.Rows[0];
            foreach (DataGridViewCell cell in row.Cells)
            {
                responseID.Add(Convert.ToByte(Int16.Parse(cell.Value.ToString(), System.Globalization.NumberStyles.HexNumber)));
            }
            message.m_messageFilter.responseID = responseID.ToArray();
            //Add Tx message
            List<byte> txMessage = new List<byte>();
            foreach (DataGridViewRow dgRow in m_transmitMessageDataGridView.Rows)
            {
                if (!dgRow.IsNewRow)
                {
                    foreach (DataGridViewCell dgCell in dgRow.Cells)
                    {
                        if (dgCell.Value != null)
                        {
                            txMessage.Add(Convert.ToByte(UInt16.Parse(dgCell.Value.ToString(), System.Globalization.NumberStyles.HexNumber)));
                        }
                    }
                }
            }
            message.m_txMessage = txMessage;
            string logMessage = "RequestID: [" + BitConverter.ToString(message.m_messageFilter.requestID.ToArray()) + "]\r\n";
            logMessage += "ResponseID: [" + BitConverter.ToString(message.m_messageFilter.responseID.ToArray()) + "]\r\n";
            logMessage += "TX Message: [" + BitConverter.ToString(message.m_txMessage.ToArray()) + "]";
            Log(logMessage);
            return message;
        }

        private void m_sendMessageButton_Click(object sender, EventArgs e)
        {

            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = CreateECUMessageFromInput();
            Log("Sending Message...");
            bool status =
m_vcsInterface.SendMessage(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(),
    message);
            if (status)
            {
                Log("Send Successful"); 
            }
            else
            {
                Log("Send Failed");
            }

        }

        private void m_getDataButton_Click(object sender, EventArgs e)
        {
            Log("Adding Module Filter");
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = CreateECUMessageFromInput();
            List<byte> data = new List<byte>();
            bool status = m_vcsInterface.AddMessageFilter(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(),
                message.m_messageFilter);
            if (status)
            {
                Log("Message Filter Added");
            }
            else
            {
                Log("Message Filter Add Failed");
            }
            status = m_vcsInterface.GetECUData(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(), message, ref data);
            Log("Sending Message...");
            if (status)
            {
                Log("Message Successful");
                Log("Received: [" + BitConverter.ToString(data.ToArray()) + "]");
            }
            else
            {
                Log("Message Failure");
            }
        }
    }
}
