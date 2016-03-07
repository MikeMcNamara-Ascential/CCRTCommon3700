using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Logger;


namespace LoggerTest
{
    public partial class LoggerTestForm : Form
    {
        public LoggerTestForm()
        {
            InitializeComponent();
            // Create a new logger
            Logger = new CcrtLogger(@"C:\Temp\", "LoggerTest", m_logMsgTextBox, 2, 500000);
            Logger.Log("Starting log timer");
            m_logTimer.Interval = 1;
            m_logTimer.Enabled = true;
            m_logTimer.Start();
        }


        private CcrtLogger Logger { get; set; }

        private void m_exitButton_Click(object sender, EventArgs e)
        {
            m_logTimer.Stop();
            Environment.Exit(0);
        }

        private void m_logTimer_Tick(object sender, EventArgs e)
        {
            Logger.Log("Timer tick at " + DateTime.Now.ToString());
            // filler text so that we can more quickly fill up the max memory buffer size
            Logger.Log("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ_ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        }
    }
}
