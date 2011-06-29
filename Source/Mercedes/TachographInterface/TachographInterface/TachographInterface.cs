using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace TachographInterface
{
    public partial class m_tacographInterface : Form
    {
        public m_tacographInterface()
        {
            InitializeComponent();
            // Start an Interface object to listen for connections
            m_ccrt = new CcrtRealtimeInterface(m_logBox);
            // Initialization complete, waiting for clients to connect
            LogMessage("Interface initialized.  Waiting for client to connect...\r\n");
        }

        private void m_exitButton_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        // Allows the new threads to log messages as well as the main thread.
        delegate void SetTextCallback(string text);
        public void LogMessage(string text)
        {
            if (m_logBox.InvokeRequired)
            {
                SetTextCallback callBack = new SetTextCallback(LogMessage);
                Invoke(callBack, new object[] { text });
            }
            else
            {
                m_logBox.AppendText(text);
            }
        }

        private CcrtRealtimeInterface m_ccrt;
    }
}