using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MES_Data_Interface
{
    public delegate void SettingsChangedEvent(Object sender, EventArgs e);

    public partial class SettingsForm : Form
    {
        Settings m_settings;

        public event SettingsChangedEvent SettingsChanged;

        public SettingsForm()
        {
            InitializeComponent();
            m_settings = Settings.ReadSettings();
            m_qnxIpAddress.Text = m_settings.QnxIpAddress;
            m_qnxListenerTcpPort.Text = m_settings.QnxListenerTcpPort;
            m_mesHostname.Text = m_settings.MesHostName;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_settings.QnxIpAddress = m_qnxIpAddress.Text;
            m_settings.QnxListenerTcpPort = m_qnxListenerTcpPort.Text;
            m_settings.MesHostName = m_mesHostname.Text;
            m_settings.SaveSettings();
            SettingsChanged(this, EventArgs.Empty);
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
