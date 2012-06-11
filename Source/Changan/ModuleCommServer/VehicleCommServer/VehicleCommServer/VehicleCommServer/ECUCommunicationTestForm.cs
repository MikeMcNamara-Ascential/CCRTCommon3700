using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using J2534ChannelLibrary;
using System.Threading;

namespace VehicleCommServer
{
    public partial class ECUCommunicationTestForm : Form
    {
        IVehicleCommServerInterface m_vcsInterface;
        delegate void SetLogCallback(string line);
        public ECUCommunicationTestForm(IVehicleCommServerInterface vcsInterface)
        {
            InitializeComponent();
            m_vcsInterface = vcsInterface;
            PopulateDeviceComboBox();
            m_moduleIDDataGridView.Rows.Add();
            m_responseIDDataGridView.Rows.Add();
            m_periodicMsgIds = new List<int>();
            //zero out visible dgview cells
            foreach (DataGridViewCell cell in m_moduleIDDataGridView.Rows[0].Cells)
            {
                cell.Value = "00";
            }
            foreach (DataGridViewCell cell in m_responseIDDataGridView.Rows[0].Cells)
            {
                cell.Value = "00";
            }
            m_threadID = 0;
        }

        private void Log(string line)
        {
            if (this.m_textBoxStatus.InvokeRequired)
            {
                SetLogCallback d = new SetLogCallback(Log);
                this.Invoke(d, new object[] { line });
            }
            else
            {
                m_textBoxStatus.AppendText(line + "\r\n");
            }
        }
        private void PopulateDeviceComboBox()
        {
            m_deviceComboBox.Items.Clear();
            m_deviceComboBox.Text = "";
            // Add the channel names to the combo box
            foreach (string name in m_vcsInterface.GetJ2534DeviceNames())
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
            foreach (string chName in m_vcsInterface.GetCommChannelNames())
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
            m_selectedDevice = m_deviceComboBox.SelectedItem.ToString();
        }
        private void m_channelComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_selectedChannel = m_channelComboBox.SelectedItem.ToString();
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
            List<byte[]> responseIDs = null;
            row = m_responseIDDataGridView.Rows[0];
            if (m_globalRequestCheckBox.Checked)
            {
                responseIDs = new List<byte[]>();
                foreach(DataGridViewRow dgvRow in m_responseIDDataGridView.Rows)
                {
                    responseID.Clear();
                    if (!dgvRow.IsNewRow)
                    {
                        foreach (DataGridViewCell cell in row.Cells)
                        {
                            responseID.Add(Convert.ToByte(Int16.Parse(cell.Value.ToString(), System.Globalization.NumberStyles.HexNumber)));
                        }
                        responseIDs.Add(responseID.ToArray());
                    }
                }
                message.m_messageFilter.responseIDs = responseIDs;
            }
            else
            {
                foreach (DataGridViewCell cell in row.Cells)
                {
                    responseID.Add(Convert.ToByte(Int16.Parse(cell.Value.ToString(), System.Globalization.NumberStyles.HexNumber)));
                }
                message.m_messageFilter.responseID = responseID.ToArray();
            }
            
            
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
            if (m_globalRequestCheckBox.Checked)
            {
                foreach (byte[] id in message.m_messageFilter.responseIDs)
                {
                    logMessage += "ResponseID: [" + BitConverter.ToString(id) + "]\r\n";
                }
            }
            else
            {
                logMessage += "ResponseID: [" + BitConverter.ToString(message.m_messageFilter.responseID.ToArray()) + "]\r\n";
            }
            logMessage += "TX Message: [" + BitConverter.ToString(message.m_txMessage.ToArray()) + "]";
            Log(logMessage);
            return message;
        }

        private void m_sendMessageButton_Click(object sender, EventArgs e)
        {

            Log("Adding Module Filter");
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = CreateECUMessageFromInput();
            List<List<byte>> datas = new List<List<byte>>();
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
            if (m_globalRequestCheckBox.Checked)
            {
                status = m_vcsInterface.GetECUData(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(), message, ref datas,m_globalRequestCheckBox.Checked);
            }
            else
            {
                status = m_vcsInterface.GetECUData(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(), message, ref data);
            }
            Log("Sending Message...");
            if (status)
            {
                Log("Message Successful");
                if (m_globalRequestCheckBox.Checked)
                {
                    for (int index = 0; index < datas.Count; index++)
                    {
                        Log("Received: " + BitConverter.ToString(message.m_messageFilter.responseIDs[index]) + " [" + BitConverter.ToString(datas[index].ToArray()) + "]");
                    }
                }
                else
                {
                    Log("Received: [" + BitConverter.ToString(data.ToArray()) + "]");
                }
            }
            else
            {
                Log("Message Failure");
            }

        }

        private void m_startMsgButton_Click(object sender, EventArgs e)
        {
            //warning only use for messages without response
            if (!m_responseExpectedCheckBox.Checked)
            {
                int id = 0;
                Log("Adding Module Filter");
                J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = CreateECUMessageFromInput();
                List<byte> data = new List<byte>();
                bool status = m_vcsInterface.AddMessageFilter(m_deviceComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString(),
                    message.m_messageFilter);
                message.m_responseExpected = false;
                if (status)
                {
                    Log("Message Filter Added");
                }
                else
                {
                    Log("Message Filter Add Failed");
                }
                status = m_vcsInterface.StartPeriodicMessage(m_deviceComboBox.SelectedItem.ToString(),
                    m_channelComboBox.SelectedItem.ToString(), message, ref id);
                Log("Starting Periodic Message...");
                if (status)
                {
                    Log("Message Successful");
                    Log("Received: [" + BitConverter.ToString(data.ToArray()) + "]");
                    m_periodicMsgIds.Add(id);
                }
                else
                {
                    Log("Message Failure");
                }
            }
            else
            {
                Log("Periodic Messages not allowed for responding messages");
            }
        }
        private List<int> m_periodicMsgIds;

        private void m_stopMsgButton_Click(object sender, EventArgs e)
        {
            bool status;
            foreach (int id in m_periodicMsgIds)
            {
                Log("Stopping Periodic Message at ID: " + id.ToString());
                status = m_vcsInterface.StopPeriodicMessage(m_deviceComboBox.SelectedItem.ToString(),
        m_channelComboBox.SelectedItem.ToString(), id);
                if (status)
                {
                    Log("Success");
                }
                else
                {
                    Log("Failure");
                }
            }
            m_periodicMsgIds.Clear();
        }
        private void MessageThread()
        {
            int threadID = m_threadID++;
            Log(threadID.ToString() + " Adding Module Filter");
            J2534ChannelLibrary.CcrtJ2534Defs.ECUMessage message = m_ecuMessage;
            List<byte> data = new List<byte>();
            Random random = new Random();
            string device = m_selectedDevice;
            string channel = m_selectedChannel;
            bool status = m_vcsInterface.AddMessageFilter(device, channel,
                message.m_messageFilter);
            if (status)
            {
                Log(threadID.ToString() + " Message Filter Added");
            }
            else
            {
                Log(threadID.ToString() + " Message Filter Add Failed");
            }
            while (!m_stopMessageThreads)
            {
                m_vcsInterface.ClearResponseBuffer(device, channel);
                status = m_vcsInterface.GetECUData(device, channel, message, ref data);
                Log(threadID.ToString() + " Sending Message...");
                if (status)
                {
                    Log(threadID.ToString() + " Message Successful");
                    Log(threadID.ToString() + " Received: [" + BitConverter.ToString(data.ToArray()) + "]");
                }
                else
                {
                    Log(threadID.ToString() + " Message Failure");
                }
                //sleep random amount of time
                Thread.Sleep(random.Next(1000));
            }
        }


        private void m_startThreadButton_Click(object sender, EventArgs e)
        {
            m_stopMessageThreads = false;
            m_ecuMessage = CreateECUMessageFromInput();
            ThreadStart taskDelegate = new ThreadStart(MessageThread);
            Thread taskThread = new Thread(taskDelegate);
            taskThread.Start();
        }

        private void m_stopThreadsButton_Click(object sender, EventArgs e)
        {
            m_stopMessageThreads = true;
        }
        private bool m_stopMessageThreads;
        private int m_threadID;
        private string m_selectedDevice;
        private string m_selectedChannel;
        private CcrtJ2534Defs.ECUMessage m_ecuMessage;


    }
}
