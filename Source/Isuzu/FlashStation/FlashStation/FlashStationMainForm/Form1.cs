using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Common.Lib.Interfaces;
using Common.Lib.Presenters;
using Common.Classes;
using LoggingInterface;
using System.IO;

namespace FlashStationMainForm
{
    public partial class Form1 : Form, IMainFormView
    {
        private MainFormDataPresenter m_presenter;
        delegate void SetPrompt1TextCallback(string text);
        delegate void SetPrompt2TextCallback(string text);
        delegate void SetPrompt1BGColorCallback(Color color);
        delegate void SetPrompt2BGColorCallback(Color color);
        delegate void SetECMResultBoxBGColorCallback(Color color);
        delegate void SetTCMResultBoxBGColorCallback(Color color);
        delegate void SetVINTextCallback(string vin);
        delegate void SetModelCodeTextCallback(string modelCode);
        delegate void SetBookCodeTextCallback(string bookCode);
        delegate void SetModelYearTextCallback(string modelYear);
        delegate void SetLotNumberTextCallback(string lotNumber);

        //delegate void SetLogCallback(string barcode);
        delegate void SetDisplayBuildDataCallback(string barcode);
        delegate void SetClearBuildDataCallback();
        delegate void SetClearLogDisplayCallback();
        delegate void SetProgressValueCallback(int value);
        delegate void SetStartProgressBarCallback();
        delegate void SetProgressRangeCallback(int min, int max);
        delegate void SetStopProgressBarCallback();
        delegate void SetEngineControllerTextCallback(string type);
        delegate void SetTransmissionTextCallback(string type);
        delegate void SetDisplayTimedDialogBoxCallback(string dialogText, string labelText, string buttonText, int timeout);
        private Logger m_logger;
        public Form1()
        {
            InitializeComponent();
            //determine if comm device is set up
            m_logger = new Logger(m_logMessageTextBox, "FlashStationLog", @"C:\FlashStation\Logs\");
            m_presenter = new MainFormDataPresenter(this,m_logger);
            string deviceName = Properties.Settings.Default.Device;
            string channelName = Properties.Settings.Default.CommChannel;
            if (deviceName == "null" || channelName == "null")
            {//need to set up device / comm channel
                EcuCommDeviceForm frm = new EcuCommDeviceForm(m_presenter,m_logger);
                frm.ShowDialog();
            }
            if (Properties.Settings.Default.Device == "null" || Properties.Settings.Default.CommChannel == "null")
            {//values were not set
                return;
            }
            else
            {
                m_presenter.SetDeviceAndChannel(Properties.Settings.Default.Device, Properties.Settings.Default.CommChannel);
            }
            string barcodeInputDeviceName = Properties.Settings.Default.BarcodeInputDevice;
            if (barcodeInputDeviceName == "null")
            {//need to set up barcode input device
                BarcodeInputDeviceForm frm = new BarcodeInputDeviceForm(m_presenter);
                frm.ShowDialog();
            }
            if (Properties.Settings.Default.BarcodeInputDevice == "null")
            {//values were not set
                return;
            }
            else
            {
                m_presenter.SetBarcodeInputDevice(Properties.Settings.Default.BarcodeInputDevice);
            }
            m_presenter.StartThreads();
            m_overallProgressBar.Minimum = 0;
            m_overallProgressBar.Maximum = 100;
            m_overallProgressBar.Value = 0;

        }
        public void SetPrompt1Text(string text)
        {
            if (this.m_promptBox1.InvokeRequired)
            {
                SetPrompt1TextCallback d = new SetPrompt1TextCallback(SetPrompt1Text);
                this.Invoke(d, new object[] { text });
            }
            else m_promptBox1.Text = text;
        }
        public void SetPrompt1BGColor(Color color)
        {
            if (this.m_promptBox1.InvokeRequired)
            {
                SetPrompt1BGColorCallback d = new SetPrompt1BGColorCallback(SetPrompt1BGColor);
                this.Invoke(d, new object[] { color });
            }
            else m_promptBox1.BackColor = color;
        }
        public void SetPrompt2Text(string text)
        {
            if (this.m_promptBox2.InvokeRequired)
            {
                SetPrompt2TextCallback d = new SetPrompt2TextCallback(SetPrompt2Text);
                this.Invoke(d, new object[] { text });
            }
            else m_promptBox2.Text = text;
        }
        public void SetPrompt2BGColor(Color color)
        {
            if (this.m_promptBox1.InvokeRequired)
            {
                SetPrompt2BGColorCallback d = new SetPrompt2BGColorCallback(SetPrompt2BGColor);
                this.Invoke(d, new object[] { color });
            }
            else m_promptBox2.BackColor = color;
        }

        public void SetECMResultBoxBGColor(Color color)
        {
            if (this.m_ecmResultBox.InvokeRequired)
            {
                SetECMResultBoxBGColorCallback d = new SetECMResultBoxBGColorCallback(SetECMResultBoxBGColor);
                this.Invoke(d, new object[] { color });
            }
            else m_ecmResultBox.BackColor = color;
        }
        public void SetTCMResultBoxBGColor(Color color)
        {
            if (this.m_tcmResultBox.InvokeRequired)
            {
                SetTCMResultBoxBGColorCallback d = new SetTCMResultBoxBGColorCallback(SetTCMResultBoxBGColor);
                this.Invoke(d, new object[] { color });
            }
            else m_tcmResultBox.BackColor = color;
        }

        public void SetBarcodeText(string barcode)
        {
            if (barcode != null)
            {
                if (barcode.Length == 30)
                {
                    string vin = barcode.Substring(0, 17);
                    string modelCode = barcode.Substring(17, 5);
                    string bookCode = barcode.Substring(22, 2);
                    string modelYear = barcode.Substring(24, 3);
                    string lotNumber = barcode.Substring(27, 3);
                    SetVINText(vin);
                    SetModelCodeText(modelCode);
                    SetBookCodeText(bookCode);
                    SetModelYearText(modelYear);
                    SetLotNumberText(lotNumber);
                }
                else
                {
                    SetVINText("");
                    SetModelCodeText("");
                    SetBookCodeText("");
                    SetModelYearText("");
                    SetLotNumberText("");
                }
            }
        }
        public void SetVINText(string vin)
        {
            if (this.m_vinLabel.InvokeRequired)
            {
                SetVINTextCallback d = new SetVINTextCallback(SetVINText);
                this.Invoke(d, new object[] { vin });
            }
            else m_vinLabel.Text = vin;
        }
        public void SetModelCodeText(string modelCode)
        {
            if (this.m_modelCodeLabel.InvokeRequired)
            {
                SetModelCodeTextCallback d = new SetModelCodeTextCallback(SetModelCodeText);
                this.Invoke(d, new object[] { modelCode });
            }
            else m_modelCodeLabel.Text = modelCode;
        }
        public void SetBookCodeText(string bookCode)
        {
            if (this.m_bookCodeLabel.InvokeRequired)
            {
                SetBookCodeTextCallback d = new SetBookCodeTextCallback(SetBookCodeText);
                this.Invoke(d, new object[] { bookCode });
            }
            else m_bookCodeLabel.Text = bookCode;
        }
        public void SetModelYearText(string modelYear)
        {
            if (this.m_modelYearLabel.InvokeRequired)
            {
                SetModelYearTextCallback d = new SetModelYearTextCallback(SetModelYearText);
                this.Invoke(d, new object[] { modelYear });
            }
            else m_modelYearLabel.Text = modelYear;
        }
        public void SetLotNumberText(string lotNumber)
        {
            if (this.m_lotNumberLabel.InvokeRequired)
            {
                SetLotNumberTextCallback d = new SetLotNumberTextCallback(SetLotNumberText);
                this.Invoke(d, new object[] { lotNumber });
            }
            else m_lotNumberLabel.Text = lotNumber;
        }
        public void DisplayBuildData(string barcode)
        {
            if (this.m_buildInfoTextBox.InvokeRequired)
            {
                SetDisplayBuildDataCallback d = new SetDisplayBuildDataCallback(DisplayBuildData);
                this.Invoke(d, new object[] { barcode });
            }
            else
            {
                //clear build data
                m_buildInfoTextBox.Clear();
                //display build data
                List<Common.Classes.ECUBuildData> buildData = m_presenter.GetBuildData();
                if (barcode.Length == 30)
                {
                    string vin = barcode.Substring(0, 17);
                    string modelCode = barcode.Substring(17, 5);
                    string bookCode = barcode.Substring(22, 2);
                    string modelYear = barcode.Substring(24, 3);
                    string lotNumber = barcode.Substring(27, 3);
                    m_buildInfoTextBox.Text += "VIN: " + vin;
                    m_buildInfoTextBox.Text += Environment.NewLine + "Model Code: " + modelCode;
                    m_buildInfoTextBox.Text += Environment.NewLine + "Book Code: " + bookCode;
                    m_buildInfoTextBox.Text += Environment.NewLine + "Model Year: " + modelYear;
                    m_buildInfoTextBox.Text += Environment.NewLine + "Lot Number: " + lotNumber;
                }

                foreach (Common.Classes.ECUBuildData build in buildData)
                {
                    m_buildInfoTextBox.Text += Environment.NewLine + Environment.NewLine + build.ECUName + Environment.NewLine + Environment.NewLine + "-------" +
                        "-------------------------------------------------------------------------------"+ 
                        "-------------------------------------------------------------------------------";
                    m_buildInfoTextBox.Text += Environment.NewLine + "Utility File Part Number: " + build.UtilityFilePartNumber;
                    m_buildInfoTextBox.Text += Environment.NewLine + "ECU Part Number: " + build.PartNumber;
                    int index = 0;
                    foreach (string pn in build.SoftwareModulePartNumbers)
                    {
                        m_buildInfoTextBox.Text += Environment.NewLine + "ECU SoftwarePartNumber index " + String.Format("{0:00}", index) +": " + pn;
                        index++;
                    }
                    m_buildInfoTextBox.Text += Environment.NewLine + Environment.NewLine;
                }
            }
        }
        public void ClearBuildData()
        {
            if (this.m_buildInfoTextBox.InvokeRequired)
            {
                SetClearBuildDataCallback d = new SetClearBuildDataCallback(ClearBuildData);
                this.Invoke(d, new object[] { });
            }
            else
            {
                //clear build data
                m_buildInfoTextBox.Clear();
            }
        }
        public void ClearLogDisplay()
        {
            if (this.m_buildInfoTextBox.InvokeRequired)
            {
                SetClearLogDisplayCallback d = new SetClearLogDisplayCallback(ClearLogDisplay);
                this.Invoke(d, new object[] { });
            }
            else
            {
                //clear build data
                m_logMessageTextBox.Clear();
            }
        }
        public void SetProgressBarValue(int value)
        {
            if (this.m_overallProgressBar.InvokeRequired)
            {
                SetProgressValueCallback d = new SetProgressValueCallback(SetProgressBarValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                if (value > m_overallProgressBar.Maximum)
                {
                    m_overallProgressBar.Value = m_overallProgressBar.Maximum;
                }
                else if (value < m_overallProgressBar.Minimum)
                {
                    m_overallProgressBar.Value = m_overallProgressBar.Minimum;
                }
                else
                {
                    m_overallProgressBar.Value = value;
                }
            }            
        }
        public void SetProgressBarRange(int minValue, int maxValue)
        {
            if (this.m_overallProgressBar.InvokeRequired)
            {
                SetProgressRangeCallback d = new SetProgressRangeCallback(SetProgressBarRange);
                this.Invoke(d, new object[] { minValue, maxValue });
            }
            else
            {
                m_overallProgressBar.Minimum = minValue;
                m_overallProgressBar.Maximum = maxValue;
            }
        }
        public void StartProgressBar()
        {
            if (this.m_overallProgressBar.InvokeRequired)
            {
                SetStartProgressBarCallback d = new SetStartProgressBarCallback(StartProgressBar);
                this.Invoke(d, new object[] {  });
            }
            else
            {
                m_overallProgressBar.Style = ProgressBarStyle.Marquee;
                m_overallProgressBar.MarqueeAnimationSpeed = 50;
            }
        }
        public void StopProgressBar()
        {
            if (this.m_overallProgressBar.InvokeRequired)
            {
                SetStopProgressBarCallback d = new SetStopProgressBarCallback(StopProgressBar);
                this.Invoke(d, new object[] { });
            }
            else
            {
                m_overallProgressBar.Style = ProgressBarStyle.Blocks;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            DialogResult result = MessageBox.Show("Exit Flash Station Program?", "Confirmation Required",
            MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.Yes)
            {
                m_presenter.Terminate();
            }
            else
            {
                e.Cancel = true;
            }
        }
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_presenter != null)
            {
                m_presenter.Terminate();
            }
        }
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void enterBarcodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BarcodeEntryForm frm = new BarcodeEntryForm(this,m_logger);
            frm.Show();
        }
        public void SubmitBarcodeValue(string value)
        {
            m_presenter.SubmitBarcodeValue(value);
        }

        private void vehicleCommunicationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            EcuCommDeviceForm frm = new EcuCommDeviceForm(m_presenter,m_logger);
            frm.Show();
        }
        public void CableConnected(bool connected)
        {
            ChangeLED(connected, m_cableConnectPB);
        }
        public void BuildDataValid(bool valid)
        {
            ChangeLED(valid, m_buildDataPB);
        }
        private void ChangeLED(bool state, PictureBox box)
        {
            if (state)
            {
                box.Image = Properties.Resources.led_green_on;
            }
            else
            {
                box.Image = Properties.Resources.led_green_off;
            }
        }
        public void DisplayTimedDialogBox(string dialogText, string labelText, string buttonText, int timeout)
        {
            if (this.InvokeRequired)
            {
                SetDisplayTimedDialogBoxCallback d = new SetDisplayTimedDialogBoxCallback(DisplayTimedDialogBox);
                this.Invoke(d, new object[] { dialogText,labelText, buttonText, timeout });
            }
            else
            {
                TimedDialogBox frm = new TimedDialogBox(dialogText, labelText, buttonText, timeout);
                frm.Show();
            }

        }

        private void barcodeInputDeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BarcodeInputDeviceForm frm = new BarcodeInputDeviceForm(m_presenter);
            frm.Show();
        }

        private void clearBuildDataToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_presenter.ClearBuildData();
        }

        private void abortFlashProcessToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("Abort Flash Proccess?", "Confirmation Required",
                MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.Yes)
            {
                m_presenter.UserAbort(false);
            }
        }

        private void forcedCableConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_presenter.ToggleForceCableConnect();
        }

        private void useForcedCableConnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_presenter.ToggleUsingForceCableConnect();
        }

        private void flashECMToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_presenter.SetPerformECMFlash(flashECMToolStripMenuItem.Checked);
            m_ecmResultBox.Enabled = flashECMToolStripMenuItem.Checked;
        }

        private void flashTCMToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_presenter.SetPerformTCMFlash(flashTCMToolStripMenuItem.Checked);
            m_tcmResultBox.Enabled = flashTCMToolStripMenuItem.Checked;
        }
    }
}
