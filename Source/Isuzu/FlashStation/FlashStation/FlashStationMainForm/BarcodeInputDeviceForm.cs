using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Common.Lib.Presenters;

namespace FlashStationMainForm
{
    public partial class BarcodeInputDeviceForm : Form
    {
        MainFormDataPresenter m_presenter;
        private string m_currentDevice;
        public BarcodeInputDeviceForm(MainFormDataPresenter presenter)
        {
            InitializeComponent();
            m_presenter = presenter;
            PopulateComboBox();
        }
        private void PopulateComboBox()
        {
            m_devicesComboBox.Items.Clear();
            m_devicesComboBox.Items.AddRange(m_presenter.GetAvailableBarcodeInputDevices().ToArray());
            m_currentDevice = m_presenter.GetCurrentBarcodeInputDevice();
            if (m_devicesComboBox.Items.Count > 0)
            {
                m_devicesComboBox.SelectedItem = m_devicesComboBox.Items[0];
            }
            if (m_currentDevice != "null")
            {
                m_devicesComboBox.SelectedItem = m_currentDevice;
            }
        }

        private void m_advancedButton_Click(object sender, EventArgs e)
        {
            m_presenter.OpenSerialDeviceCommSettings();
            m_presenter.UpdateAvailableBarcodeInputDevices();
            PopulateComboBox();
        }

        private void m_saveButton_Click(object sender, EventArgs e)
        {
            try
            {
                    m_presenter.SetBarcodeInputDevice(m_devicesComboBox.SelectedItem.ToString());
                    Properties.Settings.Default.BarcodeInputDevice = m_devicesComboBox.SelectedItem.ToString();
                    Properties.Settings.Default.Save();
            }
            catch
            {//indicate error
                MessageBox.Show("Device Name must be specified", "Invalid Device Name",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            this.Close();
        }
        private void m_cancelButton_Click(object sender, EventArgs e)
        {
            m_presenter.SetBarcodeInputDevice(m_currentDevice);
            this.Close();
        }
    }
}
