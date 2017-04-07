using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Timers;
using System.Windows.Forms;

using SerialPortLibrary;



namespace TeslaHostSimulator
{
    public partial class TeslaHostSimulator : Form
    {
        public TeslaHostSimulator()
        {
            InitializeComponent();
            m_portSettings = null;
            m_teslaPort = null;
            m_txMsgs = new Dictionary<string, byte[]>();
            SetupTxMsgs();
            SetupMessageTypes();
            m_txMsgsComboBox.SelectedIndex = 0;
            m_msgTimer = new System.Timers.Timer(50);
            m_msgTimer.AutoReset = true;
            m_msgTimer.Elapsed += m_msgTimer_Elapsed;
            m_msgTimer.Enabled = true;
            m_msgTimer.Stop();
            PopulatePeriodicMessageDataGrid(null);
            PopulateResultsMessageDataGrid("ResultRollerName", null);
        }

        private void LogMessage(String msg)
        {
            if (m_logTextBox.InvokeRequired)
            {
                LogMessageDelegate callback = new LogMessageDelegate(LogMessage);
                m_logTextBox.Invoke(callback, new object[] { msg });
            }
            else
            {
                m_logTextBox.Text += msg;
            }
        }

        private void SetupTxMsgs()
        {
            m_txMsgs.Add("Start Force Measurement - Drag", new byte[4] { 0x02, 0x46, 0x44, 0x03 });
            m_txMsgs.Add("Start Force Measurement - Brake", new byte[4] { 0x02, 0x46, 0x42, 0x03 });

            m_txMsgs.Add("Stop Force Measurement - Drag", new byte[4] { 0x02, 0x53, 0x44, 0x03 });
            m_txMsgs.Add("Stop Force Measurement - Brake", new byte[4] { 0x02, 0x53, 0x42, 0x03 });
            
            m_txMsgs.Add("Get Measured Force - Drag", new byte[4] { 0x02, 0x4D, 0x44, 0x03 });
            m_txMsgs.Add("Get Measured Force - Brake", new byte[4] { 0x02, 0x4D, 0x42, 0x03 });

            m_txMsgs.Add("Start ABS Test", new byte[3] { 0x02, 0x56, 0x03 });
            m_txMsgs.Add("Stop ABS Test", new byte[3] { 0x02, 0x54, 0x03 });
            m_txMsgs.Add("Get ABS Results", new byte[3] { 0x02, 0x41, 0x03 });

            m_txMsgs.Add("Start Test Cycle", new byte[4] { 0x02, 0x43, 0x42, 0x03 });
            m_txMsgs.Add("Stop Test Cycle", new byte[4] { 0x02, 0x43, 0x45, 0x03 });
        }

        private void SetupMessageTypes()
        {
            m_msgTypes = new Dictionary<byte, string>();
            m_msgTypes.Add(0x46, "Start Force Measurement");
            m_msgTypes.Add(0x53, "Stop Force Measurement");
            m_msgTypes.Add(0x4D, "Send Force Measurement");
            m_msgTypes.Add(0x56, "Start ABS Valve Test");
            m_msgTypes.Add(0x54, "Stop ABS Valve Test");
            m_msgTypes.Add(0x41, "ABS Results");
            m_msgTypes.Add(0x43, "Test Cycle Control");
        }

        private void PopulatePeriodicMessageDataGrid(SerialData msg = null)
        {
            for (int index = 0; index < 4; index++)
            {
                if (msg == null)
                {
                    DataGridViewRow row = new DataGridViewRow();
                    row.CreateCells(m_icmDataGridView);
                    row.Cells[m_icmDataGridView.Columns["RollerName"].Index].Value = m_rollerNames.Split(',')[index];
                    m_icmDataGridView.Rows.Add(row);
                }
                else
                {
                    Int32 rawSpeed = (msg.Data[(index * 2) + 2] << 8) | msg.Data[(index * 2) + 3];
                    Int32 rawForce = (msg.Data[(index * 2) + 10] << 8) | msg.Data[(index * 2) + 11];
                    m_icmDataGridView.Rows[index].Cells["RollerSpeed"].Value = rawSpeed / 10.0;
                    m_icmDataGridView.Rows[index].Cells["RollerForce"].Value = rawForce / 10.0;
                }
            }
        }

        private void PopulateResultsMessageDataGrid(String columnName, SerialData msg = null)
        {
            for (int index = 0; index < 4; index++)
            {
                switch(columnName)
                {
                    case "ResultRollerName":
                        DataGridViewRow row = new DataGridViewRow();
                        row.CreateCells(m_resultsDataGridView);
                        row.Cells[m_resultsDataGridView.Columns[columnName].Index].Value = m_rollerNames.Split(',')[index];
                        m_resultsDataGridView.Rows.Add(row);
                        break;

                    case "DragValues":
                    case "BrakeValues":
                        Int32 data = (msg.Data[(index * 2) + 2] << 8) | msg.Data[(index * 2) + 3];
                        m_resultsDataGridView.Rows[index].Cells[columnName].Value = data / 10.0;
                        break;

                    case "AbsResults":
                        Int32 minData = (msg.Data[(index * 4) + 2] << 8) | msg.Data[(index * 4) + 3];
                        Int32 maxData = (msg.Data[(index * 4) + 4] << 8) | msg.Data[(index * 4) + 5];
                        m_resultsDataGridView.Rows[index].Cells["AbsMinValues"].Value = minData / 10.0;
                        m_resultsDataGridView.Rows[index].Cells["AbsMaxValues"].Value = maxData / 10.0;
                        break;
                }
            }
        }

        private void ProcessMessage(SerialData message)
        {
            switch (message.Data[1])
            {
                case 0x50:
                    PopulatePeriodicMessageDataGrid(message);
                    break;

                case 0x4D:
                    String column = String.Empty;
                    switch (message.Data[2])
                    {
                        case 0x44:
                            column = "DragValues";
                            break;

                        case 0x42:
                            column = "BrakeValues";
                            break;
                    }
                    PopulateResultsMessageDataGrid(column, message);
                    break;

                case 0x41:
                    PopulateResultsMessageDataGrid("AbsResults", message);
                    break;

                case 0x06:
                case 0x0F:
                    String logMsg = String.Empty;

                    for (int index = 0; index < message.ByteCount - 1; index++)
                    {
                        logMsg += String.Format(" {0}", message.Data[index]);
                    }
                    String type = message.Data[1] == 0x06 ? "ACK" : "NAK";
                    LogMessage(String.Format("Received {3} for {0} - {1}{2}",
                                                       m_msgTypes[message.Data[2]], logMsg, Environment.NewLine, type));
                    break;
            }
        }

        /// <summary>
        /// Determine if the received message is valid
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        private Boolean IsMessageValid(SerialData message)
        {
            Boolean valid = false;
            byte chkSum = 0x00;
            String logMsg = String.Empty;
            for (int index = 0; index < (message.ByteCount - 1); index++)
            {
                chkSum += message.Data[index];
                logMsg += String.Format(" {0}", message.Data[index].ToString("X2"));
            }
            if (chkSum == message.Data[message.ByteCount - 1])
            {
                if((message.Data[0] == 0x02) && (message.Data[message.ByteCount - 2] == 0x03))
                {
                    valid = true;
                }
            }
            if (!valid)
            {
                LogMessage(String.Format("Invalid message: {0}{1}", logMsg, Environment.NewLine));
            }

            return valid;
        }


        #region Member Variables / Properties

        private PortSettings m_portSettings;
        private CcrtSerialPort m_teslaPort;
        private Dictionary<String, byte[]> m_txMsgs;
        private Dictionary<byte, String> m_msgTypes;
        private System.Timers.Timer m_msgTimer;

        private const String m_rollerNames = "Left Front,Right Front,Left Rear,Right Rear";

        #endregion


        #region Callbacks, Delegates, Events

        private delegate void LogMessageDelegate(String msg);

        /// <summary>
        /// Close the serial port and exit the application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_msgTimer.Stop();
            if (m_teslaPort != null)
            {
                m_teslaPort.StopRxDataMonitor();
                m_teslaPort.ClosePort();
            }
            Application.Exit();
        }

        /// <summary>
        /// Setup the comm port for the Tesla interface.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void commPortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_msgTimer.Stop();
            PortSetupForm frm = new PortSetupForm();
            frm.m_portSettings = m_portSettings;
            frm.ShowDialog();
            m_portSettings = frm.m_portSettings;
            if (m_teslaPort == null)
            {
                m_teslaPort = new CcrtSerialPort();
            }
            else
            {
                m_teslaPort.StopRxDataMonitor();
            }
            m_teslaPort.UpdatePortSettings(m_portSettings);
            m_teslaPort.ViewSerialPortData(true);
            m_msgTimer.Start();
        }

        /// <summary>
        /// Send the selected message to CCRT.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_txMsgButton_Click(object sender, EventArgs e)
        {
            String msgName = m_txMsgsComboBox.SelectedItem.ToString();
            byte [] tmpTxMsg = m_txMsgs[msgName];
            byte[] txMsg = new byte[tmpTxMsg.Length + 1];
            byte chkSum = 0x00;
            String logMsg = String.Empty;
            for (int index = 0; index < tmpTxMsg.Length; index++)
            {
                txMsg[index] = tmpTxMsg[index];
                chkSum += tmpTxMsg[index];
                logMsg += String.Format(" {0}", tmpTxMsg[index].ToString("X2"));
            }
            txMsg[txMsg.Length - 1] = chkSum;
            m_teslaPort.WriteSerialData(txMsg, txMsg.Length);
            LogMessage(String.Format("Sending Msg - {0} = {1}{2}", msgName, logMsg, Environment.NewLine));
        }

        /// <summary>
        /// Check for and process any new messages.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void m_msgTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            SerialData msg = m_teslaPort.RxData;
            if (msg.ByteCount > 0)
            {
                if (IsMessageValid(msg))
                {
                    ProcessMessage(msg);
                }
            }
        }

        #endregion

    }
}
