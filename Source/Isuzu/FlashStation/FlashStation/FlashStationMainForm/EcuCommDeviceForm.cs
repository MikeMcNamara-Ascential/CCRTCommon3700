using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Common.Lib.Presenters;
using Common.Classes;
using LoggingInterface;

namespace FlashStationMainForm
{
    public partial class EcuCommDeviceForm : Form
    {
        MainFormDataPresenter m_presenter;
        private string m_currentDevice;
        private string m_currentChannel;
        private Logger m_logger;
        public EcuCommDeviceForm(MainFormDataPresenter presenter, Logger logger)
        {
            InitializeComponent();
            m_presenter = presenter;
            m_logger = logger;
            PopulateComboBoxes();
        }
        private void PopulateComboBoxes()
        {
            m_devicesComboBox.Items.AddRange(m_presenter.GetAvailableDevices().ToArray());
            m_currentDevice = m_presenter.GetCurrentDevice();
            m_devicesComboBox.SelectedItem = m_devicesComboBox.Items[0];
            if (m_currentDevice != "null")
            {
                m_devicesComboBox.SelectedItem = m_currentDevice;
                m_channelComboBox.Items.AddRange(m_presenter.GetDeviceChannels(m_devicesComboBox.SelectedItem.ToString()).ToArray());
            }
            m_currentChannel = m_presenter.GetCurrentChannel();
            if (m_currentChannel != "null")
            {
                m_channelComboBox.SelectedItem = m_currentChannel;

            }
            else
            {
                m_presenter.OpenVehicleCommSettings();
                if (m_channelComboBox.Items.Count > 0)
                {
                    m_channelComboBox.Items.AddRange(m_presenter.GetDeviceChannels(m_devicesComboBox.SelectedItem.ToString()).ToArray());
                    m_channelComboBox.SelectedItem = m_channelComboBox.Items[0];
                }
            }

        }
        private void UpdateChannelsBox()
        {
            m_channelComboBox.Items.Clear();
            try
            {
                m_channelComboBox.Items.AddRange(m_presenter.GetDeviceChannels(m_devicesComboBox.SelectedItem.ToString()).ToArray());
                if (m_channelComboBox.Items.Count > 0)
                {
                    m_channelComboBox.SelectedItem = m_channelComboBox.Items[0];
                }
                else
                {
                    m_channelComboBox.Text = "";
                }
            }
            catch
            {
                m_channelComboBox.SelectedItem = null;
            }
        }

        private void m_advancedButton_Click(object sender, EventArgs e)
        {
            m_presenter.OpenVehicleCommSettings();
        }

        private void m_saveButton_Click(object sender, EventArgs e)
        {
            try
            {
                    m_presenter.SetDeviceAndChannel(m_devicesComboBox.SelectedItem.ToString(), m_channelComboBox.SelectedItem.ToString());
                    Properties.Settings.Default.CommChannel = m_channelComboBox.SelectedItem.ToString();
                    Properties.Settings.Default.Device = m_devicesComboBox.SelectedItem.ToString();
                    m_logger.Log("INFO:  Comm device updated to: " + m_devicesComboBox.SelectedItem.ToString());
                    m_logger.Log("INFO:  Comm channel updated to: " + m_channelComboBox.SelectedItem.ToString());    
                Properties.Settings.Default.Save();
            }
            catch
            {//indicate error
                MessageBox.Show("Channel Name must be specified", "Invalid Channel Name",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
                m_logger.Log("ERROR:  EcuCommDeviceEntryForm: Channel Not Specified before saving");
                return;
            }
            this.Close();
        }
        private void m_cancelButton_Click(object sender, EventArgs e)
        {
            m_presenter.SetDeviceAndChannel(m_currentDevice, m_currentChannel);
            m_logger.Log("INFO:  EcuCommDeviceEntryForm: comm devices reverted to original values");
            this.Close();
        }

        private void m_devicesComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateChannelsBox();
        }
    }
}
