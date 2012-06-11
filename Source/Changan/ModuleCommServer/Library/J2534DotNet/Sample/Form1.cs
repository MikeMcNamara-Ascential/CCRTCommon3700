using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using J2534DotNet;

namespace Sample
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void cmdDetectDevices_Click(object sender, EventArgs e)
        {
            List<J2534Device> list = J2534Detect.GetDeviceList();

            if (list.Count == 0)
            {
                MessageBox.Show("Could not find any installed J2534 devices.");
                return;
            }
            foreach (J2534Device device in list)
            {
                txtDevices.Text += "Name: " + device.Name + "\r\n";
                txtDevices.Text += "\tVendor: " + device.Vendor + "\r\n";
                txtDevices.Text += "\tConfigApplication: " + device.ConfigApplication + "\r\n";
                txtDevices.Text += "\tFunctionLibrary: " + device.FunctionLibrary + "\r\n";
                
                if(device.IsCANSupported)
                    txtDevices.Text += "\tCANChannels: " + device.CANChannels + "\r\n";
                if(device.IsISO15765Supported)
                    txtDevices.Text += "\tISO15765Channels: " + device.ISO15765Channels + "\r\n";
                if(device.IsISO14230Supported)
                    txtDevices.Text += "\tISO14230Channels: " + device.ISO14230Channels + "\r\n";
                if (device.IsISO9141Supported)
                    txtDevices.Text += "\tISO9141Channels: " + device.ISO9141Channels + "\r\n";
                if(device.IsJ1850PWMSupported)
                    txtDevices.Text += "\tJ1850PWMChannels: " + device.J1850PWMChannels + "\r\n";
                if (device.IsJ1850VPWSupported)
                    txtDevices.Text += "\tJ1850PWMChannels: " + device.J1850VPWChannels + "\r\n";
                if(device.IsSCI_A_ENGINESupported)
                    txtDevices.Text += "\tSCI_A_ENGINEChannels: " + device.SCI_A_ENGINEChannels + "\r\n";
                if (device.IsSCI_A_TRANSSupported)
                    txtDevices.Text += "\tSCI_A_TRANSChannels: " + device.SCI_A_TRANSChannels + "\r\n";
                if (device.IsSCI_B_ENGINESupported)
                    txtDevices.Text += "\tSCI_B_ENGINEChannels: " + device.SCI_B_ENGINEChannels + "\r\n";
                if (device.IsSCI_B_TRANSSupported)
                    txtDevices.Text += "\tSCI_B_TRANSChannels: " + device.SCI_B_TRANSChannels + "\r\n";
            }
        }

        private void cmdReadVoltage_Click(object sender, EventArgs e)
        {
            double voltage = 0;
            List<J2534Device> list = J2534Detect.GetDeviceList();

            if (list.Count == 0)
            {
                MessageBox.Show("Could not find any installed J2534 devices.");
                return;
            }

            // Always choose the first J2534 device in the list
            J2534Device selectedDevice = list[0];

            J2534Interface passThru = new J2534Interface();
            passThru.LoadLibrary(selectedDevice);

            ObdComm comm = new ObdComm(passThru);
            if (!comm.DetectProtocol())
            {
                MessageBox.Show(String.Format("Error connecting to device. Error: {0}", comm.GetLastError()));
                comm.Disconnect();
                return;
            }
            if (!comm.GetBatteryVoltage(ref voltage))
            {
                MessageBox.Show(String.Format("Error reading voltage.  Error: {0}", comm.GetLastError()));
                comm.Disconnect();
                return;
            }
            comm.Disconnect();
            txtVoltage.Text = voltage + " V";
        }

        private void cmdReadFaults_Click(object sender, EventArgs e)
        {
            string vin = "";
            List<J2534Device> list = J2534Detect.GetDeviceList();

            if (list.Count == 0)
            {
                MessageBox.Show("Could not find any installed J2534 devices.");
                return;
            }

            // Always choose the first J2534 device in the list
            J2534Device selectedDevice = list[0];

            J2534Interface passThru = new J2534Interface();
            passThru.LoadLibrary(selectedDevice);

            ObdComm comm = new ObdComm(passThru);
            if (!comm.DetectProtocol())
            {
                MessageBox.Show(String.Format("Error connecting to device. Error: {0}", comm.GetLastError()));
                comm.Disconnect();
                return;
            }

            if (!comm.GetVin(ref vin))
            {
                MessageBox.Show(String.Format("Error reading vin.  Error: {0}", comm.GetLastError()));
                comm.Disconnect();
                return;
            }
            comm.Disconnect();
            txtReadFaults.Text = vin;
        }

        private void button1_Click(object sender, EventArgs e)
        {

            m_obdComm.CreatePassThroughMsgs();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            List<J2534Device> list = J2534Detect.GetDeviceList();

            if (list.Count == 0)
            {
                MessageBox.Show("Could not find any installed J2534 devices.");
                return;
            }
            // Always choose the first J2534 device in the list
            J2534Device selectedDevice = list[0];

            J2534Interface passThru = new J2534Interface();
            passThru.LoadLibrary(selectedDevice);

            m_obdComm = new ObdComm(passThru);
        }
        private ObdComm m_obdComm;
    }
}
