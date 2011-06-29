using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using FileChangeMonitor;
using LoggingInterface;

namespace TestFCM
{
    public partial class FCMTestMainForm : Form
    {
        FCM m_fcm;
        public FCMTestMainForm()
        {                       
            InitializeComponent();
            Logger logger = new Logger(m_logRTB, "FCMTestApplication");
            m_fcm = new FCM("C:\\VehicleBuildRecords\\", "C:\\BuildFiles\\", "C:\\Temp\\",1000, logger);
        }

        private void FCMTestMainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_fcm.StopFCMThread();
        }
    }
}
