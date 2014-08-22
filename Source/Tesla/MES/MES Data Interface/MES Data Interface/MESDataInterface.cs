using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using LoggingInterface;

namespace MES_Data_Interface
{
    public partial class MESDataInterface : Form
    {
        String m_logsDirectory = Path.Combine(Application.StartupPath, "Logs\\");
        Settings m_settings;
        SettingsForm m_settingsForm;
        Logger m_logger;
        ResultHandler m_resultHandler;
        Boolean m_closeWhenListenerDone;
        delegate void CloseApplicationEventHandler();

        public MESDataInterface()
        {
            InitializeComponent();
            if (!Directory.Exists(m_logsDirectory)) Directory.CreateDirectory(m_logsDirectory);
            m_logger = new Logger(m_logMessagesBox, "MesDataInterfaceLog", m_logsDirectory);
            Log("Loading settings...");
            m_settings = Settings.ReadSettings();
            Log("Loading settings... done.");
            Log("Settings\n\tQNX IP Address: " + m_settings.QnxIpAddress + 
                "\n\tLocal Listener IP Address: " + m_settings.QnxListenerIpAddress +
                "\n\tLocal Listener TCP Port: " + m_settings.QnxListenerTcpPort +
                "\n\tMES Hostname: " + m_settings.MesHostName);

            m_closeWhenListenerDone = false;
            m_resultHandler = new ResultHandler(m_settings, Log);
            m_resultHandler.ListenerClosed += m_resultHandler_ListenerClosed;
        }

        void m_resultHandler_ListenerClosed(object sender, EventArgs e)
        {
            if (m_closeWhenListenerDone) CloseApplication();
        }

        void Log(String message)
        {
            m_logger.Log(message);
        }

        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_settingsForm = new SettingsForm();
            m_settingsForm.SettingsChanged += m_settingsForm_SettingsChanged;
            m_settingsForm.FormClosed += m_settingsForm_FormClosed;
            m_settingsForm.Show();
        }

        void m_settingsForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_settingsForm.FormClosed -= m_settingsForm_FormClosed;
            m_settingsForm.SettingsChanged -= m_settingsForm_SettingsChanged;
        }

        void m_settingsForm_SettingsChanged(object sender, EventArgs e)
        {
            Log("Reloading settings...");
            m_settings = Settings.ReadSettings();
            Log("Reloading settings... done.");
            Log("Settings\n\tQNX IP Address: " + m_settings.QnxIpAddress + "\n\tMES Hostname: " + m_settings.MesHostName);
        }

        void CloseApplication()
        {
            if (InvokeRequired)
            {
                CloseApplicationEventHandler cb = new CloseApplicationEventHandler(CloseApplication);
                Invoke(cb);
            }
            else
            {
                Close();
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CloseApplication();
        }

        private void MESDataInterface_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_resultHandler.ListenerActive)
            {
                m_resultHandler.CloseListener();
                m_closeWhenListenerDone = true;
                e.Cancel = true;
            }
        }
    }
}
